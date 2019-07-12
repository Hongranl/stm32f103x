#include <stdio.h>
void big_or_little(void);
int swap(int *real_a, int *real_b)
{
	int a = *real_a, b = *real_b;
	(b ^= a ^= b ^= a);
	*real_a = a;
	*real_b = b;
	return (b ^= a ^= b ^= a);
}

int main(void)
{
	int a = 12;
	int b = 13;
	swap(&a, &b);
	printf("this is a test--[%d]--[%d]\n", a, b);
	big_or_little();
}

void big_or_little(void){
		char c;
	int a=1;
	int *p=&a;
	c = *p;
	if(c==a)
	{
		printf("小端\n");
	}
	else
	{
		printf("大端\n");
	}
 

}