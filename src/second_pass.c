/**
 * @file second_pass.c
 * @brief Implementation of the second pass of the assembler
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/second_pass.h"
#include "../include/utils.h"
#include "../include/machine_word.h"

/* Forward declarations for internal functions */
static bool encode_data_image(machine_word_t **data_image, int *DCF, const char *filename, error_context_t *context);
static opcode_t get_opcode(const char *opcode_str);
static funct_t get_funct(const char *opcode_str);
static bool is_two_operand_instruction(opcode_t opcode);
static char *safe_strtok_r(char *str, const char *delim, char **saveptr);
static int parse_numbers_list(const char *str, int numbers[], int max_count, error_context_t *context);

/* Determine the addressing method for an operand */
addressing_method_t get_addressing_method(const char *operand) {
    if (!operand || !*operand) {
        return ADDR_IMMEDIATE; /* Default */
    }

    /* Check for immediate addressing (#value) */
    if (operand[0] == '#') {
        return ADDR_IMMEDIATE;
    }

    /* Check for relative addressing (&label) */
    if (operand[0] == '&') {
        return ADDR_RELATIVE;
    }

    /* Check for register addressing (r0-r7) */
    if (is_register(operand)) {
        return ADDR_REGISTER;
    }

    /* Otherwise, it's direct addressing (label) */
    return ADDR_DIRECT;
}

/* Encode an operand word based on addressing method */
bool encode_operand_word(machine_word_t *word, const char *operand,
                        addressing_method_t addr_method,
                        symbol_table_t *symbols, int current_address,
                        int word_offset, external_reference_t **ext_refs,
                        error_context_t *context) {
    symbol_t *symbol;
    int value, address, target_dist;
    char symbol_name[MAX_LABEL_LENGTH];

    /* Validate parameters */
    if (!word || !operand) {
        report_context_error(context, "Invalid parameters for encode_operand_word");
        return false;
    }

    switch (addr_method) {
        case ADDR_IMMEDIATE:
            /* Skip the '#' character and convert to integer */
            if (!is_integer(operand + 1)) {
                report_context_error(context, "Invalid immediate value: %s", operand);
                return false;
            }
            value = string_to_int(operand + 1);
            *word = encode_immediate(value);
            break;

        case ADDR_DIRECT:
            /* Look up symbol in the symbol table */
            symbol = find_symbol(symbols, operand);
            if (!symbol) {
                report_context_error(context, "Undefined symbol: %s", operand);
                return false;
            }

            /* Encode the symbol's address */
            *word = encode_direct_address(symbol->value,
                                         symbol_has_attribute(symbol, SYMBOL_ATTR_EXTERNAL));

            /* If external, add to external references */
            if (symbol_has_attribute(symbol, SYMBOL_ATTR_EXTERNAL)) {
                if (!add_external_reference(ext_refs, operand, current_address + word_offset, context)) {
                    return false;
                }
            }
            break;

        case ADDR_RELATIVE:
            /* Skip the '&' character */
            strncpy(symbol_name, operand + 1, MAX_LABEL_LENGTH - 1);
            symbol_name[MAX_LABEL_LENGTH - 1] = '\0';

            /* Look up symbol in the symbol table */
            symbol = find_symbol(symbols, symbol_name);
            if (!symbol) {
                report_context_error(context, "Undefined symbol: %s", symbol_name);
                return false;
            }

            /* Calculate the distance (target address - current address) */
            address = symbol->value;
            target_dist = address - (current_address + word_offset);

            /* Encode the relative distance */
            *word = encode_relative_address(target_dist);
            break;

        case ADDR_REGISTER:
            /* Register addressing is handled in first word or shared register word */
            *word = encode_register_word(-1, get_register_number(operand));
            break;

        default:
            report_context_error(context, "Invalid addressing method");
            return false;
    }

    return true;
}

/* Encode a machine instruction */
bool encode_instruction(parsed_line_t *line, symbol_table_t *symbols,
                       instruction_code_t *code, int current_address,
                       external_reference_t **ext_refs, error_context_t *context) {
    const char *opcode = line->opcode;
    const char *operand1 = line->operand_count > 0 ? line->operands[0] : NULL;
    const char *operand2 = line->operand_count > 1 ? line->operands[1] : NULL;
    addressing_method_t src_addr = ADDR_IMMEDIATE;
    addressing_method_t dst_addr = ADDR_IMMEDIATE;
    int src_reg = 0, dst_reg = 0;
    int word_idx = 0;
    opcode_t op_code;
    funct_t funct_code;

    /* Set current line number in error context */
    if (context) {
        context->line_number = line->line_number;
    }

    /* Initialize instruction code */
    memset(code, 0, sizeof(instruction_code_t));

    /* Get opcode and funct values */
    op_code = get_opcode(opcode);
    funct_code = get_funct(opcode);

    /* Determine operand addressing methods and register numbers */
    if (operand1) {
        src_addr = get_addressing_method(operand1);
        if (src_addr == ADDR_REGISTER) {
            src_reg = get_register_number(operand1);
        }
    }

    if (operand2) {
        dst_addr = get_addressing_method(operand2);
        if (dst_addr == ADDR_REGISTER) {
            dst_reg = get_register_number(operand2);
        }
    }

    /* No operands (rts, stop) */
    if (!operand1 && !operand2) {
        dst_addr = ADDR_IMMEDIATE;
    }
    /* One operand instructions */
    else if (operand1 && !operand2) {
        dst_addr = src_addr;
        dst_reg = src_reg;
        src_addr = ADDR_IMMEDIATE;
        src_reg = 0;
    }

    /* Encode the first word - instruction word */
    code->words[word_idx++] = encode_instruction_word(op_code, src_addr, src_reg, dst_addr, dst_reg, funct_code);

    /* Special case: if both operands are registers, encode them in a shared register word */
    if (operand1 && operand2 && src_addr == ADDR_REGISTER && dst_addr == ADDR_REGISTER) {
        code->words[word_idx++] = encode_register_word(src_reg, dst_reg);
    }
    else {
        /* Encode additional words for the first operand */
        if (operand1 && is_two_operand_instruction(op_code)) {
            if (src_addr != ADDR_REGISTER) {
                if (!encode_operand_word(&code->words[word_idx], operand1, src_addr, symbols,
                                      current_address, word_idx, ext_refs, context)) {
                    return false;
                }
                word_idx++;
            }
        }

        /* Encode additional words for the second operand */
        if (operand2) {
            if (dst_addr != ADDR_REGISTER) {
                if (!encode_operand_word(&code->words[word_idx], operand2, dst_addr, symbols,
                                      current_address, word_idx, ext_refs, context)) {
                    return false;
                }
                word_idx++;
            }
        }
        /* Single operand instruction */
        else if (operand1 && !is_two_operand_instruction(op_code)) {
            if (dst_addr != ADDR_REGISTER) {
                if (!encode_operand_word(&code->words[word_idx], operand1, dst_addr, symbols,
                                      current_address, word_idx, ext_refs, context)) {
                    return false;
                }
                word_idx++;
            }
        }
    }

    /* Set the word count */
    code->word_count = word_idx;

    return true;
}

/* Process an entry directive in second pass */
bool process_entry_second_pass(parsed_line_t *line, symbol_table_t *symbols, error_context_t *context) {
    const char *symbol_name = line->operands[0];
    symbol_t *symbol;

    /* Set current line number in error context */
    if (context) {
        context->line_number = line->line_number;
    }

    /* Look up the symbol in the symbol table */
    symbol = find_symbol(symbols, symbol_name);
    if (!symbol) {
        report_context_error(context, "Entry symbol '%s' not defined", symbol_name);
        return false;
    }

    /* Check if the symbol is already defined as external */
    if (symbol_has_attribute(symbol, SYMBOL_ATTR_EXTERNAL)) {
        report_context_error(context, "Symbol '%s' cannot be both external and entry", symbol_name);
        return false;
    }

    /* Mark the symbol as an entry */
    if (!add_symbol_attributes(symbols, symbol_name, SYMBOL_ATTR_ENTRY)) {
        report_context_error(context, "Failed to mark symbol '%s' as entry", symbol_name);
        return false;
    }

    return true;
}

/* Add an external reference */
bool add_external_reference(external_reference_t **ext_refs, const char *name, int address, error_context_t *context) {
    external_reference_t *new_ref, *current;

    /* Validate parameters */
    if (!ext_refs || !name) {
        report_context_error(context, "Invalid parameters for add_external_reference");
        return false;
    }

    /* Allocate memory for the new reference */
    new_ref = (external_reference_t *)malloc(sizeof(external_reference_t));
    if (!new_ref) {
        report_context_error(context, "Memory allocation error for external reference");
        return false;
    }

    /* Initialize the reference */
    strncpy(new_ref->name, name, MAX_LABEL_LENGTH - 1);
    new_ref->name[MAX_LABEL_LENGTH - 1] = '\0';
    new_ref->address = address;
    new_ref->next = NULL;

    /* Add to the linked list */
    if (!*ext_refs) {
        *ext_refs = new_ref;
    } else {
        current = *ext_refs;
        while (current->next) {
            current = current->next;
        }
        current->next = new_ref;
    }

    return true;
}

/* Free the list of external references */
void free_external_references(external_reference_t *ext_refs) {
    external_reference_t *current, *next;

    current = ext_refs;
    while (current) {
        next = current->next;
        free(current);
        current = next;
    }
}

/* Main function for the second pass */
bool second_pass(const char *filename, symbol_table_t *symbols,
                machine_word_t **code_image, machine_word_t **data_image,
                external_reference_t **ext_refs, int *ICF, int *DCF,
                error_context_t *context) {
    FILE *file;
    char base_filename[MAX_FILENAME_LENGTH];
    char am_filename[MAX_FILENAME_LENGTH];
    char line_buf[MAX_LINE_LENGTH];
    parsed_line_t parsed_line;
    int IC = 0, DC = 0;
    int line_number = 0;
    bool success = true;
    instruction_code_t code;

    /* Initialize/update error context */
    if (context) {
        strncpy(context->filename, filename, MAX_FILENAME_LENGTH - 1);
        context->filename[MAX_FILENAME_LENGTH - 1] = '\0';
        context->line_number = 0;
    }

    /* Build the .am filename */
    get_base_filename(filename, base_filename);
    create_filename(base_filename, EXT_MACRO, am_filename);

    /* Open the file */
    file = fopen(am_filename, "r");
    if (!file) {
        report_context_error(context, "Could not open file: %s", am_filename);
        return false;
    }

    /* Allocate memory for code image */
    *code_image = (machine_word_t *)calloc(MEMORY_START + 1000, sizeof(machine_word_t));
    if (!*code_image) {
        report_context_error(context, "Memory allocation error for code image");
        fclose(file);
        return false;
    }

    /* Initialize external references list */
    *ext_refs = NULL;

    /* Second pass through the file */
    while (fgets(line_buf, MAX_LINE_LENGTH, file)) {
        line_number++;
        if (context) {
            context->line_number = line_number;
        }

        /* Parse the line */
        if (!parse_line(line_buf, &parsed_line, line_number, context)) {
            success = false;
            continue;
        }

        /* Skip empty lines, comments, and invalid lines */
        if (parsed_line.type == INST_TYPE_INVALID) {
            continue;
        }

        /* Process the line based on its type */
        switch (parsed_line.type) {
            case INST_TYPE_DATA:
            case INST_TYPE_STRING:
                /* Data and string directives don't need processing in second pass */
                DC += parsed_line.type == INST_TYPE_DATA ?
                      parse_numbers_list(parsed_line.operands[0], NULL, 0, context) :
                      strlen(parsed_line.operands[0]) - 2 + 1; /* -2 for quotes, +1 for null */
                break;

            case INST_TYPE_ENTRY:
                if (!process_entry_second_pass(&parsed_line, symbols, context)) {
                    success = false;
                }
                break;

            case INST_TYPE_EXTERN:
                /* External directives were already processed in first pass */
                break;

            case INST_TYPE_CODE:
                /* Encode the instruction */
                if (!encode_instruction(&parsed_line, symbols, &code, MEMORY_START + IC, ext_refs, context)) {
                    success = false;
                    continue;
                }

                /* Copy the encoded instruction to the code image */
                memcpy(*code_image + MEMORY_START + IC, code.words,
                       code.word_count * sizeof(machine_word_t));

                /* Update instruction counter */
                IC += code.word_count;
                break;

            default:
                report_context_error(context, "Unknown instruction type");
                success = false;
                break;
        }
    }

    /* Encode the data image */
    if (success) {
        /* Allocate memory for data image */
        *data_image = (machine_word_t *)calloc(DC + 1, sizeof(machine_word_t));
        if (!*data_image) {
            report_context_error(context, "Memory allocation error for data image");
            fclose(file);
            free(*code_image);
            *code_image = NULL;
            free_external_references(*ext_refs);
            *ext_refs = NULL;
            return false;
        }

        /* Encode the data */
        if (!encode_data_image(data_image, &DC, am_filename, context)) {
            success = false;
        }
    }

    fclose(file);

    /* Set final counters */
    *ICF = IC;
    *DCF = DC;

    return success;
}

/* Helper function to encode data image */
static bool encode_data_image(machine_word_t **data_image, int *DCF, const char *filename, error_context_t *context) {
    FILE *file;
    char line_buf[MAX_LINE_LENGTH];
    parsed_line_t parsed_line;
    int line_number = 0;
    int DC = 0;
    int i, count;
    int numbers[MAX_LINE_LENGTH];
    const char *str;
    int len;

    /* Open the file */
    file = fopen(filename, "r");
    if (!file) {
        report_context_error(context, "Could not open file: %s", filename);
        return false;
    }

    /* Process the file */
    while (fgets(line_buf, MAX_LINE_LENGTH, file)) {
        line_number++;
        if (context) {
            context->line_number = line_number;
        }

        /* Parse the line */
        if (!parse_line(line_buf, &parsed_line, line_number, context)) {
            continue;
        }

        /* Skip empty lines, comments, and invalid lines */
        if (parsed_line.type == INST_TYPE_INVALID) {
            continue;
        }

        /* Process data and string directives */
        if (parsed_line.type == INST_TYPE_DATA) {
            /* Parse the data values */
            count = parse_numbers_list(parsed_line.operands[0], numbers, MAX_LINE_LENGTH, context);
            if (count > 0) {
                /* Encode the data values */
                for (i = 0; i < count; i++) {
                    (*data_image)[DC].value = numbers[i];
                    (*data_image)[DC].are = ARE_ABSOLUTE;
                    DC++;
                }
            }
        }
        else if (parsed_line.type == INST_TYPE_STRING) {
            /* Get the string operand */
            str = parsed_line.operands[0];

            /* Validate the string - it should be enclosed in quotes */
            len = strlen(str);
            if (len >= 2 && str[0] == '"' && str[len-1] == '"') {
                /* Remove the quotes */
                str++;
                len -= 2;

                /* Encode each character */
                for (i = 0; i < len; i++) {
                    (*data_image)[DC].value = str[i];
                    (*data_image)[DC].are = ARE_ABSOLUTE;
                    DC++;
                }

                /* Add null terminator */
                (*data_image)[DC].value = 0;
                (*data_image)[DC].are = ARE_ABSOLUTE;
                DC++;
            }
        }
    }

    fclose(file);

    /* Set final data counter */
    *DCF = DC;

    return true;
}

/* Helper function to get the opcode value */
static opcode_t get_opcode(const char *opcode_str) {
    if (!opcode_str) {
        return 0;
    }

    if (strcmp(opcode_str, "mov") == 0) {
        return OP_MOV;
    }
    else if (strcmp(opcode_str, "cmp") == 0) {
        return OP_CMP;
    }
    else if (strcmp(opcode_str, "add") == 0 || strcmp(opcode_str, "sub") == 0) {
        return OP_ADD; /* ADD and SUB share the same opcode */
    }
    else if (strcmp(opcode_str, "lea") == 0) {
        return OP_LEA;
    }
    else if (strcmp(opcode_str, "clr") == 0 || strcmp(opcode_str, "not") == 0 ||
             strcmp(opcode_str, "inc") == 0 || strcmp(opcode_str, "dec") == 0) {
        return OP_CLR; /* CLR, NOT, INC, DEC share the same opcode */
    }
    else if (strcmp(opcode_str, "jmp") == 0 || strcmp(opcode_str, "bne") == 0 ||
             strcmp(opcode_str, "jsr") == 0) {
        return OP_JMP; /* JMP, BNE, JSR share the same opcode */
    }
    else if (strcmp(opcode_str, "red") == 0) {
        return OP_RED;
    }
    else if (strcmp(opcode_str, "prn") == 0) {
        return OP_PRN;
    }
    else if (strcmp(opcode_str, "rts") == 0) {
        return OP_RTS;
    }
    else if (strcmp(opcode_str, "stop") == 0) {
        return OP_STOP;
    }

    return 0;
}

/* Helper function to get the function value */
static funct_t get_funct(const char *opcode_str) {
    if (!opcode_str) {
        return FUNCT_NONE;
    }

    if (strcmp(opcode_str, "add") == 0) {
        return FUNCT_ADD;
    }
    else if (strcmp(opcode_str, "sub") == 0) {
        return FUNCT_SUB;
    }
    else if (strcmp(opcode_str, "clr") == 0) {
        return FUNCT_CLR;
    }
    else if (strcmp(opcode_str, "not") == 0) {
        return FUNCT_NOT;
    }
    else if (strcmp(opcode_str, "inc") == 0) {
        return FUNCT_INC;
    }
    else if (strcmp(opcode_str, "dec") == 0) {
        return FUNCT_DEC;
    }
    else if (strcmp(opcode_str, "jmp") == 0) {
        return FUNCT_JMP;
    }
    else if (strcmp(opcode_str, "bne") == 0) {
        return FUNCT_BNE;
    }
    else if (strcmp(opcode_str, "jsr") == 0) {
        return FUNCT_JSR;
    }

    return FUNCT_NONE;
}

/* Helper function to check if an instruction takes two operands */
static bool is_two_operand_instruction(opcode_t opcode) {
    return opcode == OP_MOV || opcode == OP_CMP ||
           opcode == OP_ADD || opcode == OP_LEA;
}

/* Safe implementation of strtok_r for C90 compatibility */
static char *safe_strtok_r(char *str, const char *delim, char **saveptr) {
    char *token;

    if (str == NULL) {
        str = *saveptr;
    }

    /* Skip leading delimiters */
    str += strspn(str, delim);
    if (*str == '\0') {
        *saveptr = str;
        return NULL;
    }

    /* Find the end of the token */
    token = str;
    str = strpbrk(str, delim);
    if (str == NULL) {
        /* This token finishes the string */
        *saveptr = strchr(token, '\0');
    } else {
        /* Terminate the token and make saveptr point to the rest of the string */
        *str = '\0';
        *saveptr = str + 1;
    }

    return token;
}

/* Helper function to parse a list of comma-separated numbers */
static int parse_numbers_list(const char *str, int numbers[], int max_count, error_context_t *context) {
    char str_copy[MAX_LINE_LENGTH];
    char *token;
    char *saveptr = NULL;
    int count = 0;

    /* Check for NULL pointer */
    if (!str) {
        report_context_error(context, "No numbers provided");
        return 0;
    }

    /* Copy the string for tokenization */
    strncpy(str_copy, str, MAX_LINE_LENGTH - 1);
    str_copy[MAX_LINE_LENGTH - 1] = '\0';

    /* Get the first token */
    token = safe_strtok_r(str_copy, ",", &saveptr);

    /* Parse each token */
    while (token && (max_count == 0 || count < max_count)) {
        /* Trim whitespace */
        token = trim(token);

        /* Check if the token is a valid integer */
        if (!is_integer(token)) {
            report_context_error(context, "Invalid number format: %s", token);
            return -1;
        }

        /* Convert to integer and store */
        if (numbers) {
            numbers[count] = string_to_int(token);
        }
        count++;

        /* Get next token */
        token = safe_strtok_r(NULL, ",", &saveptr);
    }

    /* Check if we have more tokens (too many numbers) */
    if (token && max_count > 0 && count >= max_count) {
        report_context_error(context, "Too many numbers in list");
        return -1;
    }

    return count;
}