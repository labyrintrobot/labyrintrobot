/*
 * auto_functions.h
 *
 * Created: 2013-12-11
 * Version: 1.0
 * Authors: Viktoria Now�n, Kim Persson
 *
 * Denna fil borde vara en .c-fil
 * 
 * Inneh�ller funktioner f�r att roboten ska kunna g� i autonomt l�ge
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


int		direction_array[50];
int		i = 0;
uint8_t test_speed = 0x70;


bool inside_corridor()
{
	if(left_long_s < 70 && right_long_s < 70)
	return true;
	else
	return false;
}

//returnera true n�r roboten befinner sig mitt i 80x80-biten i en sv�ng eller en korsning med en v�gg rakt fram.
bool intersection_stop()
{
	if(forward_right_s<31)
	return true;
	else
	return false;
}

void get_into_intersection() // K�r in till mitten av korsningen, om det �r en v�gg framf�r: k�r p� avst�ndet, om det saknas v�gg: k�r p� tid
{
	send(0x01, 0x00);
	forward(0x50); //k�r fram�t
	_delay_ms(250);//en viss tid(500)

	if(forward_right_s < 80) //Om det �r en v�gg framf�r
	{
		while(!intersection_stop()) //K�r tills det �r 30 cm kvar till v�ggen
		{
			forward(0x50);
		}
		send(0x01, 0x06);
		stop();
	}
	else //Annars om det inte �r en v�gg framf�r
	{

		_delay_ms(400);
		send(0x01, 0x06);
		stop();
	}
}

// Returnerar True n�r den hittat en korsning eller sv�ng
bool intersection_detected()
{
	return ((left_long_s > 90) || (right_long_s > 90));
}


// Sv�nger v�nster som f�rstaval
int unmarked_intersection_choice()
{
	if(left_long_s > 90) // v�nster h�ll �ppet
	{
		return turn_left;
	}
	else if(right_long_s > 90) // h�ger �ppet
	{
		return turn_right;
	}
	else if(forward_right_s > 90) // forward
	{
		return go_forward;
	}
	else
	{
		return stay;
	}
}


// V�ljer att sv�nga beroende p� tejpmarkering
int marked_intersection_choice()
{
	//tape: 0x00 = right, tape: 0x01 = left
	if((tape_value == 0x00) & (right_long_s < 80))
	{
		tape_value = 0x04; // "Nollst�ll"
		return go_forward;
	}
	else if((tape_value == 0x00) & (forward_right_s < 80))
	{
		tape_value = 0x04; // "Nollst�ll"
		return turn_right;
	}
	else if((tape_value == 0x01) & (left_long_s < 80))
	{
		tape_value = 0x04;// "Nollst�ll"
		return go_forward;
	}
	else
	{
		tape_value = 0x04; // "Nollst�ll"
		return turn_left;
	}
}



// K�r fram�t i en korridor (labyrinten) med hj�lp av PD-reglering
void forward_regulated()
{
	if(forward_left_s < 30 && forward_right_s < 30)
	{
		rotate_right90(0x90);
		_delay_ms(500);
		rotate_right90(0x90);
		_delay_ms(500);
	}
	
	else if(forward_right_s < 30 && forward_left_s > 80) // avsmalning �t v�nster
	{
		stop();
		rotate_left90(0x90);
		_delay_ms(250);
		while(forward_center_s > 12)
		{
			forward(0x50);
		}
		rotate_right90(0x90);
		while(right_short_s > 25)
		{
			forward(0x80);
		}
	}
	
	else if(forward_right_s > 80 && forward_left_s < 30) // avsmalning �t h�ger
	{
		stop();
		rotate_right90(0x90);
		_delay_ms(250);
		while(forward_center_s > 12)
		{
			forward(0x50);
		}
		rotate_left90(0x90);
		while(left_short_s > 25)
		{
			forward(0x80);
		}
	}
	else
	{
		
		PORTB = 0x03;
		float u, Kp, Kd, P, D;
		Kp = Kp_lsb;
		Kd = Kd_lsb;
		P = Kp * (float)e;
		P = P / 10.0;
		D = Kd * (float)(e- e_last);
		u = P + D;
		
		if(u > 64) // Max-v�rde 0x40
		{
			u = 64;
		}
		if(u < -64) // Min-v�rde
		{
			u = -64;
		}
		
		// Reglera beroende p� u
		if((int)u > 0) // Turn right
		{
			//OCR1AL = 0xF0;
			//OCR1BL = 0xF0 - (float) u / (float) 0x70 * (float) (0xF0 - 0x40);
			OCR1BL = (speed - u);		// Right side
			OCR1AL = (speed + u);	// left side
		}
		else if((int)u < 0) // Turn left
		{
			OCR1BL = (speed - u);	// Right side
			OCR1AL = (speed + u);	// left side
		}
		
		else // Don't turn, keep going
		{
			OCR1BL = speed; // Right side
			OCR1AL = speed; // Left side
		}
	}
}





// Returnerar True n�r den hittat startmarkeringen
// START = Tv� tjocka tejpmarkeringar (TT), tape = 0x02
bool start_detected()
{
	if(tape_value == 0x02)
	{
		tape_value = 0x04; // "Nollst�ll" senaste tejpavl�sningen
		return true;
	}
	else
	return false;
}

// K�r fram�t tills startmarkeringen hittas
void find_start()
{
	while(!start_detected())
	{
		forward(0x80);
	}
}



// I sv�ngar och korsningar, anv�nder sig av direction: turn_left = 0x00, go_forward = 0x01, turn_right = 0x02
void turn(int direction)
{
	switch(direction){
		case 0x00: // turn_left
		rotate_left90(0x90);
		send(0x01, 0x06);
		stop();
		send(0x01, 0x00);
		while(!inside_corridor()) // K�r ut ur korsningen
		{
			forward(0x80);
		}
		send(0x01, 0x06);
		stop();
		break;
		case 0x01: // go_forward
		send(0x01, 0x00);
		while(!inside_corridor()) // K�r ut ur korsningen
		{
			forward(0x80);
		}
		send(0x01, 0x06);
		stop();
		break;
		case 0x02: // turn_right
		rotate_right90(0x90);
		send(0x01, 0x06);
		stop();
		while(!inside_corridor()) // K�r ut ur korsningen
		{
			forward(0x80);
		}
		send(0x01, 0x06);
		stop();
		break;
		case 0x03: // stay
		send(0x01, 0x06);
		stop();
		break;
	}
}

// I sv�ngar och korsningar p� v�g tillbaka, n�r m�let h�mtats
void turn_back(int direction)
{
	switch(direction){
		case 0x00: //turn_left
		rotate_right90(0x90);
		stop();
		while(!inside_corridor()) // K�r ut ur korsningen
		{
			forward(0x70);
		}
		stop();
		break;
		case 0x01: // go_forward
		while(!inside_corridor()) // K�r ut ur korsningen
		{
			forward(0x70);
		}
		stop();
		break;
		case 0x02: // turn_right
		rotate_left90(0x90);
		stop();
		while(!inside_corridor()) // K�r ut ur korsningen
		{
			forward(0x70);
		}
		stop();
		break;
		case 0x03:	// stay
		stop();
		break;
	}
}


// Returnerar True n�r den hittat m�lmarkeringen
bool goal_detected()
{
	return (tape_value == 0x03);
}


// Reglerar fram till m�let l�ngs en linje
void goal_regulated()
{
	PORTB = 0x03;
	float u, Kp, Kd, P, D;
	uint8_t u_max = 0x40;
	Kp = 10;
	Kd = 20;
	P = Kp*(float)e;
	P = P/10.0;
	D = Kd * (float)(e - e_last);
	u = P + D;
	
	if(u > u_max)		// Max-v�rde 0x40
	u = u_max;
	if(u < -u_max)		// Min-v�rde
	u = -u_max;
	
	uint8_t goal_speed = 0x50;
	// Reglera beroende p� u
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

// K�r tills den hittar m�let
void find_goal()
{
	int direction;
	while(!goal_detected())
	{
		send(0x01, 0x00);
		while(!intersection_detected() && !goal_detected()) // Korridor
		{
			forward_regulated();
		}
		
		// Kommit till en korsning eller sv�ng
		if(tape_value == 0x00 || tape_value == 0x01) // Markerad korsning, h�ll h�ger eller v�nster
		{
			get_into_intersection();
			direction = marked_intersection_choice();
			turn(direction);
			direction_array[i] = direction;
			i++;
		}
		else if(tape_value == 0x04)// Omarkerad korsning
		{
			get_into_intersection();
			direction = unmarked_intersection_choice();
			turn(direction);
			direction_array[i] = direction;
			i++;
		}
	}
	tape_value = 0x04; // "Nollst�ll" tejpmarkeringen
	
	// Hittat m�lmarkeringen!
	send(0x01, 0x06);
	stop();
	_delay_ms(500);
	
}

// Efter att roboten hittat m�lmarkeringen (i find_goal())
// reglerar den fram, tar f�rem�let och v�nder sig om
void get_target()
{
	uint8_t grip_switch = 4;
	// K�r sakta fram till f�rem�let l�ngs tejpmarkeringen
	while(grip_switch == 4)
	{
		grip_switch = PINA & 0x04;
		goal_regulated();
	}
	// Framme vid f�rem�let
	stop();
	grip_on();
	// send(0x13, 1); // Skickar att f�rem�let �r taget
	backward(0x100);
	_delay_ms(500);
	stop();
	rotate_left90(0x90);
	_delay_ms(100);
	rotate_left90(0x90);
}

// Anv�nder sig av direction_array och g�r bakl�nges igenom den
void return_to_start()
{
	for(int x = i-1; x > 0; x--)
	{
		while(!intersection_detected())
		{
			forward_regulated();
		}
		get_into_intersection();
		turn_back(direction_array[x]);
	}
	while(!start_detected())
	{
		forward_regulated();
	}
	stop();
}