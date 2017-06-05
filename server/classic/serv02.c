#include "lib.h"

static int nchildren;
static pid_t *pids;
long *cptr, *meter(int);

int main(int argc, char **argv)
{
	int listenfd, i;
	socklen_t addrlen;
	void sig_int(int);
	pid_t child_make(int, int, int);
	
	if(argc == 3)
	{
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	}
	else if(argc == 4)
	{
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	}
	else
	{
		printf("usage: serv02 [<host#>] <#children>\n");
		return -1;
	}
	nchildren = atoi(argv[argc - 1]);
	pids = calloc(nchildren, sizeof(pid_t));

	cptr = meter(nchildren);
	if(pids == NULL)
	{
		printf("calloc error: %s\n", strerror(errno));
		return -1;
	}

	for(i = 0; i < nchildren; i++)
	{
		pids[i] = child_make(i, listenfd, addrlen);
	}
	signal(SIGINT, sig_int);
	for(;;)
		pause();
}

void sig_int(int signo)
{
	int i;
	void pr_cpu_time(void);

	for(i = 0; i < nchildren; i++)
		kill(pids[i], SIGTERM);
	while(wait(NULL) > 0);

	if(errno != ECHILD)
	{
		printf("wait error\n");
		return;
	}
	pr_cpu_time();

	for(i = 0; i < nchildren; i++)
	{
		printf("child %d, %ld connections\n", i, cptr[i]);
	}
	exit(0);
}




















