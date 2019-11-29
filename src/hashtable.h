//
// Created by kalu on 11/26/19.
//

#ifndef SNAPPY_HASHTABLE_H
#define SNAPPY_HASHTABLE_H

#include <stdlib.h>
#include <string.h>
#include "utilityfuncs.h"
#include "avl_tree.h"

typedef struct hash_entry_type{
    avlnode *root_ptr;
}hash_entry;

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
uint gethash(char *seq);

/**
 * @param src_size
 *      the size of the block (32K/64K) to compress
 * @return
 *      a pointer to the first element of the map created
 */
hash_entry* create_hashtable(size_t src_size);


/*TODO:
 * Tries to add an element to the tree given *root
 * ONLY if the element is not already in
 * @param root
 *      The root node of the avl where we want to add
 * @param str
 *      The string of the match
 * @param pos
 *      The position of the match if src file
 * @param len
 *      The length of the matched string
 * @return true if added, false if not because it was already
 */
int hash_add_ifnotin(hash_entry *hashmap, char *str, uint pos, uint len);

#endif //SNAPPY_HASHTABLE_H
