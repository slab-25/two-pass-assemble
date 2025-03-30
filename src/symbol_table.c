/**
 * @file symbol_table.c
 * @brief Implementation of the symbol table management with simplified attribute support
 */

#include "../include/symbol_table.h"
#include "../include/utils.h"

/* Create a new symbol table */
symbol_table_t* create_symbol_table() {
    symbol_table_t *table = (symbol_table_t *)malloc(sizeof(symbol_table_t));
    if (table) {
        table->head = NULL;
    }
    return table;
}

/* Add a symbol to the table */
bool add_symbol(symbol_table_t *table, const char *name, int value, symbol_attr_t attributes) {
    symbol_t *symbol;

    /* Check if the table is valid */
    if (!table) {
        return false;
    }

    /* Check if the symbol already exists */
    if (find_symbol(table, name)) {
        return false;
    }

    /* Create a new symbol */
    symbol = (symbol_t *)malloc(sizeof(symbol_t));
    if (!symbol) {
        return false;
    }

    /* Initialize the symbol */
    strcpy(symbol->name, name);
    symbol->value = value;
    symbol->attributes = attributes;

    /* Add the symbol to the table */
    symbol->next = table->head;
    table->head = symbol;

    return true;
}

/* Find a symbol by name */
symbol_t* find_symbol(symbol_table_t *table, const char *name) {
    symbol_t *current;

    /* Check if the table is valid */
    if (!table) {
        return NULL;
    }

    /* Search for the symbol */
    current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

/* Update a symbol's value */
bool update_symbol_value(symbol_table_t *table, const char *name, int value) {
    symbol_t *symbol;

    /* Find the symbol */
    symbol = find_symbol(table, name);
    if (!symbol) {
        return false;
    }

    /* Update the value */
    symbol->value = value;

    return true;
}

/* Add attributes to a symbol */
bool add_symbol_attributes(symbol_table_t *table, const char *name, symbol_attr_t attributes) {
    symbol_t *symbol;

    /* Find the symbol */
    symbol = find_symbol(table, name);
    if (!symbol) {
        return false;
    }

    /* Check for conflicting attributes */
    if ((attributes & SYMBOL_ATTR_ENTRY) && (symbol->attributes & SYMBOL_ATTR_EXTERNAL)) {
        return false; /* Cannot be both ENTRY and EXTERNAL */
    }

    /* Add the attributes */
    symbol->attributes |= attributes;

    return true;
}

/* Check if a symbol has a specific attribute */
bool symbol_has_attribute(const symbol_t *symbol, symbol_attr_t attribute) {
    if (!symbol) {
        return false;
    }
    return (symbol->attributes & attribute) != 0;
}

/* Get a string representation of symbol attributes */
void symbol_get_attr_string(const symbol_t *symbol, char *buffer, size_t size) {
    if (!symbol || !buffer || size == 0) {
        return;
    }

    buffer[0] = '\0';

    if (symbol->attributes & SYMBOL_ATTR_CODE) {
        strncat(buffer, "CODE ", size - strlen(buffer) - 1);
    }
    if (symbol->attributes & SYMBOL_ATTR_DATA) {
        strncat(buffer, "DATA ", size - strlen(buffer) - 1);
    }
    if (symbol->attributes & SYMBOL_ATTR_EXTERNAL) {
        strncat(buffer, "EXTERNAL ", size - strlen(buffer) - 1);
    }
    if (symbol->attributes & SYMBOL_ATTR_ENTRY) {
        strncat(buffer, "ENTRY ", size - strlen(buffer) - 1);
    }

    /* Remove trailing space if there is one */
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == ' ') {
        buffer[len-1] = '\0';
    }

    /* If no attributes were set */
    if (buffer[0] == '\0') {
        strncpy(buffer, "NONE", size - 1);
        buffer[size-1] = '\0';
    }
}

/* Update all data symbols by adding an offset */
void update_data_symbols(symbol_table_t *table, int offset) {
    symbol_t *current;

    /* Check if the table is valid */
    if (!table) {
        return;
    }

    /* Update all data symbols */
    current = table->head;
    while (current) {
        if (current->attributes & SYMBOL_ATTR_DATA) {
            current->value += offset;
        }
        current = current->next;
    }
}

/* Print the symbol table (for debugging) */
void print_symbol_table(symbol_table_t *table) {
    symbol_t *current;
    char attr_str[100];

    /* Check if the table is valid */
    if (!table) {
        printf("Symbol table is NULL\n");
        return;
    }

    /* Print the table header */
    printf("Symbol Table:\n");
    printf("Name                Value     Attributes\n");
    printf("-----------------------------------------\n");

    /* Print all symbols */
    current = table->head;
    while (current) {
        symbol_get_attr_string(current, attr_str, sizeof(attr_str));
        printf("%-20s %-8d %s\n", current->name, current->value, attr_str);
        current = current->next;
    }
}

/* Free the symbol table and all its entries */
void free_symbol_table(symbol_table_t *table) {
    symbol_t *current, *next;

    /* Check if the table is valid */
    if (!table) {
        return;
    }

    /* Free all symbols */
    current = table->head;
    while (current) {
        next = current->next;
        free(current);
        current = next;
    }

    /* Free the table */
    free(table);
}