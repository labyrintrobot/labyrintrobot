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
#include "Turn_90.h"
#include "INT2_Enable.h"

//bool sensor_switch = false;


/*/========= Gyro variable ========== */


/*/===================================*/

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
	lin0 = 0x0C,
	lin1 = 0x0D,
	lin2 = 0x0E,
	lin3 = 0x0F,
	lin4 = 0x10,
	lin5 = 0x11,	
};

enum sensor_t currentSensor = S12;
bool sensors_to_read[NUMBER_OF_SENSORS];
uint8_t sensor_data[NUMBER_OF_SENSORS];
uint8_t proper_sensor_data[NUMBER_OF_SENSORS];
bool mux_sensors = false;
bool read_line = false;
uint8_t last_marking;
uint8_t counter;
bool recieve;
enum headers_t data_to_send = kortV;
enum headers_t data_to_send2 = tejpMarkering;
uint8_t recieve_header;
uint8_t recieve_data;
//void controlSensor();
//void lookUpSensor();
//void narrowing();



int main (void)
{
	board_init();		
	ADCsetup();
	Int2_setup();		//Enable Extern interrupt INT2
	SPI_Init();			//Init SPI bus for Gyro	
	

	TWI_slave_initialize(TWI_SENSOR_MODULE_ADDRESS);
	
	
	sensors_to_read[S12]=true;
	sensors_to_read[S21]=true;
	sensors_to_read[S22]=true;
	sensors_to_read[S11]=true;
	sensors_to_read[S23]=true;
	sensors_to_read[S24]=true;
	sensors_to_read[S13]=true;
	DDRB = 0xFF; // debugging
	DDRD = 0x2F; // mux signals
	sei(); // Allow interrupts
	TCNT1 = 0; //Start at 0
	TCCR1B |= (1<<CS12) | (1<<CS10); //Timer1 prescale
	
	ADMUX = 0xE5;
	

	
	
	do_turn_90();  // Testing Gyro
	
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
		//	ADMUX = 0xE4;
			ADCSRA |= 1<<ADSC; // Start a new conversion sweep
			TCNT1 = 0;
			PORTB = proper_sensor_data[S12];
			
		//	PORTB = PORTB ^ 0x01; //Toggle PB0 for test of timer 
		}		
	}
}

ISR(TWI_vect)
{
	TWI_slave_wait_for_address(&recieve);
	if(!recieve)
	{
		PORTD &= ~(1<<PORTD5); //reset request to send signal
		data_to_send= kortV;		
		TWI_slave_send_message(data_to_send,proper_sensor_data[data_to_send-4]);
		
	}
	
	/*
	if(!recieve)
	{
		PORTD &= ~(1<<PORTD5); //reset request to send signal
		if (data_to_send2 != tejpMarkering)
		{
			if(data_to_send < e)
			{
				TWI_slave_send_message(data_to_send,proper_sensor_data[data_to_send-4]);
				data_to_send++;
				PORTD |= (1<<PORTD5);
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
				data_to_send2 = tejpMarkering;
			}
			else
			{
				data_to_send = kortV;
				data_to_send2 = tejpMarkering;
			}
		}
		else
			TWI_slave_send_message(tejpMarkering,last_marking);
	}
	else
	{
		// Kalibreringsorder ska sätta alla sensorer to_read och kalibrera vinkelgivaren
		TWI_slave_receive_message(&recieve_header,&recieve_data);
	}*/
}

ISR(ADC_vect) // this interrupt will sweep all the sensors once and interpolate between the two closest points from a table,
			// when all the data has been read it will ask to send data over the i2c
{
	sensor_data[S12] = ADCH;
	proper_sensor_data[S12] = sensorTabel(sensor_data[12], S12);
	// Ask to send data
	PORTD |= (1<<PORTD5); //PD5= High
	
/*
	if(sensors_to_read[currentSensor])
	{
		sensor_data[currentSensor] = ADCH;
		proper_sensor_data[currentSensor] = sensorTabel(sensor_data[currentSensor],currentSensor);
	}
	// Ask to send data over I^2C
	PORTB = proper_sensor_data[S24];
	currentSensor++;
	switch(currentSensor)
	{
		case S11:
		ADMUX = 0xE6;
		break;
		
		case S12:
		ADMUX = 0xE4;
		break;
		
		case S13:
		ADMUX = 0xE5;
		break;
		
		case S21:
		PORTD &= ~(1<<PORTD4); //0x00;
		ADMUX = 0xE2;  //Sensor??? S21 ???
		break;
		
		case S22:
		PORTD |= (1<<PORTD4); //0x10; // OBS PD4 is mux address and PD5 is Interrupt to KM
		ADMUX = 0xE3;  //Sensor??? S23 ??? 
		break;
		
		case S23:
		PORTD &= ~(1<<PORTD4); //0x00;
		ADMUX = 0xE3;  //Sensor??? S22 ???
		break;
		
		case S24:
		PORTD |= (1<<PORTD4); //0x10;
		ADMUX = 0xE2;  //Sensor??? S24 ???
		break;
		
		default:
		currentSensor = S12;
		ADMUX = 0xE4;
		break;
	}
	if(currentSensor != S12)
		ADCSRA |= 1<<ADSC;
	// Send the regulation error over I^2C
	else
	{
		// Ask to send data
		PORTD |= (1<<PORTD5); //PD5= High
		currentSensor = S12;
		read_line = true;
		if(data_to_send == kortV)
			data_to_send2 = tejpMarkering;
	}*/
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


/*void controlSensor()
{
	///////////////////////////////////////////////////////////
	//// These if statements determines the sensors that   ////
	////			 should update its values              ////
	///////////////////////////////////////////////////////////
	if(proper_sensor_data[S12] > 25)
	sensors_to_read[S21] = true;
	if(proper_sensor_data[S13] > 25)
	sensors_to_read[S24] == true;
	if(proper_sensor_data[S11] > 25)
	{
		sensors_to_read[S22] = true;
		sensors_to_read[S23] = true;
	}
	if(proper_sensor_data[S21] = 255)
	sensors_to_read[S21] == false;
	if(proper_sensor_data[S22] = 255)
	sensors_to_read[S22] == false;
	if(proper_sensor_data[S23] = 255)
	sensors_to_read[S23] == false;
	if(proper_sensor_data[S24] = 255)
	sensors_to_read[S24] == false;
	///////////////////////////////////////////////////////////
}*/
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
