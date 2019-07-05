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