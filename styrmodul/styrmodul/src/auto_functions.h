#define turn_left 0x00;
#define go_forward 0x01;
#define turn_right 0x02;
#define stay 0x03;

bool inside_corridor(void);
bool intersection_stop(void);
void get_into_intersection(void);
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
	{
		return true;
	}
	else
	{
		return false;
	}
}

//returnera true när roboten befinner sig mitt i 80x80-biten i en sväng eller en korsning med en vägg rakt fram.
bool intersection_stop()
{
	if(forward_right_s<31)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void get_into_intersection() // Kör in till mitten av korsningen, om det är en vägg framför: kör på avståndet, om det saknas vägg: kör på tid
{
	forward(0x50); //kör framåt
	_delay_ms(250);//en viss tid(500)
	
	//uint8_t forward=max(forward_left_s   , forward_right_s  , 0);
	//uint8_t forward_center=forward+10;
	
	//forward_center+=10;
	
	//forward_center=forward_center%80;
	 
	 //run until ( max(forward_left_s   , forward_right_s  , 0)<forward-forward_center)
	
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
bool intersection_detected(int left_, int right_)
{
	return ((left_ > 120) || (right_ > 120));
}


// Svänger vänster som förstaval
int unmarked_intersection_choice(int left_, int right_, int forward_)
{
	if(left_ > 120) // vänster håll öppet
	{
		return turn_left;
	}
	else if(right_ > 120) // höger öppet
	{
		return turn_right;
	}
	else if(forward_ > 120)// forward
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
	if(forward_center_s < 20)
	//if(forward_left_s == 0 && forward_right_s == 0) // Säkerhetsstannar
	{
		rotate_right90();
	
		_delay_ms(100);
		rotate_right90();
		
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
		//Kp = 2;
		//Kd = 10; // (Kd = 1) / (deltaT = 0.1)  // Börja med P-delen först....////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//u = Kp_lsb*e + Kd_lsb*(e - e_last); // Kd-regulator
	
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
			
					
			/*OCR1BL = speed;		// Right side
			
			int diff = (int) speed + u - 0xF0;
			if (diff > 0) {
				OCR1BL = speed - (uint8_t) diff;
				OCR1AL = 0xF0;
			} else {
				OCR1BL = speed;
				OCR1AL = speed + u;
			}
			if ((int) speed + u > 255) {
				OCR1AL = 255; // left side
			} else {
				OCR1AL = speed + u; // left side
			}*/
			
		}
		else if((int)u < 0) // Turn left
		{
			OCR1BL = (speed - u);	// Right side
			OCR1AL = (speed + u);	// left side
			
			//OCR1BL = speed - u; // Right side
			//OCR1AL = speed + u;		// Left side
		}
		
		else // Don't turn, keep going
		{
			OCR1BL = speed; // Right side
			OCR1AL = speed; // Left side
		}
	}
}


// Detekterar tejpmarkeringar, inte vad det är för slags dock. Det får read_tape göra?
bool tape_detected(int tape)
{
	return (tape > 0); // "Tejptiden" överstiger 0 sek, alltså någon slags tejp
}



//-------------------------------------------------------------------------------------
// Döpa om till "tape_detected" istället???
// Göra en ny read_tape som returnerar vad det är för slags markering: START, MÅL osv...
//-------------------------------------------------------------------------------------
// Läser av tejpen, returnerar true om den hittar en tejp, nollställer sedan tejpen
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


// Returnerar True när den hittat startmarkeringen 
// START = Två tjocka tejpmarkeringar (TT)
// En viss tid för smal tejp, och en annan för tjock tejp
// Beror hastigheten på roboten?
bool start_detected()
{
	return true; // Tills vidare
}

 // Kör framåt tills startmarkeringen hittas
void find_start()
{
	while(!start_detected())
	{ 
		forward(0xA0);	
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
			stop();
			while(!inside_corridor()) // Kör ut ur korsningen
			{
				forward(0x70);
			}
			//_delay_ms(200);
			stop();	
			break;
		case 0x01: // go_forward
			while(!inside_corridor()) // Kör ut ur korsningen
			{
				forward(0x70);
			}
			//_delay_ms(200);
			stop();
			break;
		case 0x02: // turn_right
			rotate_right90();
			stop();
			while(!inside_corridor()) // Kör ut ur korsningen
			{
				forward(0x70);
			}
			//_delay_ms(200);
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
	//svängdelen klar, lås med while(1) vid testning 
	
	while(true)
	{		
		uint16_t lengthtowall = (forward_left_s + forward_right_s + 20)/2;
		
		if(4 * lengthtowall * lengthtowall < zsquare)
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