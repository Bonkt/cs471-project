/*
 * File: parser.h
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the header of the parser for the project.
 * 
 */

#include <stdio.h>
#include <datatype.h>

/**
 * Parses an instruction from the file.
 * 
 * @param file The file to read from.
 * @param index The index of the instruction to read.
 * @return The instruction read from the file.
 */
inst_t parse_inst(FILE* file, int index);

/**
 * Parses a block of instructions from the file.
 * 
 * @param file The file to read from.
 * @param start_index The index of the first instruction in the block.
 * @return The block of instructions read from the file.
 * @note The caller is responsible for freeing the memory allocated by this function.
 */
block_t *parse_block(FILE* file, int start_index, int remaining);


/**
 * Parses multiple blocks of instructions from the file until a terminating block is found.
 * 
 * @param file The file to read from.
 * @param start_index The index of the first instruction in the block.
 * @param remaining The number of instructions remaining in the trace.
 * @param size_o The size of the block.
 * @return The block of instructions read from the file.
 * @note The caller is responsible for freeing the memory allocated by this function.
 */
block_t** parse_block_terminating(FILE* file, int start_index, int remaining, size_t* size_o);

void free_blocks(block_t** blocks, size_t size);

/**
 * Prints the block of instructions to the console.
 * 
 * @param file The file to read from.
 * @param block The block of instructions to print.
 */
void print_block(FILE* file, block_t *block);















