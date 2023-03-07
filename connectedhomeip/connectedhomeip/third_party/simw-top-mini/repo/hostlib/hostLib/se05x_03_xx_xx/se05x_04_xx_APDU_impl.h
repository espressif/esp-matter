/*
 * Copyright 2020 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <se05x_tlv.h>
#include <se05x_const.h>
#include <se05x_enums.h>

#if defined(NONSECURE_WORLD)
#include "veneer_printf_table.h"
#endif

#if SSS_HAVE_SE05X_VER_GTE_06_00
/* OK */
#else
#error "Only with SE051 based build"
#endif

#ifndef NEWLINE
#define NEWLINE must be already defined
#endif

smStatus_t Se05x_API_AeadOneShot(pSe05xSession_t session_ctx,
    uint32_t objectID,
    SE05x_CipherMode_t cipherMode,
    const uint8_t *pInputData,
    size_t inputDataLen,
    const uint8_t *pAad,
    size_t aadLen,
    uint8_t *pIV,
    size_t IVLen,
    uint8_t *pTagData,
    size_t *pTagDataLen,
    uint8_t *pOutputData,
    size_t *pOutputDataLen,
    const SE05x_Cipher_Oper_OneShot_t operation)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_AEAD, operation}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
    SE05x_Result_t result;
    uint16_t ivlen16 = (uint16_t)IVLen;
    size_t ivlen32   = IVLen;
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "AeadOneShot []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U32("objectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
#if SSS_HAVE_SE05X_VER_GTE_07_02
    tlvRet = TLVSET_CipherMode("cipherMode",
        &pCmdbuf,
        &cmdbufLen,
        kSE05x_TAG_2,
        ((cipherMode == kSE05x_CipherMode_AES_GCM_INT_IV) ? kSE05x_CipherMode_AES_GCM : cipherMode));
#else
    tlvRet = TLVSET_CipherMode("cipherMode", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cipherMode);
#endif
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("inputData", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, pInputData, inputDataLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("AdditionalData", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, pAad, aadLen);
    if (0 != tlvRet) {
        goto cleanup;
    }

    if ((cipherMode == kSE05x_CipherMode_AES_GCM) ||
        ((cipherMode == kSE05x_CipherMode_AES_GCM_INT_IV) && (operation == kSE05x_Cipher_Oper_OneShot_Decrypt))) {
        tlvRet = TLVSET_u8bufOptional("IV", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, pIV, IVLen);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    else {
        tlvRet = TLVSET_U16("IVLen", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, ivlen16);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    if (operation == kSE05x_Cipher_Oper_OneShot_Decrypt) {
        tlvRet = TLVSET_u8bufOptional("tag", &pCmdbuf, &cmdbufLen, kSE05x_TAG_6, pTagData, *pTagDataLen);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    retStatus = DoAPDUTxRx_s_Case4_ext(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        if (inputDataLen != 0) {
            tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, pOutputData, pOutputDataLen);
            if (0 != tlvRet) {
                goto cleanup;
            }
        }
        if (operation == kSE05x_Cipher_Oper_OneShot_Encrypt) {
            tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, pTagData, pTagDataLen);
            if (0 != tlvRet) {
                goto cleanup;
            }
        }
        if (operation == kSE05x_Cipher_Oper_OneShot_Decrypt) {
            tlvRet = tlvGet_Result(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, &result);
            if (0 != tlvRet) {
                goto cleanup;
            }
            if (result != kSE05x_Result_SUCCESS) {
                goto cleanup;
            }
        }
        if ((operation == kSE05x_Cipher_Oper_OneShot_Encrypt) && (cipherMode == kSE05x_CipherMode_AES_GCM_INT_IV)) {
            tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_3, pIV, &ivlen32);
            if (0 != tlvRet) {
                goto cleanup;
            }
        }
        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }
cleanup:
    return retStatus;
}

smStatus_t Se05x_API_AeadInit(pSe05xSession_t session_ctx,
    uint32_t objectID,
    SE05x_CipherMode_t cipherMode,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pIV,
    size_t IVLen,
    const SE05x_Cipher_Oper_t operation)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_AEAD, operation}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
    uint16_t ivlen16                       = (uint16_t)IVLen;
    size_t ivlen32                         = IVLen;

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "AeadInit []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U32("objectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    if ((cipherMode == kSE05x_CipherMode_AES_GCM) ||
        ((cipherMode == kSE05x_CipherMode_AES_GCM_INT_IV) && (operation == kSE05x_Cipher_Oper_Decrypt))) {
        tlvRet = TLVSET_u8bufOptional("IV", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, pIV, IVLen);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    else {
        tlvRet = TLVSET_U16("IVLen", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, ivlen16);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    if ((cipherMode == kSE05x_CipherMode_AES_GCM) ||
        ((cipherMode == kSE05x_CipherMode_AES_GCM_INT_IV) && (operation == kSE05x_Cipher_Oper_Decrypt))) {
        retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);
    }
    else {
        retStatus = DoAPDUTxRx_s_Case4_ext(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
        if (retStatus == SM_OK) {
            retStatus       = SM_NOT_OK;
            size_t rspIndex = 0;
#if SSS_HAVE_SE05X_VER_GTE_07_02
            tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_3, pIV, &ivlen32);
#else
            tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, pIV, &ivlen32);
#endif
            if (0 != tlvRet) {
                goto cleanup;
            }
            if ((rspIndex + 2) == rspbufLen) {
                retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
            }
        }
    }
cleanup:
    return retStatus;
}

smStatus_t Se05x_API_AeadCCMInit(pSe05xSession_t session_ctx,
    uint32_t objectID,
    SE05x_CipherMode_t cipherMode,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pIV,
    size_t IVLen,
    size_t aadLen,
    size_t payloadLen,
    size_t tagLen,
    const SE05x_Cipher_Oper_t operation)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_AEAD, operation}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint16_t aadLen16                      = (uint16_t)aadLen;
    uint16_t payloadLen16                  = (uint16_t)payloadLen;
    uint16_t tagLen16                      = (uint16_t)tagLen;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
    uint16_t ivlen16                       = (uint16_t)IVLen;
    size_t ivlen32                         = IVLen;

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "AeadCCMInit []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U32("objectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    if ((cipherMode == kSE05x_CipherMode_AES_CCM) ||
        ((cipherMode == kSE05x_CipherMode_AES_CCM_INT_IV) && (operation == kSE05x_Cipher_Oper_Decrypt))) {
        tlvRet = TLVSET_u8bufOptional("IV", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, pIV, IVLen);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    else {
        tlvRet = TLVSET_U16("IVLen", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, ivlen16);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    tlvRet = TLVSET_U16("aadLen", &pCmdbuf, &cmdbufLen, kSE05x_TAG_6, aadLen16);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U16("payloadLen", &pCmdbuf, &cmdbufLen, kSE05x_TAG_7, payloadLen16);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U16("tagLen", &pCmdbuf, &cmdbufLen, kSE05x_TAG_8, tagLen16);
    if (0 != tlvRet) {
        goto cleanup;
    }
    if ((cipherMode == kSE05x_CipherMode_AES_CCM) ||
        ((cipherMode == kSE05x_CipherMode_AES_CCM_INT_IV) && (operation == kSE05x_Cipher_Oper_Decrypt))) {
        retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);
    }
    else {
        retStatus = DoAPDUTxRx_s_Case4_ext(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
        if (retStatus == SM_OK) {
            retStatus       = SM_NOT_OK;
            size_t rspIndex = 0;
            tlvRet          = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_3, pIV, &ivlen32);
            if (0 != tlvRet) {
                goto cleanup;
            }
            if ((rspIndex + 2) == rspbufLen) {
                retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
            }
        }
    }

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_AeadUpdate_aad(
    pSe05xSession_t session_ctx, SE05x_CryptoObjectID_t cryptoObjectID, const uint8_t *pAadData, size_t aadDataLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_AEAD, kSE05x_P2_UPDATE}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "AeadUpdate_aad []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("aad", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, pAadData, aadDataLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);
cleanup:
    return retStatus;
}

smStatus_t Se05x_API_AeadUpdate(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    const uint8_t *pInputData,
    size_t inputDataLen,
    uint8_t *pOutputData,
    size_t *pOutputLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_AEAD, kSE05x_P2_UPDATE}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "AeadUpdate []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("inputData", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, pInputData, inputDataLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTxRx_s_Case4_ext(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, pOutputData, pOutputLen);
        if (0 != tlvRet) {
            goto cleanup;
        }
        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }
cleanup:
    return retStatus;
}

smStatus_t Se05x_API_AeadCCMLastUpdate(
    pSe05xSession_t session_ctx, SE05x_CryptoObjectID_t cryptoObjectID, const uint8_t *pInputData, size_t inputDataLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_AEAD, kSE05x_P2_UPDATE}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "AeadUpdate []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("inputData", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, pInputData, inputDataLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);
cleanup:
    return retStatus;
}

smStatus_t Se05x_API_AeadCCMFinal(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pOutputData,
    size_t *pOutputLen,
    uint8_t *pTag,
    size_t *pTagLen,
    const SE05x_Cipher_Oper_t operation)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_AEAD, kSE05x_P2_FINAL}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
    size_t tagLen                          = *pTagLen;
    SE05x_Result_t result;

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "AeadFinal []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    if (operation == kSE05x_Cipher_Oper_Decrypt) {
        tlvRet = TLVSET_u8bufOptional("tag", &pCmdbuf, &cmdbufLen, kSE05x_TAG_6, pTag, tagLen);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    retStatus = DoAPDUTxRx_s_Case4_ext(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, pOutputData, pOutputLen);
        if (0 != tlvRet) {
            goto cleanup;
        }
        if (operation == kSE05x_Cipher_Oper_Encrypt) {
            tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, pTag, pTagLen);
            if (0 != tlvRet) {
                goto cleanup;
            }
        }
        if (operation == kSE05x_Cipher_Oper_Decrypt) {
            tlvRet = tlvGet_Result(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, &result);
            if (0 != tlvRet) {
                goto cleanup;
            }

            if (result != kSE05x_Result_SUCCESS) {
                goto cleanup;
            }
        }

        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }
cleanup:
    return retStatus;
}

smStatus_t Se05x_API_AeadFinal(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pTag,
    size_t *pTagLen,
    const SE05x_Cipher_Oper_t operation)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_AEAD, kSE05x_P2_FINAL}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
    size_t tagLen                          = *pTagLen;
    SE05x_Result_t result;

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "AeadFinal []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    if (operation == kSE05x_Cipher_Oper_Decrypt) {
        tlvRet = TLVSET_u8bufOptional("tag", &pCmdbuf, &cmdbufLen, kSE05x_TAG_6, pTag, tagLen);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    retStatus = DoAPDUTxRx_s_Case4_ext(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        if (operation == kSE05x_Cipher_Oper_Encrypt) {
            tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, pTag, pTagLen);
            if (0 != tlvRet) {
                goto cleanup;
            }
        }
        if (operation == kSE05x_Cipher_Oper_Decrypt) {
            tlvRet = tlvGet_Result(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, &result);
            if (0 != tlvRet) {
                goto cleanup;
            }
            if (result != kSE05x_Result_SUCCESS) {
                goto cleanup;
            }
        }
        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }
cleanup:
    return retStatus;
}

smStatus_t Se05x_API_DisableObjCreation(
    pSe05xSession_t session_ctx, SE05x_LockIndicator_t lockIndicator, SE05x_RestrictMode_t restrictMode)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_MGMT, kSE05x_P1_DEFAULT, kSE05x_P2_RESTRICT}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "DisableObjCreation []");
#endif /* VERBOSE_APDU_LOGS */

    tlvRet = TLVSET_U8("lockIndicator", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, lockIndicator);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U8("restrictMode", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, restrictMode);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_TriggerSelfTest(
    pSe05xSession_t session_ctx, SE05x_HealthCheckMode_t healthCheckMode, uint8_t *result)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_MGMT, kSE05x_P1_DEFAULT, kSE05x_P2_SANITY}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "TriggerSelfTest []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U16("health check mode", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, healthCheckMode);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_U8(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, result); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }

cleanup:
    return retStatus;
}

#if SSS_HAVE_SE05X_VER_GTE_07_02

smStatus_t Se05x_API_TriggerSelfTest_W_Attst(pSe05xSession_t session_ctx,
    SE05x_HealthCheckMode_t healthCheckMode,
    uint32_t attestID,
    SE05x_AttestationAlgo_t attestAlgo,
    const uint8_t *random,
    size_t randomLen,
    uint8_t *result,
    SE05x_TimeStamp_t *ptimeStamp,
    uint8_t *chipId,
    size_t *pchipIdLen,
    uint8_t *signature,
    size_t *psignatureLen,
    uint8_t *pObjectSize,
    size_t *pObjectSizeLen,
    uint8_t *pCmd,
    size_t *pCmdLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_MGMT | kSE05x_INS_ATTEST, kSE05x_P1_DEFAULT, kSE05x_P2_SANITY}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "TriggerSelfTest []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U16("health check mode", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, healthCheckMode);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("attestID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, attestID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U8("attestAlgo", &pCmdbuf, &cmdbufLen, kSE05x_TAG_6, attestAlgo);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8buf("random", &pCmdbuf, &cmdbufLen, kSE05x_TAG_7, random, randomLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    memcpy(pCmd, hdr.hdr, sizeof(hdr.hdr));
    *(pCmd + sizeof(hdr.hdr)) = (uint8_t)cmdbufLen;
    if (cmdbufLen == 0) {
        goto cleanup;
    }
    memcpy(pCmd + sizeof(hdr.hdr) + 1, cmdbuf, cmdbufLen);
    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        *pCmdLen        = sizeof(hdr.hdr) + 1 + cmdbufLen;
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_U8(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, result); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, chipId, pchipIdLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_4, pObjectSize, pObjectSizeLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_TimeStamp(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_TIMESTAMP, ptimeStamp); /* - */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_SIGNATURE, signature, psignatureLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }

cleanup:
    return retStatus;
}

#else

smStatus_t Se05x_API_TriggerSelfTest_W_Attst(pSe05xSession_t session_ctx,
    SE05x_HealthCheckMode_t healthCheckMode,
    uint32_t attestID,
    SE05x_AttestationAlgo_t attestAlgo,
    const uint8_t *random,
    size_t randomLen,
    uint8_t *result,
    SE05x_TimeStamp_t *ptimeStamp,
    uint8_t *outrandom,
    size_t *poutrandomLen,
    uint8_t *chipId,
    size_t *pchipIdLen,
    uint8_t *signature,
    size_t *psignatureLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr = {{kSE05x_CLA, kSE05x_INS_MGMT | kSE05x_INS_ATTEST, kSE05x_P1_DEFAULT, kSE05x_P2_SANITY}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf = &rspbuf[0];
    size_t rspbufLen = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "TriggerSelfTest []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U16("health check mode", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, healthCheckMode);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("attestID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, attestID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U8("attestAlgo", &pCmdbuf, &cmdbufLen, kSE05x_TAG_6, attestAlgo);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8buf("random", &pCmdbuf, &cmdbufLen, kSE05x_TAG_7, random, randomLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet = tlvGet_U8(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, result); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_TimeStamp(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_3, ptimeStamp); /* - */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_4, outrandom, poutrandomLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_5, chipId, pchipIdLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_6, signature, psignatureLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }

cleanup:
    return retStatus;
}

#endif // SSS_HAVE_SE05X_VER_GTE_07_02

smStatus_t Se05x_API_ReadObjectAttributes(
    pSe05xSession_t session_ctx, uint32_t objectID, uint8_t *data, size_t *pdataLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_READ, kSE05x_P1_DEFAULT, kSE05x_P2_ATTRIBUTES}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "ReadObjectAttributes []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
#if !SSS_HAVE_SE05X_VER_GTE_07_02
        //Backward compataibility
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, data, pdataLen); /*  */
#else
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_3, data, pdataLen); /*  */
#endif
        if (0 != tlvRet) {
            goto cleanup;
        }
        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }

cleanup:
    return retStatus;
}

#if SSS_HAVE_SE05X_VER_GTE_07_02
smStatus_t Se05x_API_ReadObjectAttributes_W_Attst_V2(pSe05xSession_t session_ctx,
    uint32_t objectID,
    uint32_t attestID,
    SE05x_AttestationAlgo_t attestAlgo,
    const uint8_t *random,
    size_t randomLen,
    uint8_t *pCmdapdu,
    size_t *pCmdapduLen,
    uint8_t *data,
    size_t *pdataLen,
    SE05x_TimeStamp_t *ptimeStamp,
    uint8_t *obj,
    size_t *pobjLen,
    uint8_t *chipId,
    size_t *pchipIdLen,
    uint8_t *signature,
    size_t *psignatureLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_READ_With_Attestation, kSE05x_P1_DEFAULT, kSE05x_P2_ATTRIBUTES}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_ReadObjectAttributes_W_Attst_V2 []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("attestID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, attestID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_AttestationAlgo("attestAlgo", &pCmdbuf, &cmdbufLen, kSE05x_TAG_6, attestAlgo);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("random", &pCmdbuf, &cmdbufLen, kSE05x_TAG_7, random, randomLen);
    if (0 != tlvRet) {
        goto cleanup;
    }

    memcpy(pCmdapdu, &hdr, 4);
    //As length is extended
    pCmdapdu[4] = 0x00;
    pCmdapdu[5] = 0x00;
    if (0 == cmdbufLen) {
        goto cleanup;
    }

    pCmdapdu[6] = (uint8_t)cmdbufLen;
    memcpy(pCmdapdu + 7, cmdbuf, cmdbufLen);
    retStatus = DoAPDUTxRx_s_Case4_ext(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        *pCmdapduLen    = cmdbufLen + 7;
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, chipId, pchipIdLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_3, data, pdataLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_4, obj, pobjLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_TimeStamp(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_TIMESTAMP, ptimeStamp); /* - */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_SIGNATURE, signature, psignatureLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }
    else {
        *pCmdapduLen = 0;
    }

cleanup:
    return retStatus;
}

#else
smStatus_t Se05x_API_ReadObjectAttributes_W_Attst(pSe05xSession_t session_ctx,
    uint32_t objectID,
    uint32_t attestID,
    SE05x_AttestationAlgo_t attestAlgo,
    const uint8_t *random,
    size_t randomLen,
    uint8_t *data,
    size_t *pdataLen,
    SE05x_TimeStamp_t *ptimeStamp,
    uint8_t *outrandom,
    size_t *poutrandomLen,
    uint8_t *chipId,
    size_t *pchipIdLen,
    uint8_t *signature,
    size_t *psignatureLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr = {{kSE05x_CLA, kSE05x_INS_READ_With_Attestation, kSE05x_P1_DEFAULT, kSE05x_P2_ATTRIBUTES}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP];
    uint8_t *pRspbuf = &rspbuf[0];
    size_t rspbufLen = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "ReadObjectAttributes_W_Attst []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("attestID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, attestID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_AttestationAlgo("attestAlgo", &pCmdbuf, &cmdbufLen, kSE05x_TAG_6, attestAlgo);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("random", &pCmdbuf, &cmdbufLen, kSE05x_TAG_7, random, randomLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTxRx_s_Case4_ext(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, data, pdataLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_TimeStamp(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_3, ptimeStamp); /* - */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_4, outrandom, poutrandomLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_5, chipId, pchipIdLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_6, signature, psignatureLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }

cleanup:
    return retStatus;
}
#endif

smStatus_t Se05x_API_ECDHGenerateSharedSecret_InObject(pSe05xSession_t session_ctx,
    uint32_t objectID,
    const uint8_t *pubKey,
    size_t pubKeyLen,
    uint32_t sharedSecretID,
    uint8_t invertEndianness)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {
        {kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_EC, invertEndianness == 0x01 ? kSE05x_P2_DH_REVERSE : kSE05x_P2_DH}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "ECDHGenerateSharedSecret_InObject []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U32("objectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("pubKey", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, pubKey, pubKeyLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("sharedSecretID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_7, sharedSecretID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus = SM_NOT_OK;
        if (2 == rspbufLen) {
            retStatus = (smStatus_t)((rspbuf[0] << 8) | (rspbuf[1]));
        }
    }

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_TLSCalculateRsaPreMasterSecret(pSe05xSession_t session_ctx,
    uint32_t keyPairId,
    uint32_t pskId,
    uint32_t hmacKeyId,
    const uint8_t *inputData,
    size_t inputDataLen,
    const uint8_t *clientVersion,
    size_t clientVersionLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_TLS, kSE05x_P2_TLS_PMS}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "TLSCalculateRsaPreMasterSecret []");
#endif /* VERBOSE_APDU_LOGS */
    if (pskId != 0) {
        tlvRet = TLVSET_U32("pskId", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, pskId);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    tlvRet = TLVSET_U32("keyPairId", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, keyPairId);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("hmacKeyId", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, hmacKeyId);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("inputData", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, inputData, inputDataLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("clientVersion", &pCmdbuf, &cmdbufLen, kSE05x_TAG_6, clientVersion, clientVersionLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_WriteRSAKey_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    uint32_t objectID,
    uint16_t size,
    const uint8_t *p,
    size_t pLen,
    const uint8_t *q,
    size_t qLen,
    const uint8_t *dp,
    size_t dpLen,
    const uint8_t *dq,
    size_t dqLen,
    const uint8_t *qInv,
    size_t qInvLen,
    const uint8_t *pubExp,
    size_t pubExpLen,
    const uint8_t *priv,
    size_t privLen,
    const uint8_t *pubMod,
    size_t pubModLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    const SE05x_RSAKeyFormat_t rsa_format,
    uint32_t version)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_WRITE | ins_type, kSE05x_P1_RSA | key_part, rsa_format}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

    if (Se05x_IsInValidRangeOfUID(objectID)) {
        return SM_NOT_OK;
    }

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_WriteRSAKey_Ver []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_Se05xPolicy("To be Checked(last 3 not pdf)", &pCmdbuf, &cmdbufLen, kSE05x_TAG_POLICY, policy);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U16Optional("size in bits", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, size);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("p", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, p, pLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("q", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, q, qLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("dp", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, dp, dpLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("dq", &pCmdbuf, &cmdbufLen, kSE05x_TAG_6, dq, dqLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("qnv", &pCmdbuf, &cmdbufLen, kSE05x_TAG_7, qInv, qInvLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("public exp", &pCmdbuf, &cmdbufLen, kSE05x_TAG_8, pubExp, pubExpLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("priv", &pCmdbuf, &cmdbufLen, kSE05x_TAG_9, priv, privLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("public mod", &pCmdbuf, &cmdbufLen, kSE05x_TAG_10, pubMod, pubModLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("version", &pCmdbuf, &cmdbufLen, kSE05x_TAG_11, version);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_WriteECKey_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_ECCurve_t curveID,
    const uint8_t *privKey,
    size_t privKeyLen,
    const uint8_t *pubKey,
    size_t pubKeyLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    uint32_t version)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_WRITE | ins_type, kSE05x_P1_EC | key_part, kSE05x_P2_DEFAULT}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

    if (Se05x_IsInValidRangeOfUID(objectID)) {
        return SM_NOT_OK;
    }

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_WriteECKey_Ver []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_Se05xPolicy("policy", &pCmdbuf, &cmdbufLen, kSE05x_TAG_POLICY, policy);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_MaxAttemps("maxAttempt", &pCmdbuf, &cmdbufLen, kSE05x_TAG_MAX_ATTEMPTS, maxAttempt);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_ECCurve("curveID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, curveID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("privKey", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, privKey, privKeyLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("pubKey", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, pubKey, pubKeyLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("version", &pCmdbuf, &cmdbufLen, kSE05x_TAG_11, version);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_WriteSymmKey_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_KeyID_t kekID,
    const uint8_t *keyValue,
    size_t keyValueLen,
    const SE05x_INS_t ins_type,
    const SE05x_SymmKeyType_t type,
    uint32_t version)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_WRITE | ins_type, type, kSE05x_P2_DEFAULT}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

    if (Se05x_IsInValidRangeOfUID(objectID)) {
        return SM_NOT_OK;
    }

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_WriteSymmKey_Ver []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_Se05xPolicy("policy", &pCmdbuf, &cmdbufLen, kSE05x_TAG_POLICY, policy);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_MaxAttemps("maxAttempt", &pCmdbuf, &cmdbufLen, kSE05x_TAG_MAX_ATTEMPTS, maxAttempt);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_KeyID("KEK id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, kekID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("key value", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, keyValue, keyValueLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
#if SSS_HAVE_SE05X_VER_GTE_07_02
    if (type == kSE05x_SymmKeyType_HMAC) {
        tlvRet = TLVSET_U16(
            "Min output length", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, SE05X_MINIMUM_KEY_DERIVATION_OUTPUT_LEN);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
#endif
    tlvRet = TLVSET_U32("version", &pCmdbuf, &cmdbufLen, kSE05x_TAG_11, version);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_WriteBinary_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    uint32_t objectID,
    uint16_t offset,
    uint16_t length,
    const uint8_t *inputData,
    size_t inputDataLen,
    uint32_t version)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_WRITE, kSE05x_P1_BINARY, kSE05x_P2_DEFAULT}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

    if (Se05x_IsInValidRangeOfUID(objectID)) {
        return SM_NOT_OK;
    }

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_WriteBinary_Ver []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_Se05xPolicy("policy", &pCmdbuf, &cmdbufLen, kSE05x_TAG_POLICY, policy);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U16Optional("offset", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, offset);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U16Optional("length", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, length);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("input data", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, inputData, inputDataLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("version", &pCmdbuf, &cmdbufLen, kSE05x_TAG_11, version);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_ReadState(pSe05xSession_t session_ctx, uint8_t *pstateValues, size_t *pstateValuesLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_READ, kSE05x_P1_DEFAULT, kSE05x_P2_READ_STATE}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "ReadState []");
#endif /* VERBOSE_APDU_LOGS */
    retStatus = DoAPDUTxRx_s_Case2(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, pstateValues, pstateValuesLen); /* - */
        if (0 != tlvRet) {
            goto cleanup;
        }
        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_GetExtVersion(pSe05xSession_t session_ctx, uint8_t *pappletVersion, size_t *appletVersionLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_MGMT, kSE05x_P1_DEFAULT, kSE05x_P2_VERSION_EXT}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "GetVersion []");
#endif /* VERBOSE_APDU_LOGS */
    retStatus = DoAPDUTxRx_s_Case2(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, pappletVersion, appletVersionLen); /* - */
        if (0 != tlvRet) {
            goto cleanup;
        }
        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_SendCardManagerCmd(
    pSe05xSession_t session_ctx, uint8_t *pCmdData, size_t cmdDataLen, uint8_t *pOutputData, size_t *pOutputDataLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_MGMT, kSE05x_P1_DEFAULT, kSE05x_P2_CM_COMMAND}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "SendCardManagerCmd []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_u8bufOptional("cmdData", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, pCmdData, cmdDataLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, pOutputData, pOutputDataLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_UpdateRSAKey_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    uint32_t objectID,
    uint16_t size,
    const uint8_t *p,
    size_t pLen,
    const uint8_t *q,
    size_t qLen,
    const uint8_t *dp,
    size_t dpLen,
    const uint8_t *dq,
    size_t dqLen,
    const uint8_t *qInv,
    size_t qInvLen,
    const uint8_t *pubExp,
    size_t pubExpLen,
    const uint8_t *priv,
    size_t privLen,
    const uint8_t *pubMod,
    size_t pubModLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    const SE05x_RSAKeyFormat_t rsa_format,
    uint32_t version)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_WRITE | ins_type, kSE05x_P1_RSA | key_part, rsa_format}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

    if (Se05x_IsInValidRangeOfUID(objectID)) {
        return SM_NOT_OK;
    }

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_UpdateRSAKey_Ver []");
#endif /* VERBOSE_APDU_LOGS */
    /* Tag policy Check is not applicable for Generate key */
    if (((p != NULL) && (pLen != 0)) || ((pubExp != NULL) && (pubExpLen != 0)) || ((priv != NULL) && (privLen != 0))) {
        tlvRet = TLVSET_Se05xPolicy("check policy", &pCmdbuf, &cmdbufLen, kSE05x_TAG_POLICY_CHECK, policy);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U16Optional("size in bits", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, size);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("p", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, p, pLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("q", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, q, qLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("dp", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, dp, dpLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("dq", &pCmdbuf, &cmdbufLen, kSE05x_TAG_6, dq, dqLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("qnv", &pCmdbuf, &cmdbufLen, kSE05x_TAG_7, qInv, qInvLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("public exp", &pCmdbuf, &cmdbufLen, kSE05x_TAG_8, pubExp, pubExpLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("priv", &pCmdbuf, &cmdbufLen, kSE05x_TAG_9, priv, privLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional_ByteShift("public mod", &pCmdbuf, &cmdbufLen, kSE05x_TAG_10, pubMod, pubModLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("version", &pCmdbuf, &cmdbufLen, kSE05x_TAG_11, version);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_UpdateECKey_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_ECCurve_t curveID,
    const uint8_t *privKey,
    size_t privKeyLen,
    const uint8_t *pubKey,
    size_t pubKeyLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    uint32_t version)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_WRITE | ins_type, kSE05x_P1_EC | key_part, kSE05x_P2_DEFAULT}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

    if (Se05x_IsInValidRangeOfUID(objectID)) {
        return SM_NOT_OK;
    }

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_UpdateECKey_Ver []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_MaxAttemps("maxAttempt", &pCmdbuf, &cmdbufLen, kSE05x_TAG_MAX_ATTEMPTS, maxAttempt);
    if (0 != tlvRet) {
        goto cleanup;
    }

    /* Tag policy Check is not applicable for Generate key */
    if (((privKey != NULL) && (privKeyLen != 0)) || ((pubKey != NULL) && (pubKeyLen != 0))) {
        tlvRet = TLVSET_Se05xPolicy("check policy", &pCmdbuf, &cmdbufLen, kSE05x_TAG_POLICY_CHECK, policy);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }

    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_ECCurve("curveID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, curveID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("privKey", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, privKey, privKeyLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("pubKey", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, pubKey, pubKeyLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("version", &pCmdbuf, &cmdbufLen, kSE05x_TAG_11, version);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_UpdateBinary_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    uint32_t objectID,
    uint16_t offset,
    uint16_t length,
    const uint8_t *inputData,
    size_t inputDataLen,
    uint32_t version)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_WRITE, kSE05x_P1_BINARY, kSE05x_P2_DEFAULT}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

    if (Se05x_IsInValidRangeOfUID(objectID)) {
        return SM_NOT_OK;
    }

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_UpdateBinary_Ver []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_Se05xPolicy("check policy", &pCmdbuf, &cmdbufLen, kSE05x_TAG_POLICY_CHECK, policy);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U16Optional("offset", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, offset);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U16Optional("length", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, length);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("input data", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, inputData, inputDataLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("version", &pCmdbuf, &cmdbufLen, kSE05x_TAG_11, version);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_UpdateSymmKey_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_KeyID_t kekID,
    const uint8_t *keyValue,
    size_t keyValueLen,
    const SE05x_INS_t ins_type,
    const SE05x_SymmKeyType_t type,
    uint32_t version)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_WRITE | ins_type, type, kSE05x_P2_DEFAULT}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

    if (Se05x_IsInValidRangeOfUID(objectID)) {
        return SM_NOT_OK;
    }

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_UpdateSymmKey_Ver []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_MaxAttemps("maxAttempt", &pCmdbuf, &cmdbufLen, kSE05x_TAG_MAX_ATTEMPTS, maxAttempt);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_Se05xPolicy("check policy", &pCmdbuf, &cmdbufLen, kSE05x_TAG_POLICY_CHECK, policy);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_KeyID("KEK id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, kekID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("key value", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, keyValue, keyValueLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("version", &pCmdbuf, &cmdbufLen, kSE05x_TAG_11, version);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_UpdatePCR(
    pSe05xSession_t session_ctx, pSe05xPolicy_t policy, uint32_t pcrID, const uint8_t *inputData, size_t inputDataLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_WRITE, kSE05x_P1_PCR, kSE05x_P2_DEFAULT}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

    if (Se05x_IsInValidRangeOfUID(pcrID)) {
        return SM_NOT_OK;
    }

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_UpdatePCR []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_Se05xPolicy("check policy", &pCmdbuf, &cmdbufLen, kSE05x_TAG_POLICY_CHECK, policy);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, pcrID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("inputData", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, inputData, inputDataLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_UpdateCounter(
    pSe05xSession_t session_ctx, pSe05xPolicy_t policy, uint32_t objectID, uint16_t size, uint64_t value)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_WRITE, kSE05x_P1_COUNTER, kSE05x_P2_DEFAULT}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

    if (Se05x_IsInValidRangeOfUID(objectID)) {
        return SM_NOT_OK;
    }

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_CreateCounter []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_Se05xPolicy("policy check", &pCmdbuf, &cmdbufLen, kSE05x_TAG_POLICY_CHECK, policy);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }

    if ((size > 0) && (size <= 8)) {
        if (value != 0) {
            tlvRet = TLVSET_U64_SIZE("value", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, value, size);
            if (0 != tlvRet) {
                goto cleanup;
            }
        }
    }
    else {
        LOG_E("Wrong size provided");
        goto cleanup;
    }

    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_WriteSymmKey_Ver_extended(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_KeyID_t kekID,
    const uint8_t *keyValue,
    size_t keyValueLen,
    const SE05x_INS_t ins_type,
    const SE05x_SymmKeyType_t type,
    uint32_t version,
    uint16_t min_aead_tag_len)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_WRITE | ins_type, type, kSE05x_P2_DEFAULT}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

    if (Se05x_IsInValidRangeOfUID(objectID)) {
        return SM_NOT_OK;
    }

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_WriteSymmKey_Ver_extended []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_Se05xPolicy("policy", &pCmdbuf, &cmdbufLen, kSE05x_TAG_POLICY, policy);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_MaxAttemps("maxAttempt", &pCmdbuf, &cmdbufLen, kSE05x_TAG_MAX_ATTEMPTS, maxAttempt);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("object id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_KeyID("KEK id", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, kekID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("key value", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, keyValue, keyValueLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    if (type == kSE05x_SymmKeyType_AES) {
        tlvRet = TLVSET_U16("Min tag length for AEAD", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, min_aead_tag_len);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
#if defined(SSS_HAVE_SE05X_VER_GTE_06_18) && (SSS_HAVE_SE05X_VER_GTE_06_18 == 1)
    if (type == kSE05x_SymmKeyType_HMAC) {
        tlvRet = TLVSET_U16(
            "Min output length", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, SE05X_MINIMUM_KEY_DERIVATION_OUTPUT_LEN);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
#endif
    tlvRet = TLVSET_U32("version", &pCmdbuf, &cmdbufLen, kSE05x_TAG_11, version);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

#if SSS_HAVE_SE05X_VER_GTE_07_02
smStatus_t Se05x_API_PBKDF2_extended(pSe05xSession_t session_ctx,
    uint32_t objectID,
    const uint8_t *salt,
    size_t saltLen,
    uint32_t saltID,
    uint16_t count,
    SE05x_MACAlgo_t macAlgo,
    uint16_t requestedLen,
    uint32_t derivedSessionKeyID,
    uint8_t *derivedSessionKey,
    size_t *pderivedSessionKeyLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_DEFAULT, kSE05x_P2_PBKDF}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "PBKDF2 []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U32(
        "4-byte password identifier (object type must be HMACKey)", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    if (salt != NULL) {
        tlvRet = TLVSET_u8bufOptional("salt", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, salt, saltLen);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    tlvRet = TLVSET_U16("count", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, count);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U16("requestedLen", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, requestedLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U8("MacAlgo", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, macAlgo);
    if (0 != tlvRet) {
        goto cleanup;
    }
    // Warning: TAGS must be in numerical order, so this cannot be the else statement of (salt != null)
    if (salt == NULL) {
        tlvRet = TLVSET_U32("saltID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_6, saltID);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    if (derivedSessionKey == NULL) {
        tlvRet = TLVSET_U32("derivedSessionKeyID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_7, derivedSessionKeyID);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        if (derivedSessionKey == NULL) {
            retStatus = SM_NOT_OK;
            if (2 == rspbufLen) {
                retStatus = (smStatus_t)((rspbuf[0] << 8) | (rspbuf[1]));
            }
        }
        else {
            retStatus       = SM_NOT_OK;
            size_t rspIndex = 0;
            tlvRet =
                tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, derivedSessionKey, pderivedSessionKeyLen);
            if (0 != tlvRet) {
                goto cleanup;
            }
            if ((rspIndex + 2) == rspbufLen) {
                retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
            }
        }
    }

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_ECDHGenerateSharedSecret_InObject_extended(pSe05xSession_t session_ctx,
    uint32_t objectID,
    const uint8_t *pubKey,
    size_t pubKeyLen,
    SE05x_ECDHAlgo_t ecdhAlgo,
    uint32_t sharedSecretID,
    uint8_t invertEndianness)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {
        {kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_EC, invertEndianness == 0x01 ? kSE05x_P2_DH_REVERSE : kSE05x_P2_DH}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "ECDHGenerateSharedSecret_InObject []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U32("objectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_u8bufOptional("pubKey", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, pubKey, pubKeyLen);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U8("ecdhAlgo", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, ecdhAlgo);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("sharedSecretID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_7, sharedSecretID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus = SM_NOT_OK;
        if (2 == rspbufLen) {
            retStatus =(smStatus_t)((pRspbuf[0] << 8) | (pRspbuf[1]));
        }
    }

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_ECPointMultiply_InputObj(pSe05xSession_t session_ctx,
    uint32_t objectID,
    uint32_t pubKeyID,
    uint32_t sharedSecretID,
    uint8_t *sharedSecretOuput,
    size_t *psharedSecretOuputLen,
    SE05x_ECPMAlgo_t ECPMAlgo)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_EC, kSE05x_P2_ECPM}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD];
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_ECPointMultiply_InputObj []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U32("objectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U32("pubKeyID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, pubKeyID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U8("ECPMAlgo", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, ECPMAlgo);
    if (0 != tlvRet) {
        goto cleanup;
    }
    if (sharedSecretOuput == NULL) {
        tlvRet = TLVSET_U32("sharedSecretID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_7, sharedSecretID);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    retStatus = DoAPDUTxRx_s_Case4_ext(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        if (sharedSecretOuput == NULL) {
            retStatus = SM_NOT_OK;
            if (2 == rspbufLen) {
                retStatus = (smStatus_t)((rspbuf[0] << 8) | (rspbuf[1]));
            }
        }
        else {
            retStatus       = SM_NOT_OK;
            size_t rspIndex = 0;
            tlvRet          = tlvGet_u8buf(
                pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, sharedSecretOuput, psharedSecretOuputLen); /*  */
            if (0 != tlvRet) {
                goto cleanup;
            }
            if ((rspIndex + 2) == rspbufLen) {
                retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
            }
        }
    }

cleanup:
    return retStatus;
}

#endif //#if SSS_HAVE_SE05X_VER_GTE_07_02
