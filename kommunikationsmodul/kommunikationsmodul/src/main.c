//TWBR;//läs initial clockspeed BITRATE REGISTER

//TWCR;//läs

//TWCR initierar händelser och styr bussen
//:bit7 TWINT skriv 1:a för att initiera händelse efter att de andra bitarna satts
//:bit2 Enable Bit: gör om portarna till SDA och SCL 

//TWSR;//läs status register läsläsläs

//TWDR;//läs databuffert

//TWAR;//läs if slave then input adress

#include <asf.h>
#include <avr/interrupt.h>

//ID:s for the different modules
int communicationid=1;
int steerid=2;
int sensorid=3;

volatile int testcounter=0;

//interrupt routine for setting the interrupt vector note that the StatusRegister
//must be saved
ISR(INT0_vect){
	char cSREG;
	cSREG = SREG;/* store SREG value */
	/* disable interrupts during timed sequence */
	cli();
	
	testcounter++;
	
	SREG = cSREG; /* restore SREG value (I-bit) */
	
	sei(); 
}

//function that recives data via I2C if interupt is raised
int recivedataI2C(){
	
}

//function that sends data if via I2C if needed
int senddataI2C(int adress){
	
}


int main (void)
{
	board_init();
	
	TWBR=87;//init clockspeed
	TWSR|=2;//init clockspeed scale
	
	// (clk/(16+2*(TWBR)*16))=5000 bits/s
	
	// set upon interuptrequest, tells who to serve next.
	int interuptvector=0;
	
	DDRB=0xFF;
	
	//main loop 
	while(1){
		
		//volatile char interruptflag;
		
		PORTB=testcounter;
		
	}
	

	// Insert application code here, after the board has been initialized.
}
