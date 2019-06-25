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
	int i = 0;  

	uint8_t TagType = TRACK_NOTHING   ;

	TRAN_D_struct   TRAN_info1;

	uint8_t   dat[200];
	 
	
  TagType = ConfigManager_TagHunting(tagfounds);
		// Turn on the LED if a tag was founded
		/* Check the tag type found */
		
	if(TagType == TRACK_NOTHING)
	{

	}
	else
	{ 
		LED_RFID=0;	  //  输出
		for(i=0;i<500;i++)
		{	 
			BEEP_OUT=1;	  //  输出高	 
			delay_us(150 ); 
			BEEP_OUT=0;	  //  输出   
			delay_us(150 ); 
		}
		LED_RFID=1; 

		switch(TagType)
		{
			case TRACK_NFCTYPE1:
			{
				key_tab[2]=0;//解除锁定
				LED_DAT[1]&=~L_R1C3; 

				write_595_LED( LED_DAT,2);

				TRAN_info1.dest_dev_num= (DTN_android<<8|DTN_android>>8);

				TRAN_info1.dest_addr[0]=0x00;
				TRAN_info1.dest_addr[1]=0x00;
				TRAN_info1.dest_addr[2]=0x00;
				TRAN_info1.dest_addr[3]=0x00;

				TRAN_info1.source_dev_num= (DTN_TER_RIFD<<8|DTN_TER_RIFD>>8);

				TRAN_info1.source_addr[0]=(u8)MCU_UID[0];
				TRAN_info1.source_addr[1]=(u8)(MCU_UID[0]>>8);
				TRAN_info1.source_addr[2]=(u8)(MCU_UID[0]>>16);
				TRAN_info1.source_addr[3]=(u8)(MCU_UID[0]>>24);

				TRAN_info1.TYPE_NUM=4;
				TRAN_info1.data_len= 8+2;

				dat[0]=00;
				dat[1]=07; 
				dat[2]=00; 
				dat[3]=00; 
				memcpy(dat+4 , (u8*)TagUID,6);		 

				RS485_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   ); 

			}break;

		case TRACK_NFCTYPE2:
		{
			key_tab[2]=0;//解除锁定
			LED_DAT[1]&=~L_R1C3; 

			write_595_LED( LED_DAT,2);

			TRAN_info1.dest_dev_num= (DTN_android<<8|DTN_android>>8);

			TRAN_info1.dest_addr[0]=0x00;
			TRAN_info1.dest_addr[1]=0x00;
			TRAN_info1.dest_addr[2]=0x00;
			TRAN_info1.dest_addr[3]=0x00;

			TRAN_info1.source_dev_num= (DTN_TER_RIFD<<8|DTN_TER_RIFD>>8);

			TRAN_info1.source_addr[0]=(u8)MCU_UID[0];
			TRAN_info1.source_addr[1]=(u8)(MCU_UID[0]>>8);
			TRAN_info1.source_addr[2]=(u8)(MCU_UID[0]>>16);
			TRAN_info1.source_addr[3]=(u8)(MCU_UID[0]>>24);

			TRAN_info1.TYPE_NUM=4;
			TRAN_info1.data_len= 8+2;

			dat[0]=00;
			dat[1]=07; 

			dat[2]=00;
			dat[3]=00; 
			dat[4]=00; 
			dat[5]=00;
			memcpy(dat+6 , (u8*)ISO14443A_Card.UID,4);		 

			RS485_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   ); 

		}break;

		case TRACK_NFCTYPE3:
		{
			key_tab[2]=0;//解除锁定
			LED_DAT[1]&=~L_R1C3; 

			write_595_LED( LED_DAT,2);

			TRAN_info1.dest_dev_num= (DTN_android<<8|DTN_android>>8);

			TRAN_info1.dest_addr[0]=0x00;
			TRAN_info1.dest_addr[1]=0x00;
			TRAN_info1.dest_addr[2]=0x00;
			TRAN_info1.dest_addr[3]=0x00;

			TRAN_info1.source_dev_num= (DTN_TER_RIFD<<8|DTN_TER_RIFD>>8);

			TRAN_info1.source_addr[0]=(u8)MCU_UID[0];
			TRAN_info1.source_addr[1]=(u8)(MCU_UID[0]>>8);
			TRAN_info1.source_addr[2]=(u8)(MCU_UID[0]>>16);
			TRAN_info1.source_addr[3]=(u8)(MCU_UID[0]>>24);

			TRAN_info1.TYPE_NUM=4;
			TRAN_info1.data_len= 8+2;

			dat[0]=00;
			dat[1]=07; 
			memcpy(dat+2 , (u8*)FELICA_Card.UID,8);		 

			RS485_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   ); 

		}break;

		case TRACK_NFCTYPE4A:
		{
			key_tab[2]=0;//解除锁定
			LED_DAT[1]&=~L_R1C3; 

			write_595_LED( LED_DAT,2);

			TRAN_info1.dest_dev_num= (DTN_android<<8|DTN_android>>8);

			TRAN_info1.dest_addr[0]=0x00;
			TRAN_info1.dest_addr[1]=0x00;
			TRAN_info1.dest_addr[2]=0x00;
			TRAN_info1.dest_addr[3]=0x00;

			TRAN_info1.source_dev_num= (DTN_TER_RIFD<<8|DTN_TER_RIFD>>8);

			TRAN_info1.source_addr[0]=(u8)MCU_UID[0];
			TRAN_info1.source_addr[1]=(u8)(MCU_UID[0]>>8);
			TRAN_info1.source_addr[2]=(u8)(MCU_UID[0]>>16);
			TRAN_info1.source_addr[3]=(u8)(MCU_UID[0]>>24);

			TRAN_info1.TYPE_NUM=4;
			TRAN_info1.data_len= 8+2;

			dat[0]=00;
			dat[1]=07; 

			dat[2]=00;
			dat[3]=00; 
			dat[4]=00; 
			dat[5]=00;

			memcpy(dat+6 , (u8*)ISO14443A_Card.UID,4);		 

			RS485_SD_load_buf( 0xAAAA,0xBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   ); 

		}break;

		case TRACK_NFCTYPE4B:
		{
			key_tab[2]=0;//解除锁定
			LED_DAT[1]&=~L_R1C3; 

			write_595_LED( LED_DAT,2);

			TRAN_info1.dest_dev_num= (DTN_android<<8|DTN_android>>8);

			TRAN_info1.dest_addr[0]=0x00;
			TRAN_info1.dest_addr[1]=0x00;
			TRAN_info1.dest_addr[2]=0x00;
			TRAN_info1.dest_addr[3]=0x00;

			TRAN_info1.source_dev_num= (DTN_TER_RIFD<<8|DTN_TER_RIFD>>8);

			TRAN_info1.source_addr[0]=(u8)MCU_UID[0];
			TRAN_info1.source_addr[1]=(u8)(MCU_UID[0]>>8);
			TRAN_info1.source_addr[2]=(u8)(MCU_UID[0]>>16);
			TRAN_info1.source_addr[3]=(u8)(MCU_UID[0]>>24);

			TRAN_info1.TYPE_NUM=4;
			TRAN_info1.data_len= 8+2;

			dat[0]=00;
			dat[1]=07; 

			dat[2]=00;
			dat[3]=00; 
			dat[4]=00; 
			dat[5]=00;

			memcpy(dat+6 , (u8*)ISO14443B_Card.PUPI,4); 

			RS485_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len ); 

		}break;

		case TRACK_NFCTYPE5:
		{
			key_tab[2]=0;//解除锁定
			LED_DAT[1]&=~L_R1C3; 

			write_595_LED( LED_DAT,2);

			TRAN_info1.dest_dev_num= (DTN_android<<8|DTN_android>>8);

			TRAN_info1.dest_addr[0]=0x00;
			TRAN_info1.dest_addr[1]=0x00;
			TRAN_info1.dest_addr[2]=0x00;
			TRAN_info1.dest_addr[3]=0x00;

			TRAN_info1.source_dev_num= (DTN_TER_RIFD<<8|DTN_TER_RIFD>>8);

			TRAN_info1.source_addr[0]=(u8)MCU_UID[0];
			TRAN_info1.source_addr[1]=(u8)(MCU_UID[0]>>8);
			TRAN_info1.source_addr[2]=(u8)(MCU_UID[0]>>16);
			TRAN_info1.source_addr[3]=(u8)(MCU_UID[0]>>24);

			TRAN_info1.TYPE_NUM=4;
			TRAN_info1.data_len= 8+2;

			dat[0]=00;
			dat[1]=07; 
			memcpy(dat+2 , (u8*)TagUID ,8); 

			RS485_SD_load_buf( 0xAAAA,0xBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len ); 

		} break;

		default: break;
		}
	}
}

 

