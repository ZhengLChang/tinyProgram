#include "lib.h"

int main(int argc, char **argv)
{
	int sockfd, n;
	char recvline[1024];
	socklen_t salen;
	struct sockaddr *sa;
	if(argc != 3)
	{
		printf("usage: daytimeudpcli_1 <hostname/IPaddress> <service/port#>\n");
		return -1;
	}
	sockfd = udp_client(argv[1], argv[2], (void **)&sa, &salen);
	if(sockfd < 0)
	{
		return -1;
	}
	printf("sending to %s\n", sock_ntop_host(sa, salen));
	sendto(sockfd, "", 1, 0, sa, salen);
	n = recvfrom(sockfd, servline, 1024, 0, NULL, NULL);
	recvline[n] = 'n';
	fputs(recvline, stdout);
	exit(0);
}
