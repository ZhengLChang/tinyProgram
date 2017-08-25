#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct test
{
	int a;
	int b;
};
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
		*array = malloc(5 * elem_size);
		assert(*array);
	}
	else if(*p_used == *p_size)
	{
		*p_size += 5;
		*array = realloc(*array, elem_size * (*p_size));
		assert(*array);
	}
	memcpy(*array + elem_size * (*p_used) ++, value, elem_size);
	return;
}

int main(void)
{
	struct test *arr_p = NULL, arr;
	unsigned i = 1, used = 0, size = 0;
	arr.a = 1, arr.b = 100;	
	
	for(i = 0; i< 100; i++)
	{
		array_append((void **)(&arr_p), sizeof(*arr_p), &used, &size, &arr); arr.a++, arr.b += 100;
	}
	for(i = 0;i < used; i++)
	{
		printf("%d, %d\n", arr_p[i].a, arr_p[i].b);
	}
#if 0
	int *arr = NULL;
	unsigned i = 1, used = 0, size = 0;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	array_append((void **)(&arr), sizeof(int), &used, &size, &i);
	i++;
	for(i = 0; i < used; i++)
	{
		printf("%d\n", arr[i]);
	}
#endif
	printf("used = %d, size = %d\n", used, size);
	return 0;
}
