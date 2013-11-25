/*
 * twi_test.c
 *
 * Created: 11/25/2013 10:09:26 AM
 *  Author: emibe709
 */

#include <asf.h>
#include "twi_test.h"
#include "twi_master.h"

const int TESTS = 50;

int TWI_test_send() {
	int i;
	for (i = 0; i < TESTS; i++) {
		int err = TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS, i, i + 1);
		if (err) {
			PORTA = TWSR;
			return err;
		}
	}
	
	return 0;
}

int TWI_test_receive() {
	uint8_t header;
	uint8_t data;
	int i;
	for (i = 0; i < TESTS; i++) {
		int err = TWI_master_receive_message(TWI_CONTROL_MODULE_ADDRESS, &header, &data);
		if (err) {
			PORTA = TWSR;
			return err;
		}
		if (header != i || header != i + 1) {
			return -1;
		}
	}
	
	return 0;
}

int TWI_test_both() {
	uint8_t header;
	uint8_t data;
	int i;
	for (i = 0; i < TESTS; i++) {
		int err = TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS, i, i + 1);
		if (err) {
			PORTA = TWSR;
			return err;
		}
		err = TWI_master_receive_message(TWI_CONTROL_MODULE_ADDRESS, &header, &data);
		if (err) {
			PORTA = TWSR;
			return err;
		}
		if (header != i || header != i + 1) {
			return -1;
		}
	}
	
	return 0;
}