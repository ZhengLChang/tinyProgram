#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int r = -1;
	char str[1024] = "";
	struct addrinfo hints, *res, *ressave;

	memset(&hints, 0, sizeof(hints));
#if 0
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
#endif
#if 1
//	hints.ai_flags = AI_CANONNAME;
//	hints.ai_family = AF_INET6;
	hints.ai_family = AF_INET6;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_DGRAM;
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_protocol = IPPROTO_TCP;
#endif
	if(0 != (r = getaddrinfo(NULL, "0", &hints, &res)))
	{
		printf("getaddrinfo error: %s\n", gai_strerror(r));
		return -1;
	}
	ressave = res;
	for( ; res != NULL; res = res->ai_next)
	{
		switch(res->ai_family)
		{
case AF_INET:
		printf("%s addr: %s\n", res->ai_canonname, 
				inet_ntop(res->ai_family, (void *)&((struct sockaddr_in *)(res->ai_addr))->sin_addr, str, (socklen_t)sizeof(str)));
		break;
case AF_INET6:
		printf("%s addr: %s\n", res->ai_canonname, 
				inet_ntop(res->ai_family, (void *)&((struct sockaddr_in6 *)(res->ai_addr))->sin6_addr, str, (socklen_t)sizeof(str)));
		break;
default:
		printf("error\n");
		break;
		}

	}
	freeaddrinfo(ressave);
	return 0;
}
