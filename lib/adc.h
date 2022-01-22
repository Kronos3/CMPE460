#ifndef CMPE460_ADC_H
#define CMPE460_ADC_H

#include <fw.h>

#ifndef __adc_LINKED__
#error "You need to link 'adc' to use this header"
#endif

/**
 * Initialize the Analog-to-digital converter
 */
void adc_init(void);

/**
 * Trigger ADC conversion and wait for value
 * @return 14-bit value read out of ADC
 */
U32 adc_in(void);

#endif //CMPE460_ADC_H
