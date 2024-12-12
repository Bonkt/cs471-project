# Compiler for C and C++
CC = gcc
CXX = g++

# Compiler flags
CFLAGS = -Wall -Wextra -Itrace-selection -O3 -flto -g
CXXFLAGS = -Wall -Wextra -O3 -flto -g

# Source files
C_SRCS = $(wildcard trace-selection/*.c) main.c
CPP_SRCS = trace_distance_mo.cpp

# Object files
C_OBJS = $(C_SRCS:.c=.o)
CPP_OBJS = $(CPP_SRCS:.cpp=.o)

# GLib compiler flags for C sources
GLIB_CFLAGS = $(shell pkg-config --cflags glib-2.0)
GLIB_LDFLAGS = $(shell pkg-config --libs glib-2.0)
CFLAGS += $(GLIB_CFLAGS)
LDFLAGS += $(GLIB_LDFLAGS)

# Executable names
C_EXEC = main_c
CPP_EXEC = trace_distance_mo

# Default target
all: $(C_EXEC) $(CPP_EXEC)

# Link the C object files to create the C executable
$(C_EXEC): $(C_OBJS)
	$(CC) $(C_OBJS) -o $(C_EXEC) $(LDFLAGS)

# Link the C++ object files to create the C++ executable
$(CPP_EXEC): $(CPP_OBJS)
	$(CXX) $(CPP_OBJS) -o $(CPP_EXEC)

# Compile C source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(C_OBJS) $(CPP_OBJS) $(C_EXEC) $(CPP_EXEC)

.PHONY: all clean
