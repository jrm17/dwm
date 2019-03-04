#include <stdio.h>
#include <stdlib.h>

union intParts {
  int theInt;
  char bytes[sizeof(int)];
};

struct operator {
	int type;
	union {
		int intNum;
		float floatNum;
		double doubleNum;
	} types;
};

typedef struct {
	char *name;
	int age;
} person;

void randfunc(int arg)
{
	printf("Signature: randfunc(int %d);\n", arg);
}

int compare(const void *left, const void *right)
{
	return (*(int *)right - *(int *)left);
}

void funcptr()
{
	/* define the function 'template' */
	void (*fptr)(int);

	fptr = &randfunc;

	/* this is the function call */
	(fptr)(5);
}

void funcptr2()
{
	int (*cmp)(const void*, const void*);
	cmp = &compare;

	int arr[] = {1,2,3,4,5,6,7,8,9};
	qsort(arr, sizeof(arr) / sizeof(*arr), sizeof(*arr), cmp);

	int c = 0;
	while (c < sizeof(arr) / sizeof(*arr)) {
		printf("%d \t", arr[c]);
		c++;
	}
}

void dynmem()
{
	person *p;

	p = malloc(sizeof(person));

	p->name = "Nate";
	p->age = 26;

	printf("p->name = %s\n", p->name);
	printf("p->age = %d\n", p->age);

	/* free the memory allocated from calling malloc() */
	free(p);
}

void exunion()
{
	/* union style */
	union intParts parts;
	parts.theInt = 5968145;
	printf("The int is %i\nThe bytes are [%i, %i, %i, %i]\n",
			parts.theInt, parts.bytes[0], parts.bytes[1],
			parts.bytes[2], parts.bytes[3]);

	/* int style */
	int theInt = parts.theInt;
	printf("The int is %i\nThe bytes are [%i, %i, %i, %i]\n", theInt,
			*((char *)&theInt + 0), *((char *)&theInt + 1),
			*((char *)&theInt + 2), *((char*)&theInt + 3));

	/* or with array syntax */
	printf("The int is %i\nThe bytes are [%i, %i, %i, %i]\n", theInt,
			((char*)&theInt)[0], ((char*)&theInt)[1],
			((char*)&theInt)[2], ((char*)&theInt)[3]);
}

int main()
{
	funcptr();
	funcptr2();
	dynmem();
	exunion();
	return 0;
}
