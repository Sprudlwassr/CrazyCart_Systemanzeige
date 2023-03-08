/*-------------------------------------------------------------------------*\
| File:         main.c
| Version:      1.0
| Project:      release of the program "Systemanzeige"
| Description:  first program release for the project "Systemanzeige"
| Circuit:      MEGACARD V5.5
| Author:       Pachler Fabio
| Creation:     4.03.2023
|
| Changes:      fixed calculation error for kmh calculation, error was
|               at a factor of 1.75
\*-------------------------------------------------------------------------*/

//DEFINE CLOCK FREQUENCY
#ifndef F_CPU
   #define F_CPU 16000000UL
#endif
//////////

//INCLUDE LIBS
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lcd/lcd.h"
#include "temp/ds18b20.h"
#include "adc/adc.h"
//////////

//ASIGN VARIABLES FOR TACHOMETER
volatile unsigned int Rotationen = 0;
volatile unsigned int RPM = 0;
volatile unsigned int KMH = 0;
//////////

//ROTATION COUNTING
ISR(INT0_vect)
{
   Rotationen++;                 //COUNT ROTATIONS
}
//////////

//INIT FOR ISR
void interrupt_init (void)
{
   DDRD &= ~(1<<PD2);            //PORT FOR INTERRUPT TO INPUT
   PORTD |= (1<<PD2);            //SET PULLUP FOR PORT
   GICR |= (1<<INT0);            //INTERRUPT 0 ENABLE
   
   MCUCR &= ~(1<<ISC00);
   MCUCR |= (1<<ISC01);          //TRIGGER ON FALLING EDGE

   sei();                        //ENABLE INTERRUPTS GLOBALLY
}
//////////

int main (void)
{
   //INITS
   lcd_init();       //INIT FOR LCD
   interrupt_init(); //INIT FOR INTERRUPT
   adc_init();       //INIT FOR ADC
   //////////

   //TEMPERATURE MEASURE INIT
   uint8_t temp = 0;
   //////////

   //ADC FOR BAT MEASURE
   adc_mode(0x00);
   adc_channel(ADC_CH0);
   uint16_t adc_value = 0;
   uint8_t bat = 0;
   //////////

   //ENABLE INTERRUPTS GLOBALLY
   sei();
   /////////
   
   while (1)
   {
      //RPM MEASURE
      cli();                                 //DISABLE INTERRUPTS GLOBALLY
      RPM = (Rotationen / 4) * 60;           //CALCULATE RPM
      Rotationen = 0;                        //RESET ROTATIONS
      sei();                                 //ENABLE INTERRUPTS GLOBALLY
      /////////

      //KMH CALCULATION
      KMH = (37698 * (uint16_t)RPM) / 10000UL;    //RPM -> KMH(formula: (3/25) * ? * r * RPM)
      KMH = KMH / 175;                            //FIX FOR CALCULATION ERROR
      /////////

      //TEMPERATURE MEASURE
      temp = ds18b20_gettemp();
      //////////

      //BAT MEASURE
      //    VALUE RANGE 670 - 860 (!!AVERAGE BAT!!, REAL VALUES WILL BE MEASURED!, )
      //    PERCENT    VALUE
      //    100%   -  1020
      //    90%    -  1006
      //    80%    -  992
      //    70%    -  978
      //    60%    -  962
      //    50%    -  948
      //    40%    -  934
      //    30%    -  920
      //    20%    -  906
      //    10%    -  892
      //    0%     -  875

      adc_value = adc_read();
      //unsigned int voltage_mv = ((5000UL * adc_value)>>10);
      //double voltage = (adc_value * 48828125UL) / 100000000UL;

      
      if(adc_value <= 848)
      {
         bat = 0;
      }
      
      if(adc_value >= 879)
      {
         bat = 20;
      }
      
      if(adc_value >= 906)
      {
         bat = 40;
      }
      
      if(adc_value >= 934)
      {
         bat = 60;
      }
      
      if(adc_value >= 962)
      {
         bat = 80;
      }
      
      if(adc_value >= 990)
      {
         bat = 100;
      }
      //////////
      
      //OUTPUT ON DISPLAY
      lcd_home();
      lcd_string("KMH: ");
      lcd_ul2ascii(KMH, 10, 3);
      lcd_cursor(0, 1);
      lcd_string("RPM: ");
      lcd_ul2ascii((RPM), 10, 4);
      lcd_cursor(0, 2);
      lcd_string("Temperatur: ");
      lcd_ul2ascii(temp, 10, 3);
      lcd_string("C");
      lcd_cursor(0, 3);
      lcd_string("Batterie: ");
      lcd_ul2ascii(bat, 10, 3);
      lcd_string("%");
      //////////
      
      //DELAY FOR RPM MEASURE AND EVERYTHING ELSE
      _delay_ms(1000);
      //////////
   }

   return 0;
}
