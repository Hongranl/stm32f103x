#include <Includes.H>

u8 BD_addr,BD_type;	//设故板主板地址及类型

void mainFunActive(void){
#if(DEBUG == ENABLE)
const u8 LOGSIZE = 60;
xdata	u8 Disp_log[60] = {0};
xdata	u8 temp_log[10] = {0};
xdata	u8 loop;
#endif

#if(FUN_BOOTUP == MODE_A) 		//启动行为：正常启动
	static u8 relayControlDats[RELAYDATS_SIZE];
#if(RELAYDATS_CORRECT == 1)
	u8 temp[RELAYDATS_SIZE];
#endif
	BD_addr = Localaddr_get();		//读地址
	BD_type = LocalBDType_get();	//读类型

	memset(Disp_log,0,LOGSIZE * sizeof(u8));
	sprintf(Disp_log,"设故板参数已读取");
	Debug_log(Disp_log);

	Beep_time(200,100);		//开机提示
	Beep_time(200,100);
	
	while(1){
		
		frameInsAccept();		//数据验收
	
		if(!memcmp(relayControlDats,COREDATS,RELAYDATS_SIZE)){	//是否为新数据
		
			memcpy(relayControlDats,COREDATS,RELAYDATS_SIZE);	//更新数据

			switch(BD_type){
			
				case BOARD_typeA:		
#if(RELAYDATS_CORRECT == 1)		//是否需要进行倒序更正
							BytesReverse(relayControlDats,temp,RELAYDATS_SIZE);	
							Frame595Send(temp,RELAYDATS_SIZE);		//设故板执行倒序更正后的更新数据
#endif			
							Frame595Send(relayControlDats,RELAYDATS_SIZE);	//设故板执行更新数据
#if(DEBUG == ENABLE)
							memset(Disp_log,0,LOGSIZE * sizeof(u8));
							sprintf(Disp_log,"设故板数据已更新：");
							for(loop = 0;loop < RELAYDATS_SIZE;loop ++){
								
								sprintf(temp_log,"%02X",relayControlDats[loop]);
								strcat(Disp_log,temp_log);
							}
							Debug_log(Disp_log);
#endif
							Beep_time(80,40);		//更新提示
							Beep_time(80,40);
							
							Frame595Clr(RELAYDATS_SIZE);	//设故板数据清除
							break;
							
				case BOARD_typeB:	break;

				case BOARD_typeC:	break;
							
							default: break;
			}
		}
	}
#elif(FUN_BOOTUP == MODE_B)	//启动行为：写入设故板类型
	u8 relayBoradType = BOARD_TYPE;
	
	EEPROM_SectorErase(0);
	delay_ms(20);
	EEPROM_write_n(0,&relayBoradType,1);
	delay_ms(20);
	
	Beep_time(200,100);		//类型更新提示
	Beep_time(200,100);
#endif
}

int main(void){
	
	Bsp_Init();
	
	mainFunActive();
	
	return 0;
}