/*-------------------------------------------------------------------------*\
| Datei:        twi_s_port_def.h
| Version:      1.0
| Projekt:      I2C Softwareloesung Bibliothek
| Beschreibung: I2C in Software fuer die Anzeige an der MEGACARD
| Schaltung:    SCL PB0, SDA PB1
| 
| Autor:        D.I. Leopold Moosbrugger
| Erstellung:   02.03.2021 21:05:20
|
| Aenderung:
\*-------------------------------------------------------------------------*/

#ifndef TWI_SOFT_PORT_DEF_H_
#define TWI_SOFT_PORT_DEF_H_

// TWI_S_STD  100kHz
// TWI_S_FAST 400kHz
#define TWI_S_FAST

// TWI Pins festlegen (Port und Pin)
// beliebige Pins, aber auf dem gleichen Port
#define DDR_TWI_S DDRB
#define PRT_TWI_S PORTB
#define PIN_TWI_S PINB

#define PSCL_TWI_S 0
#define PSDA_TWI_S 1

#endif /* TWI_SOFT_PORT_DEF_H_ */