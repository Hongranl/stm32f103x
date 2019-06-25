#include "global.h"	

 
	
  
  void  write_595_JDQ( u8  *dat,int  len );
 
 

 void  write_595_JDQ( u8  *dat,int  len )
{
    u8   c;
	  int  i=0;  
	for(i=len-1;i>=0;i--)
	{
		for( c=0;c<8;c++ )
		{ 
			 if(dat[i]&0x80)
			 {
			    DS_595=1; 
			 }
			 else
			 {
			   	 DS_595=0; 
			 }
		
		   SH_595=0; 
			 delay_us(1);
			 SH_595=1; 		
			 dat[i]<<=1;
			 
		} 
	}		
	     ST_595  =0;
		   delay_us(1);
       ST_595  =1;	//Ëø´æ	
}

  

