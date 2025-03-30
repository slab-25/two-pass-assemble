/**
* @file error.h
 * @brief Error handling system for the assembler
 */

#ifndef ERROR_H
#define ERROR_H

#include "assembler.h"

/**
 * @brief Error context structure
 */
typedef struct {
    bool had_error;              /* Flag indicating if an error occurred */
    char filename[MAX_FILENAME_LENGTH]; /* Current filename being processed */
    int line_number;             /* Current line number being processed */
} error_context_t;

/**
 * @brief Initialize an error context
 * @param context The error context to initialize
 * @param filename The current filename
 */
void init_error_context(error_context_t *context, const char *filename);

/**
 * @brief Report an error with the current context
 * @param context The error context
 * @param format The error message format
 * @param ... Additional arguments for the format string
 */
void report_context_error(error_context_t *context, const char *format, ...);

/**
 * @brief Set the current line number in the error context
 * @param context The error context
 * @param line_number The current line number
 */
void set_error_line(error_context_t *context, int line_number);

/**
 * @brief Cleanup function for freeing resources on error
 * @param file File pointer to close (can be NULL)
 * @param ptr1 First pointer to free (can be NULL)
 * @param ptr2 Second pointer to free (can be NULL)
 * @param ptr3 Third pointer to free (can be NULL)
 */
void cleanup_resources(FILE *file, void *ptr1, void *ptr2, void *ptr3);

#endif /* ERROR_H */