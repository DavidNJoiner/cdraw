#ifndef DEFINE_H
#define DEFINE_H

#include <assert.h>

#define WHITE       0xFFFFFFFF
#define RED         0xFFFF0000
#define BLUE        0xFF0000FF
#define GREEN       0xFF00FF00
#define CYAN        0xFF00FFFF
#define LIGHT_GRAY  0xFFD3D3D3
#define DARK_GRAY   0xFF808080
#define BLACK       0xF0000000

#define ROUND(f) ((f) >= 0 ? (int)((f) + 0.5f) : (int)((f) - 0.5f))


#define ASSERT(expr) assert((expr) && #expr)


#endif // DEFINE_H