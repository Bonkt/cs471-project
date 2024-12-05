/*
 * File: trace-selection.c
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the implementation of the trace selection for the project.
 * 
 */

#include <trace-selection.h>

void print_trace(FILE* file, Trace *trace, size_t FLAGS) {
    if (FLAGS & PRINT_TRACE) 
        printf("\n\nTrace: %d blocks,    %d instructions,    start address: %llu,    end address: %llu\n", trace->nb_blocks, trace->nb_instructions, trace->start_address, trace->end_address);
    
    if (FLAGS & PRINT_BLOCK)
    {
        for (int i = 0; i < trace->nb_blocks; i++) {
            print_block(file, trace->blocks_p[i]);
        }
    }
    
    
}


/* How to identify a trace
    - Look for the first instruction of the trace
    - If the instruction is not in the trace list, create a new trace
    - If the instruction is in the trace list, continue reading the trace
    - At the end of the trace, look at the blocks in the trace
    - If all block start address and end address are the same
*/

Trace* trace_selection(FILE *file, int* start_index) {
    int nb_blocks = 0;
    int remaining = MAX_INSTRUCTIONS;
    block_t** blocks = parse_block_terminating(file, start_index, &remaining, &nb_blocks);
    if (!blocks) {
        perror("Error parsing blocks");
        return NULL;
    }
    // add logic to check if trace already exists (need the map structure)
    //*start_index = blocks[*nb_blocks - 1]->end_index + 1;
    return trace_builder(file, blocks, nb_blocks);
}


/*
{
    size_t curr_index = 0;
    Map* map = malloc(sizeof(Map) * MAX_TRACES);
    if (!map) {
        perror("Error allocating memory");
        return;
    }
    
}
*/


Trace* trace_builder(FILE *file, block_t** blocks, int size) {
    Trace* trace = malloc(sizeof(Trace));
    if (!trace) {
        perror("Error allocating memory");
        return NULL;
    }
    trace->id = 0;
    trace->nb_blocks = size;
    trace->nb_instructions = blocks[size - 1]->end_index - blocks[0]->start_index + 1;
    trace->start_address = parse_inst(file, blocks[0]->start_index).address;
    trace->end_address = parse_inst(file, blocks[size - 1]->end_index).address;
    trace->blocks_p = blocks;
    trace->reuse = 0;
    trace->distance = 0;
    trace->last_used = 0;
    return trace; // maybe return un index de la map plutôt qu'un pointeur et faire les checks directement dans la fonction
}