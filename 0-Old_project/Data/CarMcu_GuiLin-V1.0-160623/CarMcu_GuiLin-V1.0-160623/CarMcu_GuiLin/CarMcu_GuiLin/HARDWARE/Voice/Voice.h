#ifndef __VOICE_H
#define __VOICE_H	 
#include "sys.h"



void VoiceCmd_Com(u8 *cmdbuf,u16 len);
void VoiceCmd_PlayTrack(u16 track);
void Voice_rx_proing(void);


#endif 
