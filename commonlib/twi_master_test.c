/*
 * twi_master_test.c
 *
 * Created: 11/25/2013 10:09:26 AM
 *  Author: emibe709
 */

#include <asf.h>
#include "twi_master_test.h"
#include "twi_master.h"
#include "twi_test_common.h"

int TWI_master_test_send() {
	int i;
	for (i = 0; i < TWI_TESTS; i++) {
		int err = TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS, i, i + 1);
		if (err) {
			PORTA = TWSR;
			return err;
		}
	}
	
	return 0;
}

int TWI_master_test_receive() {
	int i;
	for (i = 0; i < TWI_TESTS; i++) {
		uint8_t header;
		uint8_t data;
		int err = TWI_master_receive_message(TWI_CONTROL_MODULE_ADDRESS, &header, &data);
		if (err) {
			PORTA = TWSR;
			return err;
		}
	}
	
	return 0;
}

int TWI_master_test_both() {
	int i;
	for (i = 0; i < TWI_TESTS; i++) {
		int err = TWI_master_send_message(TWI_CONTROL_MODULE_ADDRESS, i, i + 1);
		if (err) {
			PORTA = TWSR;
			return err;
		}
		uint8_t header;
		uint8_t data;
		err = TWI_master_receive_message(TWI_CONTROL_MODULE_ADDRESS, &header, &data);
		if (err) {
			PORTA = TWSR;
			return err;
		}
		if (header != i || data != i + 1) {
			return 0b11100111;
		}
	}
	
	return 0;
}