#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define LENGTH(X) (sizeof X / sizeof X[0])

static struct termios oldt, newt;

typedef struct Jump Jump;

struct Jump {
	int key;
	char *path;
};

static const Jump jumps[] = {
	{ 'g', "/media/wdblue/git" },
	{ '.', "/home/smoke/.config" },
	{ 'v', "/home/smoke/.config/nvim" },
};

void cfmakeraw(struct termios *termios_p);

int grabkey(void)
{
	int key;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	cfmakeraw(&newt);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	key = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return key;
}

int main(int argc, char const* argv[])
{
	char *path = NULL;
	int key = grabkey();
	size_t s = LENGTH(jumps);

	for (int i = 0; i < s; i++) {
		if (key == jumps[i].key) {
			path = jumps[i].path;
			break;
		}
	}

	if (path != NULL) {
	}

	return 0;
}
