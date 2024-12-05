/*
 * File: main.c
 * Author: Robert William
 * Date: 2024-12-05
 * Description: This file contains the entry point for the project.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parser.h>
#include <trace-selection.h>


// Your code here
int main(int argc, char *argv[]) {
    /*if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }*/

    FILE *file = fopen("processed_trace", "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    // Start ------------------

    //parse one block and print
    /*
    int index = 0;
    int start_index = 0;
    int remaining = MAX_INSTRUCTIONS;
    for (size_t i = 0; i < 5; i++)
    {
        block_t **block = parse_block_terminating(file, &start_index, &remaining, &index);
        if (!block) {
            fprintf(stderr, "Error parsing block\n");
            return EXIT_FAILURE;
        }
        printf("Index: %d\n", index);
        for (size_t i = 0; i < index; i++)
        {
            print_block(file, block[i]);
        }
        free_blocks(block, index);
    }
    */
    

    // Parse the trace
    int index = 0;
    Trace *trace = NULL;
    for (size_t i = 0; i < 3; i++)
    {
        trace = trace_selection(file, &index);
        if (!trace) {
            fprintf(stderr, "Error parsing trace\n");
            return EXIT_FAILURE;
        }
        print_trace(file, trace, PRINT_TRACE | PRINT_BLOCK);
    }
    free(trace);

    // End --------------------
    fclose(file);
    return EXIT_SUCCESS;
}