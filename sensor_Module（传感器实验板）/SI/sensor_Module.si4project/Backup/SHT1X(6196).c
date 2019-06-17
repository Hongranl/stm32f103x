/***********************************************************************************************************************
Copyright 2019 - 2027 深圳国泰安教育技术股份有限公司. All rights reserved.
文件名 :		SHT1X.c
描述   :    SHT1X温湿度传感器模块程序文件
作者   :    
版本   :    V1.0
修改   :  
完成日期:	2019.3.25
备注:		SHT1X非IIC通信协议，本文件所包含函数为网络下载
************************************************************************************************************************/
#include "SHT1X.h"
#include <math.h>

/*静态局部变量*/
static uint64_t SHT11_Time = 0;		//用于监控两次测量之间的时间，必须大于1000(ms)
/**************/
/*************************************************************
  Function   ：SHT10_Dly  
  Description：SHT10时序需要的延时
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_Dly(void)
{
    u16 i = 0;
    for(i = 500; i > 0; i--);
}


/*************************************************************
  Function   ：SHT10_Config  
  Description：初始化 SHT10引脚
  Input      : none        
  return     : none    
*************************************************************/
void SHT1X_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;        
	//初始化SHT10引脚时钟
	RCC_APB2PeriphClockCmd(SHT10_AHB2_CLK|RCC_APB2Periph_AFIO ,ENABLE);	//使能重映射
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
			
	//PD0 DATA 推挽输出        
	GPIO_InitStructure.GPIO_Pin = SHT10_DATA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
	//PD1 SCK 推挽输出
	GPIO_InitStructure.GPIO_Pin = SHT10_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SHT10_SCK_PORT, &GPIO_InitStructure);

	SHT10_ConReset();        //复位通讯
}

/*************************************************************
  Function   ：SHT1X_GetValue
  Description：设置DATA引脚为输出
  Input      : none        
  return     : u8 0-失败 1-成功   
*************************************************************/
u8 SHT1X_GetValue(void)
{
	u8  err = 0, checksum = 0;

	if(SystemCnt-SHT11_Time<1000)										//距离上次测量间隔是否大于1000ms
	{
		return 0;
	}
	err += SHT10_Measure(&temp_val, &checksum, TEMP);                  //获取温度测量值
	err += SHT10_Measure(&humi_val, &checksum, HUMI);                  //获取湿度测量值
	if(err != 0)
	{
		SHT10_ConReset();
		
		return 0;
	}
	else
	{
		SHT10_Calculate(temp_val, humi_val, &temp_val_real, &humi_val_real); //计算实际的温湿度值
		dew_point = SHT10_CalcuDewPoint(temp_val_real, humi_val_real);                 //计算露点温度
	}
	SHT11_Time = SystemCnt;												//记录此次完成测量的时间
	
	return 1;
}
/*************************************************************
  Function   ：SHT10_DATAOut
  Description：设置DATA引脚为输出
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_DATAOut(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        //PD0 DATA 推挽输出        
        GPIO_InitStructure.GPIO_Pin = SHT10_DATA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         
        GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
}


/*************************************************************
  Function   ：SHT10_DATAIn  
  Description：设置DATA引脚为输入
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_DATAIn(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        //PD0 DATA 浮动输入        
        GPIO_InitStructure.GPIO_Pin = SHT10_DATA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
}


/*************************************************************
  Function   ：SHT10_WriteByte  
  Description：写1字节
  Input      : value:要写入的字节        
  return     : err: 0-正确  1-错误    
*************************************************************/
u8 SHT10_WriteByte(u8 value)
{
        u8 i, err = 0;
        
        SHT10_DATAOut();                          //设置DATA数据线为输出

        for(i = 0x80; i > 0; i /= 2)  //写1个字节
        {
                if(i & value)
                        SHT10_DATA_H();
                else
                        SHT10_DATA_L();
                SHT10_Dly();
                SHT10_SCK_H();
                SHT10_Dly();
                SHT10_SCK_L();
                SHT10_Dly();
        }
        SHT10_DATAIn();                                  //设置DATA数据线为输入,释放DATA线
        SHT10_SCK_H();
        err = SHT10_DATA_R();                  //读取SHT10的应答位
        SHT10_SCK_L();

        return err;
}

/*************************************************************
  Function   ：SHT10_ReadByte  
  Description：读1字节数据
  Input      : Ack: 0-不应答  1-应答        
  return     : err: 0-正确 1-错误    
*************************************************************/
u8 SHT10_ReadByte(u8 Ack)
{
        u8 i, val = 0;

        SHT10_DATAIn();                                  //设置DATA数据线为输入
        for(i = 0x80; i > 0; i /= 2)  //读取1字节的数据
        {
                SHT10_Dly();
                SHT10_SCK_H();
                SHT10_Dly();
                if(SHT10_DATA_R())
                        val = (val | i);
                SHT10_SCK_L();
        }
        SHT10_DATAOut();                          //设置DATA数据线为输出
        if(Ack)
                SHT10_DATA_L();                          //应答，则会接下去读接下去的数据(校验数据)
        else
                SHT10_DATA_H();                          //不应答，数据至此结束
        SHT10_Dly();
        SHT10_SCK_H();
        SHT10_Dly();
        SHT10_SCK_L();
        SHT10_Dly();

        return val;                                          //返回读到的值
}


/*************************************************************
  Function   ：SHT10_TransStart  
  Description：开始传输信号，时序如下：
                     _____         ________
               DATA:      |_______|
                         ___     ___
               SCK : ___|   |___|   |______        
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_TransStart(void)
{
        SHT10_DATAOut();                          //设置DATA数据线为输出

        SHT10_DATA_H();
        SHT10_SCK_L();
        SHT10_Dly();
        SHT10_SCK_H();
        SHT10_Dly();
        SHT10_DATA_L();
        SHT10_Dly();
        SHT10_SCK_L();
        SHT10_Dly();
        SHT10_SCK_H();
        SHT10_Dly();
        SHT10_DATA_H();
        SHT10_Dly();
        SHT10_SCK_L();

}


/*************************************************************
  Function   ：SHT10_ConReset  
  Description：通讯复位，时序如下：
                     _____________________________________________________         ________
               DATA:                                                      |_______|
                        _    _    _    _    _    _    _    _    _        ___     ___
               SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_ConReset(void)
{
        u8 i;

        SHT10_DATAOut();

        SHT10_DATA_H();
        SHT10_SCK_L();

        for(i = 0; i < 9; i++)                  //触发SCK时钟9c次
        {
                SHT10_SCK_H();
                SHT10_Dly();
                SHT10_SCK_L();
                SHT10_Dly();
        }
        SHT10_TransStart();                          //启动传输
}



/*************************************************************
  Function   ：SHT10_SoftReset  
  Description：软复位
  Input      : none        
  return     : err: 0-正确  1-错误    
*************************************************************/
u8 SHT10_SoftReset(void)
{
        u8 err = 0;

        SHT10_ConReset();                              //通讯复位
        err += SHT10_WriteByte(SOFTRESET);//写RESET复位命令

        return err;
}


/*************************************************************
  Function   ：SHT10_ReadStatusReg  
  Description：读状态寄存器
  Input      : p_value-读到的数据；p_checksun-读到的校验数据       
  return     : err: 0-正确  0-错误    
*************************************************************/
u8 SHT10_ReadStatusReg(u8 *p_value, u8 *p_checksum)
{
        u8 err = 0;

        SHT10_TransStart();                                        //开始传输
        err = SHT10_WriteByte(STATUS_REG_R);//写STATUS_REG_R读取状态寄存器命令
        *p_value = SHT10_ReadByte(ACK);                //读取状态数据
        *p_checksum = SHT10_ReadByte(noACK);//读取检验和数据
        
        return err;
}



/*************************************************************
  Function   ：SHT10_WriteStatusReg  
  Description：写状态寄存器
  Input      : p_value-要写入的数据值       
  return     : err: 0-正确  1-错误    
*************************************************************/
u8 SHT10_WriteStatusReg(u8 *p_value)
{
        u8 err = 0;

        SHT10_TransStart();                                         //开始传输
        err += SHT10_WriteByte(STATUS_REG_W);//写STATUS_REG_W写状态寄存器命令
        err += SHT10_WriteByte(*p_value);         //写入配置值

        return err;
}



/*************************************************************
  Function   ：SHT10_Measure  
  Description：从温湿度传感器读取温湿度
  Input      : p_value-读到的值；p_checksum-读到的校验数        
  return     : err: 0-正确 1—错误    
*************************************************************/
u8 SHT10_Measure(u16 *p_value, u8 *p_checksum, u8 mode)
{
        u8 err = 0;
        u32 i;
        u8 value_H = 0;
        u8 value_L = 0;

        SHT10_TransStart();                                                 //开始传输
        switch(mode)                                                         
        {
        case TEMP:                                                                 //测量温度
                err += SHT10_WriteByte(MEASURE_TEMP);//写MEASURE_TEMP测量温度命令
                break;
        case HUMI:
                err += SHT10_WriteByte(MEASURE_HUMI);//写MEASURE_HUMI测量湿度命令
                break;
        default:
                break;
        }
        SHT10_DATAIn();
        for(i = 0; i < 72000000; i++)                             //等待DATA信号被拉低
        {
                if(SHT10_DATA_R() == 0) break;             //检测到DATA被拉低了，跳出循环
        }
        if(SHT10_DATA_R() == 1)                                //如果等待超时了
                err += 1;
        value_H = SHT10_ReadByte(ACK);
        value_L = SHT10_ReadByte(ACK);
        *p_checksum = SHT10_ReadByte(noACK);           //读取校验数据
        *p_value = (value_H << 8) | value_L;
        return err;
}


/*************************************************************
  Function   ：SHT10_Calculate  
  Description：计算温湿度的值
  Input      : Temp-从传感器读出的温度值；Humi-从传感器读出的湿度值
               p_humidity-计算出的实际的湿度值；p_temperature-计算出的实际温度值        
  return     : none    
*************************************************************/
void SHT10_Calculate(u16 t, u16 rh, float *p_temperature, float *p_humidity)
{
        const float d1 = -39.7;
        const float d2 = +0.01;
        const float C1 = -2.0468;
        const float        C2 = +0.0367;
        const float C3 = -0.0000015955;        
        const float T1 = +0.01;
        const float T2 = +0.00008;

        float RH_Lin;                                                                                  //RH线性值        
        float RH_Ture;                                                                                 //RH真实值
        float temp_C;

        temp_C = d1 + d2 * t;                                                              //计算温度值        
        RH_Lin = C1 + C2 * rh + C3 * rh * rh;                            //计算湿度值
        RH_Ture = (temp_C -25) * (T1 + T2 * rh) + RH_Lin;        //湿度的温度补偿，计算实际的湿度值

        if(RH_Ture > 100)                                                                        //设置湿度值上限
                RH_Ture        = 100;
        if(RH_Ture < 0.1)
                RH_Ture = 0.1;                                                                        //设置湿度值下限

        *p_humidity = RH_Ture;
        *p_temperature = temp_C;

}


/*************************************************************
  Function   ：SHT10_CalcuDewPoint  
  Description：计算露点
  Input      : h-实际的湿度；t-实际的温度        
  return     : dew_point-露点    
*************************************************************/
float SHT10_CalcuDewPoint(float t, float h)
{
        float logEx, dew_point;

        logEx = 0.66077 + 7.5 * t / (237.3 + t) + (log10(h) - 2);
        dew_point = ((0.66077 - logEx) * 237.3) / (logEx - 8.16077);

        return dew_point; 
}


//int main(void)
//{  
//        u16 humi_val, temp_val;
//        u8 err = 0, checksum = 0;
//        float humi_val_real = 0.0; 
//        float temp_val_real = 0.0;
//        float dew_point = 0.0;
//        
//        BSP_Init();
//        printf("\nSHT10温室度测试程序!!!\n");
//        SHT10_Config();
//        while(1)
//        {
//                err += SHT10_Measure(&temp_val, &checksum, TEMP);                  //获取温度测量值
//                err += SHT10_Measure(&humi_val, &checksum, HUMI);                  //获取湿度测量值
//                if(err != 0)
//                        SHT10_ConReset();
//                else
//                {
//                        SHT10_Calculate(temp_val, humi_val, &temp_val_real, &humi_val_real); //计算实际的温湿度值
//                        dew_point = SHT10_CalcuDewPoint(temp_val_real, humi_val_real);                 //计算露点温度
//                } 
//                printf("当前环境温度为:%2.1f℃，湿度为:%2.1f%%，露点温度为%2.1f℃\r\n", temp_val_real, humi_val_real, dew_point);
//                LED1_Toggle();
//                Delay_ms(1000);
//        }
//}

///***********************************************************************************************************************
//Copyright 2019 - 2027 深圳国泰安教育技术股份有限公司. All rights reserved.
//文件名 :		SHT1X.c
//描述   :      SHT1X温湿度传感器模块程序文件
//作者   :      hongxiang.liu
//版本   :      V1.0
//修改   :  
//完成日期:		2019.3.25
//备注:			SHT1X非IIC通信协议，本文件所包含函数为网络下载
//************************************************************************************************************************/

///*************HEADS**********/
//#include "SHT1X.h"
///***************************/

///* 全局变量声明 */
///****************/

////初始化SHT1X,配置引脚
////DTA--PB3
////CLK--PB4
//void SHT1x_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能Pc端口时钟
//	
//	GPIO_InitStructure.GPIO_Pin= SDA | SCL;   //
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
//	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化

////通电传感器需要11ms进入休眠状态，在此之前不允许对传感器发送任何命令
//    SHT1x_Reset();
//}

////设置SHT1X模式——写寄存器
//void SHT1x_Config(void)
//{
//	u8 SHT1X_value = 0;
//	
//	delay_ms(50);											//延时50ms，确保SHT1X稳定
//	if(!SHT1X_read_statusreg(&SHT1X_value))
//	{ 
//		if(SHT1X_value&0x07)
//		{
//			for(SHT1X_value=5;SHT1X_value!=0;SHT1X_value--)
//			{ 
//				if(!SHT1X_write_statusreg(0))break;
//				else SHT1X_softreset();
//			}
//		}
//	}
//	SHT1x_Reset();	
//}

////读取SHT1X传感器数据
//void SHT1x_GetValue(void)
//{
//	u8 SHT1x_error = 0;
//	u8 checksum = 0;
//    u16 T_Value=0,H_Value=0;
//	
//	SHT1x_error+=SHT1x_Measure( &T_Value,&checksum,0);  //measure temperature 
//	SHT1x_error+=SHT1x_Measure( &H_Value,&checksum,1);  //measure humidity
//		
//	if(SHT1x_error!=0)SHT1x_Reset(); //通讯故障,复位传感器
//	else
//	{
//		sht1x.T_Result = T_Value;
//		sht1x.H_Result = H_Value;
//		SHT1X_Caculation1((float*)&sht1x.T_Result, (float*)&sht1x.H_Result ); 
//	   
//		sht1x.Temperature=sht1x.T_Result;
//		sht1x.Humidity=(u16)sht1x.H_Result; 
//		sht1x.DEW=SHT1X_dewpoint1(sht1x.T_Result, sht1x.H_Result);
//	}
//}

//void SDA_SET_OUT(void) //设置SDA端口的输出模式：开漏输出
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;
//  GPIO_InitStructure.GPIO_Pin = SDA;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	// 开漏输出
//  GPIO_Init(GPIOB, &GPIO_InitStructure);	
//}
//void SDA_SET_IN(void)//设置SDA端口的输出模式：开漏输入
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Pin = SDA;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;// 浮动输入
//	GPIO_Init(GPIOB, &GPIO_InitStructure);	
//}

//void SCL_Pin_OUT(u8 out)//设置SCL端口的输出状态
//{
//    if(out)GPIO_SetBits(GPIOB,SCL);
//    else GPIO_ResetBits(GPIOB,SCL);
//}

//void SDA_Pin_OUT(u8 out)//设置SDA端口的输出状态
//{
//    SDA_SET_OUT();
//    if(out)GPIO_SetBits(GPIOB,SDA);
//    else GPIO_ResetBits(GPIOB,SDA);
//}

///*=========================================
//函 数 名：u8 SHT1x_Write_Byte(u8 value)字节传送函数
//参    数：参数value 要传送的数据
//功能描述：向元器件SHT10传送一个字节的数据
//返 回 值：返回值error error＝0，传送正确
//					  error＝1，传送错误
//=========================================*/
//u8 SHT1x_Write_Byte(u8 value)
//{
//    u8 i=0,error=0;
//    SDA_SET_OUT();
//	SCL_Pin_OUT(0);
//    for(i=0x80;i>0;i/=2) 
//    {
//        if(i&value)SDA_Pin_OUT(1); //循环相与，结果即为要发送的位
//        else SDA_Pin_OUT(0);
//        
//        delay_us(30);
//        SCL_Pin_OUT(1);
//        delay_us(30);
//        SCL_Pin_OUT(0);
//        delay_us(30);
//    }
//    
//    SDA_SET_IN();//SDA端口模式为浮空输入
//    
//    SCL_Pin_OUT(1);
//    error=GPIO_ReadInputDataBit(GPIOB, SDA);
//    SCL_Pin_OUT(0);
//    
//    return error;  //error＝1，通讯有误;error＝0，通讯成功
//}

///*=========================================
//函 数 名：SHT1x_Read_Byte(u8 dat)读数据函数
//参    数：参数dat=0,接受完一个字节的回复
//              dat=1,停止通讯
//功能描述：接受元器件SHT10发送到主机的一个字节的数据，
//返 回 值：返回值val	即为接受到的数据
//=========================================*/
//u8 SHT1x_Read_Byte(u8 dat)
//{
//    u8 i=0,val=0;
//    
//    SDA_SET_IN();
//	SCL_Pin_OUT(0);
//    for(i=0x80;i>0;i/=2)
//    {
//        delay_us(30);
//        SCL_Pin_OUT(1);
//        delay_us(30);
//        
//       if(GPIO_ReadInputDataBit(GPIOB, SDA)==1)val=(val | i);
//       SCL_Pin_OUT(0);
//    }
//    
//    SDA_SET_OUT();
//    if(dat)SDA_Pin_OUT(0);
//    else SDA_Pin_OUT(1);
//    delay_us(30);
//    
//    SCL_Pin_OUT(1);
//    delay_us(30);
//    SCL_Pin_OUT(0);
//    delay_us(30);
//    
//    return val;
//}

////启动传输信号
///*时序图：
//        ____    ____
//SCL: __|    |__|    |__
//     ____         _____
//SDA:     |_______|   
//*/
//void SHT1x_Start(void)
//{
//	SDA_SET_OUT(); //设置SDA端口为输出模式
//    
//	SDA_Pin_OUT(1);
//	SCL_Pin_OUT(0);
//	delay_us(30);
//	SCL_Pin_OUT(1);   
//	delay_us(30);
//	
//	SDA_Pin_OUT(0);
//	delay_us(30);
//	SCL_Pin_OUT(0);
//	delay_us(30);
//	SCL_Pin_OUT(1);
//	delay_us(30);
//    
//	SDA_Pin_OUT(1);	
//	delay_us(30);
//	SCL_Pin_OUT(0);
//}

////通讯复位时序
///*时序图：
//        __    __    __    __    __    __    __    __    __    __
//SCL: __|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__
//     ______________________________________________________       _____ 
//SDA:                                                       |______|
//*/
//void SHT1x_Reset(void)
//{
//	u8 i;
//    SDA_SET_OUT(); //设置SDA端口为输出模式

//	SDA_Pin_OUT(1);	    
////    SCL_Pin_OUT(0);

//	for(i=0;i<9;i++)
//	{
//		SCL_Pin_OUT(0);
//        delay_us(30);
//		SCL_Pin_OUT(1);
//        delay_us(30);
//	}
//    SHT1x_Start(); //启动传输
//}
///*=========================================
//函 数 名：char s_softreset(void)软件复位函数
//参    数：无
//功能描述：resets the sensor by a softreset 
//返 回 值：error
//=========================================*/
//char SHT1X_softreset(void)
//{ 
//    unsigned char error=0;  
//    SHT1x_Reset();              //reset communication
//    error+=SHT1x_Write_Byte(RESET);       //send RESET-command to sensor
//    return error;                     //error=1 in case of no response form the sensor
//}
///*=========================================
//函 数 名：char s_write_statusreg(unsigned char value)写状态函数
//参    数：参数value
//功能描述： writes the status register with checksum (8-bit)
//返 回 值：error
//=========================================*/
//char SHT1X_write_statusreg(unsigned char value)
//{ 
//    unsigned char error=0;
//    SHT1x_Start();                   //transmission start
//    error+=SHT1x_Write_Byte(STATUS_REG_W);//send command to sensor
//    error+=SHT1x_Write_Byte(value);    //send value of status register
//    return error;                     //error>=1 in case of no response form the sensor
//}

///*=========================================
//函 数 名：char s_write_statusreg(unsigned char value)写状态函数
//参    数：参数value
//功能描述：reads the status register with checksum (8-bit)
//返 回 值：error
//=========================================*/
//char SHT1X_read_statusreg(unsigned char *p_value)
//{ 
//    unsigned char error=0;
//    SHT1x_Start();                    //transmission start
//    error+=SHT1x_Write_Byte(STATUS_REG_R); //send command to sensor
//    error+=SHT1x_Write_Byte(ACK);        //read status register (8-bit)
//    *p_value = SHT1x_Read_Byte(noACK);   //read checksum (8-bit)  
//    return error;                     //error=1 in case of no response form the sensor
//}

///*=========================================
//函 数 名：u8 SHT1x_Measure(u8 *p_value, u8 *p_checksum, u8 Mode)启动函数
//参    数：参数mode	确定是测量温度或者是测量湿度
//功能描述：发送指令到SHT10执行温度和湿度的测量转换
//返 回 值：无
//=========================================*/
//u8 SHT1x_Measure(u16 *p_value, u8 *p_checksum, u8 Mode)
//{
//    u8 error=0,Value_H=0,Value_L=0;
//    
//    SHT1x_Start();//开始启动转换
//    switch(Mode)
//    {
//        case 0: error+=SHT1x_Write_Byte(MEASURE_TEMP);break; //读温度的值  
//        case 1: error+=SHT1x_Write_Byte(MEASURE_HUMI);break; //读湿度的值
//        
//        default:   
//            break;
//    } 
//	SDA_SET_IN();//定义SDA端口为输入模式
//                      
//    delay_ms(5000);//等待测量完成
//    if(GPIO_ReadInputDataBit(GPIOB, SDA))error+=1;//若长时间数据线DQ没拉低，则说明测量有错误
//    else
//    {
//        Value_H=SHT1x_Read_Byte(ACK);		 //数据的高字节
//        Value_L=SHT1x_Read_Byte(ACK);	     //数据的低字节    		
//        *p_value=( Value_H << 8) | Value_L;
//        *p_checksum=SHT1x_Read_Byte(noACK);	     //CRC校验码	
//    }
//    return error;
//}

///*=========================================
//函 数 名：void SHT1X_Caculation(float *p_temperature,float *p_humidity )数据处理函数
//参    数：无参数
//功能描述：温度和湿度补偿及输出温度值和相对湿度值
//返 回 值：无返回值
//=========================================*/
//void SHT1X_Caculation(float *p_temperature,float *p_humidity )
//{
//    const float c1=-4.0;
//    const float c2=+0.0405;
//	const float c3=-0.0000028;			//以上为12位湿度修正公示取值
//	const float t1=+0.01;
//	const float t2=+0.00008;			//以上为14位温度修正公示取值

//	
//	float T=*p_temperature;
//	float H=*p_humidity;
//	float H_nonline_Compensation;
//	float Humi_Comp;
//	float Temp_Comp;
//	
//	Temp_Comp=T * 0.01 - 39.6;					          //温度的补偿
//		
//	H_nonline_Compensation=c3*H*H  + c2*H + c1;                  //相对湿度非线性补偿
//    Humi_Comp=( Temp_Comp - 25 ) * ( t1 + t2*H ) + H_nonline_Compensation; //相对湿度对于温度依赖性补偿
//	
//	if( Humi_Comp > 100 ) Humi_Comp=100;			  //相对湿度最大值修正
//	if( Humi_Comp < 0.1 ) Humi_Comp=0.1;			  //相对湿度最小值修正
//	 
//	*p_temperature=Temp_Comp;						//保存温度补偿后的结果
//	*p_humidity=Humi_Comp; 						  //保存相对湿度补偿后的结果
//}
///*=========================================
//函 数 名：void SHT1X_Caculation1(float *p_temperature,float *p_humidity )数据处理函数
//参    数：无参数
//功能描述：温度和湿度补偿及输出温度值和相对湿度值
//返 回 值：无返回值
//=========================================*/
//void SHT1X_Caculation1(float *p_temperature,float *p_humidity )
//{
//    const int c1=40000000;
//    const int c2=405000;
//	const int c3=28;			//以上为12位湿度修正公示取值
//	const int t1=1;
//	const int t2=8;			//以上为14位温度修正公示取值

//	
//	float T=*p_temperature;
//	float H=*p_humidity;
//	float H_nonline_Compensation;
//	int Humi_Comp;
//	int Temp_Comp;
//	
//	Temp_Comp=(signed long)(T * t1 - 3960);					          //温度的补偿
//		
//	H_nonline_Compensation=(c2*H -c3* H*H-c1)/100000;                  //相对湿度非线性补偿
//    Humi_Comp=(float)( Temp_Comp - 2500 ) * ( t1 + t2*H )/100000 + H_nonline_Compensation; //相对湿度对于温度依赖性补偿
//	
//	if( Humi_Comp > 10000 ) Humi_Comp=10000;			  //相对湿度最大值修正
//	if( Humi_Comp < 10 ) Humi_Comp=10;			  //相对湿度最小值修正
//	 
//	*p_temperature=Temp_Comp;			//保存温度补偿后的结果
//	*p_humidity=Humi_Comp; 						  //保存相对湿度补偿后的结果
//}

///*=========================================
//函 数 名：float SHT1X_dewpoint1(float t,float h)露点数据处理函数
//参    数：温度T(真实值),湿度H(真实值)
//功能描述：计算露点的值
//返 回 值：dew_point
//=========================================*/
////-----此函数计算有问题-----------------------
//float SHT1X_dewpoint(float t,float h) //露点
//{ 
//    float k,dew_point ;
//  
//    k = (log10(h)-2)/0.4343 + (17.62*t)/(243.12+t);
//    dew_point = 243.12*k/(17.62-k);
//    return dew_point;
//}
////----------------------------------------

///*=========================================
//函 数 名：float SHT1X_dewpoint1(float t,float h)露点数据处理函数
//参    数：温度T,湿度H
//功能描述：计算露点的值
//返 回 值：dew_point
//=========================================*/
//float SHT1X_dewpoint1(float t,float h) //露点
//{ 
//    const float tn1=24312;  //for 0=< T <=50;
//    const float m1=1762;    //for 0=< T <=50;
//    
//    const float tn2=27262;  //for -40=< T <0;
//    const float m2=2246;    //for -40=< T <0;
//    
//    float dew_point,h1,t1 ;
//    
//    if((t>=0)&&(t<=50))
//    {
//        h1=tn1*(log10(h)+(m1*t)/(tn1+t*100));
//        t1=m1-log10(h)*100-((m1*t*100)/(tn1+t*100));
//        dew_point=h1/t1;
//    }
//    else if((t<0)&&(t>=-40))
//    {
//        h1=tn2*(log10(h)+(m2*t)/(tn2+t*100));
//        t1=m2-log10(h)*100-((m2*t*100)/(tn2+t*100));
//        dew_point=h1/t1;    
//    }
//    return dew_point;
//}







