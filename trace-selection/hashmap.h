/*
 * File: hashmap.h
 * Author: Robert William
 * Date: 2024-11-26
 * Description: Header file for the hashmap implementation.
 */

#ifndef HASHMAP_H
#define HASHMAP_H

#include <glib.h>
#include "datatype.h"  // Contains definitions for data_t, Trace, etc.

/**
 * struct trace_key_t:
 * @start_address: The start address of the trace.
 * @end_address: The end address of the trace.
 * @nb_blocks: The number of blocks in the trace.
 * @nb_instructions: The total number of instructions in the trace.
 * @block_sequence_hash: A hash representing the sequence of blocks in the trace.
 *
 * A comprehensive key that uniquely identifies a Trace. By combining start/end 
 * addresses, the number of blocks/instructions, and a hash of the block sequence, 
 * we ensure that two distinct traces cannot share the same key.
 */
typedef struct {
    guint64 start_address;
    guint64 end_address;
    guint   nb_blocks;
    guint   nb_instructions;
    guint   block_sequence_hash;
} trace_key_t;

/**
 * create_hash_table:
 *
 * Creates and returns a new GHashTable configured to store Trace structures
 * keyed by a trace_key_t.
 *
 * Returns: (transfer full): a new GHashTable for storing Trace structures.
 */
GHashTable* create_hash_table(void);

/**
 * insert_value:
 * @data: a pointer to data_t which contains the hash_table and block information.
 * @trace: a pointer to the Trace structure to be inserted.
 *
 * Inserts the given trace into the hash table. The trace is keyed by a newly 
 * constructed trace_key_t derived from the trace's attributes.
 */
void insert_value(data_t* data, Trace* trace);

/**
 * find_value:
 * @data: a pointer to data_t which contains the hash_table and block information.
 * @trace: a pointer to a Trace structure whose attributes define the search key.
 *
 * Attempts to find a trace in the hash table that matches the given trace's key 
 * attributes (start_address, end_address, nb_blocks, nb_instructions, and 
 * block_sequence_hash).
 *
 * Returns: (transfer none) (nullable): A pointer to the matching Trace if found, 
 * or NULL if not found.
 */
Trace* find_value(data_t* data, Trace* trace);

#endif // HASHMAP_H
