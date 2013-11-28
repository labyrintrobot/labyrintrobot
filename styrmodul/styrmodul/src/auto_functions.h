#define turn_left 0x00;
#define go_forward 0x01;
#define turn_right 0x02;

int unmarked_intersection_choice(int left, int forward, int right);
int marked_intersection_choice(int tape, int left, int forward, int right);
void forward_regulated(signed e);
bool intersection_detected(int left, int right);
void find_start(void);
void find_goal(void);
bool goal_detected(int tape_value);
bool start_detected(void);
void store_data(uint8_t header, uint8_t data);
bool read_tape(int tape_value);
void turn(int direction);
void return_to_start(void);
void turn_back(int direction);


uint8_t left_distance, right_distance, forward_left_distance, forward_right_distance,
		forward_center_distance, tape_value;
uint8_t tape;
int		direction_array[50];
int		i = 0;


int unmarked_intersection_choice(int left, int forward_, int right_)
{
	if(left > 150)
	{
		return turn_left;
	}
	else if(forward_ > 150)
	{
		return go_forward;
	}
	else // right_
	{
		return turn_right;	
	}
}

int marked_intersection_choice(int tape, int left, int forward, int right)
{
//tape = 0x02 = right, tape = 0x01 = left
	if((tape == 0x02) & (right < 40))
	{
		return go_forward;
	}
	else if((tape == 0x02) & (forward < 40))
	{
		return turn_right;
	}
	else if((tape == 0x01) & (left < 40))
	{
		return go_forward;	
	}
	else
	{
		return turn_left;
	}
} 



void forward_regulated(signed e)
{
	//signed e;
	//e =right_distance - left_distance;
	PORTB = 0x03;
	signed u, Kp, Kd;
	Kp = 1;
	Kd = 10; //KD=0.01/deltaT=0.1
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
bool read_tape(int tape_value)
{
	if(tape_value == 0x01 || tape_value == 0x02)
	{
		tape_value = 0x00; //nollställ senaste tejpavläsningen
		return true;
	} 
	else
	{
		return false;
	}
}

// Returnerar True när den hittat målmarkeringen 
bool goal_detected(int tape_value)
{
	if(tape_value == 0x03)
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
		while(!intersection_detected(left_distance,right_distance)) // Korridor
		{
			forward_regulated(e);
		}
		
		// Kommit till en korsning eller sväng
		forward(0xA0);
		_delay_ms(200); //Kör in en bit i korsningen
		stop(); //Stanna
		
		if(read_tape(tape_value)) // Markerad
		{
			direction = marked_intersection_choice(tape_value, left_distance, 
														forward_center_distance, right_distance);
			turn(direction);
		}
		else // Omarkerad
		{
			direction = unmarked_intersection_choice(left_distance, forward_center_distance, 
														right_distance);
			turn(direction);			
		}
		direction_array[i] = direction;
		i++;
		
	}
	get_target();
}


void store_data(uint8_t header, uint8_t data)
{
	switch(header){
		case 0x03:
			left_distance = data;
			break;
		case 0x04:
			right_distance = data;
			break;
		case 0x05:
			forward_right_distance = data;
			break;
		case 0x06:
			forward_left_distance = data;
			break;
		case 0x07:
			forward_center_distance = data;
			break;		
	}
}


void return_to_start()
{
	for(int x = i-1; x > 0; x--)
	{
		while(!intersection_detected(left_distance, right_distance))
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