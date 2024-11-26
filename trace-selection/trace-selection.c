/*
 * File: trace-selection.c
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the implementation of the trace selection for the project.
 * 
 */

#include <trace-selection.h>

void print_trace(Trace *trace) {}


/* How to identify a trace
    - Look for the first instruction of the trace
    - If the instruction is not in the trace list, create a new trace
    - If the instruction is in the trace list, continue reading the trace
    - At the end of the trace, look at the blocks in the trace
    - If all block start address and end address are the same
*/

void trace_selection(FILE *file) 
{
    size_t curr_index = 0;
    Map* map = malloc(sizeof(Map) * MAX_TRACES);
    if (!map) {
        perror("Error allocating memory");
        return;
    }
    
}

Trace* build_trace(FILE *file, block_t** blocks, size_t size) {
    Trace* trace = malloc(sizeof(Trace));
    if (!trace) {
        perror("Error allocating memory");
        return NULL;
    }
    trace->nb_blocks = size;
    trace->nb_instructions = blocks[size - 1]->end_index - blocks[0]->start_index + 1;
    trace->start_address = blocks[0]->start_index;
    trace->end_address = blocks[size - 1]->end_index;
    trace->blocks_p = blocks;
    trace->reuse = 0;
    trace->distance = 0;
    trace->last_used = 0;
    return trace; // maybe return un index de la map plutôt qu'un pointeur et faire les checks directement dans la fonction
}