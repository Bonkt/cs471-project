/*
 * File: trace-selection.c
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the implementation of the trace selection for the project.
 */

#include <trace-selection.h>

void print_trace(data_t* data, Trace *trace, size_t FLAGS) {
    if (FLAGS & PRINT_TRACE) {
        printf("Trace: %d,   %d blocks,    %d instructions,    start address: 0x%llx,    end address: 0x%llx, index: %d\n",
               trace->id, trace->nb_blocks, trace->nb_instructions, trace->start_address, trace->end_address, data->nb_blocks);
        }
    
    if (FLAGS & PRINT_BLOCK) {
        for (unsigned int i = 0; i < trace->nb_blocks; i++) {
            print_block(data, trace->blocks_p[i]);
        }
    }
}

/*
 * Approach 2: Once we find a candidate trace using find_value, we know it's exact.
 * No need to re-check nb_blocks, nb_instructions, or block sequences here.
 * The hashing and equality checks in the hash table already guarantee uniqueness.
 */

Trace* trace_parser(data_t* data, unsigned int* start_index) {
    unsigned int nb_blocks = 0;
    unsigned int* blocks = parse_block_terminating(data, start_index, &nb_blocks);
    if (!blocks) {
        perror("Error parsing blocks");
        return NULL;
    }

    Trace* trace = trace_builder(data, blocks, nb_blocks, *start_index);
    if(!trace) {
        perror("Error building trace");
        return NULL;
    }

    // Now we use the find_trace function which fully relies on approach 2:
    Trace* lookup = find_trace(data, trace);
    if (lookup == NULL) {
        // Trace not found, insert it
        insert_trace(data, trace);
        return trace;
    } else {
        // Trace already exists, free the newly built one and update the existing one
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
    trace->nb_instructions = count_inst(data, blocks, size);
    trace->start_address = parse_inst(data, data->blocks_p[blocks[0]]->start_index).address;
    trace->end_address = parse_inst(data, data->blocks_p[blocks[size - 1]]->end_index).address;
    trace->blocks_p = blocks;
    trace->reuse = 1;
    trace->distance = 0;
    trace->last_used = start_index;
    return trace;
}

void insert_trace(data_t* data, Trace* trace) {
    trace->id = data->trace_count++;
    // Approach 2: insert_value now takes the entire Trace and computes its unique key internally.
    insert_value(data, trace);
}

Trace* find_trace(data_t* data, Trace* trace) {
    // With approach 2, if find_value returns non-NULL, it's guaranteed identical.
    Trace* lookup = find_value(data, trace);
    /*if(!lookup)
    {
        return NULL;
    }
    if(data->trace_count < 50){
        print_trace(data, lookup, PRINT_TRACE | PRINT_BLOCK);
        print_trace(data, trace, PRINT_TRACE | PRINT_BLOCK);
    }
    */
    return lookup;
}

void update_trace(Trace* trace, unsigned int index) {
    trace->distance += index - trace->last_used;
    trace->last_used = index;
    trace->reuse++;
}

void free_trace(Trace* trace) {
    free(trace->blocks_p);
    free(trace);
}
