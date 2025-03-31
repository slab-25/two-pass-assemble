#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Initialize counters
PASS_COUNT=0
FAIL_COUNT=0
ERROR_PASS_COUNT=0
ERROR_FAIL_COUNT=0

# Directories
INPUT_DIR="inputs"
OUTPUT_DIR="outputs"

# Create output directory
mkdir -p "$OUTPUT_DIR"

echo -e "${BLUE}Starting assembler tests...${NC}"
echo -e "${BLUE}==========================${NC}"

# Build the assembler
echo -e "${YELLOW}Building assembler...${NC}"
make -C ..

# Find assembler executable
ASSEMBLER="../bin/assembler"
echo "Current directory: $(pwd)"
echo -e "${YELLOW}Looking for executable files:${NC}"
ls -l $ASSEMBLER
echo "Found assembler at: $ASSEMBLER"

# Function to run a test
run_test() {
    local test_file=$1
    local expect_error=$2
    local input_path="$INPUT_DIR/${test_file}.as"
    local output_base="$OUTPUT_DIR/${test_file}"

    echo -e "\n${YELLOW}Testing: ${test_file}.as${NC}"
    echo "Running: $ASSEMBLER $input_path"

    $ASSEMBLER "$input_path" > "${output_base}.out" 2> "${output_base}.err"
    EXIT_STATUS=$?

    if [ "$expect_error" = "true" ]; then
        if [ $EXIT_STATUS -ne 0 ]; then
            echo -e "${GREEN}✓ Assembler correctly detected errors (as expected)${NC}"
            echo -e "${YELLOW}Error output:${NC}"
            cat "${output_base}.err"
            echo -e "${GREEN}Result: PASS${NC}"
            ((ERROR_PASS_COUNT++))
        else
            echo -e "${RED}✗ Assembler did not detect expected errors${NC}"
            echo -e "${RED}Result: FAIL${NC}"
            ((ERROR_FAIL_COUNT++))
        fi
    else
        if [ $EXIT_STATUS -eq 0 ]; then
            echo -e "${GREEN}✓ Assembler completed successfully${NC}"
            [ -f "${input_path%.as}.ob" ] && mv "${input_path%.as}.ob" "${output_base}.ob"
            [ -f "${input_path%.as}.ent" ] && mv "${input_path%.as}.ent" "${output_base}.ent"
            [ -f "${input_path%.as}.ext" ] && mv "${input_path%.as}.ext" "${output_base}.ext"
            [ -f "${input_path%.as}.am" ] && mv "${input_path%.as}.am" "${output_base}.am"
            echo -e "${GREEN}Result: PASS${NC}"
            ((PASS_COUNT++))
        else
            echo -e "${RED}✗ Assembler failed${NC}"
            cat "${output_base}.err"
            echo -e "${RED}Result: FAIL${NC}"
            ((FAIL_COUNT++))
        fi
    fi
    echo "------------------------"
}

# Run regular tests
for test_file in basic macro addressing directives edge_cases comprehensive \
                 macro_edge_cases macro_with_labels boundary_cases nested_macros \
                 label_conflicts data_macros macro_chains whitespace_macros \
                 comment_variations register_macros complex_macros jump_macros; do
    run_test "$test_file" "false"
done

# Run error tests
for test_file in errors macro_errors; do
    run_test "$test_file" "true"
done

echo -e "${BLUE}==========================${NC}"
echo -e "Testing complete. Results saved in ${YELLOW}${OUTPUT_DIR}${NC}"

echo -e "\n${BLUE}Final Test Summary${NC}"
echo "-----------------"
echo -e "Regular tests: ${GREEN}${PASS_COUNT} passed${NC}, ${RED}${FAIL_COUNT} failed${NC}"
echo -e "Error tests: ${GREEN}${ERROR_PASS_COUNT} passed${NC}, ${RED}${ERROR_FAIL_COUNT} failed${NC}"
echo -e "Total: ${GREEN}$((PASS_COUNT + ERROR_PASS_COUNT)) passed${NC}, ${RED}$((FAIL_COUNT + ERROR_FAIL_COUNT)) failed${NC}"