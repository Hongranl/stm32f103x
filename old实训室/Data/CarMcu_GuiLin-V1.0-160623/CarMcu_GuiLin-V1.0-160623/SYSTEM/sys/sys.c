#include "sys.h"


void Delay_us(u16 nus)
{		
	u8 i = 0;
	while(nus--)
	{
		i = 8;
		while(i--);
	} 
}

void Delay_ms(u16 nms)
{	 		  	  
	u16 i = 0;
	while(nms--)
	{
		i = 8000;
		while(i--);
	}  	    
} 

u16 Crc16Code(u8 *str,u8 len)
{
	u16 code = 0xFFFF;
	u16 Poly = 0xA001;
	u8 flag = 0;
	u8 j = 0;
	u8 i = 0;
	
	for(j=0;j<len;j++)
	{
		code ^= (unsigned int)(*str);
		for(i=0;i<8;i++)
		{
			if(code & 0x0001)
			{
			  flag = 1;
			}
			code = code >> 1;
			if(flag)
			{
				code ^= Poly;
				flag = 0;
			}
		}
		str++;
	}
	
	return code;
}

void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
