/*
 * File: parser.c
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the implementation of the parser for the project.
 * 
 */


#include "parser.h"

inst_t parse_inst(data_t* data, int index) {
    inst_t inst = {0};
    fseek(data->file, index * 9, SEEK_SET);
    fread(&inst, 1, 9, data->file);
    
    return inst;
}

unsigned int parse_block(data_t* data, unsigned int* start_index, unsigned int* remaining) {
    // Allocate memory for the block
    block_t *block = malloc(sizeof(block_t));
    if (!block) {
        perror("Error allocating memory");
        return -1;
    }
    block->start_index = *start_index;
    inst_t inst = {0};
    inst = parse_inst(data, *start_index);
    while (((inst.metadata & 0x03) == 0) && ((*start_index - block->start_index + 1) <= *remaining))
    {
        *start_index += 1;
        inst = parse_inst(data, *start_index);
        *remaining -= 1;
    }
    block->end_index = *start_index;
    *start_index += 1;

    // check if block already exists
    for (size_t i = 0; i < data->nb_blocks; i++)
    {
        if (compare_block(data, block, data->blocks_p[i]))
        {
            free(block);
            return i;
        }
    }

    // Set the metadata of the block if the block does not exist
    if (*remaining == 0)
    {
        block->metadata = 0x02;
    }
    else
    {
        block->metadata = inst.metadata & 0x03;
    }

    // Add the block to the data structure
    data->blocks_p[data->nb_blocks++] = block;
    return data->nb_blocks - 1;
}

unsigned int* parse_block_terminating(data_t* data, unsigned int* start_index, unsigned int* remaining, unsigned int* size_o) {
    // Allocate memory for the block
    unsigned int *blocks = malloc(sizeof(unsigned int) * MAX_BLOCKS);
    if (!blocks) {
        perror("Error allocating memory");
        return NULL;
    }
    size_t i = 0;
    //size_t curr_index = *start_index;
    blocks[i++] = parse_block(data, start_index, remaining);
    //curr_index = blocks[i++]->end_index + 1;
    while((data->blocks_p[blocks[i-1]]->metadata & 0x02) == 0) {
        blocks[i++] = parse_block(data, start_index, remaining);
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

// Shouldn't be used I think... remnant of a previous implementation
void free_blocks(block_t** blocks, size_t size) {
    for (size_t i = 0; i < size; i++)
    {
        free(blocks[i]);
    }
    free(blocks);
    return;
}

void print_block(data_t* data, unsigned int block) {
    block_t *block_p = data->blocks_p[block];
    printf("Block: [%zu, %zu], Metadata: %d, Block index: %d\n", block_p->start_index,block_p->end_index, block_p->metadata, block);
    inst_t inst = {0};
    for (size_t i = block_p->start_index; i <= block_p->end_index; i++)
    {
        inst = parse_inst(data, i);
        printf("Instruction index %3zu: Address: %lx, Metadata: %x\n", i, inst.address, inst.metadata);
    }
    printf("\n");
    return;
}

int compare_block(data_t* data, const block_t *a, const block_t *b) {
    inst_t inst_a = parse_inst(data, a->start_index);
    inst_t inst_b = parse_inst(data, b->start_index);
    if (inst_a.address != inst_b.address)
    {
        return 0;
    }
    inst_a = parse_inst(data, a->end_index);
    inst_b = parse_inst(data, b->end_index);
    if (inst_a.address != inst_b.address)
    {
        return 0;
    }
    return 1;
}

int realloc_blocks(data_t* data) {
    unsigned int new_size = data->size << 1;
    if(new_size < data->size) {
        perror("Error reallocating memory");
        return -1;
    }
    // check if new_size * sizeof(block_t*) is too big
    if(new_size * sizeof(block_t*) < new_size) {
        perror("Error reallocating memory");
        return -1;
    }
    block_t** new_blocks = realloc(data->blocks_p, sizeof(block_t*) * new_size);
    if (!new_blocks) {
        perror("Error reallocating memory");
        return -1;
    }
    data->blocks_p = new_blocks;
    data->size = new_size;
    return data->size;
}