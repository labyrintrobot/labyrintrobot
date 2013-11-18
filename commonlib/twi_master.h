/*
 * twi_master.h
 *
 * Created: 11/14/2013 6:00:15 PM
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#ifndef TWI_MASTER_H_
#define TWI_MASTER_H_

/************************************************************************/
/*  Least significant bit is ignored                                    */
/************************************************************************/
enum TWI_ADDRESS {
	TWI_SENSOR_ADDRESS = 0x02,
	TWI_CONTROL_MODULE_ADDRESS = 0x04
};

int TWI_initialize_bitrate(int bitrate);

int TWI_master_send_message(enum TWI_ADDRESS to_address, uint8_t header, uint8_t data);

int TWI_master_receive_message(enum TWI_ADDRESS from_address, uint8_t* header, uint8_t* data);

#endif /* TWI_MASTER_H_ */