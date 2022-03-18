#include <drv/spi.h>
#include <drv/gpio.h>

/// Base Physical Address of the BCM 2835 peripheral registers
#define BCM2835_PERI_BASE               0x20000000
/// Base Physical Address of the SPI0 registers
#define BCM2835_SPI0_BASE               (BCM2835_PERI_BASE + 0x204000)


// Defines for SPI
// GPIO register offsets from BCM2835_SPI0_BASE.
// Offsets into the SPI Peripheral block in bytes per 10.5 SPI Register Map
typedef struct
{
    volatile U32 CS;                        ///< SPI Master Control and Status
    volatile U32 FIFO;                      ///< SPI Master Tx and Rx FIFOs
    volatile U32 CLK;                       ///< SPI Master Clock Divider
    volatile U32 DLEN;                      ///< SPI Master Data Length
    volatile U32 LTOH;                      //!< SPI LOSSI mode TOH
    volatile U32 DC;                        //!< SPI DMA DREQ Controls
} bcm2835_SPI;

// Register masks for SPI0_CS
#define BCM2835_SPI0_CS_LEN_LONG             0x02000000 ///< Enable Long data word in Lossi mode if DMA_LEN is set
#define BCM2835_SPI0_CS_DMA_LEN              0x01000000 ///< Enable DMA mode in Lossi mode
#define BCM2835_SPI0_CS_CSPOL2               0x00800000 ///< Chip Select 2 Polarity
#define BCM2835_SPI0_CS_CSPOL1               0x00400000 ///< Chip Select 1 Polarity
#define BCM2835_SPI0_CS_CSPOL0               0x00200000 ///< Chip Select 0 Polarity
#define BCM2835_SPI0_CS_RXF                  0x00100000 ///< RXF - RX FIFO Full
#define BCM2835_SPI0_CS_RXR                  0x00080000 ///< RXR RX FIFO needs Reading ( full)
#define BCM2835_SPI0_CS_TXD                  0x00040000 ///< TXD TX FIFO can accept Data
#define BCM2835_SPI0_CS_RXD                  0x00020000 ///< RXD RX FIFO contains Data
#define BCM2835_SPI0_CS_DONE                 0x00010000 ///< Done transfer Done
#define BCM2835_SPI0_CS_TE_EN                0x00008000 ///< Unused
#define BCM2835_SPI0_CS_LMONO                0x00004000 ///< Unused
#define BCM2835_SPI0_CS_LEN                  0x00002000 ///< LEN LoSSI enable
#define BCM2835_SPI0_CS_REN                  0x00001000 ///< REN Read Enable
#define BCM2835_SPI0_CS_ADCS                 0x00000800 ///< ADCS Automatically Deassert Chip Select
#define BCM2835_SPI0_CS_INTR                 0x00000400 ///< INTR Interrupt on RXR
#define BCM2835_SPI0_CS_INTD                 0x00000200 ///< INTD Interrupt on Done
#define BCM2835_SPI0_CS_DMAEN                0x00000100 ///< DMAEN DMA Enable
#define BCM2835_SPI0_CS_TA                   0x00000080 ///< Transfer Active
#define BCM2835_SPI0_CS_CSPOL                0x00000040 ///< Chip Select Polarity
#define BCM2835_SPI0_CS_CLEAR                0x00000030 ///< Clear FIFO Clear RX and TX
#define BCM2835_SPI0_CS_CLEAR_RX             0x00000020 ///< Clear FIFO Clear RX
#define BCM2835_SPI0_CS_CLEAR_TX             0x00000010 ///< Clear FIFO Clear TX
#define BCM2835_SPI0_CS_CPOL                 0x00000008 ///< Clock Polarity
#define BCM2835_SPI0_CS_CPHA                 0x00000004 ///< Clock Phase
#define BCM2835_SPI0_CS_CS                   0x00000003 ///< Chip Select

/// \brief bcm2835SPIBitOrder SPI Bit order
/// Specifies the SPI data bit ordering for bcm2835_spi_setBitOrder()
typedef enum
{
    BCM2835_SPI_BIT_ORDER_LSBFIRST = 0,  ///< LSB First
    BCM2835_SPI_BIT_ORDER_MSBFIRST = 1   ///< MSB First
} bcm2835SPIBitOrder;

/// \brief SPI Data mode
/// Specify the SPI data mode to be passed to bcm2835_spi_setDataMode()
typedef enum
{
    BCM2835_SPI_MODE0 = 0,  ///< CPOL = 0, CPHA = 0
    BCM2835_SPI_MODE1 = 1,  ///< CPOL = 0, CPHA = 1
    BCM2835_SPI_MODE2 = 2,  ///< CPOL = 1, CPHA = 0
    BCM2835_SPI_MODE3 = 3,  ///< CPOL = 1, CPHA = 1
} bcm2835SPIMode;

/// \brief bcm2835SPIChipSelect
/// Specify the SPI chip select pin(s)
typedef enum
{
    BCM2835_SPI_CS0 = 0,     ///< Chip Select 0
    BCM2835_SPI_CS1 = 1,     ///< Chip Select 1
    BCM2835_SPI_CS2 = 2,     ///< Chip Select 2 (ie pins CS1 and CS2 are asserted)
    BCM2835_SPI_CS_NONE = 3, ///< No CS, control it yourself
} bcm2835SPIChipSelect;

/// \brief bcm2835SPIClockDivider
/// Specifies the divider used to generate the SPI clock from the system clock.
/// Figures below give the divider, clock period and clock frequency.
/// Clock divided is based on nominal base clock rate of 250MHz
/// It is reported that (contrary to the documentation) any even divider may used.
/// The frequencies shown for each divider have been confirmed by measurement
typedef enum
{
    BCM2835_SPI_CLOCK_DIVIDER_65536 = 0,       ///< 65536 = 262.144us = 3.814697260kHz
    BCM2835_SPI_CLOCK_DIVIDER_32768 = 32768,   ///< 32768 = 131.072us = 7.629394531kHz
    BCM2835_SPI_CLOCK_DIVIDER_16384 = 16384,   ///< 16384 = 65.536us = 15.25878906kHz
    BCM2835_SPI_CLOCK_DIVIDER_8192 = 8192,    ///< 8192 = 32.768us = 30/51757813kHz
    BCM2835_SPI_CLOCK_DIVIDER_4096 = 4096,    ///< 4096 = 16.384us = 61.03515625kHz
    BCM2835_SPI_CLOCK_DIVIDER_2048 = 2048,    ///< 2048 = 8.192us = 122.0703125kHz
    BCM2835_SPI_CLOCK_DIVIDER_1024 = 1024,    ///< 1024 = 4.096us = 244.140625kHz
    BCM2835_SPI_CLOCK_DIVIDER_512 = 512,     ///< 512 = 2.048us = 488.28125kHz
    BCM2835_SPI_CLOCK_DIVIDER_256 = 256,     ///< 256 = 1.024us = 976.5625MHz
    BCM2835_SPI_CLOCK_DIVIDER_128 = 128,     ///< 128 = 512ns = = 1.953125MHz
    BCM2835_SPI_CLOCK_DIVIDER_64 = 64,      ///< 64 = 256ns = 3.90625MHz
    BCM2835_SPI_CLOCK_DIVIDER_32 = 32,      ///< 32 = 128ns = 7.8125MHz
    BCM2835_SPI_CLOCK_DIVIDER_16 = 16,      ///< 16 = 64ns = 15.625MHz
    BCM2835_SPI_CLOCK_DIVIDER_8 = 8,       ///< 8 = 32ns = 31.25MHz
    BCM2835_SPI_CLOCK_DIVIDER_4 = 4,       ///< 4 = 16ns = 62.5MHz
    BCM2835_SPI_CLOCK_DIVIDER_2 = 2,       ///< 2 = 8ns = 125MHz, fastest you can get
    BCM2835_SPI_CLOCK_DIVIDER_1 = 1,       ///< 1 = 262.144us = 3.814697260kHz, same as 0/65536
} bcm2835SPIClockDivider;

#define SPI   ((bcm2835_SPI*) BCM2835_SPI0_BASE)

void spi_init(spi_t channel, U32 baud_rate)
{
    (void) channel; // there is only a single SPI channel on BCM2835
    (void) baud_rate;   // TODO(tumbar) Find a clock divider that will approach this

    gpio_init(GPIO_P1_26, GPIO_FUNCTION_ALT0);  // CE1
    gpio_init(GPIO_P1_26, GPIO_FUNCTION_ALT0);  // CE0
    gpio_init(GPIO_P1_26, GPIO_FUNCTION_ALT0);  // MISO
    gpio_init(GPIO_P1_26, GPIO_FUNCTION_ALT0);  // MOSI
    gpio_init(GPIO_P1_26, GPIO_FUNCTION_ALT0);  // CLK

    // Clear out the SPI peripheral
    SPI->CS = 0;
    SPI->CS |= BCM2835_SPI0_CS_CLEAR;

    // Enable DMA
    SPI->CS |= BCM2835_SPI0_CS_DMAEN;

    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);  // 4 Mhz

    // Control the CS pin externally
    bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);
}
