#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *evar = getenv("EDITOR");
	printf("The editor environment variable is set to %s\n", evar);
}
