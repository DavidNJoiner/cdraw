#ifndef Window_H
#define Window_H

#include "vec2.h"
#include <stdbool.h>
#include "surface.h"
#include <X11/Xlib.h>

typedef struct VVWindow {
    Display* display;
    Window window;  // Use VWindow type from Xlib
    GC gc;
    int screen;
    Surface* surface;
    XImage* ximage;
    XFontStruct* font;
    Pixmap backBuffer;
    bool drawQuads;
    bool randomize;
    bool shouldClose;  // Add this line
    Atom wmDeleteMessage;  // Add this line
} VWindow;

VWindow* createWindow(int w, int h);

void destroyWindow(VWindow* win);
void drawSurfaceToWindow(VWindow* win);
void handleEvents(VWindow* win);
void drawText(VWindow *win, int x, int y, const char *text, uint32_t color, int textSize);


#endif //Window_H