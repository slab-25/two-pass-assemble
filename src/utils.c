/**
 * @file utils.c
 * @brief Implementation of utility functions for the assembler
 */

#include "../include/utils.h"

/* Reserved words - opcodes and directives */
static const char *reserved_words[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec",
    "jmp", "bne", "jsr", "red", "prn", "rts", "stop",
    ".data", ".string", ".entry", ".extern", "mcro", "endmcro", NULL
};

void report_error(const char *filename, int line_number, const char *format, ...) {
    va_list args;
    fprintf(stderr, "Error in %s, line %d: ", filename, line_number);

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
}

char* trim(char *str) {
    char *end;

    /* Skip leading whitespace */
    while (isspace((unsigned char)*str)) {
        str++;
    }

    /* If the string is now empty, return it */
    if (*str == '\0') {
        return str;
    }

    /* Find the end of the string */
    end = str + strlen(str) - 1;

    /* Trim trailing whitespace */
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    /* Null-terminate the trimmed string */
    *(end + 1) = '\0';

    return str;
}

bool is_valid_label(const char *label) {
    int i, len;

    /* Check for NULL or empty string */
    if (!label || *label == '\0') {
        return false;
    }

    /* Check if the first character is alphabetic */
    if (!isalpha((unsigned char)label[0])) {
        return false;
    }

    /* Check the rest of the characters (alphanumeric) */
    len = strlen(label);
    for (i = 1; i < len; i++) {
        if (!isalnum((unsigned char)label[i])) {
            return false;
        }
    }

    /* Check if the label is too long */
    if (len > MAX_LABEL_LENGTH - 1) {
        return false;
    }

    /* Check if it's a reserved word */
    if (is_reserved_word(label)) {
        return false;
    }

    return true;
}

bool is_register(const char *str) {
    /* Check if the string is in the format 'r[0-7]' */
    if (str && str[0] == 'r' && strlen(str) == 2 &&
        str[1] >= '0' && str[1] <= '7') {
        return true;
    }
    return false;
}

int get_register_number(const char *reg_str) {
    /* If it's a valid register, return its number */
    if (is_register(reg_str)) {
        return reg_str[1] - '0';
    }
    return -1;
}

bool is_integer(const char *str) {
    char *endptr;

    /* Check for NULL or empty string */
    if (!str || *str == '\0') {
        return false;
    }

    /* Skip leading whitespace */
    while (isspace((unsigned char)*str)) {
        str++;
    }

    /* Check for sign */
    if (*str == '+' || *str == '-') {
        str++;
    }

    /* Check if there are digits */
    if (!*str) {
        return false;
    }

    /* Attempt conversion */
    strtol(str, &endptr, 10);

    /* If endptr points to the end of the string, conversion was successful */
    return *endptr == '\0';
}

int string_to_int(const char *str) {
    return (int)strtol(str, NULL, 10);
}

char* str_duplicate(const char *str) {
    char *result = (char *)malloc(strlen(str) + 1);
    if (result) {
        strcpy(result, str);
    }
    return result;
}

bool is_reserved_word(const char *str) {
    int i;

    for (i = 0; reserved_words[i] != NULL; i++) {
        if (strcmp(str, reserved_words[i]) == 0) {
            return true;
        }
    }

    return false;
}

bool parse_line(const char *line, char *label, char *opcode,
                char operands[MAX_OPERANDS][MAX_OPERAND_LENGTH], int *operand_count) {
    char temp_line[MAX_LINE_LENGTH];
    char *token, *saveptr1, *saveptr2;
    bool has_label = false;

    /* Initialize outputs */
    *label = '\0';
    *opcode = '\0';
    *operand_count = 0;

    /* Copy the line to avoid modifying the original */
    strncpy(temp_line, line, MAX_LINE_LENGTH - 1);
    temp_line[MAX_LINE_LENGTH - 1] = '\0';

    /* Remove comments */
    token = strchr(temp_line, ';');
    if (token) {
        *token = '\0';
    }

    /* Trim whitespace */
    trim(temp_line);

    /* Empty line after removing comments */
    if (temp_line[0] == '\0') {
        return true;
    }

    /* Check for label */
    token = strchr(temp_line, ':');
    if (token) {
        has_label = true;
        *token = '\0';  /* Split at the colon */
        strcpy(label, trim(temp_line));
        token++;  /* Move past the colon */
    } else {
        token = temp_line;
    }

    /* Get the opcode */
    token = strtok_r(has_label ? token : temp_line, " \t", &saveptr1);
    if (!token) {
        return false;  /* No opcode found */
    }
    strcpy(opcode, token);

    /* Get the operands */
    token = strtok_r(NULL, ",", &saveptr1);
    while (token && *operand_count < MAX_OPERANDS) {
        strcpy(operands[*operand_count], trim(token));
        (*operand_count)++;
        token = strtok_r(NULL, ",", &saveptr1);
    }

    return true;
}

void get_base_filename(const char *filename, char *base) {
    const char *ext;
    size_t len;

    /* Find the extension */
    ext = strrchr(filename, '.');
    if (ext) {
        len = ext - filename;
    } else {
        len = strlen(filename);
    }

    /* Copy the base name */
    strncpy(base, filename, len);
    base[len] = '\0';
}

void create_filename(const char *base, const char *extension, char *result) {
    sprintf(result, "%s%s", base, extension);
}