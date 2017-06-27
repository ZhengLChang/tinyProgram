void memory_management1(void)
{
	int *a = (int *)malloc(sizeof(int));
	int *b = a;
	free(a);
	*b = 0;
	return ;
}
