#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MALLOC_SIZE(size)  ((size + 4 + 2 * sizeof(size_t) - 1) & (~(2 * sizeof(size_t) - 1)))
#define GET_MALLOC_SIZE(size) (MALLOC_SIZE(size) < 16 ? 16 : MALLOC_SIZE(size))
int main(void)
{
	int needed_size = 1;
	char * str_p = (char *)malloc(needed_size * sizeof(char));
	size_t size = *((size_t *)(str_p - sizeof(size_t))) & (~0x7);
	int i = 1;
	printf("addr = %p\n", str_p);
	printf("%d\n", *((size_t *)(str_p - sizeof(size_t))) & (0x7));
	printf("forecast: size = %d\n", GET_MALLOC_SIZE(needed_size));
	printf("real: size = %d\n", size);
	free(str_p);
	for(i = 1; i < 1000; i++)
	{
		str_p = (char *)malloc(i * sizeof(char));
		if(str_p == NULL || GET_MALLOC_SIZE(i) != (*((size_t *)(str_p - sizeof(size_t))) & (~0x7)))
		{
			printf("forecast fail: i = %d\n", i);
			exit(0);
		}
		free(str_p);
	}
	printf("all right\n");
	//sleep(30);
	return 0;
		
}
	
