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
		sockfd = socket(res->family, res->ai_socktype, res->ai_protopol);
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
	hints.si_socktype = SOCK_DGRAM;
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










