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
## Usage

After building the assembler, you can run it using the following command from the project root directory:

```bash
./bin/assembler file1.as file2.as ...
```

Or if you're in another directory, provide the full path to the executable:

```bash
/path/to/two-pass-assembler/bin/assembler file1.as file2.as ...
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
- `lea`: Load effective address (requires label as source operand)
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

## Syntax Requirements and Constraints

Based on our testing and implementation, the assembler has the following requirements:

1. **Symbol Names**:
    - Should be alphanumeric (starting with a letter)
    - Should not exceed 31 characters
    - Should not contain special characters or underscores
    - Should not be a reserved word

2. **Labels**:
    - Must be followed by a colon (`:`)
    - Must be followed immediately by an instruction or directive
    - No blank lines between label and instruction

3. **Strings**:
    - Must be enclosed in double quotes (`"string"`)
    - Complex special characters may cause parsing issues
    - Escape sequences are not supported

4. **Instruction Specifics**:
    - `lea` requires a label (not a register) as its source operand
    - Register operands must be `r0` through `r7`
    - Relative addressing (`&label`) only works with branch/jump instructions

5. **Macros**:
    - Macro names should follow the same rules as symbol names
    - Macro definitions must be closed with `endmcro`
    - Nested macro definitions are not supported

## Project Structure

- `src/`: Source code files
- `include/`: Header files
- `obj/`: Object files (created during build)
- `bin/`: Binary executables (created during build)
- `tests/`: Test files for verification

## Implementation Status

- ✅ Project structure
- ✅ Core utilities
- ✅ Build system
- ✅ Pre-assembler
- ✅ Symbol table
- ✅ First pass
- ✅ Second pass
- ✅ Output generation

## Testing

A comprehensive test suite is included in the `tests/` directory:

1. **basic.as** - Tests basic assembly instructions and data directives
2. **macro.as** - Tests the pre-assembler and macro expansion functionality
3. **addressing.as** - Tests all addressing modes (immediate, direct, relative, register)
4. **directives.as** - Tests directives (.data, .string, .entry, .extern)
5. **edge_cases.as** - Tests boundary conditions and edge cases
6. **errors.as** - Tests error detection and reporting

To run the tests:

```bash
cd tests
chmod +x run_tests.sh
./run_tests.sh
```

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