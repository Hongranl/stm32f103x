#include "includes.h"
#include "Voice.h"
#include "usart.h"	  


extern u8 uart1_rx_buf[UART_RX_MAX_LEN+1];
extern u8 uart1_rx_len;
extern u8 uart1Rx_timerout;

u8 playok;

const u8 VoiceCmd_PlayStatus[5]={0x04,0xFB,0X01,0X00,0X00};		// 查询播放状态
const u8 VoiceCmd_Play[5]={0x04,0xFB,0X01,0X01,0X01};			// 播放
const u8 VoiceCmd_Puse[5]={0x04,0xFB,0X01,0X02,0X02};			// 暂停
const u8 VoiceCmd_Stop[5]={0x04,0xFB,0X01,0X03,0X03};			// 停止
const u8 VoiceCmd_PreTrack[5]={0x04,0xFB,0X01,0X04,0X04};		// 上一曲
const u8 VoiceCmd_NextTrack[5]={0x04,0xFB,0X01,0X05,0X05};		// 下一曲

const u8 VoiceCmd_GetVolume[5]={0x06,0xF9,0X01,0X00,0X00};		// 查询音量

const u8 VoiceCmd_GetPlayMode[5]={0x0B,0xF4,0X01,0X00,0X00};	// 查询循环模式


typedef enum{
	PLAY_RPT_ALL=0,		// 全部循环
	PLAY_RPT_ONE,			// 单曲循环
	PLAY_RPT_DIR,			// 文件夹内循环
	PLAY_RDM,				// 随机播放
	PLAY_ONE,				// 单曲停止
	PLAY_COM,				// 顺序播放
	PLAY_DIR_COM,			// 文件夹顺序播放
	PLAY_DIR_RDM,			// 文件夹随机播放
}E_PLAY_MODE;


// 一般命令
void VoiceCmd_Com(u8 *cmdbuf,u16 len)
{
	uart1_send_buf(cmdbuf,len);
}


// 播放指定曲目命令
void VoiceCmd_PlayTrack(u16 track)
{
	u8 buf[10];
	u8 SM,i;

	playok = 0;
	buf[0] = 0x04;
	buf[1] = 0xFB;
	buf[2] = 0x03;
	buf[3] = 0x06;
	buf[4] = (u8)(track>>8);
	buf[5] = (u8)track;
	SM = 0;
	for(i=0; i<6; i++)
	{
		SM += buf[i];
	}
	buf[6] = SM;
	uart1_send_buf(buf,7);
	
}

// 设置音量
void VoiceCmd_SetVolume(u8 Volume)
{
	u8 buf[10];
	u8 SM,i;
	
	buf[0] = 0x06;
	buf[1] = 0xF9;
	buf[2] = 0x02;
	buf[3] = 0x01;
	buf[4] = Volume;
	SM = 0;
	for(i=0; i<5; i++)
	{
		SM += buf[i];
	}
	buf[5] = SM;
	uart1_send_buf(buf,6);

}

// 设置循环模式
void VoiceCmd_SetPlayMode(E_PLAY_MODE PlayMode)
{
	u8 buf[10];
	u8 SM,i;
	
	buf[0] = 0x0B;
	buf[1] = 0xF4;
	buf[2] = 0x02;
	buf[3] = 0x01;
	buf[4] = PlayMode;
	SM = 0;
	for(i=0; i<5; i++)
	{
		SM += buf[i];
	}
	buf[5] = SM;
	uart1_send_buf(buf,6);
}

void Voice_rx_proing(void)
{
	static u8 uart1Rxfrmer[200];
	static u8 uart1Rxlen;

	if(uart1_rx_len&&(!uart1Rx_timerout))
	{
		uart1Rxlen = uart1_rx_len;
		uart1_rx_len = 0;
		memcpy(uart1Rxfrmer,uart1_rx_buf,uart1Rxlen);

		if((uart1Rxfrmer[0]==0x04)&&(uart1Rxfrmer[1]==0xFB)&&(uart1Rxfrmer[2]==0x03)&&(uart1Rxfrmer[3]==0x0E))
		{
			playok = 1;
		}
	}	
}



 
