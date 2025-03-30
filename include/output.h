/**
* @file output.h
 * @brief Output file generation for the assembler
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include "assembler.h"
#include "symbol_table.h"
#include "second_pass.h"
#include "error.h"

/**
 * @brief Output files structure
 */
typedef struct
{
    FILE* ob; /* Object file (.ob) */
    FILE* ent; /* Entries file (.ent) */
    FILE* ext; /* Externals file (.ext) */
} output_files_t;

/**
 * @brief Generate the output files
 * @param filename The base filename
 * @param symbols The symbol table
 * @param code_image The code image
 * @param data_image The data image
 * @param ext_refs The list of external references
 * @param ICF The final instruction counter
 * @param DCF The final data counter
 * @param context Error context for reporting issues
 * @return true if generation was successful, false otherwise
 */
bool generate_output_files(const char* filename, symbol_table_t* symbols,
                           machine_word_t* code_image, machine_word_t* data_image,
                           external_reference_t* ext_refs, int ICF, int DCF,
                           error_context_t* context);

/**
 * @brief Write the object file
 * @param filename The base filename
 * @param code_image The code image
 * @param data_image The data image
 * @param ICF The final instruction counter
 * @param DCF The final data counter
 * @param context Error context for reporting issues
 * @return true if writing was successful, false otherwise
 */
bool write_object_file(const char* filename, machine_word_t* code_image,
                       machine_word_t* data_image, int ICF, int DCF,
                       error_context_t* context);

/**
 * @brief Write the entries file
 * @param filename The base filename
 * @param symbols The symbol table
 * @param context Error context for reporting issues
 * @return true if writing was successful, false otherwise
 */
bool write_entries_file(const char* filename, symbol_table_t* symbols,
                       error_context_t* context);

/**
 * @brief Write the externals file
 * @param filename The base filename
 * @param ext_refs The list of external references
 * @param context Error context for reporting issues
 * @return true if writing was successful, false otherwise
 */
bool write_externals_file(const char* filename, external_reference_t* ext_refs,
                         error_context_t* context);

/**
 * @brief Check if symbol table has entries
 * @param symbols The symbol table
 * @return true if the symbol table has entries, false otherwise
 */
bool has_entries(symbol_table_t *symbols);

#endif /* OUTPUT_H */