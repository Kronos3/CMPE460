#ifndef CMPE460_I2C_MUX_H
#define CMPE460_I2C_MUX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fw.h>

#ifndef __i2c_mux_LINKED__
#error "You need to link 'i2c_mux' to use this header"
#endif

typedef enum
{
    I2C_MUX_CH_NONE = 0x0,
    I2C_MUX_CH_0 = 1 << 0,
    I2C_MUX_CH_1 = 1 << 1,
    I2C_MUX_CH_2 = 1 << 2,
    I2C_MUX_CH_3 = 1 << 3,
    I2C_MUX_CH_4 = 1 << 4,
    I2C_MUX_CH_5 = 1 << 5,
    I2C_MUX_CH_6 = 1 << 6,
    I2C_MUX_CH_7 = 1 << 7,
    I2C_MUX_CH_ALL = (
            I2C_MUX_CH_0 | I2C_MUX_CH_1 | I2C_MUX_CH_2 |
            I2C_MUX_CH_3 | I2C_MUX_CH_4 | I2C_MUX_CH_5 |
            I2C_MUX_CH_6 | I2C_MUX_CH_7
            )
} i2c_mux_t;

/**
 * Initialize the I2C multiplexer given
 * the hardware I2C address. This address depends what voltages
 * the A0, A1, A2 pins are connected:
 *      A2,  A1,  A0,     I2C (Hex)
 *       L    L    L        0x70
 *       L    L    H        0x71
 *       L    H    L        0x72
 *       L    H    H        0x73
 *       H    L    L        0x74
 *       H    L    H        0x75
 *       H    H    L        0x76
 *       H    H    H        0x77
 * @param a2_a1_a0 a2_a1_a0 mask connected to the multiplexer
 */
void i2c_mux_init(U8 a2_a1_a0);

/**
 * Mask a set of I2C channels to transmit to
 * @param i2c_channels i2c channels to enable/disable
 */
void i2c_mux_set(i2c_mux_t i2c_channels);

/**
 * Write some data to a multiplexed set of
 * I2C channels
 * @param i2c_channels channels to write to
 * @param addr addr
 * @param data data to send to I2C channels
 * @param n number of bytes to send from data
 */
void i2c_mux_write(i2c_mux_t i2c_channels,
                   U8 addr,
                   const U8* data,
                   U32 n);

/**
 * Read some dataa from an I2C device
 * Note: This API does NOT set the multiplexer before
 *       attempting to read, it will simply assert that the
 *       correct mask set is enabled before initiating the read
 *       This is because I2C slaves usually require you to send
 *       them a command before they reply with some data, this
 *       means that the mask should already be correct. If not
 *       you can always use i2c_mux_set() before.
 * @param i2c_channels channels to read from
 * @param data destination of data
 * @param n number of bytes to read
 */
void i2c_mux_read(i2c_mux_t i2c_channels,
                  U8* data,
                  U32 n);

#ifdef __cplusplus
}
#endif

#endif //CMPE460_I2C_MUX_H
