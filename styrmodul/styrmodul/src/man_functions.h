/*
 * man_functions.h
 *
 * Created: 11/20/2013
 * Authors: kimpe131, vikno623
 *
 * Denna fil borde vara en .c-fil
 * 
 * Innehåller funktioner för att kunna styra roboten i manuellt läge, 
 * samt funktionen manual_action som utför dessa styrningar.
 * Vissa av funktionerna används även i det autonoma läget,
 * bland annat roteringar i 90 grader.
 *
 */ 



// PWM
#define PULSE_WIDTH_L 0x00
#define PULSE_WIDTH_R 0x00
#define PULSE_WIDTH_G 0x0E

// Sätter PWM rätt
void pwm_start_L(void);
void pwm_start_R(void);
void pwm_start_G(void);

void forward(int speed_);
void backward(int speed_);
void rotate_left(void);
void rotate_right(void);
void rotate_left90(void);
void rotate_right90(void);
void forward_right(void);
void forward_left(void);
void stop(void);
void grip_on(void);
void grip_off(void);
void grip(void);
void manual_action(uint8_t control_command);



volatile uint8_t keep_turning = 0;


// Starta PWM för höger hjulpar
void pwm_start_L() 
{
	OCR1AL = PULSE_WIDTH_L; // Pulsbredd 0, motorerna står stilla
	OCR1AH = 0;				// Jämför med 0 och pulsbredd
	TCCR1B = 1;				// ingen förskalning på timern
}


// Starta PWM för vänster hjulpar
void pwm_start_R() 
{
	OCR1BL = PULSE_WIDTH_R; // Pulsbredd 0, motorerna står stilla
	OCR1BH = 0;             // Jämför med 0 och pulsbredd
	TCCR1B = 1;				// ingen förskalning på timern
}


// Starta PWM för griparm
void pwm_start_G() 
{
	OCR3AL = PULSE_WIDTH_G; //Ställ griparmen i öppet läge
	OCR3AH = 0;	// Jämför med 0 och pulsbredd
	TCCR3B = 5; // skala ner timern till 1/8
}

void forward(int speed_)
{
	PORTB = 0x03; 
	OCR1BL = speed_; // right side
	OCR1AL = speed_; // left side
/*	if(switch_ == 0)
	{
		send(0x01, 0x00);
	}*/
}

void backward(int speed_)
{
	PORTB = 0x00;
	OCR1BL = speed_; // right side
	OCR1AL = speed_; // left side
/*	if(switch_ == 0)
	{
		send(0x01, 0x01);
	}	*/
}

void forward_left() // int speed_
{
	PORTB = 0x03; 
	OCR1BL = 0xF0; // right side
	OCR1AL = 0x50; // left side
}

void forward_right()
{
	PORTB = 0x03;
	OCR1BL = 0x50; // right side
	OCR1AL = 0xF0; // left side
}


void rotate_left90()
{
	PORTB = 0x08;		// skicka avbrott till sensormodulen
	_delay_ms(1);		// vänta
	PORTB = 0x01;		// sluta skicka avbrott, ställ in hjulens rotationsriktning
	keep_turning = 1;	// aktivera roteringen
	
	if(switch_ == 0)
	{
		send(0x01, 0x05);
	}
	
	while(keep_turning == 1)	// rotera tills avbrott 
	{
		OCR1BL = 0x80;			// hastighet vänster sida
		OCR1AL = 0x80;			// hastighet höger sida
	}
	stop();
	control_command = 0x06; // stop
}

void rotate_right90()
{
	PORTB = 0x08;		// skicka avbrott till sensormodulen
	_delay_ms(1);		// vänta
	PORTB = 0x02;		// sluta skicka avbrott, ställ in hjulens rotationsriktning
	keep_turning = 1;	// aktivera roteringen
	
	if(switch_ == 0)
	{
		send(0x01, 0x04);
	}
		
	while(keep_turning == 1)	// rotera tills avbrott
	{
		OCR1BL = 0x80;			// hastighet vänster sida
		OCR1AL = 0x80;			// hastighet höger sida
	}
	stop();
	control_command = 0x06; // stop
}

void rotate_left()
{
	PORTB = 0x01;	// hjulens rotationsriktning
	OCR1BL = 0xF0;	// vänster sida
	OCR1AL = 0xF0;	// höger sida
}

void rotate_right()
{
	PORTB = 0x02;	// hjulens rotationsriktning
	OCR1BL = 0xF0;	// vänster sida
	OCR1AL = 0xF0;	// höger sida
}

void stop()
{
	PORTB = 0x00;	// hjulens rotationsriktning (stilla)
	OCR1BL = 0x00;	// vänster sida
	OCR1AL = 0x00;	// höger sida
/*	
	if(switch_ == 0)
	{
		send(0x01, 0x06);
	}*/
}

void grip_on() // stäng så att roboten kan hålla i en festis (lite för hårt nu)
{
	OCR3AL = 0x0A; // 0x12 ca 1.25ms, 0x11 1.2ms, 0x07 0.5ms
	data_r = 0x06;
}
void grip_off() // öppna helt
{
	OCR3AL = 0x0E; // 0x1B ca 1.9ms, 0x1D 2ms, 0x23 2.5ms
	data_r = 0x06;
}


void grip() // öppna och stänga griparmen beroende på grip_value
{
	if(grip_value == 0)
	{
		grip_on();
		data_r = 0x06;
		grip_value = 1;
	}	
	else
	{
		grip_off();
		data_r = 0x06;
		grip_value = 0;
	}
}



// Kolla senaste kontrollkommandot från kommunikationsmodulen och utför kommandot
void manual_action(uint8_t control_command_) 
{
	switch (control_command_) //Kommandon från laptopen
	{
	case 0x00:
		forward(0xF0);
		break;
	case 0x01:
		backward(0xF0);
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
	case 0x09:
		grip_off();
		break;
	case 0x0A:
		grip_on();
		break;	

	}
}


