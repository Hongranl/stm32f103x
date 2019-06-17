#include  "global.h"

 
 
 
void Hex2Char( u8* pDataIn, u16 NumberOfByte, char* pString );


 /**
  * @brief  convert a ASCII coding byte to it's representing char.
  * @param  pDataIn : pointer on the byte array to translate
	* @param  NumberOfByte : the size of the array
	* @param  pString : pointer on the string created
  * @retval None
  */
  
void Hex2Char( u8* pDataIn, u16 NumberOfByte, char* pString )
{
	u8 data;
	uint8_t i=0;
	
	for(i=0; i<NumberOfByte; i++)
	{
		/* First char */
		data = (*pDataIn & 0xF0)>>4;
		if( data < 0x0A)
			*pString = data + 0x30;  /* ASCII offset for number */
		else
			*pString = data + 0x37; 	/* ASCII offset for letter */
	
		pString++;
	
		/* Second char */
		data = (*pDataIn & 0x0F);
		if( data < 0x0A)
			*pString = data + 0x30;  /* ASCII offset for number */
		else
			*pString = data + 0x37; 	/* ASCII offset for letter */
	
		pString++;
		pDataIn++;
	}
}


void CR95HF_FUC( uint8_t tagfounds )  
{
  int i=0;  
	
  uint8_t TagType = TRACK_NOTHING   ;
	
	u8 loop;
	
	u8 CBUF_CNT = 0;
	const u8 CBUF_CNT_period = 3;
	
	const char dispLength = 30;
	char disp[dispLength];
	
	const char idLength = 20;
	char UID[idLength] = {' '};
	char InvertedUID[idLength]  = {' '};
	char LastUIDFound[idLength] = {' '};
	
	
  TagType = ConfigManager_TagHunting(tagfounds);
		// Turn on the LED if a tag was founded
		/* Check the tag type found */
		
		if(TagType == TRACK_NOTHING){
		
		 
			
			if(CBUF_CNT < CBUF_CNT_period)CBUF_CNT ++;
			else{
			
				memset(LastUIDFound,0,idLength * sizeof(char));
			}
		}else{
		
			 
			memset(InvertedUID,0,idLength * sizeof(char));
			memset(disp,0,dispLength * sizeof(char));
			
			CBUF_CNT = 0;
			
			 RFID_LED=0;	  //  输出
			 for(i=0;i<500;i++)
			 {	 
			 
			 GPIO_SetBits(GPIOC,GPIO_Pin_4);	  //  输出高	 
			 delay_us(150 ); 
			 GPIO_ResetBits(GPIOC,GPIO_Pin_4);	  //  输出   
			  delay_us(150 ); 
			 }
			 RFID_LED=1; 
			 
			switch(TagType){
			
				case TRACK_NFCTYPE1:{

						Hex2Char( TagUID, 6, UID);
				
						for(loop = 0; loop < 12; loop = loop + 2){
							
							InvertedUID[11 - loop] = UID[loop + 1];
							InvertedUID[11 - (loop + 1)] = UID[loop];
						}
						memcpy(&InvertedUID[17],"TT1",3);
						/* Set the Text Color */
						if(memcmp (LastUIDFound, TagUID, 6)){	
							
							memset(LastUIDFound,0,idLength * sizeof(char));
							memcpy(LastUIDFound,TagUID,6);
							
							sprintf(disp,"TT1:%s\r\n",UID);
							//Driver_USART3.Send((uint8_t*)disp,strlen((const char*)disp));			
							   
						}
						else{
							
							;
						}
					}break;
				
				case TRACK_NFCTYPE2:{
					
						Hex2Char( ISO14443A_Card.UID, ISO14443A_Card.UIDsize, UID);
						
						memcpy(&UID[17],"TT2",3);
						if(memcmp (LastUIDFound, ISO14443A_Card.UID, ISO14443A_Card.UIDsize)){	
							
							memset(LastUIDFound,0,idLength * sizeof(char));
							memcpy(LastUIDFound,ISO14443A_Card.UID,ISO14443A_Card.UIDsize);	
							
							sprintf(disp,"TT2:%s\r\n",UID);
						//	Driver_USART3.Send((uint8_t*)disp,strlen((const char*)disp));		
							 
						}
						else{
							
							;
						}
					}break;
				
				case TRACK_NFCTYPE3:{
				
						Hex2Char( FELICA_Card.UID, 8 , UID);
					
						memcpy(&UID[17],"TT3",3);
						
						if(memcmp (LastUIDFound, FELICA_Card.UID, 8)){	
							
							memset(LastUIDFound,0,idLength * sizeof(char));
							memcpy(LastUIDFound,FELICA_Card.UID,8);
							
							sprintf(disp,"TT3:%s\r\n",UID);
						//	Driver_USART3.Send((uint8_t*)disp,strlen((const char*)disp));		 						
						}
						else{
							
							;
						}
					}break;
				
				case TRACK_NFCTYPE4A:{
					
						Hex2Char( ISO14443A_Card.UID, ISO14443A_Card.UIDsize, UID);
						
						memcpy(&UID[16],"TT4A",4);
						
						if(memcmp (LastUIDFound, ISO14443A_Card.UID, ISO14443A_Card.UIDsize)){	
							
							memset(LastUIDFound,0,idLength * sizeof(char));
							memcpy(LastUIDFound,ISO14443A_Card.UID,ISO14443A_Card.UIDsize);	
							
							sprintf(disp,"TT4A:%s\r\n",UID);
						//	Driver_USART3.Send((uint8_t*)disp,strlen((const char*)disp)); 				
						}
						else{
							
							;
						}
					}break;
				
				case TRACK_NFCTYPE4B:{
					
						Hex2Char( ISO14443B_Card.PUPI, 4 , UID);
					
						memcpy(&UID[16],"TT4B",4);
				
						if(memcmp (LastUIDFound, ISO14443B_Card.PUPI, 4)){	
							
							memset(LastUIDFound,0,idLength * sizeof(char));
							memcpy(LastUIDFound,ISO14443B_Card.PUPI,4);		
							
							sprintf(disp,"TT4B:%s\r\n",UID);
						//	Driver_USART3.Send((uint8_t*)disp,strlen((const char*)disp));	 
						}
						else{
							
							;
						}
					}break;
				
				case TRACK_NFCTYPE5:{
					
						Hex2Char( TagUID, 8, UID);
						
						for(loop = 0; loop < 16; loop = loop + 2){
							
							InvertedUID[15-loop] = UID[loop+1];
							InvertedUID[15-(loop+1)] = UID[loop];
						}
						memcpy(&InvertedUID[17],"TT5",3);

						if(memcmp (LastUIDFound, TagUID, 8)){	
							
							memset(LastUIDFound,0,idLength * sizeof(char));
							memcpy(LastUIDFound,TagUID,8);		
							
							sprintf(disp,"TT5:%s\r\n",UID);
						//	Driver_USART3.Send((uint8_t*)disp,strlen((const char*)disp));	 
						}
						else{
							
							;
						}
					}break;
				
				default: 
					
				break;
			}
		}
 
	
}

 

