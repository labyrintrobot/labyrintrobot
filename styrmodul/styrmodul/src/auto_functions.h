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
void store_data(uint8_t header, uint8_t data);
void narrowpath(void);

// NYYY som kollar om någon tejp har körts över, typ, 
// kan kanske bara ha en funktion som hittar och avgör vad för tejp det är? 
// read_tape gör väl inget vettigt egentligen?
bool tape_detected(int tape); 
bool read_tape(int tape_value);

void turn(int direction);
void return_to_start(void);
void turn_back(int direction);
uint8_t max(uint8_t value1 , uint8_t value2 , uint8_t value3);


int		direction_array[50];
int		i = 0;
uint8_t test_speed = 0x70;


uint8_t max(uint8_t value1 , uint8_t value2 , uint8_t value3){
	
	if(value1<value2)
		value1=value2;
	
	if(value1<value3)
		value1=value3;
	
	return value1;
}


bool inside_corridor()
{
	if(left_long_s < 70 && right_long_s < 70)
		return true;
	else
		return false;
}

//returnera true när roboten befinner sig mitt i 80x80-biten i en sväng eller en korsning med en vägg rakt fram.
bool intersection_stop()
{
	if(forward_right_s<31)
		return true;
	else
		return false;
}

void get_into_intersection() // Kör in till mitten av korsningen, om det är en vägg framför: kör på avståndet, om det saknas vägg: kör på tid
{
	forward(0x50); //kör framåt
	_delay_ms(250);//en viss tid(500)

	if(forward_right_s < 80) //Om det är en vägg framför
	{
		while(!intersection_stop()) //Kör tills det är 30 cm kvar till väggen
		{
			forward(0x50);
		}
		stop();
	}
	else //Annars om det inte är en vägg framför
	{

		_delay_ms(400);
		stop();
	}
}

// Returnerar True när den hittat en korsning eller sväng
bool intersection_detected()
{
	return ((left_long_s > 90) || (right_long_s > 90));
}


// Svänger vänster som förstaval
int unmarked_intersection_choice()
{
	if(left_long_s > 90) // vänster håll öppet
	{
		return turn_left;
	}
	else if(right_long_s > 90) // höger öppet
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


// Väljer att svänga beroende på tejpmarkering
int marked_intersection_choice()
{
	//tape: 0x00 = right, tape: 0x01 = left
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
	if(forward_center_s < 20)
	//if(forward_left_s == 0 && forward_right_s == 0) // Säkerhetsstannar
	{
		rotate_right90(0x90);
	
		_delay_ms(100);
		rotate_right90(0x90);
		
		_delay_ms(100);
	}
	else
	{
		PORTB = 0x03;
		float u, Kp, Kd, P, D;
		Kp = Kp_lsb;
		Kd = Kd_lsb;
		P=Kp*(float)e;
		P=P/10.0;
		D=Kd*(float)(e-e_last);
		u=P+D;
		
		if(u > 64) // Max-värde 0x40
		{
			u = 64;
		}
		if(u < -64) // Min-värde 
		{
			u = -64; 
		}
	
		// Reglera beroende på u 
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
	while(!start_detected())
	{ 
		forward(0xA0);	
	}
}



// I svängar och korsningar, använder sig av direction: turn_left = 0x00, go_forward = 0x01, turn_right = 0x02
void turn(int direction)
{
	switch(direction){
		case 0x00: // turn_left
			rotate_left90(0x90);
			stop();
			while(!inside_corridor()) // Kör ut ur korsningen
			{
				forward(0x70);
			}
			stop();	
			break;
		case 0x01: // go_forward
			while(!inside_corridor()) // Kör ut ur korsningen
			{
				forward(0x70);
			}
			stop();
			break;
		case 0x02: // turn_right
			rotate_right90(0x90);
			stop();
			while(!inside_corridor()) // Kör ut ur korsningen
			{
				forward(0x70);
			}
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
		rotate_right90(0x90);
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
		rotate_left90(0x90);
		forward(0xA0);
		_delay_ms(200); //Kör ut ur korsningen
		stop();
		break;
	}
}


// Returnerar True när den hittat målmarkeringen
bool goal_detected()
{
	return tape_value == 0x03;
}


// Reglerar fram till målet längs en linje
void goal_regulated()
{
	PORTB = 0x03;
	float u, Kp, Kd, P, D;
	uint8_t u_max = 0x40, u_min = 0x40;
	Kp = 10;
	Kd = 20;
	P = Kp*(float)e;
	P = P/10.0;
	D = Kd * (float)(e - e_last);
	u = P + D;
	
	if(u > u_max)		// Max-värde 0x40
		u = u_max;
	if(u < -u_min)		// Min-värde 
		u = -u_min; 
	
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


// Efter att roboten hittat målmarkeringen (i find_goal()) 
// reglerar den fram, tar föremålet och vänder sig om
void get_target()
{
	uint8_t grip_switch = 4;
	
	// Kör sakta fram till föremålet längs tejpmarkeringen
	
	while(grip_switch == 4)
	{
		grip_switch = PINA & 0x04;
		goal_regulated();
	}
	stop();
	grip_on();
	send(0x13, 1); // Skickar att föremålet är taget
	backward(0x50);
	_delay_ms(500);
	stop();
	rotate_right90(0x90);
	_delay_ms(100);
	rotate_right90(0x90);
}


 // Kör tills den hittar målet
void find_goal()
{
	int direction;
	while(!goal_detected())
	{
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
	tape_value = 0x04; // "Nollställ" tejpmarkeringen
	
	// Hittat målmarkeringen!
	get_target();
}


// Använder sig av direction_array och går baklänges igenom den
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

// Vid avsmalning
void narrowpath()
{
	bool direction;
	uint16_t length_to_box;
	
	if(forward_left_s < 40 && forward_right_s > 80)
	{
		length_to_box = forward_left_s + 10; //lägg på skillnaden i längd mellan sensorn och mitten på roboten
		direction = false;
	}
	else if(forward_right_s < 40 && forward_left_s > 80)
	{
		length_to_box = forward_right_s + 10;
		direction = true;
	}
	else
		return;
	
	uint16_t  zsquare = 1600 + (length_to_box * length_to_box); // 1600=40^2
	
	while(true)
	{	
		uint16_t length_to_wall = (forward_left_s + forward_right_s + 20) / 2;
		
		if((length_to_wall * length_to_wall < zsquare) && ((direction && forward_right_s > forward_left_s) || (!direction && forward_right_s < forward_left_s)))
			break;
		
		if(direction)
			rotate_left();
		else	
			rotate_right();
	}
	// Svängdelen klar, lås med while(1) vid testning 
	
	while(true)
	{		
		uint16_t length_to_wall = (forward_left_s + forward_right_s + 20)/2;
		
		if(4 * length_to_wall * length_to_wall < zsquare)
		{
			break;		
		}	
		forward(0x80);
	}
	
	//har kört frammåt
	
	if(direction)
	{
		while(left_long_s + 10 > 60 && right_short_s + 10 > 20)
			rotate_right();
	}
	else
	{
		while(right_long_s + 10 > 60 && left_short_s + 10 > 20)
			rotate_right();
	}
	
	//har roterat tillbaka
	
}