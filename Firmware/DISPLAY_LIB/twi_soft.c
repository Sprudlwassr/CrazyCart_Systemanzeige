/*-------------------------------------------------------------------------*\
| Datei:        twi_s_soft.c
| Version:      1.0
| Projekt:      I2C Softwareloesung Bibliothek
| Beschreibung: I2C in Software fuer die Anzeige an der MEGACARD
| Schaltung:    siehe twi_s_port_def.h
| noch offen:   Clock-Stretching implementieren!
| 
| Autor:        D.I. Leopold Moosbrugger
| Erstellung:   02.03.2021 21:05:43
|
| Aenderung:
\*-------------------------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>

// TWI Pins festlegen (Port und Pin)
#include "twi_soft_port_def.h"
#include "twi_soft.h"

#ifdef TWI_S_FAST
   // Pause TWI 400kHz / I2C Fast 
   // 1us
   #define TWI_S_DELAY1() _delay_us(0.7)
   // 2us
   #define TWI_S_DELAY2() _delay_us(1.5)
#else
   // Pause TWI 100kHz / I2C Standard
   // 1us
   #define TWI_S_DELAY1() _delay_us(3.0)
   // 2us
   #define TWI_S_DELAY2() _delay_us(6.0)
#endif

#define P_SCL_LIN (1<<PSCL_TWI_S)
#define P_SDA_LIN (1<<PSDA_TWI_S)


// Definitionen fuer die Tackt- und die Datenleitung
// Open Drain
//    Low  => DDR->Ausgang, PORT->Low (0)
//    High => DDR->Eingang, PORT->kein PU (0) 
#define SDA_PSEC  (PRT_TWI_S &= ~P_SDA_LIN)
#define SDA_HIGH  (DDR_TWI_S &= ~P_SDA_LIN)
#define SDA_LOW   (DDR_TWI_S |=  P_SDA_LIN)
#define SDA_IN    (PIN_TWI_S & P_SDA_LIN)

//#define SCL_PSEC  (PRT_TWI_S &= ~P_SCL_LIN)
//#define SCL_HIGH  (DDR_TWI_S &= ~P_SCL_LIN)
//#define SCL_LOW   (DDR_TWI_S |=  P_SCL_LIN)
//#define SCL_IN    (PIN_TWI_S & P_SCL_LIN)


// Definitionen fuer die Tackt- und die Datenleitung
// Aktive Leitunge (Testbetrieb ohne Baustein)
//    Low  => DDR->Ausgang, PORT->Low (0)
//    High => DDR->Ausgang, PORT->High (1)
//#define SDA_PSEC  (DDR_TWI_S |=  P_SDA_LIN)
//#define SDA_HIGH  (PRT_TWI_S |=  P_SDA_LIN)
//#define SDA_LOW   (PRT_TWI_S &= ~P_SDA_LIN)
//#define SDA_IN    (PIN_TWI_S & P_SDA_LIN)
//
#define SCL_PSEC  (DDR_TWI_S |=  P_SCL_LIN)
#define SCL_HIGH  (PRT_TWI_S |=  P_SCL_LIN)
#define SCL_LOW   (PRT_TWI_S &= ~P_SCL_LIN)
#define SCL_IN    (PIN_TWI_S & P_SCL_LIN)


// Init TWI
void twi_s_init(void)
{
/* SCL XXX/''''
   SDA XX/'''''
*/

   SDA_HIGH; // SDA_HIGH;
   SDA_PSEC;
   SCL_HIGH; // SCL_HIGH;
   SCL_PSEC;
}

// Start TWI
uint8_t twi_s_start(uint8_t address)
{
/* SCL ''''\...
   SDA '\......
*/
   SDA_PSEC;
   SCL_PSEC;
   SDA_LOW; TWI_S_DELAY2();
   SCL_LOW; TWI_S_DELAY2();
   
   return twi_s_write(address);
}

// Stop TWI
void twi_s_stop(void)
{
/* SCL ..../''''''
   SDA '\....../''
*/   
   SDA_PSEC;
   SCL_PSEC;
   SDA_LOW;  TWI_S_DELAY2();
   SCL_HIGH; TWI_S_DELAY2();
   SDA_HIGH; TWI_S_DELAY2();
}

// TWI schreiben
uint8_t twi_s_write(uint8_t data)
{
   uint8_t i;
   uint8_t hlp;

   SDA_PSEC;
   SCL_PSEC;
  
/* SCL .../'0'\../'1'\../'2'\../'3'\../'4'\../'5'\../'6'\../'7'\../'A\
   SDA 'X======X======X======X======X======X======X======X======/'''''
*/  
   for (i=0;i<8;i++)
   {  // 8 Bit ausgeben
      if ((data & (1<<7)) != 0)
         SDA_HIGH; // wenn Bit gesetzt
      else
         SDA_LOW;  // wenn Bit nicht gesetzt

      data <<= 1;  // Bit schieben, 7 bis 0
      TWI_S_DELAY1();
      SCL_HIGH;
      TWI_S_DELAY2();
      SCL_LOW;
   }

   SDA_HIGH;
   TWI_S_DELAY1();
   SCL_HIGH;
   TWI_S_DELAY1();

   if (SDA_IN == 0) // Testen SDA auf Acknowledge low
      hlp = ACK;    // Flag Ack setzen
   else
      hlp = NACK;   // sonst Flag Ack loeschen

   SCL_LOW;
   
   return hlp;
}

// TWI Byte lesen

uint8_t twi_s_read(uint8_t ack)
{
   uint8_t data = 0;
   uint8_t i;

   SDA_PSEC;
   SCL_PSEC;

/* SCL ./'0\../'1\../'2\../'3\../'4\../'5\../'6\../'7\../''\.
   SDA ===D=====D=====D=====D=====D=====D=====D=====D=A=====/
*/
   for (i=0;i<8;i++)
   {  // 8 Bit ausgeben
      data <<= 1;			// Bit schieben, 7 bis 0
      TWI_S_DELAY1();
      SCL_HIGH;
      TWI_S_DELAY1();
      if (SDA_IN != 0) // Testen SDA auf high
         data |= 1;    // wenn high, Bit 0 setzen
      SCL_LOW;
   }

   if (ack)
      SDA_LOW;  // Acknowledge
   else
      SDA_HIGH;  // No Acknowledge
   
   TWI_S_DELAY1();
   SCL_HIGH;
   TWI_S_DELAY2();
   SCL_LOW;
   SDA_HIGH;

   return data;  // Daten zurückgeben
}


