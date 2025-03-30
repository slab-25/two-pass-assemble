/**
 * @file main.c
 * @brief Main entry point for the assembler
 *
 * This implementation complies with C90 standard and is designed to compile
 * on Ubuntu 16.04 with GCC using the flags: -std=c90 -Wall -Wextra -pedantic
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/assembler.h"
#include "../include/utils.h"
#include "../include/pre_assembler.h"
#include "../include/first_pass.h"
#include "../include/second_pass.h"
#include "../include/symbol_table.h"
#include "../include/output.h"
#include "../include/error.h"

/**
 * @brief Process a single assembly file
 * @param filename The name of the source file
 * @return true if processing was successful, false otherwise
 */
bool process_assembly_file(const char *filename) {
    symbol_table_t *symbols = NULL;
    machine_word_t *code_image = NULL;
    machine_word_t *data_image = NULL;
    external_reference_t *ext_refs = NULL;
    int ICF = 0, DCF = 0;
    error_context_t context;

    /* Initialize error context */
    init_error_context(&context, filename);

    printf("Processing file: %s\n", filename);

    /* Step 1: Pre-assembler (macro processor) */
    if (!process_file(filename, &context)) {
        fprintf(stderr, "Error in pre-assembler phase for %s\n", filename);
        return false;
    }

    printf("Pre-assembler phase successful for %s\n", filename);

    /* Step 2: Create symbol table and perform first pass */
    symbols = create_symbol_table();
    if (!symbols) {
        report_context_error(&context, "Memory allocation error for symbol table");
        return false;
    }

    if (!first_pass(filename, symbols, &context)) {
        fprintf(stderr, "Error in first pass phase for %s\n", filename);
        free_symbol_table(symbols);
        return false;
    }

    printf("First pass phase successful for %s\n", filename);

    /* Step 3: Perform second pass - encode instructions */
    if (!second_pass(filename, symbols, &code_image, &data_image, &ext_refs, &ICF, &DCF, &context)) {
        fprintf(stderr, "Error in second pass phase for %s\n", filename);
        free_symbol_table(symbols);
        return false;
    }

    printf("Second pass phase successful for %s\n", filename);

    /* Step 4: Generate output files */
    if (!generate_output_files(filename, symbols, code_image, data_image, ext_refs, ICF, DCF, &context)) {
        fprintf(stderr, "Error in output generation phase for %s\n", filename);
        cleanup_resources(NULL, symbols, code_image, data_image);
        free_external_references(ext_refs);
        return false;
    }

    printf("Successfully processed %s\n", filename);

    /* Clean up resources */
    free_symbol_table(symbols);
    free(code_image);
    free(data_image);
    free_external_references(ext_refs);

    return true;
}

/**
 * @brief Main entry point for the assembler
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return 0 on success, non-zero on failure
 */
int main(int argc, char *argv[]) {
    int i;
    bool success = true;

    /* Check command-line arguments */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file1 file2 ...\n", argv[0]);
        return 1;
    }

    /* Process each file */
    for (i = 1; i < argc; i++) {
        if (!process_assembly_file(argv[i])) {
            success = false;
        }
    }

    return success ? 0 : 1;
}