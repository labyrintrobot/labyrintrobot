/*
 * communication.h
 *
 * Created: 11/27/2013 4:29:52 PM
 *  Authors: kimpe131, vikno623
 */ 
uint8_t control_command, left_short, right_short, left_long, right_long, forward_left, forward_right,
forward_center, tape, e;

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
		uint8_t header;
		uint8_t data;
		err = TWI_slave_receive_message(&header, &data);
		
		switch(header)
		{
		case styrkommando:
			control_command = data;
			break;
		case left_short_sensor:
			left_short = data;
			break;
		case left_long_sensor:
			left_long = data;
			break;
		case forward_left_sensor:
			forward_left = data;
			break;
		case forward_center_sensor:
			forward_center = data;
			break;
		case forward_right_sensor:
			forward_right = data;
			break;
		case right_long_sensor:
			right_long = data;
			break;
		case right_short_sensor:
			right_short = data;
			break;
		case reglerfel:
			e = data;
			break;
		case tejpmarkering:
			tape = data;
			break;
		}
	} 
	else 
	{
		PORTD = 0x00; // sluta skicka interrupt
		err = TWI_slave_send_message(header, data);
	}
	
	if(err)
	{
		//hbdfsn feeeel
	}
}