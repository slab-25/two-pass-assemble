/**
 * @file utils.h
 * @brief Utility functions for the assembler
 */

#ifndef UTILS_H
#define UTILS_H

#include "assembler.h"
#include "error.h"

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

/**
 * @brief Display version information of the assembler
 */
void print_version();

#endif /* UTILS_H */