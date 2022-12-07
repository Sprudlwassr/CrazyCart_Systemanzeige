/*-------------------------------------------------------------------------*\
| Datei:        main.c
| Version:      1.0
| Projekt:      Drehzahlmessung (Hall-Sensor)
|  
| Beschreibung: Mittels eines digitalen Hall Sensor werden die RPM gemessen,
|               diese werden dann im Programm verarbeitet und an der Anzeige
|               ausgegeben
| Schaltung:    MEGACARD V5.5
| Autor:        Pachler Fabio 
| Erstellung:
|
| Aenderung:    7.11.2022
|               Optimieren und Rechenfehler (Datentypen) ausbessern
\*-------------------------------------------------------------------------*/

#include <avr/io.h>                                               
#include <avr/interrupt.h>
#include <util/delay.h>
#include "./oled/display.h"

/*-------------------------------------------------------------------------*\
   ^
 H |________     _____________   ______________________     ______________
   |        |___|                                      |___|
   +--------------------------~~~----------------------------------------->
            ISR                                       ISR                t
             <---       eine Umdrehung              -->
   ^
   |                                |         |                         
   +--------------------------~~~----------------------------------------->
                                   ISR-OVF1 ISR-OVF2                     t

   elapsed_count = ovf_count * 65536UL  +  stop_count  
      (uint32_t)   (uint8_t)  (uint32_t)   (uint16_t)                            

\*-------------------------------------------------------------------------*/

// Variablen fuer die beiden ISR
volatile uint16_t stop_count = 0;      // Datenuebergabe: ISR Stop Zaehler
volatile uint8_t  ovf_count = 0;       // Datenuebergabe: ISR overflow Zaehler

volatile uint8_t  TCN_ovf = 0;         // ISR overflow Zaehler

ISR(INT2_vect)
{
   // Uebergabe der Messwerte
   stop_count = TCNT1;
   ovf_count  = TCN_ovf;

   // Start der neuen Messung
   TCNT1   = 0x0000; 
   TCN_ovf = 0;

   // TOV1 can be cleared by writing a logic one to its bit location
   // TIFR: OCF2 TOV2 ICF1 OCF1A OCF1B TOV1 OCF0 TOV0 
   //        ?    ?    ?     ?    ?     1    ?    ?
   TIFR |= (1<<TOV1);
}

ISR(TIMER1_OVF_vect)    // Timer1 Overflows hochzaehlen
{
   TCN_ovf++;
}

void timer_init()      // initialisierung des timers
{
   // Normal Mode; CH-A no Output; CH-B no Output
   // Pre-Scaler 1/64; Timer1 Overflow-Interrupt
   
   // TCCR1A: COM1A1 COM1A0 COM1B1 COM1B0 FOC1A FOC1B WGM11 WGM10
   //           0      0      0      0      0     0     0     0
   TCCR1A = 0x00;
   // TCCR1B: ICNC1 ICES1   –   WGM13 WGM12 CS12 CS11 CS10 
   //           0     0     0     0     0     0    1    1
   TCCR1B = (1<<CS10) | (1<<CS11);
   
   // TIMSK: OCIE2 TOIE2 TICIE1 OCIE1A OCIE1B TOIE1 OCIE0 TOIE0 
   //          ?     ?     ?      ?      ?      1     ?     ?
   TIMSK |= (1<<TOIE1);
}

void interrupt_init (void) // initialisierung des interrupts
{
   DDRB  &= ~(1<<PB2);
   PORTB |=  (1<<PB2);     // Port PB2 mit Pullup schalten (notwendig warum?)

   // MCUCSR: JTD ISC2  –  JTRF WDRF BORF EXTRF PORF 
   //          ?   0    ?   ?    ?    ?    ?     ?
   MCUCSR &= ~(1<<ISC2);

   // GICR: INT1 INT0 INT2  –   –   –  IVSEL IVCE 
   //         ?    ?    1   ?   ?   ?    ?     ?
   GICR |= (1<<INT2);         // externer Interrupt 2 aktivieren
 }

int main (void)
{
   unsigned int rpm;


   uint32_t elapsed_count;
   uint16_t stop_count_l;     // ISR Stop Zaehler
   uint8_t ovf_count_l;       // ISR overflow

   interrupt_init();
   timer_init();
   display_init();  //initialisierung der Anzeige

   sei();           //Alle Interrupts ein

   while (1)                               
   {
      // Kritischer Abschnitt (Anfang; alle Interrupts aus)
      cli();
      stop_count_l  = stop_count;   // ISR Stopzeit
      ovf_count_l   = ovf_count;    // ISR overflow
      sei();
      // Kritischer Abschnitt (Ende; alle Interrupts wieder ein)

      // Berechnung der Zeit zwischen den Impulsen
      elapsed_count = (uint32_t)stop_count_l + ((uint32_t)ovf_count_l * 65536UL);
      rpm = 171.661 / elapsed_count;          // Berechnung der RPMs

      display_clear();                        // Anzeige loeschen
      display_printf("RPM: %d", rpm);         // Die RPMs an der Anzeige ausgeben
      display_printf_pos(0, 1,"elapsed: %d", elapsed_count);         
      display_printf_pos(0, 2,"count: %d", stop_count);         

      _delay_ms(1000);
   }
}
