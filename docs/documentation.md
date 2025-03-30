# Assembler API Documentation

## Table of Contents

1. [Core Components](#core-components)
2. [Symbol Table Management](#symbol-table-management)
3. [Pre-Assembler](#pre-assembler)
4. [First Pass](#first-pass)
5. [Second Pass](#second-pass)
6. [Machine Word Handling](#machine-word-handling)
7. [Output Generation](#output-generation)
8. [Utility Functions](#utility-functions)
9. [Error Handling](#error-handling)

## Core Components

### Constants

```c
#define MAX_LINE_LENGTH 81       /* 80 chars + terminating null */
#define MAX_LABEL_LENGTH 32      /* 31 chars + terminating null */
#define MAX_OPCODE_LENGTH 5      /* Maximum operation code length */
#define MAX_OPERAND_LENGTH 32    /* Maximum operand string length */
#define MAX_OPERANDS 2           /* Maximum operands per instruction */
#define MAX_FILENAME_LENGTH 256  /* Maximum filename length */
#define MAX_INSTRUCTION_WORDS 3  /* Maximum words per instruction */

#define MEMORY_START 100         /* Starting address for code section */
#define WORD_SIZE 24             /* 24-bit machine words */
```

### Enumerations

#### Instruction Types

```c
typedef enum {
    INST_TYPE_INVALID,
    INST_TYPE_DATA,     /* .data directive */
    INST_TYPE_STRING,   /* .string directive */
    INST_TYPE_ENTRY,    /* .entry directive */
    INST_TYPE_EXTERN,   /* .extern directive */
    INST_TYPE_CODE      /* Machine instruction */
} instruction_type_t;
```

#### Addressing Methods

```c
typedef enum {
    ADDR_IMMEDIATE = 0,      /* #value */
    ADDR_DIRECT = 1,         /* label */
    ADDR_RELATIVE = 2,       /* &label */
    ADDR_REGISTER = 3        /* register */
} addressing_method_t;
```

## Symbol Table Management

### Data Structures

```c
typedef enum {
    SYMBOL_ATTR_NONE     = 0x00,  /* No attributes */
    SYMBOL_ATTR_CODE     = 0x01,  /* Symbol is in code section */
    SYMBOL_ATTR_DATA     = 0x02,  /* Symbol is in data section */
    SYMBOL_ATTR_EXTERNAL = 0x04,  /* Symbol is defined externally */
    SYMBOL_ATTR_ENTRY    = 0x08   /* Symbol is an entry point */
} symbol_attr_t;

typedef struct symbol {
    char name[MAX_LABEL_LENGTH];  /* Symbol name */
    int value;                    /* Memory address */
    symbol_attr_t attributes;     /* Symbol attributes (bit flags) */
    struct symbol *next;          /* Pointer to the next symbol in the list */
} symbol_t;

typedef struct symbol_table {
    symbol_t *head;               /* Pointer to the first symbol in the list */
} symbol_table_t;
```

### Functions

#### `symbol_table_t* create_symbol_table()`

- **Description**: Create a new symbol table
- **Returns**: Pointer to the newly created symbol table

#### `bool add_symbol(symbol_table_t *table, const char *name, int value, symbol_attr_t attributes)`

- **Description**: Add a symbol to the table
- **Parameters**:
    - `table`: The symbol table
    - `name`: The name of the symbol
    - `value`: The value (address) of the symbol
    - `attributes`: The attributes of the symbol
- **Returns**: true if the symbol was added successfully, false otherwise

#### `symbol_t* find_symbol(symbol_table_t *table, const char *name)`

- **Description**: Find a symbol by name
- **Parameters**:
    - `table`: The symbol table
    - `name`: The name of the symbol to find
- **Returns**: Pointer to the symbol if found, NULL otherwise

#### `bool update_symbol_value(symbol_table_t *table, const char *name, int value)`

- **Description**: Update a symbol's value
- **Parameters**:
    - `table`: The symbol table
    - `name`: The name of the symbol to update
    - `value`: The new value for the symbol
- **Returns**: true if the symbol was updated successfully, false otherwise

## Pre-Assembler

### Data Structures

```c
typedef struct macro {
    char name[MAX_LABEL_LENGTH];  /* Macro name */
    char **lines;                 /* Array of lines in the macro */
    int line_count;               /* Number of lines in the macro */
    struct macro *next;           /* Pointer to the next macro in the list */
} macro_t;

typedef struct macro_table {
    macro_t *head;                /* Pointer to the first macro in the list */
} macro_table_t;
```

### Functions

#### `macro_table_t* create_macro_table()`

- **Description**: Create a new macro table
- **Returns**: Pointer to the newly created macro table

#### `bool add_macro(macro_table_t *table, const char *name, error_context_t *context)`

- **Description**: Add a new macro to the table
- **Parameters**:
    - `table`: The macro table
    - `name`: The name of the macro
    - `context`: Error context for reporting issues
- **Returns**: true if the macro was added successfully, false otherwise

#### `bool process_file(const char *filename, error_context_t *context)`

- **Description**: Process a source file to expand macros
- **Parameters**:
    - `filename`: The name of the source file
    - `context`: Error context for reporting issues
- **Returns**: true if processing was successful, false otherwise

## First Pass

### Data Structures

```c
typedef struct {
    instruction_type_t type;
    char label[MAX_LABEL_LENGTH];
    char opcode[MAX_OPCODE_LENGTH];
    char operands[MAX_OPERANDS][MAX_OPERAND_LENGTH];
    int operand_count;
    int line_number;
} parsed_line_t;
```

### Functions

#### `bool parse_line(const char *line, parsed_line_t *parsed, int line_number, error_context_t *context)`

- **Description**: Parse a line into its components
- **Parameters**:
    - `line`: The line to parse
    - `parsed`: Output parameter for the parsed line
    - `line_number`: The line number
    - `context`: Error context for reporting issues
- **Returns**: true if parsing was successful, false otherwise

#### `bool first_pass(const char *filename, symbol_table_t *symbols, error_context_t *context)`

- **Description**: Main function for the first pass
- **Parameters**:
    - `filename`: The name of the source file
    - `symbols`: The symbol table
    - `context`: Error context for reporting issues
- **Returns**: true if the first pass was successful, false otherwise

## Second Pass

### Data Structures

```c
typedef struct {
    machine_word_t words[MAX_INSTRUCTION_WORDS];
    int word_count;
} instruction_code_t;

typedef struct external_reference {
    char name[MAX_LABEL_LENGTH];
    int address;
    struct external_reference *next;
} external_reference_t;
```

### Functions

####

`bool encode_instruction(parsed_line_t *line, symbol_table_t *symbols, instruction_code_t *code, int current_address, external_reference_t **ext_refs, error_context_t *context)`

- **Description**: Encode a machine instruction
- **Parameters**:
    - `line`: The parsed line
    - `symbols`: The symbol table
    - `code`: Output parameter for the encoded instruction
    - `current_address`: The current instruction address
    - `ext_refs`: Output parameter for external references
    - `context`: Error context for reporting issues
- **Returns**: true if encoding was successful, false otherwise

####

`bool second_pass(const char *filename, symbol_table_t *symbols, machine_word_t **code_image, machine_word_t **data_image, external_reference_t **ext_refs, int *ICF, int *DCF, error_context_t *context)`

- **Description**: Main function for the second pass
- **Parameters**:
    - `filename`: The name of the source file
    - `symbols`: The symbol table
    - `code_image`: Output parameter for the code image
    - `data_image`: Output parameter for the data image
    - `ext_refs`: Output parameter for external references
    - `ICF`: Output parameter for the final instruction counter
    - `DCF`: Output parameter for the final data counter
    - `context`: Error context for reporting issues
- **Returns**: true if the second pass was successful, false otherwise

## Machine Word Handling

### Data Structures

```c
typedef struct {
    unsigned int value: 21;  /* 21-bit value */
    unsigned int are: 3;     /* A=1 if absolute, R=1 if relocatable, E=1 if external */
} machine_word_t;

typedef enum {
    WORD_ARE_NONE = 0,           /* No bits set */
    WORD_ARE_EXTERNAL = 1,       /* E bit = 1 (binary 001) */
    WORD_ARE_RELOCATABLE = 2,    /* R bit = 1 (binary 010) */
    WORD_ARE_ABSOLUTE = 4        /* A bit = 1 (binary 100) */
} are_type_t;
```

### Functions

#### `void word_init(machine_word_t *word, unsigned int value, are_type_t are)`

- **Description**: Initialize a machine word
- **Parameters**:
    - `word`: Pointer to the machine word to initialize
    - `value`: The initial value
    - `are`: The ARE bits

####

`machine_word_t encode_instruction_word(opcode_t opcode, addressing_method_t src_addr, int src_reg, addressing_method_t dst_addr, int dst_reg, funct_t funct)`

- **Description**: Encode the first word of an instruction
- **Parameters**:
    - `opcode`: The operation code
    - `src_addr`: The source addressing method
    - `src_reg`: The source register number (if applicable)
    - `dst_addr`: The destination addressing method
    - `dst_reg`: The destination register number (if applicable)
    - `funct`: The function code (if applicable)
- **Returns**: The encoded first word

## Output Generation

### Data Structures

```c
typedef struct {
    FILE* ob; /* Object file (.ob) */
    FILE* ent; /* Entries file (.ent) */
    FILE* ext; /* Externals file (.ext) */
} output_files_t;
```

### Functions

####

`bool generate_output_files(const char *filename, symbol_table_t *symbols, machine_word_t *code_image, machine_word_t *data_image, external_reference_t *ext_refs, int ICF, int DCF, error_context_t *context)`

- **Description**: Generate the output files
- **Parameters**:
    - `filename`: The base filename
    - `symbols`: The symbol table
    - `code_image`: The code image
    - `data_image`: The data image
    - `ext_refs`: The list of external references
    - `ICF`: The final instruction counter
    - `DCF`: The final data counter
    - `context`: Error context for reporting issues
- **Returns**: true if generation was successful, false otherwise

## Error Handling

### Data Structures

```c
typedef struct {
    bool had_error;              /* Flag indicating if an error occurred */
    char filename[MAX_FILENAME_LENGTH]; /* Current filename being processed */
    int line_number;             /* Current line number being processed */
} error_context_t;
```

### Functions

#### `void init_error_context(error_context_t *context, const char *filename)`

- **Description**: Initialize an error context
- **Parameters**:
    - `context`: The error context to initialize
    - `filename`: The current filename

#### `void report_context_error(error_context_t *context, const char *format, ...)`

- **Description**: Report an error with the current context
- **Parameters**:
    - `context`: The error context
    - `format`: The error message format
    - `...`: Additional arguments for the format string

## Utility Functions

### Functions

#### `char* trim(char *str)`

- **Description**: Trim whitespace from the beginning and end of a string
- **Parameters**:
    - `str`: The string to trim (modified in-place)
- **Returns**: Pointer to the trimmed string (same as input)

#### `bool is_valid_label(const char *label)`

- **Description**: Check if a string is a valid label name
- **Parameters**:
    - `label`: The label name to check
- **Returns**: true if valid, false otherwise

#### `bool is_register(const char *str)`

- **Description**: Check if a string is a register name (r0-r7)
- **Parameters**:
    - `str`: The string to check
- **Returns**: true if it's a register, false otherwise

#### `int get_register_number(const char *reg_str)`

- **Description**: Get the register number from a register name
- **Parameters**:
    - `reg_str`: The register name (e.g., "r3")
- **Returns**: The register number (0-7) or -1 if invalid

# Assembler API Documentation (Continued)

## Utility Functions (Continued)

#### `bool is_integer(const char *str)`

- **Description**: Check if a string is a valid integer
- **Parameters**:
    - `str`: The string to check
- **Returns**: true if it's a valid integer, false otherwise

#### `int string_to_int(const char *str)`

- **Description**: Convert a string to an integer
- **Parameters**:
    - `str`: The string to convert
- **Returns**: The integer value

#### `char* str_duplicate(const char *str)`

- **Description**: Create a copy of a string
- **Parameters**:
    - `str`: The string to copy
- **Returns**: A new string that must be freed by the caller

#### `bool is_reserved_word(const char *str)`

- **Description**: Check if a string is a reserved word (instruction or directive)
- **Parameters**:
    - `str`: The string to check
- **Returns**: true if it's a reserved word, false otherwise

#### `void get_base_filename(const char *filename, char *base)`

- **Description**: Get the extension-less base name from a filename
- **Parameters**:
    - `filename`: The filename
    - `base`: Output parameter for the base name

#### `void create_filename(const char *base, const char *extension, char *result)`

- **Description**: Create a filename with a specific extension
- **Parameters**:
    - `base`: The base filename (without extension)
    - `extension`: The extension to add
    - `result`: Output parameter for the result

## First Pass (Additional Functions)

#### `bool process_data_directive(parsed_line_t *line, symbol_table_t *symbols, int *DC, error_context_t *context)`

- **Description**: Process a .data directive
- **Parameters**:
    - `line`: The parsed line
    - `symbols`: The symbol table
    - `DC`: Pointer to the data counter
    - `context`: Error context for reporting issues
- **Returns**: true if processing was successful, false otherwise

#### `bool process_string_directive(parsed_line_t *line, symbol_table_t *symbols, int *DC, error_context_t *context)`

- **Description**: Process a .string directive
- **Parameters**:
    - `line`: The parsed line
    - `symbols`: The symbol table
    - `DC`: Pointer to the data counter
    - `context`: Error context for reporting issues
- **Returns**: true if processing was successful, false otherwise

#### `bool process_extern_directive(parsed_line_t *line, symbol_table_t *symbols, error_context_t *context)`

- **Description**: Process a .extern directive
- **Parameters**:
    - `line`: The parsed line
    - `symbols`: The symbol table
    - `context`: Error context for reporting issues
- **Returns**: true if processing was successful, false otherwise

#### `bool process_entry_directive(parsed_line_t *line, error_context_t *context)`

- **Description**: Process a .entry directive
- **Parameters**:
    - `line`: The parsed line
    - `context`: Error context for reporting issues
- **Returns**: true if processing was successful, false otherwise

#### `bool process_instruction(parsed_line_t *line, symbol_table_t *symbols, int *IC, error_context_t *context)`

- **Description**: Process a machine instruction
- **Parameters**:
    - `line`: The parsed line
    - `symbols`: The symbol table
    - `IC`: Pointer to the instruction counter
    - `context`: Error context for reporting issues
- **Returns**: true if processing was successful, false otherwise

####
`int calculate_instruction_length(const char *opcode, const char *operand1, const char *operand2, error_context_t *context)`

- **Description**: Calculate the instruction length (in words)
- **Parameters**:
    - `opcode`: The operation code
    - `operand1`: The first operand (can be NULL)
    - `operand2`: The second operand (can be NULL)
    - `context`: Error context for reporting issues
- **Returns**: The instruction length in words, or -1 if invalid

## Second Pass (Additional Functions)

#### `addressing_method_t get_addressing_method(const char *operand)`

- **Description**: Determine the addressing method for an operand
- **Parameters**:
    - `operand`: The operand string
- **Returns**: The addressing method

####
`bool encode_operand_word(machine_word_t *word, const char *operand, addressing_method_t addr_method, symbol_table_t *symbols, int current_address, int word_offset, external_reference_t **ext_refs, error_context_t *context)`

- **Description**: Encode an operand word based on its addressing method
- **Parameters**:
    - `word`: Output parameter for the encoded word
    - `operand`: The operand string
    - `addr_method`: The addressing method
    - `symbols`: The symbol table
    - `current_address`: The current instruction address
    - `word_offset`: Offset from current address for this word
    - `ext_refs`: Output parameter for external references
    - `context`: Error context for reporting issues
- **Returns**: true if encoding was successful, false otherwise

#### `bool process_entry_second_pass(parsed_line_t *line, symbol_table_t *symbols, error_context_t *context)`

- **Description**: Process an entry directive in the second pass
- **Parameters**:
    - `line`: The parsed line
    - `symbols`: The symbol table
    - `context`: Error context for reporting issues
- **Returns**: true if processing was successful, false otherwise

####
`bool add_external_reference(external_reference_t **ext_refs, const char *name, int address, error_context_t *context)`

- **Description**: Add an external reference
- **Parameters**:
    - `ext_refs`: Pointer to the list of external references
    - `name`: The name of the external symbol
    - `address`: The address where it's referenced
    - `context`: Error context for reporting issues
- **Returns**: true if the reference was added successfully, false otherwise

#### `void free_external_references(external_reference_t *ext_refs)`

- **Description**: Free the list of external references
- **Parameters**:
    - `ext_refs`: The list of external references

## Machine Word (Additional Functions)

#### `void word_set_opcode(machine_word_t *word, opcode_t opcode)`

- **Description**: Set the opcode field in a machine word
- **Parameters**:
    - `word`: Pointer to the machine word
    - `opcode`: The opcode value (0-15)

#### `void word_set_funct(machine_word_t *word, funct_t funct)`

- **Description**: Set the function code field in a machine word
- **Parameters**:
    - `word`: Pointer to the machine word
    - `funct`: The function code value (0-15)

#### `void word_set_src_addr(machine_word_t *word, addressing_method_t addr)`

- **Description**: Set the source addressing method field in a machine word
- **Parameters**:
    - `word`: Pointer to the machine word
    - `addr`: The addressing method (0-3)

#### `void word_set_dst_addr(machine_word_t *word, addressing_method_t addr)`

- **Description**: Set the destination addressing method field in a machine word
- **Parameters**:
    - `word`: Pointer to the machine word
    - `addr`: The addressing method (0-3)

#### `void word_set_are(machine_word_t *word, are_type_t are)`

- **Description**: Set the ARE bits of a machine word
- **Parameters**:
    - `word`: Pointer to the machine word
    - `are`: The ARE value

#### `void word_to_string(const machine_word_t *word, char *buffer, size_t size)`

- **Description**: Get a string representation of a machine word for debugging
- **Parameters**:
    - `word`: The machine word
    - `buffer`: The buffer to store the string
    - `size`: The size of the buffer

## Output Generation (Additional Functions)

####
`bool write_object_file(const char *filename, machine_word_t *code_image, machine_word_t *data_image, int ICF, int DCF, error_context_t *context)`

- **Description**: Write the object file
- **Parameters**:
    - `filename`: The base filename
    - `code_image`: The code image
    - `data_image`: The data image
    - `ICF`: The final instruction counter
    - `DCF`: The final data counter
    - `context`: Error context for reporting issues
- **Returns**: true if writing was successful, false otherwise

#### `bool write_entries_file(const char *filename, symbol_table_t *symbols, error_context_t *context)`

- **Description**: Write the entries file
- **Parameters**:
    - `filename`: The base filename
    - `symbols`: The symbol table
    - `context`: Error context for reporting issues
- **Returns**: true if writing was successful, false otherwise

#### `bool write_externals_file(const char *filename, external_reference_t *ext_refs, error_context_t *context)`

- **Description**: Write the externals file
- **Parameters**:
    - `filename`: The base filename
    - `ext_refs`: The list of external references
    - `context`: Error context for reporting issues
- **Returns**: true if writing was successful, false otherwise

#### `bool has_entries(symbol_table_t *symbols)`

- **Description**: Check if symbol table has entries
- **Parameters**:
    - `symbols`: The symbol table
- **Returns**: true if the symbol table has entries, false otherwise

## Pre-Assembler (Additional Functions)

#### `bool add_line_to_macro(macro_table_t *table, const char *line, error_context_t *context)`

- **Description**: Add a line to the current macro being defined
- **Parameters**:
    - `table`: The macro table
    - `line`: The line to add
    - `context`: Error context for reporting issues
- **Returns**: true if the line was added successfully, false otherwise

#### `macro_t* find_macro(macro_table_t *table, const char *name)`

- **Description**: Find a macro by name
- **Parameters**:
    - `table`: The macro table
    - `name`: The name of the macro to find
- **Returns**: Pointer to the macro if found, NULL otherwise

#### `void free_macro_table(macro_table_t *table)`

- **Description**: Free the macro table and all its entries
- **Parameters**:
    - `table`: The macro table to free

## Error Handling (Additional Functions)

#### `void set_error_line(error_context_t *context, int line_number)`

- **Description**: Set the current line number in the error context
- **Parameters**:
    - `context`: The error context
    - `line_number`: The current line number

#### `void cleanup_resources(FILE *file, void *ptr1, void *ptr2, void *ptr3)`

- **Description**: Cleanup function for freeing resources on error
- **Parameters**:
    - `file`: File pointer to close (can be NULL)
    - `ptr1`: First pointer to free (can be NULL)
    - `ptr2`: Second pointer to free (can be NULL)
    - `ptr3`: Third pointer to free (can be NULL)

## Symbol Table Management (Additional Functions)

#### `bool add_symbol_attributes(symbol_table_t *table, const char *name, symbol_attr_t attributes)`

- **Description**: Add attributes to a symbol
- **Parameters**:
    - `table`: The symbol table
    - `name`: The name of the symbol to update
    - `attributes`: The attributes to add (can be multiple with OR)
- **Returns**: true if the attributes were added successfully, false otherwise

#### `void update_data_symbols(symbol_table_t *table, int offset)`

- **Description**: Update all data symbols by adding an offset
- **Parameters**:
    - `table`: The symbol table
    - `offset`: The offset to add to data symbols

#### `void free_symbol_table(symbol_table_t *table)`

- **Description**: Free the symbol table and all its entries
- **Parameters**:
    - `table`: The symbol table to free

## Main Program Flow

The assembler follows these main steps for processing each input file:

1. **Pre-Assembler Phase**:

- Expand macros
- Generate the expanded source file (.am)

2. **First Pass**:

- Parse each line
- Build the symbol table
- Calculate addresses for labels
- Process directives (.data, .string, .entry, .extern)
- Calculate instruction sizes

3. **Second Pass**:

- Generate machine code
- Resolve symbol references
- Process .entry directives
- Track external references

4. **Output Generation**:

- Generate object file (.ob)
- Generate entry file (.ent) if needed
- Generate external file (.ext) if needed

The assembler provides detailed error reporting at each stage, with line numbers and descriptive error messages to help
debug assembly code.