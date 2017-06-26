#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "base64.h"

int main(int argc, char **argv)
{
	char *fileName = NULL, *outputFileName = NULL;
	int fd = -1;
	char *p = NULL, *output_p = NULL;
	struct stat file_stat;
	int outputSize = -1;
	int c;
	while (1) {
		char opts[] = "i:o";

		c = getopt(argc, argv, opts);
		if (c == EOF)
			break;

		switch (c) {
		case 'i':
			fileName = optarg;
			break;

		case 'o':
			outputFileName = optarg;
			break;
		default:
			break;
		}
	}
	if(fileName == NULL || outputFileName == NULL)
	{
		printf("usage: %s -f fileName -o outputFileName\n", argv[0]);
		return -1;
	}
	if((fd = open(fileName, O_RDONLY)) < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	if(fstat(fd, &file_stat) < 0)	
	{
		printf("fstat error: %s\n", strerror(errno));
		return -1;
	}
	if(file_stat.st_size <= 0)
	{
		printf("file is empty\n");
		return -1;
	}
	printf("file size: %d\n", file_stat.st_size);
	if(MAP_FAILED == (p = mmap(0, file_stat.st_size, PROT_READ, MAP_SHARED, fd, 0)))
	{
		printf("mmap error: %s\n", strerror(errno));
		return -1;
	}
	close(fd);
	if((output_p = malloc(file_stat.st_size)) == NULL)
	{
		printf("malloc error: %s\n", strerror(errno));
		munmap(p, file_stat.st_size);
		return -1;
	}
	output_p[0] = '\0';
	outputSize = base64_decode (p, output_p);
	munmap(p, file_stat.st_size);
	printf("after convert, file size: %d\n", outputSize);
	if((fd = open(outputFileName, O_RDWR | O_CREAT | O_TRUNC, 0777)) < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	write(fd, output_p, outputSize);
	close(fd);
	return 0;
}







