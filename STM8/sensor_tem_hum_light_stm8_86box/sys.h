/************************************************************************
  * 文件 ：stm8_sys.h
  * 作者 ：QinYUN575
  * 版本 ：V1.0
  * 时间 ：2017年11月5日 17:11:10
  * 描述 ：STM8实现位带操作
  * 注意&示例:
  * 将PA.0置高: PAout(0) = HIGHT_LEVEL 或者 PAout(0) = 1;
  * 读取PA.0引脚电平: bit = PAin(0);
  ***********************************************************************/
#ifndef __SYS_H_
#define __SYS_H_
#include "stm8s.h"

#define HIGHT_LEVEL	1;	/*定义高电平*/
#define LOW_LEVEL	0;	/*定义低电平*/

#define __READ_WRITE		/*可读写*/
#define __READ     const	/*只读*/

/*---------------------------------------------
 * 将NAME定义成有按位访问I/O口寄存器权限的联合体类型
 * 访问8位的寄存器: NAME
 * 访问位: NAME_bit.noXX
 *--------------------------------------------*/
#define __IO_REG8_BIT(NAME, ADDRESS, ATTRIBUTE, BIT_STRUCT)			\
						__near __no_init volatile ATTRIBUTE union	\
							{										\
								unsigned char NAME;					\
                                BIT_STRUCT NAME ## _bit;            \
							} @ADDRESS;

/*-------------------------------------------------------------------------
 *      端口寄存器定义
 *-----------------------------------------------------------------------*/
/* 端口数据输出寄存器 */
typedef struct
{
  unsigned char ODR0        : 1;
  unsigned char ODR1        : 1;
  unsigned char ODR2        : 1;
  unsigned char ODR3        : 1;
  unsigned char ODR4        : 1;
  unsigned char ODR5        : 1;
  unsigned char ODR6        : 1;
  unsigned char ODR7        : 1;
} __BITS_ODR;

/* 输入引脚寄存器 */
typedef struct
{
  unsigned char IDR0        : 1;
  unsigned char IDR1        : 1;
  unsigned char IDR2        : 1;
  unsigned char IDR3        : 1;
  unsigned char IDR4        : 1;
  unsigned char IDR5        : 1;
  unsigned char IDR6        : 1;
  unsigned char IDR7        : 1;
} __BITS_IDR;

/* 端口模式寄存器 */
typedef struct
{
  unsigned char DDR0        : 1;
  unsigned char DDR1        : 1;
  unsigned char DDR2        : 1;
  unsigned char DDR3        : 1;
  unsigned char DDR4        : 1;
  unsigned char DDR5        : 1;
  unsigned char DDR6        : 1;
  unsigned char DDR7        : 1;
} __BITS_DDR;

/* 端口控制寄存器1 */
typedef struct
{
  unsigned char C10         : 1;
  unsigned char C11         : 1;
  unsigned char C12         : 1;
  unsigned char C13         : 1;
  unsigned char C14         : 1;
  unsigned char C15         : 1;
  unsigned char C16         : 1;
  unsigned char C17         : 1;
} __BITS_CR1;

/* 端口控制寄存器2 */
typedef struct
{
  unsigned char C20         : 1;
  unsigned char C21         : 1;
  unsigned char C22         : 1;
  unsigned char C23         : 1;
  unsigned char C24         : 1;
  unsigned char C25         : 1;
  unsigned char C26         : 1;
  unsigned char C27         : 1;
} __BITS_CR2;


/*-------------------------------------------------------------------------
 *      端口地址结构体[联合体]定义
 *-----------------------------------------------------------------------*/
/* 端口输出地址定义 */
__IO_REG8_BIT(PA_ODR,      0x5000, __READ_WRITE, __BITS_ODR);	//PA输出口 
__IO_REG8_BIT(PB_ODR,      0x5005, __READ_WRITE, __BITS_ODR);	//PB输出口 
__IO_REG8_BIT(PC_ODR,      0x500A, __READ_WRITE, __BITS_ODR);	//PC输出口 
__IO_REG8_BIT(PD_ODR,      0x500F, __READ_WRITE, __BITS_ODR);	//PD输出口 
__IO_REG8_BIT(PE_ODR,      0x5014, __READ_WRITE, __BITS_ODR);	//PE输出口 
__IO_REG8_BIT(PF_ODR,      0x5019, __READ_WRITE, __BITS_ODR);	//PF输出口 
__IO_REG8_BIT(PG_ODR,      0x501E, __READ_WRITE, __BITS_ODR);	//PG输出口 
__IO_REG8_BIT(PH_ODR,      0x5023, __READ_WRITE, __BITS_ODR);	//PH输出口 
__IO_REG8_BIT(PI_ODR,      0x5028, __READ_WRITE, __BITS_ODR);	//PI输出口 

/* 端口输入地址定义 */
__IO_REG8_BIT(PA_IDR,      0x5001, __READ, __BITS_IDR);	//PA输入口 
__IO_REG8_BIT(PB_IDR,      0x5006, __READ, __BITS_IDR);	//PB输入口 
__IO_REG8_BIT(PC_IDR,      0x500B, __READ, __BITS_IDR);	//PC输入口 
__IO_REG8_BIT(PD_IDR,      0x5010, __READ, __BITS_IDR);	//PD输入口 
__IO_REG8_BIT(PE_IDR,      0x5015, __READ, __BITS_IDR);	//PE输入口 
__IO_REG8_BIT(PF_IDR,      0x501A, __READ, __BITS_IDR);	//PF输入口 
__IO_REG8_BIT(PG_IDR,      0x501F, __READ, __BITS_IDR);	//PG输入口 
__IO_REG8_BIT(PH_IDR,      0x5024, __READ, __BITS_IDR);	//PH输入口 
__IO_REG8_BIT(PI_IDR,      0x5029, __READ, __BITS_IDR);	//PI输入口 
/* 端口模式配置寄存器地址定义 */
__IO_REG8_BIT(PA_DDR,      0x5002, __READ_WRITE, __BITS_DDR);
__IO_REG8_BIT(PB_DDR,      0x5007, __READ_WRITE, __BITS_DDR);
__IO_REG8_BIT(PC_DDR,      0x500C, __READ_WRITE, __BITS_DDR);
__IO_REG8_BIT(PD_DDR,      0x5011, __READ_WRITE, __BITS_DDR);
__IO_REG8_BIT(PE_DDR,      0x5016, __READ_WRITE, __BITS_DDR);
__IO_REG8_BIT(PF_DDR,      0x501B, __READ_WRITE, __BITS_DDR);
__IO_REG8_BIT(PG_DDR,      0x5020, __READ_WRITE, __BITS_DDR);
__IO_REG8_BIT(PH_DDR,      0x5025, __READ_WRITE, __BITS_DDR);
__IO_REG8_BIT(PI_DDR,      0x502A, __READ_WRITE, __BITS_DDR);

/* 端口控制寄存器1地址定义 */
__IO_REG8_BIT(PA_CR1,      0x5003, __READ_WRITE, __BITS_CR1);
__IO_REG8_BIT(PB_CR1,      0x5008, __READ_WRITE, __BITS_CR1);
__IO_REG8_BIT(PC_CR1,      0x500D, __READ_WRITE, __BITS_CR1);
__IO_REG8_BIT(PD_CR1,      0x5012, __READ_WRITE, __BITS_CR1);
__IO_REG8_BIT(PE_CR1,      0x5017, __READ_WRITE, __BITS_CR1);
__IO_REG8_BIT(PF_CR1,      0x501C, __READ_WRITE, __BITS_CR1);
__IO_REG8_BIT(PG_CR1,      0x5021, __READ_WRITE, __BITS_CR1);
__IO_REG8_BIT(PH_CR1,      0x5026, __READ_WRITE, __BITS_CR1);
__IO_REG8_BIT(PI_CR1,      0x502B, __READ_WRITE, __BITS_CR1);

/* 端口控制寄存器2地址定义 */
__IO_REG8_BIT(PA_CR2,      0x5004, __READ_WRITE, __BITS_CR2);
__IO_REG8_BIT(PB_CR2,      0x5009, __READ_WRITE, __BITS_CR2);
__IO_REG8_BIT(PC_CR2,      0x500E, __READ_WRITE, __BITS_CR2);
__IO_REG8_BIT(PD_CR2,      0x5013, __READ_WRITE, __BITS_CR2);
__IO_REG8_BIT(PE_CR2,      0x5018, __READ_WRITE, __BITS_CR2);
__IO_REG8_BIT(PF_CR2,      0x501D, __READ_WRITE, __BITS_CR2);
__IO_REG8_BIT(PG_CR2,      0x5022, __READ_WRITE, __BITS_CR2);
__IO_REG8_BIT(PH_CR2,      0x5027, __READ_WRITE, __BITS_CR2);
__IO_REG8_BIT(PI_CR2,      0x502C, __READ_WRITE, __BITS_CR2);

/*-------------------------------------------------------------------------
 * 实现位带操作
 *-----------------------------------------------------------------------*/
#define PAout(n) PA_ODR_bit.ODR##n
#define PBout(n) PB_ODR_bit.ODR##n
#define PCout(n) PC_ODR_bit.ODR##n
#define PDout(n) PD_ODR_bit.ODR##n
#define PEout(n) PE_ODR_bit.ODR##n
#define PFout(n) PF_ODR_bit.ODR##n
#define PGout(n) PG_ODR_bit.ODR##n
#define PHout(n) PH_ODR_bit.ODR##n
#define PIout(n) PI_ODR_bit.ODR##n

#define PAin(n)  PA_IDR_bit.IDR##n
#define PBin(n)  PB_IDR_bit.IDR##n
#define PCin(n)  PC_IDR_bit.IDR##n
#define PDin(n)  PD_IDR_bit.IDR##n
#define PEin(n)  PE_IDR_bit.IDR##n
#define PFin(n)  PF_IDR_bit.IDR##n
#define PGin(n)  PG_IDR_bit.IDR##n
#define PHin(n)  PH_IDR_bit.IDR##n
#define PIin(n)  PI_IDR_bit.IDR##n

#endif

