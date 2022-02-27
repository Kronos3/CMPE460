#include <dc.h>

static bool_t dc_inited = FALSE;

static struct {
    bool_t configured;
    pwm_t channel;
    pwm_pin_t forward;
    pwm_pin_t backward;
    F64 base_frequency;
} dc_profiles[DC_N] = {0};

void dc_cfg(dc_t dc,
            pwm_t pwm_channel,
            pwm_pin_t pwm_pin_forward,
            pwm_pin_t pwm_pin_backward,
            F64 base_frequency)
{
    FW_ASSERT(!dc_inited && "dc_cfg() cannot run after dc_init()");
    FW_ASSERT(dc >= 0 && dc < DC_N, dc);

    dc_profiles[dc].channel = pwm_channel;
    dc_profiles[dc].forward = pwm_pin_forward;
    dc_profiles[dc].backward = pwm_pin_backward;
    dc_profiles[dc].base_frequency = base_frequency;
    dc_profiles[dc].configured = TRUE;
}

void dc_init()
{
    FW_ASSERT(!dc_inited && "You cant run dc_init() multiple time");

    for (dc_t dc = DC_0; dc < DC_N; dc++)
    {
        if (dc_profiles[dc].configured)
        {
            // Find the best set of prescalers to use on this PWM channel
            // given the base frequency of the PWM. This will attempt to
            // maximize the timer counter
            pwm_prescaler_t p1, p2;
            pwm_optimal_prescaler(dc_profiles[dc].base_frequency, &p1, &p2);

            pwm_init(dc_profiles[dc].channel, p1, p2,
                     (F32)dc_profiles[dc].base_frequency);
        }
    }

    dc_inited = TRUE;
}

void dc_set(dc_t dc, F64 speed)
{
    FW_ASSERT(dc_inited && "You need to run dc_init() before dc_set()");
    FW_ASSERT(dc >= 0 && dc < DC_N, dc);
    FW_ASSERT(dc_profiles[dc].configured && "DC motor has not been configured", dc);

    FW_ASSERT(speed >= -1.0 && speed <= 1.0 && "Speed must be between -1 and 1", speed * 100);

    if (speed > 0.0)
    {
        // Run the motor forward
        // Stop the reverse pwm
        pwm_set_pin(dc_profiles[dc].channel, dc_profiles[dc].forward, speed);
        pwm_set_pin(dc_profiles[dc].channel, dc_profiles[dc].backward, 0.0);
    }
    else
    {
        // Run the motor backwards
        // Stop forward pwm
        pwm_set_pin(dc_profiles[dc].channel, dc_profiles[dc].forward, 0.0);
        pwm_set_pin(dc_profiles[dc].channel, dc_profiles[dc].backward, -speed);
    }
}