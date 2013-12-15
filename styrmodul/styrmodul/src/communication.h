/*
 * communication.h
 *
 * Latest update: 2013-12-11
 * Version: 1.0
 * Authors: Viktoria Nowén, Kim Persson
 *
 * Innehåller funktioner för att kunna kommunicera med kommunikationsmodulen
 *
 */ 


#include "twi_slave.h"

void start_sending(void);
void stop_sending(void);
void send(uint8_t h, uint8_t d);

int err;	// Felmeddelande

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
	tape_mark = 0x0B,
	P_lsb = 0x0F,
	D_lsb = 0x11,
    reg_speed = 0x12
};


void start_sending()	// TWI avbrott
{
	PORTD = 0x01;
}
void stop_sending()		// TWI avbrott
{
	PORTD = 0x00;
}

// Skickar header och data
void send(uint8_t h, uint8_t d)
{
	header_s = h;
	data_s = d;
	start_sending();
}

// Tar emot kommandon, sensordata och reglerparameter
ISR(TWI_vect)
{
	bool receive; // Sann om den ska ta emot
	err = TWI_slave_wait_for_address(&receive);
	// Styrmodulen tar emot
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
			int8_t to_signed = data_r;		// Unsigned till signed
			e = to_signed;
			break;
		case tape_mark:
			tape_value = data_r;
			break;
		case P_lsb:
			Kp_lsb = data_r;
			break;
		case D_lsb:
			Kd_lsb = data_r;
			break;
		case reg_speed:
			speed = data_r;
			break;
		}
	} 
	// Styrmodulen skickar 
	else 
	{
		stop_sending();		// Sluta skicka interrupt
		err = TWI_slave_send_message(header_s, data_s);
	}
}

