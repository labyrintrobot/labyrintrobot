﻿/*
 * communication.h
 *
 * Created: 11/27/2013 4:29:52 PM
 *  Authors: kimpe131, vikno623
 */ 


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
	reglerfel = 0x0A,
	tejpmarkering = 0x0B
};

ISR(TWI_vect)
{
	bool receive;
	int err = TWI_slave_wait_for_address(&receive);
	if (receive) {
		err = TWI_slave_receive_message(&header, &data);
		
		/*switch(header)
		{
		case styrkommando:
			control_command = data;
			break;
		case left_short_sensor:
			left_short_s = data;
			break;
		case left_long_sensor:
			left_long_s = data;
			break;
		case forward_left_sensor:
			forward_left_s = data;
			break;
		case forward_center_sensor:
			forward_center_s = data;
			break;
		case forward_right_sensor:
			forward_right_s = data;
			break;
		case right_long_sensor:
			right_long_s = data;
			break;
		case right_short_sensor:
			right_short_s = data;
			break;
		case reglerfel:
			e = data;
			break;
		case tejpmarkering:
			tape = data;
			break;
		}*/
	} 
	else 
	{
		PORTD = 0x00; // sluta skicka interrupt
		err = TWI_slave_send_message(header, data);
	}
	
	//if(err)
	//{
		//hbdfsn feeeel
	//}
}

void fun() {
	header = d;
	data = d;
	// Skicka interrupt
}