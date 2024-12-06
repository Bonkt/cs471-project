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
#include <glib.h>
#include <trace-selection.h>

gint composite_key_compare(gconstpointer a, gconstpointer b, gpointer user_data) {
    user_data = user_data; // to remove warning, currently holding data_t to reference the address, maybe... This is faster but less precise
    const int *key_a = a;
    const int *key_b = b;

    // Compare first key
    if (key_a[0] < key_b[0])
        return -1;
    if (key_a[0] > key_b[0])
        return 1;

    // Compare second key if first keys are equal
    if (key_a[1] < key_b[1])
        return -1;
    if (key_a[1] > key_b[1])
        return 1;

    return 0; // Keys are equal
}

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
    GTree *tree = g_tree_new_full(composite_key_compare, (gpointer)&data, free, free);
    if (!tree) {
        perror("Error creating tree");
        return EXIT_FAILURE;
    }
    data.tree = tree;

    // Parse the trace
    unsigned int index = 0;
    Trace *trace = NULL;
    for (size_t i = 0; i < 5; i++)
    {
        trace = trace_selection(&data, &index);
        if (!trace) {
            fprintf(stderr, "Error parsing trace\n");
            return EXIT_FAILURE;
        }
        print_trace(&data, trace, PRINT_TRACE | PRINT_BLOCK);
    }
    free(trace);

    // End --------------------
    fclose(file);
    return EXIT_SUCCESS;
}