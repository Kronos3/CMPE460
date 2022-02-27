#ifndef CMPE460_DC_H
#define CMPE460_DC_H

#include <fw.h>
#include <pwm.h>

#ifndef __dc_LINKED__
#error "You need to link 'dc' to use this header"
#endif

typedef enum
{
    DC_0,       //!< DC motor channel 1
    DC_1,       //!< DC motor channel 2
    DC_N
} dc_t;

/**
 * Configure a DC motor channel to operate with PWM
 * @param dc DC channel to configure
 * @param pwm_channel pwm channel to use
 * @param pwm_pin_forward forward pin to use on this pwm channel
 * @param pwm_pin_backward backward pin to use on this pwm channel
 * @param base_frequency base pwm frequency
 */
void dc_cfg(dc_t dc,
            pwm_t pwm_channel,
            pwm_pin_t pwm_pin_forward,
            pwm_pin_t pwm_pin_backward,
            F64 base_frequency);

/**
 * Run DC initialization after all dc_cfg() calls have been run
 */
void dc_init(void);

/**
 * Run a motor at a certain speed
 * @param dc motor to run
 * @param speed speed from -1 to 1 (negative will run backwards)
 */
void dc_set(dc_t dc, F64 speed);

#endif //CMPE460_DC_H
