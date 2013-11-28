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

void pwm_start_L() // Starta PWM för höger hjulpar
{
	OCR1AL = PULSE_WIDTH_L; // Pulsbredd 0, motorerna står stilla
	OCR1AH = 0;				// Jämför med 0 och pulsbredd
	TCCR1B = 1;				// ingen förskalning på timern
}

void pwm_start_R() // Starta PWM för vänster hjulpar
{
	OCR1BL = PULSE_WIDTH_R; // Pulsbredd 0, motorerna står stilla
	OCR1BH = 0;             // Jämför med 0 och pulsbredd
	TCCR1B = 1;				// ingen förskalning på timern
}

void pwm_start_G() // Starta PWM för griparm
{
	OCR3AL = PULSE_WIDTH_G; //Ställ griparmen i öppet läge
	OCR3AH = 0;	// Jämför med 0 och pulsbredd
	TCCR3B = 3; // skala ner timern till 1/8
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
	OCR1BL = 0xA0; // right side
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
	PORTB = 0x08;		// skicka avbrott till sensormodulen
	_delay_ms(1);		// vänta
	PORTB = 0x01;		// sluta skicka avbrott, ställ in hjulens rotationsriktning
	keep_turning = 1;	// aktivera roteringen

	while(keep_turning == 1)	// rotera tills avbrott 
	{
		OCR1BL = 0xA0;			// hastighet vänster sida
		OCR1AL = 0xA0;			// hastighet höger sida
	}
	stop();
}

void rotate_right90()
{
	PORTB = 0x08;		// skicka avbrott till sensormodulen
	_delay_ms(1);		// vänta
	PORTB = 0x02;		// sluta skicka avbrott, ställ in hjulens rotationsriktning
	keep_turning = 1;	// aktivera roteringen
	
	while(keep_turning == 1)	// rotera tills avbrott
	{
		OCR1BL = 0xA0;			// hastighet vänster sida
		OCR1AL = 0xA0;			// hastighet höger sida
	}
	stop();
}

void rotate_left()
{
	PORTB = 0x01;	// hjulens rotationsriktning
	OCR1BL = 0xA0;	// vänster sida
	OCR1AL = 0xA0;	// höger sida
}

void rotate_right()
{
	PORTB = 0x02;	// hjulens rotationsriktning
	OCR1BL = 0xA0;	// vänster sida
	OCR1AL = 0xA0;	// höger sida
}

void stop()
{
	PORTB = 0x00;	// hjulens rotationsriktning (stilla)
	OCR1BL = 0x00;	// vänster sida
	OCR1AL = 0x00;	// höger sida
}

void grip_on()
{
	OCR3AL = 0x10; // 0x12 ca 1.25ms, 0x11 1.2ms, 0x07 0.5ms
}
void grip_off()
{
	OCR3AL = 0x1B; // 0x1B ca 1.9ms, 0x1D 2ms, 0x23 2.5ms
}


void start_sending()	// TWI avbrott
{
	PORTD = 0x01; 
}
void stop_sending()		// TWI avbrott
{
	PORTD = 0x00;
}

void manual_action(uint8_t control_command) //Kolla senaste kontrollkommandot från kommunikationsmodulen och utför kommandot
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


