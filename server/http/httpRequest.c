/*
 *usage: ./a.out www.baidu.com http
	./a.out http://www.baidu.com/index.html http 
 * */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int tcp_connect(const char *host, const char *servce);
int requestResource(int sockfd, const char *method, const char *URL);

char *getHostAddr(struct sockaddr_storage *sk, char *addr, int size);
int main(int argc, char **argv)
{
	int sockfd = -1, n = -1; 
	char host[2048] = "", *p = NULL;
	char requestStr[1024] = "";

	if(argc != 3)
	{
		printf("usage: %s host service\n", argv[0]);
		return -1;
	}
	memcpy(host, argv[1], strlen(argv[1]) + 1);

	p = strchr(host + sizeof("https://"), '/');
	if(p != NULL)
	{
		*(p++) = '\0';
	}
	
	if((sockfd = tcp_connect(host, argv[2])) < 0)
	{
		return -1;
	}
	
	if(requestResource(sockfd, "GET", (p == NULL) ? "/" : p) < 0)
	{
		return -1;
	}
	while((n = read(sockfd, requestStr, sizeof(requestStr))) != 0)
	{
		write(STDOUT_FILENO, requestStr, n);
		//printf("%s", requestStr);
	}
	//printf("\n");
	close(sockfd);
	return 0;
}

char *getHostAddr(struct sockaddr_storage *sa, char *addr, int size)
{
	switch(sa->ss_family)
	{
		case AF_INET:
			{
			struct sockaddr_in *si_4 = (struct sockaddr_in *)sa;
			if(NULL == inet_ntop(AF_INET, (void *)&si_4->sin_addr, addr, size))
			{
				return NULL;
			}
			return addr;
			break;
			}
		case AF_INET6:
			{
			struct sockaddr_in6 *si_6 = (struct sockaddr_in6 *)sa;
			if(NULL == inet_ntop(AF_INET, (void *)&si_6->sin6_addr, addr, size))
			{
				return NULL;
			}
			return addr;
			break;
			}
		default:
			printf("unknown family\n");
			return NULL;
			break;
	}
}

int tcp_connect(const char *host, const char *serv)
{
	int sockfd, n;
	char addr[1024] = "";
	struct addrinfo hints, *res, *ressave;

	memset((void *)&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{
		printf("getaddrinfo error for %s, %s: %s\n", 
				host, serv, gai_strerror(n));
		return -1;
	}
	ressave = res;
	for(; res != NULL; res = res->ai_next)
	{
		sockfd = socket(res->ai_family,
				res->ai_socktype, res->ai_protocol);
		if(sockfd < 0)
		{
			continue;
		}
		if(connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
		{
			char addr[1024] = "";
			printf("connect success: %s\n", 
					getHostAddr((struct sockaddr_storage *)res->ai_addr, addr, sizeof(addr))		
					/*inet_ntop(res->ai_family, res->ai_addr, addr, sizeof(addr))*/);
			break;
		}
		close(sockfd);
	}
	if(res == NULL)
	{
		printf("tcp_connect error for %s, %s: %s\n", host, serv, strerror(errno));
		return -1;
	}
	freeaddrinfo(ressave);
	return sockfd;
}

int requestResource(int sockfd, const char *method, const char *URL)
{
	char request[1024] = "";
	int n = -1;
	n = snprintf(request, sizeof(request), "%s %s %s\r\n%s\r\n\r\n",
			method, URL, "HTTP/1.1", "Host: wwww.baidu.com");
#if 0	
			"Host: 119.75.218.70\r\nCookie: BD_HOME=0; BD_UPN=143254\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12) AppleWebKit/602.1.50 (KHTML, like Gecko) Version/10.0 Safari/602.1.50\r\nAccept-Language: zh-cn\r\nCache-Control: max-age=0\r\nAccept-Encoding: gzip, deflate"*/);
#endif
	if(n == 1024)
	{
		printf("out of memory\n");
		return -1;
	}
	if(n != write(sockfd, request, n))
	{
		printf("write error\n");
		return -1;
	}
	return sockfd;
}













