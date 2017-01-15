/*Theory:
 * In x86_32, When one function named 'function_A' 
 * call another function named 'function_B'
 * the function_B's stack look like below:
 * 	Parameter #N       <---- N*4+4(%ebp)
 * 	...
 * 	Parameter #2       <---- 12(%ebp)
 * 	Parameter #1       <---- 8(%ebp)
 * 	Return Address     <---- 4(%ebp)
 * 	Old %ebp           <---- (%ebp)
 * 	Local Variable 1   <---- -4(%ebp)
 * 	Local Variable 2   <---- -8(%ebp)
 *      ...
 *
 * So, We can't use Paremeter or local variable get the address which store the
 * Return Address and change it.
 */
 
#include <stdio.h>
void modify_return_address(int num);

int main(int argc, const char **argv)
{
	printf("%s %d\n", __func__, __LINE__);
	modify_return_address(1);
	printf("%s will return\n", __func__);
	return 0;
}
void modify_return_address(int num)
{
	int *return_address = (int *)((char *)(&num) - 4); 
	*return_address = (int )(main);
	return;		
}
