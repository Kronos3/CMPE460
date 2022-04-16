#include <fw.h>
#include <drv/adc.h>
#include <drv/tim.h>
#include <drv/uart.h>
#include <lib/uartlib.h>
#include <drv/msp432p4/switch.h>

// Samples per second
#define SAMPLE_PS (100)

// Sampling time
#define SAMPLE_S (5)

#define SAMPLE_N (SAMPLE_PS * SAMPLE_S)

#define TRIGGER_TIMER (TIM32_1)

#define THRESH_HIGH_V (1.5)
#define THRESH_LOW_V (0.5)

typedef enum
{
    ACTION_GO_LOW,
    ACTION_GO_HIGH,
    ACTION_STAY,
} action_t;

typedef enum
{
    STATE_UNKNOWN = -1,
    STATE_LOW = 0,
    STATE_HIGH = 1
} state_t;

static U16 sample_buf[SAMPLE_N];
static U32 sample_i = 0;
static bool_t timer_running = FALSE;
static bool_t sample_ready = FALSE;

void get_sample(void)
{
    FW_ASSERT(sample_i < SAMPLE_S * SAMPLE_PS);
    FW_ASSERT(!sample_ready, sample_i);

    sample_buf[sample_i++] = adc_in();

    if (sample_i >= SAMPLE_N)
    {
        sample_i = 0;
        tim_stop(TRIGGER_TIMER);
        timer_running = FALSE;
        sample_ready = TRUE;
    }
}

action_t get_action(U16 sample)
{
    static U16 high_adc = 0, low_adc = 0;
    if (!high_adc || !low_adc)
    {
        high_adc = adc_voltage_to_raw(THRESH_HIGH_V);
        low_adc = adc_voltage_to_raw(THRESH_LOW_V);
    }

    if (sample >= high_adc)
    {
        return ACTION_GO_HIGH;
    }
    else if (sample <= low_adc)
    {
        return ACTION_GO_LOW;
    }
    else
    {
        return ACTION_STAY;
    }
}

void toggle_timer(void)
{
    if (sample_ready)
    {
        // The sample has not been processed yet!
        // It is not safe to start or stop the time
        return;
    }

    if (!timer_running)
    {
        uprintf("Sampling for %d seconds...\r\n", SAMPLE_S);
        tim_start(TRIGGER_TIMER);
        timer_running = TRUE;
    }
}

void stats_distance_between_edges(const U16 buffer[SAMPLE_N])
{
    F64 distance_sum = 0;
    U32 num_n = 0;
    I32 last_edge = -1;

    state_t current_state = STATE_UNKNOWN;

    for (U32 i = 0; i < SAMPLE_N; i++)
    {
        state_t last_state = current_state;
        switch(get_action(buffer[i]))
        {
            case ACTION_GO_LOW:
                current_state = STATE_LOW;
                break;
            case ACTION_GO_HIGH:
                current_state = STATE_HIGH;
                break;
            case ACTION_STAY:
                current_state = last_state;
                break;
        }

        // Check for rising edge
        if (last_state == STATE_LOW && current_state == STATE_HIGH)
        {
            // Check if there was an edge already detected
            if (last_edge >= 0)
            {
                distance_sum += i - last_edge;
                num_n++;
            }

            last_edge = (I32)i;
        }
    }

    if (num_n == 0)
    {
        uprintf("No edges found!\r\n");
        return;
    }

    F64 distance_average = distance_sum / num_n;

    // Convert distance on samples to Hz
    F64 hz = SAMPLE_PS / distance_average;

    uprintf("Found %d edges in %d seconds\r\n", num_n, SAMPLE_S);
    uprintf("Average frequency is %f Hz (%f bpm)\r\n", hz, hz * 60.0);
}

I32 main(void)
{
    uart_init(UART_USB, 115200);
    adc_init();

    switch_init(SWITCH_ALL, SWITCH_INT_PRESS, toggle_timer);
    tim_init(TRIGGER_TIMER, get_sample, SAMPLE_PS);

    while(1)
    {
        // Wait for a signal
        while (!sample_ready);

        // Measure the average distance between edges
        stats_distance_between_edges(sample_buf);

        // Mark the system as ready to take more measurements
        sample_ready = FALSE;
    }
}
