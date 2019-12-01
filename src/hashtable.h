//
// Created by kalu on 11/26/19.
//

#ifndef SNAPPY_HASHTABLE_H
#define SNAPPY_HASHTABLE_H

#include <stdlib.h>
#include <string.h>
#include "utilityfuncs.h"
#include "avl_tree.h"
#include <stdio.h>

typedef struct hash_entry_type {
    avlnode *root_ptr;
} hash_entry;

/*
* @param src_size
*      note that this will be choosen from the minimum of the input size and the
*      max_block_size;
* @return
*      the size that will have the block
*/
size_t get_maxcompr_size(size_t src_size);

/*
 * @param src_size
 *      note that this will be choosen from the minimum of the input size and the
 *      max_block_size;
 * @return
 *      the size that will be used for the hashmap
 */
size_t calculate_hashmap_size(size_t src_size);

/**
 * @param seq
 *      the string we want to search for in the hashmap, 4 chars
 * @return
 *      the unsigned integer corresponding the index
 */
uint gethash(char *seq, size_t hash_size);

//@return true if correctly added,
// or false if not becasue it was already there
int hash_addifnotin(hash_entry *hashmap, char *str, uint pos, uint len, size_t hash_size);

/**
 * @param ht Hash table to be setted up
 * @param hash_size The size to allocate
 */
void setup_hashtable(hash_entry ht[], size_t hash_size);


/*
 * Tries to add an element to the map and return NULL if it's already present
 * @param *hashmap
 *      The hashmap first element ptr
 * @param str
 *      The string of the match
 * @param pos
 *      The position of the match if src file
 * @param len
 *      The length of the matched string
 * @return the new first
 */
avlnode *hash_addifnotin_util(avlnode *, char *str, uint pos, uint len, size_t hash_size);

/**
 * @param ht hash table first element
 * @param hash_size self-explainatory
 * @param str_to_search self-expl. too
 * @return the node found or NULL if not found
 */
avlnode* hash_getel(hash_entry *ht, size_t hash_size, char * str_to_search);

#endif //SNAPPY_HASHTABLE_H
