/*
 * File:        uart.c
 * Purpose:     Provide UART routines for serial IO
 *
 * Notes:
 *
 */

#include "msp.h"
#include "uart.h"  // you need to create this file with the function prototypes
#include "Common.h"
#include <fw.h>

#define BAUD_RATE 9600      //default baud rate
extern uint32_t SystemCoreClock;  // clock rate of MCU


void uart0_init()
{
    //Set the UART to RESET state (set bit0 of EUSCI_A0->CTLW0 register to '1'
    UCA0CTLW0 |= UCSWRST;

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
    UCA0CTLW0 = UCSSEL__SMCLK | UCSWRST;

    // baud rate
    // N = clock/baud rate = clock_speed/BAUD_RATE
    // set BRW register
    UCA0BRW = SystemCoreClock / BAUD_RATE;

    // clear first and second modulation stage bit fields
    // MCTLW register;
    UCA0MCTLW &= ~(0xFFF0);

    // P1.3 = TxD
    // P1.2 = RxD
    // we will be using P1.2, P1.3 for RX and TX but not in I/O mode, so we set Port1 SEL1=0 and SEL0=1
    // set SEL0, SEL1 appropriately
    // Place pins 2 and 3 in primary function mode 01
    U32 uart_pins = BIT(2) | BIT(3);
    P1->SEL0 |= uart_pins;
    P1->SEL1 &= ~uart_pins;

    // CTLW0 register - release from reset state
    UCA0CTLW0 &= ~UCSWRST;

    // disable interrupts (transmit ready, start received, transmit empty, receive full)
    // IE register;
    UCA0IE &= ~(
              UCTXCPTIE // Transmit complete interrupt disable
            | UCSTTIE   // Start bit interrupt disable
            | UCTXIE    // Transmit interrupt disable
            | UCRXIE    // Receive interrupt disable
    );
}

U8 uart0_getchar()
{
    // Wait for data
    // IFG register
    // Receive interrupt flag. UCRXIFG is set when UCAxRXBUF has received a
    // complete character.
    while (!(UCA0IFG & UCRXIFG));

    // read character and store in in_char variable
    // RXBUF register
    //Return the 8-bit data from the receiver
    return EUSCI_A0->RXBUF & 0xFF;

}

void uart0_putchar(char ch)
{
    // Wait until transmission of previous bit is complete
    // IFG register
    // Transmit complete interrupt flag. UCTXCPTIFG is set when the entire byte in the
    // internal shift register got shifted out and UCAxTXBUF is empty.
    while (!(UCA0IFG & UCTXIFG));

    // send ch character to uart
    // TXBUF register
    UCA0TXBUF = ch;
}

void uart0_put(char* ptr_str)
{
    while (*ptr_str)
    {
        uart0_putchar(*ptr_str++);
    }
}

