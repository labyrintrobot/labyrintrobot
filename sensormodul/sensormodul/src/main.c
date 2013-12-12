
/* main.c
/ Dai Duong Trinh, Hampus Nilsson
/ Version 1.5
/ 2013-12-11
/
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
volatile uint8_t step=0,  marking;
volatile uint16_t line1, line2, line3, line_distance=0;

#define NUMBER_OF_SENSORS 7

int8_t weight();
uint8_t minimum_of(uint8_t v1,uint8_t v2);
void controlSensor();
int8_t controlled_e(int16_t val);


// Headers för att skicka data
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
	tejpMarkering = 0x0B
};

enum linjeCH_t { CH2=0x10, CH3=0x20, CH4 = 0x30, CH5 = 0x40, CH6 = 0x50, CH7 = 0x60, CH8 = 0x70, CH9=0x80, CH10=0x90};
//X1[DCBA] CH2 = 0001 = 0x00 ========X2[DCBA] CH3 = 0010 = 0x02 ======X3[DCBA]  CH4 = 0011 = 0x30=======
//X4[DCBA] CH5 = 0100 = 0x40;========X5[DCBA] CH6 = 0101 = 0x50 ======X6[DCBA]  CH7 = 0110 = 0x06=======
//X7[DCBA] CH8 = 0111 = 0x70=========X8[DCBA] CH9 = 1000 = 0x00 ======X9[DCBA] CH10 = 1001 = 0x90=======
//Legend: Xn is output port of Mux, [DCBA] is address port of Mux to get Xn, CHn is IR array channel

bool sensors_to_read[NUMBER_OF_SENSORS];
uint8_t sensor_data[NUMBER_OF_SENSORS];
uint8_t proper_sensor_data[NUMBER_OF_SENSORS];
uint8_t proper_sensor_data_old[NUMBER_OF_SENSORS];
bool read_line = false;
bool stop_sending = false;

bool recieve;
bool check;
bool finish_found = false;
enum headers_t data_to_send = kortV;
uint8_t recieve_header;
uint8_t recieve_data;
uint8_t orgmux = 0xE0;
uint8_t currentSensor = 0;



uint8_t line_sensors[9];
uint8_t current_line = 0;

int16_t regulation_error;
uint8_t linmux = 0x60;


int main (void)
{
	board_init();
	ADCsetup();
	TWI_slave_initialize(TWI_SENSOR_MODULE_ADDRESS);
	TCCR1B |= (1<<CS12) | (1<<CS10); //Timer1 Init , Prescale = 1024
	TCCR3B |= (1<<CS31) | (1<<CS30); //Timer3 Init , Prescale = 1024
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
	//============= Init Analog Compare =========================
	ACSR |= (1<<ACIE);  //Enable AC int, default mode output toggle.
	
	//============Setup timer0 and start timer, PB0 as Output and low ================
	Timer0_setup_start();   // This function is in INT0_Enable.h

	
	ADMUX = 0xE0;
	ADCSRA |= 1<<ADSC;
	//Set Mux_sel port as output. Mux_sel[3:0] @ PD[7:4] and low
	DDRD |= 0xF0;
	PORTD &= 0x0F;
	
	
	////////////////////////////_____Main loop_____///////////////////////////////////////////

	while(1)
	{
		// Läs av microswitchen för att stänga av linjeföljning när objekt är funnet
		check = PINB&0x02;
		if(check == 0)
		read_line = false;
		if(TCNT3 >=46079) // 0.2sec, reset stat-machine for line marking. For fault reading.
		{
			TCNT3 = 0;
			step = 0;
			marking = 0;
			stop_sending = false;
			ADCSRA |= 1<<ADSC;
		}
		// om vi ska läsa av linjeföljaren så updaterar vi värdena från linjesensorn med en frekvens på 1.44 kHz
		if(read_line)
		{
			if(TCNT1%9==0)
			{
				ADCSRA |= 1<<ADSC;
			}
		}
		
		// Begär att skicka data över TWI bussen med en frekvens på 10 Hz
		if(TCNT1 >= 1439 && !stop_sending)
		{
			PORTD |= (1<<PORTD0);  ///PD0 high, request to send data using TWI bus
			TCNT1 = 0;              //reset timer
		}
	}
}
////////////////////////////__END___Main loop_____///////////////////////////////////////////


///////////////////////////____TWI___INTERRUPTS_____////////////////////////////
// Hampus Nilsson 11/12-13 v1.11
ISR(TWI_vect)
{
	TWI_slave_wait_for_address(&recieve);
	if(!recieve)
	{
		PORTD &= ~(1<<PORTD0); //PD0 low, reset TWI send-request
		//================Send tjepmarkering========================
		if(over_mark)
		{
			TWI_slave_send_message(tejpMarkering, marking);
			marking = 0;
			over_mark=0;
			
		}
		//================ End Send tjepmarkering ==================
		if(!read_line)
		{
			if(data_to_send < e)
			{
				TWI_slave_send_message(data_to_send, minimum_of(proper_sensor_data[data_to_send-3],proper_sensor_data_old[data_to_send-3]));
				data_to_send++;
				PORTD |= (1<<PORTD0); //PD0 high, request to send more data
			}
			
			//  Beräkna reglerfelet beroende på vilka sensorer som är aktiva
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
		}
		else
		TWI_slave_send_message(e,weight());
	}
	else
	{
		// Kalibreringsorder ska sätta alla sensorer to_read och nollställa linjeföljning
		if(recieve_header == 0x02)
		{
			sensors_to_read[S12]=true;
			sensors_to_read[S21]=true;
			sensors_to_read[S22]=true;
			sensors_to_read[S11]=true;
			sensors_to_read[S23]=true;
			sensors_to_read[S24]=true;
			sensors_to_read[S13]=true;
			read_line = 0;
		}
	}
	// Läs av microswitchen för att stänga av linjeföljning när objekt är funnet
	check = PINB&0x02;
	if(check == 0)
	read_line = false;
	//	over_mark=0;
}



////////////////////////___ADC___INTERRUPTS___///////////////////////////////////
// this interrupt will sweep all the sensors and interpolate between the two closest points from a table,
// when all the data has been read it will update the control of the sensors
// Hampus Nilsson 10/12-13 v1.5

ISR(ADC_vect)
{
	// Läs av microswitchen för att stänga av linjeföljning när objekt är funnet
	check = PINB&0x02;
	if(check == 0)
	read_line = false;
	ADMUX = orgmux;
	if(!read_line)
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
		if(!stop_sending)
		ADCSRA |= 1<<ADSC;
		controlSensor();
	}
	else
	{
		// linmux ändrar så ref värdet blir 5 V istället för 2.56 V
		ADMUX = linmux + 7;
		line_sensors[current_line] = ADCH;
		current_line++;
		if(current_line >= 9)
		{
			current_line = 0;
		}
		switch(current_line)
		{
			case 0:
			PORTD  = (PORTD & 0x0F)| CH2;
			break;
			
			case 1:
			PORTD  = (PORTD & 0x0F)| CH3;
			break;
			
			case 2:
			PORTD  = (PORTD & 0x0F)| CH4;
			break;
			
			case 3:
			PORTD  = (PORTD & 0x0F)| CH5;
			break;
			
			case 4:
			PORTD  = (PORTD & 0x0F)| CH6;
			break;
			
			case 5:
			PORTD  = (PORTD & 0x0F)| CH7;
			break;
			
			case 6:
			PORTD  = (PORTD & 0x0F)| CH8;
			break;
			
			case 7:
			PORTD  = (PORTD & 0x0F)| CH9;
			break;
			
			case 8:
			PORTD  = (PORTD & 0x0F)| CH9;
			break;
			
			case 9:
			PORTD  = (PORTD & 0x0F)| CH10;
			break;
			
			default:
			PORTD  = (PORTD & 0x0F)| CH2;
			break;
		}
	}
	
}

///////////////////////////////////////////////////////////
////     This function determines the sensors that     ////
////			 should update its values              ////
///////////////////////////////////////////////////////////
// Hampus Nilsson 28/11-13 v1.2
void controlSensor()
{
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


////////////////////___EXTERN___INT___GYRO___TURN_90___/////////////////////////
// Dai Trinh 23/11-13 v1.1
ISR(INT0_vect)
{

	read_line = false;
	ADMUX = orgmux;
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
		
		if(abs(Degree) >= 85) //Trial n error....(delay between modules and traction) =>> 85 degree gives 90!
		{
			turn_90_done = 1;
			styrmodul_interrupt=0;
			PORTB |= (1<<PORTB0); // Signal to Styrmodul that turn_90 is done by set RB0 High
			break;
		}//End if
		
	} // End while

	SREG = cSREG;
	ADCSRA |= 1<<ADSC;
} // End ISR INT2

////////////////////___AC_COMP_INT___READ_MARKING___///////////////////////////
// Dai Trinh 09/12-13 v1.1
ISR(ANALOG_COMP_vect)
{
	// Läs av microswitchen för att stänga av linjeföljning när objekt är funnet
	check = PINB&0x02;
	if(check == 0)
	read_line = false;
	regulation_error = 0;
	stop_sending = true;
	switch(step)
	{
		case 0:
		TCNT3=0; //Timer1 = 0 start to wait for next change in ACO
		step=1;
		break;
		
		case 1:
		line1=TCNT3;
		TCNT3=0;
		step=2;
		break;
		
		case 2:
		line_distance=TCNT3;
		TCNT3=0;
		step=3;
		break;
		
		case 3:
		line2=TCNT3;
		TCNT3=0; //not needed here, we already have the distance between two lines.
		
		if(line1 > (line_distance + (line_distance>>1)))
		{   //Check what type of marking it is
			line1_small = false;
		}
		else
		line1_small = true;
		
		if(line2 > (line_distance + (line_distance>>1)))
		{
			line2_small = false;
		}
		else
		line2_small = true;
		
		
		if(!line1_small || !line2_small)
		{
			//Om en av tjepbitarna är tjock (minst 50% störe än small) =>> det finns bara två tjepbitar, annars finns det en till att vänta på
			step=0;
			if(!line1_small && line2_small)
			{ // set databyte to send begär sendning via i2c bus
				marking = 0x00;  //Header 0x0B, databyte 0x00	(Tjockt Smal) = Håll till höger
			}
			else if(line1_small && !line2_small)
			{
				marking = 0x01;  // Header 0x0B, databyte 0x01  (Smal Tjock) = Håll till vänster
			}
			else if(!line1_small && !line2_small)
			{
				marking = 0x02;  // Header 0x0B, databyte 0x02  (Tjock Tjock) = Start markering
			}
			over_mark=1;
			stop_sending = false;
			ADCSRA |= 1<<ADSC;
			PORTD |= (1<<PORTD0);  ///PD0 high, request to send data using I2C bus
		}
		else
		step = 4;
		break;
		
		case 4:
		step=5;
		ADCSRA |= 1<<ADSC;
		break;
		
		case 5:
		step=0;
		marking=0x03;
		over_mark=1;
		stop_sending = false;
		ADCSRA |= 1<<ADSC;
		PORTD |= (1<<PORTD0);
		if(!finish_found)
		{
			read_line = true;
			finish_found = true;
		}
		ADMUX = linmux+7;
		break;
	}
}

// Hampus Nilsson 27/11-13 v1.0
int8_t controlled_e(int16_t val)
{
	if(val > 120)
	return 120;
	else if (val < -120)
	return -120;
	else
	return val;
}

// Hampus Nilsson 02/12-13 v1.0
uint8_t minimum_of(uint8_t v1,uint8_t v2)
{
	if(v1 < v2)
	return v1;
	else
	return v2;
}
// dela "vikten"(resultatet från adc) från linje sensorerna med 255 för att få ett procentuelt värde över hur pass mycket över
// en sensor är linjen gångra sedan det värdet med ett värde som blir större ju längre ifrån mitten sensorn ligger
// Hampus Nilsson 09/12-13 v1.1
int8_t weight()
{
	float vikt = 0;
	vikt = (line_sensors[0]*40)/255+(line_sensors[1]*30)/255+(line_sensors[2]*20)/255+(line_sensors[3]*10)/255+
	(line_sensors[4]*(0))/255+(line_sensors[5]*(-10))/255+(line_sensors[6]*(-20))/255+(line_sensors[7]*(-30))/255+
	(line_sensors[8]*(-40))/255;
	return controlled_e((int16_t)vikt);
}
