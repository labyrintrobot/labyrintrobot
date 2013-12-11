/*
 * twi_slave_test.c
 * TWI code for slave.
 * Version: 1.1
 *  Author: Emil Berg
 */

#include <stdint.h>
#include "twi_slave_test.h"
#include "twi_slave.h"
#include "twi_test_common.h"

int TWI_slave_test() {
	uint8_t header = 0;
	uint8_t data = 0;
	int i;
	for (i = 0; i < TWI_TESTS * 2; i++) {
		bool should_receive;
		int err = TWI_slave_wait_for_address(&should_receive);
		if (err) return err;
		
		if (err) return err;
		if (should_receive) {
			err = TWI_slave_receive_message(&header, &data);
		} else {
			err = TWI_slave_send_message(header, data);
		}
		if (err) return err;
	}
	
	return 0;
}