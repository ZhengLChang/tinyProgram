#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if.h>

/*
/**
 * If head file  is not existed in your system, you could get information of IPv6 address
 * in file /proc/net/if_inet6.
 *
 * Contents of file "/proc/net/if_inet6" like as below:
 * 00000000000000000000000000000001 01 80 10 80       lo
 * fe8000000000000032469afffe08aa0f 08 40 20 80     ath0
 * fe8000000000000032469afffe08aa0f 07 40 20 80    wifi0
 * fe8000000000000032469afffe08aa0f 05 40 20 80     eth1
 * fe8000000000000032469afffe08aa0f 03 40 20 80      br0
 * fe8000000000000032469afffe08aa10 04 40 20 80     eth0
 *
 * +------------------------------+ ++ ++ ++ ++    +---+
 * |                                |  |  |  |     |
 * 1                                2  3  4  5     6
 *
 * There are 6 row items parameters:
 * 1 => IPv6 address without ':'
 * 2 => Interface index
 * 3 => Length of prefix
 * 4 => Scope value (see kernel source "include/net/ipv6.h" and "net/ipv6/addrconf.c")
 * 5 => Interface flags (see kernel source "include/linux/rtnetlink.h" and "net/ipv6/addrconf.c" "linux/include/if_addr.h")
 * 6 => Device name
 *
 * Note that all values of row 1~5 are hexadecimal string
 */
#define PATH_PROCENT_IFINET6 "/proc/net/if_inet6"

static void print_if_by_af(int af)
{
  FILE *fp = NULL;
  char addr6p[8][5];
  char devname[32], addr6_str[64];
  int if_idx, plen, dad_status, scope;
  struct sockaddr_in6 ipv6_network_addr;

  if((fp = fopen(PATH_PROCENT_IFINET6, "r")) == NULL)
    return;
  while(fscanf(fp, "%4s%4s%4s%4s%4s%4s%4s%4s %08x %02x %02x %02x %20s\n", 
        addr6p[0], addr6p[1], addr6p[2], addr6p[3], addr6p[4],
        addr6p[5], addr6p[6], addr6p[7], &if_idx, &plen, &scope,
        &dad_status, devname) != EOF)
  {
    snprintf(addr6_str, sizeof(addr6_str), "%s:%s:%s:%s:%s:%s:%s:%s",
        addr6p[0], addr6p[1], addr6p[2], addr6p[3], addr6p[4],
          addr6p[5], addr6p[6], addr6p[7]);
    inet_pton(AF_INET6, addr6_str, (void *)&ipv6_network_addr.sin6_addr);
    inet_ntop(AF_INET6, (void *)&ipv6_network_addr.sin6_addr, addr6_str, sizeof(addr6_str));
    if(IN6_IS_ADDR_LINKLOCAL((void *)&ipv6_network_addr.sin6_addr))
    {
        fprintf(stderr, "LINK: %s\n", addr6_str);
    }
    else if(IN6_IS_ADDR_LOOPBACK((void *)&ipv6_network_addr.sin6_addr))
    {
        fprintf(stderr, "Loopback: %s\n", addr6_str);
    }
    else
    {
        fprintf(stderr, "Global: %s\n", addr6_str);
    }
        //fprintf(stderr, "%s\n", addr6_str);
    /*
    switch(scope & IPV6_ADDR_SCOPE_MASK)
    {
      case 0:
        fprintf(stderr, "Global: %s\n", addr6_str);
        break;
      case IPV6_ADDR_LINKLOCAL:
        fprintf(stderr, "LINK: %s\n", addr6_str);
        break;
      case IPV6_ADDR_SITELOCAL:
        fprintf(stderr, "Site: %s\n", addr6_str);
        break;
      case IPV6_ADDR_COMPATv4:
        fprintf(stderr, "Compat: %s\n", addr6_str);
        break;
      case IPV6_ADDR_LOOPBACK:
        fprintf(stderr, "Host: %s\n", addr6_str);
        break;
      default:
        fprintf(stderr, "Unknown: %s\n", addr6_str);
        break;
    }
    */
  }
  if(fp != NULL)
  {
    fclose(fp);
    fp = NULL;
  }
}


int main()
{
  print_if_by_af(AF_INET6);
  return 0;
}








