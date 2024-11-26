/*
 * File: trace-selection.h
 * Author: Robert William, Gabin Forestier
 * Date: 2024-11-26
 * Description: This file contains the header for the trace selection for the project.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parser.h>
#include <datatype.h>

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
    block_t** blocks_p;
    // Number of reuse
    size_t reuse;
    // Distance
    u_int64_t distance;
    // index of the last execution
    size_t last_used;
} Trace;

typedef Trace* Map;


/* How to identify a trace
    - Look for the first instruction of the trace
    - If the instruction is not in the trace list, create a new trace
    - If the instruction is in the trace list, continue reading the trace
    - At the end of the trace, look at the blocks in the trace
    - If all block start address and end address are the same
*/

// Function prototypes
void print_trace(Trace *trace);

void trace_selection(FILE *file);

Trace* build_trace(FILE *file, int start_index);


