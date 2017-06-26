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

#define MAX_MALLOC_FILE_SIZE (100 * 1024 * 1024)
#define MAX_READ_CHAR (100 * 1024)
int main(int argc, char **argv)
{
	char *fileName = NULL, *outputFileName = NULL;
	int fd = -1;
	char *p = NULL, *output_p = NULL;
	struct stat file_stat;
	int outputSize = -1;
	int c;
	while (1) {
		char opts[] = "i:o:";

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
		printf("usage: %s -i fileName -o outputFileName\n", argv[0]);
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
	if(file_stat.st_size < MAX_MALLOC_FILE_SIZE)
	{
		if((output_p = malloc(1.5 * file_stat.st_size)) == NULL)
		{
			printf("malloc error: %s\n", strerror(errno));
			munmap(p, file_stat.st_size);
			return -1;
		}
		output_p[0] = '\0';
		outputSize = base64_encode (p, file_stat.st_size, output_p);
		munmap(p, file_stat.st_size);
		printf("after convert, file size: %d\n", outputSize);
		if((fd = open(outputFileName, O_RDWR | O_CREAT | O_TRUNC, 0777)) < 0)
		{
			printf("open %s error\n", strerror(errno));
			return -1;
		}
		write(fd, output_p, outputSize);
		close(fd);
	}
	else
	{
		int read_n = 0, file_size = file_stat.st_size;
		char *save_p = p;
		if((fd = open(outputFileName, O_RDWR | O_CREAT | O_TRUNC, 0777)) < 0)
		{
			munmap(p, file_stat.st_size);
			printf("open %s error\n", strerror(errno));
			return -1;
		}
		if((output_p = malloc(1.5 * MAX_READ_CHAR)) == NULL)
		{
			printf("malloc error: %s\n", strerror(errno));
			close(fd);
			munmap(p, file_stat.st_size);
			return -1;
		}
		while(file_size > 0)
		{
			if(file_size > MAX_READ_CHAR)
			{
				read_n = MAX_READ_CHAR;
			}
			else
			{
				read_n = file_size;
			}
			output_p[0] = '\0';
			outputSize = base64_encode (save_p, read_n, output_p);
			save_p += read_n;	
			file_size -= read_n;
			write(fd, output_p, outputSize);
		}
		close(fd);
		munmap(p, file_stat.st_size);
	}
	return 0;
}







