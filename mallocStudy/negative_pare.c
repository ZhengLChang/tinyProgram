#include <stdio.h>

#define ADD(a, b) ((a) + (b))

int main()
{
	int a = 100;
	printf("%d\n", ADD(1, -a));
	return 0;
}
