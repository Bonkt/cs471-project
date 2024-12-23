#ifndef DATATYPE_H
#define DATATYPE_H

#include <stdint.h>  
#include <stdlib.h>
#include <glib.h>

// Flags
#define PRINT_TRACE 0x01
#define PRINT_BLOCK 0x02

#define BRANCH 0x01
#define TERMINATING 0x02
#define _EOF 0x04

// Define the maximum number of blocks in a trace
extern uint64_t g_max_blocks;
// Define the maximum number of instructions in trace
extern uint64_t g_max_instructions;
// Define the maximum number of traces
#define MAX_TRACES 1000 // not used for now.

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
    unsigned int block_index;
    unsigned long long start_address;
    unsigned long long end_address;
    guint block_hash;
} block_t;

typedef struct {
    block_t** blocks_p;
    unsigned int nb_blocks;
    unsigned int size;
    unsigned int trace_count;
    unsigned long long file_size;
    // FILE* file; // We will no longer need this
    unsigned char *mapped_file; // base pointer to the mmapped file
    GHashTable *hash_table; // Hash table for storing the traces structure
    
    
    GHashTable *blocks_map; // hashmap<KEY(block_t), block_t*>
} data_t;

// Traces structure
typedef struct {
    // ID of the trace
    unsigned int id;
    // Number of blocks in the trace
    unsigned int nb_blocks;
    // Number of instructions in the trace
    unsigned int nb_instructions;
    // Start address of the trace
    unsigned long long start_address;
    // End address of the trace
    unsigned long long end_address;
    // Array of index to the blocks in the trace
    unsigned int* blocks_p;
    // Number of reuse
    size_t reuse;
    // Distance
    long int distance;
    // index of the last execution
    size_t last_used;
} Trace;

#endif