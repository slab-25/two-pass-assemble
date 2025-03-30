/**
 * @file assembler.h
 * @brief Common definitions for the two-pass assembler
 */

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>

/* Constants */
#define MAX_LINE_LENGTH 81       /* 80 chars + terminating null */
#define MAX_LABEL_LENGTH 32      /* 31 chars + terminating null */
#define MAX_OPCODE_LENGTH 5      /* Maximum operation code length */
#define MAX_OPERAND_LENGTH 32    /* Maximum operand string length */
#define MAX_OPERANDS 2           /* Maximum operands per instruction */
#define MAX_FILENAME_LENGTH 256  /* Maximum filename length */
#define MAX_INSTRUCTION_WORDS 3  /* Maximum words per instruction */

/* Memory model */
#define MEMORY_START 100         /* Starting address for code section */
#define WORD_SIZE 24             /* 24-bit machine words */

/* Instruction types */
typedef enum {
    INST_TYPE_INVALID,
    INST_TYPE_DATA,     /* .data directive */
    INST_TYPE_STRING,   /* .string directive */
    INST_TYPE_ENTRY,    /* .entry directive */
    INST_TYPE_EXTERN,   /* .extern directive */
    INST_TYPE_CODE      /* Machine instruction */
} instruction_type_t;

/* Symbol attributes */
typedef enum {
    SYMBOL_CODE,       /* Label for code section */
    SYMBOL_DATA,       /* Label for data section */
    SYMBOL_EXTERNAL,   /* External symbol */
    SYMBOL_ENTRY       /* Entry point */
} symbol_type_t;

/* Addressing methods */
typedef enum {
    ADDR_IMMEDIATE = 0,      /* #value */
    ADDR_DIRECT = 1,         /* label */
    ADDR_RELATIVE = 2,       /* &label */
    ADDR_REGISTER = 3        /* register */
} addressing_method_t;

/* A/R/E bits */
#define ARE_ABSOLUTE    4     /* A bit = 1, R and E = 0 (binary 100) */
#define ARE_RELOCATABLE 2     /* R bit = 1, A and E = 0 (binary 010) */
#define ARE_EXTERNAL    1     /* E bit = 1, A and R = 0 (binary 001) */

/* Opcode and funct values as defined in the specification */
typedef enum {
    OP_MOV = 0,
    OP_CMP = 1,
    OP_ADD = 2,
    OP_SUB = 2,
    OP_LEA = 4,
    OP_CLR = 5,
    OP_NOT = 5,
    OP_INC = 5,
    OP_DEC = 5,
    OP_JMP = 9,
    OP_BNE = 9,
    OP_JSR = 9,
    OP_RED = 12,
    OP_PRN = 13,
    OP_RTS = 14,
    OP_STOP = 15
} opcode_t;

/* Function values for opcodes that share codes */
typedef enum {
    FUNCT_NONE = 0,
    FUNCT_ADD = 1,
    FUNCT_SUB = 2,
    FUNCT_CLR = 1,
    FUNCT_NOT = 2,
    FUNCT_INC = 3,
    FUNCT_DEC = 4,
    FUNCT_JMP = 1,
    FUNCT_BNE = 2,
    FUNCT_JSR = 3
} funct_t;

/* File extensions */
#define EXT_SOURCE ".as"      /* Source file extension */
#define EXT_MACRO ".am"       /* After macro expansion extension */
#define EXT_OBJECT ".ob"      /* Object file extension */
#define EXT_ENTRY ".ent"      /* Entry points file extension */
#define EXT_EXTERN ".ext"     /* External references file extension */

/* Common error handling macro */
#define REPORT_ERROR(filename, line, format, ...) \
    fprintf(stderr, "Error in %s, line %d: " format "\n", filename, line, ##__VA_ARGS__)

#endif /* ASSEMBLER_H */