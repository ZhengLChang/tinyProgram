#include <stdio.h>

#define SIZE_SZ sizeof(size_t)
#define MINSIZE  16
#define MALLOC_ALIGNMENT       (2 * SIZE_SZ)
#define MALLOC_ALIGN_MASK      (MALLOC_ALIGNMENT - 1)

#define request2size(req)                                         \
	  (((req) + SIZE_SZ + MALLOC_ALIGN_MASK < MINSIZE)  ?             \
	      MINSIZE :                                                      \
	      ((req) + SIZE_SZ + MALLOC_ALIGN_MASK) & ~MALLOC_ALIGN_MASK)
#define fastbin_index(sz) \
	  ((((unsigned int) (sz)) >> (SIZE_SZ == 8 ? 4 : 3)) - 2)
#define MAX_FAST_SIZE     (80 * SIZE_SZ / 4)
#define NFASTBINS  (fastbin_index (request2size (MAX_FAST_SIZE)) + 1)

int main()
{
	size_t size = NFASTBINS;
	printf("MAX_FAST_SIZE = %d\n", MAX_FAST_SIZE);
	printf("request2size(MAX_FAST_SIZE) = %d\n", request2size(MAX_FAST_SIZE));
	printf("%d\n", size);
	return 0;
}
