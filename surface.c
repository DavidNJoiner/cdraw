#include "surface.h"

Surface* createSurface(int w, int h) 
{
    Surface* surface = (Surface*)malloc(sizeof(Surface));
    if (!surface) return NULL;
    surface->width = w;
    surface->height = h;
    surface->pixels = (unsigned int*)malloc(w * h * sizeof(unsigned int));
    if (!surface->pixels) { 
        free(surface);
        return NULL;
    }
    memset(surface->pixels, 0, w * h * sizeof(unsigned int));
    return surface;
}

void setPixel(Surface* surface, int x, int y, unsigned int color, int thickness) {
    for (int dy = -thickness / 2; dy <= thickness / 2; dy++) {
        for (int dx = -thickness / 2; dx <= thickness / 2; dx++) {
            int newX = x + dx;
            int newY = y + dy;
            
            // Add bound check considering thickness
            if (newX >= 0 && newX < surface->width && newY >= 0 && newY < surface->height) {
                surface->pixels[newY * surface->width + newX] = color;
            }
        }
    }
}

void clearSurface(Surface* surface, unsigned int color) 
{
    for (int y = 0; y < surface->height; y++) 
    {
        for (int x = 0; x < surface->width; x++) 
        {
            setPixel(surface, x, y, color, 0);
        }
    }
}

void freeSurface(Surface* surface) 
{
    free(surface->pixels);
    free(surface);
}

XImage* surfaceToXImage(Display* display, Surface* surface)
{
    return XCreateImage (display, DefaultVisual(display, 0), 24, ZPixmap, 0, (char*)surface->pixels, surface->width, surface->height, 32, 0);
}


