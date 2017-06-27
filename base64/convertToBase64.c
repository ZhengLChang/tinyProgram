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
	return (base64_encode_to_file(fileName, outputFileName));
}







