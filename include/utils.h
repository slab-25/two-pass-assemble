/**
 * @file utils.h
 * @brief Utility functions for the assembler
 */

#ifndef UTILS_H
#define UTILS_H

#include "assembler.h"

/**
 * @brief Report an error with filename and line number
 * @param filename The source filename
 * @param line_number The line number where the error occurred
 * @param format The error message format
 * @param ... Additional arguments for the format string
 */
void report_error(const char *filename, int line_number, const char *format, ...);

/**
 * @brief Trim whitespace from the beginning and end of a string
 * @param str The string to trim (modified in-place)
 * @return Pointer to the trimmed string (same as input)
 */
char* trim(char *str);

/**
 * @brief Check if a string is a valid label name
 * @param label The label name to check
 * @return true if valid, false otherwise
 */
bool is_valid_label(const char *label);

/**
 * @brief Check if a string is a register name (r0-r7)
 * @param str The string to check
 * @return true if it's a register, false otherwise
 */
bool is_register(const char *str);

/**
 * @brief Get the register number from a register name
 * @param reg_str The register name (e.g., "r3")
 * @return The register number (0-7) or -1 if invalid
 */
int get_register_number(const char *reg_str);

/**
 * @brief Check if a string is a valid integer
 * @param str The string to check
 * @return true if it's a valid integer, false otherwise
 */
bool is_integer(const char *str);

/**
 * @brief Convert a string to an integer
 * @param str The string to convert
 * @return The integer value
 */
int string_to_int(const char *str);

/**
 * @brief Create a copy of a string
 * @param str The string to copy
 * @return A new string that must be freed by the caller
 */
char* str_duplicate(const char *str);

/**
 * @brief Check if a string is a reserved word (instruction or directive)
 * @param str The string to check
 * @return true if it's a reserved word, false otherwise
 */
bool is_reserved_word(const char *str);

/**
 * @brief Parse a line into tokens (e.g., label, opcode, operands)
 * @param line The line to parse
 * @param label Output parameter for the label (if any)
 * @param opcode Output parameter for the opcode
 * @param operands Output parameter for the operands
 * @param operand_count Output parameter for the number of operands
 * @return true if parsing was successful, false otherwise
 */
bool parse_line(const char *line, char *label, char *opcode,
                char operands[MAX_OPERANDS][MAX_OPERAND_LENGTH], int *operand_count);

/**
 * @brief Get the extension-less base name from a filename
 * @param filename The filename
 * @param base Output parameter for the base name
 */
void get_base_filename(const char *filename, char *base);

/**
 * @brief Create a filename with a specific extension
 * @param base The base filename (without extension)
 * @param extension The extension to add
 * @param result Output parameter for the result
 */
void create_filename(const char *base, const char *extension, char *result);

#endif /* UTILS_H */