#include <stdlib.h>

// Flags
#define PRINT_TRACE 0x01
#define PRINT_BLOCK 0x02

#define BRANCH 0x01
#define TERMINATING 0x02

// Define the maximum number of blocks in a trace
#define MAX_BLOCKS 1000

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