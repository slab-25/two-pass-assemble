/**
 * @file machine_word.h
 * @brief Machine word representation and operations
 */

#ifndef MACHINE_WORD_H
#define MACHINE_WORD_H

#include "assembler.h"

/**
 * @brief Machine word structure with 24-bit layout
 */
typedef struct {
    unsigned int value: 21;  /* 21-bit value */
    unsigned int are: 3;     /* A=1 if absolute, R=1 if relocatable, E=1 if external */
} machine_word_t;

/**
 * @brief A/R/E values for machine words
 */
typedef enum {
    WORD_ARE_NONE = 0,           /* No bits set */
    WORD_ARE_EXTERNAL = 1,       /* E bit = 1 (binary 001) */
    WORD_ARE_RELOCATABLE = 2,    /* R bit = 1 (binary 010) */
    WORD_ARE_ABSOLUTE = 4        /* A bit = 1 (binary 100) */
} are_type_t;

/**
 * @brief Initialize a machine word
 * @param word Pointer to the machine word to initialize
 * @param value The initial value
 * @param are The ARE bits
 */
void word_init(machine_word_t *word, unsigned int value, are_type_t are);

/**
 * @brief Set the opcode field in a machine word
 * @param word Pointer to the machine word
 * @param opcode The opcode value (0-15)
 */
void word_set_opcode(machine_word_t *word, opcode_t opcode);

/**
 * @brief Set the function code field in a machine word
 * @param word Pointer to the machine word
 * @param funct The function code value (0-15)
 */
void word_set_funct(machine_word_t *word, funct_t funct);

/**
 * @brief Set the source addressing method field in a machine word
 * @param word Pointer to the machine word
 * @param addr The addressing method (0-3)
 */
void word_set_src_addr(machine_word_t *word, addressing_method_t addr);

/**
 * @brief Set the destination addressing method field in a machine word
 * @param word Pointer to the machine word
 * @param addr The addressing method (0-3)
 */
void word_set_dst_addr(machine_word_t *word, addressing_method_t addr);

/**
 * @brief Set the source register field in a machine word
 * @param word Pointer to the machine word
 * @param reg The register number (0-7)
 */
void word_set_src_reg(machine_word_t *word, int reg);

/**
 * @brief Set the destination register field in a machine word
 * @param word Pointer to the machine word
 * @param reg The register number (0-7)
 */
void word_set_dst_reg(machine_word_t *word, int reg);

/**
 * @brief Set the ARE bits of a machine word
 * @param word Pointer to the machine word
 * @param are The ARE value
 */
void word_set_are(machine_word_t *word, are_type_t are);

/**
 * @brief Get a string representation of a machine word for debugging
 * @param word The machine word
 * @param buffer The buffer to store the string
 * @param size The size of the buffer
 */
void word_to_string(const machine_word_t *word, char *buffer, size_t size);

/**
 * @brief Encode the first word of an instruction
 * @param opcode The operation code
 * @param src_addr The source addressing method
 * @param src_reg The source register number (if applicable)
 * @param dst_addr The destination addressing method
 * @param dst_reg The destination register number (if applicable)
 * @param funct The function code (if applicable)
 * @return The encoded first word
 */
machine_word_t encode_instruction_word(opcode_t opcode,
                                     addressing_method_t src_addr,
                                     int src_reg,
                                     addressing_method_t dst_addr,
                                     int dst_reg,
                                     funct_t funct);

/**
 * @brief Encode a register operand word
 * @param src_reg Source register (0-7, or -1 if not used)
 * @param dst_reg Destination register (0-7, or -1 if not used)
 * @return The encoded register word
 */
machine_word_t encode_register_word(int src_reg, int dst_reg);

/**
 * @brief Encode an immediate operand
 * @param value The immediate value
 * @return The encoded immediate operand word
 */
machine_word_t encode_immediate(int value);

/**
 * @brief Encode a direct address operand
 * @param address The memory address
 * @param is_external Flag indicating if the address is external
 * @return The encoded direct address operand word
 */
machine_word_t encode_direct_address(int address, bool is_external);

/**
 * @brief Encode a relative address operand
 * @param distance The relative distance
 * @return The encoded relative address operand word
 */
machine_word_t encode_relative_address(int distance);

#endif /* MACHINE_WORD_H */