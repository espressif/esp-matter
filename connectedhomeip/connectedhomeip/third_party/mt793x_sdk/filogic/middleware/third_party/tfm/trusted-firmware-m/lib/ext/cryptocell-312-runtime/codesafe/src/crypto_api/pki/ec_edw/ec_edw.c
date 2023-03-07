/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "cc_pal_mem.h"
#include "cc_pal_types.h"
#include "cc_pal_mutex_plat.h"
#include "cc_hal_plat.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_ecpki_error.h"
#include "cc_ec_edw_api.h"
#include "mbedtls_cc_ec_mont_edw_error.h"

#include "pka_hw_defs.h"
#include "pka_defs.h"
#include "ec_edw_local.h"
#include "pka.h"
#include "pki.h"
#include "pka_error.h"
#include "ec_edw.h"
#include "pka_ec_edw_glob_regs_def.h"

#include "pki_dbg.h"
#include "cc_int_general_defs.h"
#include "cc_general_defs.h"
#include "md.h"

/* global data definitions */
extern CC_PalMutex CCAsymCryptoMutex;


/*********************************************************************/
/*!
@brief The function performs EC Edwards key pair generation using seed.

       Libsodium analog: crypto_sign_ed25519_seed_keypair() function

 @return CCError_t
*/
CCError_t EcEdwSeedKeyPair(
                               uint8_t *pPublKey,                    /*!< [out] a pointer to publickey (compressed,
                                                                           the size = ec modulus size) */
                               uint8_t *pSecrKey,                    /*!< [out] a pointer to secret key, stated as concatenation
                                                                          of seed amd public key (the size = 2 * ec modulus size) */
                               const uint8_t  *pSeed,                /*!< [in] a pointer to the seed (the size = ec order size) */
                               const CCEcEdwDomain_t *pEcDomain,   /*!< [in] pointer to EC domain (curve). */
                               uint32_t *pTempBuff)             /*!< [in] pointer to the temp buffer. */
{
        CCError_t err = CC_OK;
        uint32_t pkaRegsUsed;
        uint32_t edwSizeWords = pEcDomain->ecModSizeInWords;
        uint32_t edwSizeBytes = edwSizeWords * sizeof(uint32_t);

        /* set pointers to temp buffers */
        uint32_t *pPrivKey32/*az*/ = pTempBuff;
        uint32_t *pPublKey32/*az*/ = pPrivKey32 + edwSizeWords;
        uint32_t *pCoordX32/*az*/ = pPublKey32 + edwSizeWords;
        const mbedtls_md_info_t *md_info=NULL;

        /* get the hardware semaphore  */
        err = CC_PalMutexLock(&CCAsymCryptoMutex, CC_INFINITE);
        if (err != CC_SUCCESS) {
                CC_PalAbort("Fail to acquire mutex\n");
        }

        /* verify that the device is not in fatal error state before activating the PKA engine */
        CC_IS_FATAL_ERR_ON(err);
        if (err == CC_TRUE) {
                /* release the hardware semaphore */
                if (CC_PalMutexUnlock(&CCAsymCryptoMutex) != CC_SUCCESS) {
                        CC_PalAbort("Fail to release mutex\n");
                }
                return PKA_FATAL_ERR_STATE_ERROR;
        }

        /* increase CC counter at the beginning of each operation */
        err = CC_IS_WAKE;
        if (err != CC_SUCCESS) {
            CC_PalAbort("Fail to increase PM counter\n");
        }

        /* init PKA, mapping and sizes tables */
        pkaRegsUsed = 30;
        err = PkaInitPka(pEcDomain->ecModSizeInBits, 0/*edwSizeWords*/,
                         &pkaRegsUsed/*regs.count*/);

        if (err != CC_SUCCESS) {
                err = CC_ECEDW_INTERNAL_ERROR;
                goto End;
        }

        /* calculate private key from seed (first half of SignSecrKey) */
        md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[CC_HASH_SHA512_mode] );
        if (NULL == md_info) {
                err = CC_ECEDW_INTERNAL_ERROR;
                goto End;
        }
        err = mbedtls_md(md_info,
                         (uint8_t*)pSeed,
                         edwSizeBytes,
                        (unsigned char *)pPrivKey32);
        if (err != CC_OK) {
                goto End;
        }

        pPrivKey32[0] &= 0xFFFFFFF8;              // & 248;
        pPrivKey32[edwSizeWords-1] &=  0x3FFFFFFF;// & 63;
        pPrivKey32[edwSizeWords-1] |=  0x40000000;// | 64;

        /* call EC scalar multiplication function */
        err = EcEdwSpecialScalarMultBase(
                                pCoordX32, /*coord.X*/
                                pPublKey32,/*coord.Y*/
                                pPrivKey32,
                                edwSizeWords,
                                pEcDomain);
        if (err)
                goto End;


        /* set high bit of Y */
        pPublKey32[edwSizeWords-1] |= ((pCoordX32[0] & 1) << 31);

        /* results output */
        CC_PalMemCopy(pSecrKey, pSeed, edwSizeBytes);
        CC_CommonConvertLswMswWordsToLsbMsbBytes(pPublKey, pPublKey32, edwSizeWords);
        CC_PalMemCopy(pSecrKey + edwSizeBytes, pPublKey, edwSizeBytes);

End:
        CC_PalMemSetZero(pTempBuff, 3*edwSizeBytes);
        PkaFinishAndMutexUnlock(pkaRegsUsed);
        return err;
}

/*********************************************************************/
/*!
 * The function performs input/output parameters for EC Edwards
 * scalar multiplication.
 *
 *         resPoint(Y,_) = k*inPoint(X,Y), where:
 *         the point is given with LE order of the words.
 *
 *         Assuming:
 *              The PKA HW is turned ON and initialized yet;
 *
 * \return CCError_t
 */
CCError_t  EcEdwScalarMultBase(
                                uint32_t  *pResPointX,             /*!< [out] a pointer (optional) to result EC point coordinate X. */
                                uint32_t  *pResPointY,             /*!< [out] a pointer to result EC point coordinate Y. */
                                uint32_t  *pScalar,                /*!< [in]  a pointer to the scalar. */
                                size_t     scalarSizeWords,        /*!< [in]  the scalar size in words. */
                                const CCEcEdwDomain_t *pEcDomain     /*!< [in]  a pointer to EC domain (curve). */)
{

        /* Definitions */

        CCError_t err = CC_OK;
        size_t edwSizeWords = pEcDomain->ecModSizeInWords;
        size_t scalarSizeInBits;


        /* Get exact scalar size in bits */
        scalarSizeInBits = CC_CommonGetWordsCounterEffectiveSizeInBits(pScalar, scalarSizeWords);
        if (scalarSizeInBits == 0)
                return CC_EC_EDW_INVALID_SCALAR_SIZE_ERROR;

        /* ********************************************* */
        /*   load needed data to defined pka registers   */
        /* ********************************************* */

        /*   pEcDomain modulus and Barr. tag */
        PkaCopyDataIntoPkaReg(EDW_REG_N, LEN_ID_MAX_BITS, pEcDomain->ecModP, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_NP, LEN_ID_MAX_BITS, pEcDomain->ecModBarrTag,
                              CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);

        /* set precalculated points data: G,2G,4G  */
        PkaCopyDataIntoPkaReg(EDW_REG_SG,   LEN_ID_MAX_BITS, pEcDomain->sg,   edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_DG,   LEN_ID_MAX_BITS, pEcDomain->dg,   edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_PG,   LEN_ID_MAX_BITS, pEcDomain->pg,   edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_MPG,  LEN_ID_MAX_BITS, pEcDomain->mpg,  edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_SG2,  LEN_ID_MAX_BITS, pEcDomain->sg2,  edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_DG2,  LEN_ID_MAX_BITS, pEcDomain->dg2,  edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_PG2,  LEN_ID_MAX_BITS, pEcDomain->pg2,  edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_MPG2, LEN_ID_MAX_BITS, pEcDomain->mpg2, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_SG4,  LEN_ID_MAX_BITS, pEcDomain->sg4,  edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_DG4,  LEN_ID_MAX_BITS, pEcDomain->dg4,  edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_PG4,  LEN_ID_MAX_BITS, pEcDomain->pg4,  edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_MPG4, LEN_ID_MAX_BITS, pEcDomain->mpg4, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_XG2,  LEN_ID_MAX_BITS, pEcDomain->xg2,  edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_YG2,  LEN_ID_MAX_BITS, pEcDomain->yg2,  edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_TG2,  LEN_ID_MAX_BITS, pEcDomain->tg2,  edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_XG4,  LEN_ID_MAX_BITS, pEcDomain->xg4,  edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_YG4,  LEN_ID_MAX_BITS, pEcDomain->yg4,  edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_TG4,  LEN_ID_MAX_BITS, pEcDomain->tg4,  edwSizeWords);
        /* set D2 */
        PkaCopyDataIntoPkaReg(EDW_REG_D2, LEN_ID_MAX_BITS, pEcDomain->ecAuxValD2, edwSizeWords);

        /*--------------------------------------------------------------------*
        * perform EC scalar multiplication:  used PKA registers defined in    *
        *    "pka_ec_edw_glob_regs_def."h file; input - scalr;                *
        *     output registers: EC_MONT_REG_YS and EDW_REG_XS                 *
        *---------------------------------------------------------------------*/

        /* used common scalarMult function; output regs:  *
         *       x<-EDW_REG_SG, y<-EDW_REG_DG             */
        err = PkaEcEdwScalarMultBase(pScalar, scalarSizeInBits);
        if (err) {
                goto End;
        }

        /* output result point: x,y */
        PkaCopyDataFromPkaReg(pResPointX/*dst*/, edwSizeWords, EDW_REG_SG/*srcReg*/);
        PkaCopyDataFromPkaReg(pResPointY/*dst*/, edwSizeWords, EDW_REG_DG/*srcReg*/);

        End:
        return err;
}

/*********************************************************************/
/*!
 * The function performs input/output parameters for EC Edwards scalar
 * multiplication for special scalars.
 *
 *         resPoint(Y,_) = k*inPoint(X,Y), where:
 *         the point is given with LE order of the words.
 *
 *         Assuming:
 *              The PKA HW is turned on and initialized yet;
 *
 * \return CCError_t
 */
CCError_t  EcEdwSpecialScalarMultBase(
                                       uint32_t  *pResPointX,             /*!< [out] a pointer to result EC point coordinate X. */
                                       uint32_t  *pResPointY,             /*!< [out] a pointer to result EC point coordinate Y. */
                                       uint32_t  *pScalar,                /*!< [in]  a pointer to the scalar. */
                                       size_t     scalarSizeWords,         /*!< [in]  the scalar size in words. */
                                       const CCEcEdwDomain_t *pEcDomain /*!< [in]  a pointer to EC domain (curve). */)
{

        /* Definitions */

        CCError_t err = CC_OK;
        size_t   edwSizeWords = pEcDomain->ecModSizeInWords;
        size_t   scalarSizeBits;

        /* Get exact scalar size in bits */
        scalarSizeBits = CC_CommonGetWordsCounterEffectiveSizeInBits(pScalar, scalarSizeWords);
        if (scalarSizeWords == 0)
                return CC_EC_EDW_INVALID_SCALAR_SIZE_ERROR;

        /* ********************************************* */
        /*   load needed data to defined pka registers   */
        /* ********************************************* */

        /*   pEcDomain modulus and Barr. tag */
        PkaCopyDataIntoPkaReg(EDW_REG_N, LEN_ID_MAX_BITS, pEcDomain->ecModP, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_NP, LEN_ID_MAX_BITS, pEcDomain->ecModBarrTag,
                              CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);

        /* set precalculated points data: G,4G...32G,  */
        PkaCopyDataIntoPkaReg(EDW_REG_SG4, LEN_ID_MAX_BITS, pEcDomain->sg4, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_DG4, LEN_ID_MAX_BITS, pEcDomain->dg4, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_PG4, LEN_ID_MAX_BITS, pEcDomain->pg4, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_MPG4, LEN_ID_MAX_BITS, pEcDomain->mpg4, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_SG8, LEN_ID_MAX_BITS, pEcDomain->sg8, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_DG8, LEN_ID_MAX_BITS, pEcDomain->dg8, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_PG8, LEN_ID_MAX_BITS, pEcDomain->pg8, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_MPG8, LEN_ID_MAX_BITS, pEcDomain->mpg8, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_SG16, LEN_ID_MAX_BITS, pEcDomain->sg16, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_DG16, LEN_ID_MAX_BITS, pEcDomain->dg16, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_PG16, LEN_ID_MAX_BITS, pEcDomain->pg16, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_MPG16, LEN_ID_MAX_BITS, pEcDomain->mpg16, edwSizeWords);
        /* s = 32G*/
        PkaCopyDataIntoPkaReg(EDW_REG_XS, LEN_ID_MAX_BITS, pEcDomain->xg32, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_YS, LEN_ID_MAX_BITS, pEcDomain->yg32, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_TS, LEN_ID_MAX_BITS, pEcDomain->tg32, edwSizeWords);
        PKA_CLEAR(LEN_ID_MAX_BITS, EDW_REG_ZS);
        PKA_SET_BIT0(LEN_ID_MAX_BITS, EDW_REG_ZS, EDW_REG_ZS);
        /* set 4N */
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_N_4, EDW_REG_N, EDW_REG_N);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_N_4, EDW_REG_N_4, EDW_REG_N_4);
        /* set D2 */
        PkaCopyDataIntoPkaReg(EDW_REG_D2, LEN_ID_MAX_BITS, pEcDomain->ecAuxValD2, edwSizeWords);

        /*--------------------------------------------------------------------*
        * perform EC scalar multiplication:  used PKA registers defined in    *
        *    "pka_ec_edw_glob_regs_def."h file; input - scalr;                *
        *     output registers: EC_MONT_REG_YS and EDW_REG_XS                 *
        *---------------------------------------------------------------------*/
        /* use special (more fasted) scalarMult function */
        err = PkaEcEdwSpecialScalarMultBase(pScalar, scalarSizeBits);

        if (err) {
                goto End;
        }

        /* output result point: X,Y */
        PkaCopyDataFromPkaReg(pResPointX/*dst*/, edwSizeWords, EDW_REG_SG8/*srcReg*/);
        PkaCopyDataFromPkaReg(pResPointY/*dst*/, edwSizeWords, EDW_REG_DG8/*srcReg*/);

        End:
        return err;
}


/**
 * The function createss EC Edwards detatched signature on given message
 *
 * Implemented algorithm of Bernstein D. etc. sign ed25519 detached.
 *
 * @author reuvenl (1/21/2016)
 *
 * @return CCError_t
 */
CCError_t  EcEdwSign (
                       uint8_t       *pEdwSign,             /*!< [out] - the pointer to the signature (detatched). */
                       const uint8_t *pMsg,                 /*!< [in] - the pointer to the message. */
                       size_t         msgSize,              /*!< [in] - the message size in bytes: it must be less, than
                                                                 CC_HASH_UPDATE_DATA_MAX_SIZE_IN_BYTES. */
                       const uint8_t *pSignSecrKey,         /*!< [in] - the pointer to the signer secret key, stated as
                                                                 concatenation of the seed and public key) */
                       const CCEcEdwDomain_t *pEcDomain,      /*!< [in] pointer to EDW EC domain (curve). */
                       CCEcEdwTempBuff_t *pEcEdwSignTempBuff)   /*!< [in] pointer to the temp buffer. */
{

        CCError_t err = CC_OK;
        size_t edwSizeWords = pEcDomain->ecOrdSizeInWords;
        size_t edwSizeBytes = edwSizeWords * sizeof(uint32_t);
        size_t hashSizeWords = CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES / sizeof(uint32_t);
        size_t sizeToHash, msgSize1;

        /* set pointers to temp buffers */
        uint32_t *pPrivKey = (uint32_t*)pEcEdwSignTempBuff; /*h1||h2 = hash(seed)*/
        uint32_t *pEphPriv = pPrivKey + edwSizeWords; /*nonce*/
        uint32_t *pSign = pEphPriv + edwSizeWords; /*h2 is placed there for hash*/
        uint32_t *pHash = pSign + 2*edwSizeWords; /*second part of */
        uint32_t *pIntegrVal/*hram*/ = pHash + CC_HASH_SHA512_DIGEST_SIZE_IN_WORDS;

        const mbedtls_md_info_t *md_info=NULL;
        mbedtls_md_context_t *p_hash_ctx = NULL;

        /*-------------------------------------------------*/
        /*          Process signature calculation          */
        /*-------------------------------------------------*/

        /* 1. Calculate hash512(seed) = pPrivKey||h2 for private key (for h2 temporary used pEphPrivKy buff) */
        md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[CC_HASH_SHA512_mode] );
        if (NULL == md_info) {
                err = CC_ECEDW_INTERNAL_ERROR;
                goto End;
        }
        err = mbedtls_md(md_info,
                         (uint8_t*)pSignSecrKey,
                         edwSizeBytes,
                        (unsigned char *)pPrivKey);
        if (err != CC_OK) {
                goto End; // h1 = 32 bytes of hash(seed) ;
        }

        /* save privKey = h1 and set bits */
        ((uint8_t*)pPrivKey)[0]  &= 248;
        ((uint8_t*)pPrivKey)[edwSizeBytes-1] &= 63;
        ((uint8_t*)pPrivKey)[edwSizeBytes-1] |= 64;

        /* 2. Calculate a nonce for ephemeral priv. key:  hash(h2||msg)  */
        /*---------------------------------------------------------------*/
        p_hash_ctx = (mbedtls_md_context_t*)(pIntegrVal + 2*edwSizeWords);
        mbedtls_md_init(p_hash_ctx);
        err = mbedtls_md_setup(p_hash_ctx, md_info, 0); // 0 = HASH, not HMAC
        if (err != CC_OK) {
                goto End;
        }
        err = mbedtls_md_starts(p_hash_ctx);
        if (err != CC_OK)
                goto End;

        if (msgSize == 0) {
                sizeToHash = edwSizeBytes;
                msgSize1 = 0;
        } else {
                if (msgSize <= CC_HASH_SHA512_BLOCK_SIZE_IN_BYTES - edwSizeBytes) {
                        CC_PalMemCopy((uint8_t*)pEphPriv + edwSizeBytes, pMsg, msgSize);
                        sizeToHash = edwSizeBytes + msgSize;
                        msgSize1 = 0;
                } else {
                        sizeToHash = CC_HASH_SHA512_BLOCK_SIZE_IN_BYTES;
                        CC_PalMemCopy((uint8_t*)pEphPriv + edwSizeBytes, pMsg, sizeToHash - edwSizeBytes);
                        msgSize1 = msgSize - (sizeToHash - edwSizeBytes);
                }
        }
        err = mbedtls_md_update(p_hash_ctx, (uint8_t*)pEphPriv/*h2*/, sizeToHash);
        if (err != CC_OK) /*full block, padded by msg*/
                goto End;

        if (msgSize1 > 0) {
                err = mbedtls_md_update(p_hash_ctx, (uint8_t*)pMsg+(sizeToHash - edwSizeBytes), msgSize1);
                if (err != CC_OK)   /*remaining bytes of the msg*/
                        goto End;
        }
        err = mbedtls_md_finish(p_hash_ctx, (unsigned char *)pEphPriv);
        if (err != CC_OK)
                goto End;

        mbedtls_md_free(p_hash_ctx);
        p_hash_ctx = NULL;

        /*------------------------------------------*/
        /*        PKA Initialization                */
        /*------------------------------------------*/
        /* get and lock the PKA hardware */
        err = CC_PalMutexLock(&CCAsymCryptoMutex, CC_INFINITE);
        if (err != CC_SUCCESS) {
                CC_PalAbort("Fail to acquire mutex\n");
        }

        /* verify that the device is not in fatal error state before activating the PKA engine */
        CC_IS_FATAL_ERR_ON(err);
        if (err == CC_TRUE) {
                /* release the hardware semaphore */
                if (CC_PalMutexUnlock(&CCAsymCryptoMutex) != CC_SUCCESS) {
                        CC_PalAbort("Fail to release mutex\n");
                }
                return PKA_FATAL_ERR_STATE_ERROR;
        }

        /* increase CC counter at the beginning of each operation */
        err = CC_IS_WAKE;
        if (err != CC_SUCCESS) {
            CC_PalAbort("Fail to increase PM counter\n");
        }

        /* init PKA mapping according to 2*EC modulus size  */
        err = PkaInitPka(2*CC_BITS_IN_32BIT_WORD*edwSizeWords,
                         0/*regSizeInPkaWords*/, NULL/*pRegsCount*/);
        if (err != CC_SUCCESS) {
                err = CC_ECEDW_INTERNAL_ERROR;
                goto End;
        }

        /* 3. calculate reduced ephemer. priv. Key */
        PkaCopyDataIntoPkaReg(EDW_REG_N, LEN_ID_N_PKA_REG_BITS, pEcDomain->ecOrdN, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_EPH_PRIV, LEN_ID_N_PKA_REG_BITS, pEphPriv, hashSizeWords);
        PKA_DIV(LEN_ID_N_PKA_REG_BITS, EDW_REG_T5/*4*/, EDW_REG_EPH_PRIV/*24*/, EDW_REG_N/*0*/);
        PkaCopyDataFromPkaReg(pEphPriv/*dst*/, edwSizeWords, EDW_REG_EPH_PRIV/*srcReg*/);

        /* remapping of PKA registers and sizes for scalar mult. */
        err = PkaInitPka(pEcDomain->ecModSizeInBits, pEcDomain->ecModSizeInWords, NULL);
        if (err != CC_SUCCESS) {
                err = CC_ECEDW_INTERNAL_ERROR;
                goto End;
        }

        /* 4. calculate ephemer. public key */
        err = EcEdwScalarMultBase(
                                 pSign+edwSizeWords, /*Eph.Publ. X*/
                                 pSign,              /*Eph.Publ. Y*/
                                 pEphPriv,           /*Eph.Priv. key*/
                                 edwSizeWords,       /*scalarSize*/
                                 pEcDomain);         /*Ec Domain*/
        if (err) {
                goto End;
        }

        /* convert eph. public key to compressed: MSBit(Y) = LSBit(X) */
        pSign[edwSizeWords-1] ^= ((pSign[edwSizeWords] & 1) << 31);

        /* copy the user public key to the signature second half */
        CC_PalMemCopy(((uint8_t*)pSign+edwSizeBytes), pSignSecrKey+edwSizeBytes, edwSizeBytes);

        /* 5. concatenate data for integrity value: EphPublKey || userPublKey || Msg */
        /*---------------------------------------------------------------------------*/

        if (msgSize == 0) {
                sizeToHash = 2*edwSizeBytes;
                msgSize1 = 0;
        } else {
                if (msgSize <= CC_HASH_SHA512_BLOCK_SIZE_IN_BYTES - 2*edwSizeBytes) {
                        CC_PalMemCopy((uint8_t*)pSign + 2*edwSizeBytes, pMsg, msgSize);
                        sizeToHash = 2*edwSizeBytes + msgSize;
                        msgSize1 = 0;
                } else {
                        sizeToHash = CC_HASH_SHA512_BLOCK_SIZE_IN_BYTES;
                        CC_PalMemCopy((uint8_t*)pSign + 2*edwSizeBytes, pMsg, sizeToHash - 2*edwSizeBytes);
                        msgSize1 = msgSize - 2*edwSizeBytes;
                }
        }

        /* 6. calculate hash for integrity value */
        p_hash_ctx = (mbedtls_md_context_t*)(pIntegrVal + 2*edwSizeWords);
        mbedtls_md_init(p_hash_ctx);
        err = mbedtls_md_setup(p_hash_ctx, md_info, 0); // 0 = HASH, not HMAC
        if (err != CC_OK) {
                goto End;
        }
        err = mbedtls_md_starts(p_hash_ctx);
        if (err != CC_OK)
                goto End;
        err = mbedtls_md_update(p_hash_ctx, (uint8_t*)pSign, sizeToHash);
        if (err != CC_OK)
                goto End;

        if (msgSize1 > 0) {
                err = mbedtls_md_update(p_hash_ctx, (uint8_t*)pMsg+2*edwSizeBytes, msgSize1);
                if (err != CC_OK)
                        goto End;
        }
        err = mbedtls_md_finish(p_hash_ctx, (unsigned char *)pIntegrVal);
        if (err != CC_OK)
                goto End;

        /* PKA registers remapping according to 2*EC order size  */
        err = PkaInitPka(2*CC_BITS_IN_32BIT_WORD*edwSizeWords,
                         0/*regSizeInPkaWords*/, NULL/*pRegsCount*/);
        if (err != CC_SUCCESS) {
                err = CC_ECEDW_INTERNAL_ERROR;
                goto End;
        }

        /* 7. set data to PKA registers and reduce integrity value */
        PkaCopyDataIntoPkaReg(EDW_REG_N, LEN_ID_N_PKA_REG_BITS, pEcDomain->ecOrdN, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_INTEGR, LEN_ID_N_PKA_REG_BITS, pIntegrVal, hashSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_USER_PRIV, LEN_ID_N_PKA_REG_BITS, pPrivKey, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_EPH_PRIV, LEN_ID_N_PKA_REG_BITS, pEphPriv, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_NP, LEN_ID_N_PKA_REG_BITS, pEcDomain->ecOrdBarrTag,
                              CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);

        PKA_DIV(LEN_ID_N_PKA_REG_BITS, EDW_REG_D2/*temp*/, EDW_REG_INTEGR, EDW_REG_N);

        /* set sizes for modMul op. according to actual size of the EC order  */
        PKA_WAIT_ON_PKA_DONE();
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_L0), pEcDomain->ecOrdSizeInBits);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_L0) + 4,
                                edwSizeWords*CC_BITS_IN_32BIT_WORD + CC_PKA_WORD_SIZE_IN_BITS);


        /* 8. calculation of the Edw. specific signature value edwSignVal */
        /*----------------------------------------------------------------*/
        PKA_MOD_MUL(LEN_ID_N_BITS, EDW_REG_SIGN/*res*/, EDW_REG_INTEGR, EDW_REG_USER_PRIV);
        PKA_MOD_ADD(LEN_ID_N_BITS, EDW_REG_SIGN/*res*/, EDW_REG_SIGN, EDW_REG_EPH_PRIV);
        /* output EdwSignVal from PKA  */
        PkaCopyDataFromPkaReg(&pSign[edwSizeWords]/*dst*/, edwSizeWords, EDW_REG_SIGN/*srcReg*/);
        CC_PalMemCopy(pEdwSign, pSign, 2*edwSizeBytes);

        End:
        if((md_info!=NULL) && (p_hash_ctx!=NULL)) {
                mbedtls_md_free(p_hash_ctx);
        }
        CC_PalMemSetZero(pEcEdwSignTempBuff, sizeof(CCEcEdwTempBuff_t));
        PkaFinishAndMutexUnlock(CC_INFINITE);

        return err;
}

/*********************************************************************/
/*!
@brief The function performs two EC Edwards scalar multiplication and adding
       of points:  resPoint = scalarA * Point(X,Y) + scalarB * BasePointG.

       Assuming: - PKA is initialized yet (including setting N and NP in registers);
                 - Input point may be given in uncompressed form (given both coord. X,Y)
                   or in compressed form: coordinate Y only and LSBit of X on bit place,
                   equalled to modulus size in bits (255 for ec25519).

 @return CCError_t
*/
CCError_t EcEdwAddTwoScalarMult(
                                 uint32_t *pResPoint,        /*!< [out] the pointer to the result point in compressed
                                                                 form (coordinate Y, the size in bytes = ec modulus size). */
                                 uint32_t *pScalarA,         /*!< [in] pointer to the scalarA, the size = ec modulus size in bytes). */
                                 uint32_t *pPointY,          /*!< [in] a pointer to the input point coordinate Y - in uncompressed
                                                                 form, if given coordinate X, or in compressed form if coordinate
                                                                 X == NULL (the size of Y in bytes = ec modulus size). */
                                 uint32_t *pPointX,          /*!< [in, optional] the pointer to input point coordinate X for uncompressed
                                                                 form of point(the size of Y in bytes = ec modulus size). */
                                 uint32_t *pScalarB,         /*!< [in] pointer to the scalarB, the size = ec modulus sizein bytes). */
                                 const CCEcEdwDomain_t *pEcDomain /*!< [in] pointer to EC domain (curve). */)
{
        CCError_t err = CC_OK;
        size_t edwSizeWords;
        size_t scalarAsizeBits, scalarBsizeBits;
        uint32_t isOddX = 0, shift;

        /* count of bits for MS bit setting of Y coordinate */
        shift = 31;

        /* get bit sizes of scalars */
        edwSizeWords = pEcDomain->ecModSizeInWords;
        scalarAsizeBits = CC_CommonGetWordsCounterEffectiveSizeInBits( pScalarA, edwSizeWords);
        scalarBsizeBits = CC_CommonGetWordsCounterEffectiveSizeInBits( pScalarB, edwSizeWords);

        /*   pEcDomain modulus and Barr. tag and input point X,Y */
        PkaCopyDataIntoPkaReg(EDW_REG_N, LEN_ID_MAX_BITS, pEcDomain->ecModP, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_NP, LEN_ID_MAX_BITS, pEcDomain->ecModBarrTag,
                              CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);
        /*--------------------------------------------------------------*/
        /* decompression of input point, if given in compressed form    */
        /*--------------------------------------------------------------*/
        if (pPointX == NULL) {
                /* load EC params data into PKA */
                PkaCopyDataIntoPkaReg(EDW_REG_D, LEN_ID_MAX_BITS, pEcDomain->ecParamD, edwSizeWords);
                PkaCopyDataIntoPkaReg(EDW_REG_Q58, LEN_ID_MAX_BITS, pEcDomain->ecAuxValQ58, edwSizeWords);
                PkaCopyDataIntoPkaReg(EDW_REG_SQRTM1, LEN_ID_MAX_BITS, pEcDomain->ecAuxValSqrt_1, edwSizeWords);

                /* extract LSBit of X from compressed Y */
                isOddX = (pPointY[edwSizeWords-1] >> shift) & 1;
                pPointY[edwSizeWords-1] &= ~(1UL << shift); /*clean MSBit in Y-buffer*/

                /* load point coord. Y into PKA */
                PkaCopyDataIntoPkaReg(EDW_REG_YP, LEN_ID_MAX_BITS, pPointY/*Y*/, edwSizeWords);

                /* decompress input Point (in/out registers: EDW_REG_YP - in, EDW_REG_XP - out) */
                PkaEcEdwDecompress(EDW_REG_XP, EDW_REG_YP, isOddX);

        } else {
                /* load point coord. X,Y data */
                PkaCopyDataIntoPkaReg(EDW_REG_YP, LEN_ID_MAX_BITS, pPointY/*Y*/, edwSizeWords);
                PkaCopyDataIntoPkaReg(EDW_REG_XP, LEN_ID_MAX_BITS, pPointX/*Y*/, edwSizeWords);
        }

        /* call EC scalar mult. add function with implicitly defined registers:
           input aff. point P is in registers: (EDW_REG_XP, EDW_REG_YP)
           output aff. point R is in registers: (EDW_REG_XR, EDW_REG_YR) */
        err = PkaEcEdwAddTwoScalarMult(
                                      EDW_REG_XR, EDW_REG_YR,
                                      EDW_REG_XP, EDW_REG_YP,
                                      pScalarA, scalarAsizeBits,
                                      pScalarB, scalarBsizeBits,
                                      pEcDomain);
        if (err != CC_OK)
                goto End;


        /* output result point in compressed form Y */
        PkaCopyDataFromPkaReg(pResPoint/*dst*/, edwSizeWords, EDW_REG_XR/*srcReg*/); /*X*/
        /* get bit b'0 of XR */
        PKA_READ_BIT0(LEN_ID_MAX_BITS, EDW_REG_XR, isOddX);
        PkaCopyDataFromPkaReg(pResPoint/*dst*/, edwSizeWords, EDW_REG_YR/*srcReg*/); /*Y*/
        /* set MSbit YR =  b'0 XR */
        if(pPointX == NULL) {
                pResPoint[edwSizeWords-1] |= (isOddX << shift);
        }

        End:
        return err;
}


/**
 * The function verifies EC Edwards detatched signature on given message
 *
 * Implemented algorithm of Bernstein D. etc.
 *
 * @author reuvenl (1/21/2016)
 *
 * @return CCError_t
 */
CCError_t  EcEdwSignVerify (
                        const uint8_t *pInSign,              /*!< [in] - the pointer to input signature. */
                        const uint8_t *pMsg,                 /*!< [in] - the pointer to the message. */
                        size_t         msgSize,              /*!< [in] - the message size in bytes: it must not great
                                                                  than CC_HASH_UPDATE_DATA_MAX_SIZE_IN_BYTES. */
                        const uint8_t *pSignPublKey,         /*!< [in] - the pointer to the signer public key, stated
                                                                  in compressed form of coordinate Y */
                        const CCEcEdwDomain_t *pEcDomain,  /*!< [in] pointer to EDW EC domain (curve). */
                        CCEcEdwTempBuff_t *pEcEdwTempBuff) /*!< [in] pointer to the temp buffer. */
{

        CCError_t err = CC_OK;
        size_t edwSizeWords = pEcDomain->ecModSizeInWords;
        size_t edwSizeBytes = edwSizeWords * sizeof(uint32_t);
        size_t hashSizeWords = CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES / sizeof(uint32_t);
        uint32_t sizeToHash, msgSize1;
        /* set pointers to 32-bit aligned buffers */
        uint32_t *pSign = (uint32_t*)pEcEdwTempBuff;
        /* next uses the same buff */
        uint32_t *pIntegrVal = pSign/*hram*/;
        uint32_t *pPublKeyY = pIntegrVal + edwSizeWords;
        uint32_t *pEdwSignVal = pPublKeyY + edwSizeWords;
        uint32_t *pEphPubl = pEdwSignVal + edwSizeWords;
        uint32_t *pResPoint = pEphPubl + edwSizeWords; /*use previous buff*/

        const mbedtls_md_info_t *md_info=NULL;
        mbedtls_md_context_t *p_hash_ctx = (mbedtls_md_context_t*)(pResPoint + 2*edwSizeWords);;

        /* get and lock the PKA hardware */
        err = CC_PalMutexLock(&CCAsymCryptoMutex, CC_INFINITE);
        if (err != CC_SUCCESS) {
                CC_PalAbort("Fail to acquire mutex\n");
        }

        /* verify that the device is not in fatal error state before activating the PKA engine */
        CC_IS_FATAL_ERR_ON(err);
        if (err == CC_TRUE) {
                /* release the hardware semaphore */
                if (CC_PalMutexUnlock(&CCAsymCryptoMutex) != CC_SUCCESS) {
                        CC_PalAbort("Fail to release mutex\n");
                }
                return PKA_FATAL_ERR_STATE_ERROR;
        }

        /* increase CC counter at the beginning of each operation */
        err = CC_IS_WAKE;
        if (err != CC_SUCCESS) {
            CC_PalAbort("Fail to increase PM counter\n");
        }

        /*-------------------------------------------------*/
        /*          Process signature verification         */
        /*-------------------------------------------------*/

        /* check that EdwSignValue have two MS bits = 0 */
        if (pInSign[2*edwSizeBytes - 1] >> ((pEcDomain->ecModSizeInBits % CC_BITS_IN_32BIT_WORD) - 2)) {
                return CC_EC_EDW_SIGN_VERIFY_FAILED_ERROR;
        }

        /* check that publ.key != 0 */
        if (PkiIsUint8ArrayEqualTo0(pSignPublKey, edwSizeBytes)) {

                return CC_EC_EDW_SIGN_VERIFY_FAILED_ERROR;
        }

        /* create integrity value on: EphPublKey || userPublKey || Msg */
        /*-------------------------------------------------------------*/

        /* copy input EphPublKey||userPublKey to aligned buffer as bytes */
        CC_PalMemCopy((uint8_t*)pSign, pInSign, edwSizeBytes);
        CC_PalMemCopy((uint8_t*)pSign + edwSizeBytes, pSignPublKey, edwSizeBytes);

        if (msgSize == 0) {
                sizeToHash = 2*edwSizeBytes;
                msgSize1 = 0;
        } else {
                if (msgSize <= CC_HASH_SHA512_BLOCK_SIZE_IN_BYTES - 2*edwSizeBytes) {
                        CC_PalMemCopy((uint8_t*)pSign + 2*edwSizeBytes, pMsg, msgSize);
                        sizeToHash = 2*edwSizeBytes + msgSize;
                        msgSize1 = 0;
                } else {
                        sizeToHash = CC_HASH_SHA512_BLOCK_SIZE_IN_BYTES;
                        CC_PalMemCopy((uint8_t*)pSign + 2*edwSizeBytes, pMsg, sizeToHash - 2*edwSizeBytes);
                        msgSize1 = msgSize - (sizeToHash - 2*edwSizeBytes);
                }
        }

        /*         HASH operations      */
        /*------------------------------*/
        md_info = mbedtls_md_info_from_string( HashAlgMode2mbedtlsString[CC_HASH_SHA512_mode] );
        if (NULL == md_info) {
                err = CC_EC_EDW_SIGN_VERIFY_FAILED_ERROR;
                goto End;
        }
        mbedtls_md_init(p_hash_ctx);
        err = mbedtls_md_setup(p_hash_ctx, md_info, 0); // 0 = HASH, not HMAC
        if (err != CC_OK) {
                goto End;
        }
        err = mbedtls_md_starts(p_hash_ctx);
        if (err != CC_OK)
                goto End;

        err = mbedtls_md_update(p_hash_ctx, (uint8_t*)pSign/*h2*/, sizeToHash);
        if (err != CC_OK) /*full block, padded by msg*/
                goto End;

        if (msgSize1 > 0) {
                err = mbedtls_md_update(p_hash_ctx, (uint8_t*)pMsg+(sizeToHash - 2*edwSizeBytes), msgSize1);
                if (err != CC_OK)   /*remaining bytes of the msg*/
                        goto End;
        }
        err = mbedtls_md_finish(p_hash_ctx, (unsigned char *)pSign);
        if (err != CC_OK)
                goto End;

        /* reduce IntegrVal by EC order, set result into EDW_REG_INTEGR  */
        /*---------------------------------------------------------------*/

        /* init PKA mapping and sizes tables according to 2 modulus size */
        err = PkaInitPka(CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES*CC_BITS_IN_BYTE,
                         0/*RegSizeInWords*/, NULL);
        if (err != CC_SUCCESS) {
                err = CC_ECEDW_INTERNAL_ERROR;
                goto End;
        }

        /* load IntegrVal and mod.P into paired register for each parameter */
        PkaCopyDataIntoPkaReg(EDW_REG_INTEGR, LEN_ID_N_PKA_REG_BITS, pSign/*IntegrVal*/, hashSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_N, LEN_ID_N_PKA_REG_BITS, pEcDomain->ecOrdN, edwSizeWords);
        /* reduce by order */
        PKA_DIV(LEN_ID_N_PKA_REG_BITS, EDW_REG_T5/*4 not used*/, EDW_REG_INTEGR/*3*/, EDW_REG_N);
        /* output reduced EphPrivKey */
        PkaCopyDataFromPkaReg(pIntegrVal/*dst*/, edwSizeWords, EDW_REG_INTEGR/*srcReg*/);

        /* init PKA mapping and sizes tables according to EC modulus size */
        /*----------------------------------------------------------------*/
        err = PkaInitPka(pEcDomain->ecModSizeInBits, 0/*pEcDomain->ecModSizeInWords*/, NULL);
        if (err != CC_SUCCESS) {
                err = CC_ECEDW_INTERNAL_ERROR;
                goto End;
        }

        /* set the user publ. key as words and negate it */
        CC_CommonConvertLsbMsbBytesToLswMswWords(pPublKeyY, pSignPublKey, edwSizeBytes);
        pPublKeyY[edwSizeWords-1] ^= (1UL << 31);

        CC_CommonConvertLsbMsbBytesToLswMswWords(pEdwSignVal, &pInSign[edwSizeBytes], edwSizeBytes);

        /* perform two scalar mult. with adding: ("**" - scalar mult)
            ResPoint = pIntegrVal**PublKey + EdwSignVal**EcBasePoint */
        err = EcEdwAddTwoScalarMult(
                                   pResPoint,
                                   pIntegrVal/*scalarA*/,
                                   pPublKeyY, NULL/*pPublKeyX*/,
                                   pEdwSignVal/*scalarB*/,
                                   pEcDomain);
        if (err) {
                goto End;
        }

        /* specific verification of the Edw. signature  */
        CC_CommonConvertLsbMsbBytesToLswMswWords(pEphPubl, pInSign, edwSizeBytes);

        if (!PkiAreBuffersEqual(pResPoint, pEphPubl, edwSizeBytes)) {
                err = CC_EC_EDW_SIGN_VERIFY_FAILED_ERROR;
                goto End;
        }

        End:
        if(md_info!=NULL){
                mbedtls_md_free(p_hash_ctx);
        }

        CC_PalMemSetZero(pEcEdwTempBuff, sizeof(CCEcEdwTempBuff_t));

        PkaFinishAndMutexUnlock(CC_INFINITE);

        return err;
}


