#include "includes.h"
#include "Follow.h" 
#include "MotorDriver.h"
#include "usart.h"
#include "cx.h"
#include "sys.h"

E_SERVO_TURN servoTurn;
E_CAR_TURN CarTurn;
u8 CarSetingTurn;
u16 CarSetTurnTimer;
u8 carStopFlag;
u8 carStopFlag2;
u16 speedOffest;

extern u16 DisableTrafficTimer;
extern u16 discRanging;


/********************************************************/
void CarFollowInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_12;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 		
	GPIO_Init(GPIOB, &GPIO_InitStructure);		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 		
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_15;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 		
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	CarSetTurnTimer = 0;
	CarSetingTurn = TURN_NULL;
	CarTurn = TURN_MIN;
	servoTurn = SERVO_TURN_M;
	speedOffest = 4;
	carStopFlag = 0;

	CarTurnMiddle();
	SetMotorF_PWMDuty(900);  

}

/*********************************************************************
传感器排列顺序左- 右
L1 L2 L3 L4 L5 L6 L7 R8 R7 R6 R5 R4 R3 R2 R1
				   L8

检查到磁条为0 否则为1
**********************************************************************/
#define		R1	0x0001
#define		R2	0x0002
#define		R3	0x0004
#define		R4	0x0008

#define		R5	0x0010
#define		R6	0x0020
#define		R7	0x0040
#define		R8	0x0080

#define		L8	0x0100
#define		L7	0x0200
#define		L6	0x0400
#define		L5	0x0800

#define		L4	0x1000
#define		L3	0x2000
#define		L2	0x4000
#define		L1	0x8000

// 小车寻迹控制
void CarFollowCtr(void)
{
	u16 FollowStats = 0;
	u16 FollowA = 0,FollowB=0,FollowC=0;

	if(CarSetingTurn !=TURN_NULL)
	{
		if(CarSetTurnTimer)
			CarSetTurnTimer--;
		else 
			CarSetingTurn = TURN_NULL;

		return;
	}
		
	FollowB=GPIO_ReadInputData(GPIOB);
	FollowC=GPIO_ReadInputData(GPIOC);
	FollowA=GPIO_ReadInputData(GPIOA);

	FollowStats = ((FollowB<<(15-12))&0x8000)|((FollowB<<(14-8))&0x4000)|((FollowB<<(13-7))&0x2000)|((FollowB<<(12-6))&0x1000)\
				|((FollowB<<(11-5))&0x0800)|((FollowB<<(10-1))&0x0400)|((FollowA>>(15-9))&0x0200)|((FollowC>>(11-8))&0x0100)\
				|((FollowC>>(10-7))&0x0080)|((FollowC<<(6-2))&0x0040)|((FollowC<<(5-3))&0x0020)|(FollowC&0x0010)\
				|((FollowC>>(5-3))&0x0008)|((FollowC>>(9-2))&0x0004)|((FollowB>>(3-1))&0x0002)|((FollowA>>6)&0x0001);

	FollowStats = ~FollowStats;
	FollowStats &= 0x3ffc;
		
	switch(FollowStats)
	{			
		case R1: 		// 去掉R1 R2
		case R2:
		case R3: 
		case R4: 
		case R3|R4: 
			CarTurn = TURN_RIGHT;
			CarTurnRight3();	
			break;
		 
		case R5: 		
			 CarTurnRight2();	
			 break;
			 
		case R6: 				  
			CarTurnRight1();
		 	break;

		case R7: 		
		case R8: 		
		case L8: 		
		case L7: 		
		case R8|R7: 	
		case R8|L7: 	
		case R8|L8: 	
		case R8|L8|R7: 	
		case R8|L8|L7: 	
			CarTurn = TURN_MIN;
			CarTurnMiddle();	
			break;

		case L6:		
			 CarTurnLeft1();  
			break;
			
		case L5:	
			 CarTurnLeft2();  
			break;
		
		case L1: 		// 去掉L1 L2
		case L2:
		case L3: 
		case L4: 
		case L3|L4: 
			CarTurn = TURN_LEFT;
			CarTurnLeft3();   
			break;
			
		default:
			break;
	}	
}


// 小车速度控制
void CarSpeedCtr(void)
{
	//if(carStopFlag||carStopFlag2||((Traffic_red&&(!DisableTrafficTimer))))	// 停车
	if(carStopFlag||carStopFlag2||Traffic_red)	// 停车
	{
		TIM_SetCompare1(TIM1,0);
		TIM_SetCompare4(TIM1,0);
		curSpeed = 0;
	}
	else
	{
		if((discRanging>5)&&(discRanging<100))	// 100mm 超声波检测
		{
			TIM_SetCompare1(TIM1,0);
			TIM_SetCompare4(TIM1,0);
			curSpeed = 0;
		}
		else
		{
			speedOvertimer++;
			if(speedOvertimer >= 2000/5) 	// 2s
			{
				speedOvertimer = 0;
				curSpeed = 0;			
				SetMotorF_PWMDuty(800);
				OSTimeDlyHMSM(0,0,2,0);
			}
			else
			{			
				switch(servoTurn)
				{
				case SERVO_TURN_R3:
				case SERVO_TURN_L3:
				//case SERVO_TURN_R2:
				//case SERVO_TURN_L2:
					SetMotorF_PWMDuty(620);		
					break;
				
				default:
					
				#if CAR_NUMBER==1
						SetMotorF_PWMDuty(240+speedOffest*60);	//Bus 1转弯太快 识别不到
				#else
						SetMotorF_PWMDuty(360+speedOffest*60);	
				#endif 
					break;
				}
			}
		}				
	}
}


