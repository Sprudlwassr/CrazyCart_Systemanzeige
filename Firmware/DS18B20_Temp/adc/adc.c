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
 *   Function file for adc library
 * -----------------------------------------
 */

#include "adc.h"

//  +---------------------------------------------------------------+
//  |                   ADC initialization                          |
//  +---------------------------------------------------------------+
void adc_init(void)
{
    // Setup ADC parameters
    ADMUX = (((0x03 & ADC_REF)<<6) | ((0x01 & ADC_DATA_SHIFT)<<5));
    
    // Setup ADC prescaler
    // F_ADC = F_CPU / ADC_PRESCALER
    ADCSRA = ((0x07 & ADC_PRESCALER));
    
    // Setup ADC permanent transformation and trigger source
    #if ADC_MODE == 0x01
        ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<<ADATE);
        
        SFIOR &= ~(0xE0);
        SFIOR |= ((0x07 & ADC_TRIGGER_SOURCE)<<5);
    #endif
    
    // Check if ADC interrupt handler is active
    #ifdef ADC_ADIE
        ADCSRA |= (1<<ADIE);    // Enable ADC interrupt
        sei();                  // Enable global interrupt
    #endif
}

//  +---------------------------------------------------------------+
//  |                   ADC disable function                        |
//  +---------------------------------------------------------------+
void adc_disable(void)
{
    ADCSRA &= ~(1<<ADEN);   // Disable ADC
}

//  +---------------------------------------------------------------+
//  |                   ADC mode selection                          |
//  +---------------------------------------------------------------+
//  | Parameter:    mode    ->  0x00 = Singled ended input          |
//  |                                  ADC0 - ADC7                  |
//  |                           0x01 = Differential input   | Gain  |
//  |                                  ADC0 - ADC0 (CH0)    |  10x  |
//  |                                  ADC1 - ADC0 (CH1)    |  10x  |
//  |                                  ADC0 - ADC0 (CH0)    | 200x  |
//  |                                  ADC1 - ADC0 (CH1)    | 200x  |
//  |                                  ADC2 - ADC2 (CH2)    |  10x  |
//  |                                  ADC3 - ADC2 (CH3)    |  10x  |
//  |                                  ADC2 - ADC2 (CH2)    | 200x  |
//  |                                  ADC3 - ADC2 (CH3)    | 200x  |
//  |                           0x02 = Differential input   | Gain  |
//  |                                  ADC0 - ADC1 (CH0)    |   1x  |
//  |                                  ADC1 - ADC1 (CH1)    |   1x  |
//  |                                  ADC2 - ADC1 (CH2)    |   1x  |
//  |                                  ADC3 - ADC1 (CH3)    |   1x  |
//  |                                  ADC4 - ADC1 (CH4)    |   1x  |
//  |                                  ADC5 - ADC1 (CH5)    |   1x  |
//  |                                  ADC6 - ADC1 (CH6)    |   1x  |
//  |                                  ADC7 - ADC1 (CH7)    |   1x  |
//  |                           0x03 = Differential input   | Gain  |
//  |                                  ADC0 - ADC2 (CH0)    |   1x  |
//  |                                  ADC1 - ADC2 (CH1)    |   1x  |
//  |                                  ADC2 - ADC2 (CH2)    |   1x  |
//  |                                  ADC3 - ADC2 (CH3)    |   1x  |
//  |                                  ADC4 - ADC2 (CH4)    |   1x  |
//  |                                  ADC5 - ADC2 (CH5)    |   1x  |
//  |                                          --- Special ---      |
//  |                                  1.22V Ref   (CH6)            |
//  |                                  0V Ref      (CH7)            |
//  +---------------------------------------------------------------+
void adc_mode(ADC_Mode mode)
{
    ADMUX = (ADMUX & 0xE7) | ((0x03 & mode)<<3);    // Setup ADC Mode
}

//  +---------------------------------------------------------------+
//  |                   ADC channel selection                       |
//  +---------------------------------------------------------------+
//  | Parameter:    channel ->  0x00 = CH0 (Standard after init)    |
//  |                           0x01 = CH1                          |
//  |                           0x02 = CH2                          |
//  |                           0x03 = CH3                          |
//  |                           0x04 = CH4                          |
//  |                           0x05 = CH5                          |
//  |                           0x06 = CH6                          |
//  |                           0x07 = CH7                          |
//  +---------------------------------------------------------------+
void adc_channel(ADC_Channel channel)
{
    ADMUX = (ADMUX & ~(0x07)) | (0x07 & channel);   // Select ADC Channel
}

#ifndef ADC_ADIE

    //  +---------------------------------------------------------------+
    //  |                   ADC read value                              |
    //  +---------------------------------------------------------------+
    //  | Return:   ->  RIGHT shift of ADC data register                |
    //  +-------------------------------+-------------------------------+
    //  |             ADCH              |             ADCL              |
    //  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //  | - | - | - | - | - | - | ? | ? | ? | ? | ? | ? | ? | ? | ? | ? |
    //  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //  |                                                               |
    //  |           ->  LEFT shift of ADC data register                 |
    //  +-------------------------------+-------------------------------+
    //  |             ADCH              |             ADCL              |
    //  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //  | ? | ? | ? | ? | ? | ? | ? | ? | ? | ? | - | - | - | - | - | - |
    //  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    unsigned int adc_read(void)
    {
        #ifdef ADC_NOISE_REDUCTION
            // Enter ADC noise reduction mode (stop the CPU)
            MCUCR &= ~((1<<SM2) | (1<<SM1));
            MCUCR |= (1<<SM0);
        #endif
        
        #if ADC_MODE == 0x01
            
            // Wait until ADC interrupt flag is zero
            while(!(ADCSRA & (1<<ADIF)))
                asm volatile("NOP");
            
            unsigned char ADC_LOW = ADCL;   // Save ADC LOW value to variable
            unsigned char ADC_HIGH = ADCH;  // Save ADC HIGH value to variable
            
            ADCSRA |= (1<<ADIF);            // Reset ADC interrupt flag
            
            // Return ADC high + low value
            return ((ADC_HIGH<<8) | ADC_LOW);
        
        #else
            
            // Start single conversion
            ADCSRA |= (1<<ADEN) | (1<<ADSC);
            
            // Wait until conversion is in progress
            while(ADCSRA & (1<<ADSC))
                asm volatile("NOP");
            
            unsigned char ADC_LOW = ADCL;   // Save ADC LOW value to variable
            unsigned char ADC_HIGH = ADCH;  // Save ADC HIGH value to variable
            
            // Return ADC high + low value
            return ((ADC_HIGH<<8) | ADC_LOW);
            
        #endif
        
        #ifdef ADC_NOISE_REDUCTION
            // Exit ADC noise reduction mode (start the CPU)
            MCUCR &= ~((1<<SM2) | (1<<SM1)) | (1<<SM0));
        #endif
        
    }

    //  +---------------------------------------------------------------+
    //  |                   ADC read average from values                |
    //  +---------------------------------------------------------------+
    //  | Parameter:    size    ->  quantity of conversions             |
    //  |                                                               |
    //  |    Return:    0x????  ->  Arithmetic average                  |
    //  |                           of converted data                   |
    //  +---------------------------------------------------------------+
    unsigned int adc_average(unsigned char size)
    {
        unsigned long average = 0; // average buffer
        
        // Read and add ADC values
        for(unsigned char i=0; i < size; i++)
        {
            #if ADC_DATA_SHIFT == 1
                average += (adc_read()>>6);
            #else
                average += adc_read();
            #endif
        }
        
        average /= size;    // Build average
        
        // Return average as unsigned int
        return (unsigned int)(average);
    }

#endif