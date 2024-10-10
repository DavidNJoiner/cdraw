#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "window.h"
#include "vec2.h"

#define QUAD_NODE_CAPACITY 6

typedef struct QuadTree QuadTree;

typedef struct sAABB
{
    vec2 center;
    float halfWidth;
    float halfHeight;
}AABB;

typedef struct QuadTree
{
    AABB boundary;
    int pointCount;
    vec2 points[QUAD_NODE_CAPACITY];
    
    QuadTree* northWest;
    QuadTree* northEast;
    QuadTree* southWest;
    QuadTree* southEast;

    QuadTree* (*constructQuadTree)(float x, float y, float width, float height) ;
    void (*subdivide)(QuadTree*);
}QuadTree;

void freeQuadTree(QuadTree* quad);
void subdivide(QuadTree* quad);

//bool intersectsAABB(AABB other);
bool insert(QuadTree* quad, vec2 p);

QuadTree* constructQuadTree(vec2 center, float halfwidth, float halfheight);
AABB constructBoundingBox(vec2 center, float halfwidth, float halfheight);
void drawQuadTree(VWindow* window, QuadTree* quad);
void eraseQuadTree(VWindow* win, QuadTree* quad);

#endif //QUADTREE_H
