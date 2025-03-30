# Two-Pass Assembler

A custom assembler for a 24-bit assembly language as defined in the Systems Programming Lab course.

## Project Overview

This project implements a two-pass assembler that translates assembly language programs into machine code according to the specified language definition. The system follows a typical two-pass approach:

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

### Docker Build (Recommended)

For consistent environment across all systems, we provide a Docker setup:

1. Install Docker and Docker Compose
2. Build and start the container:

```bash
docker-compose up --build -d
```

3. Access the container and build the project:

```bash
docker exec -it two_pass_assembler bash
make
```

This ensures a consistent Ubuntu 16.04 environment with proper C90 support.

## Usage

```bash
./assembler file1 file2 ...
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
- `tests/`: Test files and test runner
- `docs/`: Documentation

## Testing

To run the test suite:

```bash
# Build the assembler first
make

# Run all tests
cd tests
./test_runner.sh
```

The test runner checks different aspects of the assembler:
- Pre-assembler macro expansion
- First pass symbol table construction
- Second pass machine code generation
- End-to-end integration tests

## Current Implementation Status

- ✅ Project structure
- ✅ Core utilities
- ✅ Build system
- ✅ Test infrastructure
- ✅ Docker environment
- ⚠️ Pre-assembler (partial)
- ⚠️ Symbol table (partial)
- ❌ First pass
- ❌ Second pass
- ❌ Output generation

