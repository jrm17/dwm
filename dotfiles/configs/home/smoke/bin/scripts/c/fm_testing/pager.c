#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Line {
	unsigned int idx;
	unsigned int len;
	char *txt;
} Line;

typedef struct Buffer {
	unsigned int numlines;
	unsigned int numchars;
	Line *lines[];
} Buffer;

Line *mkline(char *text, int len, int idx)
{
	int i;
	static Line *l;

	l = malloc(sizeof(Line));
	l->idx = idx;
	l->len = len;
	l->txt = calloc(1, len + 1);
	for (i = 0; i < len; i++)
		l->txt[i] = text[i];
	return l;
}

Buffer *mkbuf(void)
{
	static Buffer *b;

	b = malloc(sizeof(Buffer));
	b->numlines = 0;
	b->numchars = 0;

	return b;
}

int fillbuf(FILE *fp, Buffer *b)
{
	char c;
	int i = 0;
	int len = 2048;
	char tmp[len];

	while ((c = getc(fp)) != EOF) {
		tmp[i++] = c;
		b->numchars++;

		if (i + 1 == len) {
			tmp[i] = '\0';
			b->lines[b->numlines] = mkline(tmp, i, b->numlines);
			b->numlines++;
			i = 0;
		} else if (c == '\n' || c == '\r') {
			if (i > 0) {
				tmp[--i] = '\0';
				b->lines[b->numlines] = mkline(tmp, i, b->numlines);
				b->numlines++;
			}
			i = 0;
		}
	}

	if (i && *tmp) { /* last or only line */
		tmp[i] = '\0';
		b->lines[b->numlines] = mkline(tmp, i, b->numlines);
		b->numlines++;
	}

	return b->numlines;
}

int main(int argc, char *argv[])
{
	int l = 0, i = 0;
	Buffer *b;

	if (argc == 1) {
		b = mkbuf();
		l = fillbuf(stdin, b);
	} else {
		FILE *fp = fopen(argv[1], "r");
		if (fp == NULL) {
			printf("error: can't open %s\n", argv[1]);
			return 1;
		} else {
			b = mkbuf();
			l = fillbuf(fp, b);
			fclose(fp);
		}
	}
	for (i = 0; i < l; i++)
		printf("b->lines[%d]->txt: %s\n", i, b->lines[i]->txt);
	printf("b->numlines: %d\n", b->numlines);
	printf("b->numchars: %d\n", b->numchars);

	free(b);
	return 0;
}

/* #include <unistd.h> */
/* #include <termios.h> */
/* #include <sys/ioctl.h> */

/* void cfmakeraw(struct termios *); */

/* static struct termios ot; /1* old terminal settings *1/ */
/* static struct termios nt; /1* new terminal settings *1/ */
/* static struct winsize w;  /1* terminal window size *1/ */

/* int fpage(FILE *fp) */
/* { */
/* 	int ret = 0; */
/* 	int running = 1; */
/* 	/1* int cols, rows; *1/ */

/* 	tcgetattr(STDIN_FILENO, &ot); */
/* 	nt = ot; */
/* 	cfmakeraw(&nt); */
/* 	tcsetattr(STDIN_FILENO, TCSANOW, &nt); */

/* 	while (1) { */
/* 		/1* get terminal size *1/ */
/* 		ioctl(0, TIOCGWINSZ, &w); */
/* 		/1* cols = w.ws_col; *1/ */
/* 		/1* rows = w.ws_row; *1/ */
/* 		switch (getchar()) { */
/* 			case 'q': { */
/* 				running = 0; */
/* 				break; */
/* 			} */
/* 			case 'j': { */
/* 				break; */
/* 			} */
/* 			case 'k': { */
/* 				break; */
/* 			} */
/* 		} */
/* 		if (!running) */
/* 			break; */
/* 	} */

/* 	tcsetattr(STDIN_FILENO, TCSANOW, &ot); */

/* 	return ret; */
/* } */

