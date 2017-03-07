#include <stdio.h>


#define BUFFER_PIECE_SIZE 32

int get_entry_size(int size)
{
	if(size <= 0)
		return 0;
	return ((size + BUFFER_PIECE_SIZE - 1) & ~(BUFFER_PIECE_SIZE - 1));
}


int main(int argc, char **argv)
{
	printf("%d\n", get_entry_size(1));
	return 0;
}
