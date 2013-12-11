/*
 * man_functions.h
 *
 * Latest update: 2013-12-11
 * Version: 1.0
 * Authors: Viktoria Now�n, Kim Persson
 *
 * 
 * Inneh�ller funktioner f�r att kunna styra roboten i manuellt l�ge, 
 * samt funktionen manual_action som utf�r dessa styrningar.
 * Vissa av funktionerna anv�nds �ven i det autonoma l�get,
 * bland annat roteringar i 90 grader.
 *
 */ 

// S�tter PWM r�tt
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

volatile uint8_t keep_turning = 0;	// Nollst�lls n�r ett avbrott f�r roteringar kommer

// Starta PWM f�r h�ger hjulpar
void pwm_start_L() 
{
	OCR1AL = 0x00;		// Pulsbredd 0, motorerna st�r stilla
	OCR1AH = 0;			// J�mf�r med 0 och pulsbredd
	TCCR1B = 1;			// ingen f�rskalning p� timern
}

// Starta PWM f�r v�nster hjulpar
void pwm_start_R() 
{
	OCR1BL = 0x00;	// Pulsbredd 0, motorerna st�r stilla
	OCR1BH = 0;		// J�mf�r med 0 och pulsbredd
	TCCR1B = 1;		// ingen f�rskalning p� timern
}

// Starta PWM f�r griparm
void pwm_start_G() 
{
	OCR3AL = 0x0E;	//St�ll griparmen i �ppet l�ge
	OCR3AH = 0;		// J�mf�r med 0 och pulsbredd
	TCCR3B = 5;		// skala ner timern till 1/8
}

// K�r fram�t med en hastighet
void forward(int speed_)
{
	PORTB = 0x03;		// Hjulens rotationsriktning �r fram�t
	OCR1BL = speed_;	// H�ger sida
	OCR1AL = speed_;	// V�nster sida
}

// K�r bak�t med en hastighet
void backward(int speed_)
{
	PORTB = 0x00;		// Hjulens rotationsriktning �r bak�t
	OCR1BL = speed_;	// H�ger sida
	OCR1AL = speed_;	// V�nster sida
}

// Sv�nger �t v�nster genom att h�ger sida g�r snabbare
void forward_left() 
{
	PORTB = 0x03;		// Hjulens rotationsriktning �r fram�t
	OCR1BL = 0xF8;		// H�ger sida
	OCR1AL = 0x20;		// V�nster sida
}

// Sv�nger �t h�ger genom att v�nster sida g�r snabbare
void forward_right()
{
	PORTB = 0x03;		// Hjulens rotationsriktning �r fram�t
	OCR1BL = 0x20;		// H�ger sida
	OCR1AL = 0xF8;		// V�nster sida
}

// Roterar 90 grader moturs med en hastighet med hj�lp av vinkelsensor
void rotate_left90(int speed_)
{
	PORTB = 0x08;		// Skicka avbrott till sensormodulen
	_delay_ms(1);		// V�nta
	PORTB = 0x01;		// Sluta skicka avbrott, st�ll in hjulens rotationsriktning
	keep_turning = 1;	// Aktivera roteringen
	
	if(switch_ == 0)	// I autonomt l�ge skickas styrsignal
	{
		send(0x01, 0x05);
	}
	
	while(keep_turning == 1)	// Rotera tills avbrott 
	{
		OCR1BL = speed_;		// Hastighet v�nster sida
		OCR1AL = speed_;		// Hastighet h�ger sida
	}
	stop();
	control_command = 0x06;		// Stop
}

// Roterar 90 grader medurs med en hastighet med hj�lp av vinkelsensor
void rotate_right90(int speed_)
{
	PORTB = 0x08;		// Skicka avbrott till sensormodulen
	_delay_ms(1);		// V�nta
	PORTB = 0x02;		// Sluta skicka avbrott, st�ll in hjulens rotationsriktning
	keep_turning = 1;	// Aktivera roteringen
	
	if(switch_ == 0)	// I autonomt l�ge skickas styrsignal
	{
		send(0x01, 0x04);
	}
		
	while(keep_turning == 1)	// Rotera tills avbrott
	{
		OCR1BL = speed_;		// Hastighet v�nster sida
		OCR1AL = speed_;		// Hastighet h�ger sida
	}
	stop();
	control_command = 0x06;		// Stop
}

// Roterar moturs 
void rotate_left()
{
	PORTB = 0x01;	// Hjulens rotationsriktning
	OCR1BL = 0xF8;	// V�nster sida
	OCR1AL = 0xF8;	// H�ger sida
}

// Roterar medurs 
void rotate_right()
{
	PORTB = 0x02;	// Hjulens rotationsriktning
	OCR1BL = 0xF8;	// V�nster sida
	OCR1AL = 0xF8;	// H�ger sida
}

// Stannar
void stop()
{
	PORTB = 0x00;	// Hjulens rotationsriktning (stilla)
	OCR1BL = 0x00;	// V�nster sida
	OCR1AL = 0x00;	// H�ger sida
}

 // St�nger griparmen s� att roboten kan h�lla i en festis
void grip_on()
{
	OCR3AL = 0x0A; 
	data_r = 0x06;
}

// �ppnar griparmen helt
void grip_off()		
{
	OCR3AL = 0x0E;
	data_r = 0x06;
}

// Kolla senaste styrkommandot fr�n kommunikationsmodulen och utf�r kommandot
void manual_action(uint8_t control_command_) 
{
	switch (control_command_) // Kommandon fr�n laptopen
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


