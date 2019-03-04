#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <xcb/xcb.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

#define BORDERWIDTH   2
#define FOCUSCOL      0x999999
#define UNFOCUSCOL    0x000000
#define MODKEY        XCB_MOD_MASK_1
#define CLEANMASK(m)  ((m & ~0x80))

enum { INACTIVE, ACTIVE };

typedef union Arg Arg;

union Arg {
	int i;
	unsigned int ui;
	float f;
	float sf;
	const void *v;
};

/* static const char *sterm[] = { "st", NULL }; */
/* static const char *vtogg[] = { "pamixer", "-t", NULL }; */
/* static const char *vincr[] = { "pamixer", "-i", "2", NULL }; */
/* static const char *vdecr[] = { "pamixer", "-d", "2", NULL }; */
static const char *dmenu[] = { "dmenu_run", NULL };

static xcb_connection_t *conn;
static xcb_screen_t     *scr;
static xcb_window_t     focuswin;

static int deploy(void);
static void cleanup(void);
static void spawn(const Arg *arg);
static void focus(xcb_window_t, int);
static void subscribe(xcb_window_t);

static void spawn(const Arg *arg)
{
	if (fork() == 0) {
		cleanup();
		setsid();
		execvp(((char **)arg->v)[0], (char **)arg->v);
		fprintf(stderr, "wm: execvp %s", ((char **)arg->v)[0]);
		perror(" failed");
		exit(EXIT_SUCCESS);
	}
}

static void cleanup(void)
{
	if (conn != NULL) xcb_disconnect(conn);
}

static int deploy(void)
{
	/* init xcb and grab events */
	uint32_t values[2];
	int mask;

	if (xcb_connection_has_error(conn = xcb_connect(NULL, NULL))) return -1;

	scr = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
	focuswin = scr->root;

	xcb_grab_button(conn, 0, scr->root, XCB_EVENT_MASK_BUTTON_PRESS|XCB_EVENT_MASK_BUTTON_RELEASE,
			XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, scr->root, XCB_NONE, 1, MODKEY);
	xcb_grab_button(conn, 0, scr->root, XCB_EVENT_MASK_BUTTON_PRESS|XCB_EVENT_MASK_BUTTON_RELEASE,
			XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, scr->root, XCB_NONE, 3, MODKEY);

	mask = XCB_CW_EVENT_MASK;
	values[0] = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
	xcb_change_window_attributes_checked(conn, scr->root, mask, values);

	xcb_flush(conn);

	return 0;
}

static void focus(xcb_window_t win, int mode)
{
	uint32_t values[1];
	values[0] = mode ? FOCUSCOL : UNFOCUSCOL;
	xcb_change_window_attributes(conn, win, XCB_CW_BORDER_PIXEL, values);

	if (mode == ACTIVE) {
		xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT, win, XCB_CURRENT_TIME);
		if (win != focuswin) {
			focus(focuswin, INACTIVE);
			focuswin = win;
		}
	}
}

static void subscribe(xcb_window_t win)
{
	uint32_t values[2];

	values[0] = XCB_EVENT_MASK_ENTER_WINDOW;
	values[1] = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
	xcb_change_window_attributes(conn, win, XCB_CW_EVENT_MASK, values);
	values[0] = BORDERWIDTH;
	xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_BORDER_WIDTH, values);
}

static void events_loop(void)
{
	xcb_generic_event_t *ev;
	uint32_t values[3];
	xcb_get_geometry_reply_t *geom;
	xcb_window_t win = 0;

	for (;;) {
		ev = xcb_wait_for_event(conn);
		if (!ev) errx(1, "xcb connection broken");
		switch (CLEANMASK(ev->response_type)) {
		case XCB_CREATE_NOTIFY: {
			xcb_create_notify_event_t *e;
			e = (xcb_create_notify_event_t *)ev;
			if (!e->override_redirect) {
				subscribe(e->window);
				focus(e->window, ACTIVE);
			}
		} break;
		case XCB_DESTROY_NOTIFY: {
			xcb_destroy_notify_event_t *e;
			e = (xcb_destroy_notify_event_t *)ev;
			xcb_kill_client(conn, e->window);
		} break;
		case XCB_ENTER_NOTIFY: {
			xcb_enter_notify_event_t *e;
			e = (xcb_enter_notify_event_t *)ev;
			focus(e->event, ACTIVE);
		} break;
		case XCB_MAP_NOTIFY: {
			xcb_map_notify_event_t *e;
			e = (xcb_map_notify_event_t *)ev;
			if (!e->override_redirect) {
				xcb_map_window(conn, e->window);
				focus(e->window, ACTIVE);
			}
		} break;
		case XCB_KEY_PRESS: {
			const Arg a = {.v = dmenu};
			spawn(&a);
		}
		case XCB_BUTTON_PRESS: {
			xcb_button_press_event_t *e;
			e = (xcb_button_press_event_t *)ev;
			win = e->child;
			if (!win || win == scr->root) break;
			values[0] = XCB_STACK_MODE_ABOVE;
			xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_STACK_MODE, values);
			geom = xcb_get_geometry_reply(conn, xcb_get_geometry(conn, win), NULL);
			if (e->detail == 1) {
				values[2] = 1;
				xcb_warp_pointer(conn, XCB_NONE, win, 0, 0, 0, 0, geom->width/2, geom->height/2);
			} else {
				values[2] = 3;
				xcb_warp_pointer(conn, XCB_NONE, win, 0, 0, 0, 0, geom->width, geom->height);
			}
			xcb_grab_pointer(conn, 0, scr->root,
				XCB_EVENT_MASK_BUTTON_RELEASE|XCB_EVENT_MASK_BUTTON_MOTION|XCB_EVENT_MASK_POINTER_MOTION_HINT,
				XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, scr->root, XCB_NONE, XCB_CURRENT_TIME);
			xcb_flush(conn);
		} break;
		case XCB_MOTION_NOTIFY: {
			xcb_query_pointer_reply_t *pointer;
			pointer = xcb_query_pointer_reply(conn, xcb_query_pointer(conn, scr->root), 0);
			if (values[2] == 1) {
				geom = xcb_get_geometry_reply(conn, xcb_get_geometry(conn, win), NULL);
				if (!geom) break;
				values[0] = (pointer->root_x + geom->width / 2 > scr->width_in_pixels - (BORDERWIDTH*2))
					? scr->width_in_pixels - geom->width - (BORDERWIDTH*2)
					: pointer->root_x - geom->width / 2;
				values[1] = (pointer->root_y + geom->height / 2 > scr->height_in_pixels - (BORDERWIDTH*2))
					? (scr->height_in_pixels - geom->height - (BORDERWIDTH*2))
					: pointer->root_y - geom->height / 2;
				if (pointer->root_x < geom->width/2) values[0] = 0;
				if (pointer->root_y < geom->height/2) values[1] = 0;
				xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y, values);
				xcb_flush(conn);
			} else if (values[2] == 3) {
				geom = xcb_get_geometry_reply(conn, xcb_get_geometry(conn, win), NULL);
				values[0] = pointer->root_x - geom->x;
				values[1] = pointer->root_y - geom->y;
				xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT, values);
				xcb_flush(conn);
			}
		} break;
		case XCB_BUTTON_RELEASE:
			focus(win, ACTIVE);
			xcb_ungrab_pointer(conn, XCB_CURRENT_TIME);
			break;
		case XCB_CONFIGURE_NOTIFY: {
			xcb_configure_notify_event_t *e;
			e = (xcb_configure_notify_event_t *)ev;
			if (e->window != focuswin) focus(e->window, INACTIVE);
			focus(focuswin, ACTIVE);
		} break;
		}
		xcb_flush(conn);
		free(ev);
	}
}

int main(void)
{
	atexit(cleanup);
	if (deploy() < 0) errx(EXIT_FAILURE, "error connecting to X");
	for (;;) events_loop();
	return EXIT_FAILURE;
}
