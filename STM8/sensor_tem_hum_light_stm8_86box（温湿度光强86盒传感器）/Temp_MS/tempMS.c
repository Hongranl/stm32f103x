#include "tempMS.h"//温湿度检测驱动进程函数；


/*********************************************************************
 * LOCAL FUNCTIONS
 */
void reset(void);
void sht11_start(void);
void sendByte(uint8 value);
uint8 recvByte(uint8 ack);

void SHT_WInit(void)
{
	//GPIO_DeInit(GPIOD);
//	GPIO_Init(GPIOD,GPIO_PIN_2,GPIO_MODE_OUT_PP_LOW_FAST);
//	GPIO_Init(GPIOD, GPIO_PIN_3,GPIO_MODE_OUT_PP_HIGH_FAST);
 	GPIOD->DDR |= GPIO_PIN_2;//输出模式
	GPIOD->CR1 &=~ GPIO_PIN_2;//开漏输出 
	
	GPIOD->DDR |= GPIO_PIN_3;//输出模式
	GPIOD->CR1 |= GPIO_PIN_3;//推挽输出 
	
}

void SHT_RInit(void)
{
	//GPIO_DeInit(GPIOD);
	//GPIO_Init(GPIOD,GPIO_PIN_3,GPIO_MODE_OUT_PP_HIGH_FAST);
	//GPIO_Init(GPIOD, GPIO_PIN_2,GPIO_MODE_IN_FL_IT);

	GPIOD->DDR &=~ GPIO_PIN_2;//输入模式
	
//	
}

//void SHT10_Calculate(u16 t, u16 rh, float *p_temperature, float *p_humidity)
//{
//        const float d1 = -39.7;
//        const float d2 = +0.01;
//        const float C1 = -2.0468;
//        const float        C2 = +0.0367;
//        const float C3 = -0.0000015955;        
//        const float T1 = +0.01;
//        const float T2 = +0.00008;
//
//        float RH_Lin;                                                                                  //RH线性值        
//        float RH_Ture;                                                                                 //RH真实值
//        float temp_C;
//
//        temp_C = d1 + d2 * t;                                                              //计算温度值        
//        RH_Lin = C1 + C2 * rh + C3 * rh * rh;                            //计算湿度值
//        RH_Ture = (temp_C -25) * (T1 + T2 * rh) + RH_Lin;        //湿度的温度补偿，计算实际的湿度值
//
//        if(RH_Ture > 100)                                                                        //设置湿度值上限
//                RH_Ture        = 100;
//        if(RH_Ture < 0.1)
//                RH_Ture = 0.1;                                                                        //设置湿度值下限
//
//        *p_humidity = RH_Ture;
//        *p_temperature = temp_C;
//
//}


void convert_shtxx(float * hum, float *temp)
{
	const float C1=-4;//-2.0468;           // for 12 Bit RH
	const float C2=+0.0405;//+0.0367;           // for 12 Bit RH
	const float C3=-0.0000028;//-0.0000015955;     // for 12 Bit RH
	const float T1=+0.01;             // for 12 Bit RH
	const float T2=+0.00008;          // for 12 Bit RH

	float rh=*hum;             // rh:      Humidity [Ticks] 12 Bit
	float t=*temp;           // t:       Temperature [Ticks] 14 Bit
	float rh_lin;                     // rh_lin:  Humidity linear
	float rh_true;                    // rh_true: Temperature compensated humidity
	float t_C;                        // t_C   :  Temperature

	t_C     = t*0.01 - 39.66;                //calc. temperature  from 14 bit temp. ticks @ 3.5V
	rh_lin  = C3*rh*rh + C2*rh + C1;     //calc. humidity from ticks to [%RH]
	rh_true = (t_C - 25)*(T1 + T2*rh) + rh_lin;   //calc. temperature compensated humidity [%RH]

	if(rh_true > 100)
		rh_true = 100;       //cut if the value is outside of

	if(rh_true < 0.1)
		rh_true = 0.1;       //the physical possible range

	*temp = t_C;               //return temperature
	*hum  = rh_true;              //return humidity[%RH]
}


/**************************************************************************
 *@fn       readTH
 *
 *@brief    read temperature and humidity sensor data.
 *
 *@param    pData-data buffer.
 *          sht11_startIndex- .
 *
 *@return   none
 */



void readTH(uint8 *pData, uint8 *pStartIndex)
{
	/*Reading temperature*/
	reset();
	sht11_start();
	sendByte(SHT_MEASURE_TEMP);
	SHT_DATA_IN;

	*pStartIndex = 0;

	while (SHT_RD_DATA());

	pData[(*pStartIndex) + 1] = recvByte(0x1); // send ack to sensor
	pData[(*pStartIndex) + 1] &= 0x3f; /*00111111*/
	pData[(*pStartIndex)] = recvByte(0x1); // send ack to sensor
	*pStartIndex += 2;
	(void)recvByte(0x0);//crc chksum and nack to sensor, end transfer, throw away

	/*Reading humidity*/
	reset();
	sht11_start();
	sendByte(SHT_MEASURE_HUMI);
	SHT_DATA_IN;

	while (SHT_RD_DATA());

	pData[(*pStartIndex) + 1] = recvByte(0x1);
	pData[(*pStartIndex) + 1] &= 0x0f; /*00001111*/
	pData[(*pStartIndex)] = recvByte(0x1);
	*pStartIndex += 2;
	(void)recvByte(0x0);//crc chksum and nack to sensor, end transfer, throw away
}

void sht11_start()
{
  SHT_DATA_OUT;
  //SHT_SCK_OUT;

  SHT_DATA(1);
  SHT_SCK(0);// = 0;

  udelay();

  SHT_SCK(1);// = 1;
  udelay();

  SHT_DATA(0); // = 0;
  udelay();

  SHT_SCK(0);// = 0;
  udelay();

  SHT_SCK(1);// = 1;
  udelay();

  SHT_DATA(1);// = 1;
  udelay();

  SHT_SCK(0);// = 0;
}

void reset()
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transsht11_start
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{
  uint8_t i;

  SHT_DATA_OUT;
  //SHT_SCK_OUT;


  SHT_DATA(1);// = 1;
  SHT_SCK(0);// = 0;                    //Initial state
  udelay();

  for (i=0; i<9; i++)                  //9 SCK cycles
  {
    SHT_SCK(1);// = 1;
    udelay();

    SHT_SCK(0);// = 0;

    udelay();

  }
  sht11_start();                   //transmission sht11_start
}


void sendByte(uint8 value)
{
  uint8 i;

  SHT_DATA_OUT;
  //SHT_SCK_OUT;

  SHT_SCK(0);// = 0;
   udelay();

  for (i=0x80; i>0; i/=2)
  {
    if (i & value)
    {
      SHT_DATA(1);// = 1;
    }
    else
    {
      SHT_DATA(0);// = 0;
    }

    SHT_SCK(1);// = 1;
    udelay();

    SHT_SCK(0);// = 0;
    	udelay();
  }

  //SHT_DATA(1);// = 1;
  //SHT_SCK(1);// = 1;

  SHT_DATA_IN;
  SHT_SCK(1);// = 1;

  while(SHT_RD_DATA());//wait for ack

  SHT_SCK(0);// = 0;
  SHT_DATA_OUT;
}


uint8 recvByte(uint8 ack)
{
  uint8 i,val = 0;

  //SHT_SCK_OUT;


  SHT_DATA_OUT;
  SHT_DATA(1);// = 1;////////////////////

  SHT_DATA_IN;

  udelay();

  for (i=0x80; i>0; i/=2)
  {
    SHT_SCK(1);// = 1;
    udelay();

    if (SHT_RD_DATA())
    {
      val = (val | i);
    }

    SHT_SCK(0);// = 0;
    udelay();
  }

  SHT_DATA_OUT;

  SHT_DATA(!ack);// = !ack;                     //in case of "ack==1" pull down DATA-Line
  SHT_SCK(1);// = 1;                            //clk #9 for ack
  udelay();

  SHT_SCK(0);// = 0;
  SHT_DATA(1);// = 1;                          //release DATA-line
  return val;
}



tempMS_MEAS tempMS_Thread( u8*dat ){

	
	
	result_t res;
	uint8_t len;
	 res.hum_temp.hum = 0;
	 res.hum_temp.temp = 0;
	tempMS_MEAS	sensorData = {0.0,0.0};
	
	readTH((uint8 *)res.sensor_dat, &len);
	res.sensor_dat[0] ^=res.sensor_dat[1];
	res.sensor_dat[1]  =res.sensor_dat[0] ^ res.sensor_dat[1];
	res.sensor_dat[0] ^=res.sensor_dat[1];
	
	res.sensor_dat[2] ^=res.sensor_dat[3];
	res.sensor_dat[3]  =res.sensor_dat[2] ^ res.sensor_dat[3];
	res.sensor_dat[2] ^=res.sensor_dat[3];
	sensorData.hum 	 = res.hum_temp.hum;
	sensorData.temp  = res.hum_temp.temp;
	
	convert_shtxx(&sensorData.hum, &sensorData.temp);

	memcpy(dat+0,&(sensorData.temp),4);
	memcpy(dat+4,&(sensorData.hum),4);
	return sensorData;
}

