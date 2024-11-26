
#include <stdio.h>
#include <trace-selection/datatype.h>

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















