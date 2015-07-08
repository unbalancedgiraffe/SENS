/*
 * driver.h
 *
 *  Created on: Jul 8, 2015
 *      Author: Tingkai
 */

#ifndef DRIVER_H_
#define DRIVER_H_


// functions defined in FSL_utils.c
int8 MMA8451_Freedom_Init_50Hz(LDD_TDeviceData *DeviceDataPtr, struct AccelSensor *pthisAccel);
void MMA8451_Freedom_ReadData(LDD_TDeviceData *DeviceDataPtr, struct AccelSensor *pthisAccel);


#endif /* DRIVER_H_ */
