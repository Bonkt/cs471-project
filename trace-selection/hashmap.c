/*
 * File: hashmap.c
 * Description: Implementation of a hash table approach that uses a comprehensive key 
 *              to uniquely identify traces. By including the block sequence hash 
 *              and other attributes, we ensure direct equality checks at the hash table level.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "hashmap.h"
#include "datatype.h"  // Ensure this includes the definition of data_t, Trace, etc.

// Hash the composite key
static guint hash_key(gconstpointer key) {
    const trace_key_t* k = (const trace_key_t*)key;

    // Hash individual components
    guint start_hash = g_int_hash((gconstpointer)&k->start_address);
    guint end_hash   = g_int_hash((gconstpointer)&k->end_address);
    guint nb_blocks_hash = g_int_hash((gconstpointer)&k->nb_blocks);
    guint nb_instr_hash  = g_int_hash((gconstpointer)&k->nb_instructions);

    // Combine them into a single hash value
    guint h = 0;
    h ^= start_hash;
    h ^= (end_hash << 1);
    h ^= (nb_blocks_hash << 2);
    h ^= (nb_instr_hash << 3);
    h ^= (k->block_sequence_hash << 4);

    return h;
}

// Compare two keys for equality
static gboolean hash_equal(gconstpointer a, gconstpointer b) {
    const trace_key_t* ka = (const trace_key_t*)a;
    const trace_key_t* kb = (const trace_key_t*)b;

    return (ka->start_address == kb->start_address) &&
           (ka->end_address == kb->end_address) &&
           (ka->nb_blocks == kb->nb_blocks) &&
           (ka->nb_instructions == kb->nb_instructions) &&
           (ka->block_sequence_hash == kb->block_sequence_hash);
}

// Frees the key when the entry is removed or the table is destroyed
static void key_free_func(gpointer data) {
    g_free(data);
}

// We assume that Trace* should be freed elsewhere if needed.
// If you want the hash table to own the Trace memory, provide a value_free_func.
GHashTable* create_hash_table(void) {
    return g_hash_table_new_full(hash_key, hash_equal, key_free_func, NULL);
}

// Compute the block sequence hash for a given trace
static guint compute_block_sequence_hash(data_t* data, Trace* trace) {
    guint block_sequence_hash = 0;
    for (unsigned int i = 0; i < trace->nb_blocks; i++) {
        // Get the block indices
        size_t s = data->blocks_p[trace->blocks_p[i]]->start_index;
        size_t e = data->blocks_p[trace->blocks_p[i]]->end_index;
        // Combine them into the hash
        block_sequence_hash ^= g_int_hash(&s);
        block_sequence_hash ^= (g_int_hash(&e) << 1);
    }
    return block_sequence_hash;
}

// Create a key for the given trace
static trace_key_t* create_trace_key(data_t* data, Trace* trace) {
    trace_key_t* key = g_new(trace_key_t, 1);
    key->start_address = trace->start_address;
    key->end_address = trace->end_address;
    key->nb_blocks = trace->nb_blocks;
    key->nb_instructions = trace->nb_instructions;
    key->block_sequence_hash = compute_block_sequence_hash(data, trace);

    return key;
}

// Insert a trace into the hash table
void insert_value(data_t* data, Trace* trace) {
    if (!data || !trace) {
        fprintf(stderr, "Invalid data or trace provided to insert_value.\n");
        return;
    }

    trace_key_t* key = create_trace_key(data, trace);
    g_hash_table_insert(data->hash_table, key, trace);
}

// Find a trace by its attributes
Trace* find_value(data_t* data, Trace* trace) {
    if (!data || !trace) {
        fprintf(stderr, "Invalid data or trace provided to find_value.\n");
        return NULL;
    }

    trace_key_t temp_key = {
        .start_address = trace->start_address,
        .end_address = trace->end_address,
        .nb_blocks = trace->nb_blocks,
        .nb_instructions = trace->nb_instructions,
        .block_sequence_hash = compute_block_sequence_hash(data, trace)
    };

    return g_hash_table_lookup(data->hash_table, &temp_key);
}
