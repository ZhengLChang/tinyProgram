//splint_msg.c
int func_splint_msg1(void)
{
	int a;
	return 0;
}

int func_splint_msg2(void)
{
	int *a = (int *)malloc(sizeof(int));
	a = NULL;
	return 0;
}

