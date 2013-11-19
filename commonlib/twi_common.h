/*
 * twi_common.h
 *
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#ifndef TWI_COMMON_H_
#define TWI_COMMON_H_

#include <stdbool.h>

/************************************************************************/
/*  Least significant bit is ignored                                    */
/************************************************************************/
typedef enum _TWI_ADDRESS {
	TWI_COMMUNICATION_MODULE_ADDRESS = 0x02,
	TWI_SENSOR_MODULE_ADDRESS = 0x04,
	TWI_CONTROL_MODULE_ADDRESS = 0x06
} TWI_MODULE_ADDRESS;

int TWI_common_initialize(TWI_MODULE_ADDRESS my_address, bool enable_interrupts, int bitrate, bool master);

#endif /* TWI_COMMON_H_ */