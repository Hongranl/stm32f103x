/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
//#include "RSART_MCU.h"
#include "prtApp_SH95.h"


/*
 * main: initialize and start the system
 */
int main (void) {
	
  osKernelInitialize ();                    // initialize CMSIS-RTOS

	  // initialize peripherals here
		
	  bsp_delayinit();

	  // create 'thread' functions that start executing,
	  // example: tid_name = osThreadCreate (osThread(name), NULL);
		//Thread_USRAT_data();
	  devApp95HF_ThreadActive();

  osKernelStart ();                         // start thread execution 
}
