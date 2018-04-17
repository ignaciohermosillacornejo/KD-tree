#include "kdtree.h"
#include <stdlib.h>

/** helper method to create a new box in the kdtre */
Kdnode *kdnode_init(int axis, Point pivot, Kdnode *parent, int lr_child, int lvalue, int rvalue)
{
    Kdnode *kdnode = malloc(sizeof(Kdnode));
    kdnode->axis = axis;
    kdnode->lkdnode = NULL;
    kdnode->rkdnode = NULL;
    kdnode->a = malloc(sizeof(Point));
    kdnode->b = malloc(sizeof(Point));
    kdnode->a->X = parent->a->X;
    kdnode->a->Y = parent->a->Y;
    kdnode->b->X = parent->b->X;
    kdnode->b->Y = parent->b->Y;
    kdnode->lvalue = lvalue;
    kdnode->rvalue = rvalue;
    /* if lr_child is 0, it means it is the left  child of the parent 
     * if lr_child is 1, it means it is the right child of the parent 
     * idem for cases with the Y axis 
    */
    if (lr_child == 0 && axis == 0)
        kdnode->b->X = pivot.X;
    else if (lr_child == 1 && axis == 0)
        kdnode->a->X = pivot.X;
    else if (lr_child == 0 && axis == 1)
        kdnode->a->Y = pivot.Y;
    else if (lr_child == 1 && axis == 1)
        kdnode->b->Y = pivot.Y;
}

void kdtree_insert(Kdtree *tree, char axis, Point pivot, int lvalue, int rvalue)
{
    /* For each insert, we create two kdnodes, one for each new box */
    // TODO: insert children
    Kdnode *current;
    current = tree->head;
    printf("axis: %c", axis);
    printf("(%f, %f)\n", pivot.X, pivot.Y);
    //current->lkdnode = kdnode_init(axis, pivot, current, 0, lvalue, ((rvalue - lvalue) / 2) - 1);
    //current->rkdnode = kdnode_init(axis, pivot, current, 1, ((rvalue - lvalue) / 2), rvalue - 1);
}

Kdtree *kdtree_init(int width, int height, int nuclei_count)
{
    /* We create a kdtree with a box that contains the whole image */
    Kdtree *kdtree = malloc(sizeof(Kdtree));
    kdtree->head = malloc(sizeof(Kdnode));
    kdtree->head->a = malloc(sizeof(Point));
    kdtree->head->b = malloc(sizeof(Point));
    kdtree->head->a->X = 0;
    kdtree->head->a->Y = 0;
    kdtree->head->b->X = width;
    kdtree->head->b->Y = height;
    kdtree->head->lkdnode = NULL;
    kdtree->head->rkdnode = NULL;
    kdtree->head->rvalue = 0;
    kdtree->head->rvalue = nuclei_count - 1;
    return kdtree;
}

void kdtree_destroy(Kdtree *kdtree)
{
    // TODO: all
    return;
}