#include "window.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <string.h>

VWindow* createWindow(int w, int h) {
    VWindow* win = (VWindow*)malloc(sizeof(VWindow));
    if (!win) {
        fprintf(stderr, "Failed to allocate memory for VWindow\n");
        return NULL;
    }
    
    // Initialize all pointers to NULL
    win->display = NULL;
    win->gc = NULL;
    win->surface = NULL;
    win->ximage = NULL;
    win->font = NULL;
    win->backBuffer = None;
    
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

    win->shouldClose = false;
    win->wmDeleteMessage = XInternAtom(win->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(win->display, win->window, &win->wmDeleteMessage, 1);

    return win;
}

void destroyWindow(VWindow* win) {
    printf("Entering destroyWindow\n");
    if (win) {
        printf("Win is not NULL\n");
        if (win->display) {
            printf("Display is not NULL\n");
            if (win->gc) {
                printf("Freeing GC\n");
                XFreeGC(win->display, win->gc);
                win->gc = NULL;
            }
            if (win->ximage) {
                printf("Destroying XImage\n");
                XDestroyImage(win->ximage);
                win->ximage = NULL;
            }
            if (win->backBuffer) {
                printf("Freeing Pixmap\n");
                XFreePixmap(win->display, win->backBuffer);
                win->backBuffer = None;
            }
            if (win->window) {
                printf("Destroying Window\n");
                XDestroyWindow(win->display, win->window);
                win->window = None;
            }
            if (win->font) {
                printf("Freeing Font\n");
                XFreeFont(win->display, win->font);
                win->font = NULL;
            }
            
            printf("Syncing display\n");
            XSync(win->display, True);
            
            printf("Closing display\n");
            XCloseDisplay(win->display);
            win->display = NULL;
        }
        if (win->surface) {
            printf("Freeing Surface\n");
            freeSurface(win->surface);
            win->surface = NULL;
        }
        printf("Freeing window struct\n");
        free(win);
    }
    printf("Exiting destroyWindow\n");
}

void handleEvents(VWindow* win) {
    XEvent event;
    while (XPending(win->display) > 0) {
        XNextEvent(win->display, &event);
        switch (event.type) {
            case Expose:
                printf("Expose event\n");
                drawSurfaceToWindow(win);
                break;
            case KeyPress:
                {
                    KeySym key = XLookupKeysym(&event.xkey, 0);
                    if (key == XK_space) {
                        printf("Space key pressed\n");
                        win->drawQuads = !win->drawQuads;
                        drawSurfaceToWindow(win);
                    } else if (key == XK_Escape) {
                        printf("Escape key pressed\n");
                        win->shouldClose = true;
                    } else if (key == XK_r) {
                        printf("R key pressed\n");
                        win->randomize = true;
                    }
                }
                break;
            case ClientMessage:
                if ((Atom)event.xclient.data.l[0] == win->wmDeleteMessage) {
                    printf("Window close button clicked\n");
                    win->shouldClose = true;
                }
                break;
        }
    }
}


void clearColor(VWindow* window, unsigned int color)
{
    clearSurface(window->surface, color);
    drawSurfaceToWindow(window);
}

void drawPoint(VWindow* window, int x, int y, unsigned int color, int size)
{
    setPixel(window->surface, x, y, color, size);
    drawSurfaceToWindow(window);
}

void presentWindow(VWindow* window)
{
    // Copy the back buffer to the window
    XCopyArea(window->display, window->backBuffer, window->window, window->gc, 
              0, 0, window->width, window->height, 0, 0);
    XFlush(window->display);
}

void drawSurfaceToWindow(VWindow* window)
{
    if (!window || !window->display || !window->window || !window->gc || !window->ximage || !window->surface || !window->backBuffer) {
        fprintf(stderr, "Error: Invalid WindowWrapper state in drawSurfaceToWindow\n");
        return;
    }
    // Copy surface data to XImage
    window->ximage = surfaceToXImage(window->display, window->surface);
    //memcpy(window->ximage->data, window->surface->pixels, window->surface->width * window->surface->height * sizeof(unsigned int));

    // Draw to back buffer
    XPutImage(window->display, window->backBuffer, window->gc, window->ximage, 0, 0, 0, 0, 
              window->surface->width, window->surface->height);

    // Copy back buffer to window
    XCopyArea(window->display, window->backBuffer, window->window, window->gc, 
              0, 0, window->surface->width, window->surface->height, 0, 0);

    XFlush(window->display);
}

// Existing function, modified to use the new abstractions
void drawText(VWindow* window, int x, int y, const char* text, unsigned int color, int fontSize)
{
    XSetForeground(window->display, window->gc, color);
    XDrawString(window->display, window->backBuffer, window->gc, x, y, text, strlen(text));
}
