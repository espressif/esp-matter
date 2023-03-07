/*
 * Copyright (c) 2019-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <ti/drivers/dpl/DebugP.h>

#include <ti/drivers/cryptoutils/utils/CryptoUtils.h>

#if defined(__GNUC__) || defined(__clang__)
    #define CRYPTOUTILS_NOINLINE __attribute__((noinline))
#else
    #define CRYPTOUTILS_NOINLINE
#endif


#define CryptoUtils_LIMIT_MASK (0xFFFFFFFEu)

/*
 * These constants take values at the very top of the memory map where it is unreasonable
 * for an application to have stored a different number value.
 */
#define CryptoUtils_LIMIT_ZERO 0xFFFFFFFEu
#define CryptoUtils_LIMIT_ONE  0xFFFFFFFFu

const uint8_t * CryptoUtils_limitZero = (uint8_t *) CryptoUtils_LIMIT_ZERO;
const uint8_t * CryptoUtils_limitOne  = (uint8_t *) CryptoUtils_LIMIT_ONE;

/*
 *  ======== CryptoUtils_buffersMatch ========
 */
#if defined(__IAR_SYSTEMS_ICC__)
#pragma inline=never
#elif defined(__TI_COMPILER_VERSION__) && !defined(__cplusplus)
#pragma FUNC_CANNOT_INLINE (CryptoUtils_buffersMatch)
#elif defined(__TI_COMPILER_VERSION__)
#pragma FUNC_CANNOT_INLINE
#endif
CRYPTOUTILS_NOINLINE bool CryptoUtils_buffersMatch(const volatile void *volatile buffer0,
                                                   const volatile void *volatile buffer1,
                                                   size_t bufferByteLength) {
    volatile uint8_t tempResult = 0;
    uint8_t byte0;
    uint8_t byte1;
    size_t i;

    /* XOR each byte of the buffer together and OR the results.
     * If the OR'd result is non-zero, the buffers do not match.
     * There is no branch based on the content of the buffers here to avoid
     * timing attacks.
     */
    for (i = 0; i < bufferByteLength; i++) {
        byte0 = ((uint8_t *)buffer0)[i];
        byte1 = ((uint8_t *)buffer1)[i];

        tempResult |= byte0 ^ byte1;
    }

    return tempResult == 0;
}

/*
 *  ======== CryptoUtils_buffersMatchWordAligned ========
 */
#if defined(__IAR_SYSTEMS_ICC__)
#pragma inline=never
#elif defined(__TI_COMPILER_VERSION__) && !defined(__cplusplus)
#pragma FUNC_CANNOT_INLINE (CryptoUtils_buffersMatchWordAligned)
#elif defined(__TI_COMPILER_VERSION__)
#pragma FUNC_CANNOT_INLINE
#endif
CRYPTOUTILS_NOINLINE bool CryptoUtils_buffersMatchWordAligned(const volatile uint32_t *volatile buffer0,
                                                              const volatile uint32_t *volatile buffer1,
                                                              size_t bufferByteLength) {
    volatile uint32_t tempResult = 0;
    uint32_t word0;
    uint32_t word1;
    size_t i;

    /* We could skip the branch and just set tempResult equal to the
     * statement below for the same effect but this is more explicit.
     */
    if (bufferByteLength % sizeof(uint32_t) != 0) {
        return false;
    }

    /* XOR each 32-bit word of the buffer together and OR the results.
     * If the OR'd result is non-zero, the buffers do not match.
     * There is no branch based on the content of the buffers here to avoid
     * timing attacks.
     */
    for (i = 0; i < bufferByteLength / sizeof(uint32_t); i++) {
        word0 = buffer0[i];
        word1 = buffer1[i];

        tempResult |= word0 ^ word1;
    }

    return tempResult == 0;
}

/*
 *  ======== CryptoUtils_reverseBufferBytewise ========
 */
void CryptoUtils_reverseBufferBytewise(void * buffer, size_t bufferByteLength) {
    uint8_t *bufferLow = buffer;
    uint8_t *bufferHigh = bufferLow + bufferByteLength - 1;
    uint8_t tmp;

    while (bufferLow < bufferHigh) {
        tmp = *bufferLow;
        *bufferLow = *bufferHigh;
        *bufferHigh = tmp;
        bufferLow++;
        bufferHigh--;
    }
}

/*
 *  ======== CryptoUtils_isBufferAllZeros ========
 */
bool CryptoUtils_isBufferAllZeros(const void *buffer, size_t bufferByteLength) {
    uint32_t i;
    uint8_t bufferBits = 0;

    for (i = 0; i < bufferByteLength; i++) {
        bufferBits |= ((uint8_t *)buffer)[i];
    }

    return bufferBits == 0;
}

/*
 *  ======== CryptoUtils_memset ========
 */
void CryptoUtils_memset(void *dest, size_t destSize, uint8_t val, size_t count) {
    DebugP_assert(dest);
    DebugP_assert(count <= destSize);

    volatile uint8_t * volatile p = (volatile uint8_t *)dest;

    while (destSize-- && count--) {
        *p++ = val;
    }
}

/*
 *  ======== CryptoUtils_copyPad ========
 */
void CryptoUtils_copyPad(const void *source,
                         uint32_t *destination,
                         size_t sourceLength) {
    uint32_t i;
    uint8_t remainder;
    uint32_t temp;
    uint8_t *tempBytePointer;
    const uint8_t *sourceBytePointer;

    remainder = sourceLength % sizeof(uint32_t);
    temp = 0;
    tempBytePointer = (uint8_t *)&temp;
    sourceBytePointer = (uint8_t *)source;

    /* Copy source to destination starting at the end of source and the
     * beginning of destination.
     * We assemble each word in normal order and write one word at a
     * time since the PKA_RAM requires word-aligned reads and writes.
     */

    for (i = 0; i < sourceLength / sizeof(uint32_t); i++) {
            uint32_t sourceOffset = sizeof(uint32_t) * i;

            tempBytePointer[0] = sourceBytePointer[sourceOffset + 0];
            tempBytePointer[1] = sourceBytePointer[sourceOffset + 1];
            tempBytePointer[2] = sourceBytePointer[sourceOffset + 2];
            tempBytePointer[3] = sourceBytePointer[sourceOffset + 3];

            *(destination + i) = temp;
    }

    /* Reset to 0 so we do not have to zero-out individual bytes */
    temp = 0;

    /* If sourceLength is not a word-multiple, we need to copy over the
     * remaining bytes and zero pad the word we are writing to PKA_RAM.
     */
    if (remainder == 1) {

        tempBytePointer[0] = sourceBytePointer[0];

        /* i is reused from the loop above. This write zero-pads the
         * destination buffer to word-length.
         */
        *(destination + i) = temp;
    }
    else if (remainder == 2) {

        tempBytePointer[0] = sourceBytePointer[0];
        tempBytePointer[1] = sourceBytePointer[1];

       *(destination + i) = temp;
    }
    else if (remainder == 3) {

        tempBytePointer[0] = sourceBytePointer[0];
        tempBytePointer[1] = sourceBytePointer[1];
        tempBytePointer[2] = sourceBytePointer[2];

        *(destination + i) = temp;
    }

}

/*
 *  ======== CryptoUtils_reverseCopyPad ========
 */
void CryptoUtils_reverseCopyPad(const void *source,
                                uint32_t *destination,
                                size_t sourceLength) {
    uint32_t i;
    uint8_t remainder;
    uint32_t temp;
    uint8_t *tempBytePointer;
    const uint8_t *sourceBytePointer;

    remainder = sourceLength % sizeof(uint32_t);
    temp = 0;
    tempBytePointer = (uint8_t *)&temp;
    sourceBytePointer = (uint8_t *)source;

    /* Copy source to destination starting at the end of source and the
     * beginning of destination.
     * We assemble each word in byte-reversed order and write one word at a
     * time since the PKA_RAM requires word-aligned reads and writes.
     */

    for (i = 0; i < sourceLength / sizeof(uint32_t); i++) {
            uint32_t sourceOffset = sourceLength - 1 - sizeof(uint32_t) * i;

            tempBytePointer[3] = sourceBytePointer[sourceOffset - 3];
            tempBytePointer[2] = sourceBytePointer[sourceOffset - 2];
            tempBytePointer[1] = sourceBytePointer[sourceOffset - 1];
            tempBytePointer[0] = sourceBytePointer[sourceOffset - 0];

            *(destination + i) = temp;
    }

    /* Reset to 0 so we do not have to zero-out individual bytes */
    temp = 0;

    /* If sourceLength is not a word-multiple, we need to copy over the
     * remaining bytes and zero pad the word we are writing to PKA_RAM.
     */
    if (remainder == 1) {

        tempBytePointer[0] = sourceBytePointer[0];

        /* i is reused from the loop above. This write  zero-pads the
         * destination buffer to word-length.
         */
        *(destination + i) = temp;
    }
    else if (remainder == 2) {

        tempBytePointer[0] = sourceBytePointer[1];
        tempBytePointer[1] = sourceBytePointer[0];

       *(destination + i) = temp;
    }
    else if (remainder == 3) {

        tempBytePointer[0] = sourceBytePointer[2];
        tempBytePointer[1] = sourceBytePointer[1];
        tempBytePointer[2] = sourceBytePointer[0];

        *(destination + i) = temp;
    }
}

/*
 *  ======== CryptoUtils_reverseCopy ========
 */
void CryptoUtils_reverseCopy(const void *source,
                             void *destination,
                             size_t sourceLength)
{
    /*
     * If destination address is word-aligned and source length is a word-multiple,
     * use CryptoUtils_reverseCopyPad() for better efficiency.
     */
    if ((((uint32_t)destination | sourceLength) & 0x3) == 0)
    {
        CryptoUtils_reverseCopyPad(source,
                                   (uint32_t*)destination,
                                   sourceLength);
    }
    else
    {
        const uint8_t *sourceBytePtr = (const uint8_t *)source;
        uint8_t *dstBytePtr = (uint8_t *)destination + sourceLength - 1;

        /*
         * Copy source to destination starting at the end of source and the
         * beginning of destination.
         */
        while (sourceLength--)
        {
            *dstBytePtr-- = *sourceBytePtr++;
        }
    }
}

/* limitValue must be either CryptoUtils_LIMIT_ZERO or CryptoUtils_LIMIT_ONE */
static int16_t CryptoUtils_convertLimitValueToInt(const void *limitValue) {
    int16_t value = 0;

    if (limitValue == CryptoUtils_limitOne) {
        value = 1;
    }

    return value;
}

/*
 * Returns number1[offset] - number2[offset].
 *
 * Can handle one of number1 or number2 (but not both) being one of the special limit values of
 * CryptoUtils_LIMIT_ZERO or CryptoUtils_LIMIT_ONE.
 *
 * All pointer parameters must be non-NULL.
 */
static int16_t CryptoUtils_diffAtOffset(const uint8_t number1[], const uint8_t number2[],
                                        size_t offset, size_t lsbOffset) {

    int16_t diff;

    /* Look at number2 first, as it will be more common for number2 to be one of the limit values. */
    if (number2 == CryptoUtils_limitZero) {
        diff = (int16_t)number1[offset];
    }
    else if (number2 == CryptoUtils_limitOne) {
        if (offset == lsbOffset) {
            diff = (int16_t)number1[offset] - 1;
        }
        else {
            diff = (int16_t)number1[offset];
        }
    }
    else if (number1 == CryptoUtils_limitZero) {
        diff = 0 - (int16_t)number1[offset];
    }
    else if (number1 == CryptoUtils_limitOne){
        if (offset == lsbOffset) {
            diff = 1 - (int16_t)number1[offset];
        }
        else {
            diff = 0 - (int16_t)number1[offset];
        }
    }
    else {
        diff = (int16_t)number1[offset] - (int16_t)number2[offset];
    }

    return diff;
}

/* Uses a timing constant algorithm to return 0 if value is 0 and return 1 otherwise. */
static uint16_t CryptoUtils_valueNonZeroTimingConstantCheck(int16_t value) {
    uint16_t valueNonZero;

    /* Mask and shift bits such that if any bit in value is '1' then the
       lsb of valueNonZero is 1 and otherwise valueNonZero is 0. */
    valueNonZero = (((uint16_t)value & 0xFF00u) >> 8u) | ((uint8_t)value & 0xFFu);
    valueNonZero = ((valueNonZero & 0xF0u) >> 4u) | (valueNonZero & 0x0Fu);
    valueNonZero = ((valueNonZero & 0x0Cu) >> 2u) | (valueNonZero & 0x03u);
    valueNonZero = ((valueNonZero & 0x02u) >> 1u) | (valueNonZero & 0x01u);

    return valueNonZero;
}

/*
 * Returns sign of number1 - number2:
 * negative value if number2 is larger, positive value if number1 is larger and zero if numbers are equal.
 *
 * Note that the magnitude of the return value has no meaning.
 *
 * The comparison is performed with a time-constant algorithm with respect to either of the number
 * arguments (number1, number2) when those inputs are not CryptoUtils_limitZero or CryptoUtils_limitOne.
 *
 * All pointer parameters must be non-NULL.
 */
static int16_t CryptoUtils_compareNumbers(const uint8_t number1[], const uint8_t number2[],
                                          size_t byteLength, CryptoUtils_Endianess endianess) {
    int16_t result = 0x0;
    int16_t diff;
    uint16_t diffNonZero;
    uint16_t diffResultMask;
    uint16_t resultUnknown = 0xFFFFu;
    uintptr_t number1Address;
    uintptr_t number2Address;
    size_t i;

    number1Address = (uintptr_t)number1;
    number2Address = (uintptr_t)number2;

    /*
     * Check if special RNG_limit values are being used for both values.
     * This is not expected, but is handled for completeness.
     */
    if (((number1Address & CryptoUtils_LIMIT_MASK) == CryptoUtils_LIMIT_MASK) &&
       ((number2Address & CryptoUtils_LIMIT_MASK) == CryptoUtils_LIMIT_MASK)) {

        result = CryptoUtils_convertLimitValueToInt(number1) - CryptoUtils_convertLimitValueToInt(number2);
    }
    else if (number1 != number2) {
        if (endianess == CryptoUtils_ENDIANESS_BIG) {
            i = 0u;
            while (i < byteLength) {
                diff = CryptoUtils_diffAtOffset(number1, number2, i, byteLength-1);

                /* Update result only if result was not known and is thus currently set to 0. */
                result = (int16_t)((uint16_t) result | (resultUnknown & (uint16_t)diff));

                /*
                 * Determine if result is now known and update resultUnknown
                 */

                diffNonZero = CryptoUtils_valueNonZeroTimingConstantCheck(diff);

                /* Create mask where mask value is 0 if bytes were equal, otherwise mask is all 1s. */
                diffResultMask = diffNonZero - 1u;

                /* Set resultUnknown to 0 (indicating result is known) if bytes were not equal. */
                resultUnknown &= diffResultMask;

                i++;
            }
        }
        else {
            i = byteLength;
            while (i > 0u) {
                i--;

                diff = CryptoUtils_diffAtOffset(number1, number2, i, 0);

                /* Update result only if result was not known and is thus currently set to 0. */
                result = (int16_t)((uint16_t)result | (resultUnknown & (uint16_t)diff));

                /*
                 * Determine if result is now known and update resultUnknown
                 */
                diffNonZero = CryptoUtils_valueNonZeroTimingConstantCheck(diff);

                /* Create mask where mask value is 0 if bytes were equal, otherwise mask is all 1s. */
                diffResultMask = diffNonZero - 1u;

                /* Set resultUnknown to 0 (indicating result is known) if bytes were not equal. */
                resultUnknown &= diffResultMask;
            }
        }
    }
    else {
        result = 0;
    }

    return result;
}

/*
 *  ======== CryptoUtils_isNumberInRange ========
 */
bool CryptoUtils_isNumberInRange(const void *number, size_t bitLength, CryptoUtils_Endianess endianess,
                                 const void *lowerLimit, const void *upperLimit) {
    int16_t upperResult;
    int16_t lowerResult;
    bool inUpperLimit = true;
    bool inLowerLimit = true;
    size_t byteLength;

    byteLength = (bitLength + 7u) >> 3u;

    if (upperLimit != NULL) {
        upperResult = CryptoUtils_compareNumbers(number, upperLimit, byteLength, endianess);
        if (upperResult >= 0) {
            inUpperLimit = false;
        }
    }

    if (lowerLimit != NULL) {
        lowerResult = CryptoUtils_compareNumbers(number, lowerLimit, byteLength, endianess);
        if (lowerResult < 0) {
            inLowerLimit = false;
        }
    }

    return(inUpperLimit && inLowerLimit);
}
