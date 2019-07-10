#include "global.h"	

 
	
  
 void  write_595_LED( u8  *dat,int  len );
 
 

 void  write_595_LED( u8  *dat,int  len )
{
	u8   c;
	int  i=0;
	u8  da ;

  
	for(i=len-1;i>=0;i--)
	{
		   da=dat[i];
		for( c=0;c<8;c++ )
		{ 
			 if(da&0x80)
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
			 da<<=1;
			 
		} 
	}		
	     ST_595  =0;
		   delay_us(1);
       ST_595  =1;	//Ëø´æ	
}

  

