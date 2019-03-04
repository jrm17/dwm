#include<stdio.h>

int badrunner()
{
	int count = 0;
	count++;
	return count;
}

int goodrunner()
{
	static int count = 0;
	count++;
	return count;
}

unsigned int multiply(unsigned int x, unsigned int y)
{
	if (x == 1) {
		return y;
	} else if (x > 1) {
		return y + multiply(x-1, y);
	} else {
		return 0;
	}
}

int main()
{
	printf("badrunner %d \n", badrunner());
	printf("badrunner %d \n", badrunner());
	printf("goodrunner %d \n", goodrunner());
	printf("goodrunner %d \n", goodrunner());
	printf("3 times 5 is %d\n", multiply(3, 5));
	return 0;
}

