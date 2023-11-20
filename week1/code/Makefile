# Sample Makefile for creating an executable named exercise2 from a C source file

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -std=c11

# Source file
SRCS = NguyenNgocTu_20205135_HW1.c

# Object files (automatically generated from source files)
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = test

# Default target
all: $(EXEC)

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to link object files into the executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(EXEC)

# Phony targets
.PHONY: all clean
