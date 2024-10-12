#include <math.h>
#include "window.h"
#include "vec2.h"

void drawPointOnSurface(VWindow* window, int x, int y, unsigned int color, unsigned int thickness)
{
    setPixel(window->surface, x, y, color, thickness);
}

void drawLineOnSurface(VWindow* window, vec2 start, vec2 end, unsigned int color, unsigned int thickness)
{
    // Implement Bresenham's line algorithm
    int x0 = (int)start.x, y0 = (int)start.y;
    int x1 = (int)end.x, y1 = (int)end.y;
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        setPixel(window->surface, x0, y0, color, 0);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void drawRectangleOnSurface(VWindow* window, int x, int y, int width, int height, unsigned int color, unsigned int thickness) {
    // Draw top and bottom horizontal lines
    for (int i = 0; i < width; i++) {
        for (int t = 0; t < thickness; t++) {
            setPixel(window->surface, x + i, y + t, color, 0);
            setPixel(window->surface, x + i, y + height - 1 - t, color, 0);
        }
    }
    // Draw left and right vertical lines
    for (int i = 0; i < height; i++) {
        for (int t = 0; t < thickness; t++) {
            setPixel(window->surface, x + t, y + i, color, 0);
            setPixel(window->surface, x + width - 1 - t, y + i, color, 0);
        }
    }
}

// Helper function to calculate intensity based on distance
float intensityFromDistance(float distance, float thickness) {
    float intensity = 1.0f - (distance / (thickness / 2.0f));
    return fmaxf(0.0f, fminf(intensity, 1.0f));
}

// Helper function to blend colors
unsigned int blendColors(unsigned int bg, unsigned int fg, float alpha) {
    unsigned char r1 = (bg >> 16) & 0xFF;
    unsigned char g1 = (bg >> 8) & 0xFF;
    unsigned char b1 = bg & 0xFF;
    unsigned char r2 = (fg >> 16) & 0xFF;
    unsigned char g2 = (fg >> 8) & 0xFF;
    unsigned char b2 = fg & 0xFF;

    unsigned char r = (unsigned char)(r1 * (1 - alpha) + r2 * alpha);
    unsigned char g = (unsigned char)(g1 * (1 - alpha) + g2 * alpha);
    unsigned char b = (unsigned char)(b1 * (1 - alpha) + b2 * alpha);

    return (r << 16) | (g << 8) | b;
}

void drawLineOnSurface2(VWindow* window, vec2 start, vec2 end, unsigned int color, unsigned int thickness)
{
    int x0 = (int)start.x, y0 = (int)start.y;
    int x1 = (int)end.x, y1 = (int)end.y;
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx - dy, e2, x2;
    float ed = dx + dy == 0 ? 1 : sqrtf((float)dx*dx + (float)dy*dy);

    float wd = (float)thickness - 1.0f;
    
    for (;;) {
        float distance = fabsf(err - dx + dy) / ed;
        float intensity = intensityFromDistance(distance, wd);
        unsigned int blendedColor = blendColors(0, color, intensity);
        setPixel(window->surface, x0, y0, blendedColor, 1);

        // Check perpendicular pixels
        intensity = intensityFromDistance(distance + 0.5f, wd);
        blendedColor = blendColors(0, color, intensity);
        setPixel(window->surface, x0 + sy, y0 + sx, blendedColor, 1);
        setPixel(window->surface, x0 - sy, y0 - sx, blendedColor, 1);

        e2 = err; x2 = x0;
        if (2*e2 >= -dx) {
            if (x0 == x1) break;
            if (e2 + dy < ed) {
                intensity = intensityFromDistance((e2 + dy) / ed, wd);
                blendedColor = blendColors(0, color, intensity);
                setPixel(window->surface, x0, y0 + sy, blendedColor, 1);
            }
            err -= dy; x0 += sx;
        }
        if (2*e2 <= dy) {
            if (y0 == y1) break;
            if (dx - e2 < ed) {
                intensity = intensityFromDistance((dx - e2) / ed, wd);
                blendedColor = blendColors(0, color, intensity);
                setPixel(window->surface, x2 + sx, y0, blendedColor, 1);
            }
            err += dx; y0 += sy;
        }
    }
}