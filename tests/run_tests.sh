#!/bin/bash
# Test script for the two-pass assembler

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

# Create directories
mkdir -p obj bin results

echo -e "${BLUE}Starting assembler tests...${NC}"
echo -e "${BLUE}==========================${NC}"

# Build the assembler
echo -e "${YELLOW}Building assembler...${NC}"
make -C ..

# Find assembler executable
ASSEMBLER="../bin/assembler"
echo "Current directory: $(pwd)"
echo -e "${YELLOW}Looking for executable files:${NC}"
ls -l $ASSEMBLER ../tests/run_tests.sh
echo "Found assembler at: $ASSEMBLER"

# Function to run a test
run_test() {
    local test_file=$1
    local expect_error=$2

    echo -e "\n${YELLOW}Testing: ${test_file}.as${NC}"
    echo "Running: $ASSEMBLER ${test_file}.as"

    $ASSEMBLER "${test_file}.as" > "results/${test_file}.out" 2> "results/${test_file}.err"
    EXIT_STATUS=$?

    if [ "$expect_error" = "true" ]; then
        if [ $EXIT_STATUS -ne 0 ]; then
            echo -e "${GREEN}✓ Assembler correctly detected errors (as expected)${NC}"
            echo -e "${YELLOW}Error output:${NC}"
            cat "results/${test_file}.err"
            if [ -f "${test_file}.am" ]; then
                echo -e "${GREEN}✓ Macro expansion file created${NC}"
                if grep -q "mcro" "${test_file}.am"; then
                    echo -e "${YELLOW}Warning: Found 'mcro' in output file - macro may not have expanded properly!${NC}"
                    echo -e "${YELLOW}Result: WARNING${NC}"
                else
                    echo -e "${GREEN}Result: PASS${NC}"
                fi
            else
                echo -e "${GREEN}Result: PASS${NC}"
            fi
            ((ERROR_PASS_COUNT++))
        else
            echo -e "${RED}✗ Assembler did not detect expected errors${NC}"
            echo -e "${RED}Result: FAIL${NC}"
            ((ERROR_FAIL_COUNT++))
        fi
    else
        if [ $EXIT_STATUS -eq 0 ]; then
            echo -e "${GREEN}✓ Assembler completed successfully${NC}"
            [ -f "${test_file}.ob" ] && echo -e "${GREEN}✓ Object file created${NC}"
            [ -f "${test_file}.ent" ] && echo -e "${GREEN}✓ Entry file created${NC}"
            [ -f "${test_file}.ext" ] && echo -e "${GREEN}✓ External file created${NC}"
            [ -f "${test_file}.am" ] && echo -e "${GREEN}✓ Macro expansion file created${NC}"
            echo -e "${GREEN}Result: PASS${NC}"
            ((PASS_COUNT++))
        else
            echo -e "${RED}✗ Assembler failed${NC}"
            if [ -f "results/${test_file}.err" ]; then
                echo -e "${YELLOW}Error output:${NC}"
                cat "results/${test_file}.err"
            fi
            echo -e "${RED}Result: FAIL${NC}"
            ((FAIL_COUNT++))
        fi
    fi
    
    # Move generated files to results directory
    mv -f "${test_file}.ob" "results/" 2>/dev/null || true
    mv -f "${test_file}.am" "results/" 2>/dev/null || true
    mv -f "${test_file}.ent" "results/" 2>/dev/null || true
    mv -f "${test_file}.ext" "results/" 2>/dev/null || true
    
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
echo -e "Testing complete. Results saved in the ${YELLOW}results${NC} directory."
echo -e "\n${BLUE}Final Test Summary${NC}"
echo "-----------------"
echo -e "Regular tests: ${GREEN}${PASS_COUNT} passed${NC}, ${RED}${FAIL_COUNT} failed${NC}"
echo -e "Error tests: ${GREEN}${ERROR_PASS_COUNT} passed${NC}, ${RED}${ERROR_FAIL_COUNT} failed${NC}"
echo -e "Total: ${GREEN}$((PASS_COUNT + ERROR_PASS_COUNT)) passed${NC}, ${RED}$((FAIL_COUNT + ERROR_FAIL_COUNT)) failed${NC}"

echo -e "\n${BLUE}Macro Expansion Analysis${NC}"
echo "----------------------"
for test_file in "macro" "macro_edge_cases" "macro_with_labels" "comprehensive"; do
    if [ -f "results/${test_file}.am" ]; then
        echo -e "${YELLOW}${test_file}.am:${NC}"
        if ! grep -q "mcro" "results/${test_file}.am"; then
            echo -e "${GREEN}✓ No macro definitions found (correct)${NC}"
        fi
        if grep -q "inc r1" "results/${test_file}.am"; then
            echo -e "${GREEN}✓ Found expanded macro content${NC}"
        fi
        echo
    fi
done