
#include <stdint.h>
#include "twi_slave_test.h"
#include "twi_slave.h"

const int TESTS = 50;

void f(void);

int TWI_slave_test_send() {
	int i;
	for (i = 0; i < TESTS; i++) {
		int err = TWI_slave_send_message(i, i + 1, f, f);
		if (err) {
			return err;
		}
	}
	
	return 0;
}

int TWI_slave_test_receive() {
	int i;
	for (i = 0; i < TESTS; i++) {
		uint8_t header;
		uint8_t data;
		int err = TWI_slave_receive_message(&header, &data);
		if (err) {
			return err;
		}
		if (header != i && data != i + 1) {
			return -1;
		}
	}
	
	return 0;
}

int TWI_slave_test_both() {
	int i;
	for (i = 0; i < TESTS; i++) {
		uint8_t header;
		uint8_t data;
		int err = TWI_slave_receive_message(&header, &data);
		if (err) {
			return err;
		}
		
		err = TWI_slave_send_message(header, data, f, f);
		if (err) {
			return err;
		}
	}
		
	return 0;
}