/**
 * @file error.c
 * @brief Implementation of the error handling system
 */

#include "../include/error.h"

/* Initialize an error context */
void init_error_context(error_context_t *context, const char *filename) {
    if (!context) {
        return;
    }

    context->had_error = false;

    if (filename) {
        strncpy(context->filename, filename, MAX_FILENAME_LENGTH - 1);
        context->filename[MAX_FILENAME_LENGTH - 1] = '\0';
    } else {
        context->filename[0] = '\0';
    }

    context->line_number = 0;
}

/* Report an error with the current context */
void report_context_error(error_context_t *context, const char *format, ...) {
    va_list args;

    if (!context || !format) {
        return;
    }

    /* Mark that an error occurred */
    context->had_error = true;

    /* Print the error message */
    fprintf(stderr, "Error in %s, line %d: ",
            context->filename[0] ? context->filename : "unknown",
            context->line_number);

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
}

/* Set the current line number in the error context */
void set_error_line(error_context_t *context, int line_number) {
    if (context) {
        context->line_number = line_number;
    }
}

/* Cleanup function for freeing resources on error */
void cleanup_resources(FILE *file, void *ptr1, void *ptr2, void *ptr3) {
    if (file) {
        fclose(file);
    }

    if (ptr1) {
        free(ptr1);
    }

    if (ptr2) {
        free(ptr2);
    }

    if (ptr3) {
        free(ptr3);
    }
}