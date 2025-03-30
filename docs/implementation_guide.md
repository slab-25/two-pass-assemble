# Two-Pass Assembler Implementation Guide

## Project Overview

This document provides a detailed guide for implementing a two-pass assembler for a custom assembly language as defined in the Systems Programming Lab course. The assembler processes assembly language source files, handles macros, manages symbols, and generates machine code according to the specification.

## Implementation Progress

- **Phase 1**: Project setup and core utilities ✅
- **Phase 2**: Pre-assembler (macro processor) ✅
- **Phase 3**: Symbol table management ✅
- **Phase 4**: First pass implementation ⚠️ (Stubs only)
- **Phase 5**: Second pass implementation ⚠️ (Stubs only)
- **Phase 6**: Output generation ⚠️ (Stubs only)

## System Architecture

The assembler follows a four-phase pipeline, each with a clear input, output, and responsibility:

```
Pre-Assembler → First Pass → Second Pass → Output Generation
```

| Phase | Input | Output | Primary Responsibility |
|-------|-------|--------|------------------------|
| Pre-Assembler | `.as` file | `.am` file | Macro expansion |
| First Pass | `.am` file | Symbol table, IC, DC | Symbol resolution, address calculation |
| Second Pass | `.am` file, Symbol table | Code image, Data image | Machine code generation |
| Output Generation | Code/Data images, Symbol information | `.ob`, `.ent`, `.ext` files | Output file creation |

## Module Dependency Structure

```
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│  Pre-Assembler  │───▶ │   First Pass    │───▶ │   Second Pass   │───▶ │ Output Generator │
└─────────────────┘     └─────────────────┘     └─────────────────┘     └─────────────────┘
         │                      ▲                       ▲                        ▲
         │                      │                       │                        │
         │                      │                       │                        │
         │              ┌─────────────────┐             │                        │
         └─────────────▶│  Symbol Table   │◀────────────┘                        │
                        └─────────────────┘                                      │
                               ▲                                                 │
                               │                                                 │
                        ┌─────────────────┐                                      │
                        │    Utilities    │◀─────────────────────────────────────┘
                        └─────────────────┘
```

## Implementation Iterations

### Iteration 1: Foundation Layer and Pre-Assembler (✅)

**Objective**: Set up core infrastructure and implement the macro expansion mechanism.

**Key Files**:
- `assembler.h`: Common definitions and constants
- `utils.h`/`utils.c`: Utility functions
- `pre_assembler.h`/`pre_assembler.c`: Macro processing

**Core Functions**:
- `process_file()`: Expands macros from `.as` to `.am` files
- `create_macro_table()`: Manages macro definitions
- `add_macro()`: Adds a macro to the table
- `find_macro()`: Retrieves a macro definition

### Iteration 2: Symbol Table Management (✅)

**Objective**: Implement symbol table for storing label information.

**Key Files**:
- `symbol_table.h`/`symbol_table.c`: Symbol table management

**Core Functions**:
- `create_symbol_table()`: Creates a new symbol table
- `add_symbol()`: Adds a symbol to the table
- `find_symbol()`: Finds a symbol by name
- `update_data_symbols()`: Updates data symbol addresses

### Iteration 3: First Pass Implementation (⚠️)

**Objective**: Build the symbol table and compute memory addresses for instructions and data.

**Key Files**:
- `first_pass.h`/`first_pass.c`: First pass implementation

**Core Functions**:
- `first_pass()`: Main first pass function
- `parse_line()`: Parses a line into components
- `process_data_directive()`: Processes .data directive
- `process_instruction()`: Processes machine instruction

### Iteration 4: Second Pass Implementation (⚠️)

**Objective**: Generate the final machine code, focusing on proper 24-bit encoding.

**Key Files**:
- `second_pass.h`/`second_pass.c`: Second pass implementation

**Core Functions**:
- `second_pass()`: Main second pass function
- `encode_instruction()`: Encodes a machine instruction
- `get_addressing_method()`: Determines addressing method

### Iteration 5: Output Generation (⚠️)

**Objective**: Generate output files in the specified formats.

**Key Files**:
- `output.h`/`output.c`: Output file generation

**Core Functions**:
- `generate_output_files()`: Main output generation function
- `write_object_file()`: Writes the object file
- `write_entries_file()`: Writes the entries file
- `write_externals_file()`: Writes the externals file

## Next Steps

1. **Complete First Pass**:
    - Implement full parsing for assembly lines
    - Process all directives (.data, .string, .extern, .entry)
    - Calculate instruction sizes and update IC/DC

2. **Implement Second Pass**:
    - Encode instructions with proper bit patterns
    - Resolve symbol references
    - Handle external symbols

3. **Complete Output Generation**:
    - Generate .ob file with machine code
    - Create .ent and .ext files as needed

## Testing

Use the test runner script to verify the functionality of each component:

```bash
cd tests
./test_runner.sh
```

This tests the assembler with various input files to ensure each component works correctly.

## Critical Specifications

### 24-Bit Word Format

All encoded instructions must use the exact 24-bit structure:

```
Bits:  23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
       ├─────────────┼───────┼───────┼───────┼───────┼─────────┼─────┤
Fields:   opcode    src_m  src_reg  dst_m  dst_reg   funct    ARE
```

### Memory Organization

- Code (instructions) starts at address 100 (decimal)
- Data immediately follows the code section
- Any symbol references or addresses in the output must reflect this organization

This implementation guide provides a roadmap for completing the two-pass assembler. Follow the remaining iterations to create a fully functional assembler that meets all project requirements.