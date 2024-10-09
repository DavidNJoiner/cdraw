#include "window.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <X11/keysym.h>

VWindow* createWindow(int w, int h) {
    VWindow* win = (VWindow*)malloc(sizeof(VWindow));
    if (!win) {
        fprintf(stderr, "Failed to allocate memory for VWindow\n");
        return NULL;
    }

    win->display = XOpenDisplay(NULL);
    if (win->display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        free(win);
        return NULL;
    }

    win->screen = DefaultScreen(win->display);
    win->window = XCreateSimpleWindow(win->display, RootWindow(win->display, win->screen), 
                                      10, 10, w, h, 1,
                                      BlackPixel(win->display, win->screen), 
                                      WhitePixel(win->display, win->screen));

    XSelectInput(win->display, win->window, ExposureMask | KeyPressMask);
    XMapWindow(win->display, win->window);

    win->gc = XCreateGC(win->display, win->window, 0, NULL);
    win->surface = createSurface(w, h);
    
    if (!win->surface) {
        fprintf(stderr, "Failed to create surface\n");
        XFreeGC(win->display, win->gc);
        XDestroyWindow(win->display, win->window);
        XCloseDisplay(win->display);
        free(win);
        return NULL;
    }

    win->ximage = XCreateImage(win->display, DefaultVisual(win->display, win->screen),
                           24, ZPixmap, 0, NULL, w, h, 32, 0); 

    if (win->ximage) {
        win->ximage->data = malloc(win->ximage->bytes_per_line * h);
        if (!win->ximage->data) {
            fprintf(stderr, "Failed to allocate memory for XImage data\n");
            XDestroyImage(win->ximage);
            win->ximage = NULL;
        }
    }

    
    // Create a larger font
    XFontStruct *font = XLoadQueryFont(win->display, "-*-helvetica-bold-r-*-*-18-*-*-*-*-*-*-*");
    if (font == NULL) {
        fprintf(stderr, "Failed to load font\n");
        // Handle error (use a default font or exit)
    } else {
        // Set the font in the GC
        XSetFont(win->display, win->gc, font->fid);
    }

     // Create back buffer
    win->backBuffer = XCreatePixmap(win->display, win->window, w, h, 
                                    DefaultDepth(win->display, win->screen));
    if (win->backBuffer == None) {
        fprintf(stderr, "Failed to create back buffer\n");
        // Clean up and return NULL
    }

    win->drawQuads = true;
    win->randomize = false;

    return win;
}

void destroyWindow(VWindow* win) {
    if (win) {
        if (win->font) XFreeFont(win->display, win->font);
        if (win->ximage) XDestroyImage(win->ximage);
        if (win->gc) XFreeGC(win->display, win->gc);
        if (win->window) XDestroyWindow(win->display, win->window);
        if (win->display) XCloseDisplay(win->display);
        if (win->surface) freeSurface(win->surface);
        if (win->backBuffer) XFreePixmap(win->display, win->backBuffer);
        free(win);
    }
}

void drawSurfaceToWindow(VWindow* win) {
    if (!win || !win->display || !win->window || !win->gc || !win->ximage || !win->surface || !win->backBuffer) {
        fprintf(stderr, "Error: Invalid WindowWrapper state in drawSurfaceToWindow\n");
        return;
    }
    
    // Copy surface data to XImage
    memcpy(win->ximage->data, win->surface->pixels, win->surface->width * win->surface->height * sizeof(uint32_t));

    // Draw to back buffer
    XPutImage(win->display, win->backBuffer, win->gc, win->ximage, 0, 0, 0, 0, 
              win->surface->width, win->surface->height);

    // Copy back buffer to window
    XCopyArea(win->display, win->backBuffer, win->window, win->gc, 
              0, 0, win->surface->width, win->surface->height, 0, 0);

    XFlush(win->display);
}

void handleEvents(VWindow* win) {
    XEvent event;
    while (XPending(win->display) > 0) {
        XNextEvent(win->display, &event);
        if (event.type == Expose) {
            drawSurfaceToWindow(win);  
        }
        if (event.type == KeyPress) {
            KeySym key = XLookupKeysym(&event.xkey, 0);
            if (key == XK_space) {
                win->drawQuads = !win->drawQuads;  
                drawSurfaceToWindow(win);
            } else if (key == XK_Escape) {
                return;  
            } else if (key == XK_r) {
                win->randomize = true;
            }
        }
    }
}

void drawText(VWindow *win, int x, int y, const char *text, uint32_t textColor, int textSize) {
    char fontName[256];
    snprintf(fontName, sizeof(fontName), "-*-helvetica-medium-r-*-*-%d-*-*-*-*-*-*-*", textSize);
    
    XFontStruct *font = XLoadQueryFont(win->display, fontName);
    if (font == NULL) {
        fprintf(stderr, "Failed to load font size %d\n", textSize);
        // Fallback to a default font if the requested size is not available
        font = XLoadQueryFont(win->display, "fixed");
    }
    
    if (font != NULL) {
        XSetFont(win->display, win->gc, font->fid);
    }

    // Set the text color
    XSetForeground(win->display, win->gc, textColor);

    XDrawString(win->display, win->backBuffer, win->gc, x, y, text, strlen(text));

    if (font != NULL) {
        XFreeFont(win->display, font);
    }

    // Reset to default color if needed
    // XSetForeground(win->display, win->gc, defaultColor);
}