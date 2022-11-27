/*-------------------------------------------------------------------------*\
| Datei:        twi_soft.h
| Version:      1.0
| Projekt:      I2C Softwareloesung Bibliothek
| Beschreibung: I2C in Software fuer die Anzeige an der MEGACARD
| Schaltung:    siehe twi_soft_port_def.h
| noch offen:   Clock-Stretching implementieren!
| 
| Autor:        D.I. Leopold Moosbrugger
| Erstellung:   02.03.2021 21:05:20
|
| Aenderung:
\*-------------------------------------------------------------------------*/

#ifndef TWI_SOFT_H_
#define TWI_SOFT_H_

#define  ACK 1
#define NACK 0

// Definition der Datentransfer-Richtung (lesen vom I2C Baustein)
#define I2C_READ    1

// Definition der Datentransfer-Richtung (schreiben in den I2C Baustein)
#define I2C_WRITE   0

/*-------------------------------------------------------------------------*\
| Initialisierung der beiden Pins
\*-------------------------------------------------------------------------*/
void twi_s_init(void);

/*-------------------------------------------------------------------------*\
| Legt eine  Start-Bedingung auf den Bus 
| (wird auch fuer twi_s_repstart verwendet)
\*-------------------------------------------------------------------------*/
uint8_t twi_s_start(uint8_t address);
#define twi_s_rep_start twi_s_start

/*-------------------------------------------------------------------------*\
| Legt eine  Stop-Bedingung auf den Bus
\*-------------------------------------------------------------------------*/
void twi_s_stop(void);

/*-------------------------------------------------------------------------*\
| Uebertraegt ein Byte an den Slave
\*-------------------------------------------------------------------------*/
uint8_t twi_s_write(uint8_t data);

/*-------------------------------------------------------------------------*\
| Liest ein Byte vom Slave
\*-------------------------------------------------------------------------*/
uint8_t twi_s_read(uint8_t ack);

#endif /* TWI_SOFT_H_ */