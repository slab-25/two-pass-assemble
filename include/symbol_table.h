/**
 * @file symbol_table.h
 * @brief Symbol table management for the assembler
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "assembler.h"

/**
 * @brief Symbol table entry structure
 */
typedef struct symbol {
    char name[MAX_LABEL_LENGTH];  /* Symbol name */
    int value;                    /* Memory address */
    symbol_type_t type;           /* Symbol type (CODE, DATA, EXTERNAL, ENTRY) */
    struct symbol *next;          /* Pointer to the next symbol in the list */
} symbol_t;

/**
 * @brief Symbol table structure
 */
typedef struct symbol_table {
    symbol_t *head;               /* Pointer to the first symbol in the list */
} symbol_table_t;

/**
 * @brief Create a new symbol table
 * @return Pointer to the newly created symbol table
 */
symbol_table_t* create_symbol_table();

/**
 * @brief Add a symbol to the table
 * @param table The symbol table
 * @param name The name of the symbol
 * @param value The value (address) of the symbol
 * @param type The type of the symbol
 * @return true if the symbol was added successfully, false otherwise
 */
bool add_symbol(symbol_table_t *table, const char *name, int value, symbol_type_t type);

/**
 * @brief Find a symbol by name
 * @param table The symbol table
 * @param name The name of the symbol to find
 * @return Pointer to the symbol if found, NULL otherwise
 */
symbol_t* find_symbol(symbol_table_t *table, const char *name);

/**
 * @brief Update a symbol's value
 * @param table The symbol table
 * @param name The name of the symbol to update
 * @param value The new value for the symbol
 * @return true if the symbol was updated successfully, false otherwise
 */
bool update_symbol_value(symbol_table_t *table, const char *name, int value);

/**
 * @brief Add an attribute to a symbol
 * @param table The symbol table
 * @param name The name of the symbol to update
 * @param attr The attribute to add (e.g., SYMBOL_ENTRY)
 * @return true if the attribute was added successfully, false otherwise
 */
bool add_symbol_attribute(symbol_table_t *table, const char *name, symbol_type_t attr);

/**
 * @brief Update all data symbols by adding an offset
 * @param table The symbol table
 * @param offset The offset to add to data symbols
 */
void update_data_symbols(symbol_table_t *table, int offset);

/**
 * @brief Print the symbol table (for debugging)
 * @param table The symbol table
 */
void print_symbol_table(symbol_table_t *table);

/**
 * @brief Free the symbol table and all its entries
 * @param table The symbol table to free
 */
void free_symbol_table(symbol_table_t *table);

#endif /* SYMBOL_TABLE_H */