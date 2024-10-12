#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "window.h"
#include "define.h"
#include "vec2.h"

// Function to draw a point on the surface
void drawPointOnSurface(VWindow* window, int x, int y, unsigned int color, unsigned int size);

// Function to draw a line on the surface using Bresenham's algorithm
void drawLineOnSurface(VWindow* window, vec2 start, vec2 end, unsigned int color, unsigned int thickness);

// Function to draw a rectangle on the surface
void drawRectangleOnSurface(VWindow* window, int x, int y, int width, int height, unsigned int color, unsigned int thickness);

// Helper function to calculate intensity based on distance
float intensityFromDistance(float distance, float thickness);

// Helper function to blend colors
unsigned int blendColors(unsigned int bg, unsigned int fg, float alpha);

// Function to draw a line on the surface using Gupta-Sproull algorithm
void drawLineOnSurface2(VWindow* window, vec2 start, vec2 end, unsigned int color, unsigned int thickness);

#endif // GRAPHICS_H