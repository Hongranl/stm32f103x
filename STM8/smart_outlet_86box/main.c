/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训室项目
 * @Version:    V 0.2 
 * @Module:     main
 * @Author:     RanHongLiang
 * @Date:       2019-06-20 14:52:41
 * @Description: 
 *	智能插座，
 *	重启时：蓝灯闪烁表示已入网，红灯闪烁表示未入网。
 *	正常运行时：
 * 		单次按键切换状态，长按按键3S进入网络切换状态
 * 		灯亮即电源指示，蓝灯表示通电未输出市电，红灯反之。
 * 		（红灯闪烁，为切换网络状态，蓝灯闪烁为以入网）
 *---------------------------------------------------------------------------*/

#include "ALL_Includes.h"

//定义CPU内部时钟
#define SYS_CLOCK 16
#define TIM4_PERIOD 124
//数据长度，go to->INTERRUPT_HANDLER()
u8 Data_Len = 0;
//接收标志，go to->INTERRUPT_HANDLER()
u8 uart_rx_flg = 0;
//入网状态
u8 join_flg = 0; 
//mcu的板载唯一ID
uint32_t MCU_UID[3];
//按键状态
u8 key_status = 2;
//串口数据计时
u16 UART_NO_DAT_TIME = 0;
//系统运行时间(ms)
uint32_t SystemCnt = 0; 
//消息结构体
TRAN_D_struct TRAN_info1;
//网络状态变量
NETstatus gb_Status = net_error;
//上一个网络状态变量
NETstatus gb_Status_pre = net_error;
//全局倒计时
u16 gb_countdown = 0;
//初始化倒计时
u16 gb_init_countdown = 0;
//串口倒计时，超时发送
u16 gb_countdown_uart = 0;
//全局传感器数据
u16 gb_sensor_data[2];


/**************************************
* @description: uS(微秒)延时函数
* @param 类型：u16 范围： 0~65535
* @return: 无
************************************/
void Delay(uint16_t nCount);
/**************************************
* @description: 初始化处理函数，包括IO 、串口、 ADC等
* @param 无
* @return: 无
************************************/
void All_Config(void);
/**************************************
* @description: GPIO和ADC初始化
* @param 无
* @return: 无
************************************/
void IO_Init(void);
/**************************************
* @description: TIM4初始化，用于程序的全局倒计时计数
* @param 无
* @return: 无
************************************/
void TIM4_Config(void);
/**************************************
* @description: 按键值改变检测函数，长按切换网络状态，短按切换工作状态
* @param 无
* @return: 无
************************************/
void check_key(void);
/**************************************
* @description: 网络状态LED指示函数
* @param 无
* @return: 无
************************************/
void net_led_status(void);
/**************************************
* @description: 传感器状态检测，有改变就上传
* @param 无
* @return: 无
************************************/
void check_sensor(void);
/**************************************
* @description: MCU的板载ID获取，存放在传入的指针变量中
* @param u32的指针参数
* @return: 无
************************************/
void HAL_GetUID(uint32_t *UID);
/**************************************
* @description: 获取当前继电器状态，并且输出对应的LED状态
* @param {type} 
* @return: 
************************************/
void put_raly2led_status(void);

/**************************************
* @description: 主函数，程序的启动的入口
* @param 无 
* @return: 无
************************************/
void main(void)
{
	All_Config();

	TRAN_info1.dest_dev_num = 0; //目标设备号

	TRAN_info1.dest_addr[0] = 0x00; //目标地址
	TRAN_info1.dest_addr[1] = 0x00;
	TRAN_info1.dest_addr[2] = 0x00;
	TRAN_info1.dest_addr[3] = 0x00;

	TRAN_info1.source_addr[0] = (u8)MCU_UID[0]; //源地址
	TRAN_info1.source_addr[1] = (u8)(MCU_UID[0] >> 8);
	TRAN_info1.source_addr[2] = (u8)(MCU_UID[0] >> 16);
	TRAN_info1.source_addr[3] = (u8)(MCU_UID[0] >> 24);

	TRAN_info1.TYPE_NUM = 3; //数据格式号: 第三套数据格式  用于ZIGBEE（非标准）  设备端
				 //  GPIO_Init(SHT10_SCK_PORT, SHT10_SCK_PIN,GPIO_MODE_OUT_PP_HIGH_FAST);
				 //    while(1)
				 //    	{ SHT10_Dly();
				 //                SHT10_SCK_H();
				 //                SHT10_Dly();
				 //                SHT10_SCK_L();}
	while (1)
	{
		check_key();
		net_led_status();
		check_sensor();
		;
		Rs485_COMM_uart_fuc();
	}
}
void check_sensor(void)
{

#ifdef sensor_md_BODY

	//有改变就上传

	if ((body_check != gb_sensor_data[0]) || (gb_countdown_uart < 1))
	{

		delay_ms(20);
		if ((body_check != gb_sensor_data[0]) || (gb_countdown_uart < 1))
		{
			u8 dat[20];
			dat[0] = 0;
			dat[1] = upload_info;
			dat[2] = 0;

			dat[4] = (u8)MCU_UID[0];
			dat[5] = (u8)(MCU_UID[0] >> 8);
			dat[6] = (u8)(MCU_UID[0] >> 16);
			dat[7] = (u8)(MCU_UID[0] >> 24);

			memset(dat + 8, 0, 8);

			TRAN_info1.source_dev_num = (SENSOR_body << 8 | SENSOR_body >> 8);

			TRAN_info1.data_len = 16 + 1;

			dat[3] = SENSOR_body; //

			dat[8 + 8] = body_check;
			gb_sensor_data[0] = body_check;

			//清空定时器
			gb_countdown_uart = 6000;
			Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
		}
	}

	//#else
	//有改变就上传,
	if ((get_adc(1) > 850) || (fire_check != gb_sensor_data[1]) || (gb_countdown_uart < 1))
	{
		u16 buff = get_adc(10);
		delay_ms(20);
		if ((buff > 850) || (fire_check != gb_sensor_data[1]) || (gb_countdown_uart < 1))
		{
			u8 dat[20];
			dat[0] = 0;
			dat[1] = upload_info;
			dat[2] = 0;

			dat[4] = (u8)MCU_UID[0];
			dat[5] = (u8)(MCU_UID[0] >> 8);
			dat[6] = (u8)(MCU_UID[0] >> 16);
			dat[7] = (u8)(MCU_UID[0] >> 24);

			memset(dat + 8, 0, 8);

			TRAN_info1.source_dev_num = (DTN_86_SENSOR_MQ << 8 | DTN_86_SENSOR_MQ >> 8);

			TRAN_info1.data_len = 16 + 2;

			dat[3] = DTN_86_SENSOR_MQ; //

			if (buff > 850)
			{
				dat[8 + 8] = 1;
			}
			else
			{
				dat[8 + 8] = 0;
			}

			dat[8 + 8 + 1] = fire_check;

			gb_sensor_data[1] = fire_check;

			gb_countdown_uart = 6000;

			if ((buff > 850) || (fire_check == 1))
			{

				BEEP_s(1);
			}
			else
				BEEP_s(0);

			Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
		}
	}

#endif
#ifdef tem_hum_light

	if (gb_countdown_uart < 1)
	{
		u8 dat[30] = {0};

		gb_countdown_uart = 6000;

		dat[0] = 0;
		dat[1] = upload_info;
		dat[2] = 0;

		dat[4] = (u8)MCU_UID[0];
		dat[5] = (u8)(MCU_UID[0] >> 8);
		dat[6] = (u8)(MCU_UID[0] >> 16);
		dat[7] = (u8)(MCU_UID[0] >> 24);

		memset(dat + 8, 0, 8);

		TRAN_info1.source_dev_num = (SENSOR_tem_hum_light << 8 | SENSOR_tem_hum_light >> 8);

		TRAN_info1.data_len = SENSOR_tem_LEN;

		///------------------通用数据-------------------------------------------------///
		dat[3] = SENSOR_tem_hum_light; //

		memset(dat + 16, 0, 12);
		tempMS_Thread(dat + 0x10);
		lightMS_Thread(dat + 0x18);
		//				SHT1X_Config();
		//				dat[24] = SHT1X_GetValue(&tem,&hum,&temp_real,&hum_real);
		//
		//
		//				//f =CLK_GetClockFreq();
		//				memcpy(dat+16,&tem,2);
		//				memcpy(dat+20,&hum,2);

		Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
	}

#endif
#ifdef power_outlet

	if (gb_countdown_uart < 1)
	{
		u8 dat[30] = {0};

		gb_countdown_uart = 6000*5;

		dat[0] = 0;
		dat[1] = upload_info;
		dat[2] = 0;

		dat[4] = (u8)MCU_UID[0];
		dat[5] = (u8)(MCU_UID[0] >> 8);
		dat[6] = (u8)(MCU_UID[0] >> 16);
		dat[7] = (u8)(MCU_UID[0] >> 24);

		memset(dat + 8, 0, 8);

		TRAN_info1.source_dev_num = (DTN_86_power_outlet << 8 | DTN_86_power_outlet >> 8);

		TRAN_info1.data_len = power_outlet_LEN;

		///------------------通用数据-------------------------------------------------///
		dat[3] = DTN_86_power_outlet; //

		memset(dat + 16, 0, 1);
		
		dat[16] = Rly_Vlue;
		Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
	}

#endif
	;
}

void check_key(void)
{
	u16 i = 0;
	u8 dat[2];
	if (KEY)
	{

		delay_ms(10);
		while (KEY && (++i < 500))
			delay_ms(10);
		if (i > 0 && i < 100)
		{
			
#ifdef power_outlet	
			 
			Rly(!Rly_Vlue);
			put_raly2led_status();
			gb_countdown_uart = 0;//改变即上传
#endif
		}
		if (i > 400)
		{
			TRAN_info1.data_len = 2;

			dat[0] = 0;

			if (gb_Status == net_online) //在线即退网
			{
				gb_Status_pre = gb_Status;
				gb_Status = net_error;

				dat[1] = leave_net;
			}
			else
			{
				gb_Status_pre = gb_Status;
				gb_Status = net_wait;
				gb_countdown = 6000 * 5; //5s

				dat[1] = join_sw;
			}

			Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
		}
	}
}

void net_led_status(void)
{
#ifdef tem_hum_light
	if (gb_Status == net_wait)
	{

		if (gb_countdown > 1)
		{
			LED(0);
			delay_ms(300);
			LED(1);
			delay_ms(300);
		}
		else //超时---->>>>>>未入网
		{
			gb_Status = net_error;
			LED(0);
		}
	}
	else if (gb_Status == net_error)
	{
		//if(gb_Status_pre != net_error)
		LED(0);
	}
	else if (gb_Status == net_online)
	{ //if(gb_Status_pre != net_online)
		LED(1);
	}
#endif
#ifdef sensor_md_BODY
	if (gb_Status == net_wait)
	{

		if (gb_countdown > 1)
		{
			LED(0);
			delay_ms(300);
			LED(1);
			delay_ms(300);
		}
		else //超时---->>>>>>未入网
		{
			gb_Status = net_error;
			LED(0);
		}
	}
	else if (gb_Status == net_error)
	{
		//if(gb_Status_pre != net_error)
		LED(0);
	}
	else if (gb_Status == net_online)
	{ //if(gb_Status_pre != net_online)
		LED(1);
	}
#endif
#ifdef power_outlet	
	
	if (gb_Status == net_wait)
	{

		if (gb_countdown > 1)
		{
			LED_b(0);
			LED_r(0);
			delay_ms(300);
			LED_r(1);
			delay_ms(300);
		}
		else //超时---->>>>>>未入网
		{
			gb_Status = net_error;
			put_raly2led_status();
			
		}
	}
	else if (gb_Status == net_error && gb_Status_pre != gb_Status)
	{
		u8 i = 0;
		 LED_b(0);
		for ( ; i < 7; i++)
		{
			LED_r(i%2);
			delay_ms(200);
		}
		put_raly2led_status();
	}
	else if (gb_Status == net_online && gb_Status_pre != gb_Status)
	{ 
		
		u8 i = 0;
		LED_r(0);
		for ( ; i < 7; i++)
		{
			LED_b(i%2);
			delay_ms(200);
		}
		put_raly2led_status();
	}
#endif
	gb_Status_pre = gb_Status;
	;
}
void put_raly2led_status(void)
{
	LED_b(0);
	LED_r(0);
	Rly_Vlue ? LED_r(1) : LED_b(1);
}

void HAL_GetUID(uint32_t *UID)
{
	UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
	UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
	UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
}

void All_Config(void)
{

	CLK->CKDIVR &= ~(BIT(4) | BIT(3));

	IO_Init();
	UART1_DeInit();
	UART1_Init((u32)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
	UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
	UART1_Cmd(ENABLE);

	HAL_GetUID(MCU_UID);

	TIM4->PSCR = (uint8_t)(TIM4_PRESCALER_128);
	/* Set the Autoreload value */
	TIM4->ARR = (uint8_t)(TIM4_PERIOD);
	TIM4->SR1 = (uint8_t)(~TIM4_FLAG_UPDATE);
	TIM4->IER |= (uint8_t)TIM4_IT_UPDATE;
	TIM4->CR1 |= TIM4_CR1_CEN;

	enableInterrupts();
#ifdef power_outlet

	gb_init_countdown = 5000;
	Z_rest(0);
	delay_ms(20);
	Z_rest(1);

	while (gb_init_countdown) {
		
		if (gb_Status == net_error )
		{
			u8 i = 0;
			
			for ( ; i < 7; i++)
			{
				LED_r(i%2);
				delay_ms(200);
			}
			
		}
		else if (gb_Status == net_online )
		{ 
			
			u8 i = 0;
			
			for ( ; i < 7; i++)
			{
				LED_b(i%2);
				delay_ms(200);
			}

			gb_Status_pre = gb_Status;
			gb_init_countdown = 0;
		}
		Rs485_COMM_uart_fuc();
	}

	put_raly2led_status();
#endif
}


void IO_Init(void)
{

#ifdef sensor_md_BODY

	GPIOA->DDR |= Pin(3); //输出模式
	GPIOA->CR1 |= Pin(3); //推挽输出

	GPIOA->DDR &= ~Pin(1); //输入模式
	GPIOA->DDR &= ~Pin(2); //输入模式

	//#else

	//mq5煤气传感器，fire火焰传感器
	GPIOA->DDR |= Pin(3); //输出模式
	GPIOA->CR1 |= Pin(3); //推挽输出

	GPIOC->DDR |= Pin(7); //输出模式
	GPIOC->CR1 |= Pin(7); //推挽输出

	GPIOC->DDR &= ~Pin(6); //输入模式
	GPIOD->DDR &= ~Pin(2); //输入模式
	GPIOD->DDR &= ~Pin(3); //输入模式

	GPIOD->CR1 &= ~Pin(2); //浮空输入
	//ADC1 channel3
	ADC1_DeInit();
	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, ADC1_CHANNEL_3,
		  ADC1_PRESSEL_FCPU_D2, ADC1_EXTTRIG_TIM, DISABLE,
		  ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL3, DISABLE);

	ADC1_Cmd(ENABLE);
	ADC1_StartConversion();
	BEEP_s(0);

	LED(0);

#endif

#ifdef tem_hum_light

	//KEY
	GPIOC->DDR &= ~Pin(6); //输入模式
	//led1
	GPIOC->DDR |= Pin(7); //输出模式
	GPIOC->CR1 |= Pin(7); //推挽输出
	LED(0);
#endif

#ifdef power_outlet

	//KEY
	GPIOA->DDR &= ~GPIO_PIN_1; //输入模式
	//led_r
	GPIOD->DDR |= GPIO_PIN_2; //输出模式
	GPIOD->CR1 |= GPIO_PIN_2; //推挽输出
	//led_b
	GPIOD->DDR |= GPIO_PIN_3; //输出模式
	GPIOD->CR1 |= GPIO_PIN_3; //推挽输出
	//z_rest
	GPIOD->DDR |= GPIO_PIN_4; //输出模式
	GPIOD->CR1 |= GPIO_PIN_4; //推挽输出
	//Rly
	GPIOC->DDR |= GPIO_PIN_7; //输出模式
	GPIOC->CR1 |= GPIO_PIN_7; //推挽输出
	LED_r(0);
	LED_b(0);
	Z_rest(1);
	Rly(0);
	
#endif

	//串口-TX
	GPIOD->DDR |= BIT(5); //输出模式
	GPIOD->CR1 |= BIT(5); //推挽输出

	//串口-RX
	GPIOD->DDR &= ~BIT(6); //输入模式

	
}

void Delay(uint16_t nCount)
{
	/* Decrement nCount value */
	while (nCount != 0)
	{
		nCount--;
	}
}

void delay_ms(uint16_t nCount)
{
	u16 i = 0, n = 0;

	for (i = 0; i < nCount; i++)
	{
		n = 1000;
		while (n != 0)
		{
			n--;
		}
	}
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif
