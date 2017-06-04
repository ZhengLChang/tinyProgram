#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>


int main(int argc, char **argv)
{
	struct hostent *hp;
	struct in_addr inetaddr;
	struct in_addr *inetaddrp[2];
	struct hostent *hp;
	struct servent *sp;
	int sockfd, n;
	char recvline[1024];
	struct sockaddr_in servaddr;
	if(argc != 3)
	{
		printf("usage: daytimetcpcli <hostname> <service>\n");
		return -1;
	}
	if((hp = gethostbyname(argv[1])) == NULL)
	{
		if(inet_pton(AF_INET, argv[1], (void *)&inetaddr) != 1)
		{
			printf("hostname error for %s: %s\n", argv[1],
					hstrerror(h_errno));
		}
		else
		{
			inetaddrp[0] = &inetaddrp;
			inetaddrp[1] = NULL;
			pptr = inetaddrp;
		}
	}
	else
	{
		pptr = (struct in_addr **)hp->h_addr_list;
	}
	if((sp = getservbyname(argv[2], "tcp")) == NULL)
	{
		printf("getservbynname error for %s\n", argv[2]);
		return -1;
	}
	for(; *pptr != NULL; pptr++)
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd < 0)
		{
			printf("socket error\n");
			return -1;
		}
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = sp->s_port;
		memcpy(&servaddr.sin_addr, *pptr, sizeof);
	}
	return 0;
}











