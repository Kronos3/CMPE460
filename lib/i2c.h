#ifndef CMPE460_I2C_H
#define CMPE460_I2C_H

#include <fw.h>

/**
 * Initialize an I2C device with a certain address
 * @param listenerAddress I2C slave address
 */
void i2c0_init(U32 listenerAddress);

/**
 * Write N bytes to I2C
 * @param data byte array to send over I2C
 * @param len length of data to send
 */
void i2c0_write(const U8* data, U32 len);

/**
 * Send a single byte over I2C
 * @param ch byte to send
 */
void i2c0_putc(U8 ch);

#endif //CMPE460_I2C_H
