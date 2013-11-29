#define turn_left 0x00;
#define go_forward 0x01;
#define turn_right 0x02;

int unmarked_intersection_choice(int left_, int forward_);
int marked_intersection_choice(int tape, int left_, int forward_, int right_);
void forward_regulated(signed e);
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

int unmarked_intersection_choice(int left_, int forward_)
{
	if(left_ > 150)
	{
		return turn_left;
	}
	else if(forward_ > 150)
	{
		return go_forward;
	}
	else // right
	{
		return turn_right;	
	}
}

int marked_intersection_choice(int tape, int left_, int forward_, int right_)
{
	//tape = 0x02 = right, tape = 0x01 = left
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



void forward_regulated(signed e_)
{
	PORTB = 0x03;
	signed u, Kp, Kd;
	Kp = 1;
	Kd = 10; //KD=0.01/deltaT=0.1
	u = Kp*e_ + Kd*(e_ - e_last); //KD-regulator
	e_last = e_;
	
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


// Returnerar True när den hittat en korsning eller sväng
bool intersection_detected(int left, int right)
{
	return ((left > 90) || (right > 90));
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

//tape = 0x02 = right, tape = 0x01 = left
bool read_tape(int tape)
{
	if(tape == 0x01 || tape == 0x02)
	{
	//	tape_value = 0x00; //nollställ senaste tejpavläsningen
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
		tape_value = 0x00; //nollställ senaste tejpavläsningen
		return true;
	}
	else
	{
		return false;
	}
}



void turn(int direction)
{
	switch(direction){
		case 0x00: //turn_left
			rotate_left90();
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
			rotate_right90();
			forward(0xA0);
			_delay_ms(200); //Kör ut ur korsningen
			stop();
			break;
	}
	
}

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

void get_target()
{
	//kör sakta fram till föremålet
	grip_on();
	backward(0xA0);
	_delay_ms(200);
	stop();
	rotate_right90();
	rotate_right90();
	
}


 // Kör tills den hittar målet
void find_goal()
{
	int direction;
	while(!goal_detected(tape_value))
	{
		while(!intersection_detected(left_long_s,right_long_s)) // Korridor
		{
			forward_regulated(e);
		}
		
		// Kommit till en korsning eller sväng
		forward(0xA0);
		_delay_ms(200); //Kör in en bit i korsningen
		stop(); //Stanna
		
		if(read_tape(tape_value)) // Markerad
		{
			direction = marked_intersection_choice(tape_value,	left_long_s, 
																forward_center_s, right_long_s);
			turn(direction);
		}
		else // Omarkerad
		{
			direction = unmarked_intersection_choice(left_long_s, forward_center_s);
			turn(direction);			
		}
		direction_array[i] = direction;
		i++;
		
	}
	get_target();
}


void return_to_start()
{
	for(int x = i-1; x > 0; x--)
	{
		while(!intersection_detected(left_long_s, right_long_s))
		{
			forward_regulated(e);
		}
		forward(0xA0);
		_delay_ms(200); //Kör in en bit i korsningen
		stop(); //Stanna
		turn_back(direction_array[x]);
	}
	while(!start_detected())
	{
		forward_regulated(e);
	}
	stop();
}