#include "lib.h" 

#define MAXN 16384
#define MAXLINE 1024

int main(int argc, char **argv)
{
	int i, j, fd, nchildren, nloops, nbytes;
	pid_t pid;
	ssize_t n;
	char request[MAXLINE], reply[MAXN];

	if(argc != 6)
	{
		printf("usage: client <hostname or IPaddr> <port> <#children> "
				"<#loops/child> <#bytes/request>\n");
		return -1;
	}
	nchildren = atoi(argv[3]);
	nloops = atoi(argv[4]);
	nbytes = atoi(argv[5]);
	snprintf(request, sizeof(request), "%d\n", nbytes);
	for(i = 0; i < nchildren; i++)
	{
		if((pid = fork()) == 0)
		{
			for(j = 0; j < nloops; j++)
			{
				fd = tcp_connect(argv[1], argv[2]);
				if(fd < 0)
				{
					return -1;
				}
				if(write(fd, request, strlen(request)) < 0)
				{
					printf("write error: %s\n", strerror(errno));
					return -1;
				}
				if((n = read(fd, reply, nbytes)) != nbytes)
				{
					printf("server returned %d bytes\n", n);	
					return -1;
				}
				close(fd);
			}
			printf("child %d done\n", i);
			exit(0);
		}
		else if(pid == -1)
		{
			printf("fork error: %s\n", strerror(errno));
			return -1;
		}
		
	}
	while(wait(NULL) > 0)
		;
	if(errno != ECHILD)
	{
		printf("wait error: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}












