/*-------------------------------------------------------------------------------------------*\
| File:         main.c
| Version:      1.0
| Projekc:      "Systemanzeige"
| Description:  First Beta of the Software for the projekt "Systemanzeige"
| Circuit:      MEGACARD V5.5   -   for testing purposes
| Author:       Pachler Fabio
| Creation:     10.01.2023
|
| Last Changes: Imported Code from all other Test-projects to one project | state 10.01.2023
|
| Notes: Tachometer Part not working | state 10.01.2023
\*-------------------------------------------------------------------------------------------*/

//INCLUDE LIBS
   #include <avr/io.h>
   #include <avr/interrupt.h>
   #include <util/delay.h>
   #include "./oled/display.h"
   #include "./ds18b20/ds18b20.h"
   #include "./adc/adc.h"
   //#include "./lcd/lcd.h"  <- FOR LATER USE (INSTEAD OF OLED)
//////////


//VARIABLES FOR ISR
   volatile uint16_t stop_count = 0;      // DATATRANSFER: ISR STOP-COUNTER
   volatile uint8_t  ovf_count = 0;       // DATATRANSFER: ISR OVERFLOW-COUNTER

   volatile uint8_t  TCN_ovf = 0;         // ISR OVERFLOW-COUNTER
//////////

//ISR FOR TACHOMETER
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
//////////

//TIMER FOR TACHOMETER
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
//////////

//INTERRUPT INIT FOR TACHOMETER
   void interrupt_init (void)
   {
      DDRB  &= ~(1<<PB2);

      // MCUCSR: JTD ISC2  –  JTRF WDRF BORF EXTRF PORF
      //          ?   0    ?   ?    ?    ?    ?     ?
      MCUCSR &= ~(1<<ISC2);

      //ENABLE INTERRUPT 2
      // GICR: INT1 INT0 INT2  –   –   –  IVSEL IVCE
      //         ?    ?    1   ?   ?   ?    ?     ?
      GICR |= (1 << INT2); 
   }
//////////


int main (void)
{
   display_init(); //INIT FOR OLED
   
   //TACHOMETER
      unsigned int rpm = 0;

      uint32_t elapsed_count;
      uint16_t stop_count_l;     // ISR Stop Zaehler
      uint8_t ovf_count_l;       // ISR overflow

      interrupt_init();
      timer_init();
   //////////

   //TEMPERATURE MEASURE
      uint8_t temp = 0;
   //////////

   //ADC FOR BAT MEASURE
      adc_init();
      adc_mode(0x00);
      adc_channel(0x00);
      uint16_t adc_value = 0;
      uint8_t bat = 0;
   
   //    VALUE RANGE 670 - 860 (!!AVERAGE BAT!!, REAL VALUES WILL BE MEASURED!)
   //    PERCENT    VALUE
   //    100%   -	  860
   //    90%    -	  842
   //    80%    -	  823
   //    70%    -	  809
   //    60%    -	  793
   //    50%    -	  786
   //    40%    -    778
   //    30%    -    772
   //    20%    -    764
   //    10%    -    756
   //    0%     -    670
   //////////

   //ENABLE INTERRUPTS GLOBALLY
      sei();
   //////////

   while (1)
   {
      
      //CRITICAL SECTION
         cli();
         stop_count_l  = stop_count;   // ISR Stopzeit
         ovf_count_l   = ovf_count;    // ISR overflow
         sei();
      //////////

      //////////
      //       CALCULATION:
      //       READING -> RPM -> KM/H
      //       READING * PERIOD DURATION = TIME FOR ONE REVOLUTION (tu)
      //       1/tu = rps * 60 = rpm
      //       km/h = 0.1885 * rpm * dm    (dm = DIAMETER TIRE)
      //////////

      //TIME CALCULATION FOR TACHOMETER
         elapsed_count = (uint32_t)stop_count_l + ((uint32_t)ovf_count_l * 65536UL);

         //RPM CALCULATION
         rpm = (60 /(elapsed_count * 5333)) / 100000; 
      //////////

      //TEMPERATURE MEASURE
         temp = ds18b20_gettemp();
      //////////

      //BAT MEASURE
         adc_value = adc_read();
         
         if(adc_value > 670)
         {
            bat = 20;
         }
         
         if(adc_value > 764)
         {
            bat = 40;
         }
         
         if(adc_value > 778)
         {
            bat = 60;
         }
         
         if(adc_value > 793)
         {
            bat = 80;
         }
         
         if(adc_value > 823)
         {
            bat = 100;
         }
      //////////

      //PRINT ON DISPLAY
         display_clear();
         display_printf_pos(0, 0, "RPM: %u", rpm);
         display_printf_pos(0, 1, "Temperatur:%d", temp);
         display_printf_pos(0, 2, "Spannung: %u", ((adc_value * 50) / 1024));
         display_printf_pos(0, 3, "Batterie: %u %%", bat);
      //////////

      _delay_ms(1000);
   }

   return 0;
}
