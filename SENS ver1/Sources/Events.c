/* ###################################################################
**     Filename    : Events.c
**     Project     : SENS ver1
**     Processor   : MKL46Z256VLL4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-07-08, 09:31, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 

extern bool I2C_RCVD_FLAG;
extern bool I2C_SENT_FLAG;

// I2C communications globals
#define I2C_BUF_LEN           16
uint8_t I2C_Buf[I2C_BUF_LEN];

// Freedom FRDM-KL46Z board sensor I2C addresses
#define MMA8451_FRDM_I2C_ADDR		0x1D

// MMA8451 registers and constants
#define MMA8451_STATUS					0x00
#define MMA8451_OUT_X_MSB       	  	0x01
#define MMA8451_WHO_AM_I      			0x0D
#define MMA8451_XYZ_DATA_CFG     	  	0x0E
#define MMA8451_CTRL_REG1        	 	0x2A
#define MMA8451_CTRL_REG2         		0x2B
#define MMA8451_WHO_AM_I_VALUE     		0x1A



/* User includes (#include below this line is not maintained by Processor Expert) */
#include "driver.h"

/*
** ===================================================================
**     Event       :  Cpu_OnNMIINT (module Events)
**
**     Component   :  Cpu [MKL46Z256LL4]
*/
/*!
**     @brief
**         This event is called when the Non maskable interrupt had
**         occurred. This event is automatically enabled when the [NMI
**         interrupt] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnNMIINT(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  I2CFreedom_OnMasterBlockSent (module Events)
**
**     Component   :  I2CFreedom [I2C_LDD]
*/
/*!
**     @brief
**         This event is called when I2C in master mode finishes the
**         transmission of the data successfully. This event is not
**         available for the SLAVE mode and if MasterSendBlock is
**         disabled. 
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer is passed
**                           as the parameter of Init method.
*/
/* ===================================================================*/
void I2CFreedom_OnMasterBlockSent(LDD_TUserData *UserDataPtr)
{
	//set the I2CFreedom data sent flg
	I2C_SENT_FLAG = TRUE;

	return;
}

/*
** ===================================================================
**     Event       :  I2CFreedom_OnMasterBlockReceived (module Events)
**
**     Component   :  I2CFreedom [I2C_LDD]
*/
/*!
**     @brief
**         This event is called when I2C is in master mode and finishes
**         the reception of the data successfully. This event is not
**         available for the SLAVE mode and if MasterReceiveBlock is
**         disabled.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer is passed
**                           as the parameter of Init method.
*/
/* ===================================================================*/
void I2CFreedom_OnMasterBlockReceived(LDD_TUserData *UserDataPtr)
{
  //set the I2CFreedom data received flag
	I2C_RCVD_FLAG = TRUE;

	return;
}

/*
** ===================================================================
**     Event       :  UART_OnBlockReceived (module Events)
**
**     Component   :  UART [Serial_LDD]
*/
/*!
**     @brief
**         This event is called when the requested number of data is
**         moved to the input buffer.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer is passed
**                           as the parameter of Init method.
*/
/* ===================================================================*/
void UART_OnBlockReceived(LDD_TUserData *UserDataPtr)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  UART_OnBlockSent (module Events)
**
**     Component   :  UART [Serial_LDD]
*/
/*!
**     @brief
**         This event is called after the last character from the
**         output buffer is moved to the transmitter. 
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer is passed
**                           as the parameter of Init method.
*/
/* ===================================================================*/
void UART_OnBlockSent(LDD_TUserData *UserDataPtr)
{
  /* Write your code here ... */
	//return with no action
	return;
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
