#!/bin/bash
# Test runner script for the Two-Pass Assembler

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if we're running in Docker
if [ -f "/.dockerenv" ] || grep -q docker /proc/1/cgroup; then
    # We're in Docker, use absolute path
    ASSEMBLER="/project/assembler"
else
    # We're not in Docker, use relative path
    ASSEMBLER="../assembler"
fi

TESTS_DIR="."
PASS_COUNT=0
FAIL_COUNT=0
TOTAL_TESTS=0

# Function to run a test and check the result
run_test() {
    local test_file="$1"
    local expected_result="$2"
    local test_name=$(basename "$test_file")

    echo -e "${YELLOW}Running test: ${test_name}${NC}"

    # Run the assembler on the test file
    "$ASSEMBLER" "$test_file" > /dev/null 2>&1
    local status=$?

    if [ "$status" -eq "$expected_result" ]; then
        echo -e "${GREEN}PASS: ${test_name}${NC}"
        ((PASS_COUNT++))
    else
        echo -e "${RED}FAIL: ${test_name} (expected: $expected_result, got: $status)${NC}"
        ((FAIL_COUNT++))
    fi

    ((TOTAL_TESTS++))
}

# Function to run tests from a directory if it exists
run_tests_if_dir_exists() {
    local dir="$1"
    local pattern="$2"
    local expected="$3"

    if [ -d "$dir" ]; then
        for test_file in $dir/$pattern; do
            if [ -f "$test_file" ]; then
                run_test "$test_file" "$expected"
            fi
        done
    else
        echo -e "${YELLOW}Skipping tests in non-existent directory: ${dir}${NC}"
    fi
}

# Check if assembler exists
if [ ! -f "$ASSEMBLER" ]; then
    echo -e "${RED}Error: Assembler not found at ${ASSEMBLER}${NC}"
    exit 1
fi

# Change to the tests directory if not already there
if [ "$(basename "$(pwd)")" != "tests" ]; then
    cd "$TESTS_DIR" || exit 1
fi

echo -e "${YELLOW}=============== Testing Pre-Assembler ===============${NC}"
run_tests_if_dir_exists "iteration1_preassembler" "basic_*.as" 0

echo -e "${YELLOW}=============== Testing First Pass ===============${NC}"
run_tests_if_dir_exists "iteration2_firstpass" "basic_*.am" 0

echo -e "${YELLOW}=============== Testing Second Pass ===============${NC}"
run_tests_if_dir_exists "iteration3_secondpass" "encoding_*.am" 0

echo -e "${YELLOW}=============== Testing Integration ===============${NC}"
run_tests_if_dir_exists "integration" "full_*.as" 0

# Print summary
echo -e "${YELLOW}=============== Test Summary ===============${NC}"
echo -e "Total tests: ${TOTAL_TESTS}"
echo -e "${GREEN}Passed: ${PASS_COUNT}${NC}"
if [ "$FAIL_COUNT" -gt 0 ]; then
    echo -e "${RED}Failed: ${FAIL_COUNT}${NC}"
fi

# Return 0 if all tests passed, 1 otherwise
if [ "$FAIL_COUNT" -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed.${NC}"
    exit 1
fi