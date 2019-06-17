#include "ALL_Includes.h"


void LED_ALL_OFF(void);



void LED_ALL_OFF(void)
{
  KEY_LEFT_BLUE(0); 
  KEY_LEFT_RED(0);      //×ó¼ü

  KEY_MID_BLUE(0);   
  KEY_MID_RED(0);       //ÖÐ¼ü

  KEY_RIGHT_BLUE(0);   
  KEY_RIGHT_RED(0);     //ÓÒ¼ü
}

/******* ÉÏÉý¼ü *******/
/* 
* LED   ×ó    ÖÐ     ÓÒ
*       À¶    ºì     ºì
*/
void LED_UP(void)
{
  KEY_LEFT_BLUE(1); 
  KEY_LEFT_RED(0);      //×ó¼ü

  KEY_MID_BLUE(0);   
  KEY_MID_RED(1);       //ÖÐ¼ü

  KEY_RIGHT_BLUE(0);   
  KEY_RIGHT_RED(1);     //ÓÒ¼ü
}

/******* Í£Ö¹¼ü *******/
/* 
* LED   ×ó    ÖÐ     ÓÒ
*       ºì    À¶     ºì
*/
void LED_PAUSE(void)
{
  KEY_LEFT_BLUE(0); 
  KEY_LEFT_RED(1);      //×ó¼ü

  KEY_MID_BLUE(1);   
  KEY_MID_RED(0);       //ÖÐ¼ü

  KEY_RIGHT_BLUE(0);   
  KEY_RIGHT_RED(1);     //ÓÒ¼ü
}

/******* ÏÂ½µ¼ü *******/
/* 
* LED   ×ó    ÖÐ     ÓÒ
*       ºì    ºì     À¶
*/
void LED_DOWN(void)
{
  KEY_LEFT_BLUE(0); 
  KEY_LEFT_RED(1);      //×ó¼ü

  KEY_MID_BLUE(0);   
  KEY_MID_RED(1);       //ÖÐ¼ü

  KEY_RIGHT_BLUE(1);   
  KEY_RIGHT_RED(0);     //ÓÒ¼ü
}

/******* ÈëÍø *******/
/* 
* LED   ÖÐ¼äÀ¶µÆÉÁ5´Î
*      
*/
void LED_JOIN(void)
{
  u16 LED_time = 0;
  u8 i;
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

