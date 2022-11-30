/*-------------------------------------------------------------------------*\
| Datei:        FontData.h
| Version:      1.0
| Projekt:      Display-Bibliothek fuer die MEGACARD 5.5
| Beschreibung:
| Schaltung:    MEGACARDV 5.5
| Autor:        D.I. Leopold Moosbrugger
| Erstellung:   3.3.2022
|
| Aenderung:
\*-------------------------------------------------------------------------*/
#ifndef FONTDATA_H_
#define FONTDATA_H_

// Struktur fuer die Zeichensatzdaten
typedef struct 
{
   uint8_t char_first;
   uint8_t char_last;
   uint8_t width;
   uint8_t height;
   uint8_t spacing;
} FontParam_t;

extern FontParam_t fontParam;
extern const uint8_t fontData[];

#endif /* FONTDATA_H_ */