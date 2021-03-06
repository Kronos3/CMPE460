#include <drv/i2c.h>
#include <drv/gpio.h>
#include "msp.h"

/*
 * i2c.c
 *
 *
 *  Update to the new register notation
 *	LJBeato
 *	03/23/21
 *
 *  added   listener  09/04/21
 * LJBeato
 */


static GpioPin sda = GPIO_PIN(1, 6);
static GpioPin scl = GPIO_PIN(1, 7);

// This function writes the RECEIVER address to the i2c bus.
// If a RECEIVER chip is at that address, it should respond to
// this with an "ACK".   This function returns TRUE if an
// ACK was found.  Otherwise it returns FALSE.

//// --------------------- I2C0  ---------------------------------------------

void i2c_init(U32 baud_rate)
{
    static bool_t i2c_initialized = FALSE;
    if (i2c_initialized)
    {
        return;
    }

    i2c_initialized = TRUE;

    // make sure module is disabled (in reset mode)
    // EUSCI_B0->CTLW0
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST;

    // set appropriate Port.Pins for SDA/SCL
    // UCB0SDA is P1.6 on primary function
    // UCB0SCL is P1.7 on primary function
    gpio_init(sda, GPIO_FUNCTION_PRIMARY);
    gpio_init(scl, GPIO_FUNCTION_PRIMARY);

    // configure EUSCI_B0->CTLW0 for:

    // bit15      UCA10   - 0 = Own address is a 7-bit address,      1 = Own address is a 10-bit address.
    // bit14      UCSLA10 - 0 = Address LISTENER with 7-bit address, 1 = Address LISTENER with 10-bit address
    // bit13      UCMM    - 0 = Single  MASTER environment,          1 = multi-MASTER environ
    // bit12      reserved
    // ----------------------------------------------------------------

    // bit11      UCMST   -  0 = LISTENER mode, 1 = MASTER mode
    // bits10-9   UCMODEx - 00 = 3-pin SPI
    // 											01 = 4-pin SPI (MASTER or LISTENER enabled if STE = 1)
    // 											10 = 4-pin SPI (MASTER or LISTENER enabled if STE = 0)
    // 											11 = I2C mode
    // bit8       UCSYNC  - For eUSCI_B always read and write as 1.
    // -----------------------------------------------------------------
    //
    // bits7-6    UCSSELx - (ignore in LISTENER mode)
    //											00 -  UCLKI
    //											01 - 	ACLK
    //											10 -  SMCLK
    //											11 -  SMCLK
    // bit5       UCTXACK  - 0 = do not acknowledge LISTENER, 1 = acknowledge LISTENER
    // bit4       UCTR     - 0 = LISTENER,                    1 = MASTER
    //

    // bit3       UCTXNACK - 0 = acknowledge normally,  1 = generate NACK
    // bit2       UCTXSTP  - 0 = DO NOT generate STOP,  1 = generate STOP
    // bit1       UCTXSTT  - 0 = DO NOT generate START, 1 = generate START
    // bit0       UCSWRST  - 0 = not RESET,             1 = RESET
    //
    //

    // 7 bit LISTENER and self,
    // single MASTER

    // MASTER mode
    // I2C mode
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_MODE_3;

    // SMCLK mode
    // don/t acknowledge
    // MASTER
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_UCSSEL_3 | EUSCI_B_CTLW0_TR;

    // ack normal
    // no STOP
    // no START
    // stay RESET
    //
    //EUSCI_B0->CTLW0

    // set clock: 400 KHz
    EUSCI_B0->BRW = SystemCoreClock / (baud_rate);

    // release reset
    // EUSCI_B0->CTLW0
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
}

void i2c_set_address(U8 slave_address)
{
    // initialize RECEIVER address
    // EUSCI_B0->I2CSA
    EUSCI_B0->I2CSA = slave_address;
}

static void i2c_putc(U8 ch)
{
    // write data to TXBUF
    // EUSCI_B0->TXBUF
    EUSCI_B0->TXBUF = ch;

    // wait until byte is transmitted
    // EUSCI_B0->IFG
    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));
}

void i2c_write(const U8* data, U32 len)
{
    FW_ASSERT(data);

    // enable i2c module, (remove from RESET)
    // EUSCI_B0->CTLW0
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;

    // Wait to not be busy
    while(EUSCI_B0->STATW & EUSCI_B_STATW_BBUSY);

    // generate start condition and wait for the bus
    // EUSCI_B0->CTLW0
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;

    // BIT1 of IFG reg is 0 until character has been transmitted, then changes to 1
    // wait until it changes
    // EUSCI_B0->IFG
    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));

    // write data byte by byte to i2c, use putchar
    for (U32 i = 0; i < len; i++)
    {
        i2c_putc(data[i]);
    }

    // force stop
    // EUSCI_B0->CTLW0;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    // wait for transmission to complete
    // EUSCI_B0->IFG
    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_STPIFG));

    // Clear stop signal
    EUSCI_B0->IFG &= ~EUSCI_B_IFG_STPIFG;

    // transmission completed, disable the module (put it back in reset)
    //EUSCI_B0->CTLW0
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;
}
