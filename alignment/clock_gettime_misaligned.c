#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Foo
{
	char x;
	short y;
	int z;
}__attribute__((packed));

int main(void)
{
	struct Foo foo;
	struct timespec start, end;
	unsigned long i = 0;

	clock_gettime(CLOCK, &start);
	for(i = 0; i < RUNS; i++)
	{
		foo.z = 1;
		foo.z += 1;
	}
	clock_gettime(CLOCK, &end);
	printf("start: %ld\t%ld\n", start.tv_sec, start.tv_nsec);
	printf("end:   %ld\t%ld\n", end.tv_sec, end.tv_nsec);
	printf("result: %ld\t%ld\n", end.tv_sec - start.tv_sec, end.tv_nsec - start.tv_nsec);
	return 0;
}






















Lq

