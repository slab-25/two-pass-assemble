/**
 * @file symbol_table.h
 * @brief Symbol table management for the assembler with simplified attribute support
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "assembler.h"

/**
 * @brief Symbol attributes using bit flags for more flexibility
 */
typedef enum {
    SYMBOL_ATTR_NONE     = 0x00,  /* No attributes */
    SYMBOL_ATTR_CODE     = 0x01,  /* Symbol is in code section */
    SYMBOL_ATTR_DATA     = 0x02,  /* Symbol is in data section */
    SYMBOL_ATTR_EXTERNAL = 0x04,  /* Symbol is defined externally */
    SYMBOL_ATTR_ENTRY    = 0x08   /* Symbol is an entry point */
} symbol_attr_t;

/**
 * @brief Symbol table entry structure
 */
typedef struct symbol {
    char name[MAX_LABEL_LENGTH];  /* Symbol name */
    int value;                    /* Memory address */
    symbol_attr_t attributes;     /* Symbol attributes (bit flags) */
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
 * @param attributes The attributes of the symbol
 * @return true if the symbol was added successfully, false otherwise
 */
bool add_symbol(symbol_table_t *table, const char *name, int value, symbol_attr_t attributes);

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
 * @brief Add attributes to a symbol
 * @param table The symbol table
 * @param name The name of the symbol to update
 * @param attributes The attributes to add (can be multiple with OR)
 * @return true if the attributes were added successfully, false otherwise
 */
bool add_symbol_attributes(symbol_table_t *table, const char *name, symbol_attr_t attributes);

/**
 * @brief Check if a symbol has a specific attribute
 * @param symbol The symbol to check
 * @param attribute The attribute to check for
 * @return true if the symbol has the attribute, false otherwise
 */
bool symbol_has_attribute(const symbol_t *symbol, symbol_attr_t attribute);

/**
 * @brief Get a string representation of symbol attributes
 * @param symbol The symbol
 * @param buffer The buffer to store the string
 * @param size The size of the buffer
 */
void symbol_get_attr_string(const symbol_t *symbol, char *buffer, size_t size);

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

/* Convenience macros for working with attributes */
#define SYMBOL_IS_CODE(symbol) symbol_has_attribute((symbol), SYMBOL_ATTR_CODE)
#define SYMBOL_IS_DATA(symbol) symbol_has_attribute((symbol), SYMBOL_ATTR_DATA)
#define SYMBOL_IS_EXTERNAL(symbol) symbol_has_attribute((symbol), SYMBOL_ATTR_EXTERNAL)
#define SYMBOL_IS_ENTRY(symbol) symbol_has_attribute((symbol), SYMBOL_ATTR_ENTRY)

#endif /* SYMBOL_TABLE_H */