#ifndef CMPE460_DC_H
#define CMPE460_DC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fw.h>
#include <drv/pwm.h>

#ifndef __dc_LINKED__
#error "You need to link 'dc' to use this header"
#endif

typedef enum
{
    DC_0,       //!< DC motor channel 1
    DC_1,       //!< DC motor channel 2
    DC_N
} dc_t;

typedef struct
{
    PwmPin forward;
    PwmPin backward;
    F64 base_frequency;
} DcParam;

/**
 * Configure a DC motor channel to operate with PWM
 * @param dc DC channel to configure
 * @param param.forward PWM pin to use for forward drive
 * @param param.backward PWM pin to use for backward drive
 * @param param.base_frequency base pwm frequency
 */
void dc_cfg(dc_t dc, const DcParam* param);

/**
 * Run DC initialization after all dc_cfg() calls have been run
 */
void dc_init(void);

/**
 * Start pwm running the configured dc motors
 */
void dc_start(void);

/**
 * Run a motor at a certain speed
 * @param dc motor to run
 * @param speed speed from -1 to 1 (negative will run backwards)
 */
void dc_set(dc_t dc, F64 speed);

#ifdef __cplusplus
}
#endif

#endif //CMPE460_DC_H
