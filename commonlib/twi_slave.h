/*
 * twi_slave.h
 *
 *  Author: Emil Berg, Kristoffer Borg
 */

#ifndef TWI_SLAVE_H_
#define TWI_SLAVE_H_

int TWI_slave_send_message(uint8_t header, uint8_t data);

int TWI_slave_receive_message(uint8_t* header, uint8_t* data);

#endif /* TWI_SLAVE_H_ */