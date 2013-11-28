#define PULSE_WIDTH_L 0x00
#define PULSE_WIDTH_R 0x00
#define PULSE_WIDTH_G 0x15


void pwm_start_L(void);
void pwm_start_R(void);
void pwm_start_G(void);

void forward(int speed);
void backward(int speed);
void rotate_left(void);
void rotate_right(void);
void rotate_left90(void);
void rotate_right90(void);
void forward_right(void);
void forward_left(void);
void stop(void);
void grip_on(void);
void grip_off(void);
void start_sending(void);
void stop_sending(void);
void manual_action(uint8_t control_command);
signed int e_last = 0;
uint8_t keep_turning = 0;

void pwm_start_L()
{
	OCR1AL = PULSE_WIDTH_L;
	OCR1AH = 0;
	TCCR1B = 1;
}

void pwm_start_R()
{
	OCR1BL = PULSE_WIDTH_R;
	OCR1BH = 0;
	TCCR1B = 1;
}

void pwm_start_G()
{
	OCR3AL = PULSE_WIDTH_G;
	OCR3AH = 0;
	TCCR3B = 3;
}

void forward(int speed)
{
	PORTB = 0x03;
	OCR1BL = speed;
	OCR1AL = speed;
}

void backward(int speed)
{
	PORTB = 0x00;
	OCR1BL = speed;
	OCR1AL = speed;
}

void forward_left()
{
	PORTB = 0x03;
	OCR1BL = 0xA0; //right side
	OCR1AL = 0x30; // left side
}

void forward_right()
{
	PORTB = 0x03;
	OCR1BL = 0x30;
	OCR1AL = 0xA0;
}


void rotate_left90()
{
	PORTB = 0x08;
	_delay_ms(1);
	PORTB = 0x01;
	keep_turning = 1;

	while(keep_turning == 1)
	{
		OCR1BL = 0xA0;
		OCR1AL = 0xA0;
	}
	stop();
}

void rotate_right90()
{
	PORTB = 0x08;
	_delay_ms(1);
	PORTB = 0x02;
	keep_turning = 1;
	while(keep_turning == 1)
	{
		OCR1BL = 0xA0;
		OCR1AL = 0xA0;
	}
	stop();
}

void rotate_left()
{
	PORTB = 0x01;
	OCR1BL = 0xA0;
	OCR1AL = 0xA0;
}

void rotate_right()
{
	PORTB = 0x02;
	OCR1BL = 0xA0;
	OCR1AL = 0xA0;
}

void stop()
{
	PORTB = 0x00;
	OCR1BL = 0x00;
	OCR1AL = 0x00;
}

void grip_on()
{
	OCR3AL = 0x10; // 0x12 ca 1.25ms, 0x11 1.2ms 0x07 0.5 ms
}
void grip_off()
{
	OCR3AL = 0x1B; // 0x1B ca 1.9ms, 0x1D 2 ms 0x23 2.5 ms
}


void start_sending()
{
	PORTD = 0x01; 
}
void stop_sending()
{
	PORTD = 0x00;
}

void manual_action(uint8_t control_command)
{
	switch (control_command) //Kommandon från laptopen
	{
	case 0x00:
		forward(0xA0);
		break;
	case 0x01:
		backward(0xA0);
		break;
	case 0x02:
		forward_right();
		break;
	case 0x03:
		forward_left();
		break;
	case 0x04:
		rotate_right90();
		break;
	case 0x05:
		rotate_left90();
		break;
	case 0x06:
		stop();
		break;
	case 0x07:
		rotate_right();
		break;
	case 0x08:
		rotate_left();
		break;
	}
}


