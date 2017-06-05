#include "lib.h"


struct addrinfo*
  host_serv(const char *host, const char *serv, int family, int socktype);
int
  tcp_connect(const char *host, const char *serv);
char *
  sock_ntop_host(const struct sockaddr *sa, socklen_t salen, char *str, int len);
//char *
//  sock_ntop(const struct sockaddr *sa, socklen_t salen, char *str, int len);
int 
  tcp_listen(const char *host, const char *serv, socklen_t *addrlenp);
	
// udp
int
  udp_client(const char *host, const char *serv, struct sockaddr **saptr, socklen_t *lenp);
int
  udp_connect(const char *host, const char *serv);
int
  udp_server(const char *host, const char *serv, socklen_t *addrlenp);
void
  pr_cpu_time(void);
void
  web_child(int sockfd);
ssize_t
  readline(int fd, void *vptr, size_t maxlen);
ssize_t
  writen(int fd, const void *vptr, size_t n);
pid_t
  child_make(int i, int listenfd, int addrlen);
void
  child_main(int i, int listenfd, int addrlen);
long *
  meter(int nchildren);

struct addrinfo*
  host_serv(const char *host, const char *serv, int family, int socktype)
{
	int n;
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = family;
	hints.ai_socktype = socktype;
	if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
		return NULL;
	return res;
}

int
  tcp_connect(const char *host, const char *serv)
{
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{
		printf("tcp_connect error for %s, %s: %s\n",
				host, serv, gai_strerror(n));
		return -1;
	}
	ressave = res;
	for(; res != NULL; res = res->ai_next)
	{
		sockfd = socket(res->ai_family,
			res->ai_socktype, res->ai_protocol);
		if(sockfd < 0)
			continue;
		if(connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		close(sockfd);
	}
	if(res == NULL)
	{
		printf("tcp_connect error for %s, %s\n", host, serv);
		return -1;	
	}
	freeaddrinfo(ressave);
	return sockfd;
}
/*reentrant*/
char *
  sock_ntop_host(const struct sockaddr *sa, socklen_t salen, char *str, int len)
{
	char portstr[24] = "";
	if(str == NULL || len <= 0)
		return NULL;
	switch(sa->sa_family)
	{
		case AF_INET:
		{
			struct sockaddr_in *sin = (struct sockaddr_in *)sa;
			if(NULL == inet_ntop(AF_INET, &sin->sin_addr, str, len))
			{
				return NULL;
			}
			if(ntohs(sin->sin_port) != 0)
			{
				snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
				strcat(str, portstr);
			}
			return str;
			break;
		}
		case AF_INET6:
		{
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
			if(NULL == inet_ntop(AF_INET6, &sin6->sin6_addr, str, len))
				return NULL;
			if(ntohs(sin6->sin6_port) != 0)
			{
				snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin6->sin6_port));
				strcat(str, portstr);
			}
			return str;
		}
		defualt:
			return NULL;
			break;
	}
	return NULL;
}

int 
  tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
	int listenfd, n;
	const int on = 1;
	struct addrinfo hints, *res, *ressave;
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{
		printf("tcp_listen error for %s, %s: %s",
				host, serv, gai_strerror(n));
		return -1;
	}
	ressave = res;
	for(; res != NULL; res = res->ai_next)
	{
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);	
		if(listenfd < 0)
			continue;
		if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		{
			printf("setsockfd error: %s\n", strerror(errno));
			return -1;
		}
		if(bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		close(listenfd);
	}
	if(res == NULL)
	{
		printf("tcp_listen error for %s, %s\n", host, serv);
		return -1;
	}
	if(listen(listenfd, 10) < 0)
	{
		printf("listen error: %s\n", strerror(errno));
		return -1;
	}
	if(addrlenp)
		*addrlenp = res->ai_addrlen;
	freeaddrinfo(ressave);
	return listenfd;
}


#if 0
char *
  sock_ntop(const struct sockaddr *sa, socklen_t salen, char *str, int len)
{
	char portstr[8];
	if(str == NULL || len <= 0)
		return NULL;
	switch(sa->sa_family)
	{
		case AF_INET:
		{
			struct sockaddr_in *sin = (struct sockaddr_in *)sa;
			if(inete_ntop(AF_INET, &sin->sin_addr, str, len) == NULL)
			{
				return NULL;
			}
			if(ntohs(sin->sin_port))
		}
		case AF_INET6:
		{
		}
	}
	return NULL;
}
#endif


int
  udp_client(const char *host, const char *serv, struct sockaddr **saptr, socklen_t *lenp)
{
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{
		printf("udp_client error for %s, %s: %s\n",
				host, serv, gai_strerror(n));
		return -1;
	}
	for(; res != NULL; res = res->ai_next)
	{
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(sockfd > 0)
			break;
	}
	
	if(res == NULL)
	{
		printf("udp_client error for %s, %s\n", host, serv);
		return -1;
	}
	*saptr = malloc(res->ai_addrlen);
	if(*saptr == NULL)
	{
		printf("malloc error: %s\n", strerror(errno));
		return -1;
	}
	memcpy(*saptr, res->ai_addr, res->ai_addrlen);
	*lenp = res->ai_addrlen;
	freeaddrinfo(ressave);
	return sockfd;
}

int
  udp_connect(const char *host, const char *serv)
{
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{
		printf("udp_connect error for %s, %s: %s\n",
				host, serv, gai_strerror(n));
		return -1;
	}
	ressave = res;
	for(; res != NULL; res = res->ai_next)
	{
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(sockfd < 0)
			continue;
		if(connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		close(sockfd);
	}
	if(res == NULL)
	{
		printf("udp_connect error for %s, %s\n", host, serv);
		return -1;
	}
	freeaddrinfo(ressave);
	return sockfd;
}

int
  udp_server(const char *host, const char *serv, socklen_t *addrlenp)
{
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{
		printf("udp_server error for %s, %s: %s\n",
				host, serv, gai_strerror(n));
		return -1;
	}
	ressave = res;
	for(; res != NULL; res = res->ai_next)
	{
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(sockfd < 0)
			continue;
		if(bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		close(sockfd);
	}
	if(res == NULL)
	{
		printf("udp_server error for %s, %s\n", host, serv);
	}
	if(addrlenp)
		*addrlenp = res->ai_addrlen;
	freeaddrinfo(ressave);
	return sockfd;
}

void
  pr_cpu_time(void)
{
	double user, sys;
	struct rusage myusage, childusage;
	if(getrusage(RUSAGE_SELF, &myusage) < 0)
	{
		printf("getrusage error: %s\n", strerror(errno));
		return;
	}
	if(getrusage(RUSAGE_CHILDREN, &childusage) < 0)
	{
		printf("getrusage error: %s\n", strerror(errno));
		return;
	}

	user = (double) myusage.ru_utime.tv_sec +
					myusage.ru_utime.tv_usec/1000000.0;
	user += (double) childusage.ru_utime.tv_sec +
					 childusage.ru_utime.tv_usec/1000000.0;
	sys = (double) myusage.ru_stime.tv_sec +
				   myusage.ru_stime.tv_usec/1000000.0;
	sys += (double) childusage.ru_stime.tv_sec +
					childusage.ru_stime.tv_usec/1000000.0;

	printf("\nuser time = %g, sys time = %g\n", user, sys);

}

ssize_t
  readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t n, rc;
	char c, *ptr;
	ptr = vptr;
	for(n = 1; n < maxlen; n++)
	{
		if((rc = read(fd, &c, 1)) == 1)
		{
			*ptr++ = c;
			if(c == '\n')
				break;
		}
		else if(rc == 0)
		{
			if(n == 1)
				return 0;
			else
				break;
		}
		else
			return -1;
	}
	*ptr = 0;
	return n;
}


void
  web_child(int sockfd)
{
	int ntowrite;
	ssize_t nread;
	char line[1024], result[MAXN];
	for(;;)
	{
		if((nread = readline(sockfd, line, sizeof(line))) == 0)
		{
			return;
		}
		ntowrite = atol(line);
		if((ntowrite < 0) || (ntowrite > MAXN))
		{
			printf("client request for %d bytes\n", ntowrite);
			return ;
		}
		if(writen(sockfd, result, ntowrite) < 0)
		{
			printf("writen error: %s\n", strerror(errno));
			return;
		}
	}
	return;
}

ssize_t
  writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0)
	{
		if((nwritten = write(fd, ptr, nleft)) <= 0)
		{
			if(nwritten < 0 && errno == EINTR)
			{
				nwritten = 0;
			}
			else
				return -1;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

pid_t
  child_make(int i, int listenfd, int addrlen)
{
	pid_t pid;
	if((pid = fork()) > 0)
	{
		return pid; /*parent*/
	}
	child_main(i, listenfd, addrlen);
}

void
  child_main(int i, int listenfd, int addrlen)
{
	int connfd;
	void web_child(int);
	socklen_t clilen;
	struct sockaddr *cliaddr;
	extern long *cptr;

	cliaddr = malloc(addrlen);
	if(cliaddr == NULL)
	{
		printf("malloc error: %s\n", strerror(errno));
		return;
	}
	printf("child %ld starting\n", (long)getpid());
	for(;;)
	{
		clilen = addrlen;
		connfd = accept(listenfd, cliaddr, &clilen);
		if(connfd < 0)
		{
			printf("accept error: %s\n", strerror(errno));
			return;
		}
		cptr[i]++;
		web_child(connfd);
		close(connfd);
	}
	return;
}

long *
  meter(int nchildren)
{
	int fd;
	long *ptr;
	ptr = mmap(0, nchildren * sizeof(long), PROT_READ | PROT_WRITE,
			MAP_ANON | MAP_SHARED, -1 , 0);
	return ptr;
}










