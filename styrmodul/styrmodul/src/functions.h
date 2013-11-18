#define PULSE_WIDTH_L 0x00
#define PULSE_WIDTH_R 0x00
#define PULSE_WIDTH_G 0x15


void pwm_start_L();
void pwm_start_R();
void pwm_start_G();

void forward();
void backward();
void rotate_right();
void rotate_left();
void forward_right();
void forward_left();
void stop();
void grip_on();
void grip_off();
void forward_regulated(signed e);
void start_sending();
void stop_sending();
signed int e_last = 0;

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

void forward()
{
	PORTB = 0x03;
	OCR1BL = 0xA0;
	OCR1AL = 0xA0;
}

void backward()
{
	PORTB = 0x00;
	OCR1BL = 0xA0;
	OCR1AL = 0xA0;
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

void rotate_right()
{
	PORTB = 0x02;
	OCR1BL = 0xA0;
	OCR1AL = 0xA0;
}

void rotate_left90()
{
	PORTB = 0x01;
	OCR1BL = 0xA0;
	OCR1AL = 0xA0;
	_delay_ms(560);
	stop();
}

void rotate_right90()
{
	PORTB = 0x02;
	OCR1BL = 0xA0;
	OCR1AL = 0xA0;
	_delay_ms(560);
	stop();
}

void rotate_left()
{
	PORTB = 0x01;
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

void forward_regulated(signed e)
{
	PORTB = 0x03;
	signed u, Kp, Kd;
	Kp = 2;
	Kd = 0.1; //KD=0.01/deltaT=0.1
	u = Kp*e + Kd*(e - e_last); //KD-regulator
	//u=Kp*e; //P-regulator
	e_last = e;
	
	if(u > 0) // turn right
	{
		OCR1BL = 0x80 - u; //right side
		OCR1AL = 0x80; // left side
	}
	else if(u < 0) // turn left
	{
		OCR1BL = 0x80; //right side
		OCR1AL = 0x80 + u; // left side
	}
	else if(u == 0) // don't turn
	{
		OCR1BL = 0x80; //right side
		OCR1AL = 0x80; // left side
	}
}

void start_sending()
{
	PORTD = 0x01; 
}
void stop_sending()
{
	PORTD = 0x00;
}