#ifndef		__CONFIG_H_
#define		__CONFIG_H_

#define		MODE_A	'A'  //正常运行模式
#define 	MODE_B	'B'  //本地设故板继电器类型配置模式

#define		BOARD_typeA		0x11
#define		BOARD_typeB		0x22
#define		BOARD_typeC		0x33

/*********************************************************//*启动参数配置*/
#define		FUN_BOOTUP	 	MODE_A								 //启动模式
#define		BOARD_TYPE		BOARD_typeA							 //配置模式时，设故板类型配置（启动模式为B时使用）

#define		DEBUG				ENABLE							 //是否开启调试串口输出

#define		RELAYDATS_SIZE		16								 //继电器配置字节长度

#define		RELAYDATS_CORRECT	ENABLE						 	 //595数据是否需要倒序更正
/*********************************************************/

//#define MAIN_Fosc		22118400L	//定义主时钟
//#define MAIN_Fosc		12000000L	//定义主时钟
#define MAIN_Fosc		11059200L	//定义主时钟
//#define MAIN_Fosc		 5529600L	//定义主时钟
//#define MAIN_Fosc		24000000L	//定义主时钟


/*********************************************************/

#include	"STC15Fxxxx.H"

/**************************************************************************/

#define Main_Fosc_KHZ	(MAIN_Fosc / 1000)

/***********************************************************/

#endif
