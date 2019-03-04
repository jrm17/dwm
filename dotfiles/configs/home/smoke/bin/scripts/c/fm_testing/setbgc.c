#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xmu/CurUtil.h>
#include <X11/Xcursor/Xcursor.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define Dynamic 1

static Display *dpy;
static int screen;
static Window root;
static char *program_name;
static char *fore_color = NULL;
static char *back_color = NULL;
static int reverse = 0;
static int save_colors = 0;
static int unsave_past = 0;
static Pixmap save_pixmap = (Pixmap)None;

/* Free past incarnation if needed, and retain state if needed. */
static void FixupState(void)
{
	Atom prop, type;
	int format;
	unsigned long length, after;
	unsigned char *data;

	if (!(DefaultVisual(dpy, screen)->class & Dynamic))
		unsave_past = 0;
	if (!unsave_past && !save_colors)
		return;
	prop = XInternAtom(dpy, "_XSETROOT_ID", False);
	if (unsave_past) {
		if (XGetWindowProperty(dpy, root, prop, 0L, 1L, True, AnyPropertyType,
					&type, &format, &length, &after, &data) != Success)
			fprintf(stderr, "%s: warning: cannot get _XSETROOT_ID property from root window\n", program_name);
		else if ((type == XA_PIXMAP) && (format == 32) && (length == 1) && (after == 0))
			XKillClient(dpy, *((Pixmap *)data));
		else if (type != None)
			fprintf(stderr, "%s: warning: _XSETROOT_ID property is garbage\n", program_name);
	}
	if (save_colors) {
		if (!save_pixmap)
			save_pixmap = XCreatePixmap(dpy, root, 1, 1, 1);
		XChangeProperty(dpy, root, prop, XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &save_pixmap, 1);
		XSetCloseDownMode(dpy, RetainPermanent);
	}
}

static Pixmap ReadBitmapFile(char *filename, unsigned int *width, unsigned int *height, int *x_hot, int *y_hot)
{
	Pixmap bitmap;
	int status;

	status = XReadBitmapFile(dpy, root, filename, width, height, &bitmap, x_hot, y_hot);
	if (status == BitmapSuccess)
		return(bitmap);
	else if (status == BitmapOpenFailed)
		fprintf(stderr, "%s: can't open file: %s\n", program_name, filename);
	else if (status == BitmapFileInvalid)
		fprintf(stderr, "%s: bad bitmap format file: %s\n", program_name, filename);
	else
		fprintf(stderr, "%s: insufficient memory for bitmap: %s", program_name, filename);
	exit(1);
}

static unsigned long NameToPixel(char *name, unsigned long pixel)
{
	XColor ecolor;

	if (!name || !*name)
		return pixel;
	if (!XParseColor(dpy,DefaultColormap(dpy,screen),name,&ecolor)) {
		fprintf(stderr,"%s:  unknown color \"%s\"\n",program_name,name);
		exit(1);
	}
	if (!XAllocColor(dpy, DefaultColormap(dpy, screen),&ecolor)) {
		fprintf(stderr, "%s:  unable to allocate color for \"%s\"\n", program_name, name);
		exit(1);
	}
	if ((ecolor.pixel != BlackPixel(dpy, screen)) &&
			(ecolor.pixel != WhitePixel(dpy, screen)) &&
			(DefaultVisual(dpy, screen)->class & Dynamic))
		save_colors = 1;
	return(ecolor.pixel);
}

static void SetBackgroundToBitmap(Pixmap bitmap, unsigned int width, unsigned int height)
{
	Pixmap pix;
	GC gc;
	XGCValues gc_init;

	gc_init.foreground = NameToPixel(fore_color, BlackPixel(dpy, screen));
	gc_init.background = NameToPixel(back_color, WhitePixel(dpy, screen));
	if (reverse) {
		unsigned long temp=gc_init.foreground;
		gc_init.foreground=gc_init.background;
		gc_init.background=temp;
	}
	gc = XCreateGC(dpy, root, GCForeground|GCBackground, &gc_init);
	pix = XCreatePixmap(dpy, root, width, height, (unsigned int)DefaultDepth(dpy, screen));
	XCopyPlane(dpy, bitmap, pix, gc, 0, 0, width, height, 0, 0, (unsigned long)1);
	XSetWindowBackgroundPixmap(dpy, root, pix);
	XFreeGC(dpy, gc);
	XFreePixmap(dpy, bitmap);
	if (save_colors)
		save_pixmap = pix;
	else
		XFreePixmap(dpy, pix);
	XClearWindow(dpy, root);
	unsave_past = 1;
}

static void usage(const char *errmsg)
{
	if (errmsg != NULL)
		fprintf (stderr, "%s: %s\n\n", program_name, errmsg);

	fprintf(stderr, "Usage: %s [options]\n%s\n", program_name,
			"  where options are:\n"
			"  -help                           Print this help\n"
			"  -bitmap <filename>              Use the bitmap specified in the file\n"
	       );
	exit(1);
}

int main(int argc, char *argv[])
{
	int i;
	unsigned int ww, hh;
	program_name = argv[0];
	char *bitmap_file = NULL;
	char *display_name = NULL;
	Pixmap bitmap;

	for (i = 1; i < argc; i++) {
		if (!strcmp("-h", argv[i]))
			usage(NULL);
		if (!strcmp("-f", argv[i])) {
			if (++i >= argc)
				usage("-f requires an argument");
			bitmap_file = argv[i];
			continue;
		}
		fprintf(stderr, "%s: unrecognized argument '%s'\n", program_name, argv[i]);
		usage(NULL);
	}

	dpy = XOpenDisplay(display_name);
	if (!dpy) {
		fprintf(stderr, "%s:  unable to open display '%s'\n", program_name, XDisplayName (display_name));
		exit (2);
	}
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);

	/* Handle -bitmap option */
	if (bitmap_file) {
		bitmap = ReadBitmapFile(bitmap_file, &ww, &hh, (int *)NULL, (int *)NULL);
		SetBackgroundToBitmap(bitmap, ww, hh);
	}

	FixupState();
	XCloseDisplay(dpy);
	return 0;
}
