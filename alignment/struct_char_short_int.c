#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

struct Foo
{
	char x;
	short y;
	int z;
};


int main(void)
{
	struct Foo f;
	printf("size = %d\n", sizeof(f));
	printf("%p %p %p\n", &f.x, 
			&f.y,
			&f.z);
	return 0;
}
