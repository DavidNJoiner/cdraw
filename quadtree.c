#include "quadtree.h"
#include "define.h"

QuadTree* constructQuadTree(vec2 center, float halfwidth, float halfheight) 
{
    QuadTree* quad = (QuadTree*)malloc(sizeof(QuadTree));
    if (quad == NULL) {exit(1); }
    AABB newBoundary = {center, halfwidth, halfheight};
    quad->boundary = newBoundary;
    quad->pointCount = 0;
    quad->northWest = quad->northEast = quad->southWest = quad->southEast = NULL;
    return quad;
}

AABB constructBoundingBox(vec2 center, float halfwidth, float halfheight)
{
    vec2 origin = {center.x, center.y};
    AABB boundingBox = {origin, halfwidth, halfheight};
    return boundingBox;
}

void freeQuadTree(QuadTree* quad)
{
    if (quad == NULL) {
        return;
    }
    // Recursively free the child nodes
    freeQuadTree(quad->northWest);
    freeQuadTree(quad->northEast);
    freeQuadTree(quad->southWest);
    freeQuadTree(quad->southEast);
    free(quad);
}

//bool intersectsAABB(AABB other) {};

void subdivide(QuadTree* quad)
{
    float x = quad->boundary.center.x;
    float y = quad->boundary.center.y;
    float w = quad->boundary.halfWidth;
    float h = quad->boundary.halfHeight;

    // Calculate new centers and half dimensions for child quadrants
    vec2 nw_center = {x - w/2, y + h/2};
    vec2 ne_center = {x + w/2, y + h/2};
    vec2 sw_center = {x - w/2, y - h/2};
    vec2 se_center = {x + w/2, y - h/2};

    // Construct child QuadTrees
    quad->northWest = constructQuadTree(nw_center, w/2, w/2);
    quad->northEast = constructQuadTree(ne_center, w/2, w/2);
    quad->southWest = constructQuadTree(sw_center, w/2, w/2);
    quad->southEast = constructQuadTree(se_center, w/2, w/2);
}


bool insert(QuadTree* quad, vec2 p) 
{
    if (!quad) {
        fprintf(stderr, "Error: null QuadTree pointer in insert()\n");
        return false;
    }

    if (p.x < quad->boundary.center.x - quad->boundary.halfWidth ||
        p.x > quad->boundary.center.x + quad->boundary.halfWidth ||
        p.y < quad->boundary.center.y - quad->boundary.halfHeight ||
        p.y > quad->boundary.center.y + quad->boundary.halfHeight) {
        return false;
    }

    if (quad->northWest == NULL) {
        if (quad->pointCount < QUAD_NODE_CAPACITY) {
            quad->points[quad->pointCount++] = p;
            return true;
        } else {
            subdivide(quad);
            
            // Redistribute existing points
            for (int i = 0; i < quad->pointCount; i++) {
                insert(quad->northWest, quad->points[i]) ||
                insert(quad->northEast, quad->points[i]) ||
                insert(quad->southWest, quad->points[i]) ||
                insert(quad->southEast, quad->points[i]);
            }
            
            quad->pointCount = 0;  // Reset point count for this quad
            
            // Insert the new point
            return insert(quad->northWest, p) ||
                   insert(quad->northEast, p) ||
                   insert(quad->southWest, p) ||
                   insert(quad->southEast, p);
        }
    }

    // If this quad has been subdivided, insert the point into the appropriate child
    if (insert(quad->northWest, p) || insert(quad->northEast, p) ||
        insert(quad->southWest, p) || insert(quad->southEast, p)) {
        quad->pointCount++;  // Increment point count for this quad
        return true;
    }

    return false; 
}

void drawQuadTree(VWindow* win, QuadTree* quad) 
{
    if (quad == NULL) return;

    // Set color to green
    XSetForeground(win->display, win->gc, GREEN);

    // Calculate the position and size of this quadrant
    int x = (int)(quad->boundary.center.x - quad->boundary.halfWidth);
    int y = (int)(quad->boundary.center.y - quad->boundary.halfHeight);
    int width = (int)(quad->boundary.halfWidth * 2);
    int height = (int)(quad->boundary.halfHeight * 2);

    // Draw this quad's boundary
    XDrawRectangle(win->display, win->backBuffer, win->gc, x, y, width, height);
    
    // Recursively draw child quads
    if (quad->northWest) drawQuadTree(win, quad->northWest);
    if (quad->northEast) drawQuadTree(win, quad->northEast);
    if (quad->southWest) drawQuadTree(win, quad->southWest);
    if (quad->southEast) drawQuadTree(win, quad->southEast);
}

void eraseQuadTree(VWindow* win, QuadTree* quad) 
{
    if (quad == NULL) return;

    // Set color to black (assuming 0 is black in your color scheme)
    XSetForeground(win->display, win->gc, BLACK);

    // Calculate the position and size of this quadrant
    int x = (int)(quad->boundary.center.x - quad->boundary.halfWidth);
    int y = (int)(quad->boundary.center.y - quad->boundary.halfHeight);
    int width = (int)(quad->boundary.halfWidth * 2);
    int height = (int)(quad->boundary.halfHeight * 2);

    // Draw a filled black rectangle to erase this quad's boundary
    XDrawRectangle(win->display, win->backBuffer, win->gc, x, y, width, height);
    
    // Recursively erase child quads
    if (quad->northWest) eraseQuadTree(win, quad->northWest);
    if (quad->northEast) eraseQuadTree(win, quad->northEast);
    if (quad->southWest) eraseQuadTree(win, quad->southWest);
    if (quad->southEast) eraseQuadTree(win, quad->southEast);
}