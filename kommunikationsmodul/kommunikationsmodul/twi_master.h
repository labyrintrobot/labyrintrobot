/*
 * twi_master.h
 *
 * Created: 11/14/2013 6:00:15 PM
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#ifndef TWI_MASTER_H_
#define TWI_MASTER_H_

int TWI_initialize_bitrate(int bitrate);

int TWI_master_send_message(unsigned char address, unsigned char header, unsigned char data);

int TWI_master_receive_message(unsigned char address, unsigned char* header, unsigned char* data);

#endif /* TWI_MASTER_H_ */