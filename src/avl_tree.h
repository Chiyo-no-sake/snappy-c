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


/*
 *  @param tree_root is the root of the tree to search in (or subtree)
 *  @param str_to_search is the string from we want to get data
 *  @return the node found if present, else null
 */
avlnode *avl_getel(avlnode *tree_root, char *str_to_search);

/*----------------------------------------------------*/
/*
 * Rotation functions
 * @param node unbalanced node
 * @return the new root of the given subtree
 */

//right left rotation
avlnode *avl_rotate_rr(avlnode *node);

//right left rotation
avlnode *avl_rotate_rl(avlnode *node);

//left left rotation
avlnode *avl_rotate_ll(avlnode *node);

//left right rotation
avlnode *avl_rotate_lr(avlnode *node);

/*-------------------------------------------------------*/

//free the memory and clear the tree from the given root, returning the new root
// aka NULL
avlnode *clear_tree(avlnode *tree_root);

// return weight of the given node
int node_weight(avlnode *node);

// update the height of the given node
void updateheight(avlnode *node);

// @return true if the node is in the tree
int contains(avlnode* root, char str[]);

#endif //SNAPPY_AVL_TREE_H
