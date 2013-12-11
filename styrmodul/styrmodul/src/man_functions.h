/*
 * man_functions.h
 *
 * Latest update: 2013-12-11
 * Version: 1.0
 * Authors: Viktoria Nowén, Kim Persson
 *
 * 
 * Innehåller funktioner för att kunna styra roboten i manuellt läge, 
 * samt funktionen manual_action som utför dessa styrningar.
 * Vissa av funktionerna används även i det autonoma läget,
 * bland annat roteringar i 90 grader.
 *
 */ 

// Sätter PWM rätt
void pwm_start_L(void);
void pwm_start_R(void);
void pwm_start_G(void);

void forward(int speed_);
void backward(int speed_);
void rotate_left(void);
void rotate_right(void);
void rotate_left90(int speed);
void rotate_right90(int speed);
void forward_right(void);
void forward_left(void);
void stop(void);
void grip_on(void);
void grip_off(void);
void grip(void);
void manual_action(uint8_t control_command);

volatile uint8_t keep_turning = 0;	// Nollställs när ett avbrott för roteringar kommer

// Starta PWM för höger hjulpar
void pwm_start_L() 
{
	OCR1AL = 0x00;		// Pulsbredd 0, motorerna står stilla
	OCR1AH = 0;			// Jämför med 0 och pulsbredd
	TCCR1B = 1;			// ingen förskalning på timern
}

// Starta PWM för vänster hjulpar
void pwm_start_R() 
{
	OCR1BL = 0x00;	// Pulsbredd 0, motorerna står stilla
	OCR1BH = 0;		// Jämför med 0 och pulsbredd
	TCCR1B = 1;		// ingen förskalning på timern
}

// Starta PWM för griparm
void pwm_start_G() 
{
	OCR3AL = 0x0E;	//Ställ griparmen i öppet läge
	OCR3AH = 0;		// Jämför med 0 och pulsbredd
	TCCR3B = 5;		// skala ner timern till 1/8
}

// Kör framåt med en hastighet
void forward(int speed_)
{
	PORTB = 0x03;		// Hjulens rotationsriktning är framåt
	OCR1BL = speed_;	// Höger sida
	OCR1AL = speed_;	// Vänster sida
}

// Kör bakåt med en hastighet
void backward(int speed_)
{
	PORTB = 0x00;		// Hjulens rotationsriktning är bakåt
	OCR1BL = speed_;	// Höger sida
	OCR1AL = speed_;	// Vänster sida
}

// Svänger åt vänster genom att höger sida går snabbare
void forward_left() 
{
	PORTB = 0x03;		// Hjulens rotationsriktning är framåt
	OCR1BL = 0xF8;		// Höger sida
	OCR1AL = 0x20;		// Vänster sida
}

// Svänger åt höger genom att vänster sida går snabbare
void forward_right()
{
	PORTB = 0x03;		// Hjulens rotationsriktning är framåt
	OCR1BL = 0x20;		// Höger sida
	OCR1AL = 0xF8;		// Vänster sida
}

// Roterar 90 grader moturs med en hastighet med hjälp av vinkelsensor
void rotate_left90(int speed_)
{
	PORTB = 0x08;		// Skicka avbrott till sensormodulen
	_delay_ms(1);		// Vänta
	PORTB = 0x01;		// Sluta skicka avbrott, ställ in hjulens rotationsriktning
	keep_turning = 1;	// Aktivera roteringen
	
	if(switch_ == 0)	// I autonomt läge skickas styrsignal
	{
		send(0x01, 0x05);
	}
	
	while(keep_turning == 1)	// Rotera tills avbrott 
	{
		OCR1BL = speed_;		// Hastighet vänster sida
		OCR1AL = speed_;		// Hastighet höger sida
	}
	stop();
	control_command = 0x06;		// Stop
}

// Roterar 90 grader medurs med en hastighet med hjälp av vinkelsensor
void rotate_right90(int speed_)
{
	PORTB = 0x08;		// Skicka avbrott till sensormodulen
	_delay_ms(1);		// Vänta
	PORTB = 0x02;		// Sluta skicka avbrott, ställ in hjulens rotationsriktning
	keep_turning = 1;	// Aktivera roteringen
	
	if(switch_ == 0)	// I autonomt läge skickas styrsignal
	{
		send(0x01, 0x04);
	}
		
	while(keep_turning == 1)	// Rotera tills avbrott
	{
		OCR1BL = speed_;		// Hastighet vänster sida
		OCR1AL = speed_;		// Hastighet höger sida
	}
	stop();
	control_command = 0x06;		// Stop
}

// Roterar moturs 
void rotate_left()
{
	PORTB = 0x01;	// Hjulens rotationsriktning
	OCR1BL = 0xF8;	// Vänster sida
	OCR1AL = 0xF8;	// Höger sida
}

// Roterar medurs 
void rotate_right()
{
	PORTB = 0x02;	// Hjulens rotationsriktning
	OCR1BL = 0xF8;	// Vänster sida
	OCR1AL = 0xF8;	// Höger sida
}

// Stannar
void stop()
{
	PORTB = 0x00;	// Hjulens rotationsriktning (stilla)
	OCR1BL = 0x00;	// Vänster sida
	OCR1AL = 0x00;	// Höger sida
}

 // Stänger griparmen så att roboten kan hålla i en festis
void grip_on()
{
	OCR3AL = 0x0A; 
	data_r = 0x06;
}

// Öppnar griparmen helt
void grip_off()		
{
	OCR3AL = 0x0E;
	data_r = 0x06;
}

// Kolla senaste styrkommandot från kommunikationsmodulen och utför kommandot
void manual_action(uint8_t control_command_) 
{
	switch (control_command_) // Kommandon från laptopen
	{
	case 0x00:
		forward(0x80);
		break;
	case 0x01:
		backward(0x80);
		break;
	case 0x02:
		forward_right();
		break;
	case 0x03:
		forward_left();
		break;
	case 0x04:
		rotate_right90(0xF8);
		break;
	case 0x05:
		rotate_left90(0xF8);
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


