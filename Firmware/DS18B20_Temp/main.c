/*-------------------------------------------------------------------------*\
| File:         main.c
| Version:      1.0
| Project:      DS18b20 Temp. Sensor
| Description:  test program for the sensor DS18B20
| Cirucit:      MEGACARD V5.5
| Author:       Pachler Fabio 
| Creation:     30.11.2022
|
| Change:
\*-------------------------------------------------------------------------*/

//DEFINE CLOCK FREQUENCY
   #define F_CPU 16000000UL
//////////

//INCLUDE LIBS
   #include <avr/io.h>
   #include <avr/interrupt.h>
   #include <util/delay.h>

   #include "lcd/lcd.h"
   #include "ds18b20/ds18b20.h"
//////////


int main (void)
{
   //INITS
      lcd_init();       //INIT FOR LCD
   //////////

   //TEMPERATURE MEASURE INIT
      uint8_t temp = 0;
   //////////
 
   while (1)
   {

      //OUTPUT ON DISPLAY
         lcd_home();
         lcd_string("Temperatur: ");
         lcd_ul2ascii(temp, 10, 3);
         lcd_string("C");
      //////////
      
      _delay_ms(2000);
   }

   return 0;
}
