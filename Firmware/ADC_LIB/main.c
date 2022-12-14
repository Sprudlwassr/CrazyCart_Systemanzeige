/* -----------------------------------------
 * G.Raf^engineering
 * www.sunriax.at
 * -----------------------------------------
 *    Platform: Megacard
 *    Hardware: ATmega16
 * -----------------------------------------
 *     Version: 1.0 Release
 *      Author: G.Raf
 * Description:
 *   Sample for reading A/D data with the
 *   adc library
 * -----------------------------------------
 */

#define F_CPU 12000000UL    // System clock (required for delay)

// System libraries
#include <avr/io.h>         // Required for I/O
#include <util/delay.h>     // Required for _delay_ms()

// User libraries
#include "adc.h"

int main(void)
{
    // Port setup
    DDRC = 0xFF;    // Set PORT as output
    
    // Initialize ADC with defined parameters in adc.h
    adc_init();
    
    // ADC Mode
    // Standard: Single Ended, no configuration required
    adc_mode(ADC_SingleEnded);
    
    // Select ADC channel 5 (unipolar mode)
    adc_channel(ADC_CH5);
    
    while (1) 
    {
        // Write converted value to PORT
        // PORT only has 8 Bit, left adjustment is used
        // so the lower byte can be thrown away
        // with right adjustment: PORTC = (unsigned char)(adc_read()>>2); 
        PORTC = (unsigned char)(adc_read()>>8);
        
        // With the average function it is possible to
        // make a collection of ADC samples and return
        // the arithmetic median
        PORTC = (unsigned char)(adc_average(10)>>8);
    }
}