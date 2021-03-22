//
// Created by Luca Pasini on 11/27/19.
// Made by : Luca Pasini
//

#ifndef SNAPPY_HASHTABLE_H
#define SNAPPY_HASHTABLE_H

#ifndef _ulong
#define _ulong unsigned long
#endif

#ifndef _uint
#define _uint unsigned int
#endif

#ifndef _ushort
#define _ushort unsigned short
#endif

#ifndef _size_t
#define _size_t unsigned long
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utilityfuncs.h"
#include "list.h"

//Struct for Linked List usage
typedef struct hash_entry_list_type {
    list_node *head;
} hash_entry_l;

//Struct for closed Hash Table
typedef struct hash_entry_type {
    _uint pos;
    _uint len;
    char *str;
}hash_entry;

//------------List Methods--------------------
void hash_addifnotin_l(char *str, _uint pos, _uint strlen, _uint hashcode);
void setup_hashtable_l();
list_node* hash_getel_l(char *str_to_search, _uint strlen, _uint hashcode);
void *clear_table_l();

//------------No Struct Methods----------------
void hash_add(char *str, _uint pos, _uint strlen, _uint hashcode);
hash_entry* hash_getel(char *str_to_search, _uint strlen, _uint hashcode);
void setup_hashtable();
void clear_hashtable();

//------------Common Methods-----------------

/*
* @param src_size
*      note that this will be choosen from the minimum of the input size and the
*      max_block_size;
* @return
*      the size that will have the block
*/
_size_t get_maxcompr_size(_size_t src_size);

/*
 * @param src_size
 *      note that this will be choosen from the minimum of the input size and the
 *      max_block_size;
 * @return
 *      the size that will be used for the hashmap
 */
_size_t calculate_hashmap_size(_size_t src_size);

/**
 * @param seq
 *      the string we want to search for in the hashmap, 4 chars
 * @return
 *      the unsigned integer corresponding the index
 */
_uint gethash(const char *seq, _uint len);

void set_size(_size_t sz);

#endif //SNAPPY_HASHTABLE_H
