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
 *   Header file for adc library
 * -----------------------------------------
 */

#ifndef ADC_H_
#define ADC_H_

// Definition of ADC parameters

#ifndef ADC_REF             // Reference voltage
    // 0x00 -> AREF Pin
    // 0x01 -> AVCC (5V)
    // 0x02 -> -
    // 0x03 -> 2.56V 
    #define ADC_REF 0x01
#endif

#ifndef ADC_MODE            // Running mode
    // 0x00 Single transformation
    // 0x01 Permanent transformation
    #define ADC_MODE 0x00
#endif

#ifndef ADC_DATA_SHIFT      // Data register alignment
    // 0x00 -> right
    // 0x01 -> left
    #define ADC_DATA_SHIFT 0x01
#endif

#ifndef ADC_PRESCALER       // Prescaler settings
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // !!! The prescaler settings should be setup   !!!
    // !!! to a value that the ADC clock is between !!!
    // !!! 50 - 200 kHz (for fully 10 bit mode)     !!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //
    // 0x00 -> F_CPU / 2
    // 0x01 -> F_CPU / 2
    // 0x02 -> F_CPU / 4
    // 0x03 -> F_CPU / 8
    // 0x04 -> F_CPU / 16
    // 0x05 -> F_CPU / 32
    // 0x06 -> F_CPU / 64
    // 0x07 -> F_CPU / 128
    #define ADC_PRESCALER 0x06
#endif

// #ifndef ADC_NOISE_REDUCTION  // Noise reduction enabled
//  #define ADC_NOISE_REDUCTION
// #endif

#ifndef ADC_TRIGGER_SOURCE      // Trigger source setting
    // 0x00 -> Free running mode
    // 0x01 -> Analog comparator
    // 0x02 -> External Interrupt (0)
    // 0x03 -> Timer 0 compare
    // 0x04 -> Timer 0 overflow
    // 0x05 -> Timer 1 compare B
    // 0x06 -> Timer 1 overflow
    // 0x07 -> Timer 1 capture
    #define ADC_TRIGGER_SOURCE 0x00
#endif

// Definition of ADC processing (Enabled = Interrupt/Disabled = Polling)

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Interrupts are not included in this library  !!!
// !!! The interrupt vector should be placed in the !!!
// !!! main.c file                                  !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// #ifndef ADC_ADIE
//  #define ADC_ADIE
// #endif

#include <avr/io.h>

enum ADC_Mode_t
{
    ADC_SingleEnded=0,
    ADC_DifferentialGain,
    ADC_DifferentialNoGain,
    ADC_DifferentialSpecial
};
typedef enum ADC_Mode_t ADC_Mode;

enum ADC_Channel_t
{
    ADC_CH0=0,
    ADC_CH1,
    ADC_CH2,
    ADC_CH3,
    ADC_CH4,
    ADC_CH5,
    ADC_CH6,
    ADC_CH7
};
typedef enum ADC_Channel_t ADC_Channel;

#ifdef ADC_ADIE
    #include <avr/interrupt.h>
#endif

            void adc_init(void);
            void adc_mode(ADC_Mode mode);
            void adc_channel(ADC_Channel channel);
            void adc_disable(void);
            
#ifndef ADC_ADIE
    unsigned int adc_read(void);
    unsigned int adc_average(unsigned char size);
#endif

#endif /* ADC_H_ */