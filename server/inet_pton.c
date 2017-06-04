#include <stdio.h>
#include <arpa/inet.h>

int main(void)
{
	struct in_addr addr;
	if(1 != inet_pton(AF_INET, "119.75.217.109", (void *)&addr))
	{
		printf("inet_pton error\n");
		return -1;
	}
	printf("%s\n", (char *)&addr);
}
