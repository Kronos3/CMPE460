#include "adc.h"
#include <msp.h>

#define ADC_MAX (1 << 13)
#define ADC_REF (2.5)

void adc_init(void)
{
    // wait for reference to be idle
    // REF_A->CTL0
    while(REF_A->CTL0 & REFGENBUSY);

    // set reference voltage to 2.5V
    // 1) configure reference for static 2.5V
    // REF_A->CTL0
    REF_A->CTL0 = REFON | REFTCOFF | REFVSEL_3;

    // wait for reference to be idle
    // REF_A->CTL0
    while ((REF_A->CTL0 & REFGENRDY) == 0);

    // 2) ADC14ENC = 0 to allow programming
    // ADC14->CTL0
    ADC14->CTL0 &= ~ADC14_CTL0_ENC;

    // 3) wait for BUSY to be zero
    // ADC14->CTL0
    while (ADC14->CTL0 & ADC14_CTL0_BUSY);

    // ------------------------------------------------------------------
    // 31-30 ADC14PDIV  predivider,            00b = Predivide by 1
    // 29-27 ADC14SHSx  SHM source            000b = ADC14SC bit
    // 26    ADC14SHP   SHM pulse-mode          1b = SAMPCON the sampling timer
    // 25    ADC14ISSH  invert sample-and-hold  0b =  not inverted
    // 24-22 ADC14DIVx  clock divider         000b = /1
    // 21-19 ADC14SSELx clock source select   100b = SMCLK
    // 18-17 ADC14CONSEQx mode select          00b = Single-channel, single-conversion
    // 16    ADC14BUSY  ADC14 busy              0b (read only)

    // 15-12 ADC14SHT1x sample-and-hold time 0011b = 32 clocks (use 32 clocks)

    // 11-8  ADC14SHT0x sample-and-hold time 0011b = 32 clocks (use 32 clocks)

    // 7     ADC14MSC   multiple sample         0b = not multiple
    // 6-5   reserved                          00b (reserved)
    // 4     ADC14ON    ADC14 on                1b = powered up

    // 3-2   reserved                          00b (reserved)
    // 1     ADC14ENC   enable conversion       0b = ADC14 disabled
    // 0     ADC14SC    ADC14 start             0b = No start (yet)
    // ------------------------------------------------------------------
    // 4) single, SMCLK, on, disabled, /1, 32 clocks, SHM	pulse-mode
    // ADC14->CTL0
    ADC14->CTL0 = ADC14_CTL0_SHP | ADC14_CTL0_SSEL__SMCLK | ADC14_CTL0_SHT0__32 | ADC14_CTL0_SHT1__32 | ADC14_CTL0_ON;

    // 20-16 STARTADDx  start addr          00000b = ADC14MEM0
    // 15-6  reserved                  0000000000b (reserved)
    // --
    // 5-4   ADC14RES   ADC14 resolution       11b = 14 bit, 16 clocks
    // --
    // 3     ADC14DF    data read-back format   0b = Binary unsigned
    // 2     REFBURST   reference buffer burst  0b = reference on continuously
    // 1-0   ADC14PWRMD ADC power modes        00b = Regular power mode
    //
    // 5) ADC14MEM0, 14-bit, ref on, regular power
    // ADC14->CTL1
    ADC14->CTL1 = ADC14_CTL1_RES__14BIT;

    // ADC14->MCTL[0]
    // VREF buffered
    // End of sequence
    // 00110b = If ADC14DIF = 0: A6;
    // // 6) 0 to 2.5V, channel 6
    ADC14->MCTL[0] = 0x00000186;

    // 15   ADC14WINCTH Window comp threshold   0b = not used
    // 14   ADC14WINC   Comparator enable       0b = Comparator disabled
    // 13   ADC14DIF    Differential mode       0b = Single-ended mode enabled
    // 12   reserved                            0b (reserved)
    // 11-8 ADC14VRSEL  V(R+) and V(R-)      0001b = V(R+) = VREF, V(R-) = AVSS
    // 7    ADC14EOS    End of sequence         1b = End of sequence
    // 6-5  reserved                           00b (reserved)
    // 4-0  ADC14INCHx  Input channel        0110b = A6, P4.7

    // 7) no interrupts
    // ADC14->IER0
    // ADC14->IER1
    ADC14->IER0 = 0;
    ADC14->IER1 = 0;

    // no interrupts
    //
    // P4.7 is Analog In A6
    // 8) analog mode on A6, P4.7
    // set pins for ADC A6
    // SEL0, SEL1
    P4->SEL0 |= BIT(7);
    P4->SEL1 &= ~BIT(7);

    // 9) enable
    // ADC14->CTL0
    ADC14->CTL0 |= ADC14_CTL0_ENC;
}


// ADC14->IFGR0 bit 0 is set when conversion done
// cleared on read ADC14MEM0
// ADC14->CLRIFGR0 bit 0, write 1 to clear flag
// ADC14->IVx is 0x0C when ADC14MEM0 interrupt flag; Interrupt Flag: ADC14IFG0
// ADC14->MEM[0] 14-bit conversion in bits 13-0 (31-16 undefined, 15-14 zero)
U32 adc_in(void)
{
    // 1) wait for BUSY to be zero  ADC14->CTL0
    // ADC14->CTL0
    while (ADC14->CTL0 & ADC14_CTL0_BUSY);

    // 2) start single conversion
    // ADC14->CTL0
    ADC14->CTL0 |= ADC14_CTL0_SC;

    // 3) wait for ADC14->IFGR0, ADC14->IFGR0 bit 0 is set when conversion done
    // ADC14->IFGR0
    while(!(ADC14->IFGR0 & ADC14_IFGR0_IFG0));

    // 14 bit sample returned  ADC14->MEM[0]
    // ADC14->MEM[0] 14-bit conversion in bits 13-0 (31-16 undefined, 15-14 zero)
    // ADC14->MEM[0]
    U32 out = ADC14->MEM[0] & 0x01FFF;
    FW_ASSERT(!(ADC14->IFGR0 & ADC14_IFGR0_IFG0));
    return out;
}

F64 adc_voltage(U32 adc_i)
{
    return ((F64)adc_i / ADC_MAX) * ADC_REF;
}
