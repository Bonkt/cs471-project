#ifndef DATATYPE_H
#define DATATYPE_H

#include <stdlib.h>
#include <glib.h>

// Flags
#define PRINT_TRACE 0x01
#define PRINT_BLOCK 0x02

#define BRANCH 0x01
#define TERMINATING 0x02

// Define the maximum number of blocks in a trace
#define MAX_BLOCKS 5
// Define the maximum number of instructions in trace
#define MAX_INSTRUCTIONS 1000
// Define the maximum number of traces
#define MAX_TRACES 1000

#define BLOCK_LENGTH(x) (x.end_index - x.start_index + 1)
#define IS_BRANCH(x) (x.metadata & BRANCH)
#define IS_TERMINATING(x) (x.metadata & TERMINATING)

typedef struct {
    long int address;
    char metadata;
} inst_t;

typedef struct {
    size_t start_index;
    size_t end_index;
    char metadata;
} block_t;

// Structure for storing values
typedef struct {
    unsigned int key1;
    unsigned int key2;
    char *value;
} BTreeValue;

typedef struct {
    block_t** blocks_p;
    unsigned int nb_blocks;
    unsigned int size;
    unsigned int trace_count;
    FILE* file;
    GTree *tree;
} data_t;

#endif