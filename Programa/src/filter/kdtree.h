#pragma once
#include "point.h"

/** Un kdtree2 */
struct knode;

/** Una lista ligada simple */
typedef struct knode Knode;

/** Una lista ligada simple */
struct knode
{
    int axis; // 1 if we split on the X axis, 2 if we split on the Y  
    double box1, box2; // two points that make up the box
    Knode *lknode, *rknode; // nodes of the left and right children
    Point *lvalue, *rvalue; // pointer to values of the tree
};
