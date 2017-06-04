#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	struct servent *sptr;
	char **fp;
	sptr = getservbyname("http", NULL);
	if(sptr == NULL)
		exit(0);
	printf("official name: %s\n", sptr->s_name);
	for(fp = sptr->s_aliases; *fp != NULL; fp++)	
	{
		printf("\t%s\n", *fp);
	}
	printf("\n");
	printf("service port: %d\n", ntohs(sptr->s_port));
	printf("service proto: %s\n", sptr->s_proto);
	return 0;
}
