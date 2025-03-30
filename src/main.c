/**
* @file main.c
 * @brief Main entry point for the assembler
 *
 * This implementation complies with C90 standard and is designed to compile
 * on Ubuntu 16.04 with GCC using the flags: -std=c90 -Wall -Wextra -pedantic
 */
#include <stdbool.h>
#include "../include/assembler.h"
#include "../include/utils.h"
#include "../include/pre_assembler.h"
#include "../include/first_pass.h"
#include "../include/second_pass.h"
#include "../include/symbol_table.h"
#include "../include/output.h"

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
        char filename[MAX_FILENAME_LENGTH];

        /* Create base filename without extension */
        strncpy(filename, argv[i], MAX_FILENAME_LENGTH - 1);
        filename[MAX_FILENAME_LENGTH - 1] = '\0';

        printf("Processing file: %s\n", filename);

        /* Step 1: Pre-assembler (macro processor) */
        if (!process_file(filename)) {
            fprintf(stderr, "Error in pre-assembler phase for %s\n", filename);
            success = false;
            continue;
        }

        printf("Successfully processed %s\n", filename);

        /* Placeholder for first and second pass */
        /* This will be implemented in subsequent phases */
    }

    return success ? 0 : 1;
}