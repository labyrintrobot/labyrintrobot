#define turn_left 0x00;
#define go_forward 0x01;
#define turn_right 0x02;
int unmarked_intersection_choice(int left, int forward, int right);
int marked_intersection_choice(int tape, int left, int forward, int right);
bool intersection_detected(int left, int forward, int right);

int unmarked_intersection_choice(int left, int forward, int right)
{
	if(left > 120)
	{
		return turn_left;
	}
	else if(forward > 120)
	{
		return go_forward;
	}
	else
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


bool intersection_detected(int left, int forward, int right)
{
	if( )
	{
			
	}
}