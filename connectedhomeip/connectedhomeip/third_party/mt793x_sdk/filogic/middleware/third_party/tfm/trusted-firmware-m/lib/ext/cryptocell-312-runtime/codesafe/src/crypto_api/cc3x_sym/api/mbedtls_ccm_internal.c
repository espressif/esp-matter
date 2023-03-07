/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_CC_API

#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_pal_abort.h"
#include "aesccm_driver.h"
#include "cc_common.h"
#include "mbedtls_ccm_internal.h"
#include "mbedtls_ccm_common.h"
#include "mbedtls_common.h"
#include "cc_aesccm_error.h"

/************************ Type definitions **********************/

/* AES-CCM* Security levels (ieee-802.15.4-2011, Table 58) */
#define AESCCM_STAR_SECURITY_LEVEL_ENC          4
#define AESCCM_STAR_SECURITY_LEVEL_ENC_MIC_32   5
#define AESCCM_STAR_SECURITY_LEVEL_ENC_MIC_64   6
#define AESCCM_STAR_SECURITY_LEVEL_ENC_MIC_128  7

/************************ Type definitions **********************/


/************************ static functions **********************/
static int ccm_init(mbedtls_ccm_context *ctx,
                    cryptoDirection_t encryptDecryptFlag,
                    size_t assocDataSize,
                    size_t textDataSize,
                    const uint8_t *pNonce,
                    uint8_t sizeOfN,
                    uint8_t sizeOfT,
                    uint32_t ccmMode)
{

    AesCcmContext_t *pAesCcmCtx = NULL;
    uint8_t ctrStateBuf[CC_AES_BLOCK_SIZE_IN_BYTES] = { 0 };
    uint8_t qFieldSize = 15 - sizeOfN;
    uint8_t *tempBuff;
    drvError_t rc;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;
    uint8_t securityLevelField = 0;

    /* check the Encrypt / Decrypt flag validity */
    if (encryptDecryptFlag >= CRYPTO_DIRECTION_NUM_OF_ENC_MODES)
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* check Nonce pointer */
    if (pNonce == NULL)
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    if (ccmMode == MBEDTLS_AESCCM_MODE_CCM) {
        /* check CCM MAC size: [4,6,8,10,12,14,16] */
        if ((sizeOfT < 4) || (sizeOfT > 16) || ((sizeOfT & 1) != 0)) {
            return MBEDTLS_ERR_CCM_BAD_INPUT;
        }
    }
    else if (ccmMode == MBEDTLS_AESCCM_MODE_STAR) {
        /* check CCM STAR MAC size */
        if (mbedtls_ccm_get_security_level(sizeOfT, &securityLevelField) != CC_OK) {
            return MBEDTLS_ERR_CCM_BAD_INPUT;
        }

        /* check CCM STAR Nonce size. sizeOfN == 13 */
        if (sizeOfN != MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES) {
            return MBEDTLS_ERR_CCM_BAD_INPUT;
        }

        /* check CCM STAR Security level field */
        if (pNonce[MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES - 1] != securityLevelField) {
            return MBEDTLS_ERR_CCM_BAD_INPUT;
        }
    }
    else {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* check the Q field size: according to our implementation qFieldSize <= 4*/
    if ((qFieldSize < 2) || (qFieldSize > 8))
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* check, that actual size of TextData is not great, than its control field size */
    if ((qFieldSize < 4) && ((textDataSize >> (qFieldSize * 8)) > 0))
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* check Nonce size. */
    if (sizeOfN < 7 || sizeOfN >= CC_AES_BLOCK_SIZE_IN_BYTES)
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* set pointer to user context */
    pAesCcmCtx = (AesCcmContext_t *) ctx;

    /* set current AES operation mode for AES_MAC operation on first block B0 and next blocks */
    pAesCcmCtx->mode = CIPHER_CBC_MAC;
    pAesCcmCtx->sizeOfN = sizeOfN;
    pAesCcmCtx->sizeOfT = sizeOfT;
    pAesCcmCtx->dir = encryptDecryptFlag;

    /* clear initial vector */
    mbedtls_zeroize_internal(pAesCcmCtx->ivBuf, sizeof(pAesCcmCtx->ivBuf));

    /*clear intenal buffers*/
    mbedtls_zeroize_internal(pAesCcmCtx->ctrStateBuf, sizeof(pAesCcmCtx->ctrStateBuf));
    mbedtls_zeroize_internal(pAesCcmCtx->tempBuff, sizeof(pAesCcmCtx->tempBuff));

    /* formatting the first block of CCM input B0, and encrypt with CBC-MAC */
    tempBuff = pAesCcmCtx->tempBuff;

    /* set Adata flag (bit 6 for big endian form) */
    if (assocDataSize > 0)
    {
        tempBuff[0] = 1 << 6;
    }

    /* next flag bits: (t-2)/2 concatenated with (q-1) */
    tempBuff[0] |= ((sizeOfT - 2) / 2) << 3; /* bits 3 - 5 */
    tempBuff[0] |= (qFieldSize - 1); /* bits 0 - 2 */

    /* set N and Q (in big endian form) into B0 */
    CC_PalMemCopy(tempBuff + 1, pNonce, sizeOfN);
    CC_CommonReverseMemcpy(tempBuff + 16 - min(qFieldSize, 4), (uint8_t*) &textDataSize, min(qFieldSize, 4));

    /* set data buffers structures */
    rc = SetDataBuffersInfo(tempBuff, CC_AES_BLOCK_SIZE_IN_BYTES, &inBuffInfo,
    NULL,
                            0, &outBuffInfo);
    if (rc != 0)
    {
        CC_PAL_LOG_ERR("illegal data buffers\n");
        return MBEDTLS_ERR_CCM_AUTH_FAILED;
    }

    /* calculating MAC */
    rc = ProcessAesCcmDrv(pAesCcmCtx, &inBuffInfo, &outBuffInfo, CC_AES_BLOCK_SIZE_IN_BYTES);
    if (rc != AES_DRV_OK)
    {
        CC_PAL_LOG_ERR("calculating MAC failed with error code %d\n", rc);
        return MBEDTLS_ERR_CCM_AUTH_FAILED;
    }

    /* set initial Counter value into ctrStateBuf buffer for AES CTR operations on text data */

    /* Flags byte = (qFieldSize - 1) */
    ctrStateBuf[0] = qFieldSize - 1;
    /* copy Nonce into bytes 1...(15-qFieldSize) in big endian form */
    CC_PalMemCopy(ctrStateBuf + 1, pNonce, sizeOfN);
    /* set counter i = 1 (in byte 15) */
    ctrStateBuf[15] = 1; /* Note: value i = 0 reserved for encoding MAC value */
    CC_PalMemCopy((uint8_t * )pAesCcmCtx->ctrStateBuf, ctrStateBuf, CC_AES_BLOCK_SIZE_IN_BYTES);

    return (0);
}

static int ccm_ass_data(mbedtls_ccm_context *ctx, const uint8_t *pAssocData, size_t assocDataSize)
{

    AesCcmContext_t *pAesCcmCtx;
    uint32_t firstBlockRemSize = 0;
    uint8_t Asize = 0;
    drvError_t rc;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    if (pAssocData == NULL)
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* set pointer to user context */
    pAesCcmCtx = (AesCcmContext_t *) ctx;

    /* formatting ASize and setting it into AES tempBuff buffer */
    /* clean working tempBuff */
    mbedtls_zeroize_internal(pAesCcmCtx->tempBuff, CC_AES_BLOCK_SIZE_IN_BYTES);

    /* set formatted ASize into temp buffer.
     Note: case ASize > 2^32 is not allowed by implementation */
    if (assocDataSize > 0xFFFFFFFF)
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* if ASize > 2^16-2^8, then set 6 bytes = 0xff||0xfe||ASize */
    if (assocDataSize >= 0xff00)
    { /* if ASize >= 2^16 - 2^8, then set 0xff||0xfe||ASize */
        pAesCcmCtx->tempBuff[0] = 0xff;
        pAesCcmCtx->tempBuff[1] = 0xfe;
        /* reverse copy 4 bytes */
        CC_CommonReverseMemcpy(pAesCcmCtx->tempBuff + 6 - 4, (uint8_t*) &assocDataSize, 4);
        Asize = 6;
    }
    else if (assocDataSize > 0)
    { /* else if 0 < ASize < 2^16 - 2^8, then set 2 bytes = ASize */
        /* reverse copy 2 bytes */
        CC_CommonReverseMemcpy(pAesCcmCtx->tempBuff + 2 - 2, (uint8_t*) &assocDataSize, 2);
        Asize = 2;
    }
    else
    { /* no Adata */
        Asize = 0;
    }

    firstBlockRemSize = CC_AES_BLOCK_SIZE_IN_BYTES - Asize;

    if (assocDataSize < firstBlockRemSize)
    {
        firstBlockRemSize = assocDataSize;
    }

    CC_PalMemCopy(pAesCcmCtx->tempBuff + Asize, pAssocData, firstBlockRemSize);

    assocDataSize -= firstBlockRemSize;
    pAssocData += firstBlockRemSize;
    pAesCcmCtx->mode = CIPHER_CBC_MAC;

    /* set data buffers structures */
    rc = SetDataBuffersInfo(pAesCcmCtx->tempBuff, CC_AES_BLOCK_SIZE_IN_BYTES, &inBuffInfo,
    NULL,
                            0, &outBuffInfo);
    if (rc != 0)
    {
        CC_PAL_LOG_ERR("illegal data buffers\n");
        return MBEDTLS_ERR_CCM_AUTH_FAILED;
    }

    /* encrypt a0 concatenated with the beginning of Associated data */
    rc = ProcessAesCcmDrv(pAesCcmCtx, &inBuffInfo, &outBuffInfo, CC_AES_BLOCK_SIZE_IN_BYTES);
    if (rc != AES_DRV_OK)
    {
        CC_PAL_LOG_ERR("encrypt a0 concatenated with the beginning of Associated data failed with error code %d\n", rc);
        return MBEDTLS_ERR_CCM_AUTH_FAILED;
    }

    if (assocDataSize)
    {
        /* set data buffers structures */
        rc = SetDataBuffersInfo(pAssocData, assocDataSize, &inBuffInfo,
        NULL,
                                0, &outBuffInfo);
        if (rc != 0)
        {
            CC_PAL_LOG_ERR("illegal data buffers\n");
            return MBEDTLS_ERR_CCM_AUTH_FAILED;
        }

        /* encrypt remaining Associated data */
        rc = ProcessAesCcmDrv(pAesCcmCtx, &inBuffInfo, &outBuffInfo, assocDataSize);
        if (rc != AES_DRV_OK)
        {
            CC_PAL_LOG_ERR("encrypt remaining Associated data failed with error code %d\n", rc);
            return MBEDTLS_ERR_CCM_AUTH_FAILED;
        }
    }

    return (0);
}

static int ccm_text_data(mbedtls_ccm_context *ctx, const uint8_t *pTextDataIn, size_t textDataSize, uint8_t *pTextDataOut)
{

    AesCcmContext_t *pAesCcmCtx = NULL;
    drvError_t rc;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    /* if the users Data In pointer is illegal return an error */
    if ((pTextDataIn == NULL) && (textDataSize != 0))
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    if (textDataSize > 0xFFFFFFFF)
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* if the users Data Out pointer is illegal return an error */
    if ((pTextDataOut == NULL) && (textDataSize != 0))
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* check overlapping of input-output buffers:
     1. in-placement operation is permitted, i.e. pTextDataIn = pTextDataOut
     2. If pTextDataIn > pTextDataOut, operation is valid since HW reads the block,
           perform operation and write the result to output which will not overwrite the next input operation block.
           And the tag/mac result is written to another temporary buffer see in ccm_finish()
     3. BUT,  pTextDataIn < pTextDataOut, operation is NOT valid since the output result will  overwrite the next input block,
           or expected tag/mac */
    if ((pTextDataIn < pTextDataOut) &&
        (pTextDataIn + textDataSize > pTextDataOut))
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* set pointer to user context */
    pAesCcmCtx = (AesCcmContext_t *) ctx;

    /* set data buffers structures */
    rc = SetDataBuffersInfo(pTextDataIn, textDataSize, &inBuffInfo, pTextDataOut, textDataSize, &outBuffInfo);
    if (rc != 0)
    {
        CC_PAL_LOG_ERR("illegal data buffers\n");
        return MBEDTLS_ERR_CCM_AUTH_FAILED;
    }

#ifndef AES_NO_TUNNEL

    /* use HW tunnel to process CCMPE/PD on text data */

    if (pAesCcmCtx->dir == CRYPTO_DIRECTION_DECRYPT)
    {
        pAesCcmCtx->mode = CIPHER_CCMPD;
    }
    else
    {
        pAesCcmCtx->mode = CIPHER_CCMPE;
    }

    rc = ProcessAesCcmDrv(pAesCcmCtx, &inBuffInfo, &outBuffInfo, textDataSize);
    if (rc != AES_DRV_OK)
    {
        CC_PAL_LOG_ERR("HW tunnel processing CCMPE/PD on text data failed with error code %d\n", rc);
        return MBEDTLS_ERR_CCM_AUTH_FAILED;
    }

#else

    /* invoke two separate operations in case of no tunneling is supported by HW */

    if (pAesCcmCtx->dir == CRYPTO_DIRECTION_ENCRYPT)
    {
        /* set operation mode to CBC_MAC */
        pAesCcmCtx->mode = CIPHER_CBC_MAC;

        rc = ProcessAesCcmDrv(pAesCcmCtx, &inBuffInfo, &outBuffInfo, buffAttr, textDataSize);
        if ( rc != AES_DRV_OK )
        {
            CC_PAL_LOG_ERR("CBC_MAC on text data failed with error code %d\n", rc);
            return MBEDTLS_ERR_CCM_AUTH_FAILED;
        }
    }

    /* Encrypt(decrypt) text data by AES-CTR, starting from CTR = CTR0+1 */
    pAesCcmCtx->mode = CIPHER_CTR;
    rc = ProcessAesCcmDrv(pAesCcmCtx, &inBuffInfo, &outBuffInfo, textDataSize);
    if ( rc != AES_DRV_OK)
    {
        CC_PAL_LOG_ERR("AES-CTR on text data failed with error code %d\n", rc);
        return MBEDTLS_ERR_CCM_AUTH_FAILED;
    }

    if (pAesCcmCtx->dir == CRYPTO_DIRECTION_DECRYPT)
    {
        /* execute the AES-MAC on decrypt mode */
        pAesCcmCtx->mode = CIPHER_CBC_MAC;

        /* set data buffers structures */
        rc = SetDataBuffersInfo(pTextDataOut, textDataSize, &inBuffInfo,
                        NULL, 0, &outBuffInfo);
        if (rc != 0)
        {
            CC_PAL_LOG_ERR("illegal data buffers\n");
            return MBEDTLS_ERR_CCM_AUTH_FAILED;
        }

        if (ProcessAesCcmDrv(pAesCcmCtx, &inBuffInfo, &outBuffInfo, textDataSize) != AES_DRV_OK )
        {
            CC_PAL_LOG_ERR("AES-MAC on decrypt data failed with error code %d\n", rc);
            return MBEDTLS_ERR_CCM_AUTH_FAILED;
        }
    }

#endif

    return (0);
}

static int ccm_finish(mbedtls_ccm_context *ctx, unsigned char * macBuf, size_t *sizeOfT)
{

    AesCcmContext_t *pAesCcmCtx;
    uint8_t *tempBuff;
    uint8_t qFieldSize;
    drvError_t rc;
    /* important to define different buffer for mac to support CCM with TextData in-place
     * and pTextDataIn > pTextDataOut overlapping addresses, see comment in ccm_text_data()*/
    uint8_t localMacBuf[CC_AES_BLOCK_SIZE_IN_BYTES];
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    if (sizeOfT == NULL)
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* if the users Data In pointer is illegal return an error */
    if (macBuf == NULL)
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* set pointer to user context */
    pAesCcmCtx = (AesCcmContext_t *) ctx;

    qFieldSize = 15 - pAesCcmCtx->sizeOfN;

    /* encrypt (decrypt) the CCM-MAC value */
    /* ------------------------------------------------------------------------- */

    /* set operation to CTR mode */
    pAesCcmCtx->mode = CIPHER_CTR;

    /* set CTR value = CTR0 for encrypt(decrypt) CCM-MAC */
    mbedtls_zeroize_internal((uint8_t*) pAesCcmCtx->ctrStateBuf + CC_AES_BLOCK_SIZE_IN_BYTES - qFieldSize, qFieldSize);

    /* on CCM encrypt mode*/
    if (pAesCcmCtx->dir == CRYPTO_DIRECTION_ENCRYPT)
    {
        /* set data buffers structures */
        rc = SetDataBuffersInfo((uint8_t*) pAesCcmCtx->ivBuf, CC_AES_BLOCK_SIZE_IN_BYTES, &inBuffInfo, localMacBuf, CC_AES_BLOCK_SIZE_IN_BYTES, &outBuffInfo);
        if (rc != 0)
        {
            CC_PAL_LOG_ERR("illegal data buffers\n");
            return MBEDTLS_ERR_CCM_AUTH_FAILED;
        }

        rc = ProcessAesCcmDrv(pAesCcmCtx, &inBuffInfo, &outBuffInfo, CC_AES_BLOCK_SIZE_IN_BYTES);
        if (rc != AES_DRV_OK)
        {
            CC_PAL_LOG_ERR("encryption of IV buf failed with error code %d\n", rc);
            return MBEDTLS_ERR_CCM_AUTH_FAILED;
        }

        CC_PalMemCopy(macBuf, localMacBuf, pAesCcmCtx->sizeOfT);

    }
    else
    {/* on CCM decrypt mode*/
        tempBuff = pAesCcmCtx->tempBuff;
        mbedtls_zeroize_internal(tempBuff, sizeof(pAesCcmCtx->tempBuff));

        /* copy encrypted CCM-MAC from input to temp buff padded with zeros */
        CC_PalMemCopy(tempBuff, macBuf, pAesCcmCtx->sizeOfT);

        /* set data buffers structures */
        rc = SetDataBuffersInfo(tempBuff, CC_AES_BLOCK_SIZE_IN_BYTES, &inBuffInfo, tempBuff, CC_AES_BLOCK_SIZE_IN_BYTES, &outBuffInfo);
        if (rc != 0)
        {
            CC_PAL_LOG_ERR("illegal data buffers\n");
            return MBEDTLS_ERR_CCM_AUTH_FAILED;
        }

        /* decrypt the MAC value and save it in the pAesCcmCtx->temp buffer */
        rc = ProcessAesCcmDrv(pAesCcmCtx, &inBuffInfo, &outBuffInfo, CC_AES_BLOCK_SIZE_IN_BYTES);
        if (rc != AES_DRV_OK)
        {
            CC_PAL_LOG_ERR("decryption of MAC buf failed with error code %d\n", rc);
            return MBEDTLS_ERR_CCM_AUTH_FAILED;
        }
        /* compare calculated and decrypted MAC results */
        if (CC_PalMemCmp(pAesCcmCtx->ivBuf, tempBuff, pAesCcmCtx-> sizeOfT ) != 0)
        {
            /* if MAC results are different, return an Error */
            CC_PAL_LOG_ERR("calculated and decrypted MAC results are different \n");
            return MBEDTLS_ERR_CCM_AUTH_FAILED;
        }
    }

    *sizeOfT = pAesCcmCtx->sizeOfT;

    return (0);
}

static int ccm_auth_crypt(mbedtls_ccm_context *ctx,
                          size_t length,
                          const unsigned char *iv,
                          size_t iv_len,
                          const unsigned char *add,
                          size_t add_len,
                          const unsigned char *input,
                          unsigned char *output,
                          unsigned char *tag,
                          size_t tag_len,
                          cryptoDirection_t dir,
                          uint32_t ccmMode)
{
    int rc;

    /* check for  user context */
    if (ctx == NULL)
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    rc = ccm_init(ctx, dir, add_len, length, iv, iv_len, tag_len, ccmMode);
    if (rc != 0)
    {
        return rc;
    }

    /* if there's no associated data, skip a0 encryption */
    if (add_len != 0)
    {
        rc = ccm_ass_data(ctx, add, add_len);
        if (rc != 0)
        {
            return rc;
        }
    }

    /* if there's no payload data, skip to authentication */
    if (length != 0)
    {
        rc = ccm_text_data(ctx, input, length, output);
        if (rc != 0)
        {
            return rc;
        }
    }

    rc = ccm_finish(ctx, tag, &tag_len);
    if (0 != rc)
    {
        mbedtls_zeroize_internal(output, length);
    }

    return rc;
}

/****************************** PUBLIC **************************/
int mbedtls_ccm_get_security_level(uint8_t sizeOfT, uint8_t *pSecurityLevel)
{
    if (pSecurityLevel == NULL)
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /*
     The security level field for AES-CCM* as defined in ieee-802.15.4-2011, Table 58.
     System spec requirement CCM*-3: The CCM* shall support only the security levels that include encryption (1XX values).
     */
    switch (sizeOfT)
    {
        case 0:
            *pSecurityLevel = AESCCM_STAR_SECURITY_LEVEL_ENC;
            break;
        case 4:
            *pSecurityLevel = AESCCM_STAR_SECURITY_LEVEL_ENC_MIC_32;
            break;
        case 8:
            *pSecurityLevel = AESCCM_STAR_SECURITY_LEVEL_ENC_MIC_64;
            break;
        case 16:
            *pSecurityLevel = AESCCM_STAR_SECURITY_LEVEL_ENC_MIC_128;
            break;
        default:
            return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    return CC_OK;
}

void mbedtls_ccm_init_int(mbedtls_ccm_context *ctx)
{
    if (ctx == NULL)
    {
        CC_PalAbort("!!!!CCM context is NULL!!!\n");
    }
    /* check size of structs match , memory allocated must be at least the size of AesCcmContext_t*/
    if (sizeof(mbedtls_ccm_context) < sizeof(AesCcmContext_t))
    {
        CC_PalAbort("!!!!CCM context sizes mismatch!!!\n");
    }
    mbedtls_zeroize_internal(ctx, sizeof(mbedtls_ccm_context));

}

int mbedtls_ccm_setkey_int(mbedtls_ccm_context *ctx, mbedtls_cipher_id_t cipher, const unsigned char *key, unsigned int keybits)
{

    AesCcmContext_t *aes_ccm_ctx;

    if (ctx == NULL || key == NULL)
    {
        CC_PAL_LOG_ERR("Null pointer, ctx or key are NULL\n");
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    if (cipher != MBEDTLS_CIPHER_ID_AES)
    {
        /* No real use case for CCM other then AES*/
        CC_PAL_LOG_ERR("Only AES cipher id is supported\n");
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }
    aes_ccm_ctx = (AesCcmContext_t *) ctx;
    switch (keybits)
    {
        case 128:

            aes_ccm_ctx->keySizeId = KEY_SIZE_128_BIT;
            break;

        case 192:

            aes_ccm_ctx->keySizeId = KEY_SIZE_192_BIT;
            break;

        case 256:

            aes_ccm_ctx->keySizeId = KEY_SIZE_256_BIT;
            break;

        default:
            return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* Copy user key to context */
    CC_PalMemCopy(aes_ccm_ctx->keyBuf, key, keybits / 8);

    return (0);
}

void mbedtls_ccm_free_int(mbedtls_ccm_context *ctx)
{
    if (ctx != NULL)
    {
        mbedtls_zeroize_internal(ctx, sizeof(mbedtls_ccm_context));
    }
}

int mbedtls_ccm_encrypt_and_tag_int(mbedtls_ccm_context *ctx,
                                    size_t length,
                                    const unsigned char *iv,
                                    size_t iv_len,
                                    const unsigned char *add,
                                    size_t add_len,
                                    const unsigned char *input,
                                    unsigned char *output,
                                    unsigned char *tag,
                                    size_t tag_len,
                                    uint32_t ccmMode)
{
    return ccm_auth_crypt(ctx, length, iv, iv_len, add, add_len, input, output, tag, tag_len, CRYPTO_DIRECTION_ENCRYPT, ccmMode);
}

int mbedtls_ccm_auth_decrypt_int(mbedtls_ccm_context *ctx,
                                 size_t length,
                                 const unsigned char *iv,
                                 size_t iv_len,
                                 const unsigned char *add,
                                 size_t add_len,
                                 const unsigned char *input,
                                 unsigned char *output,
                                 const unsigned char *tag,
                                 size_t tag_len,
                                 uint32_t ccmMode)
{

    uint8_t localMacBuf[CC_AES_BLOCK_SIZE_IN_BYTES];

    if (tag_len > CC_AES_BLOCK_SIZE_IN_BYTES)
    {
        CC_PAL_LOG_ERR("parameter tag_len is bigger then %d\n", CC_AES_BLOCK_SIZE_IN_BYTES);
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }
    if (tag == NULL)
    {
        CC_PAL_LOG_ERR("\ntag NULL pointer\n");
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    CC_PalMemCopy(localMacBuf, tag, tag_len);
    return ccm_auth_crypt(ctx, length, iv, iv_len, add, add_len, input, output, localMacBuf, tag_len, CRYPTO_DIRECTION_DECRYPT, ccmMode);
}


int mbedtls_ccm_star_nonce_generate(unsigned char * src_addr, uint32_t frame_counter, uint8_t size_of_t, unsigned char * nonce_buf)
{
    int rc = 0;
    uint8_t securityLevelField = 0;

    if ((src_addr == NULL) || (nonce_buf == NULL))
    {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    if ((rc = mbedtls_ccm_get_security_level(size_of_t, &securityLevelField)) != 0)
    {
        return rc;
    }

    /*
     The nonce structure for AES-CCM* is defined in ieee-802.15.4-2011, Figure 61:
     Source address (8) | Frame counter (4) | Security lvel (1)
     */

    CC_PalMemCopy(nonce_buf, src_addr, MBEDTLS_AESCCM_STAR_SOURCE_ADDRESS_SIZE_BYTES);
    CC_PalMemCopy(nonce_buf + MBEDTLS_AESCCM_STAR_SOURCE_ADDRESS_SIZE_BYTES, &frame_counter, sizeof(uint32_t));
    nonce_buf[MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES - 1] = securityLevelField;

    return 0;
}
