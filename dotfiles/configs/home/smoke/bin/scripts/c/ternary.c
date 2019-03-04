#include <stdio.h>

int main()
{
	int i, j, n;
	j = 10;
	n = 12;
	i = 2;
	printf("%d\n", j - ((i) ? n / i : 1));
}
