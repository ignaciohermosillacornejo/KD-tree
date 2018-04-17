#include "kdtree.h"
#include <stdlib.h>
#include <stdbool.h>
/* https://stackoverflow.com/questions/3437404/min-and-max-in-c */
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/* helper method to create a new box in the kdtre */
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
     * reverse for cases with the Y axis 
    */
    if (lr_child == 0 && axis == 0)
        kdnode->b->X = pivot.X;
    else if (lr_child == 1 && axis == 0)
        kdnode->a->X = pivot.X;
    else if (lr_child == 0 && axis == 1)
        kdnode->b->Y = pivot.Y;
    else if (lr_child == 1 && axis == 1)
        kdnode->a->Y = pivot.Y;
    return kdnode;
}

/* helper method to return smalles the box corresponding to coordinates x and y */
Kdnode *kdnode_search(Kdtree *tree, double x, double y)
{
    // printf("(%f, %f), (%f, %f) \n", current->a->X, current->a->Y, current->b->X, current->b->Y);
    Kdnode *current = tree->head;
    while (current->lkdnode != NULL)
    {
        if (current->lkdnode->axis == 0) /* X axis */
        {
            if (x < current->lkdnode->b->X)
            {
                current = current->lkdnode;
            }
            else
                current = current->rkdnode;
        }
        else if (current->lkdnode->axis == 1) /* Y axis */
        {
            if (y < current->lkdnode->b->Y)
            {
                current = current->lkdnode;
            }
            else
                current = current->rkdnode;
        }
    }
    return current;
}

/* helper method to check if a box has colissions with a circle */
bool collision(Kdnode *kdnode, int row, int col, double radius)
{
    /* https://github.com/IIC2133-PUC/2018-1/issues/48 */
    double delta_x = col - MAX(kdnode->a->X, MIN(col, kdnode->b->X));
    double delta_y = row - MAX(kdnode->a->Y, MIN(row, kdnode->b->Y));
    return (delta_x * delta_x + delta_y * delta_y) < (radius * radius);
}

/* helper function that returns the nearest neighbour inside a given box */
void kdnode_nns_box(Kdnode *current, Point *v, int row, int col, double *closest_distance, int *closest_point)
{
    for (int i = current->lvalue; i <= current->rvalue; i++)
    {
        double distance = euclidean_distance(v[i], row, col);
        if (distance < *closest_distance)
        {
            *closest_distance = distance;
            *closest_point = i;
        }
    }
}

/* helper recuresive function that returns the best distance for any given kdnode */
void kdnode_nns(Kdnode *kdnode, Point *v, int row, int col, double *radius, int *closest_point)
{
    // printf("(%f, %f), (%f, %f) \n", kdnode->a->X, kdnode->a->Y, kdnode->b->X, kdnode->b->Y);
    if (kdnode->lkdnode != NULL)
    {
        if (collision(kdnode->lkdnode, row, col, *radius))
            kdnode_nns(kdnode->lkdnode, v, row, col, radius, closest_point);
        if (collision(kdnode->rkdnode, row, col, *radius))
            kdnode_nns(kdnode->rkdnode, v, row, col, radius, closest_point);
    }
    /* end condition, we reach the end of the tree, so we must check all Points inside the box */
    else
        kdnode_nns_box(kdnode, v, row, col, radius, closest_point);
}

void kdtree_insert(Kdtree *tree, int axis, Point pivot, int lvalue, int rvalue, int middle)
{
    /* for each insert, we create two kdnodes, one for each new box of the partition */
    Kdnode *current = kdnode_search(tree, pivot.X, pivot.Y);
    // printf("(%f, %f), (%f, %f) \n", current->a->X, current->a->Y, current->b->X, current->b->Y);
    current->lkdnode = kdnode_init(axis, pivot, current, 0, lvalue, middle - 1);
    current->rkdnode = kdnode_init(axis, pivot, current, 1, middle, rvalue);
}
/* create our kdtree structure and return a pointer to it */
Kdtree *kdtree_init(int width, int height, int nuclei_count)
{
    /* We create a kdtree with a box that contains the whole image */
    Kdtree *kdtree = (Kdtree *)malloc(sizeof(Kdtree));
    kdtree->head = (Kdnode *)malloc(sizeof(Kdnode));
    kdtree->head->a = (Point *)malloc(sizeof(Point));
    kdtree->head->b = (Point *)malloc(sizeof(Point));
    kdtree->head->a->X = 0;
    kdtree->head->a->Y = 0;
    kdtree->head->b->X = width;
    kdtree->head->b->Y = height;
    kdtree->head->lkdnode = NULL;
    kdtree->head->rkdnode = NULL;
    kdtree->head->rvalue = 0;
    kdtree->head->rvalue = nuclei_count - 1;
    kdtree->head->axis = 0; // TODO revise this
    return kdtree;
}

int kdtree_nns(int row, int col, Kdtree *tree, Point *v)
{
    /* Nearest neighbor search as seen in class
     * Step 1: we find the smallest box containing the pixel
     * Step 2: we find the nearest neighbor inside that box and calculate its distance
     * Step 3: we do a recursive search over the tree, checking box colisions and looking for a closer nuclei
     * https://courses.cs.washington.edu/courses/cse373/02au/lectures/lecture22l.pdf
     * https://yal.cc/rectangle-circle-intersection-test/
     * 
    */

    /* Step 1 */
    Kdnode *current = kdnode_search(tree, col, row);

    /* Step 2 */
    double closest_distance = INT32_MAX;
    int closest_point;
    kdnode_nns_box(current, v, row, col, &closest_distance, &closest_point);
    // printf("(%f, %f)\n", v[closest_point].X, v[closest_point].Y);

    /* Step 3 */
    current = tree->head;
    kdnode_nns(current, v, row, col, &closest_distance, &closest_point);
    // printf("(%f, %f)\n", v[closest_point].X, v[closest_point].Y);
    return closest_point;
}

/* helper function frees a node and its children recuresively */
void kdnode_destroy(Kdnode *kdnode)
{
    if (kdnode->lkdnode != NULL)
    {
        kdnode_destroy(kdnode->lkdnode);
        kdnode_destroy(kdnode->rkdnode);
    }
    free(kdnode->a);
    free(kdnode->b);
    free(kdnode);
}

void kdtree_destroy(Kdtree *tree)
{
    kdnode_destroy(tree->head);
    free(tree);
}