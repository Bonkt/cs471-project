/*
 * File: tree.h
 * Author: Robert William
 * Date: 2024-11-26
 * Description: This file contains the header of the BTree for the project.
 * 
 */

#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <datatype.h>
#include <glib.h>
#include <trace-selection.h>

// Function prototypes

/**
    * @brief Creates a new tree.
    * 
    * @param data The global data structure.
    * @return The new tree.
*/
GHashTable* create_hash_table(void);

/**
    * @brief Hashes the key.
    * 
    * @param start_address The start address of the trace.
    * @param end_address The end address of the trace.
    * @return The hash of the key.
*/
unsigned int hash_key(unsigned int start_address, unsigned int end_address);

/**
    * @brief Finds a trace in the hashmap.
    * 
    * @param data The global data structure.
    * @param trace The trace to find.
    * @return The trace found.
*/
Trace* find_value(data_t* data, unsigned int start_address, unsigned int end_address);


/**
    * @brief Inserts a trace into the hashmap.
    * 
    * @param data The global data structure.
    * @param trace The trace to insert.
*/
void insert_value(data_t* data, unsigned int start_address, unsigned int end_address, Trace* trace);

#endif