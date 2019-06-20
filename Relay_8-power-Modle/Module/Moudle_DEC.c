#include "Moudle_DEC.h"
#include "Tips.h"
//文件内包括底板扩展模块插槽及通讯模块插槽的PCB编码检测进程
//函数，当检测到模块插入时根据不同PCB编码激活对应的驱动进程，
//当检查到模块拔出时，终结对应进程；
Moudle_attr Moudle_GTA;
bool Exmod_rest = false;

osThreadId tid_MBDEC_Thread;
osThreadDef(MBDEC_Thread, osPriorityNormal, 1, 512);

extern ARM_DRIVER_USART Driver_USART1; //设备驱动库串口一设备声明

void stdDeInit(void)
{

	TIM_BDTRInitTypeDef TIM_BDTRStruct;

	TIM_BDTRStructInit(&TIM_BDTRStruct);

	EXTI_DeInit();

	ADC_DeInit(ADC1);
}

void MoudleDEC_ioInit(void)
{ //模块检测脚初始化

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE); //使能ADC1通道时钟

	GPIO_InitStructure.GPIO_Pin |= 0xfff8; //硬件ID检测引脚初始化
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //Moudle_Check引脚初始化
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //ExtMOD引脚初始化
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void MBDEC_Thread(const void *argument)
{ //循环检测
	u16 ID_temp;

	bool M_CHG = true;

	//led2_status = led2_r;
	ID_temp = MID_EXEC_raley8_power;
	Moudle_GTA.Extension_ID = (u8)ID_temp;
	for (;;)
	{
		if (M_CHG)
		{
			//keyIFRActive();
			sourceCMThread_Active();
			M_CHG = false;
		}
		if (Exmod_rest)
		{
			M_CHG = true;

			Exmod_rest = false;

			osThreadTerminate(tid_sourceCM_Thread);
		}
		delay_ms(50);
	}
}

void MoudleDEC_Init(void)
{ //模块检测进程激活

	//MoudleDEC_ioInit();
	tid_MBDEC_Thread = osThreadCreate(osThread(MBDEC_Thread), NULL);
}
