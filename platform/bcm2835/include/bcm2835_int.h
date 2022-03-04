#ifndef CMPE460_BCM2835_INT_H
#define CMPE460_BCM2835_INT_H

#define BCM2835_INTC_TOTAL_IRQ       64 + 8

#define BCM2835_BASE_INTC           (0x2000B200)
#define BCM2835_INTC_IRQ_BASIC      (BCM2835_BASE_INTC + 0x00)
#define BCM2835_IRQ_PENDING1        (BCM2835_BASE_INTC + 0x04)
#define BCM2835_IRQ_PENDING2        (BCM2835_BASE_INTC + 0x08)
#define BCM2835_IRQ_FIQ_CTRL        (BCM2835_BASE_INTC + 0x0C)
#define BCM2835_IRQ_ENABLE1         (BCM2835_BASE_INTC + 0x10)
#define BCM2835_IRQ_ENABLE2         (BCM2835_BASE_INTC + 0x14)
#define BCM2835_IRQ_ENABLE_BASIC    (BCM2835_BASE_INTC + 0x18)
#define BCM2835_IRQ_DISABLE1        (BCM2835_BASE_INTC + 0x1C)
#define BCM2835_IRQ_DISABLE2        (BCM2835_BASE_INTC + 0x20)
#define BCM2835_IRQ_DISABLE_BASIC   (BCM2835_BASE_INTC + 0x24)

typedef enum
{
        BCM2835_IRQ_ID_AUX = 29,
        BCM2835_IRQ_ID_SPI_SLAVE = 43,
        BCM2835_IRQ_ID_PWA0 = 45,
        BCM2835_IRQ_ID_PWA1 = 46,
        BCM2835_IRQ_ID_SMI = 48,
        BCM2835_IRQ_ID_GPIO_0 = 49,
        BCM2835_IRQ_ID_GPIO_1 = 50,
        BCM2835_IRQ_ID_GPIO_2 = 51,
        BCM2835_IRQ_ID_GPIO_3 = 52,
        BCM2835_IRQ_ID_I2C = 53,
        BCM2835_IRQ_ID_SPI = 54,
        BCM2835_IRQ_ID_PCM = 55,
        BCM2835_IRQ_ID_UART = 57,

        BCM2835_IRQ_ID_TIMER_0 = 64,
        BCM2835_IRQ_ID_MAILBOX_0 = 65,
        BCM2835_IRQ_ID_DOORBELL_0 = 66,
        BCM2835_IRQ_ID_DOORBELL_1 = 67,
        BCM2835_IRQ_ID_GPU0_HALTED = 68,
} bcm2835_Interrupt;

typedef void (*bcm2835_InterruptHandler)(U32 irq);

/**
 * Register a callback for an interrupt
 * @param irq id of interrupt (bcm2835_Interrupt)
 * @param handler Interrupt handler
 */
void bcm2835_interrupt_register(bcm2835_Interrupt irq, bcm2835_InterruptHandler handler);

/**
 * Enable an interrupt
 * @param irq interrupt to enable
 */
void bcm2835_interrupt_enable(bcm2835_Interrupt irq);
void bcm2835_interrupt_disable(bcm2835_Interrupt irq);

#endif //CMPE460_BCM2835_INT_H
