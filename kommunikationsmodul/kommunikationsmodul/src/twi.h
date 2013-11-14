/*
 * twi.h
 *
 * Created: 11/14/2013 5:47:55 PM
 *  Author: emibe709
 */ 


#ifndef TWI_H_
#define TWI_H_

#define TWI_START_STATUS 0x08
#define TWI_REP_START_STATUS 0x10
#define TWI_SLAW_ACK_STATUS 0x18
#define TWI_DATA_WRITE_ACK_STATUS 0x28
#define TWI_SLAR_ACK_STATUS 0x40
#define TWI_DATA_REC_ACK_STATUS 0x50

void TWI_receive_data(unsigned char* data);

int TWI_send_start(int repeated);

void TWI_send_stop(void);

int TWI_send_address(unsigned char address , int write);

int TWI_send_data(unsigned char data);

int TWI_master_send_message(unsigned char address, unsigned char header, unsigned char data);

int TWI_master_receive_message(unsigned char address , unsigned char *header , unsigned char *data);

int TWI_initialize_bitrate(int bitrate);

#endif /* TWI_H_ */