// Copyright (c) 2014, Freescale Semiconductor, Inc.
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Freescale Semiconductor, Inc. nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL FREESCALE SEMICONDUCTOR, INC. BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "LED_RED.h"
#include "LED_GREEN.h"
#include "FTM.h"
#include "UART.h"
#include "I2CFreedom.h"
#include "SegLCD.h"
#include "include_all.h"
#include "LCD.h"

/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "include_all.h"

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/

	// local variables
	uint16 LedGreenCounter = 0;		// green LED flash
	uint8 iLCDBuffer[8];  			// LCD buffer
	int16 iRho;						// integer compass heading (deg)
	int16 idigit1, idigit2;			// compass heading digits

	// switch all three LEDs off
	LED_RED_SetVal(NULL);
	LED_GREEN_SetVal(NULL);

	// configure the ARM systick timer
	SYST_CSR = 0x5u;				// enable systick from internal clock
	SYST_RVR = 0x00FFFFFFu;			// set reload to maximum 24 bit value

	// set the sensor sampling frequency (25Hz)
	// this is set to 25Hz by default in PE but we want to set it using value in proj_config.h 
	FTM_SetPeriodTicks(FTM_DeviceData, (uint16) (FTM_INCLK_HZ / SENSORFS));

	// initialize globals
	globals.FTMReload = (uint16)(FTM_INCLK_HZ / SENSORFS);
	globals.FTMTimestamp = 0;
	globals.iPacketNumber = 0;
	globals.AngularVelocityPacketOn = true;
	globals.DebugPacketOn = true;
	globals.RPCPacketOn = true;
	globals.QuaternionPacketType = Q6MA;
	globals.MagneticPacketID = 0;
	globals.FusionGo = false;

	// initialize the sensors, magnetic calibration and fusion state vectors
	MMA8451_Freedom_Init_50Hz(I2CFreedom_DeviceData, &thisAccel);
	MAG3110_Freedom_Init_40Hz(I2CFreedom_DeviceData, &thisMag);
	fInitMagCalibration(&thisMagCal, &thisMagBuffer);
	thisSV_3DOF_G_BASIC.resetflag = true;		
	thisSV_3DOF_B_BASIC.resetflag = true;		
	thisSV_6DOF_GB_BASIC.resetflag = true;			

	// initialize the incoming command buffer to all '~' = 0x7E and trigger a callback 
	// when any single command character is received into the UART buffer
	iCommand[0] = iCommand[1] = iCommand[2] = iCommand[3] = '~';
	UART_ReceiveBlock(UART_DeviceData, sUARTInputBuf, 1);

	// initialize and clear the LCD
	SegLCD_Init(NULL);  //initialize LCD

	// loop indefinitely (typically 25Hz)
	globals.loopcounter = 0;
	while(1)
	{
		// wait here for the 25Hz fusion event from the hardware timer interrupt
		while (!globals.FusionGo)
		{
			;
		}
		globals.FusionGo = false;

		// flash the green LED to denote the sensor fusion is running and faster the better the magnetic calibration
		LedGreenCounter++;
		if (thisMagCal.fFitErrorpc <= 4.0F)
			LedGreenCounter += 8;
		else if (thisMagCal.fFitErrorpc <= 6.5F)
			LedGreenCounter += 6;
		else if (thisMagCal.fFitErrorpc <= 8.0F)
			LedGreenCounter += 4;
		else if (thisMagCal.fFitErrorpc <= 11.0F)
			LedGreenCounter += 2;
		else if (thisMagCal.fFitErrorpc <= 15.0F)
			LedGreenCounter += 1;
		if (LedGreenCounter >= 25)
		{
			LED_GREEN_NegVal(NULL);
			LedGreenCounter = 0;
		}

		// call the eCompass sensor fusion algorithm which in turn calls the magnetic calibration
		Fusion_Run();

		// LCD compass heading using workaround for absence of sprintf in this project
		iRho = (int16) thisSV_6DOF_GB_BASIC.fLPRho;
		iLCDBuffer[0] = ' ';
		iLCDBuffer[4] = '\0';
		// determine compass heading digit 1
		idigit1 = iRho / 100;
		iRho -= idigit1 * 100;
		if (idigit1 == 0)
			iLCDBuffer[1] = ' ';
		else
			iLCDBuffer[1] = '0' + idigit1;
		// determine compass heading digit 2
		idigit2 = iRho / 10;
		iRho -= idigit2 * 10;
		if ((idigit1 == 0) && (idigit2 == 0))
			iLCDBuffer[2] = ' ';
		else
			iLCDBuffer[2] = '0' + idigit2;		
		// determine compass heading digit 3
		iLCDBuffer[3] = '0' + iRho;		
		// write the compass heading to the LCD
		LCDWriteString(iLCDBuffer);
	
		// transmit the packets
		CreateAndSendPacketsViaUART(UART_DeviceData);

	}  // end of (typically 25Hz) infinite loop

	/*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/
