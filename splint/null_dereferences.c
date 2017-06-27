int func_null_dereferences(void)
{
	int *a = NULL;
	return *a;
}
