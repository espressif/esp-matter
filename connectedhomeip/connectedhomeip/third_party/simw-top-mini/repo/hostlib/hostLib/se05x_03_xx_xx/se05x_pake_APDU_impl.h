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

#if (SSS_HAVE_APPLET_SE051_H && SSS_HAVE_SE05X_VER_07_02)
/* OK */
#else
#error "Only with SE051_H based build"
#endif

#ifndef NEWLINE
#define NEWLINE must be already defined
#endif

smStatus_t Se05x_API_PAKEConfigDevice(
    pSe05xSession_t session_ctx, SE05x_CryptoObjectID_t cryptoObjectID, SE05x_SPAKE2PlusDeviceType_t deviceType)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_PAKE, kSE05x_P2_TYPE}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD] = {0,};
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "PAKEConfigDevice []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_U8("device Type", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, deviceType);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_PAKEInitDevice(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pContext,
    size_t contextLen,
    uint8_t *pIdProver,
    size_t idProverLen,
    uint8_t *pIdVerifier,
    size_t idVerifierLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_PAKE, kSE05x_P2_ID}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD] = {0,};
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "PAKEInitDevice[]");
#endif /* VERBOSE_APDU_LOGS */

    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }

    tlvRet = TLVSET_u8bufOptional("context byteString", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, pContext, contextLen);
    if (0 != tlvRet) {
        goto cleanup;
    }

    tlvRet = TLVSET_u8bufOptional("Label A byteString", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, pIdProver, idProverLen);
    if (0 != tlvRet) {
        goto cleanup;
    }

    tlvRet = TLVSET_u8bufOptional("Label B byteString", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, pIdVerifier, idVerifierLen);
    if (0 != tlvRet) {
        goto cleanup;
    }

    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_PAKEInitCredentials(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint32_t objectID_w0,
    uint32_t objectID_w1,
    uint32_t objectID_L)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_PAKE, kSE05x_P2_PARAM}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD] = {0,};
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;

#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "PAKEInitCredentials[]");
#endif /* VERBOSE_APDU_LOGS */

    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }

    if (objectID_w0 != 0) {
        tlvRet = TLVSET_U32("objectID w0", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, objectID_w0);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }
    if (objectID_w1 != 0) {
        tlvRet = TLVSET_U32("objectID w1", &pCmdbuf, &cmdbufLen, kSE05x_TAG_4, objectID_w1);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }

    if (objectID_L != 0) {
        tlvRet = TLVSET_U32("objectID L", &pCmdbuf, &cmdbufLen, kSE05x_TAG_5, objectID_L);
        if (0 != tlvRet) {
            goto cleanup;
        }
    }

    retStatus = DoAPDUTx_s_Case3(session_ctx, &hdr, cmdbuf, cmdbufLen);

cleanup:
    return retStatus;
}

smStatus_t Se05x_API_PAKEComputeKeyShare(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pInKey,
    size_t inKeyLen,
    uint8_t *pShareKey,
    size_t *pShareKeyLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_PAKE, kSE05x_P2_UPDATE}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD] = {0,};
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0,};
    uint8_t *pRspbuf = &rspbuf[0];
    size_t rspbufLen = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "PAKEComputeKeyShare []");
#endif /* VERBOSE_APDU_LOGS */

    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }

    tlvRet = TLVSET_u8bufOptional("Share key", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, pInKey, inKeyLen);
    if (0 != tlvRet) {
        goto cleanup;
    }

    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, pShareKey, pShareKeyLen);
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

smStatus_t Se05x_API_PAKEComputeSessionKeys(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pInKey,
    size_t inKeyLen,
    uint8_t *pShSecret,
    size_t *pShSecretLen,
    uint8_t *pKeyConfMessage,
    size_t *pkeyConfMessageLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_PAKE, kSE05x_P2_GENERATE}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD] = {0,};
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0,};
    uint8_t *pRspbuf = &rspbuf[0];
    size_t rspbufLen = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "PAKEComputeSessionKeys []");
#endif /* VERBOSE_APDU_LOGS */

    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }

    tlvRet = TLVSET_u8bufOptional("Share key", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, pInKey, inKeyLen);
    if (0 != tlvRet) {
        goto cleanup;
    }

    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, pShSecret, pShSecretLen);
        if (0 != tlvRet) {
            goto cleanup;
        }

        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, pKeyConfMessage, pkeyConfMessageLen);
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

smStatus_t Se05x_API_PAKEVerifySessionKeys(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pKeyConfMessage,
    size_t keyConfMessageLen,
    uint8_t *presult)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_PAKE, kSE05x_P2_VERIFY}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD] = {0,};
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0,};
    uint8_t *pRspbuf = &rspbuf[0];
    size_t rspbufLen = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "PAKEVerifySessionKeys []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }

    tlvRet = TLVSET_u8bufOptional(
        "Key Confirmation message", &pCmdbuf, &cmdbufLen, kSE05x_TAG_3, pKeyConfMessage, keyConfMessageLen);
    if (0 != tlvRet) {
        goto cleanup;
    }

    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus         = SM_NOT_OK;
        size_t rspIndex   = 0;
        size_t presultLen = 1;
        tlvRet            = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, presult, &presultLen);
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

smStatus_t Se05x_API_PAKEReadDeviceType(
    pSe05xSession_t session_ctx, SE05x_CryptoObjectID_t cryptoObjectID, SE05x_SPAKE2PlusDeviceType_t *deviceType)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_READ, kSE05x_P1_PAKE, kSE05x_P2_DEFAULT}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD] = {0,};
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0,};
    uint8_t *pRspbuf = &rspbuf[0];
    size_t rspbufLen = ARRAY_SIZE(rspbuf);
    uint8_t devType  = 0;
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "PAKEReadDeviceType []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }

    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_U8(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, &devType);
        if (0 != tlvRet) {
            goto cleanup;
        }

        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }
    if (deviceType != NULL)
        *deviceType = (SE05x_SPAKE2PlusDeviceType_t)devType;
    else
        retStatus = SM_NOT_OK;
cleanup:
    return retStatus;
}

smStatus_t Se05x_API_PAKEReadState(
    pSe05xSession_t session_ctx, SE05x_CryptoObjectID_t cryptoObjectID, SE05x_PAKEState_t *pakeState)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_READ, kSE05x_P1_PAKE, kSE05x_P2_READ_STATE}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD] = {0,};
    size_t cmdbufLen = 0;
    uint8_t *pCmdbuf = &cmdbuf[0];
    int tlvRet       = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0,};
    uint8_t *pRspbuf = &rspbuf[0];
    size_t rspbufLen = ARRAY_SIZE(rspbuf);
    uint8_t devState = 0;
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "PAKEReadDeviceType []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_CryptoObjectID("cryptoObjectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, cryptoObjectID);
    if (0 != tlvRet) {
        goto cleanup;
    }

    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_U8(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, &devState);
        if (0 != tlvRet) {
            goto cleanup;
        }

        if ((rspIndex + 2) == rspbufLen) {
            retStatus = (smStatus_t)((pRspbuf[rspIndex] << 8) | (pRspbuf[rspIndex + 1]));
        }
    }
    if (pakeState != NULL)
        *pakeState = (SE05x_PAKEState_t)devState;
    else
        retStatus = SM_NOT_OK;
cleanup:
    return retStatus;
}

smStatus_t Se05x_API_ECDSA_Internal_Sign(pSe05xSession_t session_ctx,
    uint32_t objectID,
    SE05x_ECSignatureAlgo_t ecSignAlgo,
    uint8_t *signature,
    size_t *psignatureLen,
    uint8_t *hashData,
    size_t *phashDataLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_SIGNATURE, kSE05x_P2_SIGN}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD] = {0,};
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_ECDSA_Internal_Sign []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U32("objectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_ECSignatureAlgo("ecSignAlgo", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, ecSignAlgo);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, signature, psignatureLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, hashData, phashDataLen); /*  */
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

smStatus_t Se05x_API_RSA_Internal_Sign(pSe05xSession_t session_ctx,
    uint32_t objectID,
    SE05x_RSASignatureAlgo_t rsaSigningAlgo,
    uint8_t *signature,
    size_t *psignatureLen,
    uint8_t *hashData,
    size_t *phashDataLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_SIGNATURE, kSE05x_P2_SIGN}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD] = {0,};
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_RSA_Internal_Sign []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U32("objectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_RSASignatureAlgo("rsaSigningAlgo", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, rsaSigningAlgo);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTxRx_s_Case4_ext(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, signature, psignatureLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, hashData, phashDataLen); /*  */
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

smStatus_t Se05x_API_EdDSA_Internal_Sign(pSe05xSession_t session_ctx,
    uint32_t objectID,
    SE05x_EDSignatureAlgo_t edSignAlgo,
    uint8_t *signature,
    size_t *psignatureLen,
    uint8_t *hashData,
    size_t *phashDataLen)
{
    smStatus_t retStatus = SM_NOT_OK;
    tlvHeader_t hdr      = {{kSE05x_CLA, kSE05x_INS_CRYPTO, kSE05x_P1_SIGNATURE, kSE05x_P2_SIGN}};
    uint8_t cmdbuf[SE05X_MAX_BUF_SIZE_CMD] = {0,};
    size_t cmdbufLen                       = 0;
    uint8_t *pCmdbuf                       = &cmdbuf[0];
    int tlvRet                             = 0;
    uint8_t rspbuf[SE05X_MAX_BUF_SIZE_RSP] = {0};
    uint8_t *pRspbuf                       = &rspbuf[0];
    size_t rspbufLen                       = ARRAY_SIZE(rspbuf);
#if VERBOSE_APDU_LOGS
    NEWLINE();
    nLog("APDU", NX_LEVEL_DEBUG, "Se05x_API_EdDSA_Internal_Sign []");
#endif /* VERBOSE_APDU_LOGS */
    tlvRet = TLVSET_U32("objectID", &pCmdbuf, &cmdbufLen, kSE05x_TAG_1, objectID);
    if (0 != tlvRet) {
        goto cleanup;
    }
    tlvRet = TLVSET_EDSignatureAlgo("edSignAlgo", &pCmdbuf, &cmdbufLen, kSE05x_TAG_2, edSignAlgo);
    if (0 != tlvRet) {
        goto cleanup;
    }
    retStatus = DoAPDUTxRx_s_Case4(session_ctx, &hdr, cmdbuf, cmdbufLen, rspbuf, &rspbufLen);
    if (retStatus == SM_OK) {
        retStatus       = SM_NOT_OK;
        size_t rspIndex = 0;
        tlvRet          = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_1, signature, psignatureLen); /*  */
        if (0 != tlvRet) {
            goto cleanup;
        }
        tlvRet = tlvGet_u8buf(pRspbuf, &rspIndex, rspbufLen, kSE05x_TAG_2, hashData, phashDataLen); /*  */
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
