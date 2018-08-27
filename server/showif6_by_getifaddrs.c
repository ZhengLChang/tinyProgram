#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if.h>

/* Using getifaddrs() is preferred since it can work with both IPv4 and IPv6 */
static void print_if_by_af(int af)
{
    struct ifaddrs *ifap = NULL, *it;
    char buf[1024];

    fprintf(stderr, "***************************************\n");
    fprintf(stderr, "%s %d\n", __func__, __LINE__);
    fprintf(stderr, "***************************************\n");

    if (getifaddrs(&ifap) != 0) {
        fprintf(stderr, "%s %d, getifaddrs %m\n", __func__, __LINE__);
        return ;
    }

    it = ifap;
    for (; it!=NULL; it = it->ifa_next) {
	struct sockaddr *ad = it->ifa_addr;

	if ((it->ifa_flags & IFF_UP)==0) {
    fprintf(stderr, "%s %d, interface is down\n", __func__, __LINE__);
	    continue; /* Skip when interface is down */
	}
	if (it->ifa_flags & IFF_LOOPBACK) {
    fprintf(stderr, "%s %d, loopback interface\n", __func__, __LINE__);
	    continue; /* Skip loopback interface */
	}
	if (ad==NULL) {
	    continue; /* reported to happen on Linux 2.6.25.9 
			 with ppp interface */
	}
	if (ad->sa_family != af) {
	    continue; /* Skip when interface is down */
	}
  
  switch(ad->sa_family)
  {
    case AF_INET:
  inet_ntop(ad->sa_family, &((struct sockaddr_in *)ad)->sin_addr, buf, sizeof(buf));
  break;
    case AF_INET6:
  inet_ntop(ad->sa_family, &((struct sockaddr_in6 *)ad)->sin6_addr, buf, sizeof(buf));
  break;

  }
  fprintf(stderr, "%s\n", buf);
    }

    freeifaddrs(ifap);
    return;
}

int main()
{
  print_if_by_af(AF_INET6);
  return 0;
}
