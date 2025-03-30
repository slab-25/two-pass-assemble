/**
 * @file first_pass.h
 * @brief First pass of the assembler
 */

#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "assembler.h"
#include "symbol_table.h"

/**
 * @brief Parsed line data
 */
typedef struct {
    instruction_type_t type;
    char label[MAX_LABEL_LENGTH];
    char opcode[MAX_OPCODE_LENGTH];
    char operands[MAX_OPERANDS][MAX_OPERAND_LENGTH];
    int operand_count;
    int line_number;
} parsed_line_t;

/**
 * @brief Parse a line into its components
 * @param line The line to parse
 * @param parsed Output parameter for the parsed line
 * @param line_number The line number
 * @return true if parsing was successful, false otherwise
 */
bool parse_line(const char *line, parsed_line_t *parsed, int line_number);

/**
 * @brief Process a .data directive
 * @param line The parsed line
 * @param symbols The symbol table
 * @param DC Pointer to the data counter
 * @return true if processing was successful, false otherwise
 */
bool process_data_directive(parsed_line_t *line, symbol_table_t *symbols, int *DC);

/**
 * @brief Process a .string directive
 * @param line The parsed line
 * @param symbols The symbol table
 * @param DC Pointer to the data counter
 * @return true if processing was successful, false otherwise
 */
bool process_string_directive(parsed_line_t *line, symbol_table_t *symbols, int *DC);

/**
 * @brief Process a .extern directive
 * @param line The parsed line
 * @param symbols The symbol table
 * @return true if processing was successful, false otherwise
 */
bool process_extern_directive(parsed_line_t *line, symbol_table_t *symbols);

/**
 * @brief Process a .entry directive
 * @param line The parsed line
 * @return true if processing was successful, false otherwise
 */
bool process_entry_directive(parsed_line_t *line);

/**
 * @brief Process a machine instruction
 * @param line The parsed line
 * @param symbols The symbol table
 * @param IC Pointer to the instruction counter
 * @return true if processing was successful, false otherwise
 */
bool process_instruction(parsed_line_t *line, symbol_table_t *symbols, int *IC);

/**
 * @brief Calculate the instruction length (in words)
 * @param opcode The operation code
 * @param operand1 The first operand (can be NULL)
 * @param operand2 The second operand (can be NULL)
 * @return The instruction length in words, or -1 if invalid
 */
int calculate_instruction_length(const char *opcode, const char *operand1, const char *operand2);

/**
 * @brief Main function for the first pass
 * @param filename The name of the source file
 * @param symbols The symbol table
 * @return true if the first pass was successful, false otherwise
 */
bool first_pass(const char *filename, symbol_table_t *symbols);

#endif /* FIRST_PASS_H */