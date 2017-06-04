#include "unp.h"
#include <time.h>

int main(int argc, char **argv)
{
	int sockfd;
	ssize_t n;
	time_t ticks;
	socklen_t len;
	char buff[1024];
	struct sockaddr_storage cliaddr;

	if(argc == 2)
	{
		sockfd = udp_server(NULL, argv[1], NULL);
	}
	else if(argc == 3)
	{
		sockfd = udp_server(argv[1], argv[2], NULL);
	}
	else
	{
		printf("usage: daytimeudpserv [<host>] <service or prot>\n");
		return -1;
	}
	if(sockfd < 0)
		return -1;
	for(;;)
	{
		len = sizeof(cliaddr);
		n = recvfrom(sockfd, buf, 1024, 0, (struct sockaddr *)&cliaddr, &len);
		printf("datagram form %s\n", sock_ntop_host((struct sockaddr*)&cliaddr, len));
		ticks = time(NULL);
		snprintf(buff, sizeof(buff, ".24s\r\r", ctime(&ticks)));
		sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr *)cliaddr, len);
	}
	return 0;
}











