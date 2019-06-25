#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "global.h"
 
 
 
u8  key_tab[16]; 
 
u16 key_timer_js=0; 
 
 
 
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB端口时钟
 
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;	  //  端口配置
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					  

 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	  //  端口配置
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 //推挽输出
 GPIO_Init(GPIOA, &GPIO_InitStructure);			
}

void 	key_timer_fution(void) 
{


	key_timer_js++;
 
 
}



void  key_fuc(void)
{
  TRAN_D_struct   TRAN_info1;
 	u8  dat[18]; 

	
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
	 TRAN_info1.data_len= 16+2;

	 dat[0]=00;
	 dat[1]=0x0b;

	
	K_ROW1=0;         
  K_ROW2=1;      
  K_ROW3=1;   
  K_ROW4=1; 
	if( K_COL1==0  )
	{
		delay_ms(10);
		if( K_COL1==0 )
	 {	
	   while(K_COL1==0){};

			
			 if( key_tab[0]==0)
			 {
			     key_tab[0]=1;
				   LED_DAT[1]|=L_R1C1;
			 }
			 else 
			 {
		    	 key_tab[0]=0;
				   LED_DAT[1]&=~L_R1C1;
			 }
		   memcpy( dat+2 , key_tab , 16 );
		   RS485_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   ); 
			 
			 
		 LED_DAT[0]=0X00;
	 
     write_595_LED( LED_DAT,2);
			 
  	}

	}
	else if( K_COL2==0  )
	{
		delay_ms(10);
		if( K_COL2==0 )
	  {	
	   while(K_COL2==0){};
			
			 if( key_tab[1]==0)
			 {
			     key_tab[1]=1;
				   LED_DAT[1]|=L_R1C2;
			 }
			 else 
			 {
		    	 key_tab[1]=0;
				  LED_DAT[1]&=~L_R1C2;
			 }
		   memcpy( dat+2 , key_tab , 16 );
		   RS485_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   ); 
    			 
		 LED_DAT[0]=0X00;
	 
     write_595_LED( LED_DAT,2);
  	}
	}	
	else if( K_COL3==0  )//P5  锁定  刷卡才能灯变
	{
    delay_ms(10);
		if( K_COL3==0 )
	  {	
	   while(K_COL3==0){};
			
			 if( key_tab[2]==0)
			 {
			     key_tab[2]=1;
				   LED_DAT[1]|=L_R1C3;
			 }
			 else 
			 {
		    	 //key_tab[2]=0;//P5  锁定  刷卡才能灯变
				   // LED_DAT[1]&=~L_R1C3;
			 }
		   memcpy( dat+2 , key_tab , 16 );
		   RS485_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   ); 
        
		 LED_DAT[0]=0X00;
	    
     write_595_LED( LED_DAT,2);
  	}

	}		 
  else if( K_COL4==0  )
	{ 
    delay_ms(10);
		if( K_COL4==0 )
	  {	
	   while(K_COL4==0){};
			
			 if( key_tab[3]==0)
			 {
			     key_tab[3]=1;
				 	 LED_DAT[1]|=L_R1C4;
			 }
			 else 
			 {
		    	 key_tab[3]=0;
				   LED_DAT[1]&=~L_R1C4;
			 }
		   memcpy( dat+2 , key_tab , 16 );
		   RS485_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   ); 
    	 LED_DAT[0]=0X00; 
     write_595_LED( LED_DAT,2);
  	};
	}
  else 
	{}		

	//第二排	
  K_ROW1=1;         
  K_ROW2=0;      
  K_ROW3=1;   
  K_ROW4=1; 
	if( K_COL1==0  )
	{
		delay_ms(10);
		if( K_COL1==0 )
	 {	
	   while(K_COL1==0){};

   		 if( key_tab[4]==0)
			 {
			     key_tab[4]=1;
				  LED_DAT[1]|= L_R2C1;
			 }
			 else 
			 {
		    	 key_tab[4]=0;
				   LED_DAT[1]&=~L_R2C1;
			 }
		   memcpy( dat+2 , key_tab , 16 );
		   RS485_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   ); 
			
			 LED_DAT[0]=0X00; 
       write_595_LED( LED_DAT,2);
  	}

	}
	else if( K_COL2==0  )
	{
		delay_ms(10);
		if( K_COL2==0 )
	  {	
	   while(K_COL2==0){};

       if( key_tab[5]==0)
			 {
			     key_tab[5]=1;
				   LED_DAT[1]|= L_R2C2;
			 }
			 else 
			 {
		    	 key_tab[5]=0;
				  LED_DAT[1]&=~L_R2C2;
			 }
		   memcpy( dat+2 , key_tab , 16 );
		   RS485_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   ); 
			 
			  LED_DAT[0]=0X00; 
       write_595_LED( LED_DAT,2);
  	}
	}	
	else if( K_COL3==0  )
	{
    delay_ms(10);
		if( K_COL3==0 )
	  {	
	   while(K_COL3==0){};
   		 if( key_tab[6]==0)
			 {
			     key_tab[6]=1;
				   LED_DAT[1]|= L_R2C3;
			 }
			 else 
			 {
		    	 key_tab[6]=0;
				 	 LED_DAT[1]&=~L_R2C3;
			 }
		   memcpy( dat+2 , key_tab , 16 );
		   RS485_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   ); 
       LED_DAT[0]=0X00; 
       write_595_LED( LED_DAT,2);
  	}

	}		 
  else if( K_COL4==0  )
	{ 
    delay_ms(10);
		if( K_COL4==0 )
	  {	
	   while(K_COL4==0){};

    
  	};
	}
  else 
	{}		
		
		
		
}






