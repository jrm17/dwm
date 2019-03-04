#include <ncurses.h>

#define __DEBUG__ 1


typedef struct Border {
	chtype left, right, top, bot,
	       tleft, tright, bleft, bright;
} Border;

typedef struct Window {
	int x, y;
	int h, w;
	Border bd;
} Window;


void wininit(Window *win)
{
	// size
	win->h = 5;
	win->w = 15;
	win->x = (COLS - win->w) / 2;
	win->y = (LINES - win->h) / 2;

	// sides
	win->bd.left  = ACS_VLINE;
	win->bd.right = ACS_VLINE;
	win->bd.top   = ACS_HLINE;
	win->bd.bot   = ACS_HLINE;

	// corners
	win->bd.tleft  = ACS_ULCORNER;
	win->bd.tright = ACS_URCORNER;
	win->bd.bleft  = ACS_LLCORNER;
	win->bd.bright = ACS_LRCORNER;
}

void windbg(Window *win)
{
#ifdef __DEBUG__
	mvprintw(2, 0, "%d %d %d %d", win->x, win->y, win->w, win->h);
	refresh();
#endif
}

void draw_box(Window *win, int draw_flag)
{
	int x = win->x;
	int y = win->y;
	int w = win->w;
	int h = win->h;

	if (draw_flag) {
		mvaddch(y, x, win->bd.tleft);           // top left corner
		mvaddch(y, x + w, win->bd.tright);      // top right corner
		mvaddch(y + h, x, win->bd.bleft);       // bottom left corner
		mvaddch(y + h, x + w, win->bd.bright);  // bottom right corner
		mvhline(y, x + 1, win->bd.top, w - 1);      // top side
		mvhline(y + h, x + 1, win->bd.bot, w - 1);  // bottom side
		mvvline(y + 1, x, win->bd.left, h -1);      // left side
		mvvline(y + 1, x + w, win->bd.right, h -1); // right side
	} else {
		for (int j = y; j <= y + h; ++j)
			for (int i = x; i <= x + w; ++i)
				mvaddch(j, i, ' ');
	}
	refresh();
}

int main(int argc, char *argv[])
{
	Window win;
	int ch;

	initscr();         // start curses
	cbreak();          // don't buffer input
	noecho();          // don't show the chars typed
	start_color();     // enable colour modes
	keypad(stdscr, 1); // handle function, numpad and other keys

	// setup
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	wininit(&win);
	windbg(&win);
	attron(COLOR_PAIR(1));
	mvprintw(1, 0, "Press q to exit");
	refresh();
	attroff(COLOR_PAIR(1));
	draw_box(&win, 1);

	while ((ch = getch()) != 'q') {
		switch (ch) {
			case 'h':     // fall through
			case KEY_LEFT:
				if (win.x) {
					draw_box(&win, 0);
					--win.x;
					draw_box(&win, 1);
				}
				break;
			case 'l':     // fall through
			case KEY_RIGHT:
				if (win.x + win.w < COLS - 1) {
					draw_box(&win, 0);
					++win.x;
					draw_box(&win, 1);
				}
				break;
			case 'k':     // fall through
			case KEY_UP:
				if (win.y) {
					draw_box(&win, 0);
					--win.y;
					draw_box(&win, 1);
				}
				break;
			case 'j':     // fall through
			case KEY_DOWN:
				if (win.y + win.h < LINES - 1) {
					draw_box(&win, 0);
					++win.y;
					draw_box(&win, 1);
				}
				break;
		}
		windbg(&win);
	}
	endwin();
	return 0;
}
