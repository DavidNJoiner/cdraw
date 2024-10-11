#ifndef Window_H
#define Window_H

#include "vec2.h"
#include <stdbool.h>
#include "surface.h"
#include <X11/Xlib.h>

typedef struct VVWindow {
    Display* display;
    Window window; 
    GC gc;
    Surface* surface;
    XImage* ximage;
    XFontStruct* font;
    Pixmap backBuffer;
    XID screen;
    int width;
    int height;
    bool drawQuads;
    bool randomize;
    bool shouldClose;  
    Atom wmDeleteMessage;  
} VWindow;

VWindow* createWindow(int w, int h);

void destroyWindow(VWindow* win);
void drawSurfaceToWindow(VWindow* win);
void handleEvents(VWindow* win);
void drawText(VWindow *win, int x, int y, const char *text, unsigned int color, int textSize);

void clearColor(VWindow* window, unsigned int color);
void drawPoint(VWindow* window, int x, int y, unsigned int color, int size);
void presentWindow(VWindow* window);


#endif //Window_H