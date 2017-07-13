#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(void)
{
	struct addrinfo hints, *res, *ressave;
	char str[128];

	memset(&hints, 0, sizeof(hints));

	//hints.ai_flags = AI_V4MAPPED;
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;

	if(0 != getaddrinfo(NULL, "8080", &hints, &res))
	{
		printf("getaddrinfo error: %s\n", strerror(errno));
		return -1;
	}
	ressave = res;
	for(; res != NULL; res = res->ai_next)
	{
		printf("\t%s\n", inet_ntop(AF_INET6, (void *)&((struct sockaddr_in6 *)res->ai_addr)->sin6_addr, str, sizeof(str)));
	}
	freeaddrinfo(ressave);

	return 0;
}
