//
// Created by kalu on 11/27/19.
//

#ifndef SNAPPY_AVL_TREE_H
#define SNAPPY_AVL_TREE_H

#include <stddef.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include "utilityfuncs.h"

typedef struct avlnode_type {
    //match parameters
    char *str;                      //The string corresponding to the match
    uint pos;                       //the match position realtive to src file
    uint len;                       //the lenght of the match

    //AVL parameters
    struct avlnode_type *left_child;
    struct avlnode_type *right_child;
    int height;
} avlnode;

/**
 * @return the tree
 */
avlnode *create_avl();

/**
 * @param subroot the unbalanced node / root of the subtree
 * @return the new root of the given subtree
 */
avlnode *balancenode(avlnode *subroot);


/**
 * add a node only if it's not already present
 * @param root  root node of the tree
 *
 * @param str   }
 * @param pos   } data to store for the str
 * @param len   }
 *
 * @return  the new root of the tree
 *
 */
avlnode *avl_addifnotin(avlnode *tree_root, char str[], uint pos, uint len);


/**
 * Rotation functions
 * @param node unbalanced node
 * @return the new root of the given subtree
 */
avlnode *avl_rotate_rr(avlnode *node);

avlnode *avl_rotate_rl(avlnode *node);

avlnode *avl_rotate_ll(avlnode *node);

avlnode *avl_rotate_lr(avlnode *node);

// return weight of the given node
int node_weight(avlnode *node);

// update the height of the given node
void updateheight(avlnode *node);




#endif //SNAPPY_AVL_TREE_H
