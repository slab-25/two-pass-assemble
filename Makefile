# Compiler settings
CC = gcc
CFLAGS = -std=c90 -Wall -Wextra -pedantic -g
INCLUDES = -Iinclude

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = tests
TEST_INPUTS = $(TEST_DIR)/inputs
TEST_OUTPUTS = $(TEST_DIR)/outputs

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/assembler

# Main targets
.PHONY: all clean test test-setup directories

all: directories $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)
	mkdir -p $(TEST_INPUTS) $(TEST_OUTPUTS)

test: all test-setup
	@echo "Running tests..."
	@cd $(TEST_DIR) && ./run_tests.sh

test-setup: directories
	@chmod +x $(TEST_DIR)/run_tests.sh
	@find $(TEST_OUTPUTS) -type f -delete 2>/dev/null || true

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	rm -rf $(TEST_OUTPUTS)
	find $(TEST_INPUTS) -type f \( -name "*.am" -o -name "*.ob" -o -name "*.ent" -o -name "*.ext" \) -delete