#ifndef CMPE460_BCM2835_DMA_H
#define CMPE460_BCM2835_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fw.h>

/**
 * Control Blocks (CB) are 8 words (256 bits) in length and must start at a 256-bit aligned
 * address. The format of the CB data structure in memory, is shown below.
 */
typedef struct DmaControlBlock_prv DmaControlBlock __attribute__ ((aligned (32)));
struct DmaControlBlock_prv
{
    U32 TI;                         //!< Transfer information
    U32 SOURCE_AD;                  //!< Source Address
    U32 DEST_AD;                    //!< Destination Address
    U32 TXFR_LEN;                   //!< Transfer length
    U32 STRIDE;                     //!< 2D Mode stride
    DmaControlBlock* NEXTCONBK;     //!< Next Control block address
    U32 _reserved[2];               //!< Reserved, set to zero
};

// If this assertion failed, it's probably because 'next' is 64-bit
// Same sure you are compiling for the correct 32-bit target
COMPILE_ASSERT(sizeof(DmaControlBlock) == 32, sizeof_dma);

#ifdef __cplusplus
}
#endif

#endif //CMPE460_BCM2835_DMA_H
