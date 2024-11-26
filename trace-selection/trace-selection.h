/*
    Header file for trace-selection.c 

    Author: Gabin Forestier
*/

// Define the maximum number of blocks in a trace
#define MAX_BLOCKS 1000

// Define the maximum number of instructions in trace
#define MAX_INSTRUCTIONS 1000

// Define the maximum number of traces
#define MAX_TRACES 1000

// Traces structure
typedef struct {
    // Number of blocks in the trace
    int nb_blocks;
    // Number of instructions in the trace
    int nb_instructions;
    // Start address of the trace
    unsigned long long start_address;
    // End address of the trace
    unsigned long long end_address;
    // Array of pointers to the blocks in the trace
    unsigned long long blocks_p[MAX_BLOCKS];
    // Number of reuse
    int reuse;
} Trace;

/* How to identify a trace
    - Look for the first instruction of the trace
    - If the instruction is not in the trace list, create a new trace
    - If the instruction is in the trace list, continue reading the trace
    - At the end of the trace, look at the blocks in the trace
    - If all block start address and end address are the same
*/



