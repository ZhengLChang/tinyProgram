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
    *return_address = 0x80483cd;//(int )(main);
    return;     
}
