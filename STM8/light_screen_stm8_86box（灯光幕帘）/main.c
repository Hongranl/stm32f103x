/**
  ******************************************************************************
  * @file    GPIO_LED\main.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    25-February-2011
  * @brief   This file contains the main function for GPIO  example.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  * 操作
  * 入网退网：按中间键5S以上
  *        
  * 灯光控制
  *     
  * LED        左   中    右
  * 
  * 停止       红   蓝    红
  * 上升       蓝   红    红
  * 下降       红   红    蓝
  * 入网       中间蓝灯闪5次
  * 退网       左右蓝灯闪5次
  * 入网成功   所有红灯闪5次
  * 退网成功   所有蓝灯闪5次
  */ 
    

    
/* Includes ------------------------------------------------------------------*/

#include "ALL_Includes.h"
    
//定义CPU内部时钟
#define  SYS_CLOCK        16
#define  TIM4_PERIOD      124


u8 Data_Len = 0;

u8 uart_rx_flg = 0;

u8 curtain_status = 2;

u16 motor_ck_time = 0;

u16 LED_time = 0;

u16 alarm_time = 0;

u8 join_flg = 0;  //入网状态
 
uint32_t ad_ck_0_num = 0;

uint32_t MCU_UID[3];
 
u8 key_status = 2;
 
u8 sampling_in_flg = 0;   
    
u16 UART_NO_DAT_TIME = 0;
    
u16 sampling_time = 0;
u16 sampling_status = 0;
 
u8 program_num = 0;
 
u8 LIGHT_1_STATUS = 0;  
u8 LIGHT_2_STATUS = 0;  
u8 LIGHT_3_STATUS = 0; 

//
u8 maxcnt =0;

TRAN_D_struct TRAN_info1;


void Delay (uint16_t nCount);
 
void All_Config(void);
void IO_Init(void);
 
void TIM4_Config(void);

void jiance(void);
void jiance_light_key(void);

void  JDQ_WINDOW_PAUSE(void);
void  JDQ_WINDOW_UP(void);
void  JDQ_WINDOW_DOWN(void);    
    
u8  chcek_run_status(u8); 
u16 get_adc(u8 chanel,u8 times);

u8 chcek_status_pre = 0;

/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/



/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{ 
  All_Config();

  TRAN_info1.dest_dev_num = 0;          //目标设备号

  TRAN_info1.dest_addr[0] = 0x00;       //目标地址
  TRAN_info1.dest_addr[1] = 0x00;
  TRAN_info1.dest_addr[2] = 0x00;
  TRAN_info1.dest_addr[3] = 0x00;

  TRAN_info1.source_addr[0] = (u8) MCU_UID[0];          //源地址 
  TRAN_info1.source_addr[1] = (u8)(MCU_UID[0]>>8);
  TRAN_info1.source_addr[2] = (u8)(MCU_UID[0]>>16);
  TRAN_info1.source_addr[3] = (u8)(MCU_UID[0]>>24); 

  TRAN_info1.TYPE_NUM = 3    ;   //数据格式号: 第三套数据格式  用于ZIGBEE（非标准）  设备端 
  
    
  while(1)
  {  
    if( program_num == 1 )
    {
      jiance(); 
    }
    else if( program_num == 2 || program_num == 3 )
    {
      jiance_light_key();
    }

    Rs485_COMM_uart_fuc();   
  }
}

void jiance_light_key(void)
{
	  u8 i = 0, dat[40];

	  dat[0] = 0; 
	  dat[2] = 0; //

	  dat[4] = (u8) MCU_UID[0];
	  dat[5] = (u8)( MCU_UID[0] >> 8 );
	  dat[6] = (u8)( MCU_UID[0] >> 16 );
	  dat[7] = (u8)( MCU_UID[0] >> 24 ); 

	  memset(  dat+8,0,8 ); 

	  if( LIGHT_1_STATUS == 0 )
	  {
		    KEY_LEFT_BLUE(1); 
		    //KEY_LEFT_RED(0);
		    KEY_LEFT_RED(0);  
		    //KEY_LEFT_BLUE(1);
	  }
	  else 
	  {
		    KEY_LEFT_BLUE(0); 
		    KEY_LEFT_RED(1);  
	  }
	  
	  if( program_num != 2 )
	  {
		    if( LIGHT_2_STATUS == 0 )
		    {
			      KEY_MID_BLUE(1); 
			      KEY_MID_RED(0); 
		    }
		    else 
		    {
			      KEY_MID_BLUE(0); 
			      KEY_MID_RED(1); 
		    }
	  }
	  else
	  {
		    KEY_MID_BLUE(0); 
		    KEY_MID_RED(0); 
	  }

	  if( LIGHT_3_STATUS == 0 )
	  {
		    KEY_RIGHT_BLUE(1); 
		    KEY_RIGHT_RED(0); 
		  }
	  else 
	  {
		    KEY_RIGHT_BLUE(0); 
		    KEY_RIGHT_RED(1);  
	  }
        
 /////////////////////////////////       
  if( KEY_1L == 0 )
  {  
	    alarm_time = 0;
	    while( KEY_1L == 0)
	    {
		      if(  KEY_3L == 0  )
		      {
		        //判断时间//来执行入网 退网
		        if( alarm_time >= 3500 )
		        {   
			          //长按5S后，触发一次入网或退网

			          TRAN_info1.data_len = 2 ;

			          dat[0] = 0; 
				      if( join_flg == 0 )  //如果长按5S  入网开关开，中间的灯闪5次
				          {   
					            join_flg = 1;
					            dat[1] = join_sw; // //入网开关
					            KEY_MID_BLUE(0); 
					            KEY_MID_RED(0); 
					            
					            for(i=0;i<5;i++)
					            { 
						              KEY_LEFT_BLUE(1); 
						              KEY_LEFT_RED(0); 
						              
						              KEY_RIGHT_BLUE(0); 
						              KEY_RIGHT_RED(0); 

						              LED_time = 300;
						              while( LED_time != 0 ); 
						              
						              KEY_LEFT_BLUE(0); 
						              KEY_LEFT_RED(1); 
						              
						              KEY_RIGHT_BLUE(1); 
						              KEY_RIGHT_RED(0); 
						              
						              LED_time = 300;
						              while( LED_time != 0 );
					            }
			          }
			          else    //如果长按5S  入网开关关，左右两边的灯闪5次
			          {  
				            join_flg = 0;
				            dat[1] = leave_net; //

				            KEY_MID_BLUE(0); 
				            KEY_MID_RED(0);
				            
				            for(i=0;i<5;i++)
				            { 
					              KEY_LEFT_BLUE(1); 
					              KEY_LEFT_RED(1); 
					              
					              KEY_RIGHT_BLUE(1); 
					              KEY_RIGHT_RED(1); 
					              
					              
					              LED_time = 300;
					              while( LED_time != 0 ); 

					              KEY_LEFT_BLUE(0); 
					              KEY_LEFT_RED(0); 
					              
					              KEY_RIGHT_BLUE(0); 
					              KEY_RIGHT_RED(0);
					              

					              LED_time = 300;
					              while( LED_time != 0 );
				            }
				            }
				      Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );  
				      return;
				      }
		      }
		    }
	                           
	    if( LIGHT_1_STATUS != 0 )
	    {
		      LIGHT_1_STATUS = 0;
		      
		      KEY_LEFT_BLUE(1); 
		      KEY_LEFT_RED(0);  

		      RLY1_OUT(RLY_OFF) ;
	    }
	    else 
	    {  
		      LIGHT_1_STATUS = 1;
		      
		      KEY_LEFT_BLUE(0); 
		      KEY_LEFT_RED(1); 
		      
		      RLY1_OUT(RLY_ON) ;
	    }

	    dat[1] = upload_info;                

	    if( program_num == 2)
	    {

		      TRAN_info1.source_dev_num = ( DTN_86_LIGHT_2 << 8 | DTN_86_LIGHT_2 >> 8 );

		      TRAN_info1.data_len = 16 + 3  ;

		      dat[3] = DTN_86_LIGHT_2; // 

		      dat[8+8] = 2; //1 2 3  表示路数

		      dat[8+8+1] = LIGHT_1_STATUS;  
		      dat[8+8+2] = LIGHT_3_STATUS;  

	    } 
	    if( program_num == 3)
	    {

		      TRAN_info1.source_dev_num = ( DTN_86_LIGHT_3 << 8 | DTN_86_LIGHT_3 >> 8 );

		      TRAN_info1.data_len = 16 + 4  ;

		      dat[3] = DTN_86_LIGHT_3; // 

		      dat[8+8] = 3; //1 2 3  表示路数

		      dat[8+8+1] = LIGHT_1_STATUS;  
		      dat[8+8+2] = LIGHT_2_STATUS;  
		      dat[8+8+3] = LIGHT_3_STATUS; 
	    } 

	    Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );	
  }		 
            
  if( KEY_2L == 0 && program_num == 3)
  { 
	    while( KEY_2L == 0);

	    if( LIGHT_2_STATUS != 0 )
	    {  
		      LIGHT_2_STATUS = 0;
		      
		      KEY_MID_BLUE(1); 
		      KEY_MID_RED(0);   
		      
		      RLY2_OUT(RLY_OFF);
	    }
	    else 
	    {  
		      LIGHT_2_STATUS = 1;
		      
		      KEY_MID_BLUE(0); 
		      KEY_MID_RED(1);
		      
		      RLY2_OUT(RLY_ON);
	    }
	    dat[1] = upload_info;    

	    TRAN_info1.source_dev_num = ( DTN_86_LIGHT_3 << 8 | DTN_86_LIGHT_3 >> 8 );
	    TRAN_info1.data_len = 16 + 4  ;

	    dat[3] = DTN_86_LIGHT_3; // 

	    dat[8+8] = 3; //1 2 3  表示路数

	    dat[8+8+1] = LIGHT_1_STATUS;  
	    dat[8+8+2] = LIGHT_2_STATUS;  
	    dat[8+8+3] = LIGHT_3_STATUS; 

	    Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );	
  }
	
  if( KEY_3L == 0 )
  {
	    while( KEY_3L == 0 )
	    {
		      if( KEY_1L == 0 )
		      {
		        	//判断时间  来执行入网 退网

			        if( alarm_time >= 3500 )
			        {   

				          //长按5S后，触发一次入网或退网

				          TRAN_info1.data_len = 2;

				          dat[0] = 0; 

				          if( join_flg == 0 )  //如果长按5S  入网开关开，中间的灯闪5次
				          {   
					            join_flg = 1;

					            dat[1] = join_sw; //入网 

					            KEY_MID_BLUE(0); 
					            KEY_MID_RED(0);
					            
					            for(i=0;i<5;i++)
					            { 
						              KEY_LEFT_BLUE(1); 
						              KEY_LEFT_RED(0); 
						              
						              KEY_RIGHT_BLUE(1); 
						              KEY_RIGHT_RED(0); 

						              LED_time = 300;
						              while( LED_time != 0 ); 
						              
						              KEY_LEFT_BLUE(0); 
						              KEY_LEFT_RED(1); 
						              
						              KEY_RIGHT_BLUE(0); 
						              KEY_RIGHT_RED(1); 
						              
						              LED_time = 300;
						              while( LED_time != 0 );
				            	}  
				          }
				          else    //如果长按5S  入网开关关，左右两边的灯闪5次
				          {   
					            join_flg = 0;
					            dat[1] = leave_net; //退网

					            KEY_MID_BLUE(1); 
					            KEY_MID_RED(1);
					            
					            for(i=0;i<5;i++)
					            { 
						              KEY_LEFT_BLUE(1); 
						              KEY_LEFT_RED(1); 
						              
						              KEY_RIGHT_BLUE(1); 
						              KEY_RIGHT_RED(1);  
						              
						              LED_time = 300;
						              while( LED_time != 0 ); 

						              KEY_LEFT_BLUE(0); 
						              KEY_LEFT_RED(0); 
						              
						              KEY_RIGHT_BLUE(0); 
						              KEY_RIGHT_RED(0); 

						              LED_time = 300;
						              while( LED_time != 0 );
					            }
				          }
				          Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );  
				          return; 
			        }  
		      }  
	    }        
	               
	    if( LIGHT_3_STATUS != 0 )
	    {  
		      LIGHT_3_STATUS = 0;
		      
		      KEY_RIGHT_BLUE(1); 
		      KEY_RIGHT_RED(0);   //右边蓝灯亮
		      
		      RLY3_OUT(RLY_OFF);
	    }
	    else 
	    {  
		      LIGHT_3_STATUS = 1;
		      
		      KEY_RIGHT_BLUE(0); 
		      KEY_RIGHT_RED(1);  //右边红灯亮
		      
		      RLY3_OUT(RLY_ON);
	    } 
	    
	    dat[1] = upload_info;  

	    if(program_num == 2)
	    {
		      TRAN_info1.source_dev_num = ( DTN_86_LIGHT_2 << 8 | DTN_86_LIGHT_2 >> 8 );
		      TRAN_info1.data_len = 16 + 3  ;

		      dat[3] = DTN_86_LIGHT_2; // 

		      dat[8+8] = 2; //1 2 3  表示路数

		      dat[8+8+1] = LIGHT_1_STATUS;  
		      dat[8+8+2] = LIGHT_3_STATUS;  
	    } 
	    if( program_num == 3 )
	    {
		      TRAN_info1.source_dev_num = ( DTN_86_LIGHT_3 << 8 | DTN_86_LIGHT_3 >> 8 );
		      TRAN_info1.data_len = 16 + 4;

		      dat[3] = DTN_86_LIGHT_3; // 

		      dat[8+8] = 3; //1 2 3  表示路数

		      dat[8+8+1] = LIGHT_1_STATUS;  
		      dat[8+8+2] = LIGHT_2_STATUS;  
		      dat[8+8+3] = LIGHT_3_STATUS; 
	    } 
	  
	  	Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );	  
  } 
}


/**
  * @brief  Configure TIM4 to generate an update interrupt each 1ms 
  * @param  None
  * @retval None
  */
 void TIM4_Config(void)
{

}


void All_Config(void)
{
 
  CLK->CKDIVR &= ~( BIT(4) | BIT(3) );


  IO_Init();  
  UART1_DeInit();
  UART1_Init( (u32)115200,UART1_WORDLENGTH_8D,UART1_STOPBITS_1,UART1_PARITY_NO,UART1_SYNCMODE_CLOCK_DISABLE,UART1_MODE_TXRX_ENABLE );
  UART1_ITConfig( UART1_IT_RXNE_OR, ENABLE );
  UART1_Cmd( ENABLE );

  //TIM4_Config( );       //加一个定时器
  /* TIM4 configuration:
  - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
  clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
  max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
  min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
  so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */



  TIM4->PSCR = (uint8_t)(TIM4_PRESCALER_128);
  /* Set the Autoreload value */
  TIM4->ARR = (uint8_t)(TIM4_PERIOD);
  TIM4->SR1 = (uint8_t)(~TIM4_FLAG_UPDATE);
  TIM4->IER |= (uint8_t)TIM4_IT_UPDATE;
  TIM4->CR1 |= TIM4_CR1_CEN;

  /* Time base configuration */
  // TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_PERIOD);
  /* Clear TIM4 update flag */
  // TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  //  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

  /* enable interrupts */
  //enableInterrupts();

  /* Enable TIM4 */
  //TIM4_Cmd(ENABLE);

  enableInterrupts();
}


void  JDQ_WINDOW_UP(void)
{  
  RLY1_OUT(RLY_OFF);   
  RLY2_OUT(RLY_ON);    
}

void  JDQ_WINDOW_DOWN(void)
{   
  RLY1_OUT(RLY_ON); 
  RLY2_OUT(RLY_OFF);    
}

void  JDQ_WINDOW_PAUSE(void)
{ 
  RLY1_OUT(RLY_OFF);
  RLY2_OUT(RLY_OFF);    
}


void jiance(void)
{
	  u8 i = 0;
	  u8 re = 0;
	  u8 dat[50];

	  dat[0] = 0; 
	  dat[1] = upload_info; //

	  dat[2] = 0;  // 
	  dat[3] = 51; //   设备类型号 可以去掉
	  
	  dat[4] = (u8) MCU_UID[0];     
	  dat[5] = (u8)( MCU_UID[0] >> 8 );
	  dat[6] = (u8)( MCU_UID[0] >> 16 );
	  dat[7] = (u8)( MCU_UID[0] >> 24 ); 

	  memset( dat + 8,0,8 );

	  TRAN_info1.source_dev_num = ( DTN_curtain << 8 | DTN_curtain >> 8 );
		

		//check run status
	
	  re =chcek_run_status(curtain_status);
		
			

		if( re == 2 )
		{
			      if(curtain_status != 2)
			      {
				        TRAN_info1.data_len = 17 ;
				        
				        dat[1] = upload_info; //
				        dat[2] = 0;  // 
				        dat[3] = DTN_curtain; //   设备号

				        dat[8+8] = 2; // 1个字节，1上 0下，2暂停
				        
				        curtain_status = 2;
				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );  //发送命令
			        	// delay_ms(1000);
			      }
			           
			      LED_PAUSE();

			      JDQ_WINDOW_PAUSE();
				key_status = 2;
				
			      if( curtain_status != 2 )
			      {
				        TRAN_info1.data_len = 17   ;
				        dat[8+8] = 2; //1个字节，1上 0下，2暂停
				        curtain_status = 2;
				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len ); 
				        // delay_ms(1000);
			      }
		}

	  
//	  if( SAMPING_PD2_Pin_RD != 0 )  //如果超过50MS  说明是停止运行的
//	  { 
//		    if( sampling_time > 300 ) 
//		    {   
//			      sampling_time = 300;   
//			      sampling_in_flg = 0;  //停止  
//		    } 
//	  }
//	  else
//	  {
//		    sampling_time = 0;
//		    sampling_in_flg = 1;
//		    motor_ck_time = 500;
//	  }  
//
//	  if( motor_ck_time == 0 )
//	  {   
//		    if( sampling_in_flg == 0 )
//		    {
//			      if(curtain_status != 2)
//			      {
//				        TRAN_info1.data_len = 17 ;
//				        
//				        dat[1] = upload_info; //
//				        dat[2] = 0;  // 
//				        dat[3] = DTN_curtain; //   设备号
//
//				        dat[8+8] = 2; // 1个字节，1上 0下，2暂停
//				        
//				        curtain_status = 2;
//				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );  //发送命令
//			        	// delay_ms(1000);
//			      }
//			           
//			      LED_PAUSE();
//
//			      JDQ_WINDOW_PAUSE();
//
//			      if( curtain_status != 2 )
//			      {
//				        TRAN_info1.data_len = 17   ;
//				        dat[8+8] = 2; //1个字节，1上 0下，2暂停
//				        curtain_status = 2;
//				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len ); 
//				        // delay_ms(1000);
//			      }
//		    }
//	  }              
//	        
	  if( DOWN_IN == 0 )
	  {      
		    Delay(5000);
		    
		    if( DOWN_IN == 0 )
		    {
			      alarm_time = 0;
			      motor_ck_time = 1500;
			 
			      sampling_time = 0;
			     
			      LED_DOWN();

			      JDQ_WINDOW_DOWN();

			      if( key_status != 0 )
			      {
				        TRAN_info1.data_len = 17   ;
				        
				        dat[2] = 0;  // 
				        dat[3] = DTN_curtain; //   设备号
				        
				        dat[8+8] = 0; //1个字节，1上 0下，2暂停
				        curtain_status = 0;
				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len ); 		
			      }
			      key_status = 0; //down
	    	}
	  }		 
	  else if( PAUSE_IN == 0 )
	  { 
	    	Delay(5000);

		    if( PAUSE_IN == 0 )
		    {
		      
			      LED_PAUSE();
			      
			      JDQ_WINDOW_PAUSE(); 

			      if( alarm_time >= 3500 )   //长按5S后，触发一次入网或退网
			      {   
				        TRAN_info1.data_len = 2   ;

				        dat[0] = 0; 
				        dat[2] = 0;  // 
				        dat[3] = DTN_curtain; //   设备号

				        if( join_flg == 0 )  //如果长按5S  入网开关开，中间的灯闪5次
				        {   
					          join_flg = 1;

					          dat[1] = join_sw; // 入网开关
					          
					         //LED_JOIN();
					          for(i=0;i<5;i++)
					          { 
						            KEY_MID_RED(1);
						            KEY_MID_BLUE(0);

						            LED_time = 300;
						            while( LED_time != 0 ); 

						            KEY_MID_RED(0);
						            KEY_MID_BLUE(1);

						            LED_time = 300;
						            while( LED_time != 0 );
					          }

				        }
				        else    //如果长按5S  入网开关关，左右两边的灯闪5次
				        {  
					          join_flg = 0;
					          dat[1] = leave_net; //退网

					         // LED_JOIN(join_flg);
					          
					          for(i=0;i<5;i++)
					          { 
						            KEY_LEFT_RED(0);
						            KEY_MID_RED(0);
						            KEY_RIGHT_RED(0);

						            KEY_LEFT_BLUE(1);
						            KEY_MID_BLUE(1);
						            KEY_RIGHT_BLUE(1);

						            LED_time = 300;
						            while( LED_time != 0 ); 

						            KEY_LEFT_RED(1); 
						            KEY_MID_RED(0);  
						            KEY_RIGHT_RED(1);

						            KEY_LEFT_BLUE(0);  
						            KEY_MID_BLUE(1);
						            KEY_RIGHT_BLUE(0);

						            LED_time = 300;
						            while( LED_time != 0 );
					          }
				        }

				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );  

				        alarm_time = 0;
			      }
			      else
			      {
				        if( key_status != 2 )
				        {
					          TRAN_info1.data_len = 17;
					          
					          dat[2] = 0;  // 
					          dat[3] = DTN_curtain; //   设备号
					          
					          dat[8+8] = 2; //1个字节，1上 0下，2暂停
					          
					          curtain_status = 2;
					          Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len ); 	 
				        } 
				        key_status = 2;
			      }
		    }
	  }	
	  else if( UP_IN == 0 )
	  {
		    Delay(5000);
		    if( UP_IN == 0 )
		    {
			      alarm_time = 0;
			      motor_ck_time = 1500;

			      sampling_time = 0;
			     
			      LED_UP(); //上升灯状态

			      JDQ_WINDOW_UP();

			      if(key_status != 1)
			      {
				        TRAN_info1.data_len = 17   ;
				        
				        dat[2] = 0;  // 
				        dat[3] = DTN_curtain; //   设备号
				        
				        dat[8+8] = 1; //1个字节，1上 0下，2暂停
				        curtain_status = 1;
				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len ); 	
			      }

			      key_status = 1; //up
		    }
	  }
	  else
	  {
	    	alarm_time = 0;
	  } 
 }

u8 chcek_run_status(u8 status)
{
	
	if(status != 2)
	{
		delay_ms(20);
		
		if(chcek_run != chcek_status_pre)
		{
			chcek_status_pre = chcek_run;
			delay_ms(11);
			if(chcek_run != chcek_status_pre)
			{
				maxcnt = (maxcnt < 1) ? 0:(maxcnt-1);
			}
		}
		else if(chcek_run == chcek_status_pre)
		{
				
			
			delay_ms(11);
			if(chcek_run == chcek_status_pre)
			{
				maxcnt++;
			}
			else
			{
				maxcnt = (maxcnt < 1) ? 0:(maxcnt-1);
				chcek_status_pre = chcek_run;
			}
			
			if(maxcnt >25)
			{
				maxcnt = 0;
				
				return 2;
			}
		}

		
	}
		
	return status;
}
void IO_Init(void)
{   
  uint16_t i = 0, Conversion_Value1 = 0 ;
  u8 val1,val2;

 //设置IO口为输出

  GPIOC->DDR |= RLY3_PC7_Pin;//输出模式
  GPIOC->CR1 |= RLY3_PC7_Pin;//推挽输出
   
  GPIOC->DDR |= RLY2_PC6_Pin;//输出模式
  GPIOC->CR1 |= RLY2_PC6_Pin;//推挽输出 

  GPIOA->DDR |= RLY1_PA3_Pin;//输出模式
  GPIOA->CR1 |= RLY1_PA3_Pin;//推挽输出 
   
  GPIOC->DDR &=~ KEY3_PC3_Pin;//输入模式
  GPIOC->DDR &=~ KEY1_PC4_Pin;//输入模式 
  GPIOC->DDR &=~ KEY2_PC5_Pin;//输入模式


  GPIOD->DDR &=~ SAMPING_PD2_Pin;//输入模式

  GPIOD->DDR &=~ MODE_AD_PD3_Pin;//输入模式       

   
  GPIOD->DDR |= LED1_PD4_Pin;//输出模式
  GPIOD->CR1 |= LED1_PD4_Pin;//推挽输出
   
  GPIOB->DDR |= LED5_PB4_Pin;//输出模式
  GPIOB->CR1 |= LED5_PB4_Pin;//推挽输出         

  GPIOB->DDR |= LED3_PB5_Pin;//输出模式
  GPIOB->CR1 |= LED3_PB5_Pin;//推挽输出    


  GPIOD->DDR |= LED2_SWIM_PD1_Pin;//输出模式
  GPIOD->CR1 |= LED2_SWIM_PD1_Pin;//推挽输出
   
  GPIOA->DDR |= LED4_PA1_Pin;//输出模式
  GPIOA->CR1 |= LED4_PA1_Pin;//推挽输出         

  GPIOA->DDR |= LED6_PA2_Pin;//输出模式
  GPIOA->CR1 |= LED6_PA2_Pin;//推挽输出            

   
  //串口
  GPIOD->DDR |= BIT(5);//输出模式
  GPIOD->CR1 |= BIT(5);//推挽输出
    
   //串口
  GPIOD->DDR &=~ BIT(6);//输入模式   

  RLY1_OUT(RLY_OFF) ;
  RLY2_OUT(RLY_OFF) ;    
  RLY3_OUT(RLY_OFF) ; 

  Delay(5000);
  Delay(5000);
  Delay(5000);

  delay_ms(3000);
    

  //检测是那套程序，幕帘 还是开关灯   然后初始化不同
    
 
   // ADC1->CR1  =0x00;    // fADC = fMASTER/2，复位即进入低功耗模式

  ADC1->CR2  = 0x00;    // 默认左对齐

  ADC1->CSR  = 0x04;     //4通道

  ADC1->CR1 |= 0x01;    // 开启ADC
  i = 6;
  while(i--);
  ADC1->CR1 |= 0x01;    // 从低功耗模式中唤醒ADC

  while(!(ADC1->CSR & 0x80));    // 等待转换完成

  val2 = ADC1->DRH;
  val1 = ADC1->DRL;
  Conversion_Value1 =((((unsigned int) val2 ) << 2 ) + val1 );    // 结果送入AD_Value缓冲区
  
  
  // 跟据读取到的ADC值 设置按键板的功能
  // 1：幕帘    2： 2路灯光控制  3：3路灯光控制  4：单路可调光  
  program_num = 1;//幕帘控制
  
  if( Conversion_Value1 > 180 && Conversion_Value1 < 300 )//1路的值
  {
    program_num = 2;//2路灯光控制
  }
  else if( Conversion_Value1 > 400 && Conversion_Value1 < 600 )//2路的值
  {
    program_num = 3;//3路灯光控制
  }    
  else if( Conversion_Value1 > 680  && Conversion_Value1 < 850 )//3路的值  
  {
    program_num = 4;//单路可调光
  }
     
  //设置I/O口初始化
  switch( program_num )
  {
    case 1://默认是幕帘程序
    {
      KEY_LEFT_RED(1);
      KEY_MID_RED(0);
      KEY_RIGHT_RED(1);

      KEY_LEFT_BLUE(0);
      KEY_MID_BLUE(1);
      KEY_RIGHT_BLUE(0);
    }break;
    
    case 2://2路86灯面板
    {
      KEY_LEFT_BLUE(1);    
      KEY_MID_BLUE(1);   
      KEY_RIGHT_BLUE(0);  

      KEY_LEFT_RED(0);    
      KEY_MID_RED(1);     
      KEY_RIGHT_RED(1);  
    }break; 
    
    case 3://3路86灯面板
    {
      KEY_LEFT_BLUE(1);    
      KEY_MID_BLUE(0);   
      KEY_RIGHT_BLUE(0);  

      KEY_LEFT_RED(0);    
      KEY_MID_RED(1);     
      KEY_RIGHT_RED(1);     
    }break;
    
    default: break; 
  }     
}

u16 get_adc(u8 chanel,u8 times)
{

	u16 val1,val2,i,n,ad_value;
	// ADC1->CR1  =0x00;    // fADC = fMASTER/2，复位即进入低功耗模式


	for (n = 0; n < times; ++n)
	{
			  ADC1->CR2  = 0x00;    // 默认左对齐

			  ADC1->CSR  = chanel;     //chanel通道

			  ADC1->CR1 |= 0x01;    // 开启ADC
			  i = 6;
			  while(i--);
			  ADC1->CR1 |= 0x01;    // 从低功耗模式中唤醒ADC

			  while(!(ADC1->CSR & 0x80));    // 等待转换完成

			  val2 = ADC1->DRH;
			  val1 = ADC1->DRL;

			  ad_value += ((((unsigned short) val2 ) << 2 ) + val1 );

			  delay_ms(2);
	}

	 ADC1->CR1  =0x00;    // fADC = fMASTER/2，复位即进入低功耗模式
	return ad_value/times;
	 
}

/**
  * @brief Delay
  * @param nCount
  * @retval None
  */
void Delay(uint16_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

/**
  * 毫秒延时
  *
  *
  *
  */

 void delay_ms(uint16_t nCount)
{  
  u16 i = 0, n = 0;
 
  for(i=0;i<nCount;i++)
  {  
    n = 1000;
    while ( n != 0 )
    {
      n--;
    }
  }
}
 


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
