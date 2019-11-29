//
// Created by kalu on 11/26/19.
//

#include "hashtable.h"

#define MAX_SIZE 32768
#define MIN_SIZE 256

//TODO
int hash_add_ifnotin(hash_entry *hashmap, char *str, uint pos, uint len){
    hash_entry *el = &hashmap[gethash(str)];
    if(el -> root_ptr == NULL){
        //Create root ptr
        //el -> root_ptr = malloc(X);
    }

    //if it is not already in the tree, add el into it and return true
    //else don't add and return false
}

hash_entry* create_hashtable(size_t src_size){
    size_t size = calculate_hashmap_size(src_size);
    hash_entry *hm = malloc(sizeof(hash_entry)*size);
    return  hm;
}

size_t get_maxcompr_size(size_t src_size){
    return 32 + src_size + src_size/6;
}

size_t calculate_hashmap_size(size_t src_size){
    if(src_size>MAX_SIZE) return MAX_SIZE;
    else if(src_size<MIN_SIZE) return MIN_SIZE;

    return 2 << log2_floor((int)src_size-1);
}

uint gethash(char *seq){
    //read seq as uint and multiplies by 0x1e35a7bd
    //then shift right one bit out

    uint seq_bytes = char_seq_to_uint(seq, strlen(seq));

    uint ret = 0x1e35a7bd*seq_bytes; //ONLY this line is inspired by google's GitHub page

    //shifting right to reach a number permitted in the table
    while(ret > MAX_SIZE){
        ret >>= 1;
    }

    return ret;
}