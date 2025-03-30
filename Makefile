# Makefile for the Two-Pass Assembler

# Compiler settings
CC = gcc
CFLAGS = -std=c90 -Wall -Wextra -pedantic
DEBUG = -g

# Directories
SRC_DIR = src
INCLUDE_DIR = include

# Default target
all: 
	@echo "No targets defined yet"

# Clean rule
clean:
	rm -f *.o assembler

.PHONY: all clean
