/* pixcol.c - print rgb or hex colour of the pixel clicked to stdout
 * This software is provided free of charge and WITHOUT warranty.
 *
 *            Written by Nathaniel Maia, December 2018
 *
 * Compile with:
 *
 *      gcc pixcol.c -lX11 -o pixcol
 *
 * To have output copied to the clipboard:
 *
 *      pixcol | xclip -i -selection clipboard
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

static XEvent ev;
static XColor col;
static Cursor cur;
static Window root;
static XImage *img;
static Display *dpy;
static int hexout = 1;

void grab_pixcol(void)
{
	cur = XCreateFontCursor(dpy, XC_crosshair);
	if (XGrabPointer(dpy, root, False, ButtonPressMask,
				GrabModeAsync, GrabModeAsync,
				None, cur, CurrentTime) != GrabSuccess) {
		fprintf(stderr, "error: cannot grab the root pointer\n");
		exit(1);
	}
	XNextEvent(dpy, &ev);
	img = XGetImage(dpy, root, ev.xbutton.x_root, ev.xbutton.y_root,
			1, 1, AllPlanes, XYPixmap);
	col.pixel = XGetPixel(img, 0, 0);
	XQueryColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &col);
	if (hexout)
		fprintf(stdout, "#%06X\n",
				(((col.red / 256) & 0xff) << 16) |
				(((col.green / 256) & 0xff) << 8) |
				((col.blue / 256) & 0xff));
	else
		fprintf(stdout, "rgb(%d, %d, %d)\n",
				col.red / 256,
				col.green / 256,
				col.blue / 256);
	XUngrabPointer(dpy, CurrentTime);
	XFreeCursor(dpy, cur);
	XFree(img);
}

int main(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && argv[i][1] == 'r' && argv[i][2] == '\0') {
			hexout = 0;
		} else if (argv[i][0] == '-' && argv[i][1] == 'h' && argv[i][2] == '\0') {
			fprintf(stdout, "usage: pixcol [-r]\n");
			return 0;
		} else {
			fprintf(stderr, "error: invalid option '%s'\n", argv[i]);
			return 1;
		}
	}

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "error: failed to open display\n");
		return 1;
	}

	root = RootWindow(dpy, DefaultScreen(dpy));
	grab_pixcol();
	XCloseDisplay(dpy);
	return 0;
}
