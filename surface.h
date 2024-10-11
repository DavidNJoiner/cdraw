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
    unsigned int* pixels; 
} Surface;

Surface* createSurface(int w, int h);
void setPixel(Surface* surface, int x, int y, unsigned int color, int thickness);
void clearSurface(Surface* surface, unsigned int color);
void freeSurface(Surface* surface);

XImage* surfaceToXImage(Display* display, Surface* surface);

#endif //SURFACE_H