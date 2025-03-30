# Makefile for two-pass assembler
CC = gcc
CFLAGS = -std=c90 -Wall -Wextra -pedantic -g
INCLUDES = -Iinclude

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Executable name
EXECUTABLE = $(BIN_DIR)/assembler

# Default target
all: directories $(EXECUTABLE)

# Create necessary directories
directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Linking rule
$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@

# Compilation rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Test compilation rule
test: all
	@echo "Running tests..."
	# Test commands can be added here

# Phony targets
.PHONY: all clean directories test