#include <unistd.h>  // For usleep
#include "random.h"
#include "window.h"
#include "quadtree.h"
#include "define.h"


#define WIDTH 1200
#define HEIGHT 1200

int main() 
{
    srand(time(NULL));

    VWindow* window = createWindow(WIDTH, HEIGHT);
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }

    float fhalfWidth = WIDTH/2.0f;
    float fhalfHeight = HEIGHT/2.0f;
    vec2 rootQuadCenter = {fhalfWidth, fhalfHeight};

    QuadTree* rootQuad = constructQuadTree(rootQuadCenter, fhalfWidth, fhalfHeight);
    if (!rootQuad) {
        fprintf(stderr, "Failed to create quadtree\n");
        destroyWindow(window);
        return 1;
    }

    XSetForeground(window->display, window->gc, LIGHT_GRAY);
   
    int pointCount = 0;

    while (true) 
    {
        // Clear the back buffer at the start of each frame
        XSetForeground(window->display, window->gc, BLACK);
        XFillRectangle(window->display, window->backBuffer, window->gc, 0, 0, WIDTH, HEIGHT);

        if(window->randomize)
        {
            pointCount = 0;
            freeQuadTree(rootQuad);
            rootQuad = constructQuadTree(rootQuadCenter, fhalfWidth, fhalfHeight);
            clearSurface(window->surface, BLACK);
            window->randomize = false;
        } 

        if(pointCount < 220)
        {
            vec2 p = {frand(window->surface->width), frand(window->surface->height)};
            insert(rootQuad, p);
            setPixel(window->surface, (int)p.x, (int)p.y, RED, 5);
            pointCount++;
        } 

        // Draw the surface (red dots) to the back buffer
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

        // Copy the back buffer to the window
        XCopyArea(window->display, window->backBuffer, window->window, window->gc, 
                  0, 0, WIDTH, HEIGHT, 0, 0);

        XFlush(window->display);
        
        handleEvents(window);
        usleep(16667);  // ~60 FPS
    }
    XFreeFont(window->display, window->font);
    freeQuadTree(rootQuad);
    destroyWindow(window);
    return 0;
}