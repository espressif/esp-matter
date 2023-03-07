/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "cc_pal_mem.h"
#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_ecpki_types.h"
#include "cc_ecpki_error.h"
#include "cc_ecpki_local.h"
#include "pka_hw_defs.h"
#include "pki.h"
#include "pka.h"
#include "ec_wrst.h"
#include "ec_wrst_error.h"
#include "pka_ec_wrst.h"
#include "pka_ec_wrst_glob_regs.h"
#include "pka_ec_wrst_dsa_verify_regs.h"
#include "cc_rnd_local.h"


/* canceling the lint warning:
  Info 740: Unusual pointer cast
  (incompatible indirect types) */
/*lint --e{740} */

/* canceling the lint warning:
   Info 826: Suspicious pointer-to-pointer conversion
   (area too small) */
/*lint --e{826} */

/* canceling the lint warning:
Info 716: while(1) */
/*lint --e{716} */

extern const int8_t regTemps[PKA_MAX_COUNT_OF_PHYS_MEM_REGS];

/***********      CalcSignature function      **********************/
/**
 * @brief Sets data into SRAM and calculates ECDSA Signature.
 *
 * @author reuvenl (3/29/2015)
 *
 * Note: All data in is given with LE order of words (LS word is left most).
 *       Algorithm - according ANS X9.62 denoted [1].
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static CCError_t CalcSignature(const CCEcpkiDomain_t *pDomain, /*!< [in] Pointer to EC domain structure. */
                uint32_t  *pSignPrivKey,             /*!< [in] Pointer to signer privatekey structure. */
                uint32_t  *pMsgRepres,               /*!< [in] The pointer to the message representative buffer.*/
                uint32_t  *pEphemPrivKey,            /*!< [in] pointer to private Ephemeral key buff. */
                uint32_t  *pEphemPublX,              /*!< [in] Pointer to X-coordinate of Ephemeral public. */
                uint32_t  *pSignC,                   /*!< [in] Pointer to C-part of the signature (called also R-part). */
                uint32_t  *pSignD)                   /*!< [in] Pointer to D-part of the signature (called also S-part). */
{
        CCError_t err = CC_OK;
        uint32_t status;
        uint32_t ordSizeInWords, modSizeInWords;
    uint32_t pkaReqRegs = PKA_MAX_COUNT_OF_PHYS_MEM_REGS;

        /* define regs (ECC_REG_N=0, ECC_REG_NP=1) */
    uint8_t rC    = regTemps[2]; /*C (zR) - signC*/
    uint8_t rM    = regTemps[3]; /*message (zF)*/
    uint8_t rEphK = regTemps[4]; /*ephemer.priv.key*/
    uint8_t rK    = regTemps[5]; /*priv. key (zD)*/
    uint8_t rKinv = regTemps[6];
    uint8_t rD    = regTemps[7]; /*D (zS)- signD*/
    uint8_t rT    = regTemps[8];


         /* set EC modulus and order sizes */
        ordSizeInWords = CALC_FULL_32BIT_WORDS(pDomain->ordSizeInBits);
        modSizeInWords = CALC_FULL_32BIT_WORDS(pDomain->modSizeInBits);
        if ((ordSizeInWords > (CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 1)) ||
            (modSizeInWords > CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS)) {
            return ECWRST_SCALAR_MULT_INVALID_MOD_ORDER_SIZE_ERROR;
        }

        /*  Init PKA for operations with EC order */
        err = PkaInitAndMutexLock(pDomain->ordSizeInBits , &pkaReqRegs); // RL Count regs to clean 9!
        if (err != CC_OK) {
                return err;
        }


        /*   Set data into PKA registers  */
        /* Note: ignore false positive KW warning about explicit offset:      *
           sizes, given in the EC Domain, must be right                      */
        PkaCopyDataIntoPkaReg(ECC_REG_N, 1, pDomain->ecR/*src_ptr*/, ordSizeInWords);
        PkaCopyDataIntoPkaReg(ECC_REG_NP, 1, ((EcWrstDomain_t*)&(pDomain->llfBuff))->ordTag,
                               CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);
        PkaCopyDataIntoPkaReg(rC, 1, pEphemPublX, modSizeInWords);
        PkaCopyDataIntoPkaReg(rM, 1, pMsgRepres, ordSizeInWords);
        PkaCopyDataIntoPkaReg(rEphK, 1, pEphemPrivKey, ordSizeInWords);
        PkaCopyDataIntoPkaReg(rK, 1, pSignPrivKey, ordSizeInWords);

        /* Calculate Signature  */
        /* [1] 7.3. calc. rEphK mod ECC_REG_N and check that it > 0 */
        PKA_DIV(LEN_ID_MAX_BITS, rT/*not used*/, rEphK, ECC_REG_N);
        PKA_COMPARE_IM_STATUS(LEN_ID_MAX_BITS, rEphK, 0, status);
        if(status == 1){
                err = ECWRST_DSA_SIGN_BAD_EPHEMER_KEY_TRY_AGAIN_ERROR;
                goto End;
        }

        /* check rKinv != 0  (ANS X9.62 7.3) */
        PKA_MOD_INV_W_EXP(rKinv, rEphK, rT/*temp*/); /* RL to inv_fast */
        PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, rKinv, 0, status);
        if(status == 1) {
                err = ECWRST_DSA_SIGN_BAD_EPHEMER_KEY_TRY_AGAIN_ERROR;
                goto End;
        }
        PKA_MOD_MUL(LEN_ID_N_BITS, rD, rK, rC);
        PKA_REDUCE(LEN_ID_N_BITS,rM, rM);
        PKA_MOD_ADD(LEN_ID_N_PKA_REG_BITS, rD, rD, rM);
        PKA_MOD_MUL(LEN_ID_N_BITS, rD, rD, rKinv);
        /* check rD != 0  (ANS X9.62 7.3) */
        PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, rD, 0, status);
        if(status == 1) {
                err = ECWRST_DSA_SIGN_BAD_EPHEMER_KEY_TRY_AGAIN_ERROR;
                goto End;
        }
        /* if status == 0 then copy the signature to the output */
        if(status == 0) {
                PkaCopyDataFromPkaReg(pSignC, ordSizeInWords, rC);
                PkaCopyDataFromPkaReg(pSignD, ordSizeInWords, rD);
        }

End:
        PkaFinishAndMutexUnlock(pkaReqRegs);
        return err;

}


/***********    EcWrstDsaTruncateMsg   function      **********************/
/**
 * @brief Truncates the message to given count of LS bits.
 *
 * @author reuvenl (8/19/2014)
 *
 * @return no return value.
 */
void EcWrstDsaTruncateMsg(
                uint32_t *pMsgOut,    /*!< [out] Pointer to truncated message output.
                                            Output format: bytes, LS byte is left most one. */
                uint8_t *pMsgIn,      /*!< [in] Pointer to input message in bytes. */
                uint32_t outSizeBits) /*!< [in] Exact size of truncated message in bits. */
{
    uint32_t i, shift;
    uint32_t outSizeBytes = CALC_FULL_BYTES(outSizeBits);
    uint32_t outSizeWords = CALC_FULL_32BIT_WORDS(outSizeBits);

    pMsgOut[outSizeWords-1] = 0; /* zeroing high word of output */
    /* reverse copy only a needed bytes of the message */
    CC_CommonReverseMemcpy((uint8_t*)pMsgOut, pMsgIn, outSizeBytes);

    /* shift right by redundant bits */
    shift = (8 - (outSizeBits & 7UL)) & 7UL;
    if (shift) {
        for (i = 0; i < CALC_FULL_32BIT_WORDS(outSizeBits) - 1; i++) {
            pMsgOut[i] = (pMsgOut[i] >> shift) |
            (pMsgOut[i+1] << (32-shift));
        }
        pMsgOut[i] = pMsgOut[i] >> shift;
        }
    return;
}



/***********      EcWrstDsaSign function      **********************/
/**
 * @brief Generates aphemeral key, if needed and
 *  calls specific Signature function.
 *
 * @author reuvenl (3/29/2015)
 *
 * Note: All data in buffers given with LE order of bytes and word and their sizes
 *       must be EC modulus size in words (with leading zeros)
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t EcWrstDsaSign(CCRndContext_t   *pRndContext,          /*!< [in/out] Pointer to the RND context buffer. */
        CCEcpkiPrivKey_t *pSignPrivKey,         /*!< [in] Pointer to to signer privatekey structure. */
        uint32_t             *pMsgRepres,           /*!< [in] The pointer to the message representative buffer.*/
        uint32_t              isEphemerKeyInternal, /*!< [in] boolean indicating internal (=1) or external (=0) ephemeral key. */
        uint32_t             *pEphemKey,      /*!< [in] Pointer to to private Ephemeral key buff. */
        uint32_t             *pSignC,               /*!< [in] Pointer to C-part of the signature (called also R-part). */
        uint32_t             *pSignD,               /*!< [in] Pointer to D-part of the signature (called also S-part). */
        uint32_t             *pTempBuff)      /*!< [in] Pointer to temp buffer. the buffer size must be
                                           not less than (3*ModulusSizeInWords +
                                           CC_PKA_ECPKI_SCALAR_MUL_BUFF_MAX_LENGTH_IN_WORDS)*/
{
    CCError_t err = CC_OK;
    const CCEcpkiDomain_t *pDomain = &pSignPrivKey->domain;
    uint32_t ordSizeInBits, ordSizeInWords, modSizeInWords;
    /* pointers to result EC point coordinates x, y */
    uint32_t *pEphemPublX, *pEphemPublY, *pEphemKeyBuf, *pMaxVect;
    uint32_t *funcTmpBuff;
    uint32_t countTries = 0;


    /* set EC domain parameters modulus and EC order sizes */
    ordSizeInBits  = pDomain->ordSizeInBits;
    ordSizeInWords = CALC_FULL_32BIT_WORDS(ordSizeInBits);
    modSizeInWords = CALC_FULL_32BIT_WORDS(pDomain->modSizeInBits);
    if ((ordSizeInWords > (CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 1)) ||
            (modSizeInWords > CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS)) {
        return ECWRST_SCALAR_MULT_INVALID_MOD_ORDER_SIZE_ERROR;
    }


    pEphemPublX = pTempBuff; //?? use pSignC to save mem
    pEphemKeyBuf = pEphemPublX + modSizeInWords;
    pEphemPublY = pEphemKeyBuf + ordSizeInWords;
    pMaxVect = pEphemPublY + modSizeInWords;

    while (1) {
        /*  Generate random ephemeral key   *
         * Note: Checking, that private ephemer.key  0 < k < EC order *
         * performed on LLF during scalar multiplication             */
        if(isEphemerKeyInternal) {
            /* Set bytes MaxVect= EcOrder. */
            pMaxVect[ordSizeInWords-1] = 0; /*zero MSWord of maxVect*/
            CC_PalMemCopy(pMaxVect, pDomain->ecR, sizeof(uint32_t)*ordSizeInWords);
            pEphemKeyBuf[ordSizeInWords-1] = 0; /*zero MSWord*/
            err = CC_RndGenerateVectorInRange(
                    pRndContext, pDomain->ordSizeInBits, (uint8_t*)pMaxVect/* maxVect*/, (uint8_t*)pEphemKeyBuf);
            if (err) {
                goto End;
            }
        }
        /* copy external key if provided */
        else {
            /* Note: Checking of range of the ephemeral key       *
             *  performed in PkaEcWrstScalarMult                  */
            CC_PalMemCopy(pEphemKeyBuf, pEphemKey, sizeof(uint32_t)*ordSizeInWords);
        }

        /* Calculate ephemeral public key               */
        funcTmpBuff = pMaxVect; /* because pMaxVect not needed yet */
        err = PkaEcWrstScalarMult(pDomain,
                pEphemKeyBuf/*scalar*/, ordSizeInWords, /*scalar size*/
                (uint32_t*)&pDomain->ecGx, (uint32_t*)&pDomain->ecGy, /*in point coordinates*/
                pEphemPublX/*C*/, pEphemPublY,  /*out point coordinates*/
                funcTmpBuff);
        if (err) {
            goto End;
        }


        /*  Calculate Signature S  */
        err = CalcSignature(pDomain, pSignPrivKey->PrivKey,
                pMsgRepres, pEphemKeyBuf, pEphemPublX,
                pSignC, pSignD);

        /* exit the programm if an error occurs, beside the case of   *
         *  returned error message to try a new Ephemeral Key          */
        if(err && (err != ECWRST_DSA_SIGN_BAD_EPHEMER_KEY_TRY_AGAIN_ERROR || !isEphemerKeyInternal)) {
            goto End;
        }

        /* if error is OK or count of tries > 100, then end the loop*/
        if((err == 0) || (countTries > 100)) {
            goto End;
        } else {
            countTries++;
        }
    } /* End of while() */

    End:
    return err;
}


/***********      EcWrstDsaVerify function      **********************/
/**
 * @brief Verifies the signature.
 *
 * @author reuvenl (8/7/2014)
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t EcWrstDsaVerify(CCEcpkiPublKey_t *pSignPublKey,   /*!< [in] Pointer to signer public key structure. */
                uint32_t  *pMsgRepres,         /*!< [in] The pointer to the message representative buffer.*/
                uint32_t   msgRepresSizeWords, /*!< [in] Size of the message representative buffer in words.*/
                uint32_t  *pSignC,          /*!< [in] Pointer to C-part of the signature (called also R-part). */
                uint32_t  *pSignD)          /*!< [in] Pointer to D-part of the signature (called also S-part). */
{
    CCError_t err = CC_OK;
    const CCEcpkiDomain_t *domain = &pSignPublKey->domain;
    EcWrstDomain_t *llfBuff = (EcWrstDomain_t*)&pSignPublKey->domain.llfBuff;
    int32_t modSizeInBits, modSizeInWords, ordSizeInBits, ordSizeInWords;
    uint32_t pkaReqRegs = PKA_MAX_COUNT_OF_PHYS_MEM_REGS;


    /* set domain parameters */
    modSizeInBits  = domain->modSizeInBits;
    modSizeInWords = CALC_FULL_32BIT_WORDS(modSizeInBits);
    ordSizeInBits  = domain->ordSizeInBits;
    ordSizeInWords = CALC_FULL_32BIT_WORDS(ordSizeInBits);
    if ((ordSizeInWords > (CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 1)) ||
        (modSizeInWords > CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS)) {
        return ECWRST_SCALAR_MULT_INVALID_MOD_ORDER_SIZE_ERROR;
    }

        /*  Init PKA for modular operations with EC modulus or EC order and   *
        *   registers size according to maximal of them                       */
    err = PkaInitAndMutexLock(CC_MAX(ordSizeInBits, modSizeInBits), &pkaReqRegs);
    if (err != CC_OK) {
        return err;
    }
        /* set order and modulus mod sizes */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, PKA_L0), ordSizeInBits);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, PKA_L2), modSizeInBits);

    /* Set input data into PKA registers */
        /* EC order and its Barrett tag */
    PkaCopyDataIntoPkaReg(ECC_REG_N/*dest_reg*/, 1, domain->ecR/*src_ptr*/, ordSizeInWords);
    PkaCopyDataIntoPkaReg(ECC_REG_NP, 1, llfBuff->ordTag, CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);
    /* signature C, D */
    PkaCopyDataIntoPkaReg(EC_VERIFY_REG_C, 1, pSignC, ordSizeInWords);
    PkaCopyDataIntoPkaReg(EC_VERIFY_REG_D, 1, pSignD, ordSizeInWords);
    /* message representative EC_VERIFY_REG_F */
    PkaCopyDataIntoPkaReg(EC_VERIFY_REG_F, 1, pMsgRepres, msgRepresSizeWords);
    /* Load modulus and its Barrett tag */
    PkaCopyDataIntoPkaReg(EC_VERIFY_REG_TMP_N, 1, domain->ecP, modSizeInWords);
    PkaCopyDataIntoPkaReg(EC_VERIFY_REG_TMP_NP, 1, llfBuff->modTag, CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);
    /* set pG */
    PkaCopyDataIntoPkaReg(EC_VERIFY_REG_P_GX, 1, domain->ecGx, modSizeInWords);
    PkaCopyDataIntoPkaReg(EC_VERIFY_REG_P_GY, 1, domain->ecGy, modSizeInWords);
    /* set pW */
    PkaCopyDataIntoPkaReg(EC_VERIFY_REG_P_WX, 1, pSignPublKey->x, modSizeInWords);
    PkaCopyDataIntoPkaReg(EC_VERIFY_REG_P_WY, 1, pSignPublKey->y, modSizeInWords);
    PkaCopyDataIntoPkaReg(ECC_REG_EC_A, 1, domain->ecA, modSizeInWords);

    /* Verify */
    err = PkaEcdsaVerify();

        PkaFinishAndMutexUnlock(pkaReqRegs);

    return err;

}

