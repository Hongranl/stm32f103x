#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   


extern void TIM2_Int_Init(u16 arr,u16 psc);
 
extern void TIM3_Int_Init(u16 arr,u16 psc);
 
extern void TIM8_PWM_Init(u16 arr,u16 psc);
 
 void  ENTER_critical_offtim2(void);
 void  EXTI_critical_offtim2(void);
 
#endif