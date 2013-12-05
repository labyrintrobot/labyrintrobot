#define turn_left 0x00;
#define go_forward 0x01;
#define turn_right 0x02;
#define stay 0x03;

bool intersection_detected(int left, int right);
int unmarked_intersection_choice(int left_, int right_, int forward_);
int marked_intersection_choice(int tape, int left_, int forward_, int right_);
void forward_regulated(void);
void find_start(void);
void find_goal(void);
bool goal_detected(int tape_value);
void get_target(void);
bool start_detected(void);
void store_data(uint8_t header, uint8_t data);

// NYYY som kollar om n�gon tejp har k�rts �ver, typ, 
// kan kanske bara ha en funktion som hittar och avg�r vad f�r tejp det �r? 
// read_tape g�r v�l inget vettigt egentligen?
bool tape_detected(int tape); 
bool read_tape(int tape_value);

void turn(int direction);
void return_to_start(void);
void turn_back(int direction);

int		direction_array[50];
int		i = 0;


// Returnerar True n�r den hittat en korsning eller sv�ng
bool intersection_detected(int left_, int right_)
{
	return ((left_ > 90) || (right_ > 90));
}


// Sv�nger v�nster som f�rstaval
int unmarked_intersection_choice(int left_, int right_, int forward_)
{
	if(left_ > 150) // v�nster h�ll �ppet
	{
		return turn_left;
	}
	else if(right_ > 150) // h�ger �ppet
	{
		return turn_right;
	}
	else if(forward_ > 150)// forward
	{
		return go_forward;	
	}
	else
	{
		return stay;
	}
}


// V�ljer att sv�nga beroende p� tejpmarkering
int marked_intersection_choice(int tape, int left_, int forward_, int right_)
{
	//tape: 0x02 = right, tape: 0x01 = left
	if((tape == 0x02) & (right_ < 40))
	{
		tape_value = 0x00;
		return go_forward;
	}
	else if((tape == 0x02) & (forward_ < 40))
	{
		tape_value = 0x00;
		return turn_right;
	}
	else if((tape == 0x01) & (left_ < 40))
	{
		tape_value = 0x00;
		return go_forward;	
	}
	else
	{
		tape_value = 0x00;
		return turn_left;
	}
} 



// K�r fram�t i en korridor (labyrinten) med hj�lp av PD-reglering
void forward_regulated()
{
	if(forward_center_s < 15) // S�kerhetsstannar
	{
		stop();
	}
	else
	{
		PORTB = 0x03;
		signed u, Kp, Kd;
		Kp = 20;
		Kd = 1; // (Kd = 1) / (deltaT = 0.1)
		u = Kp*e + Kd*(e - e_last); // Kd-regulator
	
		if(u > 0x70) // Max-v�rde
		{
			u = 0x70;
		}
		if(u < -0x70) // Min-v�rde 
		{
			u = -0x70;
		}
	
		// Reglera beroende p� u 
		if(u > 0) // Turn right
		{
			OCR1BL = speed;		// Right side
			OCR1AL = speed + u; // left side
		}
		else if(u < 0) // Turn left
		{
			OCR1BL = speed - u; // Right side
			OCR1AL = speed;		// Left side
		}
		else if(u == 0) // Don't turn, keep going
		{
			OCR1BL = speed; // Right side
			OCR1AL = speed; // Left side
		}
	}
}


// Detekterar tejpmarkeringar, inte vad det �r f�r slags dock. Det f�r read_tape g�ra?
bool tape_detected(int tape)
{
	return (tape > 0); // "Tejptiden" �verstiger 0 sek, allts� n�gon slags tejp
}



//-------------------------------------------------------------------------------------
// D�pa om till "tape_detected" ist�llet???
// G�ra en ny read_tape som returnerar vad det �r f�r slags markering: START, M�L osv...
//-------------------------------------------------------------------------------------
// L�ser av tejpen, returnerar true om den hittar en tejp, nollst�ller sedan tejpen
// tape = 0x02 = right, tape = 0x01 = left
bool read_tape(int tape)
{
	if(tape == 0x01 || tape == 0x02)
	{
		//
		tape_value = 0x00; // Nollst�ll senaste tejpavl�sningen
		return true;
	}
	else
	{
		return false;
	}
}


// Returnerar True n�r den hittat startmarkeringen 
// START = Tv� tjocka tejpmarkeringar (TT)
// En viss tid f�r smal tejp, och en annan f�r tjock tejp
// Beror hastigheten p� roboten?
bool start_detected()
{
	return true; // Tills vidare
}

 // K�r fram�t tills startmarkeringen hittas
void find_start()
{
	while(!start_detected())
	{ 
		forward(0xA0);	
	}
}



// Returnerar True n�r den hittat m�lmarkeringen 
bool goal_detected(int tape)
{
	if(tape == 0x03)
	{
		tape_value = 0x00; // Nollst�ll senaste tejpavl�sningen
		return true;
	}
	else
	{
		return false;
	}
}


// I sv�ngar och korsningar, anv�nder sig av direction: turn_left = 0x00, go_forward = 0x01, turn_right = 0x02
void turn(int direction)
{
	switch(direction){
		case 0x00: // turn_left
			rotate_left90();
			forward(0xA0); 
			_delay_ms(800); // K�r ut ur korsningen
			stop();
			break;
		case 0x01: // go_forward
			forward(0xA0);
			_delay_ms(1000); //K�r ut ur korsningen
			stop();
			break;
		case 0x02: // turn_right
			rotate_right90();
			forward(0xA0);
			_delay_ms(800); // K�r ut ur korsningen
			stop();
			break;
		case 0x03: // stay
			stop();
			break;
	}
	
}

// I sv�ngar och korsningar p� v�g tillbaka, n�r m�let h�mtats
void turn_back(int direction)
{
	switch(direction){
		case 0x00: //turn_left
		rotate_right90();
		forward(0xA0);
		_delay_ms(200); //K�r ut ur korsningen
		stop();
		break;
		case 0x01: //go_forward
		forward(0xA0);
		_delay_ms(200); //K�r ut ur korsningen
		stop();
		break;
		case 0x02: //turn_right
		rotate_left90();
		forward(0xA0);
		_delay_ms(200); //K�r ut ur korsningen
		stop();
		break;
	}
}


// H�mtar objektet
void get_target()
{
	uint8_t grip_switch = 4;
	// K�r sakta fram till f�rem�let
	forward(0x70);
	_delay_ms(20);
	forward(0x50);
	while(grip_switch == 4)
	{
		grip_switch = PINA & 0x04;
	}
	stop();
	grip_on();
	backward(0x50);
	_delay_ms(200);
	stop();
	//rotate_right();
	//rotate_right();
	
}


 // K�r tills den hittar m�let //
void find_goal()
{
	int direction;
	while(!goal_detected(tape_value))
	{
		while(!intersection_detected(left_long_s,right_long_s)) // Korridor
		{
			forward_regulated();
		}
		
		// Kommit till en korsning eller sv�ng //
		forward(0xA0);
		_delay_ms(200); // K�r in en bit i korsningen // 
		stop(); // Stanna
		
		if(read_tape(tape_value)) // Markerad
		{
			direction = marked_intersection_choice(tape_value,	left_long_s, 
												   forward_center_s, right_long_s);
			turn(direction);
		}
		else // Omarkerad
		{
			direction = unmarked_intersection_choice(left_long_s, right_long_s, forward_left_s);
			turn(direction);			
		}
		direction_array[i] = direction;
		i++;
		
	}
	get_target();
}


// Anv�nder sig av direction_array och g�r bakl�nges igenom den
void return_to_start()
{
	for(int x = i-1; x > 0; x--)
	{
		while(!intersection_detected(left_long_s, right_long_s))
		{
			forward_regulated();
		}
		forward(0xA0);
		_delay_ms(200); // K�r in en bit i korsningen //
		stop(); // Stanna //
		turn_back(direction_array[x]);
	}
	while(!start_detected())
	{
		forward_regulated();
	}
	stop();
}