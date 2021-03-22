//
// Created by Luca Pasini on 11/27/19.
// Made by : Luca Pasini
//

#include "hashtable.h"

#define MAX_SIZE 65536
#define MIN_SIZE 256

static _size_t size;
static void* hashmap;

//----------------No Struct Methods--------------
//setup hashtable and clear hash table are not needed
inline void hash_add(char *str, _uint pos, _uint strlen, _uint hashcode){
    hash_entry *el = ((hash_entry*)hashmap)+hashcode;

    if(el == NULL) return;

    if(el->str==NULL) el->str = malloc(strlen);
    else{
        free(el->str);
        el->str=malloc(strlen);
    }
    memcpy(el->str, str, strlen);
    el->pos=pos;
    el->len=strlen;
}

inline hash_entry* hash_getel(char *str_to_search, _uint strlen, _uint hashcode){
    hash_entry *el = ((hash_entry*)hashmap)+hashcode;
    if(el->str==NULL) return NULL;
    if(strlen == el->len)
        if(memcmp(str_to_search, el->str,strlen)==0)
            return el;

    return NULL;
}

inline void setup_hashtable(){
    hashmap = malloc(sizeof(hash_entry)*size);
    for (_size_t i = 0; i<size; i++) {
        ((hash_entry*)(hashmap) + i)->str = NULL;
    }
}

inline void clear_hashtable(){
    for (_size_t i = 0; i<size; i++) {
        if((((hash_entry*)(hashmap)) + i)->str!=NULL) free((((hash_entry*)(hashmap)) + i)->str);
        (((hash_entry*)(hashmap)) + i)->str = NULL;
    }

    free(hashmap);
}

//-----------------List Methods------------------

inline void hash_addifnotin_l(char *str, _uint pos, _uint strlen, _uint hashcode) {
    (((hash_entry_l*)(hashmap)) + hashcode)->head = list_add_ifnotin((((hash_entry_l*)(hashmap)) + hashcode)->head, str, strlen, pos);
}

inline void setup_hashtable_l(){
    hashmap = malloc(sizeof(hash_entry_l)*size);
    for (_size_t i = 0; i<size; i++) {
        (((hash_entry_l*)(hashmap) + i))->head = NULL;
    }
}

inline list_node* hash_getel_l(char *str_to_search, _uint strlen, _uint hashcode){
    return list_getel((((hash_entry_l*)(hashmap)+hashcode))->head,str_to_search, strlen);
}


inline void *clear_table_l() {
    for (_ulong i = 0; i < size; i++) {
        (((hash_entry_l*)(hashmap) + i))->head = clear_list((((hash_entry_l*)(hashmap) + i))->head);
        free((((hash_entry_l*)(hashmap) + i))->head);
    }

    free(hashmap);
}



//----------------Common Functions---------------------

inline void set_size(_size_t sz){
    size = sz;
}

inline _size_t get_maxcompr_size(_size_t src_size) {
    return 32 + src_size + src_size / 6;
}

inline _size_t calculate_hashmap_size(_size_t src_size) {
    if (src_size > MAX_SIZE) return MAX_SIZE;
    else if (src_size < MIN_SIZE) return MIN_SIZE;

    return 2u << log2_floor(src_size - 1);
}

    inline _uint gethash(const char *seq, _uint seqlen) {
        /*
         * https://en.wikipedia.org/wiki/Jenkins_hash_function
         * Hash function fast and with collisions ~= 15%
         *
         * adapted to avoid performance loss on file that contain long matches,
         * e.g. if we have a file full with 0s, an hash will be calculated
         * from a string made by nearly 32K characters, this will lead the above Jenkins function to take a lot of time
         * because it cycles throuh the length.
         *
         * Instead we calculate the hash from the first 2 characters and the last 2,
         * in order to avoid multiple collisions and tree rebalancing from the previously explained condition ( if we used
        */

        _uint hash = 0;
        for(int i=0;i<2;i++) {
            hash += seq[seqlen-i-1];
            hash += seq[i];
            if(seqlen>5){
                hash+=seq[(int)seqlen/2];
            }
            hash += hash << 10;
            hash ^= hash >> 6;
        }

        hash += hash << 3;
        hash ^= hash >> 11;
        hash += hash << 15;

        //line adapted to slightly reduce the collision ratio
        return (hash + seqlen) % size;
    }
