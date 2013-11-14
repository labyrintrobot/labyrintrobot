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

#define TWI_START_STATUS 0x08
#define TWI_REP_START_STATUS 0x10
#define TWI_SLAW_ACK_STATUS 0x18
#define TWI_DATA_WRITE_ACK_STATUS 0x28
#define TWI_SLAR_ACK_STATUS 0x40
#define TWI_DATA_REC_ACK_STATUS 0x50

volatile int testcounter=0b01010101;

//interrupt routine for setting the interrupt vector note that the StatusRegister
//must be saved
ISR(INT0_vect){
	char cSREG;
	cSREG = SREG;/* store SREG value */
	/* disable interrupts during timed sequence */
	//cli();
	
	testcounter++;
	
	SREG = cSREG; /* restore SREG value (I-bit) */
	
	//sei(); 
}

int TWI_initialize() {
	
}

//function that receives data via I2C if interrupt is raised
int TWI_receive_data(unsigned char* data){
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	*data = TWDR;
	
}

/************************************************************************/
/* Returns 0 if successful                                              */
/************************************************************************/
int TWI_send_start(int repeated) {
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // Send START condition
	while (!(TWCR & (1<<TWINT))); // Wait until TWINT has been set = START was successfully sent
	if (repeated) {
		if ((TWSR & 0xF8) != TWI_REP_START_STATUS) {
			return 1;
		}
	} else {
		if ((TWSR & 0xF8) != TWI_START_STATUS) {
			return 1;
		}
	}
	return 0;
}

int TWI_send_stop() {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

/************************************************************************/
/* address is the ´7 most significant bits in address. If write is true, 
it will be a write operation.       */
/************************************************************************/
int TWI_send_address(unsigned char address , int write) {
	
	if (address == 0) {
		return 2;
	}
	
	if(write){
		address&=0b11111110;
	}
	else{
		address|=0b00000001;
	}
	
	TWDR = address; // Write address to register
	TWCR = (1<<TWINT) | (1<<TWEN); // Send out address
	if ((write && (TWSR & 0xF8) != TWI_SLAW_ACK_STATUS) || (!write && (TWSR & 0xF8) != TWI_SLAR_ACK_STATUS) ) {
		return 1;
	}
	return 0;
}

int TWI_send_data(unsigned char data) {
	TWDR = data; // Write data to register
	TWCR = (1<<TWINT) | (1<<TWEN); // Send out data
	if ((TWSR & 0xF8) != TWI_DATA_WRITE_ACK_STATUS) {
		return 1;
	}
	return 0;
}

//function that sends data if via I2C if needed
int TWI_send_message(unsigned char address, unsigned char header, unsigned char data) {
	

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
	
	PORTB=0;
	
	sei();//enable interrupts
	
	EIMSK&=0b11111110;// clear interrupt request mask
	EICRA|=0b00000011;// interrupts on rising edge
	
	EICRA&=0b11111110;// interrupts on falling edge
	
	EIFR|=0b00000001; 
	EIMSK|=0b00000001;// set interrupt request mask
	
	//SREG=;
	
	//main loop 
	while(1){
		
		//volatile char interruptflag;
		
		PORTB=testcounter;
		
	}
	

	// Insert application code here, after the board has been initialized.
}
