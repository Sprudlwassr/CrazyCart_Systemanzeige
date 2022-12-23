/*-------------------------------------------------------------------------*\
| Datei:        main.c
| Version:      1.0
| Projekt:      Batterie Messung
| Beschreibung: Programm um die Spannung der Batterie zu messen
| Schaltung:    MEGACARD V5.5
| Autor:		Pachler Fabio
| Erstellung:	14.12.2022
|
| Aenderung:
\*-------------------------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>
#include "./oled/display.h"
#include "./adc/adc.h"
//#include "./lcd/lcd.h"

int main (void)
{
   display_init();
   
   adc_init();
   adc_mode(0x00);
   adc_channel(0x00);
   uint16_t adc_value = 0;
   uint8_t bat = 0;
   
//    Werte reichen von 670 - 860 (Durchschnitt, Werte werden noch gemessen)
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

       display_clear();
       display_printf_pos(0, 0, "Spannung: %u", ((adc_value * 50) / 1024));
       display_printf_pos(0, 1, "Batterie: %u %%", bat);
       _delay_ms(1000);
   }

   return 0;
}
