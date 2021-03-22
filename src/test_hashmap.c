//
// Created by Luca Pasini on 11/27/19.
// Made by : Luca Pasini
//

#include "hashtable.h"
#include "snappy_algo.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define SRC_SIZE 65536

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

#define STRESS 1

char words[(int)(SRC_SIZE*STRESS)][100];
void fillwords() {

    unsigned char tmp[2];
    unsigned char a[100];
    for (int i = 0; i < (int) SRC_SIZE*STRESS; i++) {
        a[0] = '\0';

        //for (int j = 0; j < 4; j++) {
        for (int j = 0; j < rand() % 50 + 4; j++) {
            tmp[0] = rand() % 256;
            tmp[1] = '\0';
            strcat(a, tmp);
        }

        strcpy(words[i], a);
    }
}

int freespaceam(hash_entry *table, _size_t size) {
    int count = 0;
    for (int i = 0; i < size; i++) {
        if ((table + i)->root_ptr->str == NULL) count++;
    }

    return count;
}

void hash_test(hash_entry *table) {
    for (int i = 0; i < (int) SRC_SIZE*STRESS; i++) {
        int res = hash_addifnotin(table, words[i], i, strlen(words[i]), calculate_hashmap_size(SRC_SIZE));
    }
}


avlnode *tree_test(avlnode *tree) {
    for (int i = 0; i < (int) SRC_SIZE*STRESS; i++) {
        tree = avl_addifnotin(tree, words[i], i, strlen(words[i]));
    }
    return tree;
}

avlnode *tree_gettest(avlnode *tree) {
    clock_t rateo = clock();
    for (int i = 0; i < SRC_SIZE*STRESS; i++) {
        avl_getel(tree, words[i], strlen(words[i]));
    }
    rateo = (clock() - rateo) / SRC_SIZE*STRESS;
}

avlnode *hash_gettest(hash_entry *hash, _size_t hash_size) {
    clock_t rateo = clock();
    for (int i = 0; i < SRC_SIZE*STRESS; i++) {
        hash_getel(hash, hash_size, words[i], strlen(words[i]));
    }

    rateo = (clock() - rateo) / SRC_SIZE;
}

void testcollisions(hash_entry *to_fill) {
    int tot_tries = 0;
    int tot_coll = 0;
    for (int i = 0; i < SRC_SIZE*STRESS; i++) {
        hash_entry *e = ((to_fill) + gethash(words[i], strlen(words[i]), calculate_hashmap_size(SRC_SIZE)));
        if ( e->root_ptr == NULL){
            e->root_ptr = create_avl();
        }else
            tot_coll++;

        e->root_ptr = avl_addifnotin(e->root_ptr,words[i], i , strlen(words[i]));
        tot_tries++;
    }

    printf("Collision / Tries : %d / %d = %f %%\n", tot_coll, tot_tries, (float) tot_coll / (float) tot_tries * 100);
}

int main() {
    srand(time(NULL));
    fillwords();

    for(int i=0;i<SRC_SIZE; i++){
        if(gethash(words[i],strlen(words[i]), calculate_hashmap_size(SRC_SIZE)) > calculate_hashmap_size(SRC_SIZE))
            printf("STR: %s\tHASH: %u\n", words[i], gethash(words[i], strlen(words[i]),calculate_hashmap_size(SRC_SIZE)));
    }
    printf("Hash size is: %lu\n", calculate_hashmap_size(SRC_SIZE));



    avlnode *tree = create_avl();

    clock_t tree_time = clock();
    tree = tree_test(tree);
    tree_time = clock() - tree_time;

    printf("Testing with %d elements\n", SRC_SIZE*STRESS);
    printf("Testing insertion:\n\n");

    hash_entry *htable = malloc(calculate_hashmap_size(SRC_SIZE)* sizeof(hash_entry));
    setup_hashtable(htable, calculate_hashmap_size(SRC_SIZE));

    clock_t hash_time = clock();
    hash_test(htable);
    hash_time = clock() - hash_time;

    printf("Hash taken: %f ms.\nTree taken: %f ms.\n\n", (float) hash_time / CLOCKS_PER_SEC * 1000,
           (float) tree_time / CLOCKS_PER_SEC * 1000);

    printf("Testing search:\n\n");
    hash_time = clock();
    hash_gettest(htable, calculate_hashmap_size(SRC_SIZE));
    hash_time = clock() - hash_time;

    tree_time = clock();
    tree_gettest(tree);
    tree_time = clock() - tree_time;
    printf("Hash taken: %f ms.\nTree taken: %f ms.\n\n", (float) hash_time / CLOCKS_PER_SEC * 1000,
           (float) tree_time / CLOCKS_PER_SEC * 1000);


    hash_entry to_fill[calculate_hashmap_size(SRC_SIZE)];
    printf("Hash size is: %lu\n", calculate_hashmap_size(SRC_SIZE));
    testcollisions(to_fill);


    return 0;
}