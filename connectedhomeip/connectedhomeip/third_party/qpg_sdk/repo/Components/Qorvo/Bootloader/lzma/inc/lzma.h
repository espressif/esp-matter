/*
 * Copyright (c) 2020, Qorvo Inc
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
 */

/** @file "lzma.h"
 *
 *  LZMA decompression
 *
 *  Declarations of the public functions and enumerations of lzma.
*/

#ifndef _LZMA_H_
#define _LZMA_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** @enum lzma_result */
//@{
#define lzma_ResultSuccess                                   0x00       /**< Successful decompression */
#define lzma_ResultDataError                                 0x01       /**< Invalid LZMA header detected */
#define lzma_ResultInsufficientMemory                        0x02       /**< The LZMA decompression requires more memory than available (LZMA_WORKING_MEMORY_SIZE), make sure to compress with the embedded platform in mind */
#define lzma_ResultIncomplete                                0x03       /**< The decompression did not complete */
#define lzma_ResultAlignmentError                            0x04       /**< The outputBuffer is not FLASH_PAGE_SIZE aligned or the decompressed image size is not a multiple of FLASH_PAGE_SIZE */
typedef Int32                             lzma_result;
//@}

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @macro LZMA_WORKING_MEMORY_SIZE */
#define LZMA_WORKING_MEMORY_SIZE                     15980 //Should be even number to align properly. 16K is the default requirement for the LZMA SDK. Higher RAM usage did not provide a speed increase
/** @macro LZMA_BASE_SIZE */
#define LZMA_BASE_SIZE                               1846
/** @macro LZMA_LIT_SIZE */
#define LZMA_LIT_SIZE                                768
/** @macro LzmaGetNumProbs(Properties) */
#define LzmaGetNumProbs(Properties)                   (LZMA_BASE_SIZE + (LZMA_LIT_SIZE << ((Properties)->lc + (Properties)->lp)))
/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @struct CLzmaProperties */
    // lc + lp <= 3 requires 16KB RAM (15980)
    // lc + lp <= 4 requires 32KB RAM (28268)
    // lc + lp <= 5 requires 64KB RAM (52844)
typedef struct {
    UInt8                          lc;  /**< Number of high bits of the previous byte to use as a context for literal encoding (default: 3) */
    UInt8                          lp;  /**< Number of low bits of the dictionary position to include in literal_pos_state     (default: 0) */
    UInt8                          pb;  /**< Number of low bits of the dictionary position to include in pos_state             (default: 2) */
} CLzmaProperties;

/** @struct CLzmaDecoderState */
typedef struct {
    CLzmaProperties                Properties;
    UInt16*                        Probs; /**< RAM buffer to perform decompression*/
} CLzmaDecoderState;

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief: Helper function to validate @p inputBuffer contains valid LZMA
           compressed data
 * @param[in]   inputBuffer     LZMA compressed data
 * @param[in]   inputBufferSize Size of the compressed data (incl. LZMA header)
 *
 * @return     lzma_ResultSuccess on success
 *             lzma_ResultDataError on invalid LZMA encoded properties
 *             lzma_ResultAlignmentError decompressed image is no multiple of
 *                                       FLASH_PAGE_SIZE
 *             lzma_ResultInsufficientMemory
 */
lzma_result lzma_IsValidInput(const UInt8* inputBuffer, UInt32 inputBufferSize);

/**
 * @brief: Helper function to validate @p outputBuffer is valid
 * @param[in]  outputBuffer     Location where the decompressed data will be
 *                              written to.
 *                              NOTE: This address should be FLASH_PAGE_SIZE
 *
 * @return     lzma_ResultSuccess on success
 *             lzma_ResultAlignmentError if output buffer is not FLASH_PAGE_SIZE
 *                                       aligned
 */
lzma_result lzma_IsValidOutput(const UInt8* outputBuffer);

/**
 * @brief: Helper function to provide the length of the data after decompressing
 * @param[in]   inputBuffer     LZMA compressed data
 * @param[in]   inputBufferSize Size of the compressed data (incl. LZMA header)
 * @return      decompressedSize
 */
UInt32 lzma_GetDecompressedSize(const UInt8* inputBuffer, UInt32 inputBufferSize);

/**
 * @brief: This function decompresses the LZMA data from @p inputBuffer.
 *         The output is written into @p outputBuffer, page by page.
 *
 *         NOTE: the @outputBuffer should be erased (as this function will only
 *               write into the buffer). Please do not risk damaging flash cells
 *               by over-writing existing flash data without erasing them first.
 *              ( More info in chip user manual, chapter on FLASH memory)
 *
 * @param[in]  inputBuffer      LZMA compressed data
 * @param[in]  inputBufferSize  Size of the compressed data (incl. LZMA header)
 * @param[in]  outputBuffer     Location where the decompressed data will be
 *                              written to.
 *                              NOTE: This address should be FLASH_PAGE_SIZE
 *                              NOTE: This buffer should be ERASED
 *
 * @return     lzma_ResultSuccess        on success
 *             lzma_ResultDataError      on invalid LZMA encoded properties
 *             lzma_ResultAlignmentError decompressed image is no multiple of
 *                                       FLASH_PAGE_SIZE
 *                                       or output buffer is not FLASH_PAGE_SIZE
 *                                       aligned
 *             lzma_ResultInsufficientMemory
 *             lzma_ResultIncomplete     Failed to complete the decompression
 */
lzma_result lzma_Decode(const UInt8* inputBuffer, UInt32 inputBufferSize, UInt8* outputBuffer);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_LZMA_H_

