# Assembler Test Suite

This directory contains test files for verifying the assembler's functionality.

## Test Files

The test suite includes the following files:

1. **basic.as** - Tests basic assembler functionality including labels, simple instructions, and data directives.

2. **macro.as** - Tests macro definition and expansion, including nested macros.

3. **addressing.as** - Tests all four addressing modes (immediate, direct, relative, register) in various combinations.

4. **directives.as** - Tests all directive types (.data, .string, .entry, .extern).

5. **edge_cases.as** - Tests various edge cases like label length limits, large numeric values, etc.

6. **errors.as** - Contains intentional errors to test error reporting functionality.

## Running Tests

To run the tests, make sure you have built the assembler, then execute:

```bash
chmod +x run_tests.sh
./run_tests.sh
```

The script will:
1. Run the assembler on each test file
2. Report success or failure
3. Save all output files to the `results` directory

## Expected Results

For all test files except `errors.as`, the assembler should complete successfully and generate:
- `.am` (after macro expansion) file
- `.ob` (object code) file
- `.ent` (entry symbols) file for tests that define entry points
- `.ext` (external references) file for tests that reference external symbols

For `errors.as`, the assembler should detect the errors and exit with a non-zero status code.

## Test Analysis

After running the tests, you can analyze:
- Generated object files to verify correct machine code
- Entry and external files for correct symbol handling
- Error messages in the error log files

## Adding New Tests

To add a new test:
1. Create a new `.as` file in this directory
2. Add the test name (without extension) to the `TEST_FILES` array in `run_tests.sh`