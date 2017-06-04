#include "lib.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>



int main(int argc, char **argv)
{
	int sockfd, n;
	socklen_t len;
	char recvline[1024];
	struct sockaddr_storage ss;

	if(argc != 3)
	{
		printf("usage: daytimetcpcli <hostname/IPaddress> <service or port#>\n");
		return -1;
	}
	sockfd = tcp_connect(argv[1], argv[2]);
	if(sockfd < 0)
	{
		printf("connect error: %s\n", strerror(errno));
		return -1;
	}
	len = sizeof(ss);
	if(getpeername(sockfd, (struct sockaddr *)&ss, &len) < 0)
	{
		printf("getpeername error: %s\n", strerror(errno));
		return -1;
	}
	printf("connect to %s\n", sock_ntop_host((struct sockaddr *)&ss, len, recvline, sizeof(recvline)));
	while((n = read(sockfd, recvline, 1024)) > 0)
	{
		recvline[n] = '\0';
		fputs(recvline, stdout);
		printf("\n");
	}
	exit(0);
}
