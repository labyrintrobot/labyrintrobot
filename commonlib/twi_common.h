/*
 * twi_common.h
 *
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#ifndef TWI_COMMON_H_
#define TWI_COMMON_H_

/************************************************************************/
/*  Least significant bit is ignored                                    */
/************************************************************************/
typedef enum _TWI_ADDRESS {
	TWI_COMMUNICATION_MODULE_ADDRESS = 0x02,
	TWI_SENSOR_MODULE_ADDRESS = 0x04,
	TWI_CONTROL_MODULE_ADDRESS = 0x06
} TWI_MODULE_ADDRESS;

#endif /* TWI_COMMON_H_ */