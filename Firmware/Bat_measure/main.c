/*-------------------------------------------------------------------------*\
| File:        main.c
| Version:      1.0
| Projekt:      battery measure
| Description:  test program for bat measurement
| Circuit:      MEGACARD V5.5
| Author:	    Pachler Fabio
| Creation:	    14.12.2022
|
| Change:
\*-------------------------------------------------------------------------*/

#include <avr/io.h>
#include <util/delay.h>
#include "adc/adc.h"
#include "lcd/lcd.h"

int main (void)
{
   lcd_init();
   adc_init();
   
   adc_mode(0x00);
   adc_channel(0x00);
   uint16_t adc_value = 0;
   uint8_t bat = 0;
   
//    VALUES FROM 670 - 860 (AVERAGE VALUES, NEEDS TO BE SLIGTHLY CHANGED)
//    Prozent    Wert
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
   
   while (1)
   {
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

	   lcd_home();
	   lcd_string("Spannung: ");
	   lcd_ul2ascii(((adc_value * 50) / 1024), 10, 4);
	   lcd_cursor(0,1);
	   lcd_string("Ladung: ");
	   lcd_ul2ascii(bat, 10, 3);
	   lcd_string("%");

       _delay_ms(1000);
   }

   return 0;
}
