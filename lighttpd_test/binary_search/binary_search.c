#include <stdio.h>

int binary_search(int array[], int n, int value)
{
	int left = 0;
	int right = n - 1;

	for(; left <= right; )
	{
		int middle = left + ((right - left) >> 1);
		if(array[middle] > value)
		{
			right = middle - 1;
		}
		else if(array[middle] < value)
		{
			left = middle + 1
		}
		else 
			return middle;
	}
	return -1;
}
