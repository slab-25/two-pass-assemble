/**
 * @file second_pass.c
 * @brief Implementation of the second pass of the assembler
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/second_pass.h"
#include "../include/utils.h"

/* Forward declarations for internal functions */
static bool encode_data_image(machine_word_t **data_image, int *DCF, const char *filename);
static bool encode_address_word(machine_word_t *word, int address, int are);
static opcode_t get_opcode(const char *opcode_str);
static funct_t get_funct(const char *opcode_str);
static bool is_two_operand_instruction(opcode_t opcode);

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

/* Encode the first word of an instruction */
machine_word_t encode_first_word(const char *opcode_str, addressing_method_t src_addr,
                               int src_reg, addressing_method_t dst_addr, int dst_reg) {
    machine_word_t word = {0};
    opcode_t opcode = get_opcode(opcode_str);
    funct_t funct = get_funct(opcode_str);
    unsigned int value = 0;

    /* Set the opcode (bits 18-23) */
    value |= (opcode & 0x0F) << 18;

    /* Set source addressing method (bits 16-17) */
    if (is_two_operand_instruction(opcode)) {
        value |= (src_addr & 0x03) << 16;
    }

    /* Set source register (bits 13-15) */
    if (src_addr == ADDR_REGISTER) {
        value |= (src_reg & 0x07) << 13;
    }

    /* Set destination addressing method (bits 11-12) */
    value |= (dst_addr & 0x03) << 11;

    /* Set destination register (bits 8-10) */
    if (dst_addr == ADDR_REGISTER) {
        value |= (dst_reg & 0x07) << 8;
    }

    /* Set function code (bits 3-7) */
    value |= (funct & 0x0F) << 3;

    /* Set ARE bits (bits 0-2) - First word is always absolute (100) */
    value |= ARE_ABSOLUTE;

    word.value = value;
    word.ARE = ARE_ABSOLUTE;

    return word;
}

/* Encode a machine instruction */
bool encode_instruction(parsed_line_t *line, symbol_table_t *symbols,
                       instruction_code_t *code, int current_address,
                       external_reference_t **ext_refs) {
    const char *opcode = line->opcode;
    const char *operand1 = line->operand_count > 0 ? line->operands[0] : NULL;
    const char *operand2 = line->operand_count > 1 ? line->operands[1] : NULL;
    addressing_method_t src_addr = ADDR_IMMEDIATE;
    addressing_method_t dst_addr = ADDR_IMMEDIATE;
    int src_reg = 0, dst_reg = 0;
    int word_idx = 0;
    symbol_t *symbol;
    int address, target_dist;
    char symbol_name[MAX_LABEL_LENGTH];

    /* Initialize instruction code */
    memset(code, 0, sizeof(instruction_code_t));

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

    /* Encode the first word */
    code->words[word_idx++] = encode_first_word(opcode, src_addr, src_reg, dst_addr, dst_reg);

    /* Encode additional words for the first operand */
    if (operand1 && is_two_operand_instruction(get_opcode(opcode))) {
        switch (src_addr) {
            case ADDR_IMMEDIATE:
                /* Skip the '#' character and convert to integer */
                code->words[word_idx].value = string_to_int(operand1 + 1);
                code->words[word_idx].ARE = ARE_ABSOLUTE;
                word_idx++;
                break;

            case ADDR_DIRECT:
                /* Look up symbol in the symbol table */
                symbol = find_symbol(symbols, operand1);
                if (!symbol) {
                    report_error("second_pass", 0, "Undefined symbol: %s", operand1);
                    return false;
                }

                /* Encode the symbol's address */
                code->words[word_idx].value = symbol->value;

                /* Set ARE bits */
                if (symbol->type == SYMBOL_EXTERNAL) {
                    code->words[word_idx].ARE = ARE_EXTERNAL;
                    /* Add external reference */
                    if (!add_external_reference(ext_refs, operand1, current_address + word_idx)) {
                        return false;
                    }
                } else {
                    code->words[word_idx].ARE = ARE_RELOCATABLE;
                }

                word_idx++;
                break;

            case ADDR_RELATIVE:
                /* Skip the '&' character */
                strncpy(symbol_name, operand1 + 1, MAX_LABEL_LENGTH - 1);
                symbol_name[MAX_LABEL_LENGTH - 1] = '\0';

                /* Look up symbol in the symbol table */
                symbol = find_symbol(symbols, symbol_name);
                if (!symbol) {
                    report_error("second_pass", 0, "Undefined symbol: %s", symbol_name);
                    return false;
                }

                /* Calculate the distance (target address - current address) */
                address = symbol->value;
                target_dist = address - (current_address + word_idx);

                /* Encode the relative distance */
                code->words[word_idx].value = target_dist;
                code->words[word_idx].ARE = ARE_RELOCATABLE;
                word_idx++;
                break;

            case ADDR_REGISTER:
                /* Register addressing is handled in the first word */
                break;
        }
    }

    /* Encode additional words for the second operand */
    if (operand2) {
        switch (dst_addr) {
            case ADDR_IMMEDIATE:
                /* Skip the '#' character and convert to integer */
                code->words[word_idx].value = string_to_int(operand2 + 1);
                code->words[word_idx].ARE = ARE_ABSOLUTE;
                word_idx++;
                break;

            case ADDR_DIRECT:
                /* Look up symbol in the symbol table */
                symbol = find_symbol(symbols, operand2);
                if (!symbol) {
                    report_error("second_pass", 0, "Undefined symbol: %s", operand2);
                    return false;
                }

                /* Encode the symbol's address */
                code->words[word_idx].value = symbol->value;

                /* Set ARE bits */
                if (symbol->type == SYMBOL_EXTERNAL) {
                    code->words[word_idx].ARE = ARE_EXTERNAL;
                    /* Add external reference */
                    if (!add_external_reference(ext_refs, operand2, current_address + word_idx)) {
                        return false;
                    }
                } else {
                    code->words[word_idx].ARE = ARE_RELOCATABLE;
                }

                word_idx++;
                break;

            case ADDR_RELATIVE:
                /* Skip the '&' character */
                strncpy(symbol_name, operand2 + 1, MAX_LABEL_LENGTH - 1);
                symbol_name[MAX_LABEL_LENGTH - 1] = '\0';

                /* Look up symbol in the symbol table */
                symbol = find_symbol(symbols, symbol_name);
                if (!symbol) {
                    report_error("second_pass", 0, "Undefined symbol: %s", symbol_name);
                    return false;
                }

                /* Calculate the distance (target address - current address) */
                address = symbol->value;
                target_dist = address - (current_address + word_idx);

                /* Encode the relative distance */
                code->words[word_idx].value = target_dist;
                code->words[word_idx].ARE = ARE_RELOCATABLE;
                word_idx++;
                break;

            case ADDR_REGISTER:
                /* Register addressing is handled in the first word */
                break;
        }
    }
    /* Single operand instruction */
    else if (operand1 && !is_two_operand_instruction(get_opcode(opcode))) {
        switch (dst_addr) {
            case ADDR_IMMEDIATE:
                /* Skip the '#' character and convert to integer */
                code->words[word_idx].value = string_to_int(operand1 + 1);
                code->words[word_idx].ARE = ARE_ABSOLUTE;
                word_idx++;
                break;

            case ADDR_DIRECT:
                /* Look up symbol in the symbol table */
                symbol = find_symbol(symbols, operand1);
                if (!symbol) {
                    report_error("second_pass", 0, "Undefined symbol: %s", operand1);
                    return false;
                }

                /* Encode the symbol's address */
                code->words[word_idx].value = symbol->value;

                /* Set ARE bits */
                if (symbol->type == SYMBOL_EXTERNAL) {
                    code->words[word_idx].ARE = ARE_EXTERNAL;
                    /* Add external reference */
                    if (!add_external_reference(ext_refs, operand1, current_address + word_idx)) {
                        return false;
                    }
                } else {
                    code->words[word_idx].ARE = ARE_RELOCATABLE;
                }

                word_idx++;
                break;

            case ADDR_RELATIVE:
                /* Skip the '&' character */
                strncpy(symbol_name, operand1 + 1, MAX_LABEL_LENGTH - 1);
                symbol_name[MAX_LABEL_LENGTH - 1] = '\0';

                /* Look up symbol in the symbol table */
                symbol = find_symbol(symbols, symbol_name);
                if (!symbol) {
                    report_error("second_pass", 0, "Undefined symbol: %s", symbol_name);
                    return false;
                }

                /* Calculate the distance (target address - current address) */
                address = symbol->value;
                target_dist = address - (current_address + word_idx);

                /* Encode the relative distance */
                code->words[word_idx].value = target_dist;
                code->words[word_idx].ARE = ARE_RELOCATABLE;
                word_idx++;
                break;

            case ADDR_REGISTER:
                /* Register addressing is handled in the first word */
                break;
        }
    }

    /* Set the word count */
    code->word_count = word_idx;

    return true;
}

/* Process an entry directive */
bool process_entry_second_pass(parsed_line_t *line, symbol_table_t *symbols) {
    const char *symbol_name = line->operands[0];
    symbol_t *symbol;

    /* Look up the symbol in the symbol table */
    symbol = find_symbol(symbols, symbol_name);
    if (!symbol) {
        report_error("second_pass", line->line_number, "Entry symbol '%s' not defined", symbol_name);
        return false;
    }

    /* Check if the symbol is already defined as external */
    if (symbol->type == SYMBOL_EXTERNAL) {
        report_error("second_pass", line->line_number,
            "Symbol '%s' cannot be both external and entry", symbol_name);
        return false;
    }

    /* Mark the symbol as an entry */
    if (!add_symbol_attribute(symbols, symbol_name, SYMBOL_ENTRY)) {
        report_error("second_pass", line->line_number,
            "Failed to mark symbol '%s' as entry", symbol_name);
        return false;
    }

    return true;
}

/* Add an external reference */
bool add_external_reference(external_reference_t **ext_refs, const char *name, int address) {
    external_reference_t *new_ref, *current;

    /* Allocate memory for the new reference */
    new_ref = (external_reference_t *)malloc(sizeof(external_reference_t));
    if (!new_ref) {
        report_error("second_pass", 0, "Memory allocation error for external reference");
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
                external_reference_t **ext_refs, int *ICF, int *DCF) {
    FILE *file;
    char base_filename[MAX_FILENAME_LENGTH];
    char am_filename[MAX_FILENAME_LENGTH];
    char line_buf[MAX_LINE_LENGTH];
    parsed_line_t parsed_line;
    int IC = 0, DC = 0;
    int line_number = 0;
    bool success = true;
    instruction_code_t code;

    /* Build the .am filename */
    get_base_filename(filename, base_filename);
    create_filename(base_filename, EXT_MACRO, am_filename);

    /* Open the file */
    file = fopen(am_filename, "r");
    if (!file) {
        report_error("second_pass", 0, "Could not open file: %s", am_filename);
        return false;
    }

    /* Allocate memory for code image */
    *code_image = (machine_word_t *)calloc(MEMORY_START + 1000, sizeof(machine_word_t));
    if (!*code_image) {
        report_error("second_pass", 0, "Memory allocation error for code image");
        fclose(file);
        return false;
    }

    /* Initialize external references list */
    *ext_refs = NULL;

    /* Second pass through the file */
    while (fgets(line_buf, MAX_LINE_LENGTH, file)) {
        line_number++;

        /* Parse the line */
        if (!parse_line(line_buf, &parsed_line, line_number)) {
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
                      parse_numbers_list(parsed_line.operands[0], NULL, 0) :
                      strlen(parsed_line.operands[0]) - 2 + 1; /* -2 for quotes, +1 for null */
                break;

            case INST_TYPE_ENTRY:
                if (!process_entry_second_pass(&parsed_line, symbols)) {
                    success = false;
                }
                break;

            case INST_TYPE_EXTERN:
                /* External directives were already processed in first pass */
                break;

            case INST_TYPE_CODE:
                /* Encode the instruction */
                if (!encode_instruction(&parsed_line, symbols, &code, MEMORY_START + IC, ext_refs)) {
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
                report_error("second_pass", line_number, "Unknown instruction type");
                success = false;
                break;
        }
    }

    /* Encode the data image */
    if (success) {
        /* Allocate memory for data image */
        *data_image = (machine_word_t *)calloc(DC + 1, sizeof(machine_word_t));
        if (!*data_image) {
            report_error("second_pass", 0, "Memory allocation error for data image");
            fclose(file);
            free(*code_image);
            *code_image = NULL;
            free_external_references(*ext_refs);
            *ext_refs = NULL;
            return false;
        }

        /* Encode the data */
        if (!encode_data_image(data_image, &DC, am_filename)) {
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
static bool encode_data_image(machine_word_t **data_image, int *DCF, const char *filename) {
    FILE *file;
    char line_buf[MAX_LINE_LENGTH];
    parsed_line_t parsed_line;
    int line_number = 0;
    int DC = 0;
    int i, count;
    int numbers[MAX_LINE_LENGTH];
    char *str;
    int len;

    /* Open the file */
    file = fopen(filename, "r");
    if (!file) {
        report_error("second_pass", 0, "Could not open file: %s", filename);
        return false;
    }

    /* Process the file */
    while (fgets(line_buf, MAX_LINE_LENGTH, file)) {
        line_number++;

        /* Parse the line */
        if (!parse_line(line_buf, &parsed_line, line_number)) {
            continue;
        }

        /* Skip empty lines, comments, and invalid lines */
        if (parsed_line.type == INST_TYPE_INVALID) {
            continue;
        }

        /* Process data and string directives */
        if (parsed_line.type == INST_TYPE_DATA) {
            /* Parse the data values */
            count = parse_numbers_list(parsed_line.operands[0], numbers, MAX_LINE_LENGTH);
            if (count > 0) {
                /* Encode the data values */
                for (i = 0; i < count; i++) {
                    (*data_image)[DC].value = numbers[i];
                    (*data_image)[DC].ARE = ARE_ABSOLUTE;
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
                    (*data_image)[DC].ARE = ARE_ABSOLUTE;
                    DC++;
                }

                /* Add null terminator */
                (*data_image)[DC].value = 0;
                (*data_image)[DC].ARE = ARE_ABSOLUTE;
                DC++;
            }
        }
    }

    fclose(file);

    /* Set final data counter */
    *DCF = DC;

    return true;
}

/* Helper function to encode an address word */
static bool encode_address_word(machine_word_t *word, int address, int are) {
    if (!word) {
        return false;
    }

    word->value = address;
    word->ARE = are;

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

/* Helper function to parse a list of comma-separated numbers */
static int parse_numbers_list(const char *str, int numbers[], int max_count) {
    char str_copy[MAX_LINE_LENGTH];
    char *token;
    char *context = NULL;
    int count = 0;

    if (!str) {
        return 0;
    }

    /* Copy the string for tokenization */
    strncpy(str_copy, str, MAX_LINE_LENGTH - 1);
    str_copy[MAX_LINE_LENGTH - 1] = '\0';

    /* Get the first token */
    token = strtok_r(str_copy, ",", &context);

    /* Parse each token */
    while (token && (max_count == 0 || count < max_count)) {
        /* Trim whitespace */
        token = trim(token);

        /* Check if the token is a valid integer */
        if (!is_integer(token)) {
            return -1;
        }

        /* Convert to integer and store */
        if (numbers) {
            numbers[count] = string_to_int(token);
        }
        count++;

        /* Get the next token */
        token = strtok_r(NULL, ",", &context);
    }

    return count;
}