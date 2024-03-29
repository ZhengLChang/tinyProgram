#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#define OFFSET_LEN (1024 *1024 * 1024)
#define CONST_STR_LEN(x) x, x ? sizeof(x) - 1: 0
int main(void)
{
	int fd = -1;

	if((fd = open("./hole.txt", O_CREAT | O_TRUNC | O_CLOEXEC | O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO)) < 0)
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	if(write(fd, "BEGIN", sizeof("BEGIN") - 1) != sizeof("BEGIN") - 1)
	//if(write(fd, CONST_STR_LEN("BEGIN")) != sizeof("BEGIN\n") - 1)
	{
		printf("write error: %s\n", strerror(errno));
		close(fd);
		return -1;
	}
#if 1
	if(lseek(fd, OFFSET_LEN, SEEK_CUR) == (off_t)-1)
	{
		printf("lseek error: %s\n", strerror(errno));
		close(fd);
		return -1;
	}
#endif
	write(fd, CONST_STR_LEN("EOF"));
	close(fd);
	return 0;
}
