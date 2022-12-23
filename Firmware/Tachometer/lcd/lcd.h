/* -----------------------------------------
 * G.Raf^engineering
 * www.sunriax.at
 * -----------------------------------------
 *    Platform: Megacard/STK500/STK600
 *    Hardware: ATmega??-????
 * -----------------------------------------
 *     Version: 1.0 Release
 *      Author: G.Raf
 * Description:
 *   Header file for lcd library
 * -----------------------------------------
 */

#ifndef LCD_H_
#define LCD_H_

// Definition of SYSTEM parameters

#ifndef F_CPU           // System clock
    #define F_CPU 12000000UL
#endif

// Definition of LCD parameters

#ifndef F_LCD           // LCD enable clock
    #define F_LCD 100000UL
#endif

// LCD control directives
#ifndef LCD_DDR_CTRL    // LCD control direction register
    #define LCD_DDR_CTRL DDRA
#endif

#ifndef LCD_PORT_CTRL   // LCD control port register
    #define LCD_PORT_CTRL PORTA
#endif

#ifndef LCD_RS          // LCD register select
    #define LCD_RS PA6
#endif

#ifndef LCD_E           // LCD start data read/write
    #define LCD_E PA4
#endif

// LCD data directives
#ifndef LCD_DDR_DATA    // LCD data direction register
    #define LCD_DDR_DATA DDRB
#endif

#ifndef LCD_PORT_DATA   // LCD control direction register
    #define LCD_PORT_DATA PORTB
#endif

#ifndef LCD_DB0         // LCD data bit 0 (LCD display bit 4)
    #define LCD_DB0 PB2
#endif

#ifndef LCD_DB1         // LCD data bit 1 (LCD display bit 5)
    #define LCD_DB1 PB5
#endif

#ifndef LCD_DB2         // LCD data bit 2 (LCD display bit 6)
    #define LCD_DB2 PB6
#endif

#ifndef LCD_DB3         // LCD data bit 3 (LCD display bit 7)
    #define LCD_DB3 PB7
#endif

// LCD display parameters
#ifndef LCD_COLUMNS     // LCD display columns/row (1 - 20)
    #define LCD_COLUMNS 8
#endif

#ifndef LCD_COLUMNS_END     // LCD character memory length
    #define LCD_COLUMNS_END 80
#endif

#ifndef LCD_START_ADDR      // Start position of the LCD RAM
    #define LCD_START_ADDR 0x80
#endif

// LCD time/instruction codes (!!! Do not change !!!)

// LCD time directives
#ifndef LCD_STARTUP_TIME    // LCD startup time in ms
    #define LCD_STARTUP_TIME 10
#endif

#ifndef LCD_RETURN_TIME     // LCD return home time in ms
    #define LCD_RETURN_TIME 2
#endif

#ifndef LCD_ENTRY_TIME      // LCD entry mode time in us
    #define LCD_ENTRY_TIME 37
#endif

// #ifndef LCD_DISPLAY_TIME // LCD display time in us
//  #define LCD_DISPLAY_TIME 37
// #endif

#ifndef LCD_SHIFT_TIME      // LCD shift time in us
    #define LCD_SHIFT_TIME 37
#endif

#ifndef LCD_FUNCTION_TIME   // LCD function time in us
    #define LCD_FUNCTION_TIME 37
#endif

#ifndef LCD_WRITE_TIME      // LCD WRITE time in us
    #define LCD_WRITE_TIME 37
#endif

#ifndef LCD_CMD_TIME        // LCD COMMAND time in us
    #define LCD_CMD_TIME 37
#endif

// LCD instruction directives
#ifndef LCD_CLEAR           // LCD clear display instruction
    #define LCD_CLEAR           0b00000001
#endif

#ifndef LCD_HOME            // LCD return home instruction
    #define LCD_HOME            0b00000010
#endif

#ifndef LCD_ENTRY           // LCD entry mode set instruction
    #define LCD_ENTRY           0b00000100
    #define LCD_ENTRY_INCREMENT 0b00000010
    #define LCD_ENTRY_SHIFT     0b00000001
#endif

#ifndef LCD_DISPLAY         // LCD display set instruction
    #define LCD_DISPLAY         0b00001000
    #define LCD_DISPLAY_ON      0b00000100
    #define LCD_CURSOR_ON       0b00000010
    #define LCD_BLINK_ON        0b00000001
#endif

#ifndef LCD_SHIFT           // LCD shift instruction
    #define LCD_SHIFT           0b00010000
    #define LCD_SHIFT_DISPLAY   0b00001000
    #define LCD_SHIFT_RIGHT     0b00000100
#endif

#ifndef LCD_FUNCTION        // LCD function instruction
    #define LCD_FUNCTION        0b00100000
    #define LCD_FUNCTION_8BIT   0b00010000
    #define LCD_FUNCTION_2LINE  0b00001000
    #define LCD_FUNCTION_5x10   0b00000100
#endif

#ifndef LCD_CGADDR          // LCD CGADDR instruction
    #define LCD_CGADDR          0b01000000
    #define LCD_CGADDR_CHAR0    0b00000000
    #define LCD_CGADDR_CHAR1    0b00000001
    #define LCD_CGADDR_CHAR2    0b00000010
    #define LCD_CGADDR_CHAR3    0b00000011
    #define LCD_CGADDR_CHAR4    0b00000100
    #define LCD_CGADDR_CHAR5    0b00000101
    #define LCD_CGADDR_CHAR6    0b00000110
    #define LCD_CGADDR_CHAR7    0b00000111
    #define LCD_CGADDDR_MAX     7
#endif

#ifndef LCD_DDADDR          // LCD DDADDR instruction
    #define LCD_DDADDR          0b10000000
#endif

#ifndef LCD_SPACE           // Space character
    #define LCD_SPACE 0x20
#endif

#ifndef LCD_NULL            // End character
    #define LCD_NULL 0x00
#endif

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

enum LCD_Shift_t
{
    LCD_Right=0,
    LCD_Left
};
typedef enum LCD_Shift_t LCD_Shift;

enum LCD_Base_t
{
    LCD_Binary=2,
    LCD_Decimal=10,
    LCD_Hexadecimal=16    
};
typedef enum LCD_Base_t LCD_Base;

enum LCD_Char_t
{
    LCD_Char0=LCD_CGADDR_CHAR0,
    LCD_Char1=LCD_CGADDR_CHAR1,
    LCD_Char2=LCD_CGADDR_CHAR2,
    LCD_Char3=LCD_CGADDR_CHAR3,
    LCD_Char4=LCD_CGADDR_CHAR4,
    LCD_Char5=LCD_CGADDR_CHAR5,
    LCD_Char6=LCD_CGADDR_CHAR6,
    LCD_Char7=LCD_CGADDR_CHAR7
};
typedef enum LCD_Char_t LCD_Char;

void lcd_init(void);
void lcd_disable(void);
void lcd_clock(void);
void lcd_set(unsigned char data);
void lcd_cmd(unsigned char instruction);
void lcd_char(unsigned char data);
void lcd_string(const char *data);
void lcd_clear(void);
void lcd_home(void);
void lcd_cursor(unsigned char x, unsigned char y);
void lcd_shift(LCD_Shift shift);
void lcd_pattern(LCD_Char address, const unsigned char *data);
void lcd_ul2ascii(unsigned long data, LCD_Base base, unsigned char length);
void lcd_sl2ascii(const signed long data, LCD_Base base, unsigned char length);
void lcd_d2ascii(const double data, signed char length, unsigned char precision);

#endif /* LCD_H_ */