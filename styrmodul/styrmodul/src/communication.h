/*
 * communication.h
 *
 * Created: 11/27/2013 4:29:52 PM
 *  Authors: kimpe131, vikno623
 */ 


#include "twi_slave.h"

void start_sending(void);
void stop_sending(void);
void send(uint8_t h, uint8_t d);


int err;

enum header_t
{
	styrkommando = 0x00,
	left_short_sensor = 0x03,
	left_long_sensor = 0x04,
	forward_left_sensor = 0x05,
	forward_center_sensor = 0x06,
	forward_right_sensor = 0x07,
	right_long_sensor = 0x08,
	right_short_sensor = 0x09,
	regulation_error = 0x0A,
	tape_mark = 0x0B
};


void start_sending()	// TWI avbrott
{
	PORTD = 0x01;
}
void stop_sending()		// TWI avbrott
{
	PORTD = 0x00;
}


void send(uint8_t h, uint8_t d)
{
	header_s = h;
	data_s = d;
	start_sending();
}

ISR(TWI_vect)
{
	bool receive;
	err = TWI_slave_wait_for_address(&receive);
	if (receive) {
		err = TWI_slave_receive_message(&header_r, &data_r);
		
		switch(header_r)
		{
		case styrkommando:
			control_command = data_r;
			break;
		case left_short_sensor:
			left_short_s = data_r;
			break;
		case left_long_sensor:
			left_long_s = data_r;
			break;
		case forward_left_sensor:
			forward_left_s = data_r;
			break;
		case forward_center_sensor:
			forward_center_s = data_r;
			break;
		case forward_right_sensor:
			forward_right_s = data_r;
			break;
		case right_long_sensor:
			right_long_s = data_r;
			break;
		case right_short_sensor:
			right_short_s = data_r;
			break;
		case regulation_error:
			e_last = e;
			int8_t to_signed = data_r; // unsigned till signed
			e = to_signed;
			break;
		case tape_mark:
			tape_value = data_r;
			break;
		}
	} 
	else 
	{
		stop_sending(); // sluta skicka interrupt
		err = TWI_slave_send_message(header_s, data_s);
		
	}
	

}

