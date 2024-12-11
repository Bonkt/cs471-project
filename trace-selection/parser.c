/*
 * File: parser.c
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the implementation of the parser for the project.
 * 
 */


#include "parser.h"

unsigned instructions_count = 0;
/*
inst_t parse_inst(data_t* data, unsigned int index) {
    inst_t inst = {0};
    if (index >= (data->file_size/9)) {
        inst.address = -1;
        return inst;
    }
    fseek(data->file, index * 9, SEEK_SET);
    fread(&inst, 1, 9, data->file);

    return inst;
}
*/

inst_t get_inst(data_t* data, unsigned int index) {
    inst_t inst;
    if (index >= data->file_size / 9) {
        inst.address = -1;
        return inst;
    }

    unsigned char *ptr = data->mapped_file + (index * 9);
    inst.address = *(long int*)ptr;
    inst.metadata = *(ptr + 8);

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
        // Print the index every 1 milion instructions
        
        if(*start_index % 1000000 == 0) printf("Index: %d\n", *start_index);

        inst = get_inst(data, *start_index); // parse the instruction
        (*start_index)++; // increment the index
        instructions_count++;
    } while (((inst.metadata & 0x03) == 0) && instructions_count < MAX_INSTRUCTIONS && inst.address != -1);

    block->end_index = *start_index - 1; // set the end index of the block

    // Precompute block start and end addresses here:
    inst_t start_inst = get_inst(data, block->start_index);
    inst_t end_inst = get_inst(data, block->end_index);
    block->start_address = start_inst.address;
    block->end_address = end_inst.address;

    // Precompute a block-specific hash. For simplicity, just hash start and end addresses:
    // You can use a simple mixing function:
    guint h = 0;
    h ^= (guint)block->start_address;
    h ^= (guint)(block->start_address >> 32);
    h ^= (guint)block->end_address;
    h ^= (guint)(block->end_address >> 32);
    // Simple mixing function (example):
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    block->block_hash = h;

    // check if block already exists
    for (size_t i = 0; i < data->nb_blocks; i++)
    {
        if (compare_block(block, data->blocks_p[i]))
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
        inst = get_inst(data, i);
        printf("Instruction index %3zu: Address: %lx, Metadata: %x\n", i, inst.address, inst.metadata);
    }
    printf("\n");
    return;
}

int compare_block(const block_t *a, const block_t *b) {
    if (a->start_address != b->start_address)
        return 0;
    if (a->end_address != b->end_address)
        return 0;
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