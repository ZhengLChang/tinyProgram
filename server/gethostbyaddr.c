#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	struct hostent *hosten_p  = NULL;
	struct in_addr addr;
	char **pp;

	if(argc != 2)
	{
		printf("%s [ipv4]\n", argv[0]);
		return -1;
	}
	if(1 != inet_pton(AF_INET, argv[1], (void *)&addr))
	{
		printf("inet_pton error\n");
		return -1;
	}
	hosten_p = gethostbyaddr((char *)&addr, (socklen_t)sizeof(addr), AF_INET);
	if(hosten_p == NULL)
	{
		printf("gethostbyaddr get NULL\n");
		return -1;
	}
	printf("official name: %s\n", hosten_p->h_name);
	for(pp = hosten_p->h_aliases; *pp != NULL; pp++)
	{
		printf("\t%s\n", *pp);
	}
	for(pp = hosten_p->h_addr_list; *pp != NULL; pp++)
	{
		printf("\t%s\n", *pp);
	}
	return 0;
}
