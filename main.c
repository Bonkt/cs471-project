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

// Comparison function for sorting by trace->id
static gint compare_trace_ids(gconstpointer a, gconstpointer b) {
    const Trace *t1 = (const Trace*)a;
    const Trace *t2 = (const Trace*)b;

    if (t1->id < t2->id)
        return -1;
    else if (t1->id > t2->id)
        return 1;
    else
        return 0;
}

// Your code here
int main(int argc, char *argv[]) {
    /*if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }*/
    argc = argc;
    argv = argv;

    FILE *file = fopen("processed_trace2", "r");
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
    //FILE* output = fopen("output2.csv", "w");
    while(trace && *index < 100000 && (data.blocks_p[trace->blocks_p[trace->nb_blocks-1]]->metadata & _EOF) == 0) // parse and print the next traces until the end of the file is reached
    {
        trace = trace_parser(&data, index);
        if (!trace) { 
            fprintf(stderr, "Error parsing trace\n");
            return EXIT_FAILURE;
        }
        if(*index % 1000 < 20) printf("Index: %d\n", *index);
        print_trace(&data, trace, PRINT_TRACE);
        // write id as 6 numbers + ';' in output file
        //fprintf(output, "%6d,", trace->id);
    }

    // End --------------------
    
    // Parse the hash table and save the traces to a file
    // Create an output CSV file
    /*
    FILE *output_file = fopen("hashmap-output2.csv", "w");
    if (!output_file) {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }

    // Start by iterating over the hash table to gather all traces into a list
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, data.hash_table);

    GList *trace_list = NULL;
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        trace_list = g_list_prepend(trace_list, value);
    }

    // Reverse the list because we used prepend (optional but keeps order consistent)
    trace_list = g_list_reverse(trace_list);

    // Now sort the list by the trace->id
    trace_list = g_list_sort(trace_list, compare_trace_ids);

    // Write the CSV header
    fprintf(output_file, "id,nb_blocks,nb_instructions,reuse,distance\n");

    // Iterate over the sorted list and print the traces
    for (GList *l = trace_list; l != NULL; l = l->next) {
        Trace *trace = (Trace *)l->data;
        fprintf(output_file, "%u,%u,%u,%lu,%lu\n",
            trace->id,
            trace->nb_blocks,
            trace->nb_instructions,
            trace->reuse,
            trace->distance
        );
    }

    // Cleanup the list (optional, depending on program lifecycle)
    g_list_free(trace_list);

    // Close the output file
    fclose(output_file);
    */

    
    fclose(file);
    return EXIT_SUCCESS;
}
