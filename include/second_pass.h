/**
 * @file second_pass.h
 * @brief Second pass of the assembler
 */

#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "assembler.h"
#include "symbol_table.h"
#include "first_pass.h"
#include "error.h"
#include "machine_word.h"

/**
 * @brief Instruction code structure
 */
typedef struct {
    machine_word_t words[MAX_INSTRUCTION_WORDS];
    int word_count;
} instruction_code_t;

/**
 * @brief External reference structure
 */
typedef struct external_reference {
    char name[MAX_LABEL_LENGTH];
    int address;
    struct external_reference *next;
} external_reference_t;

/**
 * @brief Determine the addressing method for an operand
 * @param operand The operand string
 * @return The addressing method
 */
addressing_method_t get_addressing_method(const char *operand);

/**
 * @brief Encode an operand word based on its addressing method
 * @param word Output parameter for the encoded word
 * @param operand The operand string
 * @param addr_method The addressing method
 * @param symbols The symbol table
 * @param current_address The current instruction address
 * @param word_offset Offset from current address for this word
 * @param ext_refs Output parameter for external references
 * @param context Error context for reporting issues
 * @return true if encoding was successful, false otherwise
 */
bool encode_operand_word(machine_word_t *word, const char *operand,
                         addressing_method_t addr_method,
                         symbol_table_t *symbols, int current_address,
                         int word_offset, external_reference_t **ext_refs,
                         error_context_t *context);

/**
 * @brief Encode a machine instruction
 * @param line The parsed line
 * @param symbols The symbol table
 * @param code Output parameter for the encoded instruction
 * @param current_address The current instruction address
 * @param ext_refs Output parameter for external references
 * @param context Error context for reporting issues
 * @return true if encoding was successful, false otherwise
 */
bool encode_instruction(parsed_line_t *line, symbol_table_t *symbols,
                       instruction_code_t *code, int current_address,
                       external_reference_t **ext_refs, error_context_t *context);

/**
 * @brief Process an entry directive
 * @param line The parsed line
 * @param symbols The symbol table
 * @param context Error context for reporting issues
 * @return true if processing was successful, false otherwise
 */
bool process_entry_second_pass(parsed_line_t *line, symbol_table_t *symbols, error_context_t *context);

/**
 * @brief Add an external reference
 * @param ext_refs Pointer to the list of external references
 * @param name The name of the external symbol
 * @param address The address where it's referenced
 * @param context Error context for reporting issues
 * @return true if the reference was added successfully, false otherwise
 */
bool add_external_reference(external_reference_t **ext_refs, const char *name, int address, error_context_t *context);

/**
 * @brief Free the list of external references
 * @param ext_refs The list of external references
 */
void free_external_references(external_reference_t *ext_refs);

/**
 * @brief Main function for the second pass
 * @param filename The name of the source file
 * @param symbols The symbol table
 * @param code_image Output parameter for the code image
 * @param data_image Output parameter for the data image
 * @param ext_refs Output parameter for external references
 * @param ICF Output parameter for the final instruction counter
 * @param DCF Output parameter for the final data counter
 * @param context Error context for reporting issues
 * @return true if the second pass was successful, false otherwise
 */
bool second_pass(const char *filename, symbol_table_t *symbols,
                machine_word_t **code_image, machine_word_t **data_image,
                external_reference_t **ext_refs, int *ICF, int *DCF,
                error_context_t *context);

#endif /* SECOND_PASS_H */