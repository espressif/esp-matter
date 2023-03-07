/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_mem.h"
#include "cc_pal_types.h"
#include "cc_pal_mutex.h"
#include "cc_hal_plat.h"
#include "cc_sram_map.h"
#include "dx_crys_kernel.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "pka_hw_defs.h"
#include "pki.h"
#include "ec_wrst.h"
#include "pka_hw_defs.h"
#include "pka.h"
#include "pka_error.h"
#ifdef DEBUG
        #include <assert.h>
#endif

extern const int8_t regTemps[PKA_MAX_COUNT_OF_PHYS_MEM_REGS];

#if defined PKA_DEBUG && defined DEBUG
uint32_t tempRes[PKA_MAX_REGISTER_SIZE_IN_32BIT_WORDS];
uint32_t tempRes1[PKA_MAX_REGISTER_SIZE_IN_32BIT_WORDS];
#endif


/***********    PkiCalcNp  function      **********************/
/**
 * The function uses physical data pointers to calculate and output
 * the Barrett tag Np.
 *
 *  For RSA it uses truncated sizes:
 *      Np = truncated(2^(3*A+3*X-1) / ceiling(n/(2^(N-2*A-2*X)));
 *  For ECC - full sizes of not truncated input arguments:
 *      Np = truncated(2^(N+A+X-1) / n);
 *
 * @author reuvenl (5/1/2014)
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t  PkiCalcNp(uint32_t *pNp, /*!< [out] The pointer to the Barrett tag Np buffer. If pNp = Null,
                            the function not outputs calculated Np. */
                        uint32_t *pN,       /*!< [out] The pointer to the modulus n. */
                        uint32_t  sizeNbits)    /*!< [in] The exact size of the modulus. */
{
        CCError_t err = 0;
        uint32_t  A = CC_PKA_WORD_SIZE_IN_BITS;
        uint32_t  X = PKA_EXTRA_BITS;
        uint32_t pkaReqRegs = 6;
        int32_t wN, wNp;

        /* usage of PKA registers */
        int8_t  rN = PKA_REG_N;
        int8_t  rNp = PKA_REG_NP;
        int8_t  rT2 = regTemps[2];
        int8_t  rT4 = regTemps[4];

        /* Calc. sizes of modulus in words and reminder in bits */
        wN = CALC_FULL_32BIT_WORDS(sizeNbits);
        wNp = CALC_FULL_32BIT_WORDS(A + X - 1);

        err = PkaInitAndMutexLock(sizeNbits, &pkaReqRegs);
        if (err != CC_SUCCESS) {
                CC_PalAbort("Fail to acquire mutex\n");
        }
        /* Set modulus in T1 */
        PkaCopyDataIntoPkaReg(rN/*dstReg*/, LEN_ID_MAX_BITS/*lenId*/, pN/*src_ptr*/, wN);
        err = PkaCalcNpIntoPkaReg(LEN_ID_N_BITS, sizeNbits, rN /* regN */, rNp /* regNp */, rT2, rT4);
        if (err != CC_SUCCESS) {
                goto End;
        }
        /* Output Np */
        PkaCopyDataFromPkaReg(pNp/*dst_ptr*/, wNp, rNp/*srcReg*/);
End:
        PkaFinishAndMutexUnlock(pkaReqRegs);


        return err;
}


/***********    PkiLongNumDiv   function      **********************/
/**
 * @brief  This function performs division of big numbers, passed by physical pointers,
 *              using the PKA.
 *              .
 *     Computes modRes = A mod B. divRes_ptr = floor(A/B)
 *     Lengths: A[ALen], B[BLen], modRes[BLen], divRes[ALen].
 *     Assumes:  c > 0.
 *
 *     PKA registers using: A=>r2, B=>r3, divRes=>r4, modRes=>r2 (r2 is rewritten by remainder).
 *
 * Author: R.Levin
 *
 * Last Revision: 1.00.00
 *
 * @return  CC_OK On success, otherwise indicates failure
 *
 * Update History:
 * Rev 1.00.00, Date 4 Feb. 2008,
 *
 */
CCError_t  PkiLongNumDiv(uint32_t *pNumA,        /*!< [in] The pointer to numerator A vector. */
                           uint32_t numASizeInWords,  /*!< [in] Length of numerator A in words.*/
                           uint32_t *pNumB,        /*!< [in] The pointer to divider B (modulus).*/
                           uint32_t numBSizeInWords,  /*!< [in] The size of B vector in words.*/
                           uint32_t *pModRes,   /*!< [out] The pointer to modulus result (reminder of division). */
                           uint32_t *pDivRes)   /*!< [out] The pointer to result of division.*/
{
        CCError_t error = CC_OK;
        uint32_t  opSizeWords;
        uint32_t status;
        uint32_t pkaReqRegs = 6;
    int8_t  rT2 = regTemps[2];
        int8_t  rT3 = regTemps[3];
        int8_t  rT4 = regTemps[4];


        opSizeWords = CC_MAX(numASizeInWords, numBSizeInWords);

        error = PkaInitAndMutexLock(CC_BITS_IN_32BIT_WORD*opSizeWords, &pkaReqRegs);
        if (error != CC_OK) {
                return error;
        }

        /* copying all needed data into PKA memory before starting PKA operations */
        /* A=>r2, B=>r3,                                                          */
        /* copy numerator into PKA register: A=>r2 */
        PkaCopyDataIntoPkaReg( rT2/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pNumA/*src_ptr*/, numASizeInWords);

        /* copy divisor into PKA register: B=>r3 */
        PkaCopyDataIntoPkaReg( rT3/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pNumB/*src_ptr*/, numBSizeInWords);

        /* check, that divisor is not null, else return error */
        PKA_ADD_IM( LEN_ID_N_PKA_REG_BITS/*LenID*/, rT4/*Res*/, rT3/*OpA*/, 0/*Imm OpB*/);
        PKA_GET_STATUS_ALU_OUT_ZERO(status);
        if (status == 1) {
                error = PKA_DIVIDER_IS_NULL_ERROR;
                goto End;
        }

        /* division in PKA: quotient: r4 = r2 / r3; remainder: r2 = r2 % r3        */
        PKA_DIV( LEN_ID_N_PKA_REG_BITS/*LenID*/, rT4/*Res*/, rT2/*OpA*/, rT3/*OpB*/);


        /*        output the results                                               */
        if (pDivRes != NULL) {
                PkaCopyDataFromPkaReg(pDivRes,  numASizeInWords, rT4/*srcReg*/);
        }

        if (pModRes != NULL) {
                PkaCopyDataFromPkaReg(pModRes,  numBSizeInWords, rT2/*srcReg*/);
        }

End:
        PkaFinishAndMutexUnlock(pkaReqRegs);

        return error;

}


/***********     PkiLongNumMul  function     **********************/
/**
 * @brief This function performs multiplication of big numbers, passed by physical
 *              pointers, using the PKA.
 *
 *        The RMul operation is : (A * B)
 *
 *        The function performs the following algorithm:
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t PkiLongNumMul(uint32_t *pNumA ,      /*!< [in] The pointer of A words array (LS word is left most). */
                          uint32_t  numASizeInBits, /*!< [in] The size of vectors in bits. */
                          uint32_t *pNumB ,      /*!< [in] The pointer of B words array (LS word is left most). */
                          uint32_t *pRes)    /*!< [out] The pointer to the result buffer. */
{
        CCError_t error = CC_OK;
        uint32_t  OpSizeInWords;
        uint32_t pkaReqRegs = 6;
        int8_t  rT2 = regTemps[2];
        int8_t  rT3 = regTemps[3];
        int8_t  rT4 = regTemps[4];

#ifdef LLF_PKI_PKA_DEBUG
        /* check the operands size, used for RSA only */
        if (2*numASizeInBits > (CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS+CC_PKA_WORD_SIZE_IN_BITS)) {
               return PKA_ILLEGAL_OPERAND_LEN_ERROR;
    }
#endif

        /* set operation size in words */
        if (CALC_FULL_32BIT_WORDS(2*numASizeInBits) < CALC_FULL_32BIT_WORDS(PKA_MIN_OPERATION_SIZE_BITS))
                OpSizeInWords = CALC_FULL_32BIT_WORDS(PKA_MIN_OPERATION_SIZE_BITS);

        else
                OpSizeInWords = CALC_FULL_32BIT_WORDS(2*numASizeInBits);


        error = PkaInitAndMutexLock(CC_BITS_IN_32BIT_WORD*OpSizeInWords, &pkaReqRegs);
        if (error != CC_OK) {
                return error;
        }

        /* copying all needed data into PKA memory before starting PKA operations */
        /* A=>r2, B=>r3,                                                          */
        /* copy A into PKA register: A=>r2 */
        PkaCopyDataIntoPkaReg(rT2/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pNumA/*src_ptr*/,
                               CALC_FULL_32BIT_WORDS(numASizeInBits));

        /* copy B into PKA register: B=>r2 */
        PkaCopyDataIntoPkaReg(rT3/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pNumB/*src_ptr*/,
                               CALC_FULL_32BIT_WORDS(numASizeInBits));


        /* multiply in PKA:  r4 = r2 * r3; */
        PKA_MUL_LOW(LEN_ID_N_PKA_REG_BITS/*lenId*/, rT4/*Res*/, rT2/*OpA*/, rT3/*OpB*/);

        /* output the results */
        PkaCopyDataFromPkaReg(pRes, OpSizeInWords, rT4/*srcReg*/ );

        PkaFinishAndMutexUnlock(pkaReqRegs);


        return error;
}


/***********     PkiConditionalSecureSwapUint32  function     **********************/
/**
 * @brief The function performs conditional swapping of two values in secure
 *  mode
 *
 *  if(swp == 1) {tmp = *x; *x = *y; *y = tmp;}
 *
 * @return None
 */
void PkiConditionalSecureSwapUint32(uint32_t *x,    /*!< [in/out] the pointer to x-variable. */
                    uint32_t *y,    /*!< [in/out] the pointer to y-variable. */
                    uint32_t swp)   /*!< [in] swapping condition [0,1]. */
{
        int32_t tmpX = *x;
        int32_t tmpY = *y;
        int32_t tmp  = tmpX ^ tmpY;

        swp = -swp;
        tmp &= swp;
        *x = tmpX ^ tmp;
        *y = tmpY ^ tmp;
}


/***********     PkiClearAllPka  function     **********************/
/**
 * @brief This function clears the PKA memory.
 *
 * @return  None
 */
void PkiClearAllPka(void)
{
        uint32_t pkaRegCount = PKA_MAX_COUNT_OF_PHYS_MEM_REGS;
        uint32_t regSizeInBits = ((CC_PKA_SRAM_SIZE_IN_KBYTES*CC_1K_SIZE_IN_BYTES*CC_BITS_IN_BYTE)/PKA_MAX_COUNT_OF_PHYS_MEM_REGS)-CC_PKA_WORD_SIZE_IN_BITS;

        if (PkaInitAndMutexLock(regSizeInBits, &pkaRegCount) != CC_SUCCESS) {
                return;
        }

        PkaFinishAndMutexUnlock(pkaRegCount);
        return;

}

/*!< get next two bits of scalar*/
uint32_t PkiGetNextTwoMsBits(uint32_t *pScalar, uint32_t *pWord, int32_t i)
{
        uint32_t twoBits = 0;
        //        twoBits = (*pWord >> (i & (CC_BITS_IN_32BIT_WORD - 1))) & 3;
        twoBits = (*pWord >> (CC_BITS_IN_32BIT_WORD - 2));
        /* get next bit of scalar */
        if ((i % CC_BITS_IN_32BIT_WORD) != 0) {
                *pWord <<= 2;
        } else {
                *pWord = pScalar[(i / CC_BITS_IN_32BIT_WORD)-1];
        }
        //        i--;

        return twoBits;
}


/*!< the function checks is array equal to 0: *
*    if arr == 0, then return 0, else 1.      */
bool PkiIsUint8ArrayEqualTo0(const uint8_t *arr, size_t size)
{
        uint32_t i;
        uint8_t  s = 0;
        for (i = 0; i < size; i++)
                s |= arr[i];
        /* if(arr == 0) return 1, else 0. */
        return !(bool)((0UL - s) >> 31);

}

/*!< the function compares equality of two buffers of same size:
     if they are equal - return 0, else 1. */
bool PkiAreBuffersEqual(const void *buff1, const void *buff2, size_t sizeInBytes)
{
        uint32_t i;
        uint8_t  s = 0;
        for (i = 0; i < sizeInBytes; i++)
                s |= (((uint8_t*)buff1)[i] ^ ((uint8_t*)buff2)[i]);
        /* if equalled return 0, else 1. */
        return !(bool)((0UL - s) >> 31);

}

/************************************************************************/
/**
 * @brief The function copies uint8 big endianness data into PKA register.
 *
 *  Notes: Assumed all parameters are checked before calling this function.
 *         PKA registers used: dstReg.
 *         The size of pTemp buffer is at least PKA register size (appropriate to
 *         register size lenID) and dataSizeBytes <= temp buffer size (in bytes).
 *
 * @return  no return value
 */
void PkaCopyBe8DataIntoPkaReg(uint32_t  dstReg,       /* [out] virtual pointer to destination PKA register. */
                 uint32_t lenId,          /* [in] PKA register length ID.*/
                 const uint8_t *pSrc,     /* [in] pointer to source buffer. */
                 uint32_t dataSizeBytes,  /* [in] size of the data in bytes */
                 uint32_t *pTemp)         /* [in] pointer to the temp buffer of
                                             size >= PKA SRAM register size. */
{
    uint32_t  regSizeBits, regSizeWords;
//  uint32_t currAddr, word, dataSizeWords;
//  int32_t i, j = 0;

    PKA_GET_REG_SIZE(regSizeBits, lenId);
    regSizeWords = CALC_FULL_32BIT_WORDS(regSizeBits); /*size in words*/

//  CC_CommonConvertMsbLsbBytesToLswMswWords(pTemp, regSizeWords*CC_32BIT_WORD_SIZE, pSrc, dataSizeBytes);
    CC_PalMemSetZero((uint8_t*)pTemp + dataSizeBytes, regSizeWords*CC_32BIT_WORD_SIZE - dataSizeBytes);
    CC_CommonReverseMemcpy((uint8_t*)pTemp, (uint8_t*)pSrc, dataSizeBytes);
//  dataSizeWords = CALC_32BIT_WORDS_FROM_BYTES(dataSizeBytes);
    PkaCopyDataIntoPkaReg(dstReg, lenId, pTemp, regSizeWords);

//  dataSizeWords = CALC_32BIT_WORDS_FROM_BYTES(dataSizeBytes);
//  PKA_GET_REG_ADDRESS(dstReg, currAddr);
//
//  if(i > 3) {
//      for(i = dataSizeBytes-1; i > 0; i -= 4, j++) {
//          word = (pSrc[i] << 24) | (pSrc[i-1] << 16) |
//                 (pSrc[i-2] << 8) | pSrc[i-3];
//          PKA_HW_LOAD_VALUE_TO_PKA_MEM(currAddr, word);
//          currAddr += CC_32BIT_WORD_SIZE;
//      }
    return;
}


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
            uint32_t  *pTemp)    /* [in] pointer to the temp buffer of size (in bytes) >= sizeBytes. */
{
    PkaCopyDataFromPkaReg(pTemp, CALC_32BIT_WORDS_FROM_BYTES(sizeBytes), srcReg);
    CC_CommonReverseMemcpy(pDst, (uint8_t*)pTemp, sizeBytes);

    return;
}



/***********     PkiExecExpBe  function      **********************/
/**
 * @brief Executes the modular exponentiation using PKA for BE input parameters.
 *
 *  The function input/output arrays are in big endianness order of bytes.
 *
 *  Notes: Assumed all parameters are checked before calling this function.
 *         All sizes aligned to 32-bit words, leading zeros are present if exist.
 *         PKA registers used: r0,r1,r2,r3,r4, r30,r31.
 *         The size of pTemp buffer should be at least modulus size plus 2*PKA_WORD_SIZE,
 *         where all sizes are rounded up to uint32 size.
 *
 * @return  no return value
 */
CCError_t  PkiExecModExpBe(
                uint8_t *pOut,        /* [out] pointer to the exponentiation result with size >= modulus size (in bytes),
                                               equaled to modulus size in bytes (including leading zeros. */
                uint8_t *pIn,         /* [in]  pointer to the input data. */
                uint32_t inSizeBytes, /* [in]  input data size in bytes */
                uint8_t *pMod,        /* [in]  pointer to the modulus. */
                uint32_t modSizeBits, /* [in]  modulus size in bits */
                uint8_t *pExp,        /* [in]  pointer to the exponent buffer. */
                uint32_t expSizeBytes,/* [in]  exponent size in bytes; should be <= modulus size (in bytes). */
                uint32_t *pTemp)      /* [in]  pointer to the temp buffer of size >= modulus size. */
{
    CCError_t err = CC_OK;
    uint32_t modSizeBytes;
    uint32_t pkaRegsCount = 7;
        /* define virtual registers pointers  */
    #define IN_REG  2
    #define EXP_REG 3
    #define OUT_REG 4


    err = PkaInitAndMutexLock(modSizeBits, &pkaRegsCount);
    if (err != CC_OK) {
        return err;
    }

    modSizeBytes = CALC_FULL_BYTES(modSizeBits);

    /*  copy modulus into PKA */
    PkaCopyBe8DataIntoPkaReg(PKA_REG_N/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pMod/*srcPtr*/,
                         modSizeBytes, pTemp);

    /* Calculate NP */
    err =  PkaCalcNpIntoPkaReg(LEN_ID_N_BITS,
            modSizeBits,  /* [in] The exact size of the modulus. */
            PKA_REG_N,    /* [in] PKA reg, holding the modulus n. */
            PKA_REG_NP,   /* [out PKA reg. - Np result. */
                    IN_REG, OUT_REG);  /* PKA regs, used as temporaries. */

    if (err != CC_OK) {
        goto End;
    }


        /*  copy the Data and Exponent into PKA registers                  */
    PkaCopyBe8DataIntoPkaReg(IN_REG/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pIn, inSizeBytes, pTemp);
    PkaCopyBe8DataIntoPkaReg(EXP_REG/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pExp, expSizeBytes, pTemp);

        /*  calculate the exponent result: OUT_REG */
    PKA_MOD_EXP(LEN_ID_N_BITS/*LenID*/, OUT_REG, IN_REG/*OpA*/, EXP_REG/*OpB*/);

    /* output result as big endiannness bytes array */
    PkaCopyDataFromPkaRegToBe8Buff(pOut, OUT_REG, modSizeBytes, pTemp);

End:
    /*  Finish PKA and copy result */
    PkaFinishAndMutexUnlock(pkaRegsCount);

        /* undef virtual registers pointers  */
    #undef IN_REG
    #undef EXP_REG
    #undef OUT_REG

    return err;

}

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
                uint32_t *pOut,         /* [out] pointer to the exponentiation result with size, equalled
                                                to modulus size in words (including leading zeros). */
                uint32_t *pIn,         /* [in]  pointer to the input data. */
                uint32_t inSizeWords,  /* [in]  input data size in words */
                uint32_t *pMod,        /* [in]  pointer to the modulus. */
                uint32_t modSizeBits,  /* [in]  modulus size in bits */
                uint32_t *pExp,        /* [in]  pointer to the exponent buffer. */
                uint32_t expSizeWords) /* [in]  exponent size: should be <= modulus size (in words). */
{
    CCError_t err = CC_OK;
    uint32_t modSizeWords;
    uint32_t pkaRegsCount = 7;
        /* define virtual registers pointers  */
    #define IN_REG  2
    #define EXP_REG 3
    #define OUT_REG 4


    err = PkaInitAndMutexLock(modSizeBits, &pkaRegsCount);
    if (err != CC_OK) {
        return err;
    }

    modSizeWords = CALC_FULL_32BIT_WORDS(modSizeBits);

    /*  copy modulus into PKA */
    PkaCopyDataIntoPkaReg(PKA_REG_N/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/,
                  pMod/*srcPtr*/, modSizeWords);

    /* Calculate NP */
    err =  PkaCalcNpIntoPkaReg(LEN_ID_N_BITS,
            modSizeBits,  /* [in] The exact size of the modulus. */
            PKA_REG_N,    /* [in] PKA reg, holding the modulus n. */
            PKA_REG_NP,   /* [out PKA reg. - Np result. */
            IN_REG, OUT_REG);  /* PKA regs, used as temporaries. */
    if(err) {
          goto End;
    }

    /*  copy the Data and Exponent into PKA registers  */
    PkaCopyDataIntoPkaReg(IN_REG/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pIn, inSizeWords);
    PkaCopyDataIntoPkaReg(EXP_REG/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pExp, expSizeWords);

        /*  calculate the exponent result: OUT_REG */
    PKA_MOD_EXP(LEN_ID_N_BITS/*LenID*/, OUT_REG, IN_REG/*OpA*/, EXP_REG/*OpB*/);

    /* output result as big endiannness bytes array */
    PkaCopyDataFromPkaReg(pOut, modSizeWords, OUT_REG);

End:
    /*  Finish PKA and copy result */
    PkaFinishAndMutexUnlock(pkaRegsCount);

        /* undef virtual registers pointers  */
    #undef IN_REG
    #undef EXP_REG
    #undef OUT_REG

    return err;

}



