#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


int main(void)
{
	char ipv6_addr[128] = "";
	struct in6_addr addr6 = IN6ADDR_ANY_INIT;

	if(inet_ntop(AF_INET6, (void *)&addr6, ipv6_addr, sizeof(ipv6_addr)) != NULL)
	{
		printf("%s\n", ipv6_addr);
	}
	else
	{
		printf("inet_ntop error: %s\n", strerror(errno));
	}
	return 0;
}
