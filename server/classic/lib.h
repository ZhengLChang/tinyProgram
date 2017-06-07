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
#include <sys/time.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define MAXN 16384

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
void
  pr_cpu_time(void);
void
  web_child(int sockfd);
ssize_t
  readline(int fd, void *vptr, size_t maxlen);
ssize_t
  writen(int fd, const void *vptr, size_t n);
#if 0
pid_t
  child_make(int i, int listenfd, int addrlen);
void
  child_main(int i, int listenfd, int addrlen);
#endif
long *
  meter(int nchildren);
void
  my_lock_init(char *pathname);
void
  my_lock_wait();
void
  my_lock_release();
#endif
