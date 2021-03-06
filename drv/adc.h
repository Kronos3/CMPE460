#ifndef CMPE460_ADC_H
#define CMPE460_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

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
U16 adc_in(void);

/**
 * Convert an ADC reading into a voltage
 * @param adc_i adc reading from adc_in()
 * @return voltage from 0-2.5V (ref)
 */
F64 adc_raw_to_voltage(U16 adc_i);
U16 adc_voltage_to_raw(F64 voltage);

#ifdef __cplusplus
}
#endif

#endif //CMPE460_ADC_H
