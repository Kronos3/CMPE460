// rpi.c
// C and C++ support for Broadcom BCM 2835 as used in Raspberry Pi
// http://elinux.org/RPi_Low-level_peripherals
// http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
//
// Author: Mike McCauley
// Copyright (C) 2011-2013 Mike McCauley
// $Id: rpi.c,v 1.14 2013/12/06 22:24:52 mikem Exp mikem $


#include <drv/bcm2835/bcm2835.h>

// This define enables a little test program (by default a blinking output on pin RPI_GPIO_PIN_11)
// You can do some safe, non-destructive testing on any platform with:
// gcc rpi.c -D BCM2835_TEST
// ./a.out
//#define BCM2835_TEST

// Uncommenting this define compiles alternative I2C code for the version 1 RPi
// The P1 header I2C pins are connected to SDA0 and SCL0 on V1.
// By default I2C code is generated for the V2 RPi which has SDA1 and SCL1 connected.
// #define I2C_V1

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

// I2C The time needed to transmit one byte. In microseconds.
static int i2c_byte_wait_us = 0;

//
// Low level register access functions
//

// safe read from peripheral
static inline U32 bcm2835_peri_read(const volatile U32* paddr)
{
    // Make sure we don't return the _last_ read which might get lost
    // if subsequent code changes to a different peripheral
    U32 ret = *paddr;
    *paddr; // Read without assigning to an unused variable
    return ret;
}

// read from peripheral without the read barrier
static inline U32 bcm2835_peri_read_nb(const volatile U32* paddr)
{
    return *paddr;
}

// safe write to peripheral
static inline void bcm2835_peri_write(volatile U32* paddr, U32 value)
{
    // Make sure we don't rely on the first write, which may get
    // lost if the previous access was to a different peripheral.
    *paddr = value;
    *paddr = value;
}

// write to peripheral without the write barrier
void bcm2835_peri_write_nb(volatile U32* paddr, U32 value)
{
    *paddr = value;
}

// Set/clear only the bits in value covered by the mask
static inline void bcm2835_peri_set_bits(volatile U32* paddr, U32 value, U32 mask)
{
    U32 v = bcm2835_peri_read(paddr);
    v = (v & ~mask) | (value & mask);
    bcm2835_peri_write(paddr, v);
}

//
// Low level convenience functions
//

// Function select
// pin is a BCM2835 GPIO pin number NOT RPi pin number
//      There are 6 control registers, each control the functions of a block
//      of 10 pins.
//      Each control register has 10 sets of 3 bits per GPIO pin:
//
//      000 = GPIO Pin X is an input
//      001 = GPIO Pin X is an output
//      100 = GPIO Pin X takes alternate function 0
//      101 = GPIO Pin X takes alternate function 1
//      110 = GPIO Pin X takes alternate function 2
//      111 = GPIO Pin X takes alternate function 3
//      011 = GPIO Pin X takes alternate function 4
//      010 = GPIO Pin X takes alternate function 5
//
// So the 3 bits for port X are:
//      X / 10 + ((X % 10) * 3)
COMPILE_ASSERT(&BCM2835_GPIO->FSEL[4] == (U32*)0x20200010, fsel_4_location);
void bcm2835_gpio_fsel(U8 pin, U8 mode)
{
    FW_ASSERT(pin / 10 < 6, pin);

    // Function selects are 10 pins per 32 bit word, 3 bits per pin
    volatile U32* paddr = &BCM2835_GPIO->FSEL[pin / 10];

    U8 shift = (pin % 10) * 3;
    U32 mask = BCM2835_GPIO_FSEL_MASK << shift;
    U32 value = mode << shift;
    bcm2835_peri_set_bits(paddr, value, mask);
}

// Set output pin
COMPILE_ASSERT(&BCM2835_GPIO->SET[1] == (U32*)0x20200020, gpio_set_check);
void bcm2835_gpio_set(U8 pin)
{
    FW_ASSERT(pin < 64, pin);
    volatile U32* paddr = &BCM2835_GPIO->SET[pin / 32];
    U8 shift = pin % 32;
    U32 value = bcm2835_peri_read(paddr);
    bcm2835_peri_write(paddr, value | (1 << shift));
}

// Clear output pin
void bcm2835_gpio_clr(U8 pin)
{
    FW_ASSERT(pin < 64, pin);
    volatile U32* paddr = &BCM2835_GPIO->CLR[pin / 32];
    U8 shift = pin % 32;
    U32 value = bcm2835_peri_read(paddr);
    bcm2835_peri_write(paddr, value | (1 << shift));
}

// Read input pin
U8 bcm2835_gpio_lev(U8 pin)
{
    volatile U32* paddr = &BCM2835_GPIO->LEV[pin / 32];
    U8 shift = pin % 32;
    U32 value = bcm2835_peri_read(paddr);
    return (value & (1 << shift)) ? HIGH : LOW;
}

// See if an event detection bit is set
// Sigh cant support interrupts yet
U8 bcm2835_gpio_eds(U8 pin)
{
    volatile U32* paddr = &BCM2835_GPIO->EDS[pin / 32];
    U8 shift = pin % 32;
    U32 value = bcm2835_peri_read(paddr);
    return (value & (1 << shift)) ? HIGH : LOW;
}

// Write a 1 to clear the bit in EDS
void bcm2835_gpio_set_eds(U8 pin)
{
    volatile U32* paddr = &BCM2835_GPIO->EDS[pin / 32];
    U8 shift = pin % 32;
    U32 value = 1 << shift;
    bcm2835_peri_write(paddr, value);
}

// Rising edge detect enable
void bcm2835_gpio_ren(U8 pin)
{
    volatile U32* paddr = &BCM2835_GPIO->REN[pin / 32];
    U8 shift = pin % 32;
    U32 value = 1 << shift;
    bcm2835_peri_set_bits(paddr, value, value);
}

void bcm2835_gpio_clr_ren(U8 pin)
{
    volatile U32* paddr = &BCM2835_GPIO->REN[pin / 32];
    U8 shift = pin % 32;
    U32 value = 1 << shift;
    bcm2835_peri_set_bits(paddr, 0, value);
}

// Falling edge detect enable
void bcm2835_gpio_fen(U8 pin)
{
    volatile U32* paddr = ((U32*) BCM2835_GPIO_BASE) + BCM2835_GPFEN0 / 4 + pin / 32;
    U8 shift = pin % 32;
    U32 value = 1 << shift;
    bcm2835_peri_set_bits(paddr, value, value);
}

void bcm2835_gpio_clr_fen(U8 pin)
{
    volatile U32* paddr = ((U32*) BCM2835_GPIO_BASE) + BCM2835_GPFEN0 / 4 + pin / 32;
    U8 shift = pin % 32;
    U32 value = 1 << shift;
    bcm2835_peri_set_bits(paddr, 0, value);
}

// High detect enable
void bcm2835_gpio_hen(U8 pin)
{
    volatile U32* paddr = ((U32*) BCM2835_GPIO_BASE) + BCM2835_GPHEN0 / 4 + pin / 32;
    U8 shift = pin % 32;
    U32 value = 1 << shift;
    bcm2835_peri_set_bits(paddr, value, value);
}

void bcm2835_gpio_clr_hen(U8 pin)
{
    volatile U32* paddr = ((U32*) BCM2835_GPIO_BASE) + BCM2835_GPHEN0 / 4 + pin / 32;
    U8 shift = pin % 32;
    U32 value = 1 << shift;
    bcm2835_peri_set_bits(paddr, 0, value);
}

// Low detect enable
void bcm2835_gpio_len(U8 pin)
{
    volatile U32* paddr = ((U32*) BCM2835_GPIO_BASE) + BCM2835_GPLEN0 / 4 + pin / 32;
    U8 shift = pin % 32;
    U32 value = 1 << shift;
    bcm2835_peri_set_bits(paddr, value, value);
}

void bcm2835_gpio_clr_len(U8 pin)
{
    volatile U32* paddr = ((U32*) BCM2835_GPIO_BASE) + BCM2835_GPLEN0 / 4 + pin / 32;
    U8 shift = pin % 32;
    U32 value = 1 << shift;
    bcm2835_peri_set_bits(paddr, 0, value);
}

// Async rising edge detect enable
void bcm2835_gpio_aren(U8 pin)
{
    volatile U32* paddr = ((U32*) BCM2835_GPIO_BASE) + BCM2835_GPAREN0 / 4 + pin / 32;
    U8 shift = pin % 32;
    U32 value = 1 << shift;
    bcm2835_peri_set_bits(paddr, value, value);
}

void bcm2835_gpio_clr_aren(U8 pin)
{
    volatile U32* paddr = ((U32*) BCM2835_GPIO_BASE) + BCM2835_GPAREN0 / 4 + pin / 32;
    U8 shift = pin % 32;
    U32 value = 1 << shift;
    bcm2835_peri_set_bits(paddr, 0, value);
}

// Async falling edge detect enable
void bcm2835_gpio_afen(U8 pin)
{
    volatile U32* paddr = ((U32*) BCM2835_GPIO_BASE) + BCM2835_GPAFEN0 / 4 + pin / 32;
    U8 shift = pin % 32;
    U32 value = 1 << shift;
    bcm2835_peri_set_bits(paddr, value, value);
}

void bcm2835_gpio_clr_afen(U8 pin)
{
    volatile U32* paddr = ((U32*) BCM2835_GPIO_BASE) + BCM2835_GPAFEN0 / 4 + pin / 32;
    U8 shift = pin % 32;
    U32 value = 1 << shift;
    bcm2835_peri_set_bits(paddr, 0, value);
}

// Set pullup/down
void bcm2835_gpio_pud(U8 pud)
{
    volatile U32* paddr = ((U32*) BCM2835_GPIO_BASE) + BCM2835_GPPUD / 4;
    bcm2835_peri_write(paddr, pud);
}

// Pullup/down clock
// Clocks the value of pud into the GPIO pin
void bcm2835_gpio_pudclk(U8 pin, U8 on)
{
    volatile U32* paddr = ((U32*) BCM2835_GPIO_BASE) + BCM2835_GPPUDCLK0 / 4 + pin / 32;
    U8 shift = pin % 32;
    bcm2835_peri_write(paddr, (on ? 1 : 0) << shift);
}

// Read GPIO pad behaviour for groups of GPIOs
U32 bcm2835_gpio_pad(U8 group)
{
    volatile U32* paddr = ((U32*)BCM2835_GPIO_PADS) + BCM2835_PADS_GPIO_0_27 / 4 + group * 2;
    return bcm2835_peri_read(paddr);
}

// Set GPIO pad behaviour for groups of GPIOs
// powerup value for al pads is
// BCM2835_PAD_SLEW_RATE_UNLIMITED | BCM2835_PAD_HYSTERESIS_ENABLED | BCM2835_PAD_DRIVE_8mA
void bcm2835_gpio_set_pad(U8 group, U32 control)
{
    volatile U32* paddr = ((U32*)BCM2835_GPIO_PADS) + BCM2835_PADS_GPIO_0_27 / 4 + group * 2;
    bcm2835_peri_write(paddr, control | BCM2835_PAD_PASSWRD);
}

//
// Higher level convenience functions
//

// Set the state of an output
void bcm2835_gpio_write(U8 pin, U8 on)
{
    if (on)
        bcm2835_gpio_set(pin);
    else
        bcm2835_gpio_clr(pin);
}

// Set the pullup/down resistor for a pin
//
// The GPIO Pull-up/down Clock Registers control the actuation of internal pull-downs on
// the respective GPIO pins. These registers must be used in conjunction with the GPPUD
// register to effect GPIO Pull-up/down changes. The following sequence of events is
// required:
// 1. Write to GPPUD to set the required control signal (i.e. Pull-up or Pull-Down or neither
// to remove the current Pull-up/down)
// 2. Wait 150 cycles ? this provides the required set-up time for the control signal
// 3. Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to
// modify ? NOTE only the pads which receive a clock will be modified, all others will
// retain their previous state.
// 4. Wait 150 cycles ? this provides the required hold time for the control signal
// 5. Write to GPPUD to remove the control signal
// 6. Write to GPPUDCLK0/1 to remove the clock
//
// RPi has P1-03 and P1-05 with 1k8 pullup resistor
void bcm2835_gpio_set_pud(U8 pin, U8 pud)
{
    bcm2835_gpio_pud(pud);
    for (volatile U32 i = -1; i > 0; i--); // delay
    bcm2835_gpio_pudclk(pin, 1);
    for (volatile U32 i = -1; i > 0; i--); // delay
    bcm2835_gpio_pud(BCM2835_GPIO_PUD_OFF);
    bcm2835_gpio_pudclk(pin, 0);
}

void bcm2835_spi_begin(void)
{
    // Set the SPI0 pins to the Alt 0 function to enable SPI0 access on them
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_26, BCM2835_GPIO_FSEL_ALT0); // CE1
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_24, BCM2835_GPIO_FSEL_ALT0); // CE0
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_21, BCM2835_GPIO_FSEL_ALT0); // MISO
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_19, BCM2835_GPIO_FSEL_ALT0); // MOSI
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_23, BCM2835_GPIO_FSEL_ALT0); // CLK

    // Set the SPI CS register to the some sensible defaults
    volatile U32* paddr = ((U32*)BCM2835_SPI0_BASE) + BCM2835_SPI0_CS / 4;
    bcm2835_peri_write(paddr, 0); // All 0s

    // Clear TX and RX fifos
    bcm2835_peri_write_nb(paddr, BCM2835_SPI0_CS_CLEAR);
}

void bcm2835_spi_end(void)
{
    // Set all the SPI0 pins back to input
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_26, BCM2835_GPIO_FSEL_INPT); // CE1
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_24, BCM2835_GPIO_FSEL_INPT); // CE0
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_21, BCM2835_GPIO_FSEL_INPT); // MISO
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_19, BCM2835_GPIO_FSEL_INPT); // MOSI
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_23, BCM2835_GPIO_FSEL_INPT); // CLK
}

void bcm2835_spi_setBitOrder(U8 order)
{
    // BCM2835_SPI_BIT_ORDER_MSBFIRST is the only one supported by SPI0
    (void) order;
}

// defaults to 0, which means a divider of 65536.
// The divisor must be a power of 2. Odd numbers
// rounded down. The maximum SPI clock rate is
// of the APB clock
void bcm2835_spi_setClockDivider(U16 divider)
{
    volatile U32* paddr = ((U32*)BCM2835_SPI0_BASE) + BCM2835_SPI0_CLK / 4;
    bcm2835_peri_write(paddr, divider);
}

void bcm2835_spi_setDataMode(U8 mode)
{
    volatile U32* paddr = ((U32*)BCM2835_SPI0_BASE) + BCM2835_SPI0_CS / 4;
    // Mask in the CPO and CPHA bits of CS
    bcm2835_peri_set_bits(paddr, mode << 2, BCM2835_SPI0_CS_CPOL | BCM2835_SPI0_CS_CPHA);
}

// Writes (and reads) a single byte to SPI
U8 bcm2835_spi_transfer(U8 value)
{
    volatile U32* paddr = ((U32*)BCM2835_SPI0_BASE) + BCM2835_SPI0_CS / 4;
    volatile U32* fifo = ((U32*)BCM2835_SPI0_BASE) + BCM2835_SPI0_FIFO / 4;

    // This is Polled transfer as per section 10.6.1
    // BUG ALERT: what happens if we get interupted in this section, and someone else
    // accesses a different peripheral? 
    // Clear TX and RX fifos
    bcm2835_peri_set_bits(paddr, BCM2835_SPI0_CS_CLEAR, BCM2835_SPI0_CS_CLEAR);

    // Set TA = 1
    bcm2835_peri_set_bits(paddr, BCM2835_SPI0_CS_TA, BCM2835_SPI0_CS_TA);

    // Maybe wait for TXD
    while (!(bcm2835_peri_read(paddr) & BCM2835_SPI0_CS_TXD));

    // Write to FIFO, no barrier
    bcm2835_peri_write_nb(fifo, value);

    // Wait for DONE to be set
    while (!(bcm2835_peri_read_nb(paddr) & BCM2835_SPI0_CS_DONE));

    // Read any byte that was sent back by the slave while we sere sending to it
    U32 ret = bcm2835_peri_read_nb(fifo);

    // Set TA = 0, and also set the barrier
    bcm2835_peri_set_bits(paddr, 0, BCM2835_SPI0_CS_TA);

    return ret;
}

// Writes (and reads) an number of bytes to SPI
void bcm2835_spi_transfernb(const U8* tbuf, U8* rbuf, U32 len)
{
    volatile U32* paddr = ((U32*)BCM2835_SPI0_BASE) + BCM2835_SPI0_CS / 4;
    volatile U32* fifo = ((U32*)BCM2835_SPI0_BASE) + BCM2835_SPI0_FIFO / 4;
    U32 TXCnt = 0;
    U32 RXCnt = 0;

    // This is Polled transfer as per section 10.6.1
    // BUG ALERT: what happens if we get interupted in this section, and someone else
    // accesses a different peripheral? 

    // Clear TX and RX fifos
    bcm2835_peri_set_bits(paddr, BCM2835_SPI0_CS_CLEAR, BCM2835_SPI0_CS_CLEAR);

    // Set TA = 1
    bcm2835_peri_set_bits(paddr, BCM2835_SPI0_CS_TA, BCM2835_SPI0_CS_TA);

    // Use the FIFO's to reduce the interbyte times
    while ((TXCnt < len) || (RXCnt < len))
    {
        // TX fifo not full, so add some more bytes
        while (((bcm2835_peri_read(paddr) & BCM2835_SPI0_CS_TXD)) && (TXCnt < len))
        {
            bcm2835_peri_write_nb(fifo, tbuf[TXCnt]);
            TXCnt++;
        }
        //Rx fifo not empty, so get the next received bytes
        while (((bcm2835_peri_read(paddr) & BCM2835_SPI0_CS_RXD)) && (RXCnt < len))
        {
            rbuf[RXCnt] = bcm2835_peri_read_nb(fifo);
            RXCnt++;
        }
    }
    // Wait for DONE to be set
    while (!(bcm2835_peri_read_nb(paddr) & BCM2835_SPI0_CS_DONE));

    // Set TA = 0, and also set the barrier
    bcm2835_peri_set_bits(paddr, 0, BCM2835_SPI0_CS_TA);
}

// Writes an number of bytes to SPI
void bcm2835_spi_writenb(const U8* tbuf, U32 len)
{
    volatile U32* paddr = ((U32*)BCM2835_SPI0_BASE) + BCM2835_SPI0_CS / 4;
    volatile U32* fifo = ((U32*)BCM2835_SPI0_BASE) + BCM2835_SPI0_FIFO / 4;

    // This is Polled transfer as per section 10.6.1
    // BUG ALERT: what happens if we get interupted in this section, and someone else
    // accesses a different peripheral?

    // Clear TX and RX fifos
    bcm2835_peri_set_bits(paddr, BCM2835_SPI0_CS_CLEAR, BCM2835_SPI0_CS_CLEAR);

    // Set TA = 1
    bcm2835_peri_set_bits(paddr, BCM2835_SPI0_CS_TA, BCM2835_SPI0_CS_TA);

    U32 i;
    for (i = 0; i < len; i++)
    {
        // Maybe wait for TXD
        while (!(bcm2835_peri_read(paddr) & BCM2835_SPI0_CS_TXD));

        // Write to FIFO, no barrier
        bcm2835_peri_write_nb(fifo, tbuf[i]);

        // Read from FIFO to prevent stalling
        while (bcm2835_peri_read(paddr) & BCM2835_SPI0_CS_RXD)
            (void) bcm2835_peri_read_nb(fifo);
    }

    // Wait for DONE to be set
    while (!(bcm2835_peri_read_nb(paddr) & BCM2835_SPI0_CS_DONE))
    {
        while (bcm2835_peri_read(paddr) & BCM2835_SPI0_CS_RXD)
            (void) bcm2835_peri_read_nb(fifo);
    }

    // Set TA = 0, and also set the barrier
    bcm2835_peri_set_bits(paddr, 0, BCM2835_SPI0_CS_TA);
}

// Writes (and reads) an number of bytes to SPI
// Read bytes are copied over onto the transmit buffer
void bcm2835_spi_transfern(U8* buf, U32 len)
{
    bcm2835_spi_transfernb(buf, buf, len);
}

void bcm2835_spi_chipSelect(U8 cs)
{
    volatile U32* paddr = ((U32*)BCM2835_SPI0_BASE) + BCM2835_SPI0_CS / 4;
    // Mask in the CS bits of CS
    bcm2835_peri_set_bits(paddr, cs, BCM2835_SPI0_CS_CS);
}

void bcm2835_spi_setChipSelectPolarity(U8 cs, U8 active)
{
    volatile U32* paddr = ((U32*)BCM2835_SPI0_BASE) + BCM2835_SPI0_CS / 4;
    U8 shift = 21 + cs;
    // Mask in the appropriate CSPOLn bit
    bcm2835_peri_set_bits(paddr, active << shift, 1 << shift);
}

void bcm2835_i2c_begin(void)
{
#ifdef I2C_V1
    volatile U32* paddr = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_DIV/4;
    // Set the I2C/BSC0 pins to the Alt 0 function to enable I2C access on them
    bcm2835_gpio_fsel(RPI_GPIO_P1_03, BCM2835_GPIO_FSEL_ALT0); // SDA
    bcm2835_gpio_fsel(RPI_GPIO_P1_05, BCM2835_GPIO_FSEL_ALT0); // SCL
#else
    volatile U32* paddr = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_DIV / 4;
    // Set the I2C/BSC1 pins to the Alt 0 function to enable I2C access on them
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_03, BCM2835_GPIO_FSEL_ALT0); // SDA
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_05, BCM2835_GPIO_FSEL_ALT0); // SCL
#endif

    // Read the clock divider register
    U16 cdiv = bcm2835_peri_read(paddr);
    // Calculate time for transmitting one byte
    // 1000000 = micros seconds in a second
    // 9 = Clocks per byte : 8 bits + ACK
    i2c_byte_wait_us = (I32)(((float) cdiv / BCM2835_CORE_CLK_HZ) * 1000000 * 9);
}

void bcm2835_i2c_end(void)
{
#ifdef I2C_V1
    // Set all the I2C/BSC0 pins back to input
    bcm2835_gpio_fsel(RPI_GPIO_P1_03, BCM2835_GPIO_FSEL_INPT); // SDA
    bcm2835_gpio_fsel(RPI_GPIO_P1_05, BCM2835_GPIO_FSEL_INPT); // SCL
#else
    // Set all the I2C/BSC1 pins back to input
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_03, BCM2835_GPIO_FSEL_INPT); // SDA
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_05, BCM2835_GPIO_FSEL_INPT); // SCL
#endif
}

void bcm2835_i2c_setSlaveAddress(U8 addr)
{
    // Set I2C Device Address
#ifdef I2C_V1
    volatile U32* paddr = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_A / 4;
#else
    volatile U32* paddr = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_A / 4;
#endif
    bcm2835_peri_write(paddr, addr);
}

// defaults to 0x5dc, should result in a 166.666 kHz I2C clock frequency.
// The divisor must be a power of 2. Odd numbers
// rounded down.
void bcm2835_i2c_setClockDivider(U16 divider)
{
#ifdef I2C_V1
    volatile U32* paddr = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_DIV/4;
#else
    volatile U32* paddr = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_DIV / 4;
#endif
    bcm2835_peri_write(paddr, divider);
    // Calculate time for transmitting one byte
    // 1000000 = micros seconds in a second
    // 9 = Clocks per byte : 8 bits + ACK
    i2c_byte_wait_us = (I32)(((float) divider / BCM2835_CORE_CLK_HZ) * 1000000 * 9);
}

// set I2C clock divider by means of a baudrate number
void bcm2835_i2c_set_baudrate(U32 baudrate)
{
    U32 divider;
    // use 0xFFFE mask to limit a max value and round down any odd number
    divider = (BCM2835_CORE_CLK_HZ / baudrate) & 0xFFFE;
    bcm2835_i2c_setClockDivider((U16) divider);
}

// Writes a number of bytes to I2C
U8 bcm2835_i2c_write(const U8* buf, U32 len)
{
#ifdef I2C_V1
    volatile U32* dlen    = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_DLEN/4;
    volatile U32* fifo    = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_FIFO/4;
    volatile U32* status  = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_S/4;
    volatile U32* control = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_C/4;
#else
    volatile U32* dlen = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_DLEN / 4;
    volatile U32* fifo = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_FIFO / 4;
    volatile U32* status = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_S / 4;
    volatile U32* control = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_C / 4;
#endif

    U32 remaining = len;
    U32 i = 0;
    U8 reason = BCM2835_I2C_REASON_OK;

    // Clear FIFO
    bcm2835_peri_set_bits(control, BCM2835_BSC_C_CLEAR_1, BCM2835_BSC_C_CLEAR_1);
    // Clear Status
    bcm2835_peri_write_nb(status, BCM2835_BSC_S_CLKT | BCM2835_BSC_S_ERR | BCM2835_BSC_S_DONE);
    // Set Data Length
    bcm2835_peri_write_nb(dlen, len);
    // pre populate FIFO with max buffer
    while (remaining && (i < BCM2835_BSC_FIFO_SIZE))
    {
        bcm2835_peri_write_nb(fifo, buf[i]);
        i++;
        remaining--;
    }

    // Enable device and start transfer
    bcm2835_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST);

    // Transfer is over when BCM2835_BSC_S_DONE
    while (!(bcm2835_peri_read_nb(status) & BCM2835_BSC_S_DONE))
    {
        while (remaining && (bcm2835_peri_read_nb(status) & BCM2835_BSC_S_TXD))
        {
            // Write to FIFO, no barrier
            bcm2835_peri_write_nb(fifo, buf[i]);
            i++;
            remaining--;
        }
    }

    // Received a NACK
    if (bcm2835_peri_read(status) & BCM2835_BSC_S_ERR)
    {
        reason = BCM2835_I2C_REASON_ERROR_NACK;
    }

        // Received Clock Stretch Timeout
    else if (bcm2835_peri_read(status) & BCM2835_BSC_S_CLKT)
    {
        reason = BCM2835_I2C_REASON_ERROR_CLKT;
    }

        // Not all data is sent
    else if (remaining)
    {
        reason = BCM2835_I2C_REASON_ERROR_DATA;
    }

    bcm2835_peri_set_bits(control, BCM2835_BSC_S_DONE, BCM2835_BSC_S_DONE);

    return reason;
}

// Read an number of bytes from I2C
U8 bcm2835_i2c_read(U8* buf, U32 len)
{
#ifdef I2C_V1
    volatile U32* dlen    = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_DLEN/4;
    volatile U32* fifo    = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_FIFO/4;
    volatile U32* status  = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_S/4;
    volatile U32* control = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_C/4;
#else
    volatile U32* dlen = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_DLEN / 4;
    volatile U32* fifo = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_FIFO / 4;
    volatile U32* status = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_S / 4;
    volatile U32* control = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_C / 4;
#endif

    U32 remaining = len;
    U32 i = 0;
    U8 reason = BCM2835_I2C_REASON_OK;

    // Clear FIFO
    bcm2835_peri_set_bits(control, BCM2835_BSC_C_CLEAR_1, BCM2835_BSC_C_CLEAR_1);
    // Clear Status
    bcm2835_peri_write_nb(status, BCM2835_BSC_S_CLKT | BCM2835_BSC_S_ERR | BCM2835_BSC_S_DONE);
    // Set Data Length
    bcm2835_peri_write_nb(dlen, len);
    // Start read
    bcm2835_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST | BCM2835_BSC_C_READ);

    // wait for transfer to complete
    while (!(bcm2835_peri_read_nb(status) & BCM2835_BSC_S_DONE))
    {
        // we must empty the FIFO as it is populated and not use any delay
        while (bcm2835_peri_read_nb(status) & BCM2835_BSC_S_RXD)
        {
            // Read from FIFO, no barrier
            buf[i] = bcm2835_peri_read_nb(fifo);
            i++;
            remaining--;
        }
    }

    // transfer has finished - grab any remaining stuff in FIFO
    while (remaining && (bcm2835_peri_read_nb(status) & BCM2835_BSC_S_RXD))
    {
        // Read from FIFO, no barrier
        buf[i] = bcm2835_peri_read_nb(fifo);
        i++;
        remaining--;
    }

    // Received a NACK
    if (bcm2835_peri_read(status) & BCM2835_BSC_S_ERR)
    {
        reason = BCM2835_I2C_REASON_ERROR_NACK;
    }

        // Received Clock Stretch Timeout
    else if (bcm2835_peri_read(status) & BCM2835_BSC_S_CLKT)
    {
        reason = BCM2835_I2C_REASON_ERROR_CLKT;
    }

        // Not all data is received
    else if (remaining)
    {
        reason = BCM2835_I2C_REASON_ERROR_DATA;
    }

    bcm2835_peri_set_bits(control, BCM2835_BSC_S_DONE, BCM2835_BSC_S_DONE);

    return reason;
}

// Read an number of bytes from I2C sending a repeated start after writing
// the required register. Only works if your device supports this mode
U8 bcm2835_i2c_read_register_rs(const U8* regaddr, U8* buf, U32 len)
{
#ifdef I2C_V1
    volatile U32* dlen    = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_DLEN/4;
    volatile U32* fifo    = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_FIFO/4;
    volatile U32* status  = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_S/4;
    volatile U32* control = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_C/4;
#else
    volatile U32* dlen = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_DLEN / 4;
    volatile U32* fifo = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_FIFO / 4;
    volatile U32* status = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_S / 4;
    volatile U32* control = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_C / 4;
#endif
    U32 remaining = len;
    U32 i = 0;
    U8 reason = BCM2835_I2C_REASON_OK;

    // Clear FIFO
    bcm2835_peri_set_bits(control, BCM2835_BSC_C_CLEAR_1, BCM2835_BSC_C_CLEAR_1);
    // Clear Status
    bcm2835_peri_write_nb(status, BCM2835_BSC_S_CLKT | BCM2835_BSC_S_ERR | BCM2835_BSC_S_DONE);
    // Set Data Length
    bcm2835_peri_write_nb(dlen, 1);
    // Enable device and start transfer
    bcm2835_peri_write_nb(control, BCM2835_BSC_C_I2CEN);
    bcm2835_peri_write_nb(fifo, regaddr[0]);
    bcm2835_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST);

    // poll for transfer has started
    while (!(bcm2835_peri_read_nb(status) & BCM2835_BSC_S_TA))
    {
        // Linux may cause us to miss entire transfer stage
        if (bcm2835_peri_read(status) & BCM2835_BSC_S_DONE)
            break;
    }

    // Send a repeated start with read bit set in address
    bcm2835_peri_write_nb(dlen, len);
    bcm2835_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST | BCM2835_BSC_C_READ);

    // Wait for write to complete and first byte back.
    bcm2835_st_delay(bcm2835_st_read(), i2c_byte_wait_us * 3);

    // wait for transfer to complete
    while (!(bcm2835_peri_read_nb(status) & BCM2835_BSC_S_DONE))
    {
        // we must empty the FIFO as it is populated and not use any delay
        while (remaining && bcm2835_peri_read_nb(status) & BCM2835_BSC_S_RXD)
        {
            // Read from FIFO, no barrier
            buf[i] = bcm2835_peri_read_nb(fifo);
            i++;
            remaining--;
        }
    }

    // transfer has finished - grab any remaining stuff in FIFO
    while (remaining && (bcm2835_peri_read_nb(status) & BCM2835_BSC_S_RXD))
    {
        // Read from FIFO, no barrier
        buf[i] = bcm2835_peri_read_nb(fifo);
        i++;
        remaining--;
    }

    // Received a NACK
    if (bcm2835_peri_read(status) & BCM2835_BSC_S_ERR)
    {
        reason = BCM2835_I2C_REASON_ERROR_NACK;
    }

        // Received Clock Stretch Timeout
    else if (bcm2835_peri_read(status) & BCM2835_BSC_S_CLKT)
    {
        reason = BCM2835_I2C_REASON_ERROR_CLKT;
    }

        // Not all data is sent
    else if (remaining)
    {
        reason = BCM2835_I2C_REASON_ERROR_DATA;
    }

    bcm2835_peri_set_bits(control, BCM2835_BSC_S_DONE, BCM2835_BSC_S_DONE);

    return reason;
}

// Sending an arbitrary number of bytes before issuing a repeated start 
// (with no prior stop) and reading a response. Some devices require this behavior.
U8 bcm2835_i2c_write_read_rs(const U8* cmds, U32 cmds_len, U8* buf, U32 buf_len)
{
#ifdef I2C_V1
    volatile U32* dlen    = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_DLEN/4;
    volatile U32* fifo    = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_FIFO/4;
    volatile U32* status  = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_S/4;
    volatile U32* control = ((U32*)BCM2835_BSC0_BASE) + BCM2835_BSC_C/4;
#else
    volatile U32* dlen = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_DLEN / 4;
    volatile U32* fifo = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_FIFO / 4;
    volatile U32* status = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_S / 4;
    volatile U32* control = ((U32*)BCM2835_BSC1_BASE) + BCM2835_BSC_C / 4;
#endif

    U32 remaining = cmds_len;
    U32 i = 0;
    U8 reason = BCM2835_I2C_REASON_OK;

    // Clear FIFO
    bcm2835_peri_set_bits(control, BCM2835_BSC_C_CLEAR_1, BCM2835_BSC_C_CLEAR_1);

    // Clear Status
    bcm2835_peri_write_nb(status, BCM2835_BSC_S_CLKT | BCM2835_BSC_S_ERR | BCM2835_BSC_S_DONE);

    // Set Data Length
    bcm2835_peri_write_nb(dlen, cmds_len);

    // pre populate FIFO with max buffer
    while (remaining && (i < BCM2835_BSC_FIFO_SIZE))
    {
        bcm2835_peri_write_nb(fifo, cmds[i]);
        i++;
        remaining--;
    }

    // Enable device and start transfer 
    bcm2835_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST);

    // poll for transfer has started (way to do repeated start, from BCM2835 datasheet)
    while (!(bcm2835_peri_read_nb(status) & BCM2835_BSC_S_TA))
    {
        // Linux may cause us to miss entire transfer stage
        if (bcm2835_peri_read(status) & BCM2835_BSC_S_DONE)
            break;
    }

    remaining = buf_len;
    i = 0;

    // Send a repeated start with read bit set in address
    bcm2835_peri_write_nb(dlen, buf_len);
    bcm2835_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST | BCM2835_BSC_C_READ);

    // Wait for write to complete and first byte back.	
    bcm2835_st_delay(bcm2835_st_read(), i2c_byte_wait_us * (cmds_len + 1));

    // wait for transfer to complete
    while (!(bcm2835_peri_read_nb(status) & BCM2835_BSC_S_DONE))
    {
        // we must empty the FIFO as it is populated and not use any delay
        while (remaining && bcm2835_peri_read_nb(status) & BCM2835_BSC_S_RXD)
        {
            // Read from FIFO, no barrier
            buf[i] = bcm2835_peri_read_nb(fifo);
            i++;
            remaining--;
        }
    }

    // transfer has finished - grab any remaining stuff in FIFO
    while (remaining && (bcm2835_peri_read_nb(status) & BCM2835_BSC_S_RXD))
    {
        // Read from FIFO, no barrier
        buf[i] = bcm2835_peri_read_nb(fifo);
        i++;
        remaining--;
    }

    // Received a NACK
    if (bcm2835_peri_read(status) & BCM2835_BSC_S_ERR)
    {
        reason = BCM2835_I2C_REASON_ERROR_NACK;
    }

        // Received Clock Stretch Timeout
    else if (bcm2835_peri_read(status) & BCM2835_BSC_S_CLKT)
    {
        reason = BCM2835_I2C_REASON_ERROR_CLKT;
    }

        // Not all data is sent
    else if (remaining)
    {
        reason = BCM2835_I2C_REASON_ERROR_DATA;
    }

    bcm2835_peri_set_bits(control, BCM2835_BSC_S_DONE, BCM2835_BSC_S_DONE);

    return reason;
}

// Read the System Timer Counter (64-bits)
U64 bcm2835_st_read(void)
{
    U64 st;
    st = bcm2835_peri_read(&BCM2835_ST->CHI);
    st <<= 32;
    st += bcm2835_peri_read(&BCM2835_ST->CLO);
    return st;
}

// Delays for the specified number of microseconds with offset
void bcm2835_st_delay(U64 offset_micros, U64 micros)
{
    U64 compare = offset_micros + micros;
    while (bcm2835_st_read() < compare);
}

// PWM

void bcm2835_pwm_set_clock(U32 divisor)
{
    // From Gerts code
    divisor &= 0xfff;

    // Stop PWM clock
    bcm2835_peri_write(((U32*)BCM2835_CLOCK_BASE) + BCM2835_PWMCLK_CNTL, BCM2835_PWM_PASSWRD | 0x01);

    // Wait for the clock to be not busy
    while ((bcm2835_peri_read(((U32*)BCM2835_CLOCK_BASE) + BCM2835_PWMCLK_CNTL) & 0x80) != 0);

    // set the clock divider and enable PWM clock
    bcm2835_peri_write(((U32*)BCM2835_CLOCK_BASE) + BCM2835_PWMCLK_DIV, BCM2835_PWM_PASSWRD | (divisor << 12));
    bcm2835_peri_write(((U32*)BCM2835_CLOCK_BASE) + BCM2835_PWMCLK_CNTL, BCM2835_PWM_PASSWRD | 0x11); // Source=osc and enable
}

void bcm2835_pwm_set_mode(U8 channel, U8 markspace, U8 enabled)
{
    U32 control = bcm2835_peri_read((U32*)BCM2835_GPIO_PWM + BCM2835_PWM_CONTROL);

    if (channel == 0)
    {
        if (markspace)
            control |= BCM2835_PWM0_MS_MODE;
        else
            control &= ~BCM2835_PWM0_MS_MODE;
        if (enabled)
            control |= BCM2835_PWM0_ENABLE;
        else
            control &= ~BCM2835_PWM0_ENABLE;
    }
    else if (channel == 1)
    {
        if (markspace)
            control |= BCM2835_PWM1_MS_MODE;
        else
            control &= ~BCM2835_PWM1_MS_MODE;
        if (enabled)
            control |= BCM2835_PWM1_ENABLE;
        else
            control &= ~BCM2835_PWM1_ENABLE;
    }

    // If you use the barrier here, weird things happen, and the commands don't work
    bcm2835_peri_write_nb((U32*)BCM2835_GPIO_PWM + BCM2835_PWM_CONTROL, control);
    //  bcm2835_peri_write_nb(bcm2835_pwm + BCM2835_PWM_CONTROL, BCM2835_PWM0_ENABLE | BCM2835_PWM1_ENABLE | BCM2835_PWM0_MS_MODE | BCM2835_PWM1_MS_MODE);

}

void bcm2835_pwm_set_range(U8 channel, U32 range)
{
    if (channel == 0)
        bcm2835_peri_write_nb((U32*)BCM2835_GPIO_PWM + BCM2835_PWM0_RANGE, range);
    else if (channel == 1)
        bcm2835_peri_write_nb((U32*)BCM2835_GPIO_PWM + BCM2835_PWM1_RANGE, range);
}

void bcm2835_pwm_set_data(U8 channel, U32 data)
{
    if (channel == 0)
        bcm2835_peri_write_nb((U32*)BCM2835_GPIO_PWM + BCM2835_PWM0_DATA, data);
    else if (channel == 1)
        bcm2835_peri_write_nb((U32*)BCM2835_GPIO_PWM + BCM2835_PWM1_DATA, data);
}

#pragma clang diagnostic pop