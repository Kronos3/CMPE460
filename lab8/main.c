#include <fw.h>
#include <drv/adc.h>
#include <drv/tim.h>
#include <drv/uart.h>
#include <lib/uartlib.h>
#include <drv/msp432p4/switch.h>
#include <lib/math.h>

// Samples per second
#define SAMPLE_PS (20)

// Sampling time
#define SAMPLE_S (4)

#define SAMPLE_N (SAMPLE_PS * SAMPLE_S)

#define TRIGGER_TIMER (TIM32_1)

#define THRESHOLD_V (2.0)

static U16 sample_buf[SAMPLE_N];
static U32 sample_i = 0;
static bool_t timer_running = FALSE;
static bool_t sample_ready = FALSE;

static F64 sample_converted[SAMPLE_N];
static U8 thresh[SAMPLE_N];

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

void toggle_timer(void)
{
    if (sample_ready)
    {
        // The sample has not been processed yet!
        // It is not safe to start or stop the timer

    }

    if (!timer_running)
    {
        uprintf("Sampling for %d seconds...\r\n", SAMPLE_S);
        tim_start(TRIGGER_TIMER);
        timer_running = TRUE;
    }
}

void stats_distance_between_edges(const U8 buffer[SAMPLE_N])
{
    F64 distance_sum = 0;
    U32 num_n = 0;
    I32 last_edge = -1;
    bool_t in_high = FALSE;

    for (U32 i = 0; i < SAMPLE_N; i++)
    {
        if (buffer[i] && !in_high)
        {
            in_high = TRUE;

            // Check if there was an edge already detected
            if (last_edge >= 0)
            {
                distance_sum += i - last_edge;
                num_n++;
            }

            last_edge = (I32)i;
        }
        else
        {
            in_high = FALSE;
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
    uprintf("Average frequency is %f Hz\r\n", hz);
}

I32 main(void)
{
    uart_init(UART_USB, 115200);
    adc_init();

    switch_init(SWITCH_ALL, SWITCH_INT_PRESS, toggle_timer);
    tim_init(TRIGGER_TIMER, get_sample, SAMPLE_PS);

    U16 threshold_sample = adc_voltage_to_raw(THRESHOLD_V);

    while(1)
    {
        // Wait for a signal
        while (!sample_ready);

        math_cast(sample_converted, sample_buf, SAMPLE_N);
        math_threshold(thresh, sample_converted, threshold_sample, SAMPLE_N);

        // Measure the average distance between edges
        stats_distance_between_edges(thresh);

        // Mark the system as ready to take more measurementsz
        sample_ready = FALSE;
    }
}
