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
#include <tree.h>
#include <glib.h>
#include <trace-selection.h>

// Your code here
int main(int argc, char *argv[]) {
    /*if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }*/
    argc = argc;
    argv = argv;

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

    data_t data = {0};
    data.file = file;
    data.blocks_p = malloc(sizeof(block_t*) * MAX_BLOCKS);
    if (!data.blocks_p) {
        perror("Error allocating memory");
        return EXIT_FAILURE;
    }
    data.size = MAX_BLOCKS;
    data.nb_blocks = 0;
    GTree *tree = create_tree(data);
    if (!tree) {
        perror("Error creating tree");
        return EXIT_FAILURE;
    }
    data.tree = tree;
/*
    // Parse the trace
    unsigned int index = 0;
    Trace *trace = NULL;
    for (size_t i = 0; i < 5; i++)
    {
        trace = trace_parser(&data, &index);
        if (!trace) {
            fprintf(stderr, "Error parsing trace\n");
            return EXIT_FAILURE;
        }
        print_trace(&data, trace, PRINT_TRACE | PRINT_BLOCK);
    }
    free(trace);
*/
    // Parse the trace
    unsigned int index = 0;
    Trace *trace = trace_parser(&data, &index);
    print_trace(&data, trace, PRINT_TRACE | PRINT_BLOCK);
    while(trace)
    {
        trace = trace_parser(&data, &index);
        print_trace(&data, trace, PRINT_TRACE);
    }
    free(trace);

    // End --------------------
    fclose(file);
    return EXIT_SUCCESS;
}