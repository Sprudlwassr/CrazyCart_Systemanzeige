/*-------------------------------------------------------------------------*\
| File:         main.c
| Version:      1.0
| Project:      tachometer simple
| Description:  simple version of the tachometer
|
| Circuit:      MEGACARD V5.5
| Author:       Pachler Fabio
| Creation:     15.8.2022
|
| Change:       1.3.2023
\*-------------------------------------------------------------------------*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "oled/display.h"


volatile unsigned int Rotationen = 0;
volatile unsigned int RPM = 0;

//ROTATION COUNTING
ISR(INT0_vect)
{
	Rotationen++;
}
//////////

//INIT FOR ISR
void interrupt_init (void)
{
	DDRD &= ~(1<<PD2);
	PORTD |= (1<<PD2);               
	GICR |= (1<<INT0);            //INTERRUPT 1 ENABLE


   //MCUCSR &= ~(1<<ISC2)	
 	MCUCR &= ~(1<<ISC00);
 	MCUCR |= (1<<ISC01);          //TRIGGER ON FALLING EDGE

	sei();                        //ENABLE INTERRUPTS GLOBALLY
}
//////////

int main(void)
{

	interrupt_init();
	display_init();                        


	while (1)

	{
		
		_delay_ms(2000);
		
		cli();                                 //DISABLE INTERRUPTS GLOBALLY
		RPM = Rotationen * 30;                 //CALCULATE RPM

		display_printf_pos(0,0,"RPM: %u", RPM);
		//lcd_ul2ascii(RPM, 10, 4);            //PRINT RPM ON DISPLAY

		Rotationen = 0;                        //RESET VARIABLE
		sei();                                 //ENABLE INTERRUPTS GLOBALLY
		
	}
}