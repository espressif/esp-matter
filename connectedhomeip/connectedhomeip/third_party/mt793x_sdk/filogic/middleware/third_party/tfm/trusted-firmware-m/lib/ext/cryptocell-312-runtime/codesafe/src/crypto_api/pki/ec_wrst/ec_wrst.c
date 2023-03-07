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
#include "cc_ecpki_types.h"
#include "pki.h"
#include "pka.h"
#include "pka_error.h"
#include "ec_wrst.h"
#include "pka_ec_wrst.h"
#include "ec_wrst_error.h"
#include "pki_modular_arithmetic.h"
#include "pki_dbg.h"
#include "cc_common.h"
#include "pka_point_compress_regs_def.h"

/***********    EcWrstInitPubKey   function      **********************/
/**
 *  @brief Performs uncompression (extracts Y-coordinate) checks
 *      and inits the public key (ANS X9.62-2005).
 *
 * @author reuvenl (22/09/2014)
 *
 * @return  - On success CC_OK is returned, on failure an error code.
 *
 */
CCError_t  EcWrstInitPubKey(CCEcpkiPublKey_t *pPublKey,  /*!< [in/out] Pointer to the public key structure. */
                uint8_t pointCtl)          /*!< [in] EC point control byte = (compression mode | Y-MSbit). */
{
    CCError_t err = CC_OK;
    uint32_t modSizeInBits, modSizeInWords;
    CCEcpkiDomain_t *pDomain;
    uint32_t w;
    bool  rootExist;
    uint32_t pkaReqRegs = PKA_MAX_COUNT_OF_PHYS_MEM_REGS;

    /* EC domain parameters */
    pDomain = &pPublKey->domain;

    modSizeInBits = pDomain->modSizeInBits;
    modSizeInWords = CALC_FULL_32BIT_WORDS(modSizeInBits);

    if (modSizeInWords > CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS)
        return CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_SIZE_ERROR;

    /*       lock mutex and init PKA  */
    err = PkaInitAndMutexLock(modSizeInBits, &pkaReqRegs);
    if (err != CC_OK) {
        return err;
    }

    /* set modulus P and Barrett tag NP into PKA registers 0,1 */
    PkaCopyDataIntoPkaReg(PKA_REG_N/*dstReg*/, 1, pDomain->ecP, modSizeInWords);

    PkaCopyDataIntoPkaReg(PKA_REG_NP/*dstReg*/, 1, pDomain->llfBuff/*NP*/,
        CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);

    /* set public key coordinates into PKA registers  */
    PkaCopyDataIntoPkaReg(PKA_REG_X/*dstReg*/, 1, pPublKey->x, modSizeInWords);

    /* set EC params */
    PkaCopyDataIntoPkaReg(PKA_REG_EC_A/*dstReg*/, 1, pDomain->ecA, modSizeInWords);
    PkaCopyDataIntoPkaReg(PKA_REG_EC_B/*dstReg*/, 1, pDomain->ecB, modSizeInWords);

    /* calculate  y^2 for from x */
    PKA_MOD_MUL(LEN_ID_N_BITS, PKA_REG_T/*Res*/, PKA_REG_X/*OpA*/, PKA_REG_X/*OpB*/);
    PKA_MOD_ADD(LEN_ID_N_PKA_REG_BITS, PKA_REG_T/*Res*/, PKA_REG_T/*OpA*/, PKA_REG_EC_A/*OpB*/);
    PKA_MOD_MUL(LEN_ID_N_BITS, PKA_REG_T/*Res*/, PKA_REG_X/*OpA*/, PKA_REG_T/*OpB*/);
    PKA_MOD_ADD(LEN_ID_N_PKA_REG_BITS, PKA_REG_Y2/*=PKA_REG_EC_A*/, PKA_REG_T/*OpA*/, PKA_REG_EC_B/*OpB*/);

    if((pointCtl & 6) != 2) {
        /*   Partly check uncompressed key (is it on curve ?)  */
        /* calculate y^2 directly */
        PkaCopyDataIntoPkaReg(PKA_REG_Y/*dstReg*/, 1, pPublKey->y, modSizeInWords);
        PKA_MOD_MUL(LEN_ID_N_BITS, PKA_REG_T, PKA_REG_Y, PKA_REG_Y);
        PKA_COMPARE_STATUS(LEN_ID_N_PKA_REG_BITS, PKA_REG_Y2, PKA_REG_T, w/*stat*/);
        if(w != 1) {
            err = CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_DATA_ERROR;
            goto End;
        }
    }
    else {
        /*    Uncompress the Y coordinate if needed            */
                /* derive modular square root (in/out registers according to  *
                *  included definitions file                                  */
                /*implicit parameterss: PKA_REG_Y(PKA_REG_Y1)=3(out), PKA_REG_Y2(PKA_REG_EC_A)=4(in),      *
                * PKA_REG_N=0(in)                                                    */
        rootExist = PkiIsModSquareRootExists();

                /* check is the root exists */
                if(rootExist != true){
            err = PKA_MOD_SQUARE_ROOT_NOT_EXIST_ERROR;
            goto End;
        }

                /* check Y LS-bit and set Y = -Y if need */
        PKA_READ_WORD_FROM_REG(w, 0, PKA_REG_Y);
        if((w & 1) != (pointCtl & 1)) {
            PKA_SUB(LEN_ID_N_PKA_REG_BITS, PKA_REG_Y, PKA_REG_N, PKA_REG_Y);
        }

                /* copy Y-coordinate */
        PkaCopyDataFromPkaReg(pPublKey->y/*dst*/, modSizeInWords, PKA_REG_Y/*src reg*/);
    }

End:
    PkaFinishAndMutexUnlock(pkaReqRegs);
    return err;

}


/***********     EcWrstFullCheckPublKey  function      **********************/
/**
 * @brief  Checks that the public key is valid point belonging to EC group.
 *
 *       Assuming: partly check (sizes, point is on curve) of the public key
 *                 was done previously.
 *
 * @return  - On success CC_OK is returned, on failure an error code.
 *
*/
CCError_t  EcWrstFullCheckPublKey( CCEcpkiPublKey_t    *pPublKey,    /*!< [in] Pointer to the public key structure. */
                    uint32_t             *pTempBuff) /*!< [in] Pointer to itemp buffer of size not less 2*modulusSize. */
{
    CCError_t err = CC_OK;
    uint32_t ordSizeInWords, modSizeInWords, sizeBits;
        uint32_t *outPointX, *outPointY;
        uint32_t *pTmpForFunc;

        modSizeInWords = CALC_FULL_32BIT_WORDS(pPublKey->domain.modSizeInBits);
        ordSizeInWords = CALC_FULL_32BIT_WORDS(pPublKey->domain.ordSizeInBits);

        /* scalar mult. resut coordinates  */
    outPointX = pTempBuff;
        outPointY = outPointX + modSizeInWords;
        pTmpForFunc = outPointY + modSizeInWords;


        /* scalar mult publ.key point by EC generator order ecR */
        err = PkaEcWrstScalarMult(
                                &pPublKey->domain,
                                pPublKey->domain.ecR, /*in*/
                                ordSizeInWords,       /*in - publ key X*/
                                pPublKey->x,          /*in - publ key Y*/
                                pPublKey->y,          /*in*/
                                outPointX,            /*out*/
                                outPointY,            /*out*/
                                pTmpForFunc);
        if(err) {
                err = CC_ECPKI_INTERNAL_ERROR;
                goto End;
        }

        /* check that out point is on infinity, i.e. X=0 && Y=0 */
        sizeBits = CC_CommonGetWordsCounterEffectiveSizeInBits(outPointY, modSizeInWords);
        if(sizeBits > 0){
                err = CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_DATA_ERROR;
                goto End;
        }
        /* check condition for X */
        sizeBits = CC_CommonGetWordsCounterEffectiveSizeInBits(outPointX, modSizeInWords);
        if(sizeBits > 0){
                err = CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_DATA_ERROR;
                goto End;
        }

End:
    return err;
}


/***********     EcWrstDhDeriveSharedSecret  function      **********************/
/**
 * @brief  Derives shared secrete value from provided keys. called also SVDP_DH
 *
 * @return  - On success CC_OK is returned, on failure an error code.
 *
*/
CCError_t  EcWrstDhDeriveSharedSecret(CCEcpkiPublKey_t    *pPublKey,          /*!< [in] Pointer to the public key structure. */
                    CCEcpkiPrivKey_t    *pPrivKey,        /*!< [in] Pointer to the private key structure. */
                    uint8_t                 *pSharedSecretValue,  /*!< [out] Pointer to buffer for Shared Secret Value of size
                                                 not less than modulusSize in words. */
                    CCEcdhTempData_t    *pTempBuff)       /*!< [in] Pointer to temp buffer. */
{
    CCError_t err;
    uint32_t ordSizeInWords, modSizeInWords, modSizeInBytes;
        uint32_t *outPointX, *outPointY;
        uint32_t *pTmpForFunc;

        modSizeInWords = CALC_FULL_32BIT_WORDS(pPublKey->domain.modSizeInBits);
        ordSizeInWords = CALC_FULL_32BIT_WORDS(pPublKey->domain.ordSizeInBits);
        modSizeInBytes = CALC_FULL_BYTES(pPublKey->domain.modSizeInBits);

        /* scalar mult. resut coordinates  */
    outPointX = (uint32_t*)pTempBuff;
        outPointY = outPointX + modSizeInWords;
        pTmpForFunc = outPointY + modSizeInWords;

        /* scalar mult publ.key point by EC generator order ecR */
        err = PkaEcWrstScalarMult(
                                &pPublKey->domain,
                                pPrivKey->PrivKey,    /*in*/
                                ordSizeInWords,       /*in - publ key X*/
                                pPublKey->x,          /*in - publ key Y*/
                                pPublKey->y,          /*in*/
                                outPointX,            /*out*/
                                outPointY,            /*out*/
                                pTmpForFunc);         /*in*/
        if(err)
                goto End;

        /* Note: Because output of scalar mult result point in affine         *
        *  form,there no need to check, that coordinates X,Y <= mod-1.        */
        /* Check that out point is not on infinity, i.e. X,Y != 0             */
        if(CC_CommonGetWordsCounterEffectiveSizeInBits(outPointY, modSizeInWords) == 0  ||
           CC_CommonGetWordsCounterEffectiveSizeInBits(outPointX, modSizeInWords) == 0){
                err = ECWRST_DH_SHARED_VALUE_IS_ON_INFINITY_ERROR;
                goto End;
        } else {
                /* copy X-coordinate of SecrP to output */
                CC_CommonReverseMemcpy(pSharedSecretValue, (uint8_t*)outPointX, modSizeInBytes);
        }

End:
        CC_PalMemSetZero ( pTempBuff, sizeof(CCEcdhTempData_t) );
    return err;

}



