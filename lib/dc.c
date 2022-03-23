#include <lib/dc.h>

static bool_t dc_inited = FALSE;

static DcParam dc_profiles[DC_N] = {0};
static bool_t configured[DC_N] = {FALSE};

void dc_cfg(dc_t dc, const DcParam* param)
{
    FW_ASSERT(!dc_inited && "dc_cfg() cannot run after dc_init()");
    FW_ASSERT(dc >= 0 && dc < DC_N, dc);

    dc_profiles[dc] = *param;
    configured[dc] = TRUE;
}

void dc_init(void)
{
    FW_ASSERT(!dc_inited && "You cant run dc_init() multiple time");

    for (dc_t dc = DC_0; dc < DC_N; dc++)
    {
        if (configured[dc])
        {
            pwm_init(dc_profiles[dc].forward, dc_profiles[dc].base_frequency);
            pwm_init(dc_profiles[dc].backward, dc_profiles[dc].base_frequency);
        }
    }

    dc_inited = TRUE;
}

void dc_start(void)
{
    FW_ASSERT(dc_inited && "You need to run dc_init() before dc_set()");

    for (dc_t dc = DC_0; dc < DC_N; dc++)
    {
        if (configured[dc])
        {
            dc_set(dc, 0.0);
            pwm_start(dc_profiles[dc].forward);
            pwm_start(dc_profiles[dc].backward);
        }
    }
}

void dc_set(dc_t dc, F64 speed)
{
    FW_ASSERT(dc_inited && "You need to run dc_init() before dc_set()");
    FW_ASSERT(dc >= 0 && dc < DC_N, dc);
    FW_ASSERT(configured[dc] && "DC motor has not been configured", dc);

    FW_ASSERT(speed >= -1.0 && speed <= 1.0 && "Speed must be between -1 and 1", speed * 100);

    if (speed > 0.0)
    {
        // Run the motor forward
        // Stop the reverse pwm
        pwm_set(dc_profiles[dc].forward, speed);
        pwm_set(dc_profiles[dc].backward, 0.0);
    }
    else
    {
        // Run the motor backwards
        // Stop forward pwm
        pwm_set(dc_profiles[dc].forward, 0.0);
        pwm_set(dc_profiles[dc].backward, -speed);
    }
}
