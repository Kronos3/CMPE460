#include <drv/gpio.h>
#include <drv/i2c.h> // i2c init
#include <drv/spi.h>

#include <lib/ardu_cam/ardu_cam.h>

#define SPI_PORT SPI_0

ArduCam::ArduCam(U8 addr, i2c_mux_t mux_channels, GpioPin cs_pin_)
: m_addr(addr), m_mux(mux_channels), m_cs_pin(cs_pin_)
{
    gpio_init(cs_pin_, GPIO_FUNCTION_OUTPUT);
    CS_HIGH();
}

void ArduCam::init()
{
    // Initialize the I2C interface to 400kHz
    // This will also set up the underlying pins
    i2c_init(100 * 1000);

    spi_init(SPI_PORT, 4 * 1000 * 1000);
}

void ArduCam::CS_HIGH()
{
    gpio_output(m_cs_pin, TRUE);
}

void ArduCam::CS_LOW()
{
    gpio_output(m_cs_pin, FALSE);
}

void ArduCam::flush_fifo()
{
    write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

void ArduCam::start_capture()
{
    write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);
}

void ArduCam::clear_fifo_flag()
{
    write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

U8 ArduCam::bus_read(int address)
{
    U8 value;
    CS_LOW();
    //	  SPI.transfer(address);
//		 value = SPI.transfer(0x00);
    CS_HIGH();
    return value;
}

U8 ArduCam::read_fifo()
{
    U8 data;
    data = bus_read(SINGLE_FIFO_READ);
    return data;
}

U8 ArduCam::read_reg(U8 addr)
{
    U8 value = 0;
    addr = addr & 0x7f;
    CS_LOW();
    spi_write(SPI_PORT, &addr, 1);
    spi_read(SPI_PORT, 0, &value, 1);
    CS_HIGH();
    return value;
}

void ArduCam::write_reg(U8 addr, U8 data)
{
    U8 buf[2];
    buf[0] = addr | WRITE_BIT;  // remove read bit as this is a write
    buf[1] = data;
    CS_LOW();
    spi_write(SPI_PORT, buf, 2);
    CS_HIGH();
}

U32 ArduCam::read_fifo_length()
{
    U32 len1, len2, len3, length = 0;
    len1 = read_reg(FIFO_SIZE1);
    len2 = read_reg(FIFO_SIZE2);
    len3 = read_reg(FIFO_SIZE3) & 0x7f;
    length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
    return length;
}

void ArduCam::set_fifo_burst()
{
    U8 value;
    spi_read(SPI_PORT, BURST_FIFO_READ, &value, 1);
}

//Set corresponding bit
void ArduCam::set_bit(U8 addr, U8 bit)
{
    U8 temp;
    temp = read_reg(addr);
    write_reg(addr, temp | bit);
}


//Clear corresponding bit
void ArduCam::clear_bit(U8 addr, U8 bit)
{
    U8 temp;
    temp = read_reg(addr);
    write_reg(addr, temp & (~bit));
}

// Write 8 bit values to 8 bit register address
void ArduCam::ws_8_8(const SensorReg regs[])
{
    for(const SensorReg* iter = regs;
        (iter->reg != 0xffff) | (iter->val != 0xff);
        iter++)
    {
        ws_8_8(iter->reg, iter->val);
    }
}

// Write 16 bit values to 8 bit register address
void ArduCam::ws_8_16(const SensorReg regs[])
{
    for(const SensorReg* iter = regs;
        (iter->reg != 0xffff) | (iter->val != 0xff);
        iter++)
    {
        ws_8_16(iter->reg, iter->val);
    }
}

// Write 8 bit values to 16 bit register address
void ArduCam::ws_16_8(const SensorReg regs[])
{
    for(const SensorReg* iter = regs;
        (iter->reg != 0xffff) | (iter->val != 0xff);
        iter++)
    {
        ws_16_8(iter->reg, iter->val);
    }
}

void ArduCam::ws_16_16(const SensorReg regs[])
{
    for(const SensorReg* iter = regs;
        (iter->reg != 0xffff) | (iter->val != 0xff);
        iter++)
    {
        ws_16_16(iter->reg, iter->val);
    }
}

// Read/write 8 bit value to/from 16 bit register address
void ArduCam::ws_16_8(U16 regID, U8 regDat)
{
    U8 buf[3] = {0};
    buf[0] = (regID >> 8) & 0xff;
    buf[1] = (regID) & 0xff;
    buf[2] = regDat;
    i2c_mux_write(m_mux, m_addr, buf, 3);
}

// Read/write 8 bit value to/from 8 bit register address
void ArduCam::ws_8_8(U8 regID, U8 regDat)
{
    U8 buf[2] = {regID, regDat};
    i2c_mux_write(m_mux, m_addr, buf, 2);
}

void ArduCam::rs_8_8(U8 regID, U8* regDat)
{
    i2c_mux_write(m_mux, m_addr, &regID, 1);
    i2c_mux_read(m_mux, regDat, 1);
}

void ArduCam::rs_16_8(U16 regID, U8* regDat)
{
    U8 buffer[2] = {0};
    buffer[0] = (regID >> 8) & 0xff;
    buffer[1] = regID & 0xff;
    i2c_mux_write(m_mux, m_addr, buffer, 2);
    i2c_mux_read(m_mux, regDat, 1);
}

void ArduCam::ws_8_16(U8 regID, U16 regDat)
{
    U8 buf[3] = {0};
    buf[0] = regID;
    buf[1] = (regDat >> 8) & 0xff;
    buf[2] = (regDat) & 0xff;
    i2c_mux_write(m_mux, m_addr, buf, 3);
}

void ArduCam::rs_8_16(U8 regID, U16* regDat)
{
    i2c_mux_write(m_mux, m_addr, &regID, 1);
    i2c_mux_read(m_mux, reinterpret_cast<U8*>(regDat), 2);
}

void ArduCam::ws_16_16(U16 regID, U16 regDat)
{
    U8 buf[4] = {0};
    buf[0] = (regID >> 8) & 0xff;
    buf[1] = regID & 0xff;
    buf[2] = (regDat >> 8) & 0xff;
    buf[3] = regDat & 0xff;
    i2c_mux_write(m_mux, m_addr, buf, 4);
}

void ArduCam::rs_16_16(U16 regID, U16* regDat)
{
    U8 buf[2] = {0};
    buf[0] = (regID >> 8) & 0xff;
    buf[1] = regID & 0xff;
    i2c_mux_write(m_mux, m_addr, buf, 2);
    i2c_mux_read(m_mux, reinterpret_cast<U8*>(regDat), 2);
}
