/*
 * File: main.c
 * Author: Robert William
 * Date: 2024-12-05
 * Description: This file contains the entry point for the project.
 * 
 */

#define _POSIX_C_SOURCE 199309L

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parser.h>
#include <hashmap.h>
#include <glib.h>
#include <trace-selection.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file> [-o <output_file>] [-O <hashmap_output_file>] [-pt] [-pb]\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *input_file = NULL;

    input_file = argv[1];

    char *output_file = NULL;
    char *hashmap_output_file = NULL;
    char flags = 0;

    // Parse command line arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (strcmp(argv[i], "-O") == 0 && i + 1 < argc) {
            hashmap_output_file = argv[++i];
        } else if (strcmp(argv[i], "-pt") == 0 && i + 1 < argc) {
            flags |= PRINT_TRACE;
        } else if (strcmp(argv[i], "-pb") == 0 && i + 1 < argc) {
            flags |= PRINT_BLOCK;
        } 
        
    }

    struct timespec start, end;
    // Record start time
    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        perror("clock_gettime start");
        return EXIT_FAILURE;
    }

    int fd = open(input_file, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    data_t data = {0};

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return EXIT_FAILURE;
    }
    data.file_size = st.st_size;

    data.mapped_file = mmap(NULL, data.file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (data.mapped_file == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

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

    if(output_file)
    FILE* output = fopen(output_file, "w");

    // Parse the trace
    unsigned int* index = malloc(sizeof(unsigned int)); // index of the next block
    Trace *trace = trace_parser(&data, index); // parse the first trace
    if (!trace) {
        fprintf(stderr, "Error parsing trace\n");
        return EXIT_FAILURE;
    }
    print_trace(&data, trace, PRINT_TRACE | PRINT_BLOCK); // print the first trace
    if (output_file) { fprintf(output, "%05d,", trace->id); }  
    //print_trace(&data, trace, PRINT_TRACE | PRINT_BLOCK); // print the first trace
    while(trace && *index < (data.file_size/9) && (data.blocks_p[trace->blocks_p[trace->nb_blocks-1]]->metadata & _EOF) == 0) // parse and print the next traces until the end of the file is reached
    {
        trace = trace_parser(&data, index);
        if (!trace) { 
            fprintf(stderr, "Error parsing trace\n");
            return EXIT_FAILURE;
        }
        //if(*index % 1000 < 20) printf("Index: %d\n", *index);
        if(flags) print_trace(&data, trace, flags);
        //print_trace(&data, trace, PRINT_TRACE);
        // write id as 6 numbers + ';' in output file
        if (output_file) { fprintf(output, "%05d,", trace->id); }  
    }
    if (output_file) { fclose(output); }

    // Print the last index
    printf("Index: %d\n", *index);
    // Print the number of blocks
    printf("Number of blocks: %d\n", data.nb_blocks);
    // Print the number of traces
    printf("Number of traces: %d\n", data.trace_count);

    // End --------------------
    
    // Parse the hash table and save the traces to a file
    // Create an output CSV file
    if(!hashmap_output_file) hashmap_output_file = "hashmap-output.csv";
    FILE *output_file = fopen(hashmap_output_file, "w");
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
    //fclose(file);

        // Record end time and compute elapsed
    if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
        perror("clock_gettime end");
        return EXIT_FAILURE;
    }

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1000000000.0;
    printf("Total runtime: %.3f seconds\n", elapsed);

    return EXIT_SUCCESS;
}
