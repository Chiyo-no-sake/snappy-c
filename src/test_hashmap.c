//
// Created by kalu on 11/27/19.
//
#include "hashtable.h"
#include "snappy_algo.h"
#include <stdio.h>

void hash_test(){
    const short sizetest=7;
    char* list[sizetest];
    list[0] = "abcd";
    list[1] = "abab";
    list[2] = "aaaa";
    list[3] = "aaaab";
    list[4] = "aaab";
    list[5] = "aaaaa";
    list[6] = "aaaaab";

    for(int i=0; i<sizetest; i++){
        uint hash = gethash(list[i]);
        printf("sequence %-8s \t-> 0x%X == %d \n", list[i], hash, hash);
    }
}


int main(){
    printf("Testing hash collisions:\n");
    hash_test();

    printf("\nTesting hash creation and adding el:\n");
    avlnode *root = create_hashtable(256);

    if(hash_add_ifnotin(root, "abab", 0, 4)){
        printf("Added to the map abab");
    }else{
        printf("abab already present");
    }


    return 0;
}