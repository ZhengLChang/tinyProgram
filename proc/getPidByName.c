#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>

static void getProcessName(const int pid, char *pid_name, int pid_size);
/*return:
 *      -1 not found
 *      not-negative is pid number
 * */
int getPidByName(const char *name)
{
	DIR *dir_p = NULL;
	struct dirent entry, *res = NULL;
	int pid = -1, error_ret = 0;
	if(name == NULL)
	{
		return -1;
	}

	dir_p = opendir("/proc");
	if(NULL == dir_p)
	{
		printf("opendir error: %s\n", strerror(errno));
		goto ERR;
	}
	for(;;)
	{
		error_ret = readdir_r(dir_p, &entry, &res);
		if(error_ret != 0)
		{
			printf("readdir_r error: %s\n", strerror(error_ret));
			goto ERR;
		}
		if(res == NULL)
		{
			break;
		}
		if(isdigit(res->d_name[0]))
		{
			char pid_name[56] = "";
			
			pid = atoi(res->d_name);
			
			getProcessName(pid, pid_name, sizeof(pid_name));
			if(pid_name[0] != '\0' && strcmp(pid_name, name) == 0)
			{
				break;
			}
		}
		pid = -1;
	}


	if(dir_p != NULL)
	{
		closedir(dir_p);
		dir_p = NULL;
	}
	return pid;
ERR:
	if(dir_p != NULL)
	{
		closedir(dir_p);
		dir_p = NULL;
	}
	return -1;
}


static void getProcessName(const int pid, char *pid_name, int pid_size)
{
	int fd = -1;
	char name[56] = "", *p = NULL, str[56] = "", buf[1024] = "";
	int size = 0;
	struct stat file_stat;

	pid_name[0] = '\0';
	size = snprintf(name, sizeof(name) - 1, "/proc/%d/status", pid);
	if(size <= 0 || size > sizeof(name) - 1)
	{
		printf("%s %d, snprintf error\n", __func__, __LINE__);
		goto ERR;
	}

	fd = open(name, O_RDONLY);	
	if(fd < 0)
	{
		printf("%s %d, open error: %s\n", __func__, __LINE__, strerror(errno));
		goto ERR;
	}
	if(fstat(fd, &file_stat) < 0)
	{
		printf("fstat error: %s\n", strerror(errno));
		goto ERR;
	}
	size = read(fd, buf, sizeof(buf) - 1);
	if(size <= 0)
	{
		printf("read error: %s\n", strerror(errno));
		goto ERR;
	}
	buf[size] = '\0';
	/*
	p = mmap(0, file_stat.st_size, PROT_READ, MAP_ANONYMOUS, fd, 0);
	if(p == MAP_FAILED)	
	{
		printf("%s %d, mmap error: %s\n", __func__, __LINE__, strerror(errno));
		goto ERR;
	}
	if(fd != -1)
	{
		close(fd);
		fd = -1;
	}
*/	
	if(sscanf(buf, "%s %s", str, pid_name) != 2)	
	{
		goto ERR;
	}

	if(fd != -1)
	{
		close(fd);
		fd = -1;
	}
/*
	if(p != NULL && p != MAP_FAILED)
	{
		munmap(p, file_stat.st_size);
		p = NULL;
	}
	*/
	return ;
ERR:
	if(fd !=  -1)
	{
		close(fd);
		fd = -1;
	}
	/*
	if(p != NULL && p != MAP_FAILED)
	{
		munmap(p, file_stat.st_size);
		p = NULL;
	}
	*/
	return ;
}

int main(void)
{
	printf("%d\n", getpid());
	printf("%d\n", getPidByName("eclipse1"));
	return 0;
}











