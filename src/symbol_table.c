/**
 * @file symbol_table.c
 * @brief Implementation of the symbol table management
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
bool add_symbol(symbol_table_t *table, const char *name, int value, symbol_type_t type) {
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
    symbol->type = type;

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

/* Add an attribute to a symbol */
bool add_symbol_attribute(symbol_table_t *table, const char *name, symbol_type_t attr) {
    symbol_t *symbol;

    /* Find the symbol */
    symbol = find_symbol(table, name);
    if (!symbol) {
        return false;
    }

    /* Add the attribute */
    if (attr == SYMBOL_ENTRY) {
        /* Only add ENTRY attribute if the symbol is internal (not EXTERNAL) */
        if (symbol->type == SYMBOL_EXTERNAL) {
            return false;
        }
        symbol->type = SYMBOL_ENTRY;
    }

    return true;
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
        if (current->type == SYMBOL_DATA) {
            current->value += offset;
        }
        current = current->next;
    }
}

/* Print the symbol table (for debugging) */
void print_symbol_table(symbol_table_t *table) {
    symbol_t *current;

    /* Check if the table is valid */
    if (!table) {
        printf("Symbol table is NULL\n");
        return;
    }

    /* Print the table header */
    printf("Symbol Table:\n");
    printf("Name                Value     Type\n");
    printf("-----------------------------------------\n");

    /* Print all symbols */
    current = table->head;
    while (current) {
        printf("%-20s %-8d ", current->name, current->value);

        /* Print the type */
        switch (current->type) {
            case SYMBOL_CODE:
                printf("CODE\n");
                break;
            case SYMBOL_DATA:
                printf("DATA\n");
                break;
            case SYMBOL_EXTERNAL:
                printf("EXTERNAL\n");
                break;
            case SYMBOL_ENTRY:
                printf("ENTRY\n");
                break;
            default:
                printf("UNKNOWN\n");
        }

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