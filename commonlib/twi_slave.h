/*
 * twi_slave.h
 *
 *  Author: Emil Berg, Kristoffer Borg
 */

#ifndef TWI_SLAVE_H_
#define TWI_SLAVE_H_

#include "twi_common.h"

int TWI_slave_send_message(uint8_t header, uint8_t data, void (*start_sending_irq_fn)(void), void (*stop_sending_irq_fn)(void));

int TWI_slave_receive_message(uint8_t* header, uint8_t* data);

#endif /* TWI_SLAVE_H_ */