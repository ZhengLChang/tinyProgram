#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>

int main(void)
{
	int sock = -1;
	struct addrinfo hints, *res, *ressave;
	struct sockaddr_in addr;
	socklen_t addrlen;
	char buf[1024];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	
	if(getaddrinfo(NULL, "5858", &hints, &res) != 0)
	{
		printf("getaddrinfo error: %s\n", gai_strerror(errno));
		return -1;
	}
	ressave = res;
	for(; res != NULL; res = res->ai_next)
	{
		int val = 1;
		if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		{
			printf("could not socket: %s\n", strerror(errno));
			continue;
		}
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
		{
			printf("setsockopt error: %s\n", strerror(errno));
			close(sock);
			continue;
		}
		if(bind(sock, res->ai_addr, res->ai_addrlen) < 0)
		{
		printf("could not bind : %s\n", strerror(errno));
			close(sock);
			continue;
		}
		break;
	}
	if(res == NULL)
	{
		printf("could not bind: %s\n", strerror(errno));
		return -1;
	}
	freeaddrinfo(ressave);

#if 1
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	if(getaddrinfo("172.16.0.149", "5859", &hints, &res) != 0)
	{
		printf("getaddrinfo error: %s\n", gai_strerror(errno));
		return -1;
	}
	if(connect(sock, res->ai_addr, res->ai_addrlen) < 0)	
	{
		printf("connect error: %s\n", strerror(errno));
		close(sock);
		return -1;
	}

#endif
	for(; ;)
	{
		char dst[158];
		addrlen = sizeof(addr);
		if(recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&addr, &addrlen) < 0)
		{
			printf("recvfrom error: %s\n", strerror(errno));
			continue;
		}
		if(inet_ntop(AF_INET, &addr, dst, sizeof(dst)) == NULL)
		{
			printf("inet_ntop error\n", strerror(errno));
			continue;
		}
		printf("recv from %s, %s\n", dst, buf);
	}
	close(sock);

	return 0;
}









