/**
 * @file output.c
 * @brief Implementation of output file generation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/output.h"
#include "../include/utils.h"

/* Base64-like character set for encoding machine words */
static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* Helper function to convert a machine word to a base64-like representation */
static void word_to_base64(machine_word_t word, char *base64);

/* Generate the output files */
bool generate_output_files(const char *filename, symbol_table_t *symbols,
                         machine_word_t *code_image, machine_word_t *data_image,
                         external_reference_t *ext_refs, int ICF, int DCF,
                         error_context_t *context) {
    bool success = true;

    /* Initialize/update error context */
    if (context) {
        strncpy(context->filename, filename, MAX_FILENAME_LENGTH - 1);
        context->filename[MAX_FILENAME_LENGTH - 1] = '\0';
        context->line_number = 0;
    }

    /* Write the object file */
    if (!write_object_file(filename, code_image, data_image, ICF, DCF, context)) {
        report_context_error(context, "Failed to write object file");
        success = false;
    }

    /* Write the entries file (only if there are entries) */
    if (success && has_entries(symbols)) {
        if (!write_entries_file(filename, symbols, context)) {
            report_context_error(context, "Failed to write entries file");
            success = false;
        }
    }

    /* Write the externals file (only if there are external references) */
    if (success && ext_refs) {
        if (!write_externals_file(filename, ext_refs, context)) {
            report_context_error(context, "Failed to write externals file");
            success = false;
        }
    }

    return success;
}

/* Write the object file */
bool write_object_file(const char *filename, machine_word_t *code_image,
                      machine_word_t *data_image, int ICF, int DCF,
                      error_context_t *context) {
    FILE *ob_file;
    char base_filename[MAX_FILENAME_LENGTH];
    char ob_filename[MAX_FILENAME_LENGTH];
    int i;
    char base64[3]; /* Base64 requires 2 chars plus null terminator */

    /* Build the .ob filename */
    get_base_filename(filename, base_filename);
    create_filename(base_filename, EXT_OBJECT, ob_filename);

    /* Open the file */
    ob_file = fopen(ob_filename, "w");
    if (!ob_file) {
        report_context_error(context, "Could not open file: %s", ob_filename);
        return false;
    }

    /* Write the header with IC and DC values */
    fprintf(ob_file, "%d %d\n", ICF, DCF);

    /* Write the code image */
    for (i = 0; i < ICF; i++) {
        /* Convert the machine word to base64 */
        word_to_base64(code_image[MEMORY_START + i], base64);

        /* Write the address and the encoded word */
        fprintf(ob_file, "%04d %s\n", MEMORY_START + i, base64);
    }

    /* Write the data image */
    for (i = 0; i < DCF; i++) {
        /* Convert the machine word to base64 */
        word_to_base64(data_image[i], base64);

        /* Write the address and the encoded word */
        fprintf(ob_file, "%04d %s\n", MEMORY_START + ICF + i, base64);
    }

    fclose(ob_file);
    return true;
}

/* Write the entries file */
bool write_entries_file(const char *filename, symbol_table_t *symbols,
                       error_context_t *context) {
    FILE *ent_file;
    char base_filename[MAX_FILENAME_LENGTH];
    char ent_filename[MAX_FILENAME_LENGTH];
    symbol_t *symbol;

    /* Build the .ent filename */
    get_base_filename(filename, base_filename);
    create_filename(base_filename, EXT_ENTRY, ent_filename);

    /* Open the file */
    ent_file = fopen(ent_filename, "w");
    if (!ent_file) {
        report_context_error(context, "Could not open file: %s", ent_filename);
        return false;
    }

    /* Iterate through the symbol table and write entries */
    symbol = symbols->head;
    while (symbol) {
        if (symbol_has_attribute(symbol, SYMBOL_ATTR_ENTRY)) {
            fprintf(ent_file, "%s %04d\n", symbol->name, symbol->value);
        }
        symbol = symbol->next;
    }

    fclose(ent_file);
    return true;
}

/* Write the externals file */
bool write_externals_file(const char *filename, external_reference_t *ext_refs,
                         error_context_t *context) {
    FILE *ext_file;
    char base_filename[MAX_FILENAME_LENGTH];
    char ext_filename[MAX_FILENAME_LENGTH];
    external_reference_t *ref;

    /* Build the .ext filename */
    get_base_filename(filename, base_filename);
    create_filename(base_filename, EXT_EXTERN, ext_filename);

    /* Open the file */
    ext_file = fopen(ext_filename, "w");
    if (!ext_file) {
        report_context_error(context, "Could not open file: %s", ext_filename);
        return false;
    }

    /* Iterate through the external references and write them */
    ref = ext_refs;
    while (ref) {
        fprintf(ext_file, "%s %04d\n", ref->name, ref->address);
        ref = ref->next;
    }

    fclose(ext_file);
    return true;
}

/* Helper function to check if symbol table has entries */
bool has_entries(symbol_table_t *symbols) {
    symbol_t *symbol;

    if (!symbols || !symbols->head) {
        return false;
    }

    symbol = symbols->head;
    while (symbol) {
        if (symbol_has_attribute(symbol, SYMBOL_ATTR_ENTRY)) {
            return true;
        }
        symbol = symbol->next;
    }

    return false;
}

/* Helper function to convert a machine word to a base64-like representation
 * The 24-bit word is converted to four base64 characters (6 bits each)
 */
static void word_to_base64(machine_word_t word, char *base64) {
    unsigned int combined_value;
    int i;

    /* Combine the value and ARE bits */
    combined_value = (word.value << 3) | word.are;

    /* Extract 6-bit segments and convert to base64 */
    for (i = 0; i < 2; i++) {
        /* Extract 6 bits, starting from most significant bits */
        int index = (combined_value >> (12 - i * 6)) & 0x3F;
        base64[i] = base64_chars[index];
    }
    base64[2] = '\0';
}