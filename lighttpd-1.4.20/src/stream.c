#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>

#include "stream.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sys-mmap.h"

#ifndef O_BINARY
# define O_BINARY 0
#endif

int stream_open(stream *f, buffer *fn) {
	struct stat st;
	int fd;

	f->start = NULL;

	if (-1 == stat(fn->ptr, &st)) {
		return -1;
	}

	f->size = st.st_size;

	if (-1 == (fd = open(fn->ptr, O_RDONLY | O_BINARY))) {
		return -1;
	}

	f->start = mmap(0, f->size, PROT_READ, MAP_SHARED, fd, 0);

	close(fd);

	if (MAP_FAILED == f->start) {
		return -1;
	}


	return 0;
}

int stream_close(stream *f) {
	if (f->start) munmap(f->start, f->size);
	f->start = NULL;

	return 0;
}
