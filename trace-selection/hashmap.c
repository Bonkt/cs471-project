/*
 * File: hashmap.c
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the implementation of the hash table for the project.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "hashmap.h"


GHashTable* create_hash_table(void) {
    return g_hash_table_new(hash_key, hash_equal);
}

guint hash_key(gconstpointer start_address, gconstpointer end_address) {
    return *(unsigned int*)start_address + *(unsigned int*)end_address;
}

gboolean hash_equal(gconstpointer key1, gconstpointer key2) {
    return *(guint*)key1 == *(guint*)key2;
}

Trace* find_value(data_t* data, unsigned int start_address, unsigned int end_address) {
    guint key = hash_key(&start_address, &end_address);
    hash_t_value* value = g_hash_table_lookup(data->hash_table, &key);
    if (value) {
        return value->value;
    }
    return NULL;
}

void insert_value(data_t* data, unsigned int start_address, unsigned int end_address, Trace* trace) {
    hash_t_value* value = malloc(sizeof(hash_t_value));
    if (!value) {
        perror("Error allocating memory");
        return;
    }
    value->key = hash_key(&start_address, &end_address);
    value->value = trace;
    g_hash_table_insert(data->hash_table, &value->key, value);
}