//
// Created by kalu on 11/27/19.
//

#include <stdio.h>
#include "avl_tree.h"

void print_tree(avlnode *root) {
    if (root->str == NULL) return;
    print_tree(root->left_child);

    printf("STR: %-8s, POS: %-3d, OFF: %-3d, HEIGHT:%-3d\n", root->str, root->pos, root->len, root->height);

    print_tree(root->right_child);
}

int main() {
    printf("Test for AVL Tree\n\n");

    avlnode *tree = create_avl();


    tree = avl_addifnotin(tree, "a", 1, 4);
    tree = avl_addifnotin(tree, "b", 2, 4);
    tree = avl_addifnotin(tree, "0", 3, 4);

    tree = avl_addifnotin(tree, "1", 4, 5);
    tree = avl_addifnotin(tree, "2", 5, 4);
    tree = avl_addifnotin(tree, "f", 1, 4);
    tree = avl_addifnotin(tree, "z", 2, 4);
    tree = avl_addifnotin(tree, "c", 3, 4);
    tree = avl_addifnotin(tree, "4", 4, 5);
    tree = avl_addifnotin(tree, "3", 5, 4);
    tree = avl_addifnotin(tree, "f", 1, 4);
    tree = avl_addifnotin(tree, "f", 1, 4);
    tree = avl_addifnotin(tree, "f", 1, 4);
    tree = avl_addifnotin(tree, "f", 1, 4);

    print_tree(tree);

    return 0;
}

