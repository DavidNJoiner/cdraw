#ifndef SURFACE_H
#define SURFACE_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <string.h>  // For memset
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct Surface 
{
    int width, height;
    uint32_t* pixels; 
} Surface;

Surface* createSurface(int w, int h);

void setPixel(Surface* surface, int x, int y, uint32_t color, int thickness);

void clearSurface(Surface* surface, uint32_t color);

void freeSurface(Surface* surface);

#endif //SURFACE_H