/*
 * auto_functions.h
 *
 * Latest update: 2013-12-11
 * Version: 1.0
 * Authors: Viktoria Nowén, Kim Persson
 *
 * Innehåller funktioner för att roboten ska kunna gå i autonomt läge.
 * Använder funktionen send för att skicka styrsignaler.
 *
 */ 

#define turn_left 0x00;
#define go_forward 0x01;
#define turn_right 0x02;
#define stay 0x03;

bool inside_corridor(void);
bool intersection_stop(void);
void get_into_intersection(void);
bool intersection_detected(void);
int unmarked_intersection_choice(void);
int marked_intersection_choice(void);
void forward_regulated(void);
void find_start(void);
void find_goal(void);
bool goal_detected(void);
void goal_regulated(void);
void get_target(void);
bool start_detected(void);

void turn(int direction);
void return_to_start(void);
void turn_back(int direction);


int	direction_array[50];	// En array med sparade vägval
int	i = 0;					// Indexering för arrayen

// Returnera true när roboten kommit in i en korridor
bool inside_corridor()
{
	if(left_long_s < 60 && right_long_s < 60)
		return true;
	else
		return false;
}

// Returnera true när roboten befinner sig mitt i 80x80-biten i en sväng eller en korsning med en vägg rakt fram.
bool intersection_stop()
{
	if(forward_center_s < 31)
		return true;
	else
		return false;
}

// Kör in till mitten av korsningen, om det är en vägg framför: kör på avståndet, om det saknas vägg: kör på tid
void get_into_intersection() 
{
	//send(0x01, 0x00);
	forward(0x50);					// Kör framåt
	_delay_ms(450);					// En viss tid

	if(forward_right_s < 80 && forward_left_s < 80)		// Om det är en vägg framför
	{
		while(!intersection_stop()) // Vänta tills det är 30 cm kvar till väggen
		{
			forward(0x50);
		}
		//send(0x01, 0x06);			// Skicka att den stannat
		stop();						// Stanna
	}
	else //Annars om det inte är en vägg framför
	{
		_delay_ms(400);				// En viss tid
		//send(0x01, 0x06);			// Skicka att den stannat
		stop();						// Stanna
	}
}

// Returnerar True när den hittat en korsning eller sväng
bool intersection_detected()
{
	return ((left_long_s > 110) || (right_long_s > 110));
}

// Svänger vänster som förstaval
int unmarked_intersection_choice()
{
	if(left_long_s > 90)			// Vänster håll öppet
	{
		return turn_left;
	}
	else if(forward_right_s > 90)	// Framåt
	{
		return go_forward;
	}
	else if(right_long_s > 90)		// Höger öppet
	{
		return turn_right;
	}
	else
	{
		return stay;
	}
}

// Väljer att svänga beroende på tejpmarkering
int marked_intersection_choice()
{
	// tape: 0x00 = right, tape: 0x01 = left
	if((tape_value == 0x00) & (right_long_s < 80))
	{
		tape_value = 0x04; // "Nollställ"
		return go_forward;
	}
	else if((tape_value == 0x00) & (forward_right_s < 80))
	{
		tape_value = 0x04; // "Nollställ"
		return turn_right;
	}
	else if((tape_value == 0x01) & (left_long_s < 80))
	{
		tape_value = 0x04;// "Nollställ"
		return go_forward;
	}
	else
	{
		tape_value = 0x04; // "Nollställ"
		return turn_left;
	}
}

// Kör framåt i en korridor (labyrinten) med hjälp av PD-reglering
void forward_regulated()
{
	if(forward_left_s < 30 && forward_right_s < 30) // Ifall det kommer en återvändsgränd
	{
		//send(0x01, 0x04);
		rotate_right90(0x90);
		_delay_ms(500);
		//send(0x01, 0x04);
		rotate_right90(0x90);
		_delay_ms(500);
	}
	
	else if(forward_right_s < 30 && forward_left_s > 80) // Avsmalning åt vänster, kör in till vänster vägg
	{
		//send(0x01, 0x06);
		stop();
		//send(0x01, 0x05);
		rotate_left90(0x80);
		_delay_ms(250);
		//send(0x01, 0x00);
		while(forward_center_s > 13)
		{
			forward(0x50);
		}
		//send(0x01, 0x04);
		rotate_right90(0x80);
		_delay_ms(250);
		//send(0x01, 0x00);
		while(right_short_s > 25)
		{
			forward(0x80);
		}
	}
	
	else if(forward_right_s > 80 && forward_left_s < 30) // Avsmalning åt höger, kör in till höger vägg
	{
		//send(0x01, 0x06);
		stop();
		//send(0x01, 0x04);
		rotate_right90(0x80);
		_delay_ms(250);
		//send(0x01, 0x00);
		while(forward_center_s > 13)
		{
			forward(0x50);
		}
		//send(0x01, 0x05);
		rotate_left90(0x80);
		_delay_ms(250);
		//send(0x01, 0x00);
		while(left_short_s > 25)
		{
			forward(0x80);
		}
	}
	else // Reglera som vanligt i korridor
	{
		
		PORTB = 0x03;
		float u, Kp, Kd, P, D;
		Kp = Kp_lsb;
		Kd = Kd_lsb;
		P = Kp * (float)e; 
		P = P / 10.0; // Beräkna P-del
		D = Kd * (float)(e - e_last); // Beräkna D-del
		u = P + D;
		
		if(u > 56) // Max-värde 0x40
		{
			u = 56;
		}
		if(u < -56) // Min-värde
		{
			u = -56;
		}
		
		// Reglera beroende på u
		if((int)u > 0) // Styr åt höger
		{
			OCR1BL = (speed - u);	// Höger sida
			OCR1AL = (speed + u);	// Vänster sida
		}
		else if((int)u < 0) // Styr åt vänster
		{
			OCR1BL = (speed - u);	// Höger sida
			OCR1AL = (speed + u);	// Vänster sida
		}
		
		else // Styr rakt fram
		{
			OCR1BL = speed; // Höger sida
			OCR1AL = speed; // Vänster sida
		}
	}
}

// Returnerar True när den hittat startmarkeringen
// START = Två tjocka tejpmarkeringar (TT), tape = 0x02
bool start_detected()
{
	if(tape_value == 0x02)
	{
		tape_value = 0x04; // "Nollställ" senaste tejpavläsningen
		return true;
	}
	else
		return false;
}

// Kör framåt tills startmarkeringen hittas
void find_start()
{	
	//send(0x01, 0x00);
	while(!start_detected())
	{	if(forward_left_s > 40 && forward_right_s > 40)
		{
			forward(0x80);
		}
		else
			stop();
	}
}

// I svängar och korsningar, använder sig av direction: turn_left = 0x00, go_forward = 0x01, turn_right = 0x02
void turn(int direction)
{
	switch(direction){
		case 0x00: // turn_left
			//send(0x01, 0x05);
			rotate_left90(0x80);
			_delay_ms(250);
			//send(0x01, 0x06);
			stop();
			//send(0x01, 0x00);
			while(!inside_corridor()) // Kör ut ur korsningen
			{
				forward(0x80);
			}
			_delay_ms(350);
			e = 0;
			e_last = 0;
			//send(0x01, 0x06);
			stop();
			break;
		case 0x01: // go_forward
			//send(0x01, 0x00);
			while(!inside_corridor()) // Kör ut ur korsningen
			{
				forward(0x80);
			}
			_delay_ms(350);
			e = 0;
			e_last = 0;
			//send(0x01, 0x06);
			stop();
			break;
		case 0x02: // turn_right
			//send(0x01, 0x04);
			rotate_right90(0x80);
			_delay_ms(250);
			//send(0x01, 0x06);
			stop();
			//send(0x01, 0x00);			
			while(!inside_corridor()) // Kör ut ur korsningen
			{
				forward(0x80);
			}
			_delay_ms(350);
			e = 0;
			e_last = 0;			
			//send(0x01, 0x06);
			stop();
			break;
		case 0x03: // stay
			//send(0x01, 0x06);
			stop();
			break;
	}
}

// I svängar och korsningar på väg tillbaka, när målet hämtats
void turn_back(int direction)
{
	switch(direction){
		case 0x00: //turn_left
			//send(0x01, 0x04);
			rotate_right90(0x80);
			_delay_ms(250);
			//send(0x01, 0x06);
			stop();
			//send(0x01, 0x00);
			while(!inside_corridor()) // Kör ut ur korsningen
			{
				forward(0x80);
			}
			_delay_ms(350);
			e = 0;
			e_last = 0;			
			//send(0x01, 0x06);
			stop();
			break;
		case 0x01: // go_forward
			//send(0x01, 0x00);
			while(!inside_corridor()) // Kör ut ur korsningen
			{
				forward(0x80);
			}
			_delay_ms(350);
			e = 0;
			e_last = 0;			
			//send(0x01, 0x06);
			stop();
			break;
		case 0x02: // turn_right
			//send(0x01, 0x05);
			rotate_left90(0x80);
			_delay_ms(250);
			//send(0x01, 0x06);
			stop();
			//send(0x01, 0x00);
			while(!inside_corridor()) // Kör ut ur korsningen
			{
				forward(0x80);
			}
			_delay_ms(350);
			e = 0;
			e_last = 0;			
			//send(0x01, 0x06);
			stop();
			break;
		case 0x03:	// stay
			//send(0x01, 0x06);
			stop();
			break;
	}
}

// Returnerar True när den hittat målmarkeringen
bool goal_detected()
{
	return (tape_value == 0x03);
}

// Reglerar fram till målet längs en linje
void goal_regulated()
{
	PORTB = 0x03;
	float u, Kp, Kd, P, D;
	uint8_t u_max = 0x40;
	Kp = 5;
	Kd = 10;
	P = Kp*(float)e;
	P = P/10.0;
	D = Kd * (float)(e - e_last);
	u = P + D;
	
	if(u > u_max)		// Max-värde 0x40
	u = u_max;
	if(u < -u_max)		// Min-värde
	u = -u_max;
	
	uint8_t goal_speed = 0x50;
	// Reglera beroende på u
	if( (int) u > 0) // Turn right
	{
		OCR1BL = (goal_speed - u);		// Right side
		OCR1AL = (goal_speed + u);		// left side
	}
	else if( (int) u < 0) // Turn left
	{
		OCR1BL = (goal_speed - u);	// Right side
		OCR1AL = (goal_speed + u);	// left side
	}
	else // Don't turn, keep going
	{
		OCR1BL = goal_speed;	// Right side
		OCR1AL = goal_speed;	// Left side
	}
}

// Kör tills den hittar målet
void find_goal()
{
	int direction;	// Vägval, fås från funktionerna marked_intersection_choice och unmarked_intersection_choice
	while(!goal_detected())
	{
		//send(0x01, 0x00);
		while(!intersection_detected() && !goal_detected()) // Korridor
		{
			forward_regulated();
		}
		// Kommit till en korsning eller sväng
		if(tape_value == 0x00 || tape_value == 0x01) // Markerad korsning, håll höger eller vänster
		{
			get_into_intersection();
			direction = marked_intersection_choice();
			turn(direction);
			direction_array[i] = direction;		// Sparar det senaste vägvalet för att kunna köra tillbaka
			i++;
		}
		else if(tape_value == 0x04) // Omarkerad korsning
		{
			get_into_intersection();
			direction = unmarked_intersection_choice();
			turn(direction);
			direction_array[i] = direction;		// Sparar det senaste vägvalet för att kunna köra tillbaka
			i++;
		}
	}
	tape_value = 0x04; // "Nollställ" tejpmarkeringen
	
	// Hittat målmarkeringen!
	//send(0x01, 0x06);
	stop();
	_delay_ms(500);
	
}

// Efter att roboten hittat målmarkeringen (i find_goal())
// reglerar den fram, tar föremålet och vänder sig om
void get_target()
{
	uint8_t grip_switch = 4;	// 4 = griparmsswitchen orörd
	// Kör sakta fram till föremålet längs tejpmarkeringen
	while(grip_switch == 4)
	{
		grip_switch = PINA & 0x04;	// Kollar switchen på griparmen
		goal_regulated();			// Reglerar längs linjen fram till föremålet
	}
	// Framme vid föremålet
	//send(0x01, 0x06);
	stop();
	//send(0x01, 0x0A);
	
	grip_on();
	_delay_ms(500);
	//send(0x01, 0x01);
	backward(0x80);
	_delay_ms(500);
	//send(0x01, 0x06);
	stop();
	//send(0x01, 0x04);
	rotate_left90(0x90);
	_delay_ms(250);
	//send(0x01, 0x04);
	rotate_left90(0x90);
	_delay_ms(500);
	
}

// Går igenom direction_array för att gå tillbaka till startpositionen i labyrinten
void return_to_start()
{
	for(int x = i-1; x >= 0; x--)
	{
		//send(0x01, 0x00);
		while(!intersection_detected())
		{
			forward_regulated();
		}
		get_into_intersection();
		turn_back(direction_array[x]);
	}
	//send(0x01, 0x00);
	while(!start_detected())
	{
		forward_regulated();
	}
	forward(0x80);
	_delay_ms(400);
	//send(0x01, 0x06);
	stop();
	_delay_ms(1000);
	//send(0x01, 0x09);
	grip_off();
}