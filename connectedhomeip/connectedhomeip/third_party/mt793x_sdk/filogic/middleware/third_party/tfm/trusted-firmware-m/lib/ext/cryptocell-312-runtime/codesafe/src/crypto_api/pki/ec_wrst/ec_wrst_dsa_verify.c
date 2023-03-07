/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "cc_pal_mem.h"
#ifndef ARM_DSM
#include "cc_pal_mutex.h"
#endif
#include "cc_pal_abort.h"
#include "cc_hal.h"
#include "cc_common_math.h"
#include "cc_ecpki_types.h"
#include "cc_ecpki_error.h"
#include "cc_ecpki_local.h"
#include "pka_hw_defs.h"
#include "cc_ecpki_types.h"
#include "pki.h"
#include "pka.h"
#include "ec_wrst.h"
#include "ec_wrst_error.h"
#include "pka_ec_wrst_dsa_verify_regs.h"
#include "pka_ec_wrst_glob_regs.h"


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

