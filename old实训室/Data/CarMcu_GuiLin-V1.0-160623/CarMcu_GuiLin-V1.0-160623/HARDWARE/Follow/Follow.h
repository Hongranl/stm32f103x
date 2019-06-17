#ifndef __FOLLOW_H
#define __FOLLOW_H

#include "sys.h"	

// 小车转向
typedef enum{
	TURN_MIN=0,
	TURN_LEFT,
	TURN_RIGHT,
	TURN_NULL,
}E_CAR_TURN;


// 舵机控制
typedef enum
{
	SERVO_TURN_R3=0,
	SERVO_TURN_R2,
	SERVO_TURN_R1,
	SERVO_TURN_M,
	SERVO_TURN_L1,
	SERVO_TURN_L2,
	SERVO_TURN_L3,
		
}E_SERVO_TURN;



extern E_CAR_TURN CarTurn;
extern E_SERVO_TURN servoTurn;
extern u8 CarSetingTurn;
extern u16 CarSetTurnTimer;
extern u8 carStopFlag;
extern u8 carStopFlag2;
extern u16 speedOffest;

#if CAR_NUMBER==1
#define CarTurnLeft3()		{  servoTurn = SERVO_TURN_L3; SetSever_PWMDuty(38); } 
#define CarTurnLeft2()		{  servoTurn = SERVO_TURN_L2; SetSever_PWMDuty(60); }
#define CarTurnLeft1()		{  servoTurn = SERVO_TURN_L1; SetSever_PWMDuty(70); }
//#define CarTurnMiddle()		{  servoTurn = SERVO_TURN_M;  SetSever_PWMDuty(75); }
#define CarTurnMiddle()		{  servoTurn = SERVO_TURN_M;  SetSever_PWMDuty(80); }
#define CarTurnRight1()		{  servoTurn = SERVO_TURN_R1; SetSever_PWMDuty(85); }
#define CarTurnRight2()		{  servoTurn = SERVO_TURN_R2; SetSever_PWMDuty(95); }
#define CarTurnRight3()		{  servoTurn = SERVO_TURN_R3; SetSever_PWMDuty(110); }  

#elif CAR_NUMBER==2
#define CarTurnLeft3()		{  servoTurn = SERVO_TURN_L3; SetSever_PWMDuty(35); } 
#define CarTurnLeft2()		{  servoTurn = SERVO_TURN_L2; SetSever_PWMDuty(60); }
#define CarTurnLeft1()		{  servoTurn = SERVO_TURN_L1; SetSever_PWMDuty(70); }
#define CarTurnMiddle()		{  servoTurn = SERVO_TURN_M;  SetSever_PWMDuty(75); }
#define CarTurnRight1()		{  servoTurn = SERVO_TURN_R1; SetSever_PWMDuty(85); }
#define CarTurnRight2()		{  servoTurn = SERVO_TURN_R2; SetSever_PWMDuty(95); }
#define CarTurnRight3()		{  servoTurn = SERVO_TURN_R3; SetSever_PWMDuty(120); }  

#elif CAR_NUMBER==3

#define CarTurnLeft3()		{  servoTurn = SERVO_TURN_L3; SetSever_PWMDuty(45); } 
#define CarTurnLeft2()		{  servoTurn = SERVO_TURN_L2; SetSever_PWMDuty(60); }
#define CarTurnLeft1()		{  servoTurn = SERVO_TURN_L1; SetSever_PWMDuty(70); }
#define CarTurnMiddle()		{  servoTurn = SERVO_TURN_M;  SetSever_PWMDuty(75); }
#define CarTurnRight1()		{  servoTurn = SERVO_TURN_R1; SetSever_PWMDuty(85); }
#define CarTurnRight2()		{  servoTurn = SERVO_TURN_R2; SetSever_PWMDuty(92); }
#define CarTurnRight3()		{  servoTurn = SERVO_TURN_R3; SetSever_PWMDuty(95); }  


#else

#define CarTurnLeft3()		{  servoTurn = SERVO_TURN_L3; SetSever_PWMDuty(38); } 
#define CarTurnLeft2()		{  servoTurn = SERVO_TURN_L2; SetSever_PWMDuty(60); }
#define CarTurnLeft1()		{  servoTurn = SERVO_TURN_L1; SetSever_PWMDuty(70); }
#define CarTurnMiddle()		{  servoTurn = SERVO_TURN_M;  SetSever_PWMDuty(75); }
#define CarTurnRight1()		{  servoTurn = SERVO_TURN_R1; SetSever_PWMDuty(85); }
#define CarTurnRight2()		{  servoTurn = SERVO_TURN_R2; SetSever_PWMDuty(95); }
#define CarTurnRight3()		{  servoTurn = SERVO_TURN_R3; SetSever_PWMDuty(110); }  

#endif


#define CarStop()				{ carStopFlag = 1; SetMotorF_PWMDuty(0); }
#define CarStar()				{ carStopFlag = 0;carStopFlag2=0;}

#define CarStop2()				{ carStopFlag2 = 1; SetMotorF_PWMDuty(0); }
#define CarStar2()				{ carStopFlag2 = 0;}

void CarFollowInit(void);
void CarFollowCtr(void);
void CarSpeedCtr(void);


#endif


