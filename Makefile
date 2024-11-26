# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Itrace-selection -O0 -g

# Source files
SRCS = $(wildcard trace-selection/*.c) main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = main

# Default target
all: $(EXEC)

# Link the object files to create the executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean