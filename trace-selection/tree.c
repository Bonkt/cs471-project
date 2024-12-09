/*
 * File: tree.c
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the implementation of the BTree for the project.
 * 
 */

#include <tree.h>

// Function to compare two composite keys
gint composite_key_compare(gconstpointer a, gconstpointer b, gpointer user_data) {
    data_t* data = (data_t*)user_data;
    data = data; // remove warning, useless for now, might change later
    unsigned int key1_a = ((BTreeValue*)a)->key1;
    unsigned int key2_a = ((BTreeValue*)a)->key2;
    unsigned int key1_b = ((BTreeValue*)b)->key1;
    unsigned int key2_b = ((BTreeValue*)b)->key2;
    if (key1_a < key1_b) {
        return -1;
    } else if (key1_a > key1_b) {
        return 1;
    } else {
        if (key2_a < key2_b) {
            return -1;
        } else if (key2_a > key2_b) {
            return 1;
        } else {
            return 0;
        }
    }
}

// Function to insert a value in the BTree
void insert_value(data_t* data, unsigned int key1, unsigned int key2, Trace* value) {
    BTreeValue *btree_value = malloc(sizeof(BTreeValue));
    if (!btree_value) {
        perror("Error allocating memory");
        return;
    }
    btree_value->key1 = key1;
    btree_value->key2 = key2;
    btree_value->value = (char*) value;
    g_tree_insert(data->tree, btree_value, btree_value);
}

// Function to find a value in the BTree
Trace* find_value(data_t* data, unsigned int key1, unsigned int key2) {
    BTreeValue btree_value;
    btree_value.key1 = key1;
    btree_value.key2 = key2;
    btree_value.value = NULL; // Ensure all fields are initialized
    BTreeValue* value = g_tree_lookup(data->tree, &btree_value);
    if (!value) {
        return NULL;
    }
    return (Trace*) value->value;
}

// Function to remove a value from the BTree
void remove_value(data_t* data, unsigned int key1, unsigned int key2) {
    BTreeValue btree_value = {0};
    btree_value.key1 = key1;
    btree_value.key2 = key2;
    g_tree_remove(data->tree, &btree_value);
}

// Function to create a new BTree
GTree* create_tree(data_t* data) {
    return g_tree_new_full(composite_key_compare, data, free, free);
}