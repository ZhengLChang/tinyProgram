#include <stdio.h>

int function(int a, int b, int c);

void main()
{
	int x;

	x = 0;
	function(1, 2, 3);
	x = 1;
	printf("%d\n", x);
}


int function(int a, int b, int c)
{
	char buffer[14];
	int sum;
	int *ret;
	int i = 0;
	
	for(i = 1; i < 40; i++)
	{
		if((*((int *)buffer + i)) & 0xf000000 == 0x8000000)
		{
			printf("i = %d, address = %x\n", i, ((int *)buffer + i));
			break;
		}
	}
#if 0
	ret = (int *)(buffer + 0x30);
	printf("%x\n", ret);
	(*ret) += 10;
	sum = a + b + c;
#endif
	return sum;
}

