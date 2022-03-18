#include <drv/rpi/bcm2835.h>
#include <drv/i2c.h>

void i2c_init(U32 baud_rate)
{
    static bool_t i2c_initialized = FALSE;
    if (i2c_initialized)
    {
        return;
    }

    i2c_initialized = TRUE;

    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(baud_rate);
}

void i2c_set_address(U8 slave_address)
{
    bcm2835_i2c_setSlaveAddress(slave_address);
}

void i2c_write(const U8* data, U32 len)
{
    bcm2835_i2c_write(data, len);
}

void i2c_read(U8* data, U32 len)
{
    bcm2835_i2c_read(data, len);
}
