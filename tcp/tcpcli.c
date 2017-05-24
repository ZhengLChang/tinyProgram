#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error\n");
		return -1;
	}
	
	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(88);
	if(1 != inet_pton(AF_INET, "192.168.1.103", (void *)&servaddr.sin_addr))
	{
		printf("inet_pton error\n");
		return -1;
	}
	if(0  != connect(sockfd, (struct sockaddr *)&servaddr, (socklen_t)sizeof(servaddr)))
	{
		printf("accetp error\n");
		return -1;
	}
	write(sockfd, "zheng", sizeof("zheng") - 1);
	close(sockfd);	
	return 0;
}

