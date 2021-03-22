//
// Created by Luca Pasini on 11/27/19.
// Made by : Luca Pasini
//

#include <stdio.h>

#define COUNT 10

void print_tree(avlnode *root) {
    if (root->str == NULL) return;
    print_tree(root->left_child);

    printf("STR: %-8s, POS: %-3d, OFF: %-3d, HEIGHT:%-3d\n", root->str, root->pos, root->len, root->height);

    print_tree(root->right_child);
}

// Function to print binary tree in 2D
// It does reverse inorder traversal
// From geekforgeeks.org just for testing -----------------------------------------------
void print2DUtil(avlnode *root, int space)
{
    // Base case
    if (root==NULL || root -> str == NULL )
        return;

    // Increase distance between levels
    space += COUNT;

    // Process right child first
    print2DUtil(root->right_child, space);

    // Print current node after space
    // count
    printf("\n");
    for (int i = COUNT; i < space; i++)
        printf(" ");
    printf("%s, H=%d\n", root->str, root->height);

    // Process left child
    print2DUtil(root->left_child, space);
}

// Wrapper over print2DUtil()
void print2D(avlnode *root)
{
    // Pass initial space count as 0
    print2DUtil(root, 0);
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

void get_test(avlnode* root, char c[], _uint strlen){
    printf("\n\nfinding element %s:\n", c);
    avlnode *fnd = avl_getel(root, c, strlen);

    if(fnd)
        printf("STR=%-6s, H=%d\n", fnd->str, fnd->height);
    else
        printf("Not found\n");
}

int main() {
    printf("Test for AVL Tree\n\n");

    avlnode *tree = create_avl();


    tree = avl_addifnotin(tree, "a", 1, 1);
    tree = avl_addifnotin(tree, "b", 2, 1);
    tree = avl_addifnotin(tree, "0", 3, 1);

    tree = avl_addifnotin(tree, "1", 4, 1);
    tree = avl_addifnotin(tree, "2", 5, 1);
    tree = avl_addifnotin(tree, "f", 1, 1);
    tree = avl_addifnotin(tree, "z", 2, 1);
    tree = avl_addifnotin(tree, "c", 3, 1);
    tree = avl_addifnotin(tree, "4", 4, 1);
    tree = avl_addifnotin(tree, "3", 5, 1);
    tree = avl_addifnotin(tree, "f", 1, 1);
    tree = avl_addifnotin(tree, "f", 1, 1);
    tree = avl_addifnotin(tree, "f", 1, 1);
    tree = avl_addifnotin(tree, "f", 1, 1);

    print2D(tree);//from geeksforgeeks just to test the tree

    get_test(tree, "4", 1);
    get_test(tree, "f",1);
    get_test(tree, "10",2);

    tree=clear_tree(tree);


    if(tree){
        print2D(tree);
    }else{
        printf("\n\nThe tree is now empty\n");
    }


    return 0;
}

