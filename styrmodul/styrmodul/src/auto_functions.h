#define turn_left 0x00;
#define go_forward 0x01;
#define turn_right 0x02;
#define stay 0x03;

int unmarked_intersection_choice(int left_, int right_, int forward_);
int marked_intersection_choice(int tape, int left_, int forward_, int right_);
void forward_regulated(void);
bool intersection_detected(int left, int right);
void find_start(void);
void find_goal(void);
bool goal_detected(int tape_value);
void get_target(void);
bool start_detected(void);
void store_data(uint8_t header, uint8_t data);
bool read_tape(int tape_value);
void turn(int direction);
void return_to_start(void);
void turn_back(int direction);

int		direction_array[50];
int		i = 0;





// Returnerar True när den hittat en korsning eller sväng
bool intersection_detected(int left, int right)
{
	return ((left > 90) || (right > 90));
}


// Svänger vänster som förstaval
int unmarked_intersection_choice(int left_, int right_, int forward_)
{
	if(left_ > 150) // vänster håll öppet
	{
		return turn_left;
	}
	else if(right_ > 150) // höger öppet
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


// Väljer att svänga beroende på tejpmarkering
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


// Kör framåt i en korridor (labyrinten) med hjälp av PD-reglering
void forward_regulated()
{
	PORTB = 0x03;
	signed u, Kp, Kd;
	Kp = 2;
	Kd = 8; // (Kd = 1) / (deltaT = 0.1)
	u = Kp*e + Kd*(e - e_last); // Kd-regulator
	
	if(u > 0x60) // Max-värde
	{
		u = 0x60;
	}
	if(u < -0x60) // Min-värde 
	{
		u = -0x60;
	}
	
	// Reglera beroende på u 
	if(u > 0) // turn right
	{
		OCR1BL = 0xA0 - u; // Right side
		OCR1AL = 0xA0; // left side
	}
	else if(u < 0) // turn left
	{
		OCR1BL = 0xA0; //right side
		OCR1AL = 0xA0 + u; // Left side
	}
	else if(u == 0) // Don't turn, keep going
	{
		OCR1BL = 0xA0; // Right side
		OCR1AL = 0xA0; // Left side
	}
}


// Returnerar True när den hittat startmarkeringen 
bool start_detected()
{
	return true; // tills vidare
}

 // Kör framåt tills startmarkeringen hittas
void find_start()
{
	while(!start_detected())
	{ 
		forward(0xA0);	
	}
}

// Läser av tejpen
// tape = 0x02 = right, tape = 0x01 = left
bool read_tape(int tape)
{
	if(tape == 0x01 || tape == 0x02)
	{
		//
		tape_value = 0x00; // Nollställ senaste tejpavläsningen
		return true;
	} 
	else
	{
		return false;
	}
}

// Returnerar True när den hittat målmarkeringen 
bool goal_detected(int tape)
{
	if(tape == 0x03)
	{
		tape_value = 0x00; // Nollställ senaste tejpavläsningen
		return true;
	}
	else
	{
		return false;
	}
}


// I svängar och korsningar, använder sig av direction: turn_left = 0x00, go_forward = 0x01, turn_right = 0x02
void turn(int direction)
{
	switch(direction){
		case 0x00: // turn_left
			rotate_left90();
			forward(0xA0);
			_delay_ms(800); // Kör ut ur korsningen
			stop();
			break;
		case 0x01: // go_forward
			forward(0xA0);
			_delay_ms(800); //Kör ut ur korsningen
			stop();
			break;
		case 0x02: // turn_right
			rotate_right90();
			forward(0xA0);
			_delay_ms(800); // Kör ut ur korsningen
			stop();
			break;
		case 0x03: // stay
			stop();
			break;
	}
	
}

// I svängar och korsningar på väg tillbaka, när målet hämtats
void turn_back(int direction)
{
	switch(direction){
		case 0x00: //turn_left
		rotate_right90();
		forward(0xA0);
		_delay_ms(200); //Kör ut ur korsningen
		stop();
		break;
		case 0x01: //go_forward
		forward(0xA0);
		_delay_ms(200); //Kör ut ur korsningen
		stop();
		break;
		case 0x02: //turn_right
		rotate_left90();
		forward(0xA0);
		_delay_ms(200); //Kör ut ur korsningen
		stop();
		break;
	}
}


// Hämtar objektet
void get_target()
{
	uint8_t grip_switch = 4;
	// Kör sakta fram till föremålet
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


 // Kör tills den hittar målet //
void find_goal()
{
	int direction;
	while(!goal_detected(tape_value))
	{
		while(!intersection_detected(left_long_s,right_long_s)) // Korridor
		{
			forward_regulated();
		}
		
		// Kommit till en korsning eller sväng //
		forward(0xA0);
		_delay_ms(200); // Kör in en bit i korsningen // 
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


// Använder sig av direction_array och går baklänges igenom den
void return_to_start()
{
	for(int x = i-1; x > 0; x--)
	{
		while(!intersection_detected(left_long_s, right_long_s))
		{
			forward_regulated();
		}
		forward(0xA0);
		_delay_ms(200); // Kör in en bit i korsningen //
		stop(); // Stanna //
		turn_back(direction_array[x]);
	}
	while(!start_detected())
	{
		forward_regulated();
	}
	stop();
}