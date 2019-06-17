#ifndef __DATMANAGER_H_
#define __DATMANAGER_H_

#include "sigAnalog_Detect.h"
#include "sigDigital_Detect.h"
#include "RelayCon.h"

#include "UART_dataTransfer.h"

typedef union{

	sigAna_Det_datsType	datsSigAna_Det;
	sigDig_Det_datsType	datsSigDig_Det;
	RelayCon_datsType 	datsRelayCon;
}SourceBD_Data;

typedef enum{

	datsAnalogDet = 0,
	datsDigitalDet,
	datsRelayCon,
}SourceBD_DataType;

typedef enum{

	roleUT485DNLoad = 1,
	roleUT485UPLoad,
	roleUTZigBDNLoad,
	roleUTZigBUPLoad,
}datsRole;

typedef struct{

	SourceBD_DataType	datsType;
	datsRole			role;
	SourceBD_Data		dats;
}SourceBD_MEAS;

void osMsg_memPoolInit(void);

extern osPoolId memid_SourceBD_pool;	

extern osMessageQId MsgBoxID_SBD_Relay;
extern osMessageQId MsgBoxID_SBD_AnaDet;
extern osMessageQId MsgBoxID_SBD_DigDet;
extern osMessageQId MsgBoxID_SBD_UTZigB;
extern osMessageQId MsgBoxID_SBD_UT485;

#endif

