/**
  ******************************************************************************
  * @file    main.c 
  * @author  MMY Application Team
  * @version V1.0.0
  * @date    03/21/2013
  * @brief   Main program body
  ******************************************************************************
  * @copyright
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "prtApp_SH95.h"

extern ISO14443B_CARD 	ISO14443B_Card;
extern ISO14443A_CARD 	ISO14443A_Card;
extern uint8_t TagUID[16];
extern FELICA_CARD 	FELICA_Card;

extern ARM_DRIVER_USART Driver_USART3;

osThreadId tid_devApp95HF_Thread;
osThreadDef(devApp95HF_Thread,osPriorityNormal,	1,	2048);

/** @addtogroup User_Appli
 * 	@{
 *  @brief      <b>This folder contains the application files</b> 
 */
 
/** @addtogroup Main
 * 	@{
 * @brief      This file contains the entry point of this demonstration firmeware
 */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static __IO uint32_t TimingDelay = 0;

/* Variables for the different modes */
extern DeviceMode_t devicemode;
extern TagType_t nfc_tagtype;

/**
* @brief  buffer to exchange data with the RF tranceiver.
*/
extern uint8_t u95HFBuffer [RFTRANS_95HF_MAX_BUFFER_SIZE+3];

__IO uint32_t SELStatus = 0;

bool USB_Control_Allowed = false;

extern int8_t HIDTransaction;
/* bmp saved in well defined area, initialize pointer on this bmp file */
/* adress is forced in header file */
  
 /* Private function prototypes -----------------------------------------------*/
 
 
/** @addtogroup Main_Private_Functions
 * 	@{
 */

/**
  * @}
  */


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

/** @addtogroup Main_Public_Functions
 * 	@{
 */

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */


void dev95HF_Init(void){
	
/* -- Configures Main system clocks & power */
	RCC_APB2PeriphClockCmd(USB_DISCONNECT_GPIO_CLOCK, ENABLE);
	
	RCC_APB2PeriphClockCmd(INTERFACE_GPIO_CLK, ENABLE);
	  
/*------------------- Resources Initialization -----------------------------*/
	
	/* configure the interuptions  */
	Interrupts_Config();

/*----CD--------------- Drivers Initialization -------------------------------*/
	//USART3_dev95HF_Init();

		Driver_USART3.Send("step 1 complete.\r\n", 18);
		osDelay(20);

  /* Led Configuration */
	LED_Config(LED1);	
	delay_ms(10);
		
	/* ST95HF HW Init */
	ConfigManager_HWInit();
	
		Driver_USART3.Send("step 2 complete.\r\n", 18);
		osDelay(20);
    
	LED_On(LED1);
	delay_ms(100);
	LED_Off(LED1);
	
	Driver_USART3.Send("RFID_95HF Initialize completed.\r\n", 33);
}
  
void prtApp_95HF(void)
{
	uint8_t TagType = TRACK_NOTHING, tagfounds = TRACK_ALL;
	
	u8 loop;
	
	u8 CBUF_CNT = 0;
	const u8 CBUF_CNT_period = 3;
	
	const char dispLength = 30;
	char disp[dispLength];	
	
	const char idLength = 20;
	char UID[idLength] = {' '};
	char InvertedUID[idLength]  = {' '};
	char LastUIDFound[idLength] = {' '};

	for(;;){
		
		TagType = ConfigManager_TagHunting(tagfounds);
		// Turn on the LED if a tag was founded
		/* Check the tag type found */
		
		if(TagType == TRACK_NOTHING){
		
			LED_Off(LED1);
			
			if(CBUF_CNT < CBUF_CNT_period)CBUF_CNT ++;
			else{
			
				memset(LastUIDFound,0,idLength * sizeof(char));
			}
		}else{
		
			LED_On(LED1);
			memset(InvertedUID,0,idLength * sizeof(char));
			memset(disp,0,dispLength * sizeof(char));
			
			CBUF_CNT = 0;
			
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
							Driver_USART3.Send((uint8_t*)disp,strlen((const char*)disp));			
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
							Driver_USART3.Send((uint8_t*)disp,strlen((const char*)disp));		
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
							Driver_USART3.Send((uint8_t*)disp,strlen((const char*)disp));				
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
							Driver_USART3.Send((uint8_t*)disp,strlen((const char*)disp));		
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
							Driver_USART3.Send((uint8_t*)disp,strlen((const char*)disp));	
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
							Driver_USART3.Send((uint8_t*)disp,strlen((const char*)disp));	
						}
						else{
							
							;
						}
					}break;
				
				default:LED_Off(LED1);break;
			}
		}

		delay_ms(20); 
	}
}

void devApp95HF_Thread(const void *argument){

	prtApp_95HF();
}

void devApp95HF_ThreadActive(void){

	dev95HF_Init();
	tid_devApp95HF_Thread = osThreadCreate(osThread(devApp95HF_Thread),NULL);
}


