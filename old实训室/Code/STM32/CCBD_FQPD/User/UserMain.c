/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions

#include "dataManager.h"
#include "RelayCon.h"
#include "UART_dataTransfer.h"
#include "sigAnalog_Detect.h"
#include "sigDigital_Detect.h"

/*
 * main: initialize and start the system
 */
int main (void) {
	
  osKernelInitialize ();                    // initialize CMSIS-RTOS

  // initialize peripherals here

  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	
	  osMsg_memPoolInit();
	
	  RelayConThread_Active();
		
	  communicationActive(comObj_Dev485);
	  communicationActive(comObj_Zigbee);
	  sigAna_DetThread_Active();
	  sigDig_DetThread_Active();

  osKernelStart ();                         // start thread execution 
}
