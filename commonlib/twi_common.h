/*
 * twi_common.h
 *
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#ifndef TWI_COMMON_H_
#define TWI_COMMON_H_

/************************************************************************/
/*  Least significant bit is ignored in address                         */
/************************************************************************/
typedef enum _TWI_MODULE_ADDRESS {
	TWI_COMMUNICATION_MODULE_ADDRESS = 0x02,
	TWI_SENSOR_MODULE_ADDRESS = 0x04,
	TWI_CONTROL_MODULE_ADDRESS = 0x06
} TWI_MODULE_ADDRESS;

typedef enum _TWI_BITRATE {
	TWI_BITRATE_5_KHZ,
	TWI_BITRATE_50_KHZ,
	TWI_BITRATE_200_KHZ,
	TWI_BITRATE_400_KHZ
} TWI_BITRATE;

#endif /* TWI_COMMON_H_ */