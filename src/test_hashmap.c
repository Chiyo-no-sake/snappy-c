//
// Created by kalu on 11/27/19.
//
#include "hashtable.h"
#include "snappy_algo.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
//TODO: TEST FOR 32000 input bugged on OOFUEI
#define SRC_SIZE 65000

char words[SRC_SIZE][100];

void fillwords() {
    char tmp[2];
    char a[100];
    for (int i = 0; i < (int) SRC_SIZE; i++) {
        a[0] = '\0';

        //for (int j = 0; j < 4; j++) {
        for (int j = 0; j < rand() % 50 + 4; j++) {
            tmp[0] = rand() % 31 + 41;
            tmp[1] = '\0';
            strcat(a, tmp);
        }

        strcpy(words[i], a);
    }
}

int freespaceam(hash_entry *table, size_t size) {
    int count = 0;
    for (int i = 0; i < size; i++) {
        if ((table + i)->root_ptr->str == NULL) count++;
    }

    return count;
}

void hash_test(hash_entry *table) {
    for (int i = 0; i < (int) SRC_SIZE; i++) {
        int res = hash_addifnotin(table, words[i], i, strlen(words[i]), calculate_hashmap_size(SRC_SIZE));

        printf("Inserting: %s\n", words[i]);
        if (!res)
            printf("\tAlready present.\n");
        else
            printf("\tInserted ok\n");

    }
}

avlnode *tree_test(avlnode *tree) {
    for (int i = 0; i < (int) SRC_SIZE; i++) {
        tree = avl_addifnotin(tree, words[i], i, strlen(words[i]));
    }
    return tree;
}

avlnode *tree_gettest(avlnode *tree){
    clock_t rateo = clock();
    for(int i=0; i< SRC_SIZE; i++){
        avl_getel(tree, words[i]);
        /*    printf("1 found.\n");
        else{
            printf("Not found.\n");
        }*/
    }
    rateo = (clock()-rateo)/SRC_SIZE;
    printf("Rateo: %f ms/word", (float)rateo/100000);
}

avlnode *hash_gettest(hash_entry *hash, size_t hash_size){
    clock_t rateo = clock();
    for(int i=0; i< SRC_SIZE; i++){
        hash_getel(hash, hash_size, words[i]);
            //printf("1 found.\n");
        //else{
            //printf("Not found.\n");
        //}
    }
    rateo = (clock()-rateo)/SRC_SIZE;
    printf("Rateo: %f ms/word", (float)rateo/100000);
}

/*
void hashtest(hash_entry* table, size_t hash_size){
    char *strings[5];
    strings[0] = "Ciao";
    strings[1] = "Ciao";
    strings[2] = "marzapane";
    strings[3] = "parole";
    strings[4] = "prole";

    for(int i=0; i<5; i++){
        hash_addifnotin(table,strings[i], i, strlen(strings[i]),hash_size);
    }

    for(int i=0; i<5; i++){
        if(hash_getel(table, hash_size, strings[i]))
            printf("Found.\n");
        else
            printf("Not Found.\n");
    }

}
*/

int main() {

    fillwords();

    avlnode *tree = create_avl();

    clock_t tree_time = clock();
    tree = tree_test(tree);
    tree_time = clock() - tree_time;

    printf("Testing hash:\n");

    hash_entry htable[calculate_hashmap_size(SRC_SIZE)];
    setup_hashtable(htable, calculate_hashmap_size(SRC_SIZE));

    clock_t hash_time = clock();
    hash_test(htable);
    hash_time = clock() - hash_time;

    printf("\nHash taken: %f sec.\nTree taken: %f sec.", (float) hash_time / 100000, (float) tree_time / 100000);

    hash_time = clock();
    hash_gettest(htable, calculate_hashmap_size(SRC_SIZE));
    hash_time = clock()-hash_time;

    tree_time = clock();
    tree_gettest(tree);
    tree_time = clock()-tree_time;

    printf("\nHash taken: %f sec.\nTree taken: %f sec.", (float) hash_time / 100000, (float) tree_time / 100000);
    return 0;
}