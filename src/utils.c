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

/* Trim whitespace from beginning and end of string */
char* trim(char *str) {
    char *end;

    /* Check for NULL pointer */
    if (!str)
        return NULL;

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

/* Check if string is a valid label name */
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

/* Check if string is a register name */
bool is_register(const char *str) {
    /* Check if the string is in the format 'r[0-7]' */
    if (str && str[0] == 'r' && strlen(str) == 2 &&
        str[1] >= '0' && str[1] <= '7') {
        return true;
    }
    return false;
}

/* Get register number from register name */
int get_register_number(const char *reg_str) {
    /* If it's a valid register, return its number */
    if (is_register(reg_str)) {
        return reg_str[1] - '0';
    }
    return -1;
}

/* Check if string is a valid integer */
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

/* Convert string to integer */
int string_to_int(const char *str) {
    return (int)strtol(str, NULL, 10);
}

/* Duplicate a string */
char* str_duplicate(const char *str) {
    char *result;

    if (!str) {
        return NULL;
    }

    result = (char *)malloc(strlen(str) + 1);
    if (result) {
        strcpy(result, str);
    }
    return result;
}

/* Check if string is a reserved word */
bool is_reserved_word(const char *str) {
    int i;

    if (!str) {
        return false;
    }

    for (i = 0; reserved_words[i] != NULL; i++) {
        if (strcmp(str, reserved_words[i]) == 0) {
            return true;
        }
    }

    return false;
}

/* Get base filename without extension */
void get_base_filename(const char *filename, char *base) {
    const char *ext;
    size_t len;

    if (!filename || !base) {
        return;
    }

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

/* Create filename with extension */
void create_filename(const char *base, const char *extension, char *result) {
    if (!base || !extension || !result) {
        return;
    }

    sprintf(result, "%s%s", base, extension);
}

/* Display version information */
void print_version() {
    printf("Two-Pass Assembler v%s\n", ASSEMBLER_VERSION);
    printf("C90 compliant implementation\n");
}