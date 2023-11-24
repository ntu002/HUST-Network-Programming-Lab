#include <stdio.h>

void foo(int *ptr)
{
	*ptr = 20;
}

int main()
{
	int value = 10;

    printf("%d\n", value);

	foo(&value);

    printf("%d", value);
	
	return 0;
}
