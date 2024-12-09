/*
 * File: tree.h
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the header of the BTree for the project.
 * 
 */

#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <datatype.h>
#include <glib.h>
#include <trace-selection.h>

// Flags

#define PRINT_TRACE 0x01
#define PRINT_BLOCK 0x02

// Function prototypes

/**
 * @brief Compares two composite keys.
 * 
 * @param a The first key.
 * @param b The second key.
 * @param user_data The user data.
 * @return The comparison result.
 */
gint composite_key_compare(gconstpointer a, gconstpointer b, gpointer user_data);

/**
 * @brief Inserts a value in the BTree.
 * 
 * @param tree The BTree.
 * @param key1 The first key.
 * @param key2 The second key.
 * @param value The value.
 */
void insert_value(data_t* data, unsigned int key1, unsigned int key2, Trace* value);

/**
 * @brief Finds a value in the BTree.
 * 
 * @param tree The BTree.
 * @param key1 The first key.
 * @param key2 The second key.
 * @return The value found.
 */
Trace* find_value(data_t* data, unsigned int key1, unsigned int key2);

/**
 * @brief Removes a value from the BTree.
 * 
 * @param tree The BTree.
 * @param key1 The first key.
 * @param key2 The second key.
 */
void remove_value(data_t* data, unsigned int key1, unsigned int key2);

/**
 * @brief Creates a new BTree.
 * 
 * @return The new BTree.
 */
GTree* create_tree();

#endif