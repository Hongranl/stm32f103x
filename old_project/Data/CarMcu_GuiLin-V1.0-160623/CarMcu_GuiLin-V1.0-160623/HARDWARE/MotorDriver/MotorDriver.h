#ifndef __MOTOR_DRIVER_H
#define __MOTOR_DRIVER_H	 
#include "sys.h"



void Motor_Init(void);
void SeverPWMInit(void);
void SetSever_PWMDuty(u16 Pulse);
void SetMotorF_PWMDuty(u16 Pulse);
void SetMotorB_PWMDuty(u16 Pulse);

#endif


	
