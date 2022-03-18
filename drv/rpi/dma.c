#include <drv/bcm2835/dma.h>

typedef struct
{
    volatile U32 CS;                                   //!< Control and Status
    volatile U32 CONBLK_AD;                            //!< Control Block Address
    const volatile U32 SOURCE_AD;                      //!< Source Address
    const volatile U32 DEST_AD;                        //!< Destination Address
    const volatile U32 TXFR_LEN;                       //!< Transfer length
    const volatile U32 STRIDE;                         //!< 2D Mode stride
    const volatile DmaControlBlock* NEXTCONBK;         //!< Next Control block address
} DmaChannel;


