/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



/************* Include Files ****************/

#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_common_error.h"
#include "cc_pal_mem.h"
#include "cc_bitops.h"

/************************ Defines ******************************/

/************************ Enums ********************************/

/************************ Typedefs *****************************/

/************************ Global Data **************************/

/************* Private function prototype **********************/

/************************ Public Functions *********************/

/***********************************************************************
 **
 * @brief This function executes a reverse bytes copying from one buffer to another buffer.
 *
 *        Overlapping of buffers is not allowed, excluding the case, when destination and source
 *        buffers are the same.
 *        Example of a 5 byte buffer:
 *
 *        dst_ptr[4] = src_ptr[0]
 *        dst_ptr[3] = src_ptr[1]
 *        dst_ptr[2] = src_ptr[2]
 *        dst_ptr[1] = src_ptr[3]
 *        dst_ptr[0] = src_ptr[4]
 *
 * @param[in] dst_ptr - The pointer to destination buffer.
 * @param[in] src_ptr - The pointer to source buffer.
 * @param[in] size    - The size in bytes.
 *
 */
CCError_t CC_CommonReverseMemcpy( uint8_t *dst_ptr , uint8_t *src_ptr , uint32_t size )
{
        /* FUNCTION DECLARATIONS */

        /* loop variable */
        uint32_t i;

        /* FUNCTION LOGIC */

        /* check overlapping */
        if ((dst_ptr > src_ptr && dst_ptr < (src_ptr + size)) ||
            (dst_ptr < src_ptr && (dst_ptr + size) >= src_ptr)) {
                return CC_COMMON_DATA_OUT_DATA_IN_OVERLAP_ERROR;
        }


        /* execute the reverse copy in case of different buffers */
        if (dst_ptr != src_ptr) {
                for (i = 0 ; i < size ; i++)
                        dst_ptr[i] = src_ptr[size - 1 - i];
        }

        /* execute the reverse copy in the same place */
        else {
                uint8_t  temp;

                for (i = 0; i < size / 2; i++) {
                        temp = src_ptr[i];
                        src_ptr[i] = src_ptr[size - 1 - i];
                        src_ptr[size - 1 - i] = temp;
                }
        }

        return CC_OK;

}/* END OF CC_CommonReverseMemcpy */


#ifndef DX_OEM_FW

/***********************************************************************/
/**
 * @brief This function converts in place words byffer to bytes buffer with
 *        reversed endianity of output array.
 *
 *        The function can convert:
 *           - big endian bytes array to words array with little endian order
 *             of words and backward.
 *
 *      Note:
 *      1. Endianness of each word in words buffer should be set allways
 *      according to processor used.
 *      2. Implementation is given for both big and little endianness of
 *      processor.
 *
 * @param[in]  buf_ptr - The 32-bits pointer to input/output buffer.
 * @param[in]  sizeWords - The size in words (sizeWords > 0).
 *
 * @return - no return value.
 */
void CC_CommonInPlaceConvertBytesWordsAndArrayEndianness(
                                                        uint32_t *buf_ptr,
                                                        uint32_t  sizeWords)
{
        /* FUNCTION DECLARATIONS */

        uint32_t i, tmp;

        /* FUNCTION logic */

#ifndef BIG__ENDIAN
        /* If sizeWords is odd revert middle word */
        if (sizeWords & 1UL) {
                buf_ptr[sizeWords/2] = CC_COMMON_REVERSE32(buf_ptr[sizeWords/2]);
        }
#endif

                /* Reverse order of words and order of bytes in each word.    *
                *  Note: Condition (sizeWords >= 2) inserted inside for() to  *
                *        prevent wrong false positive warnings.               *
                *                                                             */
                for (i = 0; ((i < sizeWords/2) && (sizeWords >= 2)); i++) {
#ifndef BIG__ENDIAN
                        tmp = CC_COMMON_REVERSE32(buf_ptr[i]);
                        buf_ptr[i] = CC_COMMON_REVERSE32(buf_ptr[sizeWords-i-1]);
#else
                        tmp = buf_ptr[i];
                        buf_ptr[i] = buf_ptr[sizeWords-i-1];
#endif
                        buf_ptr[sizeWords-i-1] = tmp;
                }

        return;

}/* End of CC_CommonInPlaceConvertBytesWordsAndArrayEndianness */


/***********************************************************************/
/**
* @brief This function converts big endianness bytes array to aligned words
*        array with words order according to little endian.
*
*            1. Assumed, that input bytes order is set according
*         to big endianness: MS Byte is most left, i.e. order is from
*         Msb to Lsb.
*            2. Output words array should set according to
*         little endianness words order: LSWord is most left, i.e. order
*         is from Lsw to Msw. Order bytes in each word - according to
*         processor endianness.
*            3. Owerlapping of buffers is not allowed, besides in
*         place operation and size aligned to full words.
*            4. Implementation is given for both big and little
*         endianness of processor.
*
* @param[out] out32_ptr - The 32-bits pointer to output buffer.
* @param[in] sizeOutBuffBytes - The size in bytes of output buffer, must be
*            aligned to 4 bytes and not less than inpSizeInBytes.
* @param[in] in8_ptr - The pointer to input buffer.
* @param[in] inpSizeInBytes - The size in bytes of input data, where
*                  0 < inpSizeInBytes < UINT32_MAX - CC_32BIT_WORD_SIZE.
*
* @return CCError_t - On success CC_OK is returned, on failure a
*                        value MODULE_* as defined in .
*/
CCError_t CC_CommonConvertMsbLsbBytesToLswMswWords(
                                                  uint32_t *out32_ptr,
                                                  uint32_t  sizeOutBuffBytes,
                                                  const uint8_t  *in8_ptr,
                                                  uint32_t  inpSizeInBytes)
{
        /* FUNCTION DECLARATIONS */

        /* the rounded up size of the input data in bytes */
        uint32_t roundedInpSizeInBytes;

        /* FUNCTION LOGIC */

        /* Check pointers and size */
        if ((in8_ptr == NULL) || (out32_ptr == NULL))
                return CC_COMMON_DATA_IN_POINTER_INVALID_ERROR;

        /*  Check the size and in place operation:       *
        *   the size must be > 0 and aligned to words    */
        if ((inpSizeInBytes == 0) || (inpSizeInBytes >= UINT32_MAX - CC_32BIT_WORD_SIZE) ||
            (sizeOutBuffBytes & 3) || (inpSizeInBytes > sizeOutBuffBytes)) {
                return CC_COMMON_DATA_SIZE_ILLEGAL;
        }

        /* Size of input data in words rounded up */
        roundedInpSizeInBytes = ROUNDUP_BYTES_TO_32BIT_WORD(inpSizeInBytes);

        if (roundedInpSizeInBytes > sizeOutBuffBytes)
                return CC_COMMON_OUTPUT_BUFF_SIZE_ILLEGAL;

        /*  If the conversion is not "in place" or data size not aligned to
            words, then copy the data into output buffer and zeroe leading bytes */
        if (((CCVirtAddr_t)out32_ptr != (CCVirtAddr_t)in8_ptr) || (inpSizeInBytes & 3)) {
                CC_PalMemMove((uint8_t*)out32_ptr + roundedInpSizeInBytes - inpSizeInBytes, in8_ptr, inpSizeInBytes);
                /* set leading zeros in output buffer */
                if (roundedInpSizeInBytes > inpSizeInBytes)
                        CC_PalMemSetZero((uint8_t*)out32_ptr, roundedInpSizeInBytes - inpSizeInBytes); /*leading zeros*/
        }

        /* set tailing zeros in output buffer */
        CC_PalMemSetZero((uint8_t*)out32_ptr + roundedInpSizeInBytes,
                         sizeOutBuffBytes - roundedInpSizeInBytes); /*tailing zeros*/

        /* Reverse words order and set endianness of each word */
        CC_CommonInPlaceConvertBytesWordsAndArrayEndianness(out32_ptr, CALC_32BIT_WORDS_FROM_BYTES(inpSizeInBytes));

        return CC_OK;
}


/***********************************************************************/
/**
* @brief This function converts LE 32bit-words array to BE bytes array.
*
*            1. Assumed, that output bytes order is according
*         to big endianness: MS Byte is most left, i.e. order is from
*         Msb to Lsb.
*            2. Input words array should be set according to
*         little endianness words order: LSWord is most left, i.e. order
*         is from Lsw to Msw. Bytes order in each word - according to
*         processor endianness.
*            3. Owerlapping of buffers is not allowed, besides in
*         place operation and size aligned to full words.
*            4. Implementation is given for both big and little
*         endianness of processor.
*
* @param[in] out32_ptr - The 32-bits pointer to output buffer.
* @param[in] sizeOutBuffBytes - The size in bytes of output buffer, must be
*       not less than inpSizeInBytes.
* @param[out] in8_ptr - The pointer to input buffer.
* @param[in] sizeInBytes - The size in bytes. The size must be not 0 and
*       aligned to 4 bytes word.
*
* @return CCError_t - On success CC_OK is returned, on failure a
*                        value MODULE_* as defined in .
*/
CCError_t CC_CommonConvertLswMswWordsToMsbLsbBytes(
                                                  uint8_t  *out8_ptr,
                                                  size_t   sizeOutBuffBytes,
                                                  uint32_t *in32_ptr,
                                                  uint32_t  sizeInBytes)
{
        /* FUNCTION DECLARATIONS */

        uint32_t sizeInWords;

        /* FUNCTION LOGIC */

        /* Check pointers and size */
        if ((in32_ptr == NULL) || (out8_ptr == NULL))
                return CC_COMMON_DATA_IN_POINTER_INVALID_ERROR;

        /* Size in words rounded up */
        sizeInWords = (sizeInBytes + 3) / 4;

        if ((sizeInBytes == 0) || (sizeOutBuffBytes < sizeInBytes))
                return CC_COMMON_DATA_SIZE_ILLEGAL;

        /* Check in place operation: the size must be aligned to word */
        if (((CCVirtAddr_t)in32_ptr == (CCVirtAddr_t)out8_ptr) && (sizeInBytes & 3UL))
                return CC_COMMON_DATA_SIZE_ILLEGAL;

        /* Reverse words order and bytes according to endianness of CPU */
        CC_CommonInPlaceConvertBytesWordsAndArrayEndianness(in32_ptr, sizeInWords);

        /* Copy output buffer */
        if ((CCVirtAddr_t)out8_ptr != (CCVirtAddr_t)in32_ptr) {

                /* Check overlapping */
                if (((CCVirtAddr_t)out8_ptr > (CCVirtAddr_t)in32_ptr && (CCVirtAddr_t)out8_ptr < ((CCVirtAddr_t)in32_ptr + sizeInBytes)) ||
                    ((CCVirtAddr_t)out8_ptr < (CCVirtAddr_t)in32_ptr && ((CCVirtAddr_t)out8_ptr + sizeInBytes) > (CCVirtAddr_t)in32_ptr))
                        return CC_COMMON_DATA_OUT_DATA_IN_OVERLAP_ERROR;

                CC_PalMemCopy(out8_ptr, (uint8_t *)in32_ptr + ((4 - (sizeInBytes & 3UL)) & 3UL), sizeInBytes);
                /* Revert the input buffer to previous state */
                CC_CommonInPlaceConvertBytesWordsAndArrayEndianness(in32_ptr, sizeInWords);
        }

        return CC_OK;
}


/***********************************************************************/
/**
 * @brief This function converts bytes array with little endian (LE) order of
 *        bytes to 32-bit words array with little endian order of words and bytes.
 *
 *   Assuming:  No owerlapping of buffers; in/out pointers and sizes are not equall to NULL.
 *              If is in-place conversion, then the size must be multiple of 4 bytes.
 * @param[out] out32Le - The 32-bits pointer to output buffer. The buffer size must be
 *                       not less, than input data size.
 * @param[in] in8Le - The pointer to input buffer.
 * @param[in] sizeInBytes - The size in bytes of input data(sizeBytes > 0).
 *
 * @return CCError_t - On success CC_OK is returned, on failure a
 *                        value MODULE_* as defined in .
 */
void CC_CommonConvertLsbMsbBytesToLswMswWords(
                                             uint32_t *out32Le,
                                             const uint8_t  *in8Le,
                                             size_t  sizeInBytes)
{

        uint32_t sizeInWords;

        /* Size in words rounded up */
        sizeInWords = (sizeInBytes + 3)/4;

        /* if not in place, then zero empty bytes of MS word and copy data */
        if ((uint8_t*)out32Le != in8Le) {
                out32Le[sizeInWords-1] = 0;
                CC_PalMemCopy((uint8_t*)out32Le, in8Le, sizeInBytes);
        }

#ifdef BIG__ENDIAN
        /* Reverse endianness of each word */
        {
                uint32_t i;
                for (i = 0; i < sizeInWords; i++) {
                        CC_COMMON_REVERSE32(out32Le[i]);
                }
        }
#endif
        return;
}

/***********************************************************************/
/**
* @brief This function converts 32-bit words array with little endian
*        order of words to bytes array with little endian (LE) order of bytes.
*
*    Assuming: no buffers overlapping, in/out pointers and sizes not equall to NULL,
               the buffer size must be not less, than input data size.
*
* @param[out] out8Le - The bytes pointer to output buffer.
* @param[in] in32Le - The pointer to input 32-bit words buffer.
* @param[in] sizeInWords - The size in words of input data (sizeWords >= 0).
*
* @return CCError_t - On success CC_OK is returned, on failure a
*                        value MODULE_* as defined in .
*/
void CC_CommonConvertLswMswWordsToLsbMsbBytes(
                                             uint8_t *out8Le,
                                             const uint32_t  *in32Le,
                                             size_t  sizeInWords)
{
        /* FUNCTION LOGIC */

#ifndef BIG__ENDIAN
        if (out8Le != (uint8_t*)in32Le) {
                CC_PalMemCopy(out8Le, (uint8_t*)in32Le, sizeInWords*sizeof(uint32_t));
        }
#else
        /* Reverse endianness of each word and copy it to output */
        size_t i;

        for (i = 0; i < sizeInWords; i++) {
                if (out8Le != (uint8_t*)in32Le) {
                        uint32_t tmp;
                        tmp = in32Le[i / sizeof(uint32_t)];
                        CC_COMMON_REVERSE32(tmp);
                        out8Le[i*sizeof(uint32_t) + 0] = tmp & 0xFF;
                        out8Le[i*sizeof(uint32_t) + 1] = (tmp >>  8) & 0xFF;
                        out8Le[i*sizeof(uint32_t) + 2] = (tmp >> 16) & 0xFF;
                        out8Le[i*sizeof(uint32_t) + 3] = (tmp >> 24) & 0xFF;
                } else {
                        CC_COMMON_REVERSE32(in32Le[i]);
                }
        }

#endif
        return;
}

#endif
