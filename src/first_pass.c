/**
 * @file first_pass.c
 * @brief Implementation of the first pass of the assembler
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/first_pass.h"
#include "../include/utils.h"

/* Forward declarations for internal functions */
static bool process_label(const char *label, symbol_table_t *symbols, int address,
                         symbol_attr_t attributes, error_context_t *context);
static instruction_type_t get_directive_type(const char *directive);
static int parse_numbers_list(const char *str, int numbers[], int max_count, error_context_t *context);
static char *safe_strtok_r(char *str, const char *delim, char **saveptr);

/* Parse a line into its components */
bool parse_line(const char *line, parsed_line_t *parsed, int line_number, error_context_t *context) {
    char line_copy[MAX_LINE_LENGTH];
    char *token;
    char *next_token;
    char *saveptr = NULL;
    int i;

    /* Set current line number in error context */
    if (context) {
        context->line_number = line_number;
    }

    /* Validate parameters */
    if (!line || !parsed) {
        report_context_error(context, "Invalid parameters for parse_line");
        return false;
    }

    /* Initialize the parsed structure */
    memset(parsed, 0, sizeof(parsed_line_t));
    parsed->line_number = line_number;
    parsed->type = INST_TYPE_INVALID;

    /* Check for empty line or comment */
    if (!*line || line[0] == ';') {
        return true;
    }

    /* Copy the line for tokenization */
    strncpy(line_copy, line, MAX_LINE_LENGTH - 1);
    line_copy[MAX_LINE_LENGTH - 1] = '\0';

    /* Remove comments */
    token = strchr(line_copy, ';');
    if (token) {
        *token = '\0';
    }

    /* Trim whitespace */
    token = trim(line_copy);

    /* If the line is empty after trimming, return */
    if (!token || !*token) {
        return true;
    }

    /* Move trimmed line to beginning of buffer if necessary */
    if (token != line_copy) {
        memmove(line_copy, token, strlen(token) + 1);
    }

    /* Get the first token */
    token = safe_strtok_r(line_copy, " \t", &saveptr);
    if (!token) {
        return true; /* Empty line */
    }

    /* Check if the first token is a label */
    if (token[strlen(token) - 1] == ':') {
        if (strlen(token) <= 1) {
            report_context_error(context, "Invalid label name (empty)");
            return false;
        }

        token[strlen(token) - 1] = '\0';

        /* Validate the label */
        if (!is_valid_label(token)) {
            report_context_error(context, "Invalid label name: %s", token);
            return false;
        }

        /* Store the label */
        strncpy(parsed->label, token, MAX_LABEL_LENGTH - 1);
        parsed->label[MAX_LABEL_LENGTH - 1] = '\0';

        /* Get the next token */
        token = safe_strtok_r(NULL, " \t", &saveptr);
        if (!token) {
            report_context_error(context, "Label defined without instruction or directive");
            return false;
        }
    }

    /* Check if the token is a directive */
    if (token[0] == '.') {
        /* Get directive type */
        parsed->type = get_directive_type(token);

        if (parsed->type == INST_TYPE_INVALID) {
            report_context_error(context, "Unknown directive: %s", token);
            return false;
        }

        /* For data and string directives, we need the operand(s) */
        if (parsed->type == INST_TYPE_DATA) {
            /* Get the rest of the line for data values */
            token = safe_strtok_r(NULL, "", &saveptr);
            if (!token) {
                report_context_error(context, "No data values specified for .data directive");
                return false;
            }

            /* Store the operand */
            token = trim(token);
            if (token) {
                strncpy(parsed->operands[0], token, MAX_OPERAND_LENGTH - 1);
                parsed->operands[0][MAX_OPERAND_LENGTH - 1] = '\0';
                parsed->operand_count = 1;
            }
        }
        else if (parsed->type == INST_TYPE_STRING) {
            /* Get the string operand */
            token = safe_strtok_r(NULL, "", &saveptr);
            if (!token) {
                report_context_error(context, "No string specified for .string directive");
                return false;
            }

            /* Store the operand */
            token = trim(token);
            if (token) {
                strncpy(parsed->operands[0], token, MAX_OPERAND_LENGTH - 1);
                parsed->operands[0][MAX_OPERAND_LENGTH - 1] = '\0';
                parsed->operand_count = 1;
            }
        }
        else if (parsed->type == INST_TYPE_ENTRY || parsed->type == INST_TYPE_EXTERN) {
            /* Get the symbol name */
            token = safe_strtok_r(NULL, " \t", &saveptr);
            if (!token) {
                report_context_error(context, "No symbol specified for %s directive",
                    parsed->type == INST_TYPE_ENTRY ? ".entry" : ".extern");
                return false;
            }

            /* Check if the symbol is valid */
            if (!is_valid_label(token)) {
                report_context_error(context, "Invalid symbol name: %s", token);
                return false;
            }

            /* Store the operand */
            strncpy(parsed->operands[0], token, MAX_OPERAND_LENGTH - 1);
            parsed->operands[0][MAX_OPERAND_LENGTH - 1] = '\0';
            parsed->operand_count = 1;

            /* Check for extra tokens */
            next_token = safe_strtok_r(NULL, " \t", &saveptr);
            if (next_token) {
                report_context_error(context, "Extra tokens after symbol in %s directive",
                    parsed->type == INST_TYPE_ENTRY ? ".entry" : ".extern");
                return false;
            }
        }
    }
    else {
        /* This is a machine instruction */
        parsed->type = INST_TYPE_CODE;

        /* Store the opcode */
        strncpy(parsed->opcode, token, MAX_OPCODE_LENGTH - 1);
        parsed->opcode[MAX_OPCODE_LENGTH - 1] = '\0';

        /* Get operands */
        token = safe_strtok_r(NULL, ",", &saveptr);
        i = 0;

        while (token && i < MAX_OPERANDS) {
            /* Trim whitespace */
            token = trim(token);

            /* Store non-empty operand */
            if (token && *token) {
                strncpy(parsed->operands[i], token, MAX_OPERAND_LENGTH - 1);
                parsed->operands[i][MAX_OPERAND_LENGTH - 1] = '\0';
                i++;
            }

            /* Get next operand */
            token = safe_strtok_r(NULL, ",", &saveptr);
        }

        parsed->operand_count = i;

        /* Check for extra operands */
        if (token) {
            report_context_error(context, "Too many operands for instruction");
            return false;
        }
    }

    return true;
}

/* Process a .data directive */
bool process_data_directive(parsed_line_t *line, symbol_table_t *symbols, int *DC, error_context_t *context) {
    int numbers[MAX_LINE_LENGTH]; /* Temporary buffer for parsed numbers */
    int count;

    /* Set current line number in error context */
    if (context) {
        context->line_number = line->line_number;
    }

    /* Process the label if present */
    if (line->label[0] != '\0') {
        if (!process_label(line->label, symbols, *DC + MEMORY_START, SYMBOL_ATTR_DATA, context)) {
            return false;
        }
    }

    /* Parse the data values */
    count = parse_numbers_list(line->operands[0], numbers, MAX_LINE_LENGTH, context);
    if (count <= 0) {
        report_context_error(context, "Invalid or missing data values");
        return false;
    }

    /* Update data counter */
    *DC += count;

    return true;
}

/* Process a .string directive */
bool process_string_directive(parsed_line_t *line, symbol_table_t *symbols, int *DC, error_context_t *context) {
    const char *str = line->operands[0];
    int len;

    /* Set current line number in error context */
    if (context) {
        context->line_number = line->line_number;
    }

    /* Process the label if present */
    if (line->label[0] != '\0') {
        if (!process_label(line->label, symbols, *DC + MEMORY_START, SYMBOL_ATTR_DATA, context)) {
            return false;
        }
    }

    /* Validate the string - it should be enclosed in quotes */
    len = strlen(str);
    if (len < 2 || str[0] != '"' || str[len-1] != '"') {
        report_context_error(context, "String must be enclosed in quotes");
        return false;
    }

    /* Remove the quotes for length calculation */
    str++;
    len = strlen(str) - 1;  /* -1 for the closing quote */

    /* Update data counter (add 1 for the null terminator) */
    *DC += len + 1;

    return true;
}

/* Process a .extern directive */
bool process_extern_directive(parsed_line_t *line, symbol_table_t *symbols, error_context_t *context) {
    const char *symbol_name = line->operands[0];
    symbol_t *existing;

    /* Set current line number in error context */
    if (context) {
        context->line_number = line->line_number;
    }

    /* Check if the label is defined in the same line */
    if (line->label[0] != '\0') {
        report_context_error(context, "Cannot define a label for .extern directive");
        return false;
    }

    /* Add the external symbol to the symbol table */
    if (!add_symbol(symbols, symbol_name, 0, SYMBOL_ATTR_EXTERNAL)) {
        existing = find_symbol(symbols, symbol_name);

        /* Check if the symbol already exists but is not external */
        if (existing && !symbol_has_attribute(existing, SYMBOL_ATTR_EXTERNAL)) {
            report_context_error(context, "Symbol '%s' already defined as non-external", symbol_name);
            return false;
        }

        /* Symbol already exists as external, which is fine */
    }

    return true;
}

/* Process a .entry directive */
bool process_entry_directive(parsed_line_t *line, error_context_t *context) {
    /* Set current line number in error context */
    if (context) {
        context->line_number = line->line_number;
    }

    /* Check if the label is defined in the same line */
    if (line->label[0] != '\0') {
        report_context_error(context, "Cannot define a label for .entry directive");
        return false;
    }

    /* Entry directives are fully processed in the second pass */
    return true;
}

/* Process a machine instruction */
bool process_instruction(parsed_line_t *line, symbol_table_t *symbols, int *IC, error_context_t *context) {
    int instruction_length;

    /* Set current line number in error context */
    if (context) {
        context->line_number = line->line_number;
    }

    /* Process the label if present */
    if (line->label[0] != '\0') {
        if (!process_label(line->label, symbols, *IC + MEMORY_START, SYMBOL_ATTR_CODE, context)) {
            return false;
        }
    }

    /* Calculate the instruction length */
    instruction_length = calculate_instruction_length(
        line->opcode,
        line->operand_count > 0 ? line->operands[0] : NULL,
        line->operand_count > 1 ? line->operands[1] : NULL,
        context
    );

    if (instruction_length < 0) {
        report_context_error(context, "Invalid instruction format");
        return false;
    }

    /* Update instruction counter */
    *IC += instruction_length;

    return true;
}

/* Calculate the instruction length in words */
int calculate_instruction_length(const char *opcode, const char *operand1, const char *operand2, error_context_t *context) {
    int length = 1;  /* First word is always present (opcode, funct, registers/addressing) */
    addressing_method_t src_addr = ADDR_IMMEDIATE;
    addressing_method_t dst_addr = ADDR_IMMEDIATE;

    /* Validate opcode */
    if (!opcode || !*opcode) {
        report_context_error(context, "Empty opcode");
        return -1;
    }

    /* Check operand counts for different instructions */
    if (strcmp(opcode, "rts") == 0 || strcmp(opcode, "stop") == 0) {
        /* No operands */
        if (operand1 != NULL) {
            report_context_error(context, "%s instruction takes no operands", opcode);
            return -1;
        }
        return length;
    }

    if (strcmp(opcode, "mov") == 0 || strcmp(opcode, "cmp") == 0 ||
        strcmp(opcode, "add") == 0 || strcmp(opcode, "sub") == 0 ||
        strcmp(opcode, "lea") == 0) {
        /* Two operands required */
        if (!operand1 || !operand2) {
            report_context_error(context, "%s instruction requires two operands", opcode);
            return -1;
        }

        /* Determine addressing mode for both operands */
        if (operand1[0] == '#') {
            src_addr = ADDR_IMMEDIATE;
            length++;  /* Immediate value needs an extra word */
        }
        else if (operand1[0] == '&') {
            src_addr = ADDR_RELATIVE;
            length++;  /* Relative address needs an extra word */
        }
        else if (is_register(operand1)) {
            src_addr = ADDR_REGISTER;
            /* No extra word needed for register addressing */
        }
        else {
            src_addr = ADDR_DIRECT;
            length++;  /* Direct address needs an extra word */
        }

        if (operand2[0] == '#') {
            dst_addr = ADDR_IMMEDIATE;
            length++;
        }
        else if (operand2[0] == '&') {
            dst_addr = ADDR_RELATIVE;
            length++;
        }
        else if (is_register(operand2)) {
            dst_addr = ADDR_REGISTER;
            /* No extra word needed for register addressing */
        }
        else {
            dst_addr = ADDR_DIRECT;
            length++;
        }

        /* Special case: if both operands are registers, they share a word */
        if (src_addr == ADDR_REGISTER && dst_addr == ADDR_REGISTER) {
            length--;
        }

        /* Validate lea instruction - source operand must be a label (direct addressing) */
        if (strcmp(opcode, "lea") == 0 && src_addr != ADDR_DIRECT) {
            report_context_error(context, "lea instruction source operand must be a label");
            return -1;
        }

        return length;
    }

    if (strcmp(opcode, "clr") == 0 || strcmp(opcode, "not") == 0 ||
        strcmp(opcode, "inc") == 0 || strcmp(opcode, "dec") == 0 ||
        strcmp(opcode, "red") == 0 || strcmp(opcode, "prn") == 0) {
        /* One operand required */
        if (!operand1 || operand2) {
            report_context_error(context, "%s instruction requires one operand", opcode);
            return -1;
        }

        /* Determine addressing mode */
        if (operand1[0] == '#') {
            dst_addr = ADDR_IMMEDIATE;
            length++;
        }
        else if (operand1[0] == '&') {
            dst_addr = ADDR_RELATIVE;
            length++;
        }
        else if (is_register(operand1)) {
            dst_addr = ADDR_REGISTER;
            /* No extra word needed for register addressing */
        }
        else {
            dst_addr = ADDR_DIRECT;
            length++;
        }

        return length;
    }

    if (strcmp(opcode, "jmp") == 0 || strcmp(opcode, "bne") == 0 ||
        strcmp(opcode, "jsr") == 0) {
        /* One operand required */
        if (!operand1 || operand2) {
            report_context_error(context, "%s instruction requires one operand", opcode);
            return -1;
        }

        /* Determine addressing mode */
        if (operand1[0] == '&') {
            dst_addr = ADDR_RELATIVE;
            length++;
        }
        else if (is_register(operand1)) {
            dst_addr = ADDR_REGISTER;
            /* No extra word needed for register addressing */
        }
        else {
            dst_addr = ADDR_DIRECT;
            length++;
        }

        return length;
    }

    /* Unknown opcode */
    report_context_error(context, "Unknown opcode: %s", opcode);
    return -1;
}

/* Main function for the first pass */
bool first_pass(const char *filename, symbol_table_t *symbols, error_context_t *context) {
    FILE *file;
    char base_filename[MAX_FILENAME_LENGTH];
    char am_filename[MAX_FILENAME_LENGTH];
    char line_buf[MAX_LINE_LENGTH];
    parsed_line_t parsed_line;
    int IC = 0;  /* Instruction Counter */
    int DC = 0;  /* Data Counter */
    int line_number = 0;
    bool success = true;

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

    /* First pass through the file */
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
                if (!process_data_directive(&parsed_line, symbols, &DC, context)) {
                    success = false;
                }
                break;

            case INST_TYPE_STRING:
                if (!process_string_directive(&parsed_line, symbols, &DC, context)) {
                    success = false;
                }
                break;

            case INST_TYPE_EXTERN:
                if (!process_extern_directive(&parsed_line, symbols, context)) {
                    success = false;
                }
                break;

            case INST_TYPE_ENTRY:
                if (!process_entry_directive(&parsed_line, context)) {
                    success = false;
                }
                break;

            case INST_TYPE_CODE:
                if (!process_instruction(&parsed_line, symbols, &IC, context)) {
                    success = false;
                }
                break;

            default:
                report_context_error(context, "Unknown instruction type");
                success = false;
                break;
        }
    }

    /* Update addresses of data symbols to be after code section */
    update_data_symbols(symbols, IC);

    fclose(file);
    return success;
}

/* Helper function to process a label */
static bool process_label(const char *label, symbol_table_t *symbols, int address,
                         symbol_attr_t attributes, error_context_t *context) {
    symbol_t *existing;

    /* Check if the label already exists */
    existing = find_symbol(symbols, label);
    if (existing) {
        if (symbol_has_attribute(existing, SYMBOL_ATTR_EXTERNAL)) {
            report_context_error(context, "Label '%s' already defined as external", label);
            return false;
        }
        else if (!(attributes & SYMBOL_ATTR_ENTRY)) {
            report_context_error(context, "Label '%s' already defined", label);
            return false;
        }
    }

    /* Add the label to the symbol table */
    if (!add_symbol(symbols, label, address, attributes)) {
        report_context_error(context, "Failed to add label '%s' to symbol table", label);
        return false;
    }

    return true;
}

/* Helper function to get the directive type */
static instruction_type_t get_directive_type(const char *directive) {
    if (strcmp(directive, ".data") == 0) {
        return INST_TYPE_DATA;
    }
    else if (strcmp(directive, ".string") == 0) {
        return INST_TYPE_STRING;
    }
    else if (strcmp(directive, ".entry") == 0) {
        return INST_TYPE_ENTRY;
    }
    else if (strcmp(directive, ".extern") == 0) {
        return INST_TYPE_EXTERN;
    }
    return INST_TYPE_INVALID;
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
    while (token && count < max_count) {
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
    if (token && count >= max_count) {
        report_context_error(context, "Too many numbers in list");
        return -1;
    }

    return count;
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