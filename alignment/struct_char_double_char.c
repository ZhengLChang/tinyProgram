#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

struct Foo
{
	char x;
	double y;
	char z;
	int m;
};


int main(void)
{
	struct Foo f;
	printf("size = %d\n", sizeof(f));
	printf("sizeof(double) = %d\n", sizeof(double));
	printf("%p %p %p %p\n", &f.x, 
			&f.y,
			&f.z,
			&f.m);
	return 0;
}
