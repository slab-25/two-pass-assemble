/**
 * @file pre_assembler.c
 * @brief Implementation of the macro processor
 */

#include "../include/pre_assembler.h"
#include "../include/utils.h"

#define MAX_MACRO_LINES 1000  /* Maximum number of lines in a macro */

/* Create a new macro table */
macro_table_t* create_macro_table() {
    macro_table_t *table = (macro_table_t *)malloc(sizeof(macro_table_t));
    if (table) {
        table->head = NULL;
    }
    return table;
}

/* Add a new macro to the table */
bool add_macro(macro_table_t *table, const char *name) {
    macro_t *macro;

    /* Check if the table is valid */
    if (!table) {
        return false;
    }

    /* Check if the name is valid (not a reserved word) */
    if (is_reserved_word(name)) {
        return false;
    }

    /* Check if a macro with this name already exists */
    if (find_macro(table, name)) {
        return false;
    }

    /* Create a new macro */
    macro = (macro_t *)malloc(sizeof(macro_t));
    if (!macro) {
        return false;
    }

    /* Initialize the macro */
    strcpy(macro->name, name);
    macro->lines = (char **)malloc(MAX_MACRO_LINES * sizeof(char *));
    if (!macro->lines) {
        free(macro);
        return false;
    }
    macro->line_count = 0;

    /* Add the macro to the table */
    macro->next = table->head;
    table->head = macro;

    return true;
}

/* Add a line to the current macro being defined */
bool add_line_to_macro(macro_table_t *table, const char *line) {
    macro_t *macro;

    /* Check if the table is valid */
    if (!table || !table->head) {
        return false;
    }

    /* Get the current macro (the one at the head of the list) */
    macro = table->head;

    /* Check if we can add more lines */
    if (macro->line_count >= MAX_MACRO_LINES) {
        return false;
    }

    /* Add the line */
    macro->lines[macro->line_count] = str_duplicate(line);
    if (!macro->lines[macro->line_count]) {
        return false;
    }
    macro->line_count++;

    return true;
}

/* Find a macro by name */
macro_t* find_macro(macro_table_t *table, const char *name) {
    macro_t *current;

    /* Check if the table is valid */
    if (!table) {
        return NULL;
    }

    /* Search for the macro */
    current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

/* Free the macro table and all its entries */
void free_macro_table(macro_table_t *table) {
    macro_t *current, *next;
    int i;

    /* Check if the table is valid */
    if (!table) {
        return;
    }

    /* Free all macros */
    current = table->head;
    while (current) {
        next = current->next;

        /* Free all lines */
        for (i = 0; i < current->line_count; i++) {
            free(current->lines[i]);
        }
        free(current->lines);
        free(current);

        current = next;
    }

    /* Free the table */
    free(table);
}

/* Process a source file to expand macros */
bool process_file(const char *filename) {
    FILE *source, *output;
    char base_filename[MAX_FILENAME_LENGTH];
    char source_filename[MAX_FILENAME_LENGTH];
    char output_filename[MAX_FILENAME_LENGTH];
    char line[MAX_LINE_LENGTH];
    macro_table_t *macro_table;
    bool in_macro_def = false;
    bool success = true;
    char macro_name[MAX_LABEL_LENGTH];
    int line_number = 0;
    char *token, *saveptr;

    /* Get the base filename */
    get_base_filename(filename, base_filename);

    /* Create source and output filenames */
    create_filename(base_filename, EXT_SOURCE, source_filename);
    create_filename(base_filename, EXT_MACRO, output_filename);

    /* Open the source file */
    source = fopen(source_filename, "r");
    if (!source) {
        report_error(filename, 0, "Could not open source file: %s", source_filename);
        return false;
    }

    /* Open the output file */
    output = fopen(output_filename, "w");
    if (!output) {
        fclose(source);
        report_error(filename, 0, "Could not open output file: %s", output_filename);
        return false;
    }

    /* Create the macro table */
    macro_table = create_macro_table();
    if (!macro_table) {
        fclose(source);
        fclose(output);
        report_error(filename, 0, "Could not create macro table");
        return false;
    }

    /* Process the file line by line */
    while (fgets(line, MAX_LINE_LENGTH, source)) {
        char processed_line[MAX_LINE_LENGTH];
        bool write_line = true;

        line_number++;

        /* Remove newline character */
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }

        /* Make a copy of the line for processing */
        strcpy(processed_line, line);

        /* Skip comments */
        if (processed_line[0] == ';') {
            fprintf(output, "%s\n", processed_line);
            continue;
        }

        /* Trim whitespace */
        trim(processed_line);

        /* Skip empty lines */
        if (processed_line[0] == '\0') {
            fprintf(output, "\n");
            continue;
        }

        /* Tokenize the line */
        token = strtok_r(processed_line, " \t", &saveptr);

        /* Check for macro definition start */
        if (token && strcmp(token, "mcro") == 0) {
            if (in_macro_def) {
                report_error(filename, line_number, "Nested macro definition");
                success = false;
                continue;
            }

            /* Get the macro name */
            token = strtok_r(NULL, " \t", &saveptr);
            if (!token) {
                report_error(filename, line_number, "Missing macro name");
                success = false;
                continue;
            }

            /* Check for extra tokens */
            if (strtok_r(NULL, " \t", &saveptr)) {
                report_error(filename, line_number, "Extra tokens after macro name");
                success = false;
                continue;
            }

            /* Add the macro to the table */
            if (!add_macro(macro_table, token)) {
                report_error(filename, line_number, "Invalid macro name or duplicate macro: %s", token);
                success = false;
                continue;
            }

            /* Remember the macro name and that we're in a macro definition */
            strcpy(macro_name, token);
            in_macro_def = true;
            write_line = false;
        }
        /* Check for macro definition end */
        else if (token && strcmp(token, "endmcro") == 0) {
            if (!in_macro_def) {
                report_error(filename, line_number, "endmcro without matching mcro");
                success = false;
                continue;
            }

            /* Check for extra tokens */
            if (strtok_r(NULL, " \t", &saveptr)) {
                report_error(filename, line_number, "Extra tokens after endmcro");
                success = false;
                continue;
            }

            /* End the macro definition */
            in_macro_def = false;
            write_line = false;
        }
        /* Check for macro usage */
        else if (token && !in_macro_def) {
            macro_t *macro = find_macro(macro_table, token);

            if (macro) {
                int i;

                /* Replace the macro with its lines */
                for (i = 0; i < macro->line_count; i++) {
                    fprintf(output, "%s\n", macro->lines[i]);
                }

                write_line = false;
            }
        }
        /* Inside a macro definition */
        else if (in_macro_def) {
            /* Add the line to the macro */
            if (!add_line_to_macro(macro_table, line)) {
                report_error(filename, line_number, "Could not add line to macro");
                success = false;
                continue;
            }

            write_line = false;
        }

        /* Write the line to the output file if needed */
        if (write_line) {
            fprintf(output, "%s\n", line);
        }
    }

    /* Check if we ended with an open macro definition */
    if (in_macro_def) {
        report_error(filename, line_number, "Macro definition not closed");
        success = false;
    }

    /* Clean up */
    free_macro_table(macro_table);
    fclose(source);
    fclose(output);

    return success;
}