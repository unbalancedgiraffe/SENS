/*
 * driver.c
 *
 *  Created on: Jul 8, 2015
 *      Author: Tingkai
 */

#include "Events.h"
#include "I2CFreedom.h"
#include "string.h"

extern bool I2C_SENT_FLAG;
extern bool I2C_RCVD_FLAG;

#define true 1
#define false 0
#define X 0
#define Y 1
#define Z 2


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

// accelerometer sensor structure definition
struct AccelSensor
{
	int16 iGp[3];			// 25Hz integer readings (counts)
	float fGp[3];			// 25Hz readings (g)
	float fgPerCount;		// initialized to FGPERCOUNT
};

// initialize MMA8451 accelerometer sensor on Freedom board for 50Hz
int8 MMA8451_Freedom_Init_50Hz(LDD_TDeviceData *DeviceDataPtr, struct AccelSensor *pthisAccel)
{
	LDD_I2C_TBusState BusState;		// I2C bus state
	LDD_I2C_TErrorMask MMA8451_I2C_Error;

	// set up the MMA8451 I2C address
	I2CFreedom_SelectSlaveDevice(DeviceDataPtr, LDD_I2C_ADDRTYPE_7BITS, MMA8451_FRDM_I2C_ADDR);

	// write 0000 0000 = 0x00 to CTRL_REG1 to place MMA8451 into standby
	// [7-1] = 0000 000
	// [0]: active=0
	I2C_Buf[0] = MMA8451_CTRL_REG1;
	I2C_Buf[1] = 0x00;
	I2C_SENT_FLAG = FALSE; //clear I2C_SENT_FLAG

	// transmit the bytes
	I2CFreedom_MasterSendBlock(DeviceDataPtr, I2C_Buf, 2, LDD_I2C_SEND_STOP);

	// loop while no error detected and the I2C sent callback has not yet set the sent flag
	do
	{
		// read the error flag
		// ERR_OK = 0x00: device is present
		// ERR_DISABLED = 0x07: device is disabled
		// ERR_SPEED = 0x01: device does not work in the active speed mode
		I2CFreedom_GetError(DeviceDataPtr, &MMA8451_I2C_Error);
	}
	while ((!MMA8451_I2C_Error) && !I2C_SENT_FLAG);

	// return immediately with error condition if MPL3115 is not present
	if (MMA8451_I2C_Error)
		return false;

	// wait until the I2C bus is idle
	do
	{
		I2CFreedom_CheckBus(DeviceDataPtr, &BusState);
	} while (BusState != LDD_I2C_IDLE);

	// write 0000 0001 = 0x01 to XYZ_DATA_CFG register to set g range
	// [7-5]: reserved=000
	// [4]: HPF_OUT=0
	// [3-2]: reserved=00
	// [1-0]: FS=01 for +/-4g: 512 counts / g = 8192 counts / g after 4 bit left shift
	I2C_Buf[0] = MMA8451_XYZ_DATA_CFG;
	I2C_Buf[1] = 0x01;
	I2C_SENT_FLAG = FALSE;
	I2CFreedom_MasterSendBlock(DeviceDataPtr, I2C_Buf, 2, LDD_I2C_SEND_STOP);
	// wait until the I2C sent callback function sets the sent flag
	while (!I2C_SENT_FLAG);
	// wait until the I2C bus is idle
	do
	{
		I2CFreedom_CheckBus(DeviceDataPtr, &BusState);
	} while (BusState != LDD_I2C_IDLE);

	// write 0010 0001 = 0x21 to CTRL_REG1
	// [7-6]: aslp_rate=00
	// [5-3]: dr=100 for 50Hz data rate
	// [2]: unused=0
	// [1]: f_read=0 for normal 16 bit reads
	// [0]: active=1 to take the part out of standby and enable sampling
	I2C_Buf[0] = MMA8451_CTRL_REG1;
	I2C_Buf[1] = 0x21;
	I2C_SENT_FLAG = FALSE;
	I2CFreedom_MasterSendBlock(DeviceDataPtr, I2C_Buf, 2, LDD_I2C_SEND_STOP);
	// wait until the I2C sent callback function sets the sent flag
	while (!I2C_SENT_FLAG);
	// wait until the I2C bus is idle
	do
	{
		I2CFreedom_CheckBus(DeviceDataPtr, &BusState);
	} while (BusState != LDD_I2C_IDLE);

	return true;
}


// read MMA8451 accelerometer data on Freedom board data over I2C
void MMA8451_Freedom_ReadData(LDD_TDeviceData *DeviceDataPtr, struct AccelSensor *pthisAccel)
{
	LDD_I2C_TBusState BusState;		// I2C bus state

	// set up the MMA8451 I2C address
	I2CFreedom_SelectSlaveDevice(DeviceDataPtr, LDD_I2C_ADDRTYPE_7BITS, MMA8451_FRDM_I2C_ADDR);
	// set up the address of the first output register
	I2C_Buf[0] = MMA8451_OUT_X_MSB;
	I2C_SENT_FLAG = FALSE;
	I2CFreedom_MasterSendBlock(DeviceDataPtr, I2C_Buf, 1, LDD_I2C_NO_SEND_STOP);
	// wait until the I2C sent callback function sets the sent flag
	while (!I2C_SENT_FLAG);

	// read the 6 bytes of sequential sensor data
	I2C_RCVD_FLAG = FALSE;
	I2CFreedom_MasterReceiveBlock(DeviceDataPtr, I2C_Buf, 6, LDD_I2C_SEND_STOP);
	// wait until the I2C received callback function sets the received flag
	while (!I2C_RCVD_FLAG);
	// wait until the I2C bus is idle
	do
	{
		I2CFreedom_CheckBus(DeviceDataPtr, &BusState);
	} while (BusState != LDD_I2C_IDLE);

	// place the 12 bytes read into the 16 bit accelerometer structure
	pthisAccel->iGp[X] = (I2C_Buf[0] << 8) | I2C_Buf[1];
	pthisAccel->iGp[Y] = (I2C_Buf[2] << 8) | I2C_Buf[3];
	pthisAccel->iGp[Z] = (I2C_Buf[4] << 8) | I2C_Buf[5];

	// check for -32768 in the accelerometer since
	// this value cannot be negated in a later HAL operation
	if (pthisAccel->iGp[X] == -32768) pthisAccel->iGp[X]++;
	if (pthisAccel->iGp[Y] == -32768) pthisAccel->iGp[Y]++;
	if (pthisAccel->iGp[Z] == -32768) pthisAccel->iGp[Z]++;

	// store the gain terms in the accelerometer sensor structure
#define MMA8451_GPERCOUNT 0.0001220703125F			// equal to 1/8192
	pthisAccel->fgPerCount = MMA8451_GPERCOUNT;

	return;
}

