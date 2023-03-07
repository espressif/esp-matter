/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_mem.h"
#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#include "cc_common_math.h"
#include "cc_ecpki_types.h"
#include "cc_ecpki_error.h"
#include "cc_ecpki_local.h"
#include "pka_hw_defs.h"
#include "pki.h"
#include "pka.h"
#include "pka_error.h"
#include "ec_wrst.h"
#include "ec_wrst_error.h"
#include "pka_ec_wrst.h"
#include "pka_ec_wrst_glob_regs.h"


//! RL Temporary for ECDSA Verify testing
#include "pki_dbg.h"

/* canceling the lint warning:
   Info 717: do ... while(0) */
/*lint --e{717} */

/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/* canceling the lint warning:
Info 716: while(1) ... */
/*lint --e{716} */

extern const int8_t regTemps[PKA_MAX_COUNT_OF_PHYS_MEM_REGS];


/***********    EcWrstDivideVectorBy2   function      **********************/
/**
 * @brief Divides a vector by 2 - in a secured way
 *
 *        The LSB of the vector is stored in the first cell in the array.
 *
 *        for example:
 *
 *        a vector of 128 bit : the value is :
 *
 *        word[3] << 96 | word[2] << 64 ............ word[1] << 32 | word[0]
 *
 * @return - no return value.
 */

static void EcWrstDivideVectorBy2(uint32_t *pVecBuff, /*!< [in] Vector buffer. */
    uint32_t SizeInWords)/*!< [in] Size of vecrot in Words. */
{
        uint32_t i;
        uint32_t Temp;

        /* for loop for dividing the vectors arrays by 2 */
        for (i=0;i < (SizeInWords)-1 ;i++) {
                pVecBuff[i]=pVecBuff[i] >> 1;
                Temp=pVecBuff[i+1]&1UL;
                pVecBuff[i]=pVecBuff[i] | Temp<<(32-1);
        }

        /* dividing the MS word */
        pVecBuff[SizeInWords-1]=pVecBuff[SizeInWords-1]>>1;

        return;

}


/***********    PkaScalarMultAff   function      **********************/
/**
 * @brief Performs EC scalar multiplication p = k*p, not SCA-resistant
 *
 *  Implemented the algorithm, enhanced by A.Klimov
 *
 * @author reuvenl (03/19/2015)
 *
 * @return  - No return value.
 */
/* static */ void PkaScalarMultAff(
              const uint32_t xr, const uint32_t yr, /*!< [in/out] Virtual registers containing coordinates of result EC point. */
              const char *k,  /*!< [in] Virtual registers containing scalar. */
              const uint32_t xp, const uint32_t yp) /*!< [in] Virtual registers containing coordinates of input EC point. */
{
        uint8_t tp = regTemps[14];
        uint8_t zr = regTemps[15];
        uint8_t tr = regTemps[16];

        /* calculate auxiliary values */
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_N4 , ECC_REG_N,   ECC_REG_N  );
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_N4 , ECC_REG_N4, ECC_REG_N4);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_N8 , ECC_REG_N4, ECC_REG_N4);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_N12, ECC_REG_N8, ECC_REG_N4);

        PKA_SUB(LEN_ID_N_PKA_REG_BITS, tp, ECC_REG_N4, yp); // ry of -p
        //! RL may be changed to return error
        PKA_ASSERT(*k == '+', "*k == '+'\n");

        PKA_COPY(LEN_ID_N_PKA_REG_BITS, xr, xp);
        PKA_COPY(LEN_ID_N_PKA_REG_BITS, yr, yp); // r = p
        PKA_SET_VAL(zr, 1);
        PKA_COPY(LEN_ID_N_PKA_REG_BITS, tr, ECC_REG_EC_A);

        while (*++k) {
                if (*k == '0') {
                        PkaDoubleMdf2Mdf(xr,yr,zr,tr, xr,yr,zr,tr);     // *k = '0'
                } else {
                        PkaDoubleMdf2Jcb(xr,yr,zr, xr,yr,zr,tr);
                        if (*k == '+') {
                                PkaAddJcbAfn2Mdf(xr,yr,zr,tr, xr,yr,zr, xp,yp); // *k = '+'
                        } else {
                                PkaAddJcbAfn2Mdf(xr,yr,zr,tr, xr,yr,zr, xp,tp); // *k = '-'
                        }
                }
        }

        /* convert to affine */
        PkaJcb2Afn(SCAP_Inactive, xr,yr,zr);

        return;
}


/***********    PkaBuildNaf   function      **********************/
/**
 * @brief Transforms integer buffer K to NAF string.
 *
 * @author reuvenl (6/20/2014)
 *
 * @return  - On success CC_OK is returned, on failure an error code.
 */
/* static */CCError_t PkaBuildNaf(char **pNaf,     /*!< [out] Pointer to NAF key buffer (msb->lsb). */
            uint32_t *pNafSz,  /*!< [in/out] size in bytes of the NAF output.
                        Input - size of user given buffer, output - actual size of NAF key. */
                        uint32_t *pK,      /*!< [in] Pointer to key buffer. */
            uint32_t keySzBit) /*!< [in] Size of key in bits. */
{
        CCError_t err = CC_OK;
        uint32_t wK, i = 0;
        char *p; /* a pointer to the current NAF digit */

        if (keySzBit == 0 || (keySzBit+2) > *pNafSz) {
                err = PKA_NAF_KEY_SIZE_ERROR;
                goto End;
        }
        /* MSBit must be 1 */
        if ((pK[(keySzBit-1)/32] >> ((keySzBit-1)&0x1F)) != 1) {
                err = PKA_NAF_KEY_SIZE_ERROR;
                goto End;
        }

        /* set initial values */
        *pNafSz = 0; /*NAF size in bytes*/
        p = *pNaf + keySzBit + 1; /* start from the last byte */
        *p = 0;
        wK = CALC_FULL_32BIT_WORDS(keySzBit)/*+1*/; /*key size + extra word*/

        /* zeroing extra word of key buffer */
        pK[wK] = 0;

        /* scan key bits and convert to NAF */
        while (keySzBit) {
                uint32_t carry, msBit;

                i++;
                (*pNafSz)++;
                --p;
                /* check overflow */
                if (p < *pNaf) {
                        err = PKA_NAF_KEY_SIZE_ERROR;
                        goto End;
                }
                /* set NAF digit */
                *p = (pK[0] & 1) ? ((pK[0] & 2) ? '-' : '+') : '0';

                msBit = pK[wK-1] >> ((keySzBit%32)-1);
                if (*p == '-') {
                        carry = CC_CommonIncLsbUnsignedCounter(pK, 1, wK); // k += 1
                        if (carry) {
                                pK[wK] = 1;
                                keySzBit++;
                        } else if ((pK[wK-1] >> ((keySzBit%32)-1)) > msBit) {
                                keySzBit++;
                        }
                }

                EcWrstDivideVectorBy2(pK, wK+1); // k >>= 1
                keySzBit--;

                /* if MSbit is zeroed set new size value */
                wK = (CALC_FULL_32BIT_WORDS(keySzBit));
        }

        /* actual NAF vector begin */
        *pNaf = p;

End:
        return err;
}


/***********    ScalarMultAff   function      **********************/
/**
 * @brief EC scalar multiplication p = k*p, without SCA-protection features.
 *
 *  The function is more fast, than SCA protected function and performs:
 *  - PKA init,
 *  - setting input data into PKA registers,
 *  - calls PkaScalarMultAff() and then output of result data from PKA.
 *
 * @author reuvenl (03/19/2015)
 *
 * @return  - On success CC_OK is returned, on failure an error code.
 *
 */
static CCError_t ScalarMultAff(
                         const CCEcpkiDomain_t *domain,            /*!< [in] Pointer to EC domain. */
                         uint32_t *bxr, uint32_t *byr,             /*!< [out] Pointers to coordinates of result EC point.
                                                                         The size of each of buffers must be not less, than
                                                                         EC modulus size (in words). */
                         const uint32_t *k, uint32_t kSizeBit,     /*!< [in] Pointer to the scalar and its size. */
                         const uint32_t *bxp, const uint32_t *byp, /*!< [in] Pointer to coordinates of input EC point. */
                         uint32_t *tmpBuff)                        /*!< [in] Pointer to temp buffer of size
                                                                             not less than (2*ecOrderSizeInBits+1) in bytes. */
{
        CCError_t err = CC_OK;
        uint32_t nafSz;
        uint32_t modSizeInBits, modSizeInWords, ordSizeInWords;
        uint32_t *kt = tmpBuff;
        char *naf;
        uint32_t pkaReqRegs = PKA_MAX_COUNT_OF_PHYS_MEM_REGS;
            /* Define pka registers used*/
        uint8_t  xp = regTemps[18];
        uint8_t  yp = regTemps[19];
        uint8_t  xr = regTemps[20];
        uint8_t  yr = regTemps[21];

        /* set domain parameters */
        modSizeInBits  = domain->modSizeInBits;
        modSizeInWords = CALC_FULL_32BIT_WORDS(modSizeInBits);
        ordSizeInWords = CALC_FULL_32BIT_WORDS(domain->ordSizeInBits);

    if ((ordSizeInWords > (CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 1)) ||
        (modSizeInWords > CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS)) {
        return ECWRST_SCALAR_MULT_INVALID_MOD_ORDER_SIZE_ERROR;
    }

        /* temp key buf + 1 word */
        kt[ordSizeInWords] = 0; kt[ordSizeInWords-1] = 0;
        CC_PalMemCopy(kt, k, sizeof(uint32_t)*ordSizeInWords);
        /* Naf buffer */
        naf = (char*)(kt+ordSizeInWords+1);
        nafSz = (ordSizeInWords+1)*32; /*NAF size in bytes*/
        CC_PalMemSet(naf, 0, nafSz);

        /* build NAF */
        err = PkaBuildNaf(&naf, &nafSz, kt, kSizeBit);
        if (err)
                goto End;

        /*  Init PKA for modular operations */
        err = PkaInitAndMutexLock(modSizeInBits, &pkaReqRegs);
        if (err != CC_OK) {
                return err;
        }

        /*   Set data into PKA registers  */
        /* set EC parameters */
        PkaCopyDataIntoPkaReg(ECC_REG_N, 1, domain->ecP/*src_ptr*/, modSizeInWords);
        PkaCopyDataIntoPkaReg(ECC_REG_NP, 1, ((EcWrstDomain_t*)&domain->llfBuff)->modTag,
                               CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);
        PkaCopyDataIntoPkaReg(ECC_REG_EC_A, 1, domain->ecA, modSizeInWords);
        /* set point */
        PkaCopyDataIntoPkaReg(xp, 1, bxp, modSizeInWords);
        PkaCopyDataIntoPkaReg(yp, 1, byp, modSizeInWords);

        /* Call scalar mult */
        PkaScalarMultAff(xr, yr, naf,  xp,  yp);

        /*  Output data from PKA registers  */
        PkaCopyDataFromPkaReg(bxr, modSizeInWords, xr);
        PkaCopyDataFromPkaReg(byr, modSizeInWords, yr);

        PkaFinishAndMutexUnlock(pkaReqRegs);

 End:

        /* zeroing of kt and naf buffers */
        // RL NAF size according to NAF representation
        CC_PalMemSetZero(tmpBuff, (ordSizeInWords+1)*sizeof(uint32_t) + (ordSizeInWords+1)*32/*NAF buff size in bytes*/);
        return err;
}



/***********    PkaEcWrstScalarMult   function      **********************/
/**
 * @brief ECC scalar multiplication without SCA protection
 *          features (NoScap).
 *               outPoint = scalsr * inPoint.
 *       1. Checks the validity of input parameters.
 *       3. Calls the low level functions: ScalarMultAff according to SCA protection mode, to generate EC public key.
 *       4. Outputs the user public and private key structures in little endian form.
 *       5. Cleans temporary buffers.
 *       6. Exits.
 *     Mote: All buffers are given as 32-bit words arrays, where LSWord is a leftmost one.
 *
 * @return  - On success CC_OK is returned, on failure an error code.
 */
CCError_t PkaEcWrstScalarMult(const CCEcpkiDomain_t *pDomain,   /*!< [in] Pointer to current EC domain. */
                             const uint32_t       *scalar,         /*!< [in] Pointer to the scalar buffer. */
                             uint32_t             scalSizeInWords,/*!< [in] The exact size of the scalsr in words. */
                             const uint32_t       *inPointX,       /*!< [in] Pointer to the point X coordinate. */
                             const uint32_t       *inPointY,       /*!< [in] Pointer to the point Y coordinate. */
                             uint32_t             *outPointX,      /*!< [out] Pointer to the point X coordinate. */
                             uint32_t             *outPointY,      /*!< [out] Pointer to the point Y coordinate. */
                             uint32_t             *tmpBuff)        /*!< [in] The pointer to the temp buffer of size not less,
                                                                             than CC_PKA_ECPKI_SCALAR_MUL_BUFF_MAX_LENGTH_IN_WORDS. */
{
        CCError_t err = CC_OK;
        uint32_t scalarSizeInBits;
        CCCommonCmpCounter_t cmp;

        /* get exact size of scalar */
        scalarSizeInBits = CC_CommonGetWordsCounterEffectiveSizeInBits(scalar, scalSizeInWords);

        /* compare scalar to EC generator order (0 < scalar < EC order) */
        cmp = CC_CommonCmpLsWordsUnsignedCounters(scalar, scalSizeInWords,
                                                     pDomain->ecR, CALC_FULL_32BIT_WORDS(pDomain->ordSizeInBits));

        if ((scalarSizeInBits == 0) || (cmp == CC_COMMON_CmpCounter1GreaterThenCounter2)) {
                return ECWRST_SCALAR_MULT_INVALID_SCALAR_VALUE_ERROR;
        }

        /* call scalar mult. function with affine coordinates, no SCAP */
        err = ScalarMultAff(pDomain,
                            outPointX, outPointY,
                            scalar, scalarSizeInBits,
                            inPointX, inPointY,
                            tmpBuff);
        /* Note: ScalarMultAff has zeroing the tmpBuff */

        return err;

}



