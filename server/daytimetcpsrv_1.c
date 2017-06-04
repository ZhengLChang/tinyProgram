#include "lib.h"
#include <time.h>

int 
  main(int argc, char **argv)
{
	int listenfd, connfd;	
	socklen_t len;
	time_t ticks;
	struct sockaddr_storage cliaddr;
	char str[1024] = "";
	char *buff = str;
#if 0	
	if(argc != 2)
	{
		printf("usage: daytimetcpsrv_1 <service or port#>\n");
		return -1;
	}
#endif
	if(argc == 2)
		listenfd = tcp_listen(NULL, argv[1], NULL);
	else if(argc == 3)
		listenfd = tcp_listen(argv[1], argv[2], NULL);
	else
	{
		printf("usage: daytimetcpsrv [<host>] <service or port#>\n");
	}
	if(listenfd < 0)
	{
		printf("listen error: %s\n", strerror(errno));
		return -1;
	}
	for(;;)
	{
		len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
		if(connfd < 0)
		{
			printf("accept error: %s\n", strerror(errno));
			continue;
		}
		printf("connect from %s\n", sock_ntop_host((struct sockaddr *)&cliaddr, len, str, sizeof(str)));
		ticks = time(NULL);
		snprintf(buff, 100, "%.24s\n", ctime(&ticks));
		printf("\t%s", buff);
		write(connfd, buff, 100);
		if(shutdown(connfd, SHUT_RD) == 0)
		{
			if(read(connfd, buff, 100) == 0)
			{
				printf("\tcli read over\n");
			}
			else
			printf("\tcli send: %s\n", buff);
		}
		close(connfd);
	}
	return 0;
}
