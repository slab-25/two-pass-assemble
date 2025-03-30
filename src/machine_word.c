/**
 * @file machine_word.c
 * @brief Implementation of machine word operations
 */

#include <stdio.h>
#include <string.h>
#include "../include/machine_word.h"

/* Bit field positions in 24-bit machine word */
#define OPCODE_SHIFT 18
#define OPCODE_MASK 0x0F

#define SRC_ADDR_SHIFT 16
#define SRC_ADDR_MASK 0x03

#define SRC_REG_SHIFT 13
#define SRC_REG_MASK 0x07

#define DST_ADDR_SHIFT 11
#define DST_ADDR_MASK 0x03

#define DST_REG_SHIFT 8
#define DST_REG_MASK 0x07

#define FUNCT_SHIFT 3
#define FUNCT_MASK 0x0F

#define ARE_MASK 0x07

/* Initialize a machine word */
void word_init(machine_word_t *word, unsigned int value, are_type_t are) {
    if (!word) {
        return;
    }

    word->value = value & 0x1FFFFF; /* 21 bits */
    word->are = are & ARE_MASK;     /* 3 bits */
}

/* Set the opcode field in a machine word */
void word_set_opcode(machine_word_t *word, opcode_t opcode) {
    if (!word) {
        return;
    }

    /* Clear the opcode field and set the new value */
    word->value &= ~(OPCODE_MASK << OPCODE_SHIFT);
    word->value |= ((opcode & OPCODE_MASK) << OPCODE_SHIFT);
}

/* Set the function code field in a machine word */
void word_set_funct(machine_word_t *word, funct_t funct) {
    if (!word) {
        return;
    }

    /* Clear the funct field and set the new value */
    word->value &= ~(FUNCT_MASK << FUNCT_SHIFT);
    word->value |= ((funct & FUNCT_MASK) << FUNCT_SHIFT);
}

/* Set the source addressing method field in a machine word */
void word_set_src_addr(machine_word_t *word, addressing_method_t addr) {
    if (!word) {
        return;
    }

    /* Clear the source addressing field and set the new value */
    word->value &= ~(SRC_ADDR_MASK << SRC_ADDR_SHIFT);
    word->value |= ((addr & SRC_ADDR_MASK) << SRC_ADDR_SHIFT);
}

/* Set the destination addressing method field in a machine word */
void word_set_dst_addr(machine_word_t *word, addressing_method_t addr) {
    if (!word) {
        return;
    }

    /* Clear the destination addressing field and set the new value */
    word->value &= ~(DST_ADDR_MASK << DST_ADDR_SHIFT);
    word->value |= ((addr & DST_ADDR_MASK) << DST_ADDR_SHIFT);
}

/* Set the source register field in a machine word */
void word_set_src_reg(machine_word_t *word, int reg) {
    if (!word) {
        return;
    }

    /* Clear the source register field and set the new value */
    word->value &= ~(SRC_REG_MASK << SRC_REG_SHIFT);
    word->value |= ((reg & SRC_REG_MASK) << SRC_REG_SHIFT);
}

/* Set the destination register field in a machine word */
void word_set_dst_reg(machine_word_t *word, int reg) {
    if (!word) {
        return;
    }

    /* Clear the destination register field and set the new value */
    word->value &= ~(DST_REG_MASK << DST_REG_SHIFT);
    word->value |= ((reg & DST_REG_MASK) << DST_REG_SHIFT);
}

/* Set the ARE bits of a machine word */
void word_set_are(machine_word_t *word, are_type_t are) {
    if (!word) {
        return;
    }

    word->are = are & ARE_MASK;
}

/* Get a string representation of a machine word for debugging */
void word_to_string(const machine_word_t *word, char *buffer, size_t size) {
    char are_str[4];

    if (!word || !buffer || size == 0) {
        return;
    }

    /* Create ARE string representation first */
    are_str[0] = (word->are & WORD_ARE_ABSOLUTE) ? 'A' : '-';
    are_str[1] = (word->are & WORD_ARE_RELOCATABLE) ? 'R' : '-';
    are_str[2] = (word->are & WORD_ARE_EXTERNAL) ? 'E' : '-';
    are_str[3] = '\0';

    /* Format using sprintf (C90 compliant) */
    sprintf(buffer, "Value: 0x%06X, ARE: %s",
            (word->value << 3) | word->are, are_str);
}

/* Encode the first word of an instruction */
machine_word_t encode_instruction_word(opcode_t opcode,
                                     addressing_method_t src_addr,
                                     int src_reg,
                                     addressing_method_t dst_addr,
                                     int dst_reg,
                                     funct_t funct) {
    machine_word_t word;

    /* Initialize with zero value and absolute ARE */
    word_init(&word, 0, WORD_ARE_ABSOLUTE);

    /* Set the fields */
    word_set_opcode(&word, opcode);
    word_set_src_addr(&word, src_addr);
    word_set_src_reg(&word, src_reg);
    word_set_dst_addr(&word, dst_addr);
    word_set_dst_reg(&word, dst_reg);
    word_set_funct(&word, funct);

    return word;
}

/* Encode a register operand word */
machine_word_t encode_register_word(int src_reg, int dst_reg) {
    machine_word_t word;

    /* Initialize with zero value and absolute ARE */
    word_init(&word, 0, WORD_ARE_ABSOLUTE);

    /* Set register fields */
    if (src_reg >= 0) {
        word_set_src_reg(&word, src_reg);
    }

    if (dst_reg >= 0) {
        word_set_dst_reg(&word, dst_reg);
    }

    return word;
}

/* Encode an immediate operand */
machine_word_t encode_immediate(int value) {
    machine_word_t word;

    /* Initialize with the immediate value and absolute ARE */
    word_init(&word, value & 0x1FFFFF, WORD_ARE_ABSOLUTE);

    return word;
}

/* Encode a direct address operand */
machine_word_t encode_direct_address(int address, bool is_external) {
    machine_word_t word;

    /* Initialize with the address value and appropriate ARE */
    word_init(&word, address & 0x1FFFFF,
             is_external ? WORD_ARE_EXTERNAL : WORD_ARE_RELOCATABLE);

    return word;
}

/* Encode a relative address operand */
machine_word_t encode_relative_address(int distance) {
    machine_word_t word;

    /* Initialize with the distance value and relocatable ARE */
    word_init(&word, distance & 0x1FFFFF, WORD_ARE_RELOCATABLE);

    return word;
}