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
 *   Function file for lcd library
 * -----------------------------------------
 */

#include "lcd.h"

//  +---------------------------------------------------------------+
//  |                       LCD initialization                      |
//  +---------------------------------------------------------------+
void lcd_init(void)
{
    // Initialize LCD command and data lines
    LCD_DDR_CTRL |= (1<<LCD_RS) | (1<<LCD_E);
    LCD_DDR_DATA |= (1<<LCD_DB3) | (1<<LCD_DB2) | (1<<LCD_DB1) | (1<<LCD_DB0);
    
    LCD_PORT_CTRL &= ~((1<<LCD_RS) | (1<<LCD_E));
    LCD_PORT_DATA &= ~((1<<LCD_DB3) | (1<<LCD_DB2) | (1<<LCD_DB1) | (1<<LCD_DB0));
    
    // Wait until LCD has started
    _delay_ms(LCD_STARTUP_TIME);
    
    // Setup 4 bit mode & wait function time
    lcd_set(LCD_FUNCTION);
    _delay_us(LCD_FUNCTION_TIME);
    
    // Setup LCD settings
    lcd_cmd(LCD_FUNCTION | LCD_FUNCTION_2LINE);                                 // Setup number of display lines and character font
    lcd_cmd(LCD_DISPLAY | LCD_DISPLAY_ON);  // | LCD_CURSOR_ON | LCD_BLINK_ON); // Setup display status, display cursor, display cursor blink
    lcd_cmd(LCD_ENTRY | LCD_ENTRY_INCREMENT);                                   // Setup display cursor move direction

    // Clears entire display
    lcd_clear();
}

//  +---------------------------------------------------------------+
//  |                   LCD disable function                        |
//  +---------------------------------------------------------------+
void lcd_disable(void)
{
    // Disable LCD
    LCD_DDR_CTRL &= ~((1<<LCD_RS) | (1<<LCD_E));
    LCD_DDR_DATA &= ~((1<<LCD_DB3) | (1<<LCD_DB2) | (1<<LCD_DB1) | (1<<LCD_DB0));
    
    LCD_PORT_CTRL &= ~((1<<LCD_RS) | (1<<LCD_E));
    LCD_PORT_DATA &= ~((1<<LCD_DB3) | (1<<LCD_DB2) | (1<<LCD_DB1) | (1<<LCD_DB0));
}

//  +---------------------------------------------------------------+
//  |                       LCD generate clock                      |
//  +---------------------------------------------------------------+
void lcd_clock(void)
{
    LCD_PORT_CTRL |= (1<<LCD_E);        // Enable E (HIGH) @ control port
    _delay_us((1000000UL/(F_LCD * 2))); // Wait half of defined clock period
    LCD_PORT_CTRL &= ~(1<<LCD_E);       // Disable E (LOW) @ control port
    _delay_us((1000000UL/(F_LCD * 2))); // Wait half of defined clock period
}

//  +---------------------------------------------------------------+
//  |               LCD set nibble at display databus               |
//  +---------------------------------------------------------------+
//  | Parameter:    data(7:4)   ->  higher nibble data              |
//  +---------------------------------------------------------------+
void lcd_set(unsigned char data)
{
    // Reset LCD data port
    LCD_PORT_DATA &= ~((1<<LCD_DB3) | (1<<LCD_DB2) | (1<<LCD_DB1) | (1<<LCD_DB0));

    if(data & (1<<7))                   // Check if data bit 7 is set
        LCD_PORT_DATA |= (1<<LCD_DB3);  // Set data bit 3 @ lcd bus
    if(data & (1<<6))                   // Check if data bit 6 is set
        LCD_PORT_DATA |= (1<<LCD_DB2);  // Set data bit 2 @ lcd bus
    if(data & (1<<5))                   // Check if data bit 5 is set
        LCD_PORT_DATA |= (1<<LCD_DB1);  // Set data bit 1 @ lcd bus
    if(data & (1<<4))                   // Check if data bit 4 is set
        LCD_PORT_DATA |= (1<<LCD_DB0);  // Set data bit 0 @ lcd bus

    // Generate a clock period
    lcd_clock();
}

//  +---------------------------------------------------------------+
//  |                   LCD instruction transfer                    |
//  +---------------------------------------------------------------+
//  | Parameter:    instruction ->  display command                 |
//  +---------------------------------------------------------------+
void lcd_cmd(unsigned char instruction)
{
    // Select instruction register
    LCD_PORT_CTRL &= ~(1<<LCD_RS);
    
    lcd_set(instruction);       // Write high nibble to databus
    lcd_set((instruction<<4));  // Write low nibble to databus
    
    // Wait until instruction is accessed on the LCD
    _delay_us(LCD_CMD_TIME);
}

//  +---------------------------------------------------------------+
//  |                   LCD character transfer                      |
//  +---------------------------------------------------------------+
//  | Parameter:    data    ->  ASCII/defined character             |
//  +---------------------------------------------------------------+
void lcd_char(unsigned char data)
{
    // Select data register
    LCD_PORT_CTRL |= (1<<LCD_RS);
    
    lcd_set(data);      // Write high nibble to databus
    lcd_set((data<<4)); // Write low nibble to databus
    
    // Wait until data is accessed on the LCD
    _delay_us(LCD_WRITE_TIME);
}

//  +---------------------------------------------------------------+
//  |                       LCD string transfer                     |
//  +---------------------------------------------------------------+
//  | Parameter:    data (ptr)  ->  ASCII/ defined character array  |
//  +---------------------------------------------------------------+
void lcd_string(const char *data)
{
    // Wait until \0 escape char is reached
    while(*data != LCD_NULL)
    {
        lcd_char((unsigned char)*data);    // Send character
        data++;                             // Increment data
    }
}

//  +---------------------------------------------------------------+
//  |                       LCD clear screen                        |
//  +---------------------------------------------------------------+
void lcd_clear(void)
{
    lcd_cmd(LCD_CLEAR);         // Clear the LCD screen
    _delay_ms(LCD_RETURN_TIME); // Wait LCD is cleared
}

//  +---------------------------------------------------------------+
//  |                   LCD cursor to home position                 |
//  +---------------------------------------------------------------+
void lcd_home(void)
{
    lcd_cmd(LCD_HOME);          // Return cursor to the start position (0/0)
    _delay_ms(LCD_RETURN_TIME); // Wait until CURSOR is at home position
}

//  +---------------------------------------------------------------+
//  |                       LCD cursor position                     |
//  +---------------------------------------------------------------+
//  | Parameter:    x   ->  cursor to X position                    |
//  |               y   ->  cursor to Y position                    |
//  +---------------------------------------------------------------+
void lcd_cursor(unsigned char x, unsigned char y)
{
    // Switch to LCD display line
    switch(y)
    {
        case 0  :   lcd_cmd(LCD_START_ADDR + x);                    break;  // Start at display line 0 and column x
        case 1  :   lcd_cmd(LCD_START_ADDR + 0x40 + x);             break;  // Start at display line 1 and column x
        case 2  :   lcd_cmd(LCD_START_ADDR + LCD_COLUMNS * 2 + x);  break;  // Start at display line 2 and column x
        case 3  :   lcd_cmd(LCD_START_ADDR + 0x50 + x);             break;  // Start at display line 3 and column x
        default :   return; // Return if no LCD display line is set
    }
}

//  +---------------------------------------------------------------+
//  |                   LCD character to next block                 |
//  +---------------------------------------------------------------+
//  | Parameter:    shift   ->  0 = right                           |
//  |                           1 = left                            |
//  +---------------------------------------------------------------+
void lcd_shift(LCD_Shift shift)
{
    if(shift == LCD_Right)
        lcd_cmd(LCD_SHIFT | LCD_SHIFT_DISPLAY | LCD_SHIFT_RIGHT);
    else
        lcd_cmd(LCD_SHIFT | LCD_SHIFT_DISPLAY);
    
    _delay_us(LCD_SHIFT_TIME);
}

//  +---------------------------------------------------------------+
//  |                   LCD save patterns to RAM                    |
//  +---------------------------------------------------------------+
//  | Parameter: address    ->  pattern ram address                 |
//  |                           predefined: LCD_CGADDR_CHAR0 - (n)  |
//  |            data (ptr) ->  pattern array (5 bit x 7 rows)      |
//  +---------------------------------------------------------------+
void lcd_pattern(LCD_Char address, const unsigned char *data)
{
    // Check if address is greater than max. allowed address
    if(address > LCD_CGADDDR_MAX)
        return;

    lcd_cmd(LCD_CGADDR + (address * 8));    // Setup LCD display CGADDR start address + which character
    
    // Write data to LCD display CGRAM
    for(unsigned char i=0; i < 8; i++)
        lcd_char(data[i]);
    
    lcd_cmd(LCD_DDADDR);                    // Setup LCD display DDADDR (HOME position)
}

//  +---------------------------------------------------------------+
//  |                   LCD unsigned long to ASCII                  |
//  +---------------------------------------------------------------+
//  | Parameter:    data    ->  number to convert                   |
//  |               radix   ->  2/10/16                             |
//  |               length  ->  ASCII characters 1 - 10             |
//  +---------------------------------------------------------------+
void lcd_ul2ascii(const unsigned long data, LCD_Base base, unsigned char length)
{
    // If base is wrong or length is to short/wide return
    if((base != 2 && base != 10 && base != 16) || (length < 1 || length > 10))
        return;
    
    unsigned char count = 0;    // Internal counter for character length
    char buffer[(sizeof(unsigned long) * 8 + 1)];   // ASCII buffer 9 digits + \0 escape character
    
    // Convert unsigned long to ASCII
    ultoa(data, buffer, base);

    // Loop until \0 escape char is reached
    for(unsigned char i=0; i < sizeof(buffer); i++)
    {
        // Check if buffer data is not equal to \0 escape char
        if(buffer[i] != LCD_NULL)
            count++;    // increment data counter
        else
            break;      // exit loop
    }
        
    // Check if length is greater than count
    if(count < length)
    {
        // Write spaces to LCD display until position adjustment is done
        for(unsigned char i=0; i < (length - count); i++)
        {
            if(base != 10)
                lcd_char('0');
            else
                lcd_char(LCD_SPACE);
        }            
    }
    // Check if length is lower than count
    else if(count > length)
    {
        // Write \0 escape char to the max. length allowed through position adjustment
        buffer[length] = LCD_NULL;
    }
    
    // Write string to LCD
    lcd_string(buffer);
}

//  +---------------------------------------------------------------+
//  |                   LCD signed long to ASCII                    |
//  +---------------------------------------------------------------+
//  | Parameter:    data    ->  number to convert                   |
//  |               radix   ->  2/10/16                             |
//  |               length  ->  ASCII characters 1 - 10             |
//  +---------------------------------------------------------------+
void lcd_sl2ascii(const signed long data, LCD_Base base, unsigned char length)
{
    // If base is wrong or length is to short/wide return
    if((base != 2 && base != 10 && base != 16) || (length < 1 || length > 10))
        return;
    
    unsigned char count = 0;    // Internal counter for character length
    char buffer[(sizeof(long) * 8 + 2)];   // ASCII buffer 10 digits + \0 escape character
    
    // Convert data to ASCII
    ltoa(data, buffer, base);
    
    // Loop until \0 escape char is reached
    for(unsigned char i=0; i < sizeof(buffer); i++)
    {
        // Check if buffer data is not equal to \0 escape char
        if(buffer[i] != LCD_NULL)
            count++;    // increment data counter
        else
            break;      // exit loop
    }
        
    // Check if length is greater than count
    if(count < length)
    {
        // Write spaces to LCD display until position adjustment is done
        for(unsigned char i=0; i < (length - count); i++)
        {
            if(base != 10)
                lcd_char('0');
            else
                lcd_char(LCD_SPACE);
        }            
    }
    // Check if length is lower than count
    else if(count > length)
    {
        // Write \0 escape char to the max. length allowed through position adjustment
        buffer[length] = LCD_NULL;
    }
    
    if(base == 10)
    {
        // Write string to LCD
        lcd_string(buffer);
    }
    else
    {
        if(data < 0)
            lcd_string(&buffer[count - length]);
        else
            lcd_string(buffer);
    }
}

//  +---------------------------------------------------------------+
//  |                       LCD double to ASCII                     |
//  +---------------------------------------------------------------+
//  | Parameter:    data    ->  number to convert                   |
//  |               radix   ->  2/10/16                             |
//  |               length  ->  ASCII characters 1 - 10             |
//  +---------------------------------------------------------------+
void lcd_d2ascii(const double data, signed char length, unsigned char precision)
{
    char buffer[60];
    
    // Convert double to ASCII
    if((length > 0) && (length < 60))
        dtostrf(data, length, precision, buffer);
    else
        dtostre(data, buffer, precision, DTOSTR_ALWAYS_SIGN | DTOSTR_UPPERCASE);
    
    // Write string to LCD
    lcd_string(buffer);
}