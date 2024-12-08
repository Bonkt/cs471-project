/*
 * File: trace-selection.h
 * Author: Robert William, Gabin Forestier
 * Date: 2024-11-26
 * Description: This file contains the header for the trace selection for the project.
 * 
 */

#ifndef TRACE_H
#define TRACE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parser.h>
#include <glib.h>
#include <datatype.h>

// Traces structure
typedef struct {
    // ID of the trace
    unsigned int id;
    // Number of blocks in the trace
    unsigned int nb_blocks;
    // Number of instructions in the trace
    unsigned int nb_instructions;
    // Start address of the trace
    unsigned long long start_address;
    // End address of the trace
    unsigned long long end_address;
    // Array of pointers to the blocks in the trace
    unsigned int* blocks_p;
    // Number of reuse
    size_t reuse;
    // Distance
    long int distance;
    // index of the last execution
    size_t last_used;
} Trace;

typedef Trace* Map;


/* How to identify a trace
    - Look for the first instruction of the trace
    - If the instruction is not in the trace list, create a new trace
    - If the instruction is in the trace list, continue reading the trace
    - At the end of the trace, look at the blocks in the trace
    - If all block start address and end address are the same
*/

// Function prototypes

/**
    * @brief Prints the trace to the console.
    * 
    * @param data The global data structure.
    * @param trace The trace to print.
    * @param FLAGS The flags to determine what to print.
*/
void print_trace(data_t* data, Trace *trace, size_t FLAGS);


/**
    * @brief Parses a trace from the file.
    * 
    * @param data The global data structure.
    * @param start_index The index of the first instruction in the trace. Updated for the next trace.
    * @return The trace read from the file.
*/
Trace* trace_parser(data_t* data, unsigned int* start_index);

/**
    * @brief Builds a trace from the blocks.
    * 
    * @param data The global data structure.
    * @param blocks The blocks of the trace.
    * @param size The size of the blocks array.
    * @return The trace built from the blocks.
 */
Trace* trace_builder(data_t* data, unsigned int* blocks, unsigned int size, unsigned int start_index);


/**
    * @brief Inserts a trace into the tree.
    * 
    * @param data The global data structure.
    * @param trace The trace to insert.
 */
void insert_trace(data_t* data, Trace* trace);


/**
    * @brief Finds a trace in the tree.
    * 
    * @param data The global data structure.
    * @param trace The trace to find.
    * @return NULL if the trace is not found, the trace otherwise.
 */
Trace* find_trace(data_t* data, Trace* trace);


/**
    * @brief Increment a trace reuse count and updates the distance
    * 
    * @param trace The trace to find.
    * @param start_index Current index of the program
 */
void update_trace(Trace* trace, unsigned int start_index);


/**
    * @brief Free a trace and its block list
    * 
    * @param trace The trace to find.
 */
void free_trace(Trace* trace);

#endif