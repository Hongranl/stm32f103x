#include "dataManager.h"

osPoolId  memid_SourceBD_pool;								 
osPoolDef(SourceBD_pool, 32, SourceBD_MEAS);	// 内存池定义

osMessageQId MsgBoxID_SBD_Relay;
osMessageQDef(SBD_MsgBox_Relay, 6, &SourceBD_MEAS);		// 消息队列定义
osMessageQId MsgBoxID_SBD_AnaDet;
osMessageQDef(SBD_MsgBox_AnaDet, 6, &SourceBD_MEAS);		// 消息队列定义
osMessageQId MsgBoxID_SBD_DigDet;
osMessageQDef(SBD_MsgBox_DigDet, 6, &SourceBD_MEAS);		// 消息队列定义
osMessageQId MsgBoxID_SBD_UTZigB;
osMessageQDef(SBD_MsgBox_UTZigB, 6, &SourceBD_MEAS);		// 消息队列定义
osMessageQId MsgBoxID_SBD_UT485;
osMessageQDef(SBD_MsgBox_UT485, 6, &SourceBD_MEAS);		// 消息队列定义

void osMsg_memPoolInit(void){

	static bool memInit_flg = false;

	if(!memInit_flg){

		memid_SourceBD_pool	= osPoolCreate(osPool(SourceBD_pool));	//创建内存池
		
		MsgBoxID_SBD_Relay 	= osMessageCreate(osMessageQ(SBD_MsgBox_Relay), NULL);    //创建消息队列
		MsgBoxID_SBD_AnaDet = osMessageCreate(osMessageQ(SBD_MsgBox_AnaDet), NULL);   //创建消息队列
		MsgBoxID_SBD_DigDet = osMessageCreate(osMessageQ(SBD_MsgBox_DigDet), NULL);   //创建消息队列
		MsgBoxID_SBD_UTZigB = osMessageCreate(osMessageQ(SBD_MsgBox_UTZigB), NULL);   //创建消息队列
		MsgBoxID_SBD_UT485 	= osMessageCreate(osMessageQ(SBD_MsgBox_UT485), NULL);    //创建消息队列

		memInit_flg = true;
	}
}
