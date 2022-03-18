#ifndef CMPE460_BCM2835_TIM_H
#define CMPE460_BCM2835_TIM_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BCM2835_TIMER_BASE_FREQ
#define BCM2835_TIMER_BASE_FREQ (1000)
#endif // BCM2835_TIMER_BASE_FREQ

// The BCM2835 board has only one timer
// This means we don't have hardware that can
// perform independent timing tasks
// All timers will be 'virtual' timers and be derived from the
// 500 kHz base frequency used on the single timer
typedef enum
{
    TIM_0,
    TIM_1,
    TIM_2,
    TIM_3,
    TIM_4,
    TIM_5,
    TIM_N,
} tim_t;

#ifdef __cplusplus
}
#endif

#endif //CMPE460_BCM2835_TIM_H
