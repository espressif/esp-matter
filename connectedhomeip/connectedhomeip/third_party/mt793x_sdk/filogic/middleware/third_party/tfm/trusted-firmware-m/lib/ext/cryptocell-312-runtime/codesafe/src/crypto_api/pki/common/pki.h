/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef PKA_EXPORT_H
#define PKA_EXPORT_H

#include "cc_pal_types.h"
#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* the macro gets two bits [i+1,i] LE words array */
#define PKI_GET_TWO_BITS_FROM_WORDS_ARRAY(pArr, i) \
   ((pArr[(i)>>5] >> ((i)&31)) & 3)
/* the macro gets bit[i] from LE words array */
#define PKI_GET_BIT_FROM_WORDS_ARRAY(pArr, i) \
   ((pArr[(i)>>5] >> ((i)&31)) & 1)


bool PkiIsModSquareRootExists(void);

void PkiClearAllPka(void);

void PkiConditionalSecureSwapUint32(uint32_t *x,
                    uint32_t *y,
                    uint32_t swp);

CCError_t  PkiCalcNp(uint32_t *pNp,
            uint32_t *pN,
            uint32_t  sizeNbits);


CCError_t  PkiLongNumDiv(uint32_t *pNumA,
               uint32_t numASizeInWords,
               uint32_t *pNumB,
               uint32_t numBSizeInWords,
               uint32_t *pModRes,
               uint32_t *pDivRes);


CCError_t PkiLongNumMul(uint32_t *pNumA ,
              uint32_t  ASizeInBits,
              uint32_t *pNumB ,
              uint32_t *pRes);

/*!< get next two bits of scalar*/
uint32_t PkiGetNextTwoMsBits(uint32_t *pScalar, uint32_t *pWord, int32_t i);

/*!< the function checks is array equal to 0  *
*    if(arr == 0) return 0, else 1.           */
bool PkiIsUint8ArrayEqualTo0(const uint8_t *arr, size_t size);

/*!< the function compares equality of two buffers of same size:
     if they are equal - return 1, else 0. */
bool PkiAreBuffersEqual(const void *buff1, const void *buff2, size_t sizeInBytes);

/************************************************************************/
/**
 * @brief The function copies uint8 big endianness data into PKA register.
 *
 *  Notes: Assumed all parameters are checked before calling this function.
 *         PKA registers used: dstReg.
 *         The size of pTemp buffer is at least PKA register size appropriate to
 *         register size lenID and dataSizeBytes <= temp buffer size (in bytes).
 *
 * @return  no return value
 */
void PkaCopyBe8DataIntoPkaReg(uint32_t  dstReg,       /* [out] virtual pointer to destination PKA register. */
                 uint32_t lenId,          /* [in] PKA register length ID.*/
                 const uint8_t *pSrc,     /* [in] pointer to source buffer. */
                 uint32_t dataSizeBytes,  /* [in] size of the data in bytes */
                 uint32_t *pTemp);        /* [in] pointer to the temp buffer of size >= register size. */

/************************************************************************/
/**
 * @brief The function copies data from PKA register into uint8 buffer in big endianness order.
 *
 *  Notes: Assumed all parameters are checked before calling this function.
 *         PKA registers used: srcReg.
 *         The size (in 32-bit words) of pTemp buffer is at least the data size,
 *         in words (rounded up).
 *
 * @return  no return value
 */
void PkaCopyDataFromPkaRegToBe8Buff(
            uint8_t  *pDst,      /* [out] pointer to the destination buffer of size >= sizeBytes. */
            uint32_t  srcReg,    /* [in] virtual pointer to source PKA register. */
            uint32_t  sizeBytes, /* [in] size of the data in PKA register in bytes */
            uint32_t  *pTemp);   /* [in] pointer to the temp buffer of size (in bytes) >= sizeBytes. */


/***********     PkiExecExpBe  function      **********************/
/**
 * @brief Executes the modular exponentiation using PKA.
 *
 *  The function input/output arrays are in big endianness order of bytes.
 *
 *  Notes: Assumed all parameters are checked before calling this function.
 *         PKA registers used: r0,r1,r2,r3,r4, r30,r31.
 *         The size of the pOut and pTemp buffers is at least the modulus size.
 *
 * @return  no return value
 */
CCError_t  PkiExecModExpBe(
                uint8_t *pOut,        /* [out] pointer to the exponentiation result with size,
                                               equaled to modulus size in words (including leading zeros. */
                uint8_t *pIn,         /* [in]  pointer to the input data. */
                uint32_t inSizeBytes, /* [in]  input data size in words */
                uint8_t *pMod,        /* [in]  pointer to the modulus. */
                uint32_t modSizeBits, /* [in]  modulus size in bits */
                uint8_t *pExp,        /* [in]  pointer to the exponent buffer. */
                uint32_t expSizeBytes,/* [in]  exponent size in words. */
                uint32_t *pTemp);     /* [in]  pointer to the temp buffer of size >= modulus size. */


/***********     PkiExecExpLeW  function      **********************/
/**
 * @brief Executes modular exponentiation using PKA for LE input parameters.
 *
 *  The function input/output arrays are given as little endianness words arrays.
 *
 *  Notes: Assumed all parameters are checked before calling this function.
 *         All sizes aligned to 32-bit words, leading zeros are present if exist.
 *         PKA registers used: r0,r1,r2,r3,r4, r30,r31.
 *
 * @return  no return value
 */
CCError_t  PkiExecModExpLeW(
                uint32_t *pOut,        /* [out] pointer to the exponentiation result with size, equalled
                                                to modulus size in words (including leading zeros). */
                uint32_t *pIn,         /* [in]  pointer to the input data. */
                uint32_t inSizeWords,  /* [in]  input data size in words */
                uint32_t *pMod,        /* [in]  pointer to the modulus. */
                uint32_t modSizeBits,  /* [in]  modulus size in bits */
                uint32_t *pExp,        /* [in]  pointer to the exponent buffer. */
                uint32_t expSizeWords);/* [in]  exponent size: should be <= modulus size (in words). */


#ifdef __cplusplus
}
#endif

#endif
