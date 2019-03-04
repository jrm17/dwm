/* example of missing the & in scanf() can cause memory problems */
#include <stdio.h>

int main()
{
    int a;
    scanf("%d", a);
    printf("%d\n", a);
    return 0;
}
