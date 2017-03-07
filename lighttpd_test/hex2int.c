#include <stdio.h>

char hex2int(unsigned char hex)
{
	hex = hex - '0';
	if(hex > 9)
	{
		hex = (hex + '0' - 1) | 0x20;
		hex = hex - 'a' + 11;
	}
	if(hex > 15)
	{
		hex = 0xFF;
	}
	return hex;
}


int main()
{
	printf("%d\n", hex2int('@'));
	return 0;
}
