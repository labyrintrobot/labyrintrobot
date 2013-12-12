/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */

#include <asf.h>
#include <util/delay.h>
#include <avr/interrupt.h> 
#include <avr/io.h>
#include "ADC_setup.h"
#include "sensor_tabel.h"
#include "twi_slave.h"
#include "twi_common.h"
#include "twi_common_private.h"
#include "Gyro.h"
#include "INT0_Enable.h"


volatile bool styrmodul_interrupt = false;
volatile bool turn_90_done = false;
volatile bool over_mark = false, keep_right=0, keep_left=0, line1_small, line2_small;
volatile uint8_t step=0, line_distance=0, marking;
volatile uint16_t line1, line2;

#define NUMBER_OF_SENSORS 7
#define OVER_MARKING 0

enum headers_t
{
	kortV = 0x03,
	longV = 0x04,
	longFV = 0x05,
	kortF = 0x06,
	longFH = 0x07,
	longH = 0x08,
	kortH = 0x09,
	e = 0x0A,
	tejpMarkering = 0x0B,
	lin0 = 0x0C,  // Specify which CH this belong to!
	lin1 = 0x0D, 
	lin2 = 0x0E,
	lin3 = 0x0F,
	lin4 = 0x10,
	lin5 = 0x11,	
};

enum linjeCH_t { CH4 = 0x30, CH5 = 0x40, CH6 = 0x50, CH7 = 0x60, CH8 = 0x70,}; 
	//X3[DCBA] CH4  = 0011 = 0x30===== X4[DCBA] CH5 = 0100 = 0x40;======= X5[DCBA] CH6 = 0101 = 0x50
	//X6[DCBA] CH7 = 0110 = 0x06=======X7[DCBA] CH8 = 0111 = 0x70
	//Legend: Xn is output port of Mux, [DCBA] is address port of Mux to get Xn, CHn is IR array channel 

bool sensors_to_read[NUMBER_OF_SENSORS];
uint8_t sensor_data[NUMBER_OF_SENSORS];
uint8_t proper_sensor_data[NUMBER_OF_SENSORS];
uint8_t proper_sensor_data_old[NUMBER_OF_SENSORS];
bool mux_sensors = false;
bool read_line = false;
uint8_t last_marking;
uint8_t counter;
bool recieve;
enum headers_t data_to_send = kortV;
enum headers_t data_to_send2 = tejpMarkering;
uint8_t recieve_header;
uint8_t recieve_data;
bool test;
uint8_t portdmux = 0;
uint8_t orgmux = 0xE0;
uint8_t currentSensor = 0;
uint8_t sensorToSend = 0;
void controlSensor();
uint8_t minimum_of(uint8_t v1,uint8_t v2);
//void lookUpSensor();
//int8_t narrowing();
int8_t controlled_e(int16_t val);

int main (void)
{
	board_init();
	ADCsetup();
	TWI_slave_initialize(TWI_SENSOR_MODULE_ADDRESS);
	TCCR1B |= (1<<CS12) | (1<<CS10); //Timer1 Init , Prescale = 1024
	sensors_to_read[S12]=true;
	sensors_to_read[S21]=true;
	sensors_to_read[S22]=true;
	sensors_to_read[S11]=true;
	sensors_to_read[S23]=true;
	sensors_to_read[S24]=true;
	sensors_to_read[S13]=true;
	
	
	DDRD = 0x01; ;    //PORTD0 as output ( KM module Int)
	PORTD &= ~(1<<PORTD0); //PORTD0 Low
	
//================ 1 sec delay using 16bits timer1 === OBS Delay less than 221ms is OKKKK @ 14.7456 MHz ===========
//	 Wait to other boards to init...
	_delay_ms(200);_delay_ms(200);_delay_ms(200);_delay_ms(200);_delay_ms(200);
	
//============= Init SPI, Timer0, Extern int ================
	Int0_setup();		//Enable Extern interrupt INT2
	SPI_Init();			//Init SPI bus for Gyro

//	Timer0_setup_start(); //Setup timer0 and start timer, PB0 as Output and low
	Timer0_setup_start();

		
	ADMUX = 0xE0;
	ADCSRA |= 1<<ADSC;
	//Set Mux_sel port as output. Mux_sel[3:0] @ PD[7:4] and low
	DDRD |= 0xF0;
	PORTD &= 0x0F;
//===================Linljesensor Test==============================
//while(1){


//Test Set CH4 
//To set only the upper nipple and preserver the lower, use PORTD  = (PORTD & 0x0F)| CHn;
// Result: On-time different every cycle for all channel !!!!__-__--______-__--___---_
	
/*	PORTD  = (PORTD & 0x0F)| CH4; //This loop give lowest on-time for mux_ch = 68 us !!!! ???
	_delay_ms(1); 
	PORTD  = (PORTD & 0x0F)| CH5;
	_delay_ms(1);
	PORTD  = (PORTD & 0x0F)| CH6;
	_delay_ms(1);
	PORTD  = (PORTD & 0x0F)| CH7;
	_delay_ms(1);
	PORTD  = (PORTD & 0x0F)| CH8;
	_delay_ms(1);
	


}//End while */
//===================End Test of linjesensor =======================	
	while(1)
	{	
		
		// start a conversion at the first sensor to be read
		//_delay_ms(100);
		//lookUpSensor();
		//if(TCNT1%107==0)
		//{
			// Linjesensor
			// Set admux
			// Set mux
			// Starta ADC för linjesensor
		//}
	
		
			
		if(TCNT1 >= 1439)
		{
			//data_to_send2 = kortV;
			//currentSensor = S12;
			//_delay_ms(100);
			//currentSensor = S12;
			//ADMUX = 0xE4;
			//ADCSRA |= 1<<ADSC; // Start a new conversion sweep
			// skicka data
			//ADCSRA |= 1<<ADSC;
			
		//	PORTB = PORTB ^0x01;  //10HZ
			PORTD  = (PORTD & 0x0F)| CH8; // Enable Ch8
			PORTD |= (1<<PORTD0);  ///PD0 high, request to send data using I2C bus
			TCNT1 = 0;              //reset timer
		}		
	}
}

ISR(TWI_vect)
{
	cli();
	int16_t regulation_error;
	TWI_slave_wait_for_address(&recieve);
	if(!recieve)
	{
		PORTD &= ~(1<<PORTD0); //PD0 low
		//Test sending Ch8
	//	TWI_slave_send_message(lin0,Ch8 );
		
		if(data_to_send < e)
		{
			TWI_slave_send_message(data_to_send, minimum_of(proper_sensor_data[data_to_send-3],proper_sensor_data_old[data_to_send-3]));
			data_to_send++;
			PORTD |= (1<<PORTD0); //PD0 high, request to send more data
		}
		else if (data_to_send == e)
		{
			if(sensors_to_read[S21]&&sensors_to_read[S24])
				regulation_error = minimum_of(proper_sensor_data[S24],proper_sensor_data_old[S24]) - minimum_of(proper_sensor_data[S21],proper_sensor_data_old[S21]);
			else if(sensors_to_read[S21] && ( minimum_of(proper_sensor_data[S12],proper_sensor_data_old[S12]) > 27))
				regulation_error =  minimum_of(proper_sensor_data[S13],proper_sensor_data_old[S13]) -  minimum_of(proper_sensor_data[S21],proper_sensor_data_old[S21]);
			else if(sensors_to_read[S24] && ( minimum_of(proper_sensor_data[S13],proper_sensor_data_old[S13]) > 27))
				regulation_error =  minimum_of(proper_sensor_data[S24],proper_sensor_data_old[S24])-  minimum_of(proper_sensor_data[S12],proper_sensor_data_old[S12]);
			else
				regulation_error =  minimum_of(proper_sensor_data[S13],proper_sensor_data_old[S13]) - minimum_of(proper_sensor_data[S12],proper_sensor_data_old[S12]);
			TWI_slave_send_message(e,controlled_e(regulation_error));
			data_to_send = kortV;
		
		}
		/*if (data_to_send2 != tejpMarkering)
		{
			if(data_to_send < e)
			{
				TWI_slave_send_message(data_to_send,proper_sensor_data[data_to_send-3]);
				//data_to_send++;
				//PORTD |= (1<<PORTD5);
			}
			else if(data_to_send == e)
			{
				// Kolla mellan vilka sensorer som reglerfelet skall tas ut
				if(sensors_to_read[S21]&&sensors_to_read[S22])
					TWI_slave_send_message(e,proper_sensor_data[S21]-proper_sensor_data[S22]);
				else if(sensors_to_read[S21]&&sensors_to_read[S13])
					TWI_slave_send_message(e,proper_sensor_data[S21]-proper_sensor_data[S13]);
				else if(sensors_to_read[S12]&&sensors_to_read[S22])
					TWI_slave_send_message(e,proper_sensor_data[S12]-proper_sensor_data[S22]);
				else
					TWI_slave_send_message(e,proper_sensor_data[S12]-proper_sensor_data[S13]);
				data_to_send = kortV;
				//data_to_send2 = tejpMarkering;
			}
			else
			{
				data_to_send = kortV;
				data_to_send2 = tejpMarkering;
			}
		}
		else
			TWI_slave_send_message(tejpMarkering,last_marking);
			*/
	}
	else
	{
		// Kalibreringsorder ska sätta alla sensorer to_read och kalibrera vinkelgivaren
		TWI_slave_receive_message(&recieve_header,&recieve_data);
	}
	sei();
}

ISR(ADC_vect) // this interrupt will sweep all the sensors once and interpolate between the two closest points from a table, when all the data has been read it will ask to send data over the i2c
{
	if(sensors_to_read[currentSensor])
	{
		sensor_data[currentSensor] = ADCH;
		proper_sensor_data_old[currentSensor] = proper_sensor_data[currentSensor];
		proper_sensor_data[currentSensor] = sensorTabel(sensor_data[currentSensor],currentSensor);
	}
	currentSensor++;
	ADMUX = orgmux + currentSensor;
	if(currentSensor > NUMBER_OF_SENSORS)
	{
		currentSensor = 0;
		ADMUX = orgmux;
	}
	ADCSRA |= 1<<ADSC;
	controlSensor();
}
/*else
	{
		if(ADCH < OVER_MARKING)
		{
			counter++;
			last_marking = counter;
		}
		else
		{
			if(counter != 0)
			{
				data_to_send2 = tejpMarkering;
				PORTD = 0x10;
				PORTD = 0x00;
			}
			counter = 0;
		}
	}
}*/


void controlSensor()
{
	///////////////////////////////////////////////////////////
	//// These if statements determines the sensors that   ////
	////			 should update its values              ////
	///////////////////////////////////////////////////////////
	if( minimum_of(proper_sensor_data[S21],proper_sensor_data_old[S21]) == 0)
		sensors_to_read[S21] = false;
	if(minimum_of(proper_sensor_data[S22],proper_sensor_data_old[S22]) == 0)
		sensors_to_read[S22] = false;
	if(minimum_of(proper_sensor_data[S23],proper_sensor_data_old[S23]) == 0)
		sensors_to_read[S23] = false;
	if(minimum_of(proper_sensor_data[S24],proper_sensor_data_old[S24]) == 0)
		sensors_to_read[S24] = false;
	if(minimum_of(proper_sensor_data[S12],proper_sensor_data_old[S12]) > 25)
		sensors_to_read[S21] = true;
	if(minimum_of(proper_sensor_data[S13],proper_sensor_data_old[S13]) > 25)
		sensors_to_read[S24] = true;
	if(minimum_of(proper_sensor_data[S11],proper_sensor_data_old[S11]) > 25)
	{
		sensors_to_read[S22] = true;
		sensors_to_read[S23] = true;
	}
	//////////////////////////////////////////////////////////
}
/*void lookUpSensor()
{
	while(!sensors_to_read[currentSensor] && currentSensor < NUMBER_OF_SENSORS)
		currentSensor++;
	if(currentSensor < NUMBER_OF_SENSORS)
	{
		switch(currentSensor)
		{
			case S11:
			ADMUX = 0xE4;
			break;
			case S12:
			ADMUX = 0xE5;
			break;
			case S13:
			ADMUX = 0xE6;
			case S21:
			PORTD |= (0<<PORTD5);
			ADMUX = 0xE2;
			break;
			case S22:
			PORTD |= (1<<PORTD5);
			ADMUX = 0xE2;
			break;
			case S23:
			PORTD |= (0<<PORTD5);
			ADMUX = 0xE3;
			break;
			case S24:
			PORTD|= (0<<PORTD5);
			ADMUX = 0xE3;
			break;
		}
	}
}*/

/*uint8_t narrowing()
{
	if(proper_sensor_data[S22] - proper_sensor_data[S23] > 30);
		return 40;// Right side gets narrow
	else if(proper_sensor_data[S23] - proper_sensor_data[S22] > 30)
		return -40;// Left side gets narrow
	else if(proper_sensor_data[S22] = 0 && proper_sensor_data[S23] < 120)
		return 40;// Right side gets narrow
	else if(proper_sensor_data[S22] < 120 && proper_sensor_data[S23] = 0)
		return -40;// Left side gets narrow
	else 
		return 0;
}*/


ISR(INT0_vect)
{
	uint8_t cSREG;
	cSREG = SREG;
	
	int Angula_Rate;
	float Angle=0, Degree=0;
	
	styrmodul_interrupt = true;
	turn_90_done = false;
	PORTB &= ~(1<<PORTB0);  //Reset interrupt signal to Styrmodul, Pb0 = low

	TCNT0 = 0;   //set timer to 0
	while(!turn_90_done)
	{
		if(TCNT0 == 255 && !turn_90_done) // This happens every 0.01770833333s
		{
			Angula_Rate = SPI_GetRate();
			Angula_Rate = Angula_Rate - 1009;  // Use function SPI_GetNullvalue to get null value or set to 1009
			Angle = (float)((Angula_Rate/3.2)*0.01771);
			Degree += Angle;
		} // End if
		
		if(abs(Degree) >= 88) //Trial n error....delay between modules =>> 88 degree gives 90!! 
		{
			turn_90_done = 1;
			styrmodul_interrupt=0;
			PORTB |= (1<<PORTB0); // Signal to Styrmodul that turn_90 is done by set RB0 High
			break;
		}//End if
		
	} // End while

	SREG = cSREG;
} // End ISR INT2


ISR(ANALOG_COMP_vect)
{
	
	//over_mark = true;
	TCNT1 = 0; //Timer1 = 0 start to wait for next change in ACO

		if(step==0){
			TCNT1=0; //Timer1 = 0 start to wait for next change in ACO
			step=1;		
		}
		if(step==1){
			line1=TCNT1;
			TCNT1=0;
			step=2;
		}
		if(step==2){
			line_distance=TCNT1;
			TCNT1=0;
			step=3;
		}
		if(step==3){
			line2=TCNT1;
			TCNT1=0;
			
			if(line1 > (line_distance + (line_distance>>2))){   //Check what type of marking it is
				line1_small = false;
			}
			else line1_small = true;
			
			if(line2 > (line_distance + (line_distance>>2))){
				line2_small = false;
			}
			else line2_small = true;
			
			
			if(!line1_small | !line2_small){   //Om en av tjepbitarna är tjock (minst 25% störe än small) =>> det finns bara två tjepbitar, annars finns det en till att vänta på
				step=0;
				if(!line1_small && line2_small) {
					marking = 00;  //Header 0x0B, databyte 0x00	
					
				}
				
			}
		}
	
}


int8_t controlled_e(int16_t val)
{
	if(val < 120 && val > -120)
		return val;
	else if (val < -120)
		return -120;
	else
		return 120;
}

uint8_t minimum_of(uint8_t v1,uint8_t v2)
{
	if(v1 < v2)
		return v1;
	else
		return v2;	
}