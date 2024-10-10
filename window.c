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
        // Free the previous font if it exists
        if (win->font) {
            XFreeFont(win->display, win->font);
        }
        win->font = font;
        XSetFont(win->display, win->gc, win->font->fid);
    }

    // Set the text color
    XSetForeground(win->display, win->gc, textColor);

    XDrawString(win->display, win->backBuffer, win->gc, x, y, text, strlen(text));

    // Don't free the font here, it will be freed in destroyWindow
}