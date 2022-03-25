#ifndef CMPE460_SPI_H
#define CMPE460_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fw.h>

#ifndef __spi_LINKED__
#error "You need to link 'spi' to use this header"
#endif

#ifdef __BCM2835__
#include "rpi/spi.h"
#elif defined(__MSP432P4__)
//#include "msp432p4/spi.h"
#error "SPI not supported on __MSP432P4__ yet!"
#endif

/**
 * Initialize a SPI channel to a certain baud rate
 * Also initialization GPIO pins to the correct modes
 * @param channel SPI channel (platform specific)
 * @param baud_rate SPI baud rate
 */
void spi_init(spi_t channel, U32 baud_rate);

/**
 * Read bytes from a SPI channel
 * repeated_tx_data is output repeatedly on TX as data is read in from RX.
 * Generally this can be 0, but some devices require a specific value here,
 *      e.g. SD cards expect 0xff
 * @param channel channel to read from
 * @param repeated_tx_data Buffer of data to write
 * @param data destination buffer to read from
 * @param n number of bytes to read from
 */
void spi_read(spi_t channel, U8 repeated_tx_data, U8* data, U32 n);

/**
 * Write some bytes over a SPI channel
 * @param channel channel to write to
 * @param data data to write
 * @param n number of bytes to send
 */
void spi_write(spi_t channel, const U8* data, U32 n);

#ifdef __cplusplus
}
#endif

#endif //CMPE460_SPI_H
