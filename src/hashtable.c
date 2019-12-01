//
// Created by kalu on 11/26/19.
//

#include "hashtable.h"

#define MAX_SIZE 65536
#define MIN_SIZE 256

int tot_coll = 0;
int tot_insertion = 0;

int hash_addifnotin(hash_entry *hashmap, char *str, uint pos, uint len, size_t hash_size) {
    if (contains((hashmap + gethash(str, hash_size))->root_ptr, str))
        return 0;

    hashmap->root_ptr = avl_addifnotin(
            (hashmap + gethash(str, hash_size))->root_ptr,
            str,
            pos,
            len);

    return 1;
}

void setup_hashtable(hash_entry *ht, size_t hash_size) {
    for (size_t i = 0; hash_size > i; i++) {
        (ht + i)->root_ptr = create_avl();
    }
}

size_t get_maxcompr_size(size_t src_size) {
    return 32 + src_size + src_size / 6;
}

size_t calculate_hashmap_size(size_t src_size) {
    if (src_size > MAX_SIZE) return MAX_SIZE;
    else if (src_size < MIN_SIZE) return MIN_SIZE;

    return 2u << log2_floor((uint) src_size - 1);
}

avlnode* hash_getel(hash_entry *ht, size_t hash_size, char * str_to_search){
    return avl_getel((ht + gethash(str_to_search, hash_size))->root_ptr, str_to_search);
}

uint gethash(char *seq, size_t hash_size) {
    /*
     * https://en.wikipedia.org/wiki/Jenkins_hash_function
     * Hash function fast and with collisions ~ 15%
     */
    size_t i = 0;
    uint hash = 0;
    while (i != strlen(seq)) {
        hash += seq[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return (hash + strlen(seq)) % hash_size;
}

