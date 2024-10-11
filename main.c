#include <unistd.h>  // For usleep
#include "random.h"
#include "window.h"
#include "quadtree.h"
#include "define.h"


#define WIDTH 1200
#define HEIGHT 1200

int main() 
{
    // Seed the random number generator with a constant value for reproducibility
    srand(time(NULL));

    float fhalfWidth = WIDTH/2.0f;
    float fhalfHeight = HEIGHT/2.0f;
    vec2 rootQuadCenter = {fhalfWidth, fhalfHeight};

    VWindow* window = createWindow(WIDTH, HEIGHT);
    ASSERT(window != NULL);

    QuadTree* rootQuad = constructQuadTree(rootQuadCenter, fhalfWidth, fhalfHeight);
    ASSERT(rootQuad != NULL);
    if (rootQuad == NULL)
    {
        destroyWindow(window);
        return 1;
    }

    window->width = WIDTH;
    window->height = HEIGHT;

    XSetForeground(window->display, window->gc, LIGHT_GRAY);
   
    int pointCount = 0;
    clearColor(window, BLACK);

    while (!window->shouldClose) 
    {
        
        if(window->randomize)
        {
            pointCount = 0;
            freeQuadTree(rootQuad);
            rootQuad = constructQuadTree(rootQuadCenter, fhalfWidth, fhalfHeight);
            clearColor(window, BLACK);
            window->randomize = false;
        } 

        if(pointCount < 1000)
        {
            vec2 p = {frand_clustered(window->width, 0.5f), frand_clustered(window->height, 1.0f)};
            insert(rootQuad, p);
            drawPoint(window, (int)p.x, (int)p.y, RED, 3);
            pointCount++;
        } 

        // Draw the surface (red dots) to the window
        drawSurfaceToWindow(window);

        // Draw the QuadTree
        if(window->drawQuads)
        {
            drawQuadTree(window, rootQuad);
        }

        // Draw the text
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Point Count: %d", pointCount);
        drawText(window, 10, 30, buffer, WHITE, 32);

        // Present the window (swap buffers)
        presentWindow(window);
        
        handleEvents(window);
        usleep(16667);  // ~60 FPS
    }

    // Clean up
    freeQuadTree(rootQuad);
    destroyWindow(window);
    return 0;
}