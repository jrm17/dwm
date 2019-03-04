#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    WINDOW *win;

    initscr();
    win = newwin(w.ws_row, w.ws_col, 0, 0);
    box(win, '|', '-');
    touchwin(win);
    wrefresh(win);
    getchar();
    endwin();
    return 0;
}
