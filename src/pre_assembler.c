/**
 * @file pre_assembler.c
 * @brief Implementation of the macro processor
 */
#include <string.h>
#include "../include/pre_assembler.h"
#include "../include/utils.h"

#define MAX_MACRO_LINES 1000  /* Maximum number of lines in a macro */
#define MAX_MACRO_NESTING 10  /* Maximum nesting level for macros */

/* Create a new macro table */
macro_table_t* create_macro_table() {
    macro_table_t *table = (macro_table_t *)malloc(sizeof(macro_table_t));
    if (table) {
        table->head = NULL;
    }
    return table;
}

/* Add a new macro to the table */
bool add_macro(macro_table_t *table, const char *name, error_context_t *context) {
    macro_t *macro;

    /* Check if the table is valid */
    if (!table) {
        report_context_error(context, "Invalid macro table");
        return false;
    }

    /* Check if the name is valid (not a reserved word) */
    if (is_reserved_word(name)) {
        report_context_error(context, "Macro name '%s' is a reserved word", name);
        return false;
    }

    /* Check if a macro with this name already exists */
    if (find_macro(table, name)) {
        report_context_error(context, "Macro '%s' already defined", name);
        return false;
    }

    /* Create a new macro */
    macro = (macro_t *)malloc(sizeof(macro_t));
    if (!macro) {
        report_context_error(context, "Memory allocation error");
        return false;
    }

    /* Initialize the macro */
    strncpy(macro->name, name, MAX_LABEL_LENGTH - 1);
    macro->name[MAX_LABEL_LENGTH - 1] = '\0';

    macro->lines = (char **)malloc(MAX_MACRO_LINES * sizeof(char *));
    if (!macro->lines) {
        free(macro);
        report_context_error(context, "Memory allocation error");
        return false;
    }
    macro->line_count = 0;

    /* Add the macro to the table */
    macro->next = table->head;
    table->head = macro;

    return true;
}

/* Add a line to the current macro being defined */
bool add_line_to_macro(macro_table_t *table, const char *line, error_context_t *context) {
    macro_t *macro;

    /* Check if the table is valid */
    if (!table || !table->head) {
        report_context_error(context, "Invalid macro table or no current macro");
        return false;
    }

    /* Get the current macro (the one at the head of the list) */
    macro = table->head;

    /* Check if we can add more lines */
    if (macro->line_count >= MAX_MACRO_LINES) {
        report_context_error(context, "Macro '%s' has too many lines", macro->name);
        return false;
    }

    /* Add the line */
    macro->lines[macro->line_count] = str_duplicate(line);
    if (!macro->lines[macro->line_count]) {
        report_context_error(context, "Memory allocation error");
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

/* Process a source file to expand macros */
bool process_file(const char *filename, error_context_t *context) {
    FILE *source, *output;
    char base_filename[MAX_FILENAME_LENGTH];
    char source_filename[MAX_FILENAME_LENGTH];
    char output_filename[MAX_FILENAME_LENGTH];
    char line[MAX_LINE_LENGTH];
    macro_table_t *macro_table;
    bool success = true;
    char macro_name_stack[MAX_MACRO_NESTING][MAX_LABEL_LENGTH];
    int macro_nesting_level = 0;
    int line_number = 0;
    char *token, *saveptr;

    /* Initialize error context */
    if (context) {
        strncpy(context->filename, filename, MAX_FILENAME_LENGTH - 1);
        context->filename[MAX_FILENAME_LENGTH - 1] = '\0';
    }

    /* Get the base filename */
    get_base_filename(filename, base_filename);

    /* Create source and output filenames */
    create_filename(base_filename, EXT_SOURCE, source_filename);
    create_filename(base_filename, EXT_MACRO, output_filename);

    /* Open the source file */
    source = fopen(source_filename, "r");
    if (!source) {
        report_context_error(context, "Could not open source file: %s", source_filename);
        return false;
    }

    /* Open the output file */
    output = fopen(output_filename, "w");
    if (!output) {
        fclose(source);
        report_context_error(context, "Could not open output file: %s", output_filename);
        return false;
    }

    /* Create the macro table */
    macro_table = create_macro_table();
    if (!macro_table) {
        fclose(source);
        fclose(output);
        report_context_error(context, "Could not create macro table");
        return false;
    }

    /* Process the file line by line */
    while (fgets(line, MAX_LINE_LENGTH, source)) {
        char processed_line[MAX_LINE_LENGTH];
        bool write_line = true;

        line_number++;
        if (context) {
            context->line_number = line_number;
        }

        /* Remove newline character */
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }

        /* Make a copy of the line for processing */
        strncpy(processed_line, line, MAX_LINE_LENGTH - 1);
        processed_line[MAX_LINE_LENGTH - 1] = '\0';

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
        token = safe_strtok_r(processed_line, " \t", &saveptr);

        /* Check for macro definition start */
        if (token && strcmp(token, "mcro") == 0) {
            if (macro_nesting_level >= MAX_MACRO_NESTING) {
                report_context_error(context, "Macro nesting level exceeded");
                success = false;
                continue;
            }

            /* Get the macro name */
            token = safe_strtok_r(NULL, " \t", &saveptr);
            if (!token) {
                report_context_error(context, "Missing macro name");
                success = false;
                continue;
            }

            /* Check for extra tokens */
            if (safe_strtok_r(NULL, " \t", &saveptr)) {
                report_context_error(context, "Extra tokens after macro name");
                success = false;
                continue;
            }

            /* Add the macro to the table */
            if (!add_macro(macro_table, token, context)) {
                success = false;
                continue;
            }

            /* Remember the macro name and increase nesting level */
            strncpy(macro_name_stack[macro_nesting_level], token, MAX_LABEL_LENGTH - 1);
            macro_name_stack[macro_nesting_level][MAX_LABEL_LENGTH - 1] = '\0';
            macro_nesting_level++;
            write_line = false;
        }
        /* Check for macro definition end */
        else if (token && strcmp(token, "endmcro") == 0) {
            if (macro_nesting_level == 0) {
                report_context_error(context, "endmcro without matching mcro");
                success = false;
                continue;
            }

            /* Check for extra tokens */
            if (safe_strtok_r(NULL, " \t", &saveptr)) {
                report_context_error(context, "Extra tokens after endmcro");
                success = false;
                continue;
            }

            /* Decrease nesting level */
            macro_nesting_level--;
            write_line = false;
        }
        /* Check for macro usage */
        else if (token && macro_nesting_level == 0) {
            macro_t *macro = find_macro(macro_table, token);

            /* Check if this is a label followed by a macro */
            if (!macro && token[strlen(token) - 1] == ':') {
                /* This might be a label, check if next token is a macro */
                char *next_token = safe_strtok_r(NULL, " \t", &saveptr);
                if (next_token) {
                    macro = find_macro(macro_table, next_token);
                    if (macro) {
                        int i;

                        /* Write the label part */
                        fprintf(output, "%s ", token);

                        /* Replace the macro with its lines */
                        for (i = 0; i < macro->line_count; i++) {
                            fprintf(output, "%s\n", macro->lines[i]);
                        }

                        write_line = false;
                    }
                }
            }
            /* Regular case - token is directly a macro */
            else if (macro) {
                int i;

                /* Replace the macro with its lines */
                for (i = 0; i < macro->line_count; i++) {
                    fprintf(output, "%s\n", macro->lines[i]);
                }

                write_line = false;
            }
        }
        /* Inside a macro definition */
        else if (macro_nesting_level > 0) {
            /* Add the line to the current macro */
            if (!add_line_to_macro(macro_table, line, context)) {
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
    if (macro_nesting_level > 0) {
        report_context_error(context, "Macro definition not closed");
        success = false;
    }

    /* Clean up */
    free_macro_table(macro_table);
    fclose(source);
    fclose(output);

    return success;
}