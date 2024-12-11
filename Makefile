# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Itrace-selection -O3 -g

# Source files
SRCS = $(wildcard trace-selection/*.c) main.c

# Object files
OBJS = $(SRCS:.c=.o)

# GLib compiler flags
GLIB_CFLAGS = $(shell pkg-config --cflags glib-2.0)
GLIB_LDFLAGS = $(shell pkg-config --libs glib-2.0)
CFLAGS += $(GLIB_CFLAGS)
LDFLAGS = $(GLIB_LDFLAGS)
CFLAGS += -pg
LDFLAGS += -pg


# Executable name
EXEC = main

# Default target
all: $(EXEC)

# Link the object files to create the executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

# Clean up build files
clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean