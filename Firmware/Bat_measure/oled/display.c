/*-------------------------------------------------------------------------*\
| Datei:        display.c
| Version:      1.0
| Projekt:      Display-Bibliothek fuer die MEGACARD V5.5
| Beschreibung: Bibliotheksfunktionen (Implementierung)
| Schaltung:    MEGACARD V5.5
| Autor:        D.I. Leopold Moosbrugger
| Erstellung:   3.3.2022
|
| Aenderung:    
\*-------------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdarg.h>
#include "twi_soft.h"  // Sofware TWI-Schnittstelle
#include "FontData.h"  // Font-Informationen
#include "display.h" // Schnittstellendefinitionen

// Lokale Makros
#define OLED_CTRL_BYTE_CMD  0x80 // 0b1000 0000
#define OLED_CTRL_BYTE_DATA 0x40 // 0b0100 0000
#define OLED_DEV_ADDR 0x78       // 0b0111 1000

#define OLED_PAGES 8
#define OLED_PIXEL_X 128
#define OLED_PIXEL_Y 64

#define DISP_LINES (OLED_PIXEL_Y/fontParam.height)
#define DISP_COLS (OLED_PIXEL_X/(fontParam.width+fontParam.spacing))

#define  OLED_SEND_CMD(value)  {twi_s_write(OLED_CTRL_BYTE_CMD); twi_s_write((value));} 

// Modullokale Variablen
static uint8_t d_offset = 0;

// Display initialisieren und einschalten
void display_init(void)
{
   // Initialisierung der SW-I2C Schnittstelle
   twi_s_init();
   
   // Initialisierungssequenzen senden
   // Siehe Application Note [SOLOMON SYSTECH]
   // http://www.solomon-systech.com

   twi_s_start(OLED_DEV_ADDR);
   OLED_SEND_CMD(0xA8); // Set Mux Ratio
   OLED_SEND_CMD(0x3F); // S32 (R)

   OLED_SEND_CMD(0xD3); // Set Display offset 
   OLED_SEND_CMD(0x00); // S33 (R)

   OLED_SEND_CMD(0x40); // Set Segment Start Line S32 (R)

   OLED_SEND_CMD(0xA0); // Set Segment re-map S32 (R)

   OLED_SEND_CMD(0xC0); // Set COM Output Scan Direction S32 (R)

   OLED_SEND_CMD(0xDA); // Set COM Pins hardware configuration
   OLED_SEND_CMD(0x12); // S33 (R)

   OLED_SEND_CMD(0x81); // Set Contrast Control 
   OLED_SEND_CMD(0x7F); // S27 (R)

   OLED_SEND_CMD(0xA4); // Disable Entre Display On S27 (R)

   OLED_SEND_CMD(0xA6); // Set Normal Display S27 (R)

   OLED_SEND_CMD(0xD5); // Set Osc Frequency
   OLED_SEND_CMD(0x80); // S33 (R)

   OLED_SEND_CMD(0x20); // Page addressing Mode
   OLED_SEND_CMD(0x02); // S31 (R)

   OLED_SEND_CMD(0x8D); // Enable charge pump regulator 
   OLED_SEND_CMD(0x14); // during display on 7.5V S34 (R)
   twi_s_stop();

   display_clear();

   twi_s_start(OLED_DEV_ADDR);
   OLED_SEND_CMD(0xAF); // Display ON S27
   twi_s_stop();

   // Modullokale Variablen initialisieren
   d_offset = 0;
}

// Informationsfunktionen zur Anzeige 
uint8_t display_lines (void)
{
   return DISP_LINES;
}

uint8_t display_chars (void)
{
   return DISP_COLS;
}

uint8_t display_char_first (void)
{
   return fontParam.char_first;
}

uint8_t display_char_last (void)
{
   return fontParam.char_last;
}

// Anzeigefunktionen (Implementierung)
void display_scroll_up (void)
{
   uint8_t ic;

   // oberste Zeile loeschen
   display_pos(0, 0);

   twi_s_start(OLED_DEV_ADDR);
   twi_s_write(OLED_CTRL_BYTE_DATA);
   // gesamte Page loeschen
   for(ic=0; ic<OLED_PIXEL_X; ic++)
   {
      twi_s_write(0x00);
   }
   twi_s_stop();

   d_offset += 1;
   if (d_offset >= DISP_LINES)
      d_offset -= DISP_LINES;

   twi_s_start(OLED_DEV_ADDR);
   OLED_SEND_CMD(0xD3); // Set Display offset
   OLED_SEND_CMD(d_offset<<3); // S33 
   twi_s_stop();
}

void display_scroll_down (void)
{
   uint8_t ic;

   // unterste Zeile loeschen
   display_pos(0, DISP_LINES-1);

   twi_s_start(OLED_DEV_ADDR);
   twi_s_write(OLED_CTRL_BYTE_DATA);
   // gesamte Page loeschen
   for(ic=0; ic<OLED_PIXEL_X; ic++)
   {
      twi_s_write(0x00);
   }
   twi_s_stop();

   if (d_offset > 0)
   {
      d_offset--;
   } else
   {
      d_offset = DISP_LINES - 1;
   }

   twi_s_start(OLED_DEV_ADDR);
   OLED_SEND_CMD(0xD3); // Set Display offset
   OLED_SEND_CMD(d_offset<<3); // S33
   twi_s_stop();
}
// Setzt den Cursor an eine beliebige Position im Display
// Bei Ueberschreiten der Limite (DISP_LINES, DISP_COLS) bleibt 
// der Cursor unveraendert. (Zaehler beginnt bei 0)
// x-Position von links nach rechts, y-Position von oben nach unten
void display_pos(uint8_t col, uint8_t row)
{
   uint8_t pixel_col;

   // ungueltige Parameter abfangen
   if ((row < DISP_LINES) && (col < DISP_COLS)) 
   {
	   pixel_col = col * (fontParam.width+fontParam.spacing);

      // Zeilenadresse anpassen
      row += d_offset;
      if (row >= DISP_LINES) 
         row -= DISP_LINES;

	   twi_s_start(OLED_DEV_ADDR);
	   // Seitenadresse setzen
	   OLED_SEND_CMD((0b10110000 | row));

	   // unteres Nibble der Col-Addresse ((col*8) & 0x0F)
	   OLED_SEND_CMD(pixel_col & 0x0F);

	   // oberes Nibble der Col-Addresse ((col*8/16) & 0x0F)
	   OLED_SEND_CMD(0x10 | ((pixel_col>>4) & 0x0F));

	   twi_s_stop();
   }
}

// Loescht den Inhalt des Displays und setzt den Cursor oben links (Home)
void display_clear(void)
{
   uint8_t ip, ic;
   
   // Alle Speicher Pages loeschen
   for(ip=0; ip<OLED_PAGES; ip++)
   {
      display_pos(0, ip);

	   twi_s_start(OLED_DEV_ADDR);
      twi_s_write(OLED_CTRL_BYTE_DATA);
      // gesamte Page loeschen	
      for(ic=0; ic<OLED_PIXEL_X; ic++)
      {
         twi_s_write(0x00);
      }
      twi_s_stop();
   }

   // Cursor in Position 0, 0
   display_pos(0, 0);
}

// Ausgabe eines Zeichens an der aktuellen Position
void display_char (uint8_t c)
{
   uint8_t cnt;
   uint16_t f_index;
   
   // Ungueltige Zeichen abfangen (erstes Zeichen imZeichensatz)
   if ((c < fontParam.char_first) || (c > fontParam.char_last)) 
      c = fontParam.char_first;
   
   // Index im FontData-Arry berechnen
   f_index = (c - fontParam.char_first) * fontParam.width;

   // write zero to next GDRAM Position
   twi_s_start(OLED_DEV_ADDR);
   twi_s_write(OLED_CTRL_BYTE_DATA);

   // Display character c at current position
   for (cnt=0; cnt<fontParam.width; cnt++)
   {
      twi_s_write(pgm_read_byte(&fontData[f_index++]));
   }

   // Spacing ausgeben
   for (cnt=0; cnt<fontParam.spacing; cnt++)
   {
      twi_s_write(0);
   }

   twi_s_stop();
}

void display_string (char *line)
{
   while (*line)
      display_char(*line++);
}

void display_string_pos (uint8_t posx, uint8_t posy, char *line)
{
   display_pos(posx, posy); 
   display_string(line);
}

int8_t  display_printf(const char *fmt, ...)
{
   char buf[25];
   int8_t c_count;

   va_list args;
   va_start(args, fmt);
   c_count = vsnprintf(buf, sizeof buf, fmt, args);
   va_end(args);
   
   if (c_count >= 0)
      display_string(buf);
   return c_count;
}

int8_t display_printf_pos(uint8_t posx, uint8_t posy, const char *fmt, ...)
{
   char buf[25];
   int8_t c_count;
   
   va_list args;
   va_start(args, fmt);
   c_count = vsnprintf(buf, sizeof buf, fmt, args);
   va_end(args);
   
   if (c_count >= 0)
      display_string_pos(posx, posy, buf);
   return c_count;
}

void display_pixel_byte (uint8_t pcol, uint8_t prow, uint8_t pbyte)
{
   if ((prow < OLED_PIXEL_Y) && (pcol < OLED_PIXEL_X)) 
   {
	  prow >>= 3; // prow / 8

	  twi_s_start(OLED_DEV_ADDR);
	  // Seitenadresse setzen
	  OLED_SEND_CMD((0b10110000 | prow));
      // unteres Nibble der Col-Addresse ((col*8) & 0x0F)
	  OLED_SEND_CMD(pcol & 0x0F);
	  // oberes Nibble der Col-Addresse ((col*8/16) & 0x0F)
	  OLED_SEND_CMD(0x10 | ((pcol>>4) & 0x0F));
	  twi_s_stop();
	  
     twi_s_start(OLED_DEV_ADDR);
     twi_s_write(OLED_CTRL_BYTE_DATA);
	  twi_s_write(pbyte); 
 	  twi_s_stop();
  }	
}




