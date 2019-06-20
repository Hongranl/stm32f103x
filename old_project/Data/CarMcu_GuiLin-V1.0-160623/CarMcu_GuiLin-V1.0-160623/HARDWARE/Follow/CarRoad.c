#include "includes.h"
#include "CarRoad.h" 
#include "Follow.h" 
#include "MotorDriver.h"
#include "led.h"
#include "usart2.h"
#include "usart3.h"
#include "sys.h"
#include "cx.h"
#include "Voice.h"


u16 LastCardID;
u16 NewCardID;
u8 NewRoadNum;
u8 SettingRoad;
u8 CarPosition;	
u16 DisableTrafficTimer;

typedef struct{
	u16	CardID;
	u8	Turn;
	u8	StopTimer;
	u8	flag;			
	u8	res;				// res 
}ROAD_MSG;

#define RoadMsgMax		30
ROAD_MSG CarRoadMsgBuf[RoadMsgMax];
ROAD_MSG NewRoadMsgBuf[RoadMsgMax];

// 路标ID
#define RID1		0xEB2C
#define RID2		0xAC3C
#define RID3		0x6D2C
#define RID4		0x852C
#define RID5		0x9CFC
#define RID6		0xFE4C
#define RID7		0x8FDC
#define RID8		0x34DC
#define RID9		0x7C7C
#define RID10	0xEA9C
#define RID11	0x556C
#define RID12	0x6E6C
#define RID13	0x4FAC
#define RID14	0xBDCC
#define RID15	0x7D6C
#define RID16	0x85BC
#define RID17	0xE95C
#define RID18	0x6DDC
#define RID19	0x654C
#define RID20	0x74DC
#define RID21	0x31CC


#ifdef ENABLE_BUS_CAR
const u16 BusStationId[4]=
{
	RID13,
	RID16,
	RID2,
	RID20,
};

u8 IsBusStationId(u16 carId)
{
	u8 i;
	for(i=0;i<sizeof(BusStationId);i++)
	{				
		if(BusStationId[i]==carId)
		{
			break;
		}
	}
	if(i<sizeof(BusStationId))return 1+i;
	else return 0xff;
};

// 公交车道
const ROAD_MSG CarRoad_default1[]={
//	CardID	Turn	  	    Timer flag  res
	{RID21,	TURN_NULL,	0,	1,	0},
	{RID1,	TURN_MIN,	0,	1,	0},
	{RID17,	TURN_NULL,	0,	1,	0},	 
	{RID18,	TURN_RIGHT,	0,	1,	0},	 
	{RID13,	TURN_NULL,	3,	1,	0},	 // 站台
	{RID14,	TURN_MIN,	0,	1,	0},	
	{RID15,	TURN_NULL,	0,	1,	0},	
	{RID16,	TURN_NULL,	3,	1,	0},	 // 站台
	{RID2,	TURN_NULL,	3,	1,	0},	 // 站台
	{RID3,	TURN_MIN,	0,	1,	0},
	{RID4,	TURN_NULL,	0,	1,	0},	
	{RID20,	TURN_NULL,	3,	1,	0},	 // 站台
	{RID5,	TURN_RIGHT,	0,	1,	0},	
	{RID6,	TURN_MIN,	0,	1,	0},	
	{RID9,	TURN_LEFT,	0,	1,	0},
	{RID1,	TURN_NULL,	0,	1,	0},
	{RID21,	TURN_NULL,	0xff,1,	0},

};
#else

const ROAD_MSG CarRoad_default1[]={
//	CardID	Turn	  	    Timer flag  res
	{RID21,	TURN_NULL,	0,	1,	0},
	{RID1,	TURN_RIGHT,	0,	1,	0},
	{RID10,	TURN_RIGHT,	0,	1,	0},	 
	{RID11,	TURN_NULL,	0,	1,	0},	
	{RID15,	TURN_NULL,	0,	1,	0},	
	{RID16,	TURN_NULL,	0,	1,	0},	
	{RID2,	TURN_NULL,	0,	1,	0},	
	{RID3,	TURN_RIGHT,	0,	1,	0},
	{RID8,	TURN_RIGHT,	0,	1,	0},	
	{RID9,	TURN_LEFT,	0,	1,	0},
	{RID1,	TURN_NULL,	0,	1,	0},
	{RID21,	TURN_NULL,	0xff,1,	0},
};
#endif


const ROAD_MSG CarRoad_default2[]={
//	CardID	Turn	  	    Timer flag  res
	{RID21,	TURN_NULL,	0,	1,	0},
	{RID1,	TURN_RIGHT,	0,	1,	0},
	{RID10,	TURN_RIGHT,	0,	1,	0},	 
	{RID11,	TURN_NULL,	0,	1,	0},	
	{RID15,	TURN_NULL,	0,	1,	0},	
	{RID16,	TURN_NULL,	0,	1,	0},	
	{RID2,	TURN_NULL,	0,	1,	0},	
	{RID3,	TURN_MIN,	0,	1,	0},
	{RID4,	TURN_NULL,	0,	1,	0},	
	{RID20,	TURN_NULL,	0,	1,	0},	
	{RID5,	TURN_RIGHT,	0,	1,	0},
	{RID6,	TURN_MIN,	0,	1,	0},	
	{RID9,	TURN_LEFT,	0,	1,	0},
	{RID1,	TURN_NULL,	0,	1,	0},
	{RID21,	TURN_NULL,	0xff,1,	0},
};

const ROAD_MSG CarRoad_default3[]={
//	CardID	Turn	  	    Timer flag  res
	{RID21,	TURN_NULL,	0,	1,	0},
	{RID1,	TURN_RIGHT,	0,	1,	0},
	{RID10,	TURN_MIN,	0,	1,	0},	 
	{RID19,	TURN_RIGHT,	0,	1,	0},	 
	{RID13,	TURN_NULL,	0,	1,	0},	
	{RID14,	TURN_MIN,	0,	1,	0},	
	{RID15,	TURN_NULL,	0,	1,	0},	
	{RID16,	TURN_NULL,	0,	1,	0},	
	{RID2,	TURN_NULL,	0,	1,	0},	
	{RID3,	TURN_RIGHT,	0,	1,	0},
	{RID8,	TURN_RIGHT,	0,	1,	0},	
	{RID9,	TURN_LEFT,	0,	1,	0},
	{RID1,	TURN_NULL,	0,	1,	0},
	{RID21,	TURN_NULL,	0xff,1,	0},
};

const ROAD_MSG CarRoad_default4[1]={0};
const ROAD_MSG CarRoad_default5[1]={0};



// 路口锁定转向时间
typedef struct{
	u16	RoadID;
	u16	LeftTimer;	
	u16	MiderTimer;	
	u16	RightTimer;	
}ROAD_SET_TIMERD;

const ROAD_SET_TIMERD RoadSetingTimerd[]={
//	RoadID    Left		Mider	Right
	{RID1,	1000,	2000,	1000},	
	//{RID1,	1000,	1000,	1000},//修改：2000->1000防止小车直行盲走走不直
	{RID2,	2000,	2000,	2000},	
	{RID3,	2000,	2000,	2000},	
	{RID4,	2000,	2000,	2000},	
	{RID5,	2000,	2000,	2000},	
	{RID6,	2000,	2000,	2000},	
	{RID7,	2000,	2000,	2000},	
	{RID8,	2000,	2000,	2000},	
	{RID9,	2000,	2000,	2000},	
	{RID10,	2000,	2000,	2000},	
	{RID11,	2000,	2000,	2000},	
	{RID12,	2000,	2000,	2000},	
	{RID13,	2000,	2000,	2000},	
	{RID14,	2000,	500,		3000},	
	{RID15,	2000,	2000,	2000},	
	{RID16,	2000,	2000,	2000},	
	{RID17,	2000,	2000,	2000},	
	{RID18,	2000,	2000,	2000},	
	{RID19,	2000,	2000,	2000},	
	{RID20,	2000,	2000,	2000},	
	{RID21,	2000,	2000,	2000},	

};


void CarRoadMsgInt(void)
{
	SettingRoad = 0;
	NewRoadNum = 0;

#ifdef ENABLE_BUS_CAR
	memcpy(CarRoadMsgBuf,CarRoad_default1,sizeof(CarRoad_default1));	
#else

#if CAR_NUMBER==3
	memcpy(CarRoadMsgBuf,CarRoad_default1,sizeof(CarRoad_default1));	
#elif CAR_NUMBER==4
	memcpy(CarRoadMsgBuf,CarRoad_default2,sizeof(CarRoad_default2));	
#elif CAR_NUMBER==5
	memcpy(CarRoadMsgBuf,CarRoad_default3,sizeof(CarRoad_default3));	
#endif

#endif

}

/******************************************************/

u8 CheckCarRoadDefault1(u16 carId)
{
	u8 i;
	for(i=0;i<sizeof(CarRoad_default1)/6;i++)
	{				
		if(CarRoad_default1[i].CardID==carId)
		{
				break;
		}
	}
	if(i<sizeof(CarRoad_default1)/6)
		return i;	
	else
		return 0xff;		
}

u8 CheckCarRoadDefault2(u16 carId)
{
	u8 i;
	for(i=0;i<sizeof(CarRoad_default2)/6;i++)
	{				
		if(CarRoad_default2[i].CardID==carId)
		{
				break;
		}
	}
	if(i<sizeof(CarRoad_default2)/6)
		return i;	
	else
		return 0xff;		
}

u8 CheckCarRoadDefault3(u16 carId)
{
	u8 i;
	for(i=0;i<sizeof(CarRoad_default3)/6;i++)
	{				
		if(CarRoad_default3[i].CardID==carId)
		{
				break;
		}
	}
	if(i<sizeof(CarRoad_default3)/6)
		return i;	
	else
		return 0xff;		
}

u8 CheckCarRoadDefault4(u16 carId)
{
	u8 i;
	for(i=0;i<sizeof(CarRoad_default4)/6;i++)
	{				
		if(CarRoad_default4[i].CardID==carId)
		{
				break;
		}
	}
	if(i<sizeof(CarRoad_default4)/6)
		return i;	
	else
		return 0xff;		
}

u8 CheckCarRoadDefault5(u16 carId)
{
	u8 i;
	for(i=0;i<sizeof(CarRoad_default5)/6;i++)
	{				
		if(CarRoad_default5[i].CardID==carId)
		{
				break;
		}
	}
	if(i<sizeof(CarRoad_default5)/6)
		return i;	
	else
		return 0xff;		
}

void SetCarRoadParking(void)
{
	u8 i;
	for(i=0;i<sizeof(CarRoadMsgBuf)/6;i++)
	{				
		if((CarRoadMsgBuf[i].CardID==RID1)||(CarRoadMsgBuf[i].CardID==RID2)||(CarRoadMsgBuf[i].CardID==RID3))
		{
			CarRoadMsgBuf[i].StopTimer = 0xff;
			break;
		}
	}	
}

void ClearCarRoadParking(void)
{
	u8 i;
	for(i=0;i<sizeof(CarRoadMsgBuf)/6;i++)
	{				
		if((CarRoadMsgBuf[i].CardID==RID1)||(CarRoadMsgBuf[i].CardID==RID2)||(CarRoadMsgBuf[i].CardID==RID3))
		{
			if(CarRoadMsgBuf[i].StopTimer==0xff)CarRoadMsgBuf[i].StopTimer = 0x00;
			break;
		}
	}	
}


u8 CheckCarNewRoad(u16 carId)
{
	u8 i;
	for(i=0;i<sizeof(NewRoadMsgBuf)/6;i++)
	{				
		if(NewRoadMsgBuf[i].CardID==carId)
		{
				break;
		}
	}
	if(i<sizeof(NewRoadMsgBuf)/6)
		return i;	
	else
		return 0xff;		
}



u8 GetCarRoadPosition(u16 carId)
{
	u8 i,Position,flag;
	for(i=0;i<sizeof(CarRoadMsgBuf)/6;i++)		// 先搜索未跑路径(flag=1)
	{				
		if(CarRoadMsgBuf[i].flag)
		{
			CarRoadMsgBuf[i].flag = 0;
			if(CarRoadMsgBuf[i].CardID == carId)break;
		}	
	}
	
	if(i<sizeof(CarRoadMsgBuf)/6)return i;		
	else
	{
		flag = 0;
		Position = 0xff;
		for(i=0;i<sizeof(CarRoadMsgBuf)/6;i++)		// 再搜索全部路径
		{				
			if(flag)CarRoadMsgBuf[i].flag = 1; 		// 从搜到的CardID的下一个ID开始重新设置为未跑路径	
			else if(CarRoadMsgBuf[i].CardID==carId){flag = 1;Position = i;}			
		}		
		if(Position < sizeof(CarRoadMsgBuf)/6)return Position;			
			
	}
	
	return 0xff;		// 未搜到CardID，路径出错
}

u8 GetCarRoadTurn(u8 id)
{
	if(id >= sizeof(CarRoadMsgBuf)/6)
		return  0;
	else
		return  CarRoadMsgBuf[id].Turn;
}

u8 GetCarRoadStopTimer(u8 id)
{
	if(id >= sizeof(CarRoadMsgBuf)/6)
		return  0;
	else
		return  CarRoadMsgBuf[id].StopTimer;
}

u16 GetCarRoadSetTurnTimer(u8 pisotion)
{
	u8 turn ,i;
	u16 id;

	id = CarRoadMsgBuf[pisotion].CardID;
	turn = CarRoadMsgBuf[pisotion].Turn;	
	for(i=0; i<sizeof(RoadSetingTimerd)/8; i++)
	{
		if(RoadSetingTimerd[i].RoadID == id)break;
	}
	if(i<sizeof(RoadSetingTimerd)/8)
	{
		switch(turn)
		{
			case TURN_LEFT:
				return RoadSetingTimerd[i].LeftTimer;
			case TURN_MIN:
				return RoadSetingTimerd[i].MiderTimer;
			case TURN_RIGHT:
				return RoadSetingTimerd[i].RightTimer;					
		}
	}

	return 0;	
}


/***************************************************************/
// PC 机串口通信数据处理
void Uart_PC_Handler(void)
{
	static u8 uart2_check=0;
	u8 ptr,i,temp;

// WIFI/ZIGBEE UART 
	if(uart2_check !=uart2_package_ptr)
	{
		 ptr = uart2_check;
		 uart2_check++;
		 if(uart2_check>UART_PACKAGE_MAX)  uart2_check = 0;
		 uart2_rx_buf[uart2_check][UART_RX_MAX_LEN] = 0;			 

		// AA+55+CMD+LEN+carNum+Ndata+crc1+crc2
		 if(uart2_rx_buf[ptr][4] == CAR_NUMBER)		// PC 设置路径CMD	
		 {
			switch(uart2_rx_buf[ptr][2])
			{
			case 0x08:	// 启动/ 停车
				if(uart2_rx_buf[ptr][5])		// 停车
				{
					CarStop();
				}
				else	// 启动
				{
					CarStar();
				}
				break;
				
			case 0x09:	//  设置车速
				if(speedOffest<11)
		 			speedOffest = uart2_rx_buf[ptr][5];
				break;
				
			case 0x0f:	// 选择路径
				LastCardID = 0x0000;	
		 		SettingRoad = uart2_rx_buf[ptr][5];
				break;
				
			case 0x11:	// 设置自定义路径
				NewRoadNum = uart2_rx_buf[ptr][5];
				for(i=0;i<NewRoadNum;i++)
				{
					temp = uart2_rx_buf[ptr][7+i*4];
					NewRoadMsgBuf[i].CardID = ((u16)temp<<8) |uart2_rx_buf[ptr][6+i*4];
					NewRoadMsgBuf[i].Turn = uart2_rx_buf[ptr][8+i*4];
					NewRoadMsgBuf[i].StopTimer= uart2_rx_buf[ptr][9+i*4];				
				}
				break;
			}		 	
		}
	}
	
}


/***************************************************************/
// RFID 串口通信数据处理
void Uart_Rfid_Handler(void)
{
	static u8 tick=0;
	static u8 uart3_check=0;
	u8 ptr;
	u16 temp;
	u16 crc;
	u8 crc1,crc2;

// RFID UART	
	if(uart3_check !=uart3_package_ptr)
	{
		ptr = uart3_check;
		uart3_check++;
		if(uart3_check>UART_PACKAGE_MAX)uart3_check = 0;
		uart3_rx_buf[uart3_check][UART_RX_MAX_LEN] = 0;	
		
		if(uart3_rx_buf[ptr][2] == 0x21)	 	// 接收到RFID 路标 ID
		{
			temp = uart3_rx_buf[ptr][5];
			NewCardID = ((u16)temp<<8) |uart3_rx_buf[ptr][4];
			uart3_rx_buf[ptr][2] = 0x81;				//  cmd
			uart3_rx_buf[ptr][3] = 0x03;				//  len
			uart3_rx_buf[ptr][4] = CAR_NUMBER;	//  car number	
			uart3_rx_buf[ptr][5] = NewCardID;
			uart3_rx_buf[ptr][6] = NewCardID>>8;					
			crc = Crc16Code(&uart3_rx_buf[ptr][2],5);  // crc       
			crc1 =  crc;
			crc2 =  crc>>8;
			uart3_rx_buf[ptr][7] = crc1;	
			uart3_rx_buf[ptr][8] = crc2;	
			uart2_send_buf(&uart3_rx_buf[ptr][0], 9);	// 发送小车编号+路标ID 给PC
			tick = 0;
				
			if(LastCardID != NewCardID)
			{
				LastCardID = NewCardID;	
				
				// 切换预设路径
				switch(SettingRoad)
				{
				case 1:
					if(CheckCarRoadDefault1(NewCardID) != 0xff)
					{
						memcpy(CarRoadMsgBuf,CarRoad_default1,sizeof(CarRoad_default1));	
						SettingRoad = 0;
					}
					break;
				case 2:
					if(CheckCarRoadDefault2(NewCardID) != 0xff)
					{
						memcpy(CarRoadMsgBuf,CarRoad_default2,sizeof(CarRoad_default2));	
						SettingRoad = 0;
					}
					break;
				case 3:
					if(CheckCarRoadDefault3(NewCardID) != 0xff)
					{
						memcpy(CarRoadMsgBuf,CarRoad_default3,sizeof(CarRoad_default3));	
						SettingRoad = 0;
					}
					break;
				case 4:
					if(CheckCarRoadDefault4(NewCardID) != 0xff)
					{
						memcpy(CarRoadMsgBuf,CarRoad_default4,sizeof(CarRoad_default4));	
						SettingRoad = 0;
					}
					break;	
				case 5:
					if(CheckCarRoadDefault5(NewCardID) != 0xff)
					{
						memcpy(CarRoadMsgBuf,CarRoad_default5,sizeof(CarRoad_default5));	
						SettingRoad = 0;
					}
					break;
				default:
					break;
					
				}						

				// 切换新路径
				if(NewRoadNum)
				{
					if(CheckCarNewRoad(NewCardID) != 0xff)
					{
						memcpy(CarRoadMsgBuf,NewRoadMsgBuf,sizeof(NewRoadMsgBuf));	
						NewRoadNum = 0;
					}	
				}
				
				CarPosition = GetCarRoadPosition(NewCardID);
				if(CarPosition==0xff)	// err 路径出错
				{
					CarStop2();
					BEEP_ON();		// beep	
					OSTimeDlyHMSM(0,0,2,0);	
					BEEP_OFF();	
				}
				else
				{ 
				#ifdef ENABLE_BUS_CAR
					temp = IsBusStationId(NewCardID);
					if(temp !=0xff)
					{
						VoiceCmd_PlayTrack(temp);	// 语音报站
					}
				#endif					

					temp = GetCarRoadStopTimer(CarPosition);			
					if(temp)		
					{
						CarStop2();					
						if(temp!=0xff)
						{
							OSTimeDlyHMSM(0,0,temp,0);
							CarStar2();
						}						
					}
					
					CarSetingTurn = GetCarRoadTurn(CarPosition);	// 获取转向
					CarSetTurnTimer = GetCarRoadSetTurnTimer(CarPosition)/5;		// 获取锁定转向时间
					
					switch(CarSetingTurn)
					{
						case TURN_LEFT:
							CarTurn = TURN_LEFT;
							CarTurnLeft3();
						break;
						case TURN_RIGHT:
							CarTurn = TURN_RIGHT;
							CarTurnRight3();
						break;						
						case TURN_MIN:
							CarTurn = TURN_MIN;
							CarTurnMiddle();
						break;
					}
						
				}			
				
			}
		}
	} 

	tick++;
	if(tick>60)
	{
		tick = 0;
		LastCardID = 0x0000;		
	}	
}





