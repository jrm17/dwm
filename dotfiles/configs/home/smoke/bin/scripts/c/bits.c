#include <stdio.h>

int set(int *num, int pos)
{
	/* left shift 1, then bitwise OR */
	*num |= (1 << pos);
	return *num;
}

int main()
{
	int num = 4; /* number */
	int pos = 1; /* pos is the position to set the bit */

	num = set(&num, pos);
	printf("%d\n", num);
	return 0;
}
