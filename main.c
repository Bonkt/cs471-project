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
#include <hashmap.h>
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

    data_t data = {0}; // initialize data structure
    data.file = file; // set file pointer in data structure

    fseek(file, 0, SEEK_END);  
    size_t size = ftell(file);
    rewind(file);
    printf("File size: %zu\n", size); 

    data.file_size = size; // set file size in data structure

    data.blocks_p = malloc(sizeof(block_t*) * MAX_BLOCKS); // allocate memory for blocks
    if (!data.blocks_p) {
        perror("Error allocating memory");
        return EXIT_FAILURE;
    }
    data.size = MAX_BLOCKS; // set size of blocks array in data structure
    data.nb_blocks = 0; // set number of blocks in data structure

    GHashTable *hash_table = create_hash_table();
    if (!hash_table) {
        perror("Error creating hash table");
        return EXIT_FAILURE;
    }
    data.hash_table = hash_table; // set hash table in data structure

    // Parse the trace
    unsigned int* index = malloc(sizeof(unsigned int)); // index of the next block
    Trace *trace = trace_parser(&data, index); // parse the first trace
    if (!trace) {
        fprintf(stderr, "Error parsing trace\n");
        return EXIT_FAILURE;
    }
    print_trace(&data, trace, PRINT_TRACE | PRINT_BLOCK); // print the first trace
    //print_trace(&data, trace, PRINT_TRACE | PRINT_BLOCK); // print the first trace
    while(trace && *index < (data.file_size/9) && (data.blocks_p[trace->blocks_p[trace->nb_blocks-1]]->metadata & _EOF) == 0) // parse and print the next traces until the end of the file is reached
    {
        trace = trace_parser(&data, index);
        if (!trace) { 
            fprintf(stderr, "Error parsing trace\n");
            return EXIT_FAILURE;
        }
        if(*index % 1000 < 20) printf("Index: %d\n", *index);
        print_trace(&data, trace, PRINT_TRACE);
    }

    // End --------------------
    fclose(file);
    return EXIT_SUCCESS;
}








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