#include "lib.h"

int childnum = 0;
int
  main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;
	void sig_chld(int), sig_int(int), web_child(int);
	socklen_t clilen, addrlen;
	struct sockaddr *cliaddr;
	
	if(argc == 2)
	{
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	}
	else if(argc == 3)
	{
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	}
	else
	{
		printf("usage: serv01 [<host>] <port#>\n");
		return -1;
	}
	if(listenfd < 0)
	{
		return -1;
	}
	cliaddr = malloc(addrlen);
	if(cliaddr == NULL)
	{
		printf("malloc error: %s\n",strerror(errno));
		return -1;
	}
	signal(SIGCHLD, sig_chld);
	signal(SIGINT, sig_int);
	for(;;)
	{
		clilen = addrlen;
		if((connfd =accept(listenfd, cliaddr, &clilen)) < 0)
		{
			if(errno == EINTR)
				continue;
			else
			{
				printf("accept error\n");
				return -1;
			}
		}
		if((childpid = fork()) == 0)
		{
			close(listenfd);
			web_child(connfd);
			exit(0);
		}
		childnum ++;
		close(connfd);
	}
}

void 
sig_int(int signo)
{
	void pr_cpu_time(void);
	printf("child num %d\n", childnum);
	pr_cpu_time();
	exit(0);
}

void
sig_chld(int signo)
{
	pid_t pid;
	int stat;
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
		;
	}
	return;
}












