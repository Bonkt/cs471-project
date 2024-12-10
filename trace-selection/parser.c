/*
 * File: parser.c
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the implementation of the parser for the project.
 * 
 */


#include "parser.h"

unsigned instructions_count = 0;

inst_t parse_inst(data_t* data, unsigned int index) {
    inst_t inst = {0};
    if (index >= (data->file_size/9)) {
        inst.address = -1;
        return inst;
    }
    fseek(data->file, index * 9, SEEK_SET);
    fread(&inst, 1, 9, data->file);

    /*
    
    if(fseek(data->file, index * 9, SEEK_SET))
    {
        inst.address = -1;
        return inst;
    }
    if(fread(&inst, 1, 9, data->file) != 9)
    {
        if(feof(data->file)) printf("Reached EOF\n");
        else printf("Error while reading inst at index : %d\n", index);
        inst.address = -1;
        return inst;
    }
    */

    return inst;
}

unsigned int parse_block(data_t* data, unsigned int* start_index) {
    // Allocate memory for the block
    block_t *block = malloc(sizeof(block_t));
    if (!block) {
        perror("Error allocating memory");
        return -1;
    }
    block->start_index = *start_index;
    inst_t inst = {0};
    do {
        inst = parse_inst(data, *start_index); // parse the instruction
        (*start_index)++; // increment the index
        instructions_count++;
    } while (((inst.metadata & 0x03) == 0) && instructions_count < MAX_INSTRUCTIONS && inst.address != -1);

    block->end_index = *start_index - 1; // set the end index of the block

    // check if block already exists
    for (size_t i = 0; i < data->nb_blocks; i++)
    {
        if (compare_block(data, block, data->blocks_p[i]))
        {
            free(block);
            // update the metadata of the block if the block already exists
            if (instructions_count == MAX_INSTRUCTIONS)
            {
                data->blocks_p[i]->metadata |= TERMINATING;
                return i;
            }
            else if (inst.address == -1)
            {
                data->blocks_p[i]->metadata |= _EOF;
                return i;
            }
            else
            {
                data->blocks_p[i]->metadata = inst.metadata & 0x03;
                return i;
            }
        }
    }

    // Set the metadata of the block if the block does not exist
    if (instructions_count == MAX_INSTRUCTIONS)
    {
        block->metadata |= TERMINATING;
    }
    else if (inst.address == -1)
    {
        block->metadata |= _EOF;
    }
    else
    {
        block->metadata = inst.metadata & 0x03;
    }

    // Add the block to the data structure
    insert_block(data, block);
    return data->nb_blocks - 1;
}

unsigned int* parse_block_terminating(data_t* data, unsigned int* start_index, unsigned int* size_o) {
    // Allocate memory for the block
    unsigned int *blocks = malloc(sizeof(unsigned int) * MAX_BLOCKS);
    if (!blocks) {
        perror("Error allocating memory");
        return NULL;
    }

    size_t i = 0;
    instructions_count = 0;
    do {
        blocks[i++] = parse_block(data, start_index);
        if(data->blocks_p[blocks[i-1]]->metadata & 0x04) {
            *size_o = i;
            return blocks;
        }
    } while (i < MAX_BLOCKS && (data->blocks_p[blocks[i-1]]->metadata & 0x06) == 0);
    *size_o = i; // set the size of the blocks
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

void insert_block(data_t* data, block_t* block) {
    if(data->nb_blocks == data->size) {
        if(realloc_blocks(data) == -1) {
            return;
        }
    }
    data->blocks_p[data->nb_blocks++] = block;
    return;
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

unsigned int count_inst(data_t* data, unsigned int* blocks, size_t size)
{
    size_t count = 0;
    for(size_t i = 0; i < size; i++)
    {
        block_t* block = data->blocks_p[blocks[i]];
        count+= block->end_index - block->start_index + 1;
    }
    return count;
}