#include <stdio.h>
#define BINMAPSHIFT 5
                                   /*make hight bit zero*/
#define idx2bit(i) ((1U << ((i) & ((1U << BINMAPSHIFT) - 1))))
/*#define idx2bit(i) ((1U << ((i))))*/


int main(void)
{
	int i = 0; 
	for(i = 0; i < 8; i++)
	{
		printf("%d\t", idx2bit(i));
	}
	printf("\n");
}
