/*
 * File:        uart.c
 * Purpose:     Provide UART routines for serial IO
 *
 * Notes:
 *
 */

#include "msp.h"
#include "uart.h"  // you need to create this file with the function prototypes
#include <fw.h>

typedef struct {
    EUSCI_A_Type* uart;
    DIO_PORT_Odd_Interruptable_Type* port;
    U32 pin_mask;
} UART_Device;

static const UART_Device uart_devices[] = {
    {EUSCI_A0, P1, BIT(2) | BIT(3)}, // USB UART pins
    {EUSCI_A2, P3, BIT(2) | BIT(3)}, // Pins exposed to IO header for BT
};

void uart_init(uart_t id, U32 baud_rate)
{
    FW_ASSERT(id < sizeof(uart_devices) / sizeof(uart_devices[0]), id);
    const UART_Device* device = &uart_devices[id];

    //Set the UART to RESET state (set bit0 of EUSCI_A0->CTLW0 register to '1'
    device->uart->CTLW0 |= UCSWRST;

    // bit15=0,      no parity bits
    // bit14=x,      not used when parity is disabled
    // bit13=0,      LSB first
    // bit12=0,      8-bit data length
    // bit11=0,      1 stop bit
    // bits10-8=000, asynchronous UART mode
    // bits7-6=11,   clock source to SMCLK
    // bit5=0,       reject erroneous characters and do not set flag
    // bit4=0,       do not set flag for break characters
    // bit3=0,       not dormant
    // bit2=0,       transmit data, not address (not used here)
    // bit1=0,       do not transmit break (not used here)
    // bit0=1,       hold logic in reset state while configuring

    // set CTLW0 - hold logic and configure clock source to SMCLK
    device->uart->CTLW0 = UCSSEL__SMCLK | UCSWRST;

    // baud rate
    // N = clock/baud rate = clock_speed/BAUD_RATE
    // set BRW register
    device->uart->BRW = SystemCoreClock / baud_rate;

    // clear first and second modulation stage bit fields
    // MCTLW register
    device->uart->MCTLW &= ~(0xFFF0);

    // P1.3 = TxD
    // P1.2 = RxD
    // we will be using P1.2, P1.3 for RX and TX but not in I/O mode, so we set Port1 SEL1=0 and SEL0=1
    // set SEL0, SEL1 appropriately
    // Place pins 2 and 3 in primary function mode 01
    device->port->SEL0 |= device->pin_mask;
    device->port->SEL1 &= ~device->pin_mask;

    // CTLW0 register - release from reset state
    device->uart->CTLW0 &= ~UCSWRST;

    // disable interrupts (transmit ready, start received, transmit empty, receive full)
    // IE register;
    device->uart->IE &= ~(
              UCTXCPTIE // Transmit complete interrupt disable
            | UCSTTIE   // Start bit interrupt disable
            | UCTXIE    // Transmit interrupt disable
            | UCRXIE    // Receive interrupt disable
    );
}

U8 uart_getchar(uart_t id)
{
    FW_ASSERT(id < sizeof(uart_devices) / sizeof(uart_devices[0]), id);
    const UART_Device* device = &uart_devices[id];

    // Wait for data
    // IFG register
    // Receive interrupt flag. UCRXIFG is set when UCAxRXBUF has received a
    // complete character.
    while (!(device->uart->IFG & UCRXIFG));

    // read character and store in in_char variable
    // RXBUF register
    //Return the 8-bit data from the receiver
    return device->uart->RXBUF & 0xFF;

}

void uart_putchar(uart_t id, char ch)
{
    FW_ASSERT(id < sizeof(uart_devices) / sizeof(uart_devices[0]), id);
    const UART_Device* device = &uart_devices[id];

    // Wait until transmission of previous bit is complete
    // IFG register
    // Transmit complete interrupt flag. UCTXCPTIFG is set when the entire byte in the
    // internal shift register got shifted out and UCAxTXBUF is empty.
    while (!(device->uart->IFG & UCTXIFG));

    // send ch character to uart
    // TXBUF register
    device->uart->TXBUF = ch;
}

void uart_put(uart_t id, const char* ptr_str)
{
    while (*ptr_str)
    {
        uart_putchar(id, *ptr_str++);
    }
}

void uart_write(uart_t id, const char* ptr, U32 len)
{
    for(U32 i = 0; i < len; i++)
    {
        uart_putchar(id, ptr[i]);
    }
}

char* uart_getline(uart_t id, char buf[], U32 len)
{
    I32 i;
    for (i = 0; i < len - 1; i++)
    {
        buf[i] = uart_getchar(id);

        // Allow the user to view what they are typing
        uart_putchar(id, buf[i]);

        if (buf[i] == '\r')
        {
            // End of line
            uart_putchar(id, '\n');
            break;
        }
        else if (buf[i] == 0x7F) // handle the DEL key
        {
            if (i == 0)
            {
                // No more characters to delete
                i--;
                continue;
            }

            const char* b = "\b \b"; // backspace
            uart_write(id, b, 3);
            i--; // remove the DEL character
            i--; // Delete the last inputted character
        }
    }

    // Add the null terminator
    buf[i] = 0;
    return buf;
}

bool_t uart_peek(uart_t id)
{
    FW_ASSERT(id < sizeof(uart_devices) / sizeof(uart_devices[0]), id);
    const UART_Device* device = &uart_devices[id];

    // Check the UART flag
    return (device->uart->IFG & UCRXIFG) != 0;
}
