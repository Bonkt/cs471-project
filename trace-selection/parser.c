/*
 * File: parser.c
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the implementation of the parser for the project.
 * 
 */


#include "parser.h"

inst_t parse_inst(FILE *file, int index) {
    inst_t inst = {0};
    fseek(file, index * 9, SEEK_SET);
    fread(&inst, 1, 9, file);
    
    return inst;
}

block_t *parse_block(FILE *file, int start_index, int remaining) {
    // Allocate memory for the block
    block_t *block = malloc(sizeof(block_t));
    if (!block) {
        perror("Error allocating memory");
        return NULL;
    }
    block->start_index = start_index;
    inst_t inst = {0};
    inst = parse_inst(file, start_index);
    while ((inst.metadata & 0x03) == 0 && ((start_index - block->start_index + 1) < remaining))
    {
        inst = parse_inst(file, ++start_index);
    }
    block->end_index = start_index;
    if (block->end_index - block->start_index == remaining)
    {
        block->metadata = 0x02;
    }
    else
    {
        block->metadata = inst.metadata & 0x03;
    }
    return block;
}

block_t** parse_block_terminating(FILE *file, int start_index, int remaining, int* size_o) {
    // Allocate memory for the block
    block_t **blocks = malloc(sizeof(block_t*) * MAX_BLOCKS);
    if (!blocks) {
        perror("Error allocating memory");
        return NULL;
    }
    size_t i = 0;
    size_t curr_index = start_index;
    blocks[i] = parse_block(file, curr_index, MAX_INSTRUCTIONS);
    curr_index = blocks[i]->end_index + 1;
    while((blocks[i]->metadata & 0x02) == 0) {
        blocks[++i] = parse_block(file, curr_index + 1, MAX_INSTRUCTIONS);
        curr_index = blocks[i]->end_index + 1;
    }
    *size_o = i;
    /* if we want to optimize memory usage, we can realloc the blocks array to the correct size
    // currently losing up to 8Kb of memory for each trace, way too much
    blocks = realloc(blocks, sizeof(block_t*) * (i + 1));
    if (!blocks) {
        perror("Error reallocating memory");
        return NULL;
    }
    */ 
    return blocks;
}

void free_blocks(block_t** blocks, size_t size) {
    for (size_t i = 0; i < size; i++)
    {
        free(blocks[i]);
    }
    free(blocks);
    return;
}

void print_block(FILE* file, block_t *block) {
    printf("Block: [%zu, %zu], Metadata: %d\n", block->start_index, block->end_index, block->metadata);
    inst_t inst = {0};
    for (size_t i = block->start_index; i <= block->end_index; i++)
    {
        inst = parse_inst(file, i);
        printf("Instruction index %3zu: Address: %lx, Metadata: %x\n", i, inst.address, inst.metadata);
    }
    return;
}