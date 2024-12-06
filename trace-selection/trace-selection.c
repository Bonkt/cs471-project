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
        printf("\n\nTrace: %d blocks,    %d instructions,    start address: %llu,    end address: %llu\n", trace->nb_blocks, trace->nb_instructions, trace->start_address, trace->end_address);
    
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

Trace* trace_selection(data_t* data, unsigned int* start_index) {
    unsigned int nb_blocks = 0;
    unsigned int remaining = MAX_INSTRUCTIONS;
    unsigned int* blocks = parse_block_terminating(data, start_index, &remaining, &nb_blocks);
    if (!blocks) {
        perror("Error parsing blocks");
        return NULL;
    }
    // add logic to check if trace already exists (need the map structure)
    //*start_index = blocks[*nb_blocks - 1]->end_index + 1;
    return trace_builder(data, blocks, nb_blocks);
}


Trace* trace_builder(data_t* data, unsigned int* blocks, unsigned int size) {
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
    trace->reuse = 0;
    trace->distance = 0;
    trace->last_used = 0;
    return trace; // maybe return un index de la map plutôt qu'un pointeur et faire les checks directement dans la fonction
}

void insert_trace(data_t* data, Trace* trace) {
    BTreeValue* value = malloc(sizeof(BTreeValue));
    if (!value) {
        perror("Error allocating memory");
        return;
    }
    value->key1 = (unsigned int) trace->start_address;
    value->key2 = (unsigned int) trace->end_address;
    value->value = (char*) trace;
    g_tree_insert(data->tree, value, value);
}

Trace* find_trace(data_t* data, Trace* trace) {
    BTreeValue* value = malloc(sizeof(BTreeValue));
    if (!value) {
        perror("Error allocating memory");
        return NULL;
    }
    value->key1 = (unsigned int) trace->start_address;
    value->key2 = (unsigned int) trace->end_address;
    BTreeValue* result = g_tree_lookup(data->tree, value);
    if (!result || ((Trace*) result->value)->id != trace->id 
                || ((Trace*) result->value)->nb_blocks != trace->nb_blocks 
                || ((Trace*) result->value)->nb_instructions != trace->nb_instructions) {
        return NULL;
    }
    for (size_t i = 0; i < trace->nb_blocks; i++)
    {
        if(compare_block(data, data->blocks_p[trace->blocks_p[i]], data->blocks_p[((Trace*) result->value)->blocks_p[i]]))
            continue;
        else
            return NULL;
    }
    
    return (Trace*) result->value;
}