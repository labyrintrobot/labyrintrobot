/*
 * init.c
 *
 * Created: 2013-12-11
 * Version: 1.0
 * Authors: Viktoria Nowén, Kim Persson
 *
 * Initierar portar, avbrott och PWM.
 */ 

#include <board.h>
#include <compiler.h>
#include <conf_board.h>
#include <stdio.h>



void board_init(void)
{
	
	DDRA = 0x00; // PORTA input
	DDRB = 0x4B; // PORTB, PB0,PB1,PB3,PB6 output
	DDRC = 0x00; // PORTC input
	DDRD = 0x31; // PortD, pin PD0,PD4,PD5 output
	PORTA = 0x00;
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;
	EIMSK = 0x04; // Tar emot avbrott
	EICRA = 0x30; // Avbrott stigande flank 
	
	TCCR1A = 0xA1; // PWM
	TCCR3A = 0xA1; // PWM
}
