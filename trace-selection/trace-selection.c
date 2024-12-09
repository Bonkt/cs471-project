/*
 * File: trace-selection.c
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the implementation of the trace selection for the project.
 * 
 */

#include <trace-selection.h>

void print_trace(data_t* data, Trace *trace, size_t FLAGS) {
    if (FLAGS & PRINT_TRACE) 
        printf("\n\nTrace: %d,   %d blocks,    %d instructions,    start address: %llu,    end address: %llu\n", trace->id, trace->nb_blocks, trace->nb_instructions, trace->start_address, trace->end_address);
    
    if (FLAGS & PRINT_BLOCK)
    {
        for (unsigned int i = 0; i < trace->nb_blocks; i++) {
            print_block(data, trace->blocks_p[i]);
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

Trace* trace_parser(data_t* data, unsigned int* start_index) {
    unsigned int nb_blocks = 0;
    unsigned int remaining = MAX_INSTRUCTIONS;
    unsigned int* blocks = parse_block_terminating(data, start_index, &remaining, &nb_blocks);
    if (!blocks) {
        perror("Error parsing blocks");
        return NULL;
    }

    Trace* trace = trace_builder(data, blocks, nb_blocks, *start_index);
    if(!trace) {
        perror("Error building trace");
        return NULL;
    }
    Trace* lookup = find_trace(data, trace);
    if(lookup == NULL) {
        insert_trace(data, trace);
        return trace;
    } else {
        free_trace(trace);
        update_trace(lookup, *start_index);
        return lookup;
    }
}


Trace* trace_builder(data_t* data, unsigned int* blocks, unsigned int size, unsigned int start_index) {
    Trace* trace = malloc(sizeof(Trace));
    if (!trace) {
        perror("Error allocating memory");
        return NULL;
    }
    trace->id = 0;
    trace->nb_blocks = size;
    trace->nb_instructions = data->blocks_p[blocks[size - 1]]->end_index - data->blocks_p[blocks[0]]->start_index + 1;
    trace->start_address = parse_inst(data, data->blocks_p[blocks[0]]->start_index).address;
    trace->end_address = parse_inst(data, data->blocks_p[blocks[size - 1]]->end_index).address;
    trace->blocks_p = blocks;
    trace->reuse = 1;
    trace->distance = 0;
    trace->last_used = start_index;
    return trace; // maybe return un index de la map plutôt qu'un pointeur et faire les checks directement dans la fonction
}

void insert_trace(data_t* data, Trace* trace) {
    trace->id = data->trace_count++;
    insert_value(data, (unsigned int) trace->start_address, (unsigned int) trace->end_address, trace);
}

Trace* find_trace(data_t* data, Trace* trace) {
    Trace* result = find_value(data, (unsigned int) trace->start_address, (unsigned int) trace->end_address);
    if (!result || result->nb_blocks != trace->nb_blocks 
                || result->nb_instructions != trace->nb_instructions) {
        return NULL;
    }
    for (size_t i = 0; i < trace->nb_blocks; i++)
    {
        if(compare_block(data, data->blocks_p[trace->blocks_p[i]], data->blocks_p[result->blocks_p[i]]))
            continue;
        else
            return NULL;
    }
    
    return result;
}

void update_trace(Trace* trace, unsigned int index)
{
    trace->distance += index - trace->last_used;
    trace->last_used = index;
    trace->reuse++;
}

void free_trace(Trace* trace)
{
    free(trace->blocks_p);
    free(trace);
}