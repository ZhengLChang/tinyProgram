#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>

int main(void)
{
	struct servent *sptr;
	sptr = getservbyname("ftp", "tcp");
	printf("%d\n", ntohs(sptr->s_port));
	return 0;
}
