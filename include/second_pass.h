/**
 * @file second_pass.h
 * @brief Second pass of the assembler
 */

#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "assembler.h"
#include "symbol_table.h"
#include "first_pass.h"

/**
 * @brief Machine word structure
 */
typedef struct {
    unsigned int value: 21;  /* 21-bit value */
    unsigned int ARE: 3;     /* A=1 if absolute, R=1 if relocatable, E=1 if external */
} machine_word_t;

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
 * @brief Encode the first word of an instruction
 * @param opcode The operation code
 * @param src_addr The source addressing method
 * @param src_reg The source register number (if applicable)
 * @param dst_addr The destination addressing method
 * @param dst_reg The destination register number (if applicable)
 * @return The encoded first word
 */
machine_word_t encode_first_word(const char *opcode, addressing_method_t src_addr,
                                int src_reg, addressing_method_t dst_addr, int dst_reg);

/**
 * @brief Encode a machine instruction
 * @param line The parsed line
 * @param symbols The symbol table
 * @param code Output parameter for the encoded instruction
 * @param current_address The current instruction address
 * @param ext_refs Output parameter for external references
 * @return true if encoding was successful, false otherwise
 */
bool encode_instruction(parsed_line_t *line, symbol_table_t *symbols,
                       instruction_code_t *code, int current_address,
                       external_reference_t **ext_refs);

/**
 * @brief Process an entry directive
 * @param line The parsed line
 * @param symbols The symbol table
 * @return true if processing was successful, false otherwise
 */
bool process_entry_second_pass(parsed_line_t *line, symbol_table_t *symbols);

/**
 * @brief Add an external reference
 * @param ext_refs Pointer to the list of external references
 * @param name The name of the external symbol
 * @param address The address where it's referenced
 * @return true if the reference was added successfully, false otherwise
 */
bool add_external_reference(external_reference_t **ext_refs, const char *name, int address);

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
 * @return true if the second pass was successful, false otherwise
 */
bool second_pass(const char *filename, symbol_table_t *symbols,
                machine_word_t **code_image, machine_word_t **data_image,
                external_reference_t **ext_refs, int *ICF, int *DCF);

#endif /* SECOND_PASS_H */