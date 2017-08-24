#include <stdio.h>


static void array_append( void **array,
		unsigned elem_size,
		unsigned *p_used,
		unsigned *p_size,
		const void *value)
{
	int i = 0;
	assert(array != NULL && p_used != NULL && p_size != NULL);
	if(*p_size == 0)
	{
		*p_size += 5;
		*array = malloc(5, elem_size);
		assert(*array);
	}
	else if(*p_used == *p_size)
	{
		*p_size += 5;
		*array = realloc(*array, elem_size * (*p_size));
		assert(*array);
	}
	memcpy(*array + (*p_used) ++, value, elem_size);
	return;
}
