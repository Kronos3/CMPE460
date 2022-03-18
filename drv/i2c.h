#ifndef CMPE460_I2C_H
#define CMPE460_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fw.h>

#ifndef __i2c_LINKED__
#error "You need to link 'i2c' to use this header"
#endif

/**
 * Initialize an I2C device with a certain address
 * @param baud_rate I2C clocking speed
 */
void i2c_init(U32 baud_rate);

/**
 * Set the slave address to send bytes to
 * @param slave_address slave address
 */
void i2c_set_address(U8 slave_address);

/**
 * Write N bytes to I2C
 * @param data byte array to send over I2C
 * @param len length of data to send
 */
void i2c_write(const U8* data, U32 len);

/**
 * Read N bytes from I2C
 * @param data destination buffer
 * @param len number of bytes to read
 */
void i2c_read(U8* data, U32 len);

#ifdef __cplusplus
}
#endif

#endif //CMPE460_I2C_H
