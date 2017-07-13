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
	char buf[1024];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	
	if(getaddrinfo(NULL, "322", &hints, &res) != 0)
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

	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	if(getaddrinfo("172.16.0.56", "5858", &hints, &res) != 0)
	{
		printf("getaddrinfo error: %s\n", gai_strerror(errno));
		return -1;
	}
	if(sendto(sock, "test", sizeof("test"), 0, res->ai_addr, res->ai_addrlen) != sizeof("test"))
	{
		printf("sendto error: %s\n", strerror(errno));
		close(sock);
		return -1;
	}
	if(recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL) < 0)
	{
		printf("recvfrom error: %s\n", strerror(errno));
		return -1;
	}
	printf("%s\n", buf);
	close(sock);
	freeaddrinfo(res);

	return 0;
}









