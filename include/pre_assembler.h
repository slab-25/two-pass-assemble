/**
* @file pre_assembler.h
 * @brief Macro processor for the assembler
 */

#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include "assembler.h"
#include "error.h"

/**
 * @brief Macro definition structure
 */
typedef struct macro {
    char name[MAX_LABEL_LENGTH];  /* Macro name */
    char **lines;                 /* Array of lines in the macro */
    int line_count;               /* Number of lines in the macro */
    int usage_count;              /* Number of times the macro is used */
    struct macro *next;           /* Pointer to the next macro in the list */
} macro_t;

/**
 * @brief Macro table structure
 */
typedef struct macro_table {
    macro_t *head;                /* Pointer to the first macro in the list */
} macro_table_t;

/**
 * @brief Create a new macro table
 * @return Pointer to the newly created macro table
 */
macro_table_t* create_macro_table();

/**
 * @brief Add a new macro to the table
 * @param table The macro table
 * @param name The name of the macro
 * @param context Error context for reporting issues
 * @return true if the macro was added successfully, false otherwise
 */
bool add_macro(macro_table_t *table, const char *name, error_context_t *context);

/**
 * @brief Add a line to the current macro being defined
 * @param table The macro table
 * @param line The line to add
 * @param context Error context for reporting issues
 * @return true if the line was added successfully, false otherwise
 */
bool add_line_to_macro(macro_table_t *table, const char *line, error_context_t *context);

/**
 * @brief Find a macro by name
 * @param table The macro table
 * @param name The name of the macro to find
 * @return Pointer to the macro if found, NULL otherwise
 */
macro_t* find_macro(macro_table_t *table, const char *name);

/**
 * @brief Free the macro table and all its entries
 * @param table The macro table to free
 */
void free_macro_table(macro_table_t *table);

/**
 * @brief Process a source file to expand macros
 * @param filename The name of the source file
 * @param context Error context for reporting issues
 * @return true if processing was successful, false otherwise
 *
 * This function reads an assembly source file, expands all macros using the
 * specified syntax (mcro/mcroend), and writes the expanded code to a new file.
 * Macros are defined with the 'mcro' directive and terminated with 'mcroend'.
 * Macro invocation is done by simply using the macro name as a token.
 */
bool process_file(const char *filename, error_context_t *context);

#endif /* PRE_ASSEMBLER_H */