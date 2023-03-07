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

/** @file "lzma.c"
 *
 *  LZMA decompression
 *
 *  Implementation of lzma
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_LZMA

/* <CodeGenerator Placeholder> General */
/* </CodeGenerator Placeholder> General */


#include "lzma.h"

/* <CodeGenerator Placeholder> Includes */
#include "hal.h"
#include "gpHal.h"
#include "lzma_def.h"
/* </CodeGenerator Placeholder> Includes */


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> Macro */
/* </CodeGenerator Placeholder> Macro */

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> FunctionalMacro */
/* </CodeGenerator Placeholder> FunctionalMacro */

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> TypeDefinitions */
#if defined(__IAR_SYSTEMS_ICC__)
#pragma pack(1)
typedef struct lzma_header
#else
typedef PACKED_PRE struct PACKED_POST lzma_header
#endif
{
    UInt8 properties;
    UInt32 dictionarySize;
    UInt32 decompressedSize;             /**< Warning: Not filled in by all lzma compression tools, but required for this decompression module*/
    UInt32 reserved;
    UInt8 data[LZMA_BASE_SIZE];          /**< buffer containing the compressed data, only represents the minimal required size here; can be much bigger */
} lzma_header_t;
#if defined(__IAR_SYSTEMS_ICC__)
#pragma pack()
#endif
/* </CodeGenerator Placeholder> TypeDefinitions */

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> StaticData */
/* </CodeGenerator Placeholder> StaticData */

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/* <CodeGenerator Placeholder> StaticFunctionPrototypes */
static lzma_result DecodeProperties(UInt8 encoded_properties, CLzmaProperties* decoded_properties);
/* </CodeGenerator Placeholder> StaticFunctionPrototypes */

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> StaticFunctionDefinitions */
/**
 * @brief: Helper function to decode the properties to use for LZMA decompression
 * @param[in]   encoded_properties
 * @param[out]  decoded_properties
 *
 * @return     lzma_ResultSuccess on success
 *             lzma_ResultDataError on invalid LZMA encoded properties
 */
static lzma_result DecodeProperties(UInt8 encoded_properties, CLzmaProperties* decoded_properties)
{
    // Check max value (pb=4, lp=4, lc=8)
    // properties = (pb * 5 + lp) * 9 + lc
    if (encoded_properties > ((4 * 5 + 4) * 9 + 8))
    {
        return lzma_ResultDataError;
    }

    // Decode properties from LZMA header, save in decoded_properties
    // Set decoded_properties->pb
    for (decoded_properties->pb = 0; encoded_properties >= (9 * 5); decoded_properties->pb++, encoded_properties -= (9 * 5));
    // Set decoded_properties->lp
    for (decoded_properties->lp = 0; encoded_properties >= 9; decoded_properties->lp++, encoded_properties -= 9);
    // Set decoded_properties->lc
    decoded_properties->lc = encoded_properties;

    // Validate individual max values
    if (decoded_properties->lc > 8)
    {
        return lzma_ResultDataError;
    }

    if (decoded_properties->lp > 4)
    {
        return lzma_ResultDataError;
    }

    if (decoded_properties->pb > 4)
    {
        return lzma_ResultDataError;
    }

    return lzma_ResultSuccess;
}
/* </CodeGenerator Placeholder> StaticFunctionDefinitions */

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

lzma_result lzma_IsValidInput(const UInt8* inputBuffer, UInt32 inputBufferSize)
{
    /* <CodeGenerator Placeholder> lzma_IsValidHeader */
    lzma_result result;
    lzma_header_t*      compressedDataHeader;               /**< Pointer for easier data access */
    CLzmaProperties     properties;                         /**< Holds decompressed LZMA configuration properties*/

    // Use a lzma_header_t variable to access data from @p inputBuffer
    compressedDataHeader = (lzma_header_t*)inputBuffer;

    // Validate header is complete
    if (inputBufferSize < sizeof(lzma_header_t) - sizeof(compressedDataHeader->data))
    {
        return lzma_ResultDataError;
    }

     // Validate decompressed size is filled in
    if (compressedDataHeader->decompressedSize == 0 || compressedDataHeader->decompressedSize == 0xFFFFFFFF)
    {
        return lzma_ResultDataError;
    }

    // Validate decompressed size is page aligned
    if (compressedDataHeader->decompressedSize % FLASH_PAGE_SIZE != 0)
    {
        return lzma_ResultAlignmentError;
    }

    // Validate LZMA configuration properties
    result = DecodeProperties(compressedDataHeader->properties, &properties);
    if (result != lzma_ResultSuccess)
    {
        return result;
    }

    // Validate sufficient memory is provided to decompress the inputBuffer
    if (LZMA_WORKING_MEMORY_SIZE < LzmaGetNumProbs(&properties) * sizeof(UInt16))
    {
        return lzma_ResultInsufficientMemory;
    }

    return lzma_ResultSuccess;
    /* </CodeGenerator Placeholder> lzma_IsValidHeader */
}

lzma_result lzma_IsValidOutput(const UInt8* outputBuffer)
{
    // Validate output buffer is page aligned
    if ((UInt32)outputBuffer % FLASH_SECTOR_SIZE != 0)
    {
        return lzma_ResultAlignmentError;
    }

    return lzma_ResultSuccess;
}

UInt32 lzma_GetDecompressedSize(const UInt8* inputBuffer, UInt32 inputBufferSize)
{
    lzma_header_t*      compressedDataHeader;               /**< Pointer for easier data access */

    // Use a lzma_header_t variable to access data from @p inputBuffer
    compressedDataHeader = (lzma_header_t*)inputBuffer;

    // Validate header is complete
    if (inputBufferSize < 9)
    {
        return lzma_ResultDataError;
    }

    return compressedDataHeader->decompressedSize;
}

lzma_result lzma_Decode(const UInt8* inputBuffer, UInt32 inputBufferSize, UInt8* outputBuffer)
{
    /* </CodeGenerator Placeholder> lzma_Decode */
    volatile lzma_result result;                            /**< LZMA decode result - Make sure this value is pushed to the stack and not accessed via a register (as we manually recover registers at the end)**/
    lzma_header_t*      compressedDataHeader;               /**< Pointer for easier data access */
    CLzmaDecoderState   state;                              /**< Holds LZMA configuration and working buffer location*/
#ifdef __GNUC__
    // Define working area as static reserved space in RAM, avoid wError on stack-usage
    static UInt16       probs[LZMA_WORKING_MEMORY_SIZE/2];  /**< LZMA working buffer*/
#else
    // Initialize on the stack for other compilers
    UInt16              probs[LZMA_WORKING_MEMORY_SIZE/2];  /**< LZMA working buffer*/
#endif
    UInt32              inProcessed  = 0;                   /**< Number of bytes processed from @p inputBuffer*/
    UInt32              outProcessed = 0;                   /**< Number of bytes written into @p outputBuffer - Can be referenced by register, as it should be properly recovered*/
    UInt32              backup_r12;                         /**< Local backup of a register, touched by lzmza and its lower layers */

    // Verify input params
    result = lzma_IsValidInput(inputBuffer, inputBufferSize);
    if (result != lzma_ResultSuccess)
    {
        return result;
    }

    // Verify output params
    result = lzma_IsValidOutput(outputBuffer);
    if (result != lzma_ResultSuccess)
    {
        return result;
    }

    // Get LZMA configuration
    // from data header of the compressed data
    compressedDataHeader = (lzma_header_t*)inputBuffer;
    result = DecodeProperties(compressedDataHeader->properties, &state.Properties);
    if (result != lzma_ResultSuccess)
    {
        return result;
    }
    // Provide working memory buffer for performing the decompression
    state.Probs = (UInt16 *) probs;
   
    /* Reset the lzma_gpHal_Flash module, which is called directly from lzma_decode */
    lzma_gpHal_Flash_ResetStatus();

    // Back up register r12 (not restored by lzma_decode)
    asm (   "MOVS      %0, R12"  // Copy value of R12 to wildcard %0
         : "=r"(backup_r12)      // [output operands] Fill in wildcard %0 with the register appointed to "backup_r12"
    );

    // Decode inputBuffer + write page per page into outputBuffer
    // Call the assembly lzma implementation
    result = lzma_decode(&state, compressedDataHeader->data, inputBufferSize - (sizeof(lzma_header_t) - sizeof(compressedDataHeader->data)), (UInt32*)&inProcessed, (void*)outputBuffer, compressedDataHeader->decompressedSize, (UInt32*)&outProcessed);

    /* Restore lzma_decode caller registers*/
    // NOTE: According to lzma_decode function documentation,
    //       caller registers start at SP + 4*13 instead of 4*10
#ifdef __GNUC__
    asm volatile(   "SUB SP,   SP, #4*10  \n" 
                    "pop      {r8-r11}    \n" 
                    "ADD SP,   SP, #4     \n" 
                    "pop      {r4-r7}     \n" 
                    "ADD SP,   SP, #4     \n" 
     );
#else
    asm volatile(   "SUB SP,   SP, #4*10  \n" \
                    "pop      {r8-r11}    \n" \
                    "ADD SP,   SP, #4     \n" \
                    "pop      {r4-r7}     \n" \
                    "ADD SP,   SP, #4     \n" \
     );
#endif

    // Restore register r12
    asm (   "MOVS      r12, %0"
         :                      // [output operands] None
         :"r"(backup_r12)       // [input operands] Fill in wildcard %0 with the register appointed to "backup_r12
    );

    // Validate all data is written
    if (outProcessed != compressedDataHeader->decompressedSize)
    {
        return lzma_ResultIncomplete;
    }

    // Validate all data is processed
    if (inProcessed != (inputBufferSize - (sizeof(lzma_header_t) - sizeof(compressedDataHeader->data))))
    {
        return lzma_ResultIncomplete;
    }

    if (lzma_gpHal_Flash_GetStatus() != gpHal_FlashError_Success)
    {
        return lzma_ResultDataError;
    }

    return result;
    /* </CodeGenerator Placeholder> lzma_Decode */
}
