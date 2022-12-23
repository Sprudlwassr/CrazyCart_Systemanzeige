/*-------------------------------------------------------------------------*\
| Datei:        main.c
| Version:      1.0
| Projekt:      DS18b20 Temp. Sensor
| Beschreibung: Auswerten und Ausgeben von Temperaturen mittels DS18B20
| Schaltung:    MEGACARD V5.5
| Autor:        Pachler Fabio 
| Erstellung:   30.11.2022
|
| Aenderung:
\*-------------------------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>
#include "./oled/display.h"
#include "./ds18b20/ds18b20.h"

void init (void)
{
   
}

int main (void)
{
   init ();        // Aufruf der Grundinitialisierungen
   display_init(); // Initialisierung der Anzeige
   
   uint8_t temp = 0;

   while (1)
   {
      temp = ds18b20_gettemp();

      display_clear();
      display_printf("Temperatur:%d", temp);
      _delay_ms(1000);
   }

   return 0;
}
