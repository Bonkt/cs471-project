/*
 * File: parser.h
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the header of the parser for the project.
 * 
 */
#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <glib.h>
#include <datatype.h>

/**
 * Parses an instruction from the file.
 * 
 * @param data The global data structure.
 * @param index The index of the instruction to read.
 * @return The instruction read from the file.
 * 
 * @exception On reading error, write -1 in the instruction's address.
 */
inst_t parse_inst(data_t* data, unsigned int index);

/**
 * Parses a block of instructions from the file.
 * 
 * @param data The global data structure.
 * @param start_index The index of the first instruction in the block.
 * @return The block of instructions read from the file.
 * @note The caller is responsible for freeing the memory allocated by this function.
 */
unsigned int parse_block(data_t* data, unsigned int* start_index, unsigned int* remaining);


/**
 * Parses multiple blocks of instructions from the file until a terminating block is found.
 * 
 * @param data The global data structure.
 * @param start_index The index of the first instruction in the block.
 * @param remaining The number of instructions remaining in the trace.
 * @param size_o The size of the block.
 * @return The block of instructions read from the file.
 * @note The caller is responsible for freeing the memory allocated by this function.
 */
unsigned int* parse_block_terminating(data_t* data, unsigned int* start_index, unsigned int* remaining, unsigned int* size_o);

// Shouldn't be used I think... remnant of a previous implementation
void free_blocks(block_t** blocks, size_t size);

/**
 * Prints the block of instructions to the console.
 * 
 * @param data The global data structure.
 * @param block The block of instructions to print.
 */
void print_block(data_t* data, unsigned int block);

/**
 * Compares two blocks of instructions.
 * 
 * @param data The global data structure.
 * @param a The first block of instructions.
 * @param b The second block of instructions.
 * @return 1 if the blocks are equal, 0 otherwise.
 */
int compare_block(data_t* data, const block_t *a, const block_t *b);

void insert_block(data_t* data, block_t* block);

/**
 * Reallocates the memory for the blocks array.
 * 
 * @param data The global data structure.
 * @return new size if successful, -1 otherwise.
 */
int realloc_blocks(data_t* data);

unsigned int count_inst(data_t* data, unsigned int* blocks, size_t size);

#endif













