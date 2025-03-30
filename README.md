# Two-Pass Assembler

A custom assembler for a 24-bit assembly language as defined in the Systems Programming Lab course.

## Project Overview

This project implements a two-pass assembler that translates assembly language programs into machine code according to
the specified language definition. The system follows a typical two-pass approach:

1. **Pre-Assembler**: Expands macros and generates an expanded source file (.am)
2. **First Pass**: Builds the symbol table and calculates addresses
3. **Second Pass**: Generates machine code using the symbol table
4. **Output Generation**: Creates object, entry, and external files

## Features

- Macro expansion
- Symbol resolution for labels
- Support for data and string directives
- Support for external and entry references
- Comprehensive error detection and reporting
- Multiple file processing

## Building the Project

### Standard Build

1. Clone the repository
2. Run `make` to build the assembler
3. The executable will be named `assembler`

```bash
git clone https://github.com/username/two-pass-assembler.git
cd two-pass-assembler
make
```

## Usage

```bash
./bin/assembler file1 file2 ...
```

For each source file (.as), the assembler will generate:

- A macro-expanded file (.am)
- An object file (.ob) with machine code
- An entry points file (.ent) if any entry points are defined
- An external references file (.ext) if any external references are used

## Assembly Language Specification

### Instructions

The assembler supports the following instructions:

- `mov`: Move value between operands
- `cmp`: Compare operands
- `add`: Add source to destination
- `sub`: Subtract source from destination
- `lea`: Load effective address
- `clr`: Clear operand
- `not`: Logical NOT on operand
- `inc`: Increment operand
- `dec`: Decrement operand
- `jmp`: Jump to address
- `bne`: Branch if not equal (to zero)
- `jsr`: Jump to subroutine
- `red`: Read from standard input
- `prn`: Print to standard output
- `rts`: Return from subroutine
- `stop`: Halt program

### Directives

- `.data`: Define data values
- `.string`: Define a string
- `.entry`: Define an entry point
- `.extern`: Declare an external symbol

### Addressing Methods

- **Immediate addressing**: `#value`
- **Direct addressing**: `label`
- **Relative addressing**: `&label`
- **Register addressing**: `r0` to `r7`

## Project Structure

- `src/`: Source code files
- `include/`: Header files
- `obj/`: Object files (created during build)
- `bin/`: Binary executables (created during build)

## Implementation Status

- ✅ Project structure
- ✅ Core utilities
- ✅ Build system
- ✅ Pre-assembler
- ✅ Symbol table
- ✅ First pass
- ✅ Second pass
- ✅ Output generation

## C90 Compliance

This project strictly adheres to the C90 standard for compatibility with older systems and compiler requirements. It has been tested to compile and run correctly on Ubuntu 16.04 with gcc using the flags: `-std=c90 -Wall -Wextra -pedantic`.

## Error Handling

The assembler includes a comprehensive error reporting system that identifies:

- Syntax errors
- Undefined symbols
- Duplicate definitions
- Invalid addressing modes
- And more...

All errors are reported with filename and line number for easy debugging.