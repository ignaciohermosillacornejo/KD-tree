#pragma once
#include "point.h"

/** Un kdtree2 */
struct kdnode;
struct kdtree;

typedef struct kdnode Kdnode;
typedef struct kdtree Kdtree;

/** Una lista ligada simple */
struct kdnode
{
    int axis; // 0 if we split on the X axis, 1 if we split on the Y  
    Point *a, *b; // two points that make up the box
    Kdnode *lkdnode, *rkdnode; // nodes of the left and right children
    int lvalue, rvalue; // range to values of the points inside the box
};

struct kdtree
{
    Kdnode * head;
};

Kdtree *kdtree_init(int width, int height, int nuclei_count);
void kdtree_destroy(Kdtree *kdtree);
void kdtree_insert(Kdtree *tree, char axis, Point pivot, int lvalue, int rvalue, int middle);