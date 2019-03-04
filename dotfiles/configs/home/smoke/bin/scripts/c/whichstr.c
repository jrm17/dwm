#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

static const char which[] = "which ";
void *memcpy(void *str1, const void *str2, size_t n);

int main(int argc, const char *argv[])
{
	if (argc < 2)
		return 1;

	char cmd[128];
	int n, i = 6;

	memcpy(cmd, which, i);
	for (n = 0; argv[1][n] != '\0' && i < 110; n++, i++)
		cmd[i] = argv[1][n];
	cmd[i] = '\0';
	printf("'%s' is %d chars long\n", cmd, i);

	if (system(cmd) != 0)
		printf("'%s' is not and available command\n", cmd);
	else
		printf("'%s' is an available command\n", cmd);
	return 0;
}
