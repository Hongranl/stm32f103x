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
  */ 

/* Includes ------------------------------------------------------------------*/



#include "ALL_Includes.h"
//定义CPU内部时钟
#define  SYS_CLOCK        16
#define  TIM4_PERIOD      124


u8    Data_Len = 0;

u8    uart_rx_flg = 0;

u16   UART_NO_DAT_TIME = 0;

u8    dat_595 = 0;

u8    motor_status = 0; 
u8    screen_status = 0; 

u8    screen_sw_mode = 0;

u16   screen_relay_off_time = 0;

u16   screen_relay_on_time = 0;

u16   screen_on_time = 0;

u8    start_screen_on_tim_flg = 0;

u16   CLAMP_CK_TIM = 0;

u8    flash_led_power = 0;

u8    motor_limit = 0;

u8    motor_running_status = 0;

u16   JDQ_TIME = 0;

u8   Key_c = Key_None; //按键状态

u8   UART_RX_CMD = UART_RX_CMD_None; //





void Delay (uint16_t nCount);

void CLOCK_Config(u8 SYS_CLK);
void All_Congfig(void);
void IO_Init(void);
void write_595( u8  *dat,int  len );
void UART1_Config(void);
 
void TIM4_Config(void);

u8 KeyScan(void);
void jiance(void);
 
void  RAM_INIT( void );

/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  TRAN_D_struct   TRAN_info1 ;
  u8 dat[UART_RX_LEN];

  u8 motor_run_status_cun = 0;      //电机状态
  u8 screen_status_cun = 0;         //显示屏状态
  u8 ab = 0;

  All_Congfig();
    
 
  while (1)
  { 
    if( motor_run_status_cun != motor_running_status || screen_status_cun != screen_status )  //状态改变 向上位机报告状态
    {  
      TRAN_info1.dest_dev_num = ( DTN_JCZX_BOARD << 8 | DTN_JCZX_BOARD >> 8 );

      TRAN_info1.dest_addr[0] = 0x00;
      TRAN_info1.dest_addr[1] = 0x00;
      TRAN_info1.dest_addr[2] = 0x00;
      TRAN_info1.dest_addr[3] = 0x00;

      TRAN_info1.source_dev_num = ( DTN_JCZX_fanzhuang << 8 | DTN_JCZX_fanzhuang >> 8 );

      TRAN_info1.source_addr[0] = 0;
      TRAN_info1.source_addr[1] = 0;
      TRAN_info1.source_addr[2] = 0;
      TRAN_info1.source_addr[3] = 0;

      TRAN_info1.TYPE_NUM = 4;   //第4类
      TRAN_info1.data_len = 4; 

      dat[0] = 0;
      dat[1] = 0x0a;
      dat[2] = screen_status;
      dat[3] = motor_running_status;


      if(send_urt_time == 0)
      {
        Rs485_COMM_SD_load_buf( 0xAAAA  ,0xBBBB ,&TRAN_info1 , dat , TRAN_info1.data_len );  //发送给上位机

        ab =! ab;
        RS485_LED_PA2_Pin_OUT(ab);  //控制LED灯 反转

        motor_run_status_cun = motor_running_status; //保存状态
        screen_status_cun = screen_status;
      }  
    }
        
    Key_c = KeyScan();  //判断按按状态
   
    Rs485_COMM_uart_fuc(); //监视串口
    
    jiance();
     
    switch( screen_sw_mode )
    { 
      case 0:


      break;
      
      case screen_on://执行屏幕开操作
      {
        if(  screen_relay_on_time == 0 && start_screen_on_tim_flg == 0 )//执行打开屏，并开始5S 检测是否开屏成功
        {
          dat_595 |=SCREEN_ON_595Pin; //0  是开
          write_595( &dat_595 ,1 );
          Delay (100);
          dat_595 &= ~ SCREEN_ON_595Pin; //1  是 
          write_595( &dat_595 ,1 );

          screen_on_time = 5000;
          start_screen_on_tim_flg = 1;
        }
     
        if( start_screen_on_tim_flg )
        {
          if( screen_on_time == 0 )
          { 
            if( SCREEN_LED_R_PC6_Pin_RD != 0 && SCREEN_LED_G_PC7_Pin_RD != 0 )//都是高电平表示屏关
            {
              screen_relay_on_time = 200; //接着开
              start_screen_on_tim_flg = 0;
            } 
          } 
        }   
            
        if( SCREEN_LED_R_PC6_Pin_RD == 0 || SCREEN_LED_G_PC7_Pin_RD == 0 )//其中有一个等于0，表示显示器已经开机完成
        {
          screen_on_time = 0;
          screen_sw_mode = 0;
          flash_led_power = 0;
          screen_status = screen_on;
          //上传给上级设备 信息
        } 
            
        if(flash_led_power)
        {
          dat_595 |= LED1_595Pin; // 
          write_595( &dat_595 ,1 ); 
        }
        else
        {
          dat_595 &= ~ LED1_595Pin; // 
          write_595( &dat_595 ,1 ); 
        }

      } break;
      
      
      case screen_off://执行屏幕关操作
      {
        if( screen_relay_off_time == 0 )
        {
          //关屏继电器
          dat_595 |=RLY1_595Pin; 
          dat_595 |=  LED1_595Pin; // 关灯
          write_595( &dat_595 ,1 ); 

          if( SCREEN_LED_R_PC6_Pin_RD != 0 && SCREEN_LED_G_PC7_Pin_RD != 0 )// 
          {
            screen_sw_mode = 0;
            screen_status = screen_off;
            //上传给上级设备 信息
          }  
        }
      }break;
      
      default:
      break;
    }
  }
}



void UART1_Config(void)
{
  UART1_DeInit();
  UART1_Init((u32)57600,UART1_WORDLENGTH_8D,UART1_STOPBITS_1,UART1_PARITY_NO,UART1_SYNCMODE_CLOCK_DISABLE,UART1_MODE_TXRX_ENABLE);
}


/**
  * @brief  Configure TIM4 to generate an update interrupt each 1ms 
  * @param  None
  * @retval None
  */
 void TIM4_Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_PERIOD);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  
  /* enable interrupts */
  enableInterrupts();

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}


void All_Congfig(void)
{
  CLOCK_Config(SYS_CLOCK);//系统时钟初始化  
  RAM_INIT();
  IO_Init();  
  UART1_Config();
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
  UART1_Cmd(ENABLE);
  TIM4_Config();       //加一个定时器
  enableInterrupts();
}



void  RAM_INIT( void )
{
  dat_595 = 0;
  motor_running_status = motor_stop;
  motor_status = motor_stop; 
  screen_status = screen_off; 

  screen_sw_mode = 0;
}

//按键扫描

u8 KeyScan(void)
{
  if( KEY1_PA3_Pin_RD == 0 &&  KEY2_PB5_Pin_RD == 0 &&  KEY3_PB4_Pin_RD == 0 )
  {
    Delay(10000); 
    if( KEY1_PA3_Pin_RD == 0 &&  KEY2_PB5_Pin_RD == 0 &&  KEY3_PB4_Pin_RD == 0 )
    {
      if( KEY1_PA3_Pin_RD == 0 ) return Key_Screen;
      if( KEY2_PB5_Pin_RD == 0 ) return Key_Up;
      if( KEY3_PB4_Pin_RD == 0 ) return Key_Down;
    }
    else
    {
      return Key_None;
    }
  }
  else
  {
    return Key_None;
  }
}

//控制屏幕上升
void contrl_motor_up(void)
{
  dat_595 |=RLY2_595Pin;  
  dat_595 |=RLY3_595Pin; 

  write_595( &dat_595 ,1 );
  Delay(50000); 
  Delay(50000); 
  Delay(50000); 
  
  dat_595 &=~RLY2_595Pin;
  dat_595 |= RLY3_595Pin;   //Up  RLY3 = 1 工作

  dat_595 &=~LED2_595Pin; // 
  dat_595 |= LED3_595Pin; //

  write_595( &dat_595 ,1 );

  motor_status = motor_up; 
  motor_running_status = motor_up;
}


//控制屏幕下降
void contrl_motor_down(void)
{
  dat_595 |=RLY2_595Pin;  
  dat_595 |=RLY3_595Pin; 

  write_595( &dat_595 ,1 );
  Delay(50000); 
  Delay(50000); 
  Delay(50000); 

  dat_595 |= RLY2_595Pin;   //  RLY2 = 1 工作
  dat_595 &=~RLY3_595Pin;

  dat_595 |= LED2_595Pin; //
  dat_595 &=~LED3_595Pin; // 

  write_595( &dat_595 ,1 );

  motor_status = motor_down; 
  motor_running_status = motor_down;
}

//控制屏幕停止
void contrl_motor_stop(void)
{
  dat_595 |=RLY2_595Pin;  
  dat_595 |=RLY3_595Pin; 
  
  dat_595 |=  LED2_595Pin; 
  dat_595 |=  LED3_595Pin;
  
  write_595( &dat_595 ,1 );
  
  motor_status = motor_stop;  
  motor_running_status = motor_stop; 
}


void jiance(void)
{
  //屏开关机键
  if( KEY1_PA3_Pin_RD == 0 )
  {  
    Delay(10000);  
    if( KEY1_PA3_Pin_RD == 0 )
    {
      JDQ_TIME = 5000;
      while( KEY1_PA3_Pin_RD == 0 )
      {    
        Rs485_COMM_uart_fuc(); 
        Delay (100); 
        if( JDQ_TIME == 0 )
        {
          break;
        }
      }; 

      //其中有一个等于0，表示显示器已经开机完成
          /*
      如果显示屏原来状态是开机状态，则关闭显示屏，如果显示屏是关机状态则开启显示屏，显示屏开机过程中LED1指示灯闪烁；
      开机完成，LED1常亮；显示器关机LED1灭 
          */ 
      if( screen_status == screen_off )//执行屏关的操作
      {
        screen_sw_mode = screen_on;

        dat_595 &=~ RLY1_595Pin; //取电
        write_595( &dat_595 ,1 ); 

        screen_relay_on_time = 1000;//2S 后开屏
      }
         
      if( screen_status == screen_on )//执行屏开的操作
      {
        screen_sw_mode = screen_off; 
        //关闭屏 
        dat_595 |= SCREEN_ON_595Pin; //0  是开
        write_595( &dat_595 ,1 );
        Delay(100);
        dat_595 &= ~ SCREEN_ON_595Pin; //1  是 
        write_595( &dat_595 ,1 );

        //等待3s 后断电
        screen_relay_off_time = 1000;//3S后 继电器关
      }
    } 
  }
  
  
  //上升键
  if( KEY2_PB5_Pin_RD == 0 )
  {    
    Delay (10000);  
    if( KEY2_PB5_Pin_RD == 0 )   
    {
      JDQ_TIME = 5000;
      while( KEY2_PB5_Pin_RD == 0 )
      { 
        Rs485_COMM_uart_fuc();
        Delay (100);
        if( JDQ_TIME == 0 )
        {
          break;
        }
      }; 

      if( motor_status == motor_up )  //在上升过程中再按上升键，则停止
      {
        dat_595 |=RLY2_595Pin;  //Down
        dat_595 |=RLY3_595Pin;  //Up

        dat_595 |= LED2_595Pin; // 
        dat_595 |= LED3_595Pin; // 

        write_595( &dat_595 ,1 );
        
        motor_status = motor_stop;  
        motor_running_status = motor_stop;
        
      }
      else 
      {
        if( UPLIMIT_PC4_Pin_RD == 0 )  //上限位，再按向上 无效(未限位)
        {
          dat_595 |=RLY2_595Pin;  
          dat_595 |=RLY3_595Pin; 
          
          write_595( &dat_595 ,1 );
          Delay(50000); 
          Delay(50000); 
          Delay(50000); 
          
          dat_595 |= RLY3_595Pin;   //Up  RLY3 = 1 工作
          dat_595 &=~RLY2_595Pin;
          
          dat_595 &=~LED2_595Pin; // 
          dat_595 |= LED3_595Pin; //
          
          write_595( &dat_595 ,1 );

          motor_status = motor_up; 
          motor_running_status = motor_up;
        }
      } 
    }
  } //上升键
  
  
  
  //下降键
  if( KEY3_PB4_Pin_RD == 0 )
  {    
    Delay (10000); 
    if( KEY3_PB4_Pin_RD == 0 )   //去抖
    { 
      JDQ_TIME = 5000;
      while( KEY3_PB4_Pin_RD == 0 )  //
      {    
        Rs485_COMM_uart_fuc();  
        Delay (100);
        if( JDQ_TIME == 0 )
        {
          break;
        }
      }; 

      if( motor_status == motor_down )  //在下降过程中再按下降键，则停止
      {
        dat_595 |=RLY2_595Pin;   //Down
        dat_595 |=RLY3_595Pin;   //Up

        dat_595 |= LED2_595Pin;    // 
        dat_595 |= LED3_595Pin;    // 
        
        write_595( &dat_595 ,1 );  
        
        motor_status = motor_stop;  
        motor_running_status = motor_stop;
        

      }
      else 
      {
        if( DNLIMIT_PC3_Pin_RD == 0 )  //下限位，再按向下 无效（未限位）
        {    
          dat_595 |=RLY2_595Pin;  
          dat_595 |=RLY3_595Pin; 
          
          write_595( &dat_595 ,1 );
          
          Delay(50000); 
          Delay(50000); 
          Delay(50000); 
          
          dat_595 |= RLY2_595Pin; 
          dat_595 &=~RLY3_595Pin; 
          
          dat_595 &=~LED3_595Pin; // 
          dat_595 |= LED2_595Pin; //
          
          write_595( &dat_595 ,1 );

          motor_status = motor_down;
          motor_running_status = motor_down;
        }
      }  
    }  
  } //下降键

    
  //下限位   
  if( DNLIMIT_PC3_Pin_RD != 0 )
  {    
    if( motor_status == motor_up )
    {   
      JDQ_TIME = 5000;
      while( DNLIMIT_PC3_Pin_RD != 0 )
      {   
        Rs485_COMM_uart_fuc(); 
        Delay (100);
        if( JDQ_TIME == 0 )
        {
          break;
        }
      }
    }
    else   //停止
    {
      dat_595 |=RLY2_595Pin;  
      dat_595 |=RLY3_595Pin; //控制继电器
      
      dat_595 |=  LED2_595Pin; // 
      dat_595 |=  LED3_595Pin; // 控制指示灯
      
      write_595( &dat_595 ,1 );
      Delay(5000); 
      write_595( &dat_595 ,1 );
      Delay(5000); 
      write_595( &dat_595 ,1 );
      Delay(5000); 
      motor_status = motor_stop; 

      motor_running_status = motor_limit_down;
    } 
  }  //下限位 
  
  //上限位
  if( UPLIMIT_PC4_Pin_RD != 0 )
  {    
    if( motor_status == motor_down )
    {     
      JDQ_TIME = 5000;
      while( UPLIMIT_PC4_Pin_RD != 0 )
      {   
        Rs485_COMM_uart_fuc(); 
        Delay (100);
        if( JDQ_TIME == 0 )
        {
          break;
        }
      }
    }
    else 
    {
      dat_595 |=RLY2_595Pin;  
      dat_595 |=RLY3_595Pin; 
      dat_595 |=  LED2_595Pin; // 
      dat_595 |=  LED3_595Pin; // 
      write_595( &dat_595 ,1 );
      Delay(5000); 
      write_595( &dat_595 ,1 );
      Delay(5000); 
      write_595( &dat_595 ,1 );
      Delay(5000); 
      motor_status = motor_stop;   

      motor_running_status = motor_limit_up;
    }
  }//上限位

     
  //防夹
  if( CLAMP_PC5_Pin_RD != 0 )
  {
    Delay (500);  
    if( CLAMP_PC5_Pin_RD != 0 )   
    {
      if( CLAMP_CK_TIM == 0 )
      {

      /*
      if(  motor_status==motor_up  )//防夹  上变下，下变上
      {

      dat_595&=~RLY2_595Pin;  
      dat_595&=~RLY3_595Pin; 
      write_595( &dat_595 ,1 );
      Delay (50000); 
      Delay (50000); 
      Delay (50000); 
      dat_595 |= RLY2_595Pin; 
      dat_595&=~RLY3_595Pin; 

      dat_595&=~LED3_595Pin; // 
      dat_595|= LED2_595Pin; // 
      write_595( &dat_595 ,1 );


      motor_status=motor_down; 
      motor_running_status= motor_down;
      //3s后再做检测
      CLAMP_CK_TIM=2500;
      }
      else
      */

        if( motor_status == motor_down )
        {
          dat_595 |=RLY2_595Pin;  
          dat_595 |=RLY3_595Pin; 
          
          write_595( &dat_595 ,1 );
          Delay(5000); 
          write_595( &dat_595 ,1 );
          Delay(5000); 
          write_595( &dat_595 ,1 );
          Delay(50000); 
          Delay(50000); 
          Delay(50000);  
          
          dat_595 |= RLY3_595Pin; 
          dat_595 &= ~RLY2_595Pin;

          dat_595 &= ~LED2_595Pin; // 
          dat_595 |= LED3_595Pin; // 
          write_595( &dat_595 ,1 );
          Delay(5000); 
          write_595( &dat_595 ,1 );
          Delay(5000); 
          write_595( &dat_595 ,1 );

          motor_status = motor_up;
          motor_running_status = motor_up;
          //3s后再做检测
          CLAMP_CK_TIM = 2500;
        }
        else 
        {
        
        }
      }
    }
  } //防夹 

  //SCREEN_LED_R 和SCREEN_LED_G 有一个是低电平就是开机了

}


void IO_Init(void)
{
  //设置IO口为输出
  GPIOA->DDR |= RS485_LED_PA2_Pin;//输出模式
  GPIOA->CR1 |= RS485_LED_PA2_Pin;//推挽输出

  GPIOD->DDR |= RS485_DIR_PD4_Pin;//输出模式
  GPIOD->CR1 |= RS485_DIR_PD4_Pin;//推挽输出


  GPIOD->DDR |= H595_DS_PD2_Pin;//输出模式
  GPIOD->CR1 |= H595_DS_PD2_Pin;//推挽输出 

  GPIOD->DDR |= H595_ST_PD3_Pin;//输出模式
  GPIOD->CR1 |= H595_ST_PD3_Pin;//推挽输出   

  GPIOA->DDR |= H595_SH_PA1_Pin;//输出模式
  GPIOA->CR1 |= H595_SH_PA1_Pin;//推挽输出 


  GPIOC->DDR &=~ SCREEN_LED_R_PC6_Pin;//输入模式
  GPIOC->DDR &=~ SCREEN_LED_G_PC7_Pin;//输入模式

  GPIOA->DDR &=~ KEY1_PA3_Pin;//输入模式
  GPIOB->DDR &=~ KEY2_PB5_Pin;//输入模式
  GPIOB->DDR &=~ KEY3_PB4_Pin;//输入模式


  GPIOC->DDR &=~ DNLIMIT_PC3_Pin;//输入模式
  GPIOC->DDR &=~ UPLIMIT_PC4_Pin;//输入模式
  GPIOC->DDR &=~ CLAMP_PC5_Pin;//输入模式

  /* 用的595 
  #define  LED_SCREEN_Pin   (1 << 3) 
  #define  LED_UP_Pin       (1 << 3) 
  #define  LED_DOWN_Pin     (1 << 3) 
  */

  RS485_DIR_PD4_Pin_OUT(0);  //0是接收 
  RS485_LED_PA2_Pin_OUT(1);


  H595_DS_PD2_Pin_OUT(1);     
  H595_ST_PD3_Pin_OUT(1);     
  H595_SH_PA1_Pin_OUT(1);  


  dat_595 &=~ SCREEN_ON_595Pin;//1 是关
  dat_595 |= RLY1_595Pin;
  dat_595 |= RLY2_595Pin;
  dat_595 |= RLY3_595Pin;

  dat_595 |=  LED1_595Pin;
  dat_595 |=  LED2_595Pin;
  dat_595 |=  LED3_595Pin; 

  write_595( &dat_595 ,1 );
}
 
/*********************************************
函数功能： 595
输入参数： 
输出参数：无
备    注： 
*********************************************/
//#define norly_on
 void  write_595( u8  *dt,int  len )
{  
  u8 dat[1];
  u8 c;
  int i = 0;  

  dat[0] = dt[0];

  for(i=len-1;i>=0;i--)
  {
    for( c=0;c<8;c++ )
    { 
#ifdef   norly_on  //常开或者常闭
      if(dat[i] & 0x80)
      {
        H595_DS_PD2_Pin_OUT(1); 
        H595_DS_PD2_Pin_OUT(1); 
      }
      else
      {
        H595_DS_PD2_Pin_OUT(0); 
        H595_DS_PD2_Pin_OUT(0); 
      }
#else
      if(dat[i] & 0x80)
      {
        H595_DS_PD2_Pin_OUT(0); 
        H595_DS_PD2_Pin_OUT(0); 
      }
      else
      {
        H595_DS_PD2_Pin_OUT(1); 
        H595_DS_PD2_Pin_OUT(1); 
      }
#endif
      H595_SH_PA1_Pin_OUT(0); 
      H595_SH_PA1_Pin_OUT(0); 
      Delay(5);  
      H595_SH_PA1_Pin_OUT(1); 
      H595_SH_PA1_Pin_OUT(1); 
      dat[i] <<= 1;
    } 
  }

  H595_ST_PD3_Pin_OUT(0);
  H595_ST_PD3_Pin_OUT(0);
  Delay (5);
  H595_ST_PD3_Pin_OUT(1);	//锁存	
  H595_ST_PD3_Pin_OUT(1);
}

 
/*********************************************
函数功能：系统内部时钟配置
输入参数：SYS_CLK : 2、4、8、16
输出参数：无
备    注：系统启动默认内部2ＭＨＺ
*********************************************/
void CLOCK_Config(u8 SYS_CLK)
{
  //时钟配置为内部RC，16M
  CLK->CKDIVR &=~(BIT(4)|BIT(3));

  switch(SYS_CLK)
  {
    case 2: CLK->CKDIVR |=((1<<4)|(1<<3)); break;
    case 4: CLK->CKDIVR |=(1<<4); break;
    case 8: CLK->CKDIVR |=(1<<3); break;
  }
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
