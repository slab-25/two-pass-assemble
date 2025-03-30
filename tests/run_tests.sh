#!/bin/bash
# Test script for the two-pass assembler

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Create a results directory
mkdir -p results

echo "Starting assembler tests..."
echo "=========================="

# First, let's build the assembler (in case it wasn't built)
echo "Building assembler..."
(cd .. && make) || { echo -e "${RED}Error: Failed to build assembler${NC}"; exit 1; }

# Debug: Show current directory and project structure
echo "Current directory: $(pwd)"
echo "Looking for executable files:"
find .. -type f -executable | grep -v "\.git"

# Find the assembler executable
ASSEMBLER=$(find .. -type f -name "assembler" -o -name "assembler.exe" | head -1)

if [ -z "$ASSEMBLER" ]; then
    echo -e "${RED}Error: Could not find assembler executable${NC}"
    echo "Please check your build process and Makefile"
    exit 1
fi

echo -e "${GREEN}Found assembler at: $ASSEMBLER${NC}"

# Check assembler permissions
if [ ! -x "$ASSEMBLER" ]; then
    echo -e "${YELLOW}Warning: Assembler not executable, fixing permissions...${NC}"
    chmod +x "$ASSEMBLER"
fi

# Array of test files (without extension)
TEST_FILES=("basic" "macro" "addressing" "directives" "edge_cases" "comprehensive")

# Error test file
ERROR_TEST="errors"

# Function to run a test and check results
run_test() {
    local test_file=$1
    echo -e "${YELLOW}Testing: ${test_file}.as${NC}"

    # Check if test file exists
    if [ ! -f "${test_file}.as" ]; then
        echo -e "${RED}Error: Test file '${test_file}.as' not found${NC}"
        return 1
    fi

    # Run assembler on the test file
    echo "Running: $ASSEMBLER ${test_file}.as"
    "$ASSEMBLER" "${test_file}.as" > "results/${test_file}.out" 2> "results/${test_file}.err"

    # Check if the assembler returned success
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Assembler completed successfully${NC}"

        # Check for output files
        if [ -f "${test_file}.ob" ]; then
            echo -e "${GREEN}✓ Object file created${NC}"
            mv "${test_file}.ob" "results/"
        else
            echo -e "${RED}✗ Object file not created${NC}"
            echo -e "${YELLOW}Error output:${NC}"
            cat "results/${test_file}.err"
        fi

        if [ -f "${test_file}.ent" ]; then
            echo -e "${GREEN}✓ Entry file created${NC}"
            mv "${test_file}.ent" "results/"
        fi

        if [ -f "${test_file}.ext" ]; then
            echo -e "${GREEN}✓ External file created${NC}"
            mv "${test_file}.ext" "results/"
        fi
    else
        echo -e "${RED}✗ Assembler failed${NC}"
        echo -e "${YELLOW}Error output:${NC}"
        cat "results/${test_file}.err"
    fi

    # Move the macro expansion file if it exists
    if [ -f "${test_file}.am" ]; then
        echo -e "${GREEN}✓ Macro expansion file created${NC}"
        mv "${test_file}.am" "results/"
    fi

    echo "------------------------"
}

# Test all normal test files
for test_file in "${TEST_FILES[@]}"; do
    run_test "$test_file"
done

# Special test for error file
echo -e "${YELLOW}Testing error detection: ${ERROR_TEST}.as${NC}"
"$ASSEMBLER" "${ERROR_TEST}.as" > "results/${ERROR_TEST}.out" 2> "results/${ERROR_TEST}.err"

# We expect this to fail
if [ $? -ne 0 ]; then
    echo -e "${GREEN}✓ Assembler correctly detected errors${NC}"
    echo -e "${YELLOW}Error output:${NC}"
    cat "results/${ERROR_TEST}.err"
else
    echo -e "${RED}✗ Assembler did not detect errors in error test file${NC}"
fi

# Move any output files from error test
for ext in ".am" ".ob" ".ent" ".ext"; do
    if [ -f "${ERROR_TEST}${ext}" ]; then
        mv "${ERROR_TEST}${ext}" "results/"
    fi
done

echo "=========================="
echo "Testing complete. Results saved in the 'results' directory."