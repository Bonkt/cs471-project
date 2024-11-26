/*
 * File: parser.c
 * Author: Your Name
 * Date: YYYY-MM-DD
 * Description: This file contains the implementation of the parser for the project.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trace-selection/datatype.h"
#include "trace-selection/parser.h"

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
    short length = 0;
    inst_t inst = {0};
    do
    {
        inst = parse_inst(file, start_index);
        if (inst.metadata & 0x01 == 0)
        {
            block->end_index = start_index;
        }
        start_index++;
        length++;
    } while (inst.metadata & 0x01 == 0 && length < remaining);
    if (length == remaining)
    {
        block->metadata = 0x02;
    }
    else
    {
        block->metadata = 0x00;
    }
    
    return block;
}

void print_block(FILE* file, block_t *block) {
    printf("Block: [%zu, %zu], Metadata: %d\n", block->start_index, block->end_index, block->metadata);
    inst_t inst = {0};
    for (size_t i = block->start_index; i <= block->end_index; i++)
    {
        inst = parse_inst(file, i);
        printf("Instruction %zu: Address: %ld, Metadata: %d\n", i, inst.address, inst.metadata);
    }
    return;
}