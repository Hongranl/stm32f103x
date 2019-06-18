#include <stdio.h>

int swap(int *a, int *b)
{
	return (*b ^= *a ^= *b ^= *a);
}

int main(void)
{
	int a = 12;
	int b = 23;
	swap(&a, &b);
	printf("this is a test--[%d]--[%d]\n", a, b);
}