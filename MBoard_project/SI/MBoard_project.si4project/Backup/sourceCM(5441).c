#include "sourceCM.h"    //���ܵ���������̺�����

extern ARM_DRIVER_USART Driver_USART1;		//�豸�����⴮��һ�豸����
extern osThreadId tid_USARTDebug_Thread;

//extern datsTransCMD_FLAG1;
	
//Ĭ��RS485 9600B

osThreadId tid_sourceCM_Thread;
osThreadDef(sourceCM_Thread,osPriorityNormal,1,512);
			 
osPoolId  sourceCM_pool;								 
osPoolDef(sourceCM_pool, 10, sourceCM_MEAS);                  // �ڴ�ض���
osMessageQId  MsgBox_sourceCM;
osMessageQDef(MsgBox_sourceCM, 2, &sourceCM_MEAS);            // ��Ϣ���ж��壬����ģ���߳�������ͨѶ�߳�
osMessageQId  MsgBox_MTsourceCM;
osMessageQDef(MsgBox_MTsourceCM, 2, &sourceCM_MEAS);          // ��Ϣ���ж���,��������ͨѶ�߳���ģ���߳�
osMessageQId  MsgBox_DPsourceCM;
osMessageQDef(MsgBox_DPsourceCM, 2, &sourceCM_MEAS);          // ��Ϣ���ж��壬����ģ���߳�����ʾģ���߳�

//���ջ����� 	
u8 RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
//���յ������ݳ���
u8 RS485_RX_CNT =0;   	

void sourceCM_ioInit(void){//�˿ڳ�ʼ��

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	                

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(unsigned char  *buf,unsigned char  len)
{
	
	RS485_TX_EN=1;			//����Ϊ����ģʽ
	osDelay(10);
  	Driver_USART1.Send(buf,len);	//���ݷ���
  	Driver_USART1.Send("i'm usart1 for debug log\r\n", 26);
	osDelay(100);	
	RS485_RX_CNT=0;	  
	RS485_TX_EN=0;				//����Ϊ����ģʽ	
}

void SOURCE_USARTInitCallback(uint32_t event){

	;
}

void USART1source_Init(void){

	/*Initialize the USART driver */
	Driver_USART1.Initialize(SOURCE_USARTInitCallback);
	/*Power up the USART peripheral */
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	Driver_USART1.Control(ARM_USART_MODE_ASYNCHRONOUS |
									ARM_USART_DATA_BITS_8 |
									ARM_USART_PARITY_EVEN |
									ARM_USART_STOP_BITS_1 |
							ARM_USART_FLOW_CONTROL_NONE, 9600);

	/* Enable Receiver and Transmitter lines */
	Driver_USART1.Control (ARM_USART_CONTROL_TX, 1);
	Driver_USART1.Control (ARM_USART_CONTROL_RX, 1);

	Driver_USART1.Send("i'm usart1 for debug log\r\n", 26);
}

void sourceCM_Init(void){
	USART1source_Init();
	//uart_init(9600);
	sourceCM_ioInit();
	//sourceCM_ADCInit();
}

unsigned char ADD_CHECK_8b(u8 dats [ ], u8 length)
{
	u8 loop;
	u8	result = 0;
	
	for(loop = 0;loop < length;loop ++){
	
		result += dats[loop];
	}
	return result;
}

unsigned char empty_id(unsigned char *id)
{
	char i = 0;
	for ( ; i  < 6; i++)
		{
			if(id[i] != 0xAA)
				return 0;
		}
	return 1;
}

unsigned char read_data2id(unsigned char *data,unsigned char *id)
{
	unsigned char i = 4;
	unsigned char add = 0;
	unsigned char len = data[13];
	if(data[4] != 0x68 )
				return 0;
	for( ; i  < len+13; i++)//data[4]->data[len-2]�ĺ�У��
		{
			
			add += data[i];
			
		}
	if(add == data[len+13])
	{
		for ( i = 0; i  < 6; i++)
		{
			id[i] = data[i+5];
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

void sourceCM_Thread(const void *argument){

	osEvent  evt;
    osStatus status;
	unsigned char temp_id[6] 	= {0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA };
	unsigned char read_id[12] 	= {0x68 ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0x68 ,0x13 ,0x00 ,0xDF ,0x16};
	unsigned char power_kwh[4]	= {0x33,0x33,0x34,0x33};//����ָ���λkwh,���ַ��ÿλ������Ҫ��0x33
	unsigned char power_v[4]	= {0x33,0x32,0x34,0x35};//��ѹָ���λV
	unsigned char power_a[4] 	= {0x33,0x32,0x35,0x35};//����ָ���λA

	unsigned char send_buff[24]	= {0x68 ,
									0,0,0,0,0,0,
									0x68,
									0x11,0x04,
									0x00,0x00,0x00,0x00,
									0x00,
									0x16};
	
	const bool UPLOAD_MODE = false;	//1�����ݱ仯ʱ���ϴ� 0�����ڶ�ʱ�ϴ�
	
	const uint8_t upldPeriod = 5;	//�����ϴ�����������UPLOAD_MODE = false ʱ��Ч��
	
	static char flag=0;
	uint8_t UPLDcnt = 0;
	bool UPLD_EN = false;
	
	
	const uint8_t dpPeriod = 200;
	
	static uint8_t Pcnt = 0;
	

	sourceCM_MEAS actuatorData;	//����������
	static sourceCM_MEAS Data_temp   = {1};	//��������������ͬ���ԱȻ���
	static sourceCM_MEAS Data_tempDP = {1};	//������������ʾ���ݶԱȻ���
	
	sourceCM_MEAS *mptr = NULL;
	sourceCM_MEAS *rptr = NULL;
	
	for(;;){
		
		evt = osMessageGet(MsgBox_MTsourceCM, 100);
		if (evt.status == osEventMessage){		//�ȴ���Ϣָ��
			
				rptr = evt.value.p;
				/*�Զ��屾���߳̽������ݴ���������������������������*/
				if (rptr->source_addr == datsTransCMD_FLAG1)
				{
					actuatorData.Switch	= rptr->Switch;
					//actuatorData.anaVal	= rptr->anaVal;
					flag=1;

					Data_temp.Switch = actuatorData.Switch;		//����������ͬ���������������ݻط�
					//Data_temp.anaVal = actuatorData.anaVal;
				}

				do{status = osPoolFree(sourceCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
				rptr = NULL;
				
			
				
				/*ִ�����������һ��״̬*/
				do{mptr = (sourceCM_MEAS *)osPoolCAlloc(sourceCM_pool);}while(mptr == NULL);	//�������ݴ�����Ϣ����
				mptr->source_addr = datsTransCMD_FLAG1;
				mptr->Switch = actuatorData.Switch;
				mptr->anaVal = actuatorData.anaVal;
				osMessagePut(MsgBox_sourceCM, (uint32_t)mptr, 50);
		}	
		
		actuatorData.anaVal = 4;
		
		if(!UPLOAD_MODE){	//ѡ���ϴ�����ģʽ
		
				if(UPLDcnt < upldPeriod)UPLDcnt ++;
				else{
				
					UPLDcnt = 0;
					UPLD_EN = true;
				}
		}
		else
		{
		
			if(Data_temp.power_kwh != actuatorData.power_kwh){	
			
					Data_temp.power_kwh = actuatorData.power_kwh;
					UPLD_EN = true;
				}
		}
		
		if(UPLD_EN){
			
				UPLD_EN = false;
				
				do{mptr = (sourceCM_MEAS *)osPoolCAlloc(sourceCM_pool);}while(mptr == NULL);	//�������ݴ�����Ϣ����
				if(flag==1){mptr->source_addr = datsTransCMD_FLAG1;}
				mptr->power_kwh = actuatorData.power_kwh;
				mptr->power_A = actuatorData.power_A;
				mptr->power_v = actuatorData.power_v;
				osMessagePut(MsgBox_sourceCM, (uint32_t)mptr, 100);
		}
		
		if(Data_tempDP.power_kwh != actuatorData.power_kwh 
			|| Data_tempDP.power_A != actuatorData.power_A 
			|| Data_tempDP.power_v != actuatorData.power_v){
		
				Data_tempDP.power_kwh	= actuatorData.power_kwh; 
				Data_tempDP.power_A		= actuatorData.power_A ;
				Data_tempDP.power_v 	= actuatorData.power_v;
		   
				do{mptr = (sourceCM_MEAS *)osPoolCAlloc(sourceCM_pool);}while(mptr == NULL);	//1.44��Һ����ʾ��Ϣ����
				if(flag==1){mptr->source_addr = datsTransCMD_FLAG1;}
				mptr->power_kwh	= actuatorData.power_kwh; 
				mptr->power_A	= actuatorData.power_A ;
				mptr->power_v 	= actuatorData.power_v;
				osMessagePut(MsgBox_DPsourceCM, (uint32_t)mptr, 100);
				osDelay(10);
		   }
	
		if(Pcnt < dpPeriod)
			{
				osDelay(10);Pcnt ++;
				
		}
		else{
		
				Pcnt = 0;
				
				osDelay(20);
		}
		

		//δ��ȡID

		if( empty_id(temp_id) == 1  )
		{
			RS485_Send_Data(read_id, 12);
			delay_ms(100);
			Driver_USART1.Receive(RS485_RX_BUF,20);		
			osDelay(500);
			RS485_Send_Data(RS485_RX_BUF, 20);
			RS485_RX_CNT =  RS485_RX_BUF[9+4]+12+4;
			if(RS485_RX_CNT != 0)
			{
				read_data2id(RS485_RX_BUF, temp_id);
				RS485_RX_CNT = 0;
			}

			memcpy(send_buff+1, temp_id , 6);//��ID�����ͻ���
		}
		else//�Ի�ȡID
		{

			
			//��ȡ����
			memcpy(send_buff+10, power_kwh , 4);
			send_buff[14] = ADD_CHECK_8b(send_buff,15);
			RS485_Send_Data(read_id, 12);
			delay_ms(10);
			Driver_USART1.Receive(RS485_RX_BUF,16+8);		
			osDelay(500);
			RS485_RX_CNT = RS485_RX_BUF[9+4]+12+4;
			if(RS485_RX_BUF[RS485_RX_CNT - 2] == ADD_CHECK_8b(RS485_RX_BUF+4,RS485_RX_CNT - 2 -4))
			{
				actuatorData.power_kwh = 
					((RS485_RX_BUF[18]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f) + 
					((RS485_RX_BUF[19]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f)*10 +
					((RS485_RX_BUF[20]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f)*100+
					((RS485_RX_BUF[21]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f)*1000;
				RS485_RX_CNT = 0;
				
			}
			//��ȡ��ѹ
			memcpy(send_buff+10, power_v , 4);
			send_buff[14] = ADD_CHECK_8b(send_buff,15);
			RS485_Send_Data(send_buff, 16);
			delay_ms(10);
			Driver_USART1.Receive(RS485_RX_BUF,16+10);		
			osDelay(500);
			RS485_RX_CNT =  RS485_RX_BUF[9+4]+12+4;
			if( RS485_RX_BUF[RS485_RX_CNT - 2] == ADD_CHECK_8b(RS485_RX_BUF+4,RS485_RX_CNT - 2 -4))
			{
				actuatorData.power_v = 
					((RS485_RX_BUF[18]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f) + 
					((RS485_RX_BUF[19]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f)*10 +
					((RS485_RX_BUF[20]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f)*100+
					((RS485_RX_BUF[21]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f)*1000+
					((RS485_RX_BUF[22]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f)*1000+
					((RS485_RX_BUF[23]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f)*10000;
			}
			//��ȡ����
			memcpy(send_buff+10, power_a , 4);
			send_buff[14] = ADD_CHECK_8b(send_buff,15);
			RS485_Send_Data(send_buff, 16);
			delay_ms(10);
			Driver_USART1.Receive(RS485_RX_BUF,16+13);		
			osDelay(500);
			RS485_RX_CNT =  RS485_RX_BUF[9+4]+12+4;
			if(RS485_RX_BUF[RS485_RX_CNT - 2] == ADD_CHECK_8b(RS485_RX_BUF+4,RS485_RX_CNT - 2 -4))
			{
				actuatorData.power_A = 
					((RS485_RX_BUF[18]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f) + 
					((RS485_RX_BUF[19]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f)*10 +
					((RS485_RX_BUF[20]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f)*100+
					((RS485_RX_BUF[21]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f)*1000+
					((RS485_RX_BUF[22]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f)*1000+
					((RS485_RX_BUF[23]-0x33)>>4 *10 + (RS485_RX_BUF[18]-0x33)&0x0f)*10000;
				RS485_RX_CNT = 0;
			}
		}

		delay_ms(200);


	}
}

void sourceCMThread_Active(void){

	static bool memInit_flg = false;
	osThreadTerminate(tid_USARTDebug_Thread);   //�ر�debug���������軥��
	if(!memInit_flg){
	
		sourceCM_pool   = osPoolCreate(osPool(sourceCM_pool));	//�����ڴ��
		MsgBox_sourceCM = osMessageCreate(osMessageQ(MsgBox_sourceCM), NULL);   //������Ϣ����
		MsgBox_MTsourceCM = osMessageCreate(osMessageQ(MsgBox_MTsourceCM), NULL);//������Ϣ����
		MsgBox_DPsourceCM = osMessageCreate(osMessageQ(MsgBox_DPsourceCM), NULL);//������Ϣ����
		
		memInit_flg = true;
	}

	sourceCM_Init();
	tid_sourceCM_Thread = osThreadCreate(osThread(sourceCM_Thread),NULL);
}