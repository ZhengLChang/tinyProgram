#include "lib.h"

static pid_t
child_make(int i, int listenfd, int addrlen);
static void
child_main(int i, int listenfd, int addrlen);

static int nchildren;
static pid_t *pids;
long *cptr, *meter(int);

int
main(int argc, char **argv)
{
	int listenfd, i;
	socklen_t addrlen;
	void sig_int(int);

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
		printf("usage: serv_pthread_lock [<host>] <port#><#children>\n");
		return -1;
	}
	nchildren = atoi(argv[argc - 1]);
	pids = calloc(nchildren, sizeof(pid_t));
	
	cptr = meter(nchildren);
	my_lock_init(NULL);
	for(i = 0; i < nchildren; i++)
	{
		pids[i] = child_make(i, listenfd, addrlen);
	}
	signal(SIGINT, sig_int);
	for(;;)
		pause();
}

void
sig_int(int signo)
{
	int i;
	void pr_cpu_time(void);

	for(i = 0; i < nchildren; i++)
	{
		kill(pids[i], SIGTERM);
	}
	while(wait(NULL) > 0)
		;
	if(errno != ECHILD)
	{
		printf("wait error: %s\n", strerror(errno));
		return;
	}
	pr_cpu_time();
	for(i = 0; i < nchildren; i++)
	{
		printf("child %d, %ld connections\n", i, cptr[i]);
	}
	exit(0);
}



static pid_t
child_make(int i, int listenfd, int addrlen)
{
	pid_t pid;
	
	if((pid = fork()) > 0)
	{
		return pid;
	}
	child_main(i, listenfd, addrlen);
}

static void
child_main(int i, int listenfd, int addrlen)
{
	int connfd;
	void web_child(int);
	socklen_t clilen;
	struct sockaddr *cliaddr;
	
	cliaddr = malloc(addrlen);
	printf("child %ld starting\n", (long)getpid());
	for(; ;)
	{
		clilen = addrlen;
		my_lock_wait();
		connfd = accept(listenfd, cliaddr, &clilen);
		if(connfd < 0)
		{
			printf("accept error: %s\n", strerror(errno));
			return;
		}
		cptr[i]++;
		my_lock_release();
		
		web_child(connfd);
		close(connfd);
	}
}
