/*-------------------------------------------------------------------------*\
| Datei:        display.h
| Version:      1.0
| Projekt:      Display-Bibliothek fuer die MEGACARD V5.5
| Beschreibung: Font-Information
| Schaltung:    MEGACARD V5.5
| Autor:        D.I. Leopold Moosbrugger
| Erstellung:   3.3.2022
|
| Aenderung:
\*-------------------------------------------------------------------------*/
#ifndef DISPLAY_H_
#define DISPLAY_H_

// Konfigurieren des Displays und der Leitungen
void display_init(void);

// Informationen zur Anzeige
// Anzahl der Zeilen
uint8_t display_lines (void);

// Anzahl der Zeichen pro Zeile (zeichensatzabhaengig)
uint8_t display_chars (void);

// erstes gueltige Zeichen im Zeichensatz
uint8_t display_char_first (void);

// letztes gueltige Zeichen im Zeichensatz
uint8_t display_char_last (void);

// Die Anzeige loeschen und den Cursor an die Position (0, 0) setzen
void display_clear();

// Verschiebt den Bildschirminhalt eine Zeile nach oben
// letzte Zeile ist leer
void display_scroll_up (void);

// Verschiebt den Bildschirminhalt eine Zeile nach unten
// erste Zeile ist leer
void display_scroll_down (void);

// Den Cursor an eine beliebige Position im Display setzen
// Bei Ueberschreiten der Limite bleibt der Cursor unveraendert
// X-Position von links nach rechts, y-Position von oben nach unten
// Zaehler beginnt bei 0
void display_pos(uint8_t posx, uint8_t posy);

// Ein Zeichen an der aktuellen Cursor-Position ausgeben und
// den Cursor um eine Stelle weiterrücken. 
void display_char(uint8_t c);

// Schreibt eine Zeichenkette an die aktuelle Position
void display_string (char *line);

// Schreibt eine Zeichenkette an die Position posx, posy
void display_string_pos (uint8_t posx, uint8_t posy, char *line);

// Schreibt die formatierte Zeichenkette an die aktuelle Position
int8_t  display_printf(const char *fmt, ...);

// Schreibt die formatierte Zeichenkette an die Position posx, posy
int8_t  display_printf_pos(uint8_t posx, uint8_t posy, const char *fmt, ...);

// Schreibt ein Byte in den Grafikspeicher (Position: pcol, prow)
void display_pixel_byte (uint8_t pcol, uint8_t prow, uint8_t pbyte);

#endif /* DISPLAY_H_ */

