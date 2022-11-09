/*-------------------------------------------------------------------------*\
| Datei:        main.c
| Version:      2.0
| Projekt:      Drehzahlmessung (Hall-Sensor)
|  
| Beschreibung: Mittels eines digitalen Hall Sensor werden die RPM gemessen,
|               diese werden dann im Programm verarbeitet und an der Anzeige
|               ausgegeben
| Schaltung:    MEGACARD V5.5
| Autor:        Pachler Fabio 
| Erstellung:
|
| Aenderung:    
|               
\*-------------------------------------------------------------------------*/

#include <avr/io.h>                                               
#include <avr/interrupt.h>
#include <util/delay.h>
#include "display.h"

volatile unsigned int rotation = 0;
volatile unsigned int rpm = 0;

ISR(INT2_vect)
{
	rotation++;
}

ISR(TIMER1_COMPA_vect)
{
	rpm = (rotation * 10) * 60;
	
	rotation = 0;
}

void interrupt_init()
{	
	DDRB &= ~(1<<PB2);
	PORTB &= ~(1<<PB2);
	
	//JTD ISC2 – JTRF WDRF BORF EXTRF PORF    MCUCSR
	// ?   0   ?  ?    ?    ?     ?    ?
	MCUCSR &= ~(1<<ISC2);
		
	//INT1 INT0 INT2 – – – IVSEL IVCE         GICR
	// ?    ?    1   ? ? ?   ?    ?
	GICR |= (1<<INT2);
}

void timer_init()
{
	OCR1A = 14999;
	
	//COM1A1 COM1A0 COM1B1 COM1B0 FOC1A FOC1B WGM11 WGM10       //TCCR1A
	//  ?      ?      ?      ?      ?     ?     0     0  
	TCCR1A &= ~((1<<WGM10) | (1<<WGM11));
	
	//ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10                  //TCCR1B 
	// ?     ?    ?  0      1    0    1    0
	TCCR1B &= ~((1<<WGM13) | (1<<CS12));
	TCCR1B |= (1<<WGM12) | (1<<CS11) | (1<<CS10);
	
	TIMSK |= (1<<OCIE1A);
}

int main (void)
{
	interrupt_init();
	timer_init();
	display_init();
	
	// Enable Interrupts globally
	sei();
	
	while(1)
	{
		_delay_ms(1000);
		
		display_clear();
		display_printf("RPM: %d", rpm);
	}
}
