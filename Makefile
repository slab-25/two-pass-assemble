# Makefile for the Two-Pass Assembler

# Compiler settings
CC = gcc
CFLAGS = -std=c90 -Wall -Wextra -pedantic
DEBUG = -g


# Directories
SRC_DIR = src
INCLUDE_DIR = include

# Source files
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/pre_assembler.c \
          $(SRC_DIR)/first_pass.c \
          $(SRC_DIR)/second_pass.c \
          $(SRC_DIR)/symbol_table.c \
          $(SRC_DIR)/output.c \
          $(SRC_DIR)/utils.c

# Header files
HEADERS = $(INCLUDE_DIR)/assembler.h \
          $(INCLUDE_DIR)/pre_assembler.h \
          $(INCLUDE_DIR)/first_pass.h \
          $(INCLUDE_DIR)/second_pass.h \
          $(INCLUDE_DIR)/symbol_table.h \
          $(INCLUDE_DIR)/output.h \
          $(INCLUDE_DIR)/utils.h

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable name
EXECUTABLE = assembler

# Default target
all: $(EXECUTABLE)

# Linking rule
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

# Compilation rule
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean rule
clean:
	rm -f $(SRC_DIR)/*.o $(EXECUTABLE)

# Debug build
debug: CFLAGS += $(DEBUG)
debug: all

# Testing rule
test: $(EXECUTABLE)
	cd tests && ./test_runner.sh

# Phony targets
.PHONY: all clean debug test