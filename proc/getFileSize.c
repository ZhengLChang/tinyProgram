#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
char *aprintf (const char *fmt, ...) 
{
      	/* Use vasprintf. */
      	int ret; 
	va_list args;
	char *str;
	va_start (args, fmt);
	ret = vasprintf (&str, fmt, args);
	va_end (args);
	if (ret < 0 && errno == ENOMEM)
	{
		abort();
	}
	else if (ret < 0) 
		return NULL;
	return str; 
}

int get_file_size(char *path, bool is_allocated)
{
	struct stat buf;
	int file_size = 0;
	if(path == NULL)
	{
		fprintf(stderr, "%s %d, path == NULL\n", __func__, __LINE__);
		return -1;
	}
	if(stat(path, &buf) != -1)
	{
		switch(buf.st_mode & S_IFMT)
		{
			case S_IFREG:	
			{
				file_size = buf.st_size;
				break;
			}
			case S_IFDIR:
			{
				DIR *dir_p = NULL;
				int error_ret = 0;
				struct dirent entry, *res = NULL;
				dir_p = opendir(path);
				if(NULL == dir_p)
				{
					abort();
				}
				for(;;)
				{
					error_ret = readdir_r(dir_p, &entry, &res);
					if(error_ret != 0)
					{
						abort();
					}
					if(res == NULL)
					{
						break;
					}
					if(res->d_name[0] == '.')
					{
						continue;
					}
					file_size += get_file_size(aprintf("%s/%s", path, res->d_name), true);
				}
				if(dir_p != NULL)
				{
					closedir(dir_p);
					dir_p = NULL;
				}
				break;
			}
		}
	}
	if(is_allocated)
	{
		free(path);
	}
	return file_size;
ERROR:
	return -1;
}


int main(int argc, char **argv)
{
	if(argc == 2)
	{
		fprintf(stderr, "%d\n", get_file_size(argv[1], false));
	}
	else
	{
		fprintf(stderr, "%d\n", get_file_size(".", false));
	}
	fprintf(stderr, "%d scan over\n", getpid());
	return 0;
}
