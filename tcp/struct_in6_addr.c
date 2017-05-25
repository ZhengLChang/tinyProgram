#include <arpa/inet.h>
#include <stdio.h>

/*128*/
int main(void)
{
	printf("%d\n", sizeof(struct in6_addr));
	return 0;
}
