#include <lib/i2c_mux.h>
#include <drv/i2c.h>

static struct
{
    U8 mux_addr;
    U8 mask;

    // If the mux is not configured, bytes sent to
    // the mux device will not be sent and dropped
    // When not configured, this module will act exactly
    // like the I2C driver
    bool_t configured;
} mux = {0};


void i2c_mux_init(U8 a2_a1_a0)
{
    FW_ASSERT(!(a2_a1_a0 & ~0x7) && "Invalid a2_a1_a0 mask", a2_a1_a0);
    mux.mux_addr = 0x70 | (a2_a1_a0 & 0x7);
    mux.mask = 0x0;
    mux.configured = TRUE;

    // Enable all channels by default
    i2c_mux_set(I2C_MUX_CH_ALL);
}

void i2c_mux_set(i2c_mux_t i2c_channels)
{
    if (mux.configured && mux.mask != i2c_channels)
    {
        mux.mask = i2c_channels;
        i2c_set_address(mux.mux_addr);
        i2c_write(&mux.mask, 1);
    }
}

void i2c_mux_write(i2c_mux_t i2c_channels,
                   U8 addr,
                   const U8* data,
                   U32 n)
{
    FW_ASSERT(data);
    i2c_mux_set(i2c_channels);
    i2c_set_address(addr);
    i2c_write(data, n);
}

void i2c_mux_read(i2c_mux_t i2c_channels,
                  U8* data,
                  U32 n)
{
    FW_ASSERT(data);
    FW_ASSERT(mux.mask == i2c_channels && "Expected i2c channels to already be selected: expected, got",
              i2c_channels, mux.mask);

    i2c_read(data, n);
}
