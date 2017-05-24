#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;

	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error\n");
		return -1;
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(88);
	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("bind error\n");
		return -1;
	}
	if(listen(listenfd, 10) < 0)
	{
		printf("bind error\n");
		return -1;
	}
//	for(;;)
	{
		char buf[1024] = "";
		clilen = sizeof(cliaddr);
		if((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0)
		{
			printf("accept error\n");
			return -1;
		}
		close(listenfd);
		read(connfd, buf, sizeof(buf));
		printf(buf, "\nca");
		close(connfd);
	}
	return 0;
}











