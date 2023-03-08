/*-------------------------------------------------------------------------------------------*\
| File:         main.c
| Version:      1.0
| Project:      display test
| Description:  test project for the LCD
| Circuit:      MEGACARD V5.5   -   for testing purposes
| Author:       Pachler Fabio
| Creation:     10.01.2023
|
| Last Changes: fixed LCD Lib for Display: MC42008A6W | state 1.02.2023
|
| Notes:
\*-------------------------------------------------------------------------------------------*/

#define F_CPU 12000000UL

//INCLUDE LIBS
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "./lcd/lcd.h"

int main (void)
{

	lcd_init(); //INIT FOR LCD

   lcd_home();
   lcd_string("TEST 1");
   lcd_cursor(0,1);
   lcd_string("TEST 2");
   lcd_cursor(0,2);
   lcd_string("TEST 3");
   lcd_cursor(0,3);
   lcd_string("TEST 4");

	while (1)
	{
      
	}

	return 0;
}
