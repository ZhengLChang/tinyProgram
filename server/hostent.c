#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	char *ptr, **pptr;
	char str[INET_ADDRSTRLEN];
	struct hostent *hptr;
	struct in_addr addr;

	printf("size of in_addr: %d\n", sizeof(addr));
	while(--argc > 0)
	{
		ptr = *++argv;
		if((hptr = gethostbyname(ptr)) == NULL)
		{
			printf("gethostname error for host: %s: %s", 
					ptr, hstrerror(h_errno));	
			continue;
		}
		printf("official hostname: %s\n", hptr->h_name);
		for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
		{
			printf("\talias: %s\n", *pptr);
		}
		switch(hptr->h_addrtype)
		{
			case AF_INET:
				pptr = hptr->h_addr_list;
				printf("size of *pptr: %d\n", strlen(*pptr));
				printf("*pptr: %s\n", *pptr);
				memcpy(&addr, *pptr, sizeof(pptr));
				printf("(char *)&addr: %s\n", (char *)&addr);
				for(; *pptr != NULL; pptr++)
					printf("\taddress: %s\n",
						inet_ntop(hptr->h_addrtype,
							(void *)&addr, str, sizeof(str)));
				break;
			default:
				printf("unknown address type\n");
				break;
		}
	}
	exit(0);
}








