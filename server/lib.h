#ifndef _LIB_H_
#define _LIB_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>


struct addrinfo*
  host_serv(const char *host, const char *serv, int family, int socktype);
int
  tcp_connect(const char *host, const char *serv);
char *
  sock_ntop_host(const struct sockaddr *sa, socklen_t salen, char *str, int len);
int 
  tcp_listen(const char *host, const char *serv, socklen_t *addrlenp);
//udp
int
  udp_client(const char *host, const char *serv, struct sockaddr **saptr, socklen_t *lenp);
int
  udp_connect(const char *host, const char *serv);
int
  udp_server(const char *host, const char *serv, socklen_t *addrlenp);
#endif
