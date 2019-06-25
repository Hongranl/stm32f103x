

#ifndef _ALL_Includes_H
#define _ALL_Includes_H

#include "stdio.h"
#include "stm8s.h"
#include "stm8s_adc1.h"
#include "stm8s_clk.h"
#include "stm8s_gpio.h"
#include "uart.h"
#include <ctype.h>
#include <string.h>

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

//设备号的定义
#define DTN_motor_window 0x32  //设备号   电动窗帘
#define DTN_curtain 0x33       //设备号   电动幕帘
#define DTN_POWER_SUBAREA 0x34 //设备号   电源分区管理
// 86开关灯面板1路 	54
// 86开关灯面板2路 	55
// 86开关灯面板3路 	56
#define DTN_86_LIGHT_1  0x36       //设备号   一路灯光
#define DTN_86_LIGHT_2  0x37       //设备号   两路灯光
#define DTN_86_LIGHT_3  0x38       //设备号   三路灯光
#define DTN_86_SENSOR_MQ 0x39     //设备号   煤气泄漏和火焰传感器
#define SENSOR_body     0x3A          //设备号   人体传感器
#define SENSOR_tem_hum_light 0x3B //设备号   温湿度和光照强度
#define DTN_86_power_outlet 0x3C  //设备号   智能插座
//数据包长度
#define SENSOR_tem_LEN 0x1c //温湿度模块-数据长度
#define power_outlet_LEN 0x11 //智能插座模块-数据长度
//mcu的板载ID宏定义和地址
#define READ_REG(REG) ((REG))
#define UID_BASE 0X4865
//CMD的枚举变量
typedef enum
{
        join_sw = 1,       // 入网开关
        leave_net,         // 退网命令
        notify_net_status, // 通知入网状态
        upload_info,       // 上传信息到Android
        control_info,      // 控制设备
        query_dev_info     // 查询设备信息

} dev_cmd;
//网络状态的枚举
typedef enum
{
        net_error,
        net_wait,
        net_online

} NETstatus;

//使用模块切换
//#define sensor_md_BODY
//#define tem_hum_light
#define power_outlet

//GPIO->PIN_X  自用宏定义
#define Pin(P) (1 << P)

// SENSOR传感器使能，（定义:使用人体，反之使用煤气报警）
#ifdef sensor_md_BODY

#define body_check (((GPIOA->IDR & Pin(1)) == 0) ? 1 : 0)
#define KEY \
        (((GPIOA->IDR & Pin(2)) == 0) ? 1 : 0) // (GPIOA->IDR & Pin(2) )
#define LED(a) \
        (a == 1) ? (GPIOA->ODR &= ~Pin(3)) : (GPIOA->ODR |= Pin(3))

//#else

// mq5煤气传感器，fire火焰传感器

#define fire_check (((GPIOD->IDR & Pin(3)) == 0) ? 1 : 0)
#define KEY (((GPIOC->IDR & Pin(6)) == 0) ? 1 : 0)
#define LED(a) \
        (a == 1) ? (GPIOA->ODR &= ~Pin(3)) : (GPIOA->ODR |= Pin(3))
#define BEEP_s(a) \
        (a == 0) ? (GPIOC->ODR &= ~Pin(7)) : (GPIOC->ODR |= Pin(7))

#endif

// 定义使用温湿度和光照强度模块！
#ifdef tem_hum_light

#define KEY (((GPIOC->IDR & Pin(6)) == 0) ? 1 : 0)

#define LED(a) \
        (a == 1) ? (GPIOC->ODR &= ~Pin(7)) : (GPIOC->ODR |= Pin(7))
#endif

// 定义使用智能插座模块！
#ifdef power_outlet

#define KEY (((GPIOA->IDR & GPIO_PIN_1) == 0) ? 1 : 0)

#define LED_r(a)    (a == 1) ? (GPIOD->ODR &= ~GPIO_PIN_2) : (GPIOD->ODR |= GPIO_PIN_2)
#define LED_b(a)    (a == 1) ? (GPIOD->ODR &= ~GPIO_PIN_3) : (GPIOD->ODR |= GPIO_PIN_3)
#define Z_rest(a)   (a == 0) ? (GPIOD->ODR &= ~GPIO_PIN_4) : (GPIOD->ODR |= GPIO_PIN_4)
#define Rly(a)      (a == 0) ? (GPIOC->ODR &= ~GPIO_PIN_7) : (GPIOC->ODR |= GPIO_PIN_7)
#define Rly_Vlue    (((GPIOC->ODR & GPIO_PIN_7) != 0) ? 1 : 0)
#endif

//函数和变量，作用范围的声明
extern NETstatus gb_Status;
extern NETstatus gb_Status_pre;
extern u16 gb_init_countdown;
extern u16 gb_countdown;
extern u16 gb_countdown_uart;
extern uint32_t SystemCnt;

extern uint32_t MCU_UID[3];
extern u32 ANDROID_DEST_ADDR;
extern u8 search_addr_flg;
extern u8 join_flg;
extern u8 Data_Len;
extern u8 uart_rx_flg;

extern u8 sampling_in_flg;

extern u16 UART_NO_DAT_TIME;
extern TRAN_D_struct TRAN_info1;

extern void Delay(uint16_t nCount);
extern void delay_ms(uint16_t nCount);
extern u16 get_adc(u8 times);

#endif
