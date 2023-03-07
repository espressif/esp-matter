/*
 * Copyright (c) 2017, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#ifndef _HAL_DMA_H_
#define _HAL_DMA_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


#define HAL_DMA_RESULT_SUCCESS 0
#define HAL_DMA_RESULT_FAIL    1

#define HAL_DMA_CHANNEL_INVALID 0xff

#define HAL_DMA_MAX_CHANNELS 8

#define DMA_CHANNEL_REGMAP_SIZE     (GP_WB_DMAS_SRC_ADDR_1_ADDRESS - GP_WB_DMAS_SRC_ADDR_0_ADDRESS)

#define HAL_DMA_GET_DMA_BASE(x)     (GP_WB_DMAS_BASE_ADDRESS + (x * DMA_CHANNEL_REGMAP_SIZE))
#define HAL_DMA_POINTERS_EQUAL(x,y) ((x).wrap == (y).wrap && (x).offset == (y).offset)
#define HAL_DMA_POINTERS_DIFFER(x,y) (!HAL_DMA_POINTERS_EQUAL(x,y))
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/


typedef UInt8 hal_DmaResult_t;
typedef UInt8 hal_DmaWordMode_t;
typedef UInt8 hal_DmaCircBufSel_t;
typedef UInt8 hal_DmaChannel_t;
typedef UInt8 hal_DmaBufCompleteIntMode_t;

typedef void (*hal_DmaBufferAlmostCompleteInterrupt_t)(hal_DmaChannel_t);
typedef void (*hal_DmaBufferCompleteInterrupt_t)(hal_DmaChannel_t);

typedef struct
{
    UInt16 offset;  /**< Pointer relative to start of DMA buffer */
    UInt8  wrap;    /**< Wrap flag, toggles between 0 and 1 every time the offset wraps past the end of the buffer. */
} hal_DmaPointer_t;

typedef struct
{
    hal_DmaChannel_t channel; /* Index from 0 to 7, must be unique and claimed */
    hal_DmaBufferAlmostCompleteInterrupt_t cbAlmostComplete; /* callback function triggered when buffer almost complete interrupt is fired */
    hal_DmaBufferCompleteInterrupt_t cbComplete; /* callback function triggered when buffer complete interrupt is fired */
    hal_DmaWordMode_t wordMode; /* dma_word_mode */
    UInt16 bufferSize; /* Buffer size in bytes */
    hal_DmaPointer_t writePtr; /* Initial buffer write pointer */
    hal_DmaCircBufSel_t circBufSel; /* GP_WB_ENUM_CIRCULAR_BUFFER_SRC_BUFFER or GP_WB_ENUM_CIRCULAR_BUFFER_DEST_BUFFER */
    UInt16 threshold; /* Threshold in bytes */
    UInt8 dmaTriggerSelect; /* dma_trigger_src_select */
    UInt32 srcAddr; /* Source address pointer */
    Bool srcAddrInRam; /* true when in RAM, false when on WB interface */
    UInt32 destAddr; /* Destination address pointer */
    Bool destAddrInRam; /* true when in RAM, false when on WB interface */
    hal_DmaBufCompleteIntMode_t bufCompleteIntMode; /* GP_WB_ENUM_DMA_BUFFER_COMPLETE_MODE_ERROR_MODE or GP_WB_ENUM_DMA_BUFFER_COMPLETE_MODE_STATUS_MODE */
} hal_DmaDescriptor_t;



/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

/* @brief function for claiming a DMA channel
 *
 * returns a channel from 0 to HAL_DMA_MAX_CHANNELS-1 which should be used as unique index for starting the DMA and updating the pointers subsequently
 * Note: there is no unclaim mechanism. if more than HAL_DMA_MAX_CHANNELS distinct channels are needed during the lifetime of a application, the application should manage the claimed DMA channels and repurpose claimed channels when necessary.
 *
 */
hal_DmaChannel_t hal_DmaClaim(void);

/*
 * @brief start a DMA based on the parameters in the DMA descriptor
 *
 */
hal_DmaResult_t hal_DmaStart(hal_DmaDescriptor_t *dma);

/*
 * @brief Update the write or read pointer and pointer wrap values for a given DMA channel
 *
 * wordMode should be the same as the wordMode used in the hal_DmaDescriptor_t
 */
void hal_DmaUpdatePointers(hal_DmaChannel_t channel, hal_DmaPointer_t param);

/*
 * @brief stop a DMA
 *
 */
hal_DmaResult_t hal_DmaStop(hal_DmaChannel_t channel);

/**
 * @brief Return the internal read/write buffer pointer and wrap flag used by the hardware.
 */
hal_DmaPointer_t hal_DmaGetInternalPointer(hal_DmaChannel_t channel);

/**
 * @brief Return the internal read/write buffer pointer and wrap flag used by the hardware.
 */
hal_DmaPointer_t hal_DmaGetInternalPointer(hal_DmaChannel_t channel);


/**
 * @brief Disable or enable a DMA channel's interrupt processing.
 * Use when no user data is temporarily available for a src buffer channel instead of
 * deconfiguring/reconfiguring the channel.
 */
void hal_EnableDmaInterrupt(hal_DmaChannel_t channel, Bool enabled);

/**
 * @brief Add a value to a dma pointer, handling wrap.
 */
hal_DmaPointer_t hal_DmaPointer_add(hal_DmaChannel_t channel, hal_DmaPointer_t a, Int32 delta);

/**
 * @brief Compute the difference between two dma pointers
 */

UInt32 hal_DmaPointer_substract(hal_DmaChannel_t channel, hal_DmaPointer_t head, hal_DmaPointer_t tail);

/**
 * @brief Initialize static data structures (use when no automatic bss init is available)
 */
void hal_DmaInit(void);

/**
 * @brief Convenience function to process dma buffers in continuous chunks
 */
UInt16 hal_DmaBuffer_GetNextContinuousSize(hal_DmaPointer_t head, hal_DmaPointer_t tail, UInt16 bufferSize);

/**
 * @brief Retrieve the unmasked buffer complete interrupt status for a dma channel
 */
UInt8 hal_DmaGetUnmaskedBufferCompleteInterrupt(hal_DmaChannel_t channel);

/**
 * @brief Enable or disable the interrupt mask for a dma channel
 */
void hal_DmaEnableCompleteInterruptMask(UInt8 channel, Bool enabled);

/**
 * @brief Enable or disable the interrupt mask for a dma channel
 */
void hal_DmaEnableAlmostCompleteInterruptMask(UInt8 channel, Bool enabled);

/**
 * @brief Check if almost complete interrupt is enabked
 */
Bool hal_DmaIsAlmostCompleteInterruptMaskEnabled(hal_DmaChannel_t channel);

#ifdef __cplusplus
}
#endif

#endif // _HAL_DMA_H_
