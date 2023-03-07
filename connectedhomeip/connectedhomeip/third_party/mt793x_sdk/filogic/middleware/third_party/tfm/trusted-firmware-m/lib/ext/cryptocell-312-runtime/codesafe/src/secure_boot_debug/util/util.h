/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef UTIL_H
#define UTIL_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/* invers the bytes on a word- used for output from HASH */
#ifdef BIG__ENDIAN
#define UTIL_INVERSE_UINT32_BYTES( val )    (val)
#else
#define UTIL_INVERSE_UINT32_BYTES( val ) \
   ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24) )
#endif

/* invers the bytes on a word - used for input data for HASH */
#ifdef BIG__ENDIAN
#define UTIL_REVERT_UINT32_BYTES( val ) \
   ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24) )
#else
#define UTIL_REVERT_UINT32_BYTES( val ) (val)
#endif

/* the minimum and maximum macros */
#undef min
#define min( a , b ) ( ( (a) < (b) ) ? (a) : (b) )

#undef max
#define max( a , b ) ( ( (a) > (b) ) ? (a) : (b) )

/* MACRO to count one bits */
#define COUNT_ONE_BITS(number, BitCount) \
do \
{ \
      uint32_t tmp_num = number; \
      BitCount = 0; \
      while (tmp_num)   \
      {         \
        tmp_num = tmp_num & (tmp_num - 1); \
        BitCount = BitCount + 1; \
      }         \
} while (0)

#define CONVERT_BYTE_ARR_TO_WORD(inPtr, outWord, numOfBytes) \
do{\
    uint8_t index;\
    outWord = 0;\
    for (index = 0; index < numOfBytes; index ++){\
        outWord |= (*(inPtr + index)<<8*index);\
    }\
} while(0)



/************************ Enums ********************************/

/************************ Typedefs  *****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions **********************/

/* ------------------------------------------------------------
 **
 * @brief This function executes a reversed words copy on a specified buffer.
 *
 *        on a 6 words byffer:
 *
 *        buff[5] <---> buff[0]
 *        buff[4] <---> buff[1]
 *        buff[3] <---> buff[2]
 *
 * @param[in] dst_ptr - The counter buffer.
 * @param[in] size    - The counter size in words.
 *
 */
 void UTIL_ReverseWordsBuff( uint32_t *pBuff , uint32_t sizeWords );


/* ------------------------------------------------------------
 **
 * @brief This function executes a reversed byte copy on a specified buffer.
 *
 *        on a 6 byte byffer:
 *
 *        buff[5] <---> buff[0]
 *        buff[4] <---> buff[1]
 *        buff[3] <---> buff[2]
 *
 * @param[in] pBuff - The buffer.
 * @param[in] size - The size in bytes.
 *
 */
 void UTIL_ReverseBuff( uint8_t *pBuff , uint32_t size );

/* ------------------------------------------------------------
 **
 * @brief This function executes a memory copy between 2 buffers.
 *
 * @param[in] pDst - The first counter buffer.
 * @param[in] pSrc - The second counter buffer.
 * @param[in] size - The counter size in bytes.
 *
 */
 void UTIL_MemCopy( uint8_t *pDst, const uint8_t *pSrc, uint32_t size );

/* ------------------------------------------------------------
 **
 * @brief This function executes a memory set operation on a buffer.
 *
 * @param[in] pBuff - the buffer.
 * @param[in] val   - The value to set the buffer.
 * @param[in] size  - the buffers size.
 *
 */
 void UTIL_MemSet( uint8_t *pBuff, uint8_t val, uint32_t size );

/* ------------------------------------------------------------
 **
 * @brief This function executes secure memory comparing of two buffers.
 *
 * @param[in] pBuff1 - The first counter buffer.
 * @param[in] pBuff2 - The second counter buffer.
 * @param[in] size   - the counters size in bytes.
 *
 */
 uint32_t UTIL_MemCmp( uint8_t *pBuff1, uint8_t *pBuff2, uint32_t size );

/* ------------------------------------------------------------
 **
 * @brief This function executes a reverse bytes copying from one buffer to another buffer.
 *
 * @param[in] pDst  - The pointer to destination buffer.
 * @param[in] pSrc  - The pointer to source buffer.
 * @param[in] size  - The size in bytes.
 *
 */
 void UTIL_ReverseMemCopy( uint8_t *pDst, uint8_t *pSrc, uint32_t size );


#ifdef __cplusplus
}
#endif

#endif



