#include <time.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <ncurses.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/stat.h>
#include <linux/limits.h>

/* #include <stdio.h> */

typedef unsigned int uint;
typedef unsigned char uchar;


typedef struct Entry {
	char path[PATH_MAX];
	char name[NAME_MAX];
	uint mode;
	uint colour;
} Entry;

struct border {
	chtype left, right, top, bot, tleft, tright, bleft, bright;
};

typedef struct window {
	int x, y, h, w;
	struct border bd;
} window;


int lstat(const char *path, struct stat *buf);


void mexipager(char *file)
{
	int ch, prev, row, col;
	prev = EOF;
	FILE *fp;
	int y, x;

	if ((fp = fopen(file, "r")) == NULL) {
		perror("Cannot open input file");
		exit(1);
	}
	initscr();
	getmaxyx(stdscr, row, col);
	while((ch = fgetc(fp)) != EOF) {
		getyx(stdscr, y, x);
		if (y == (row - 1)) {
			printw("<-Press Any Key->");
			getch();
			clear();
			move(0, 0);
		}
		if (prev == '/' && ch == '*') {
			attron(A_BOLD);
			getyx(stdscr, y, x);
			move(y, x - 1);
			printw("%c%c", '/', ch);
		} else {
			printw("%c", ch);
		}
		refresh();
		if (prev == '*' && ch == '/')
			attroff(A_BOLD);
		prev = ch;
	}
	endwin();
	fclose(fp);
}

void lsdir(DIR *dir)
{
	char *n;  /* current name */
	int ret, fd, row, col, i = 0;
	time_t rtime;
	struct dirent *d; /* dir entry struct */
	struct stat s; /* file stats */

	time(&rtime);

	if ((fd = dirfd(dir)) == -1)
		return;

	while ((d = readdir(dir)) != NULL && ++i < row) {
		getmaxyx(stdscr, row, col);
		n = d->d_name;
		if (!(n[0] == '.' && (n[1] == '\0' || (n[1] == '.' && n[2] == '\0')))) {
			printw("%s", n);
			if ((ret = lstat(n, &s)) != -1) {
				/* printw("dev: %d\n", s.st_dev); */
				/* printw("ino: %d\n", s.st_ino); */
				printw(" %dKB", s.st_size / 1000);
				printw(" %dSec", rtime - s.st_mtime);
				if (S_ISLNK(s.st_mode) && !fstatat(fd, n, &s, 0))
					printw(" SYM%s-%d", (S_ISDIR(s.st_mode)) ? "DIR" : "FILE", s.st_mode);
				else if (S_ISDIR(s.st_mode))
					printw(" %s-%d", "DIR", s.st_mode);
				else
					printw(" %s-%d", "FILE", s.st_mode);
			}
			printw("\n");
		}
	}
}

char *bitsof(size_t size, void *ptr)
{
	uchar *b = (uchar *)ptr;
	static char *ret;
	int i, j, n = 0;

	ret = malloc(size);
	for (i = size - 1; i >= 0; i--)  /* size - the trailing null */
		for (j = 7; j >= 0; j--) /* bits in the current byte */
			sprintf(ret + n++, "%u", (b[i] >> j) & 1);
	ret[n] = '\0';
	return ret;
}

int main(int argc, char *argv[])
{
	DIR *d;
	int ch;
	char *dir;
	dir = (argc < 2) ? "." : argv[1];

	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();

	if ((d = opendir(dir)) != NULL) {
		lsdir(d);
		closedir(d);
		refresh();
		/* printw("\nType a character to see different representations\n"); */
		/* if((ch = getch()) != EOF) */
		/* 	printw("\nKEY:\n\tASCII:\t%d\n\tPRINT:\t%c\n\tHEX:\t0x%X\n\tBIN:\t0b%s\n", */
		/* 			ch, ch, ch, bitsof(4, &ch)); */
		/* refresh(); */
		getch();
	}
	endwin();
	return 0;
}
