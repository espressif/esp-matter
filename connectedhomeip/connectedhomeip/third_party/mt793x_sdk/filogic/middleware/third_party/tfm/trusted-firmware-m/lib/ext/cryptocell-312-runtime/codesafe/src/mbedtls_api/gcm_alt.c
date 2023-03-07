/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Definition of GCM:
 * http://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38d.pdf
 * Recommendation for Block Cipher Modes of Operation:
 * Galois/Counter Mode (GCM) and GMAC
 *
 * The API supports AES-GCM as defined in NIST SP 800-38D.
 *
 */
#if defined(MBEDTLS_CONFIG_FILE)
    #include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_GCM_C) && defined(MBEDTLS_GCM_ALT)

#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_pal_abort.h"
#include "cc_common.h"
#include "aesgcm_driver.h"
#include "mbedtls_common.h"
#include "mbedtls/gcm.h"

#define MBEDTLS_ERR_GCM_API_IS_NOT_SUPPORTED        -0x0016  /**< API is NOT supported. */

/*! AES GCM data in maximal size in bytes. */
#define MBEDTLS_AESGCM_DATA_IN_MAX_SIZE_BYTES       0xFFFF // (64KB - 1)
/*! AES GCM IV maximal size in bytes. */
#define MBEDTLS_AESGCM_IV_MAX_SIZE_BYTES            0xFFFF // (64KB - 1)
/*! AES GCM AAD maximal size in bytes. */
#define MBEDTLS_AESGCM_AAD_MAX_SIZE_BYTES           0xFFFF // (64KB - 1)

/*! AES GCM 96 bits size IV. */
#define MBEDTLS_AESGCM_IV_96_BITS_SIZE_BYTES        12

/*! AES GCM Tag size: 4 bytes. */
#define MBEDTLS_AESGCM_TAG_SIZE_4_BYTES             4
/*! AES GCM Tag size: 8 bytes. */
#define MBEDTLS_AESGCM_TAG_SIZE_8_BYTES             8
/*! AES GCM Tag size: 12 bytes. */
#define MBEDTLS_AESGCM_TAG_SIZE_12_BYTES            12
/*! AES GCM Tag size: 13 bytes. */
#define MBEDTLS_AESGCM_TAG_SIZE_13_BYTES            13
/*! AES GCM Tag size: 14 bytes. */
#define MBEDTLS_AESGCM_TAG_SIZE_14_BYTES            14
/*! AES GCM Tag size: 15 bytes. */
#define MBEDTLS_AESGCM_TAG_SIZE_15_BYTES            15
/*! AES GCM Tag size: 16 bytes. */
#define MBEDTLS_AESGCM_TAG_SIZE_16_BYTES            16

/*
 * Initialize a context
 */
void mbedtls_gcm_init(mbedtls_gcm_context *ctx)
{
    if (NULL == ctx) {
        CC_PalAbort("!!!!GCM context is NULL!!!\n");
    }

    if (sizeof(mbedtls_gcm_context) < sizeof(AesGcmContext_t)) {
        CC_PalAbort("!!!!GCM context sizes mismatch!!!\n");
    }

    mbedtls_zeroize_internal(ctx, sizeof(mbedtls_gcm_context));
}

int mbedtls_gcm_setkey(mbedtls_gcm_context *ctx,
               mbedtls_cipher_id_t cipher,
               const unsigned char *key,
               unsigned int keybits)
{
    AesGcmContext_t *aes_gcm_ctx ;

    if (ctx == NULL || key == NULL) {
        CC_PAL_LOG_ERR("Null pointer, ctx or key are NULL\n");
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    if (cipher != MBEDTLS_CIPHER_ID_AES) {
        /* No real use case for GCM other then AES*/
        CC_PAL_LOG_ERR("Only AES cipher id is supported\n");
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    aes_gcm_ctx = (AesGcmContext_t *)ctx;
    switch (keybits) {
        case 128:
            aes_gcm_ctx->keySizeId = KEY_SIZE_128_BIT;
            break;
        case 192:
            aes_gcm_ctx->keySizeId = KEY_SIZE_192_BIT;
            break;
        case 256:
            aes_gcm_ctx->keySizeId = KEY_SIZE_256_BIT;
            break;
        default:
            return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    /* Copy user key to context */
    CC_PalMemCopy(aes_gcm_ctx->keyBuf, key, (keybits/8));

    return(0);
}

/*
 * Free context
 */
void mbedtls_gcm_free(mbedtls_gcm_context *ctx)
{
    mbedtls_zeroize_internal(ctx, sizeof(mbedtls_gcm_context));
}

static int gcm_calc_h(mbedtls_gcm_context *ctx)
{
    AesGcmContext_t *pAesGcmCtx = NULL;
    drvError_t rc;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    /* set pointer to user context */
    pAesGcmCtx = (AesGcmContext_t *)ctx;

    /* Set process mode to 'CalcH' */
    pAesGcmCtx->processMode = DRV_AESGCM_Process_CalcH;

    /* set data buffers structures */
    rc = SetDataBuffersInfo((uint8_t*)(pAesGcmCtx->tempBuf), CC_AESGCM_GHASH_DIGEST_SIZE_BYTES, &inBuffInfo,
                            (uint8_t*)(pAesGcmCtx->H), AES_128_BIT_KEY_SIZE, &outBuffInfo);
    if (rc != 0) {
         CC_PAL_LOG_ERR("illegal data buffers\n");
         return MBEDTLS_ERR_GCM_AUTH_FAILED;
    }

    /* Calculate H */
    rc = ProcessAesGcm(pAesGcmCtx, &inBuffInfo, &outBuffInfo, CC_AES_BLOCK_SIZE_IN_BYTES);
    if (rc != AES_DRV_OK) {
        CC_PAL_LOG_ERR("calculating H failed with error code %d\n", rc);
        return MBEDTLS_ERR_GCM_AUTH_FAILED;
    }

    return (0);
}

static int gcm_init(mbedtls_gcm_context *ctx,
                    cryptoDirection_t encryptDecryptFlag,
            const uint8_t* pIv,
            size_t ivSize,
            const uint8_t* pAad,
            size_t aadSize,
            const uint8_t* pDataIn,
            size_t dataSize,
            uint8_t* pDataOut,
            const uint8_t* pTag,
            size_t tagSize)
{
    AesGcmContext_t *pAesGcmCtx = NULL;
    drvError_t rc;

    /* Check the Encrypt / Decrypt flag validity */
    if (CRYPTO_DIRECTION_NUM_OF_ENC_MODES <= encryptDecryptFlag) {
        return  MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    /* Check the data in size validity */
    if (MBEDTLS_AESGCM_DATA_IN_MAX_SIZE_BYTES < dataSize) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    if (0 != dataSize) {
        /* Check dataIn pointer */
        if (NULL == pDataIn) {
            return MBEDTLS_ERR_GCM_BAD_INPUT;
        }

        /* Check dataOut pointer */
        if (NULL == pDataOut) {
            return MBEDTLS_ERR_GCM_BAD_INPUT;
        }
    }

    /* Check the IV size validity */
    if ((MBEDTLS_AESGCM_IV_MAX_SIZE_BYTES < ivSize) || (0 == ivSize)) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    /* Check iv pointer */
    if (NULL == pIv) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    /* Check the AAD size validity */
    if (MBEDTLS_AESGCM_AAD_MAX_SIZE_BYTES < aadSize) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    /* Check aad pointer */
    if ((NULL == pAad) && (aadSize != 0)) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    /* Check the Tag size validity */
    if ((MBEDTLS_AESGCM_TAG_SIZE_4_BYTES  != tagSize) && (MBEDTLS_AESGCM_TAG_SIZE_8_BYTES  != tagSize) &&
        (MBEDTLS_AESGCM_TAG_SIZE_12_BYTES != tagSize) && (MBEDTLS_AESGCM_TAG_SIZE_13_BYTES != tagSize) &&
        (MBEDTLS_AESGCM_TAG_SIZE_14_BYTES != tagSize) && (MBEDTLS_AESGCM_TAG_SIZE_15_BYTES != tagSize) &&
        (MBEDTLS_AESGCM_TAG_SIZE_16_BYTES != tagSize)) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    /* Check Tag pointer */
    if (NULL == pTag) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    /* set pointer to user context */
    pAesGcmCtx = (AesGcmContext_t *)ctx;

    /* Set direction of operation: enc./dec. */
    pAesGcmCtx->dir = MBEDTLS_2_DRIVER_DIRECTION(encryptDecryptFlag);
    pAesGcmCtx->dataSize = dataSize;
    pAesGcmCtx->ivSize = ivSize;
    pAesGcmCtx->aadSize = aadSize;
    pAesGcmCtx->tagSize = tagSize;

    /******************************************************/
    /***                Calculate H                     ***/
    /******************************************************/
    rc = gcm_calc_h(ctx);
    if (rc != AES_DRV_OK) {
        CC_PAL_LOG_ERR("calculating H failed with error code %d\n", rc);
        return MBEDTLS_ERR_GCM_AUTH_FAILED;
    }

    return 0;
}

static int gcm_process_j0(mbedtls_gcm_context *ctx, const uint8_t* pIv)
{
    AesGcmContext_t *pAesGcmCtx = NULL;
    drvError_t rc;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    /* set pointer to user context */
    pAesGcmCtx = (AesGcmContext_t *)ctx;

    if (MBEDTLS_AESGCM_IV_96_BITS_SIZE_BYTES == pAesGcmCtx->ivSize) {
        // Concatenate IV||0(31)||1
        CC_PalMemCopy(pAesGcmCtx->J0, pIv, MBEDTLS_AESGCM_IV_96_BITS_SIZE_BYTES);
        pAesGcmCtx->J0[3] = SWAP_ENDIAN(0x00000001);
    } else {

        /***********************************************/
        /* Calculate GHASH over the first phase buffer */
        /***********************************************/
        /* Set process mode to 'CalcJ0' */
        pAesGcmCtx->processMode = DRV_AESGCM_Process_CalcJ0_FirstPhase;

        /* set data buffers structures */
        rc = SetDataBuffersInfo(pIv, pAesGcmCtx->ivSize, &inBuffInfo,
                                NULL, 0, &outBuffInfo);
        if (rc != 0) {
             CC_PAL_LOG_ERR("illegal data buffers\n");
             return MBEDTLS_ERR_GCM_AUTH_FAILED;
        }

        /* Calculate J0 - First phase */
        rc = ProcessAesGcm(pAesGcmCtx, &inBuffInfo, &outBuffInfo, pAesGcmCtx->ivSize);
        if (rc != AES_DRV_OK) {
            CC_PAL_LOG_ERR("calculating J0 (phase 1) failed with error code 0x%X\n", rc);
            return MBEDTLS_ERR_GCM_AUTH_FAILED;
        }

        /*********************************************/
        /* Build & Calculate the second phase buffer */
        /*********************************************/
        CC_PalMemSetZero(pAesGcmCtx->tempBuf, sizeof(pAesGcmCtx->tempBuf));
        pAesGcmCtx->tempBuf[3] = (pAesGcmCtx->ivSize << 3) & BITMASK(CC_BITS_IN_32BIT_WORD);
        pAesGcmCtx->tempBuf[3] = SWAP_ENDIAN(pAesGcmCtx->tempBuf[3]);

        /* Set process mode to 'CalcJ0' */
        pAesGcmCtx->processMode = DRV_AESGCM_Process_CalcJ0_SecondPhase;

        /* set data buffers structures */
        rc = SetDataBuffersInfo((uint8_t*)(pAesGcmCtx->tempBuf), CC_AESGCM_GHASH_DIGEST_SIZE_BYTES, &inBuffInfo,
                                NULL, 0, &outBuffInfo);
        if (rc != 0) {
             CC_PAL_LOG_ERR("illegal data buffers\n");
             return MBEDTLS_ERR_GCM_AUTH_FAILED;
        }

        /* Calculate J0 - Second phase  */
        rc = ProcessAesGcm(pAesGcmCtx, &inBuffInfo, &outBuffInfo, CC_AESGCM_GHASH_DIGEST_SIZE_BYTES);
        if (rc != AES_DRV_OK) {
            CC_PAL_LOG_ERR("calculating J0 (phase 2) failed with error code %d\n", rc);
            return MBEDTLS_ERR_GCM_AUTH_FAILED;
        }
    }

    return 0;
}

static int gcm_process_aad(mbedtls_gcm_context *ctx, const uint8_t* pAad)
{
    AesGcmContext_t *pAesGcmCtx = NULL;
    drvError_t rc = 0;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    /* set pointer to user context */
    pAesGcmCtx = (AesGcmContext_t *)ctx;

    /* Clear Ghash result buffer */
    CC_PalMemSetZero(pAesGcmCtx->ghashResBuf, sizeof(pAesGcmCtx->ghashResBuf));

    if (0 == pAesGcmCtx->aadSize) {
        return rc;
    }

    /* Set process mode to 'Process_A' */
    pAesGcmCtx->processMode = DRV_AESGCM_Process_A;

    /* set data buffers structures */
    rc = SetDataBuffersInfo(pAad, pAesGcmCtx->aadSize, &inBuffInfo,
                            NULL, 0, &outBuffInfo);
    if (rc != 0) {
         CC_PAL_LOG_ERR("illegal data buffers\n");
         return MBEDTLS_ERR_GCM_AUTH_FAILED;
    }

    /* Calculate GHASH(A) */
    rc = ProcessAesGcm(pAesGcmCtx, &inBuffInfo, &outBuffInfo, pAesGcmCtx->aadSize);
    if (rc != AES_DRV_OK) {
        CC_PAL_LOG_ERR("processing AAD failed with error code %d\n", rc);
        return MBEDTLS_ERR_GCM_AUTH_FAILED;
    }

    return 0;
}

static int gcm_process_cipher(mbedtls_gcm_context *ctx, const uint8_t* pTextDataIn, uint8_t* pTextDataOut)
{
    AesGcmContext_t *pAesGcmCtx = NULL;
    drvError_t rc;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    /* set pointer to user context */
    pAesGcmCtx = (AesGcmContext_t *)ctx;

    /* Must NOT perform in this case */
    if (0 == pAesGcmCtx->dataSize) {
        return 0;
    }

    /* Set process mode to 'Process_DataIn' */
    pAesGcmCtx->processMode = DRV_AESGCM_Process_DataIn;

    /* set data buffers structures */
    rc = SetDataBuffersInfo(pTextDataIn, pAesGcmCtx->dataSize, &inBuffInfo,
                            pTextDataOut, pAesGcmCtx->dataSize, &outBuffInfo);
    if (rc != 0) {
         CC_PAL_LOG_ERR("illegal data buffers\n");
         return MBEDTLS_ERR_GCM_AUTH_FAILED;
    }

    rc = ProcessAesGcm(pAesGcmCtx, &inBuffInfo, &outBuffInfo, pAesGcmCtx->dataSize);
    if (rc != AES_DRV_OK) {
        CC_PAL_LOG_ERR("processing cipher failed with error code %d\n", rc);
        return MBEDTLS_ERR_GCM_AUTH_FAILED;
    }

    return 0;
}

static int gcm_process_lenA_lenC(mbedtls_gcm_context *ctx)
{
    AesGcmContext_t *pAesGcmCtx = NULL;
    drvError_t rc;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    /* set pointer to user context */
    pAesGcmCtx = (AesGcmContext_t *)ctx;

    /* Build buffer */
    pAesGcmCtx->tempBuf[1] = (pAesGcmCtx->aadSize << 3) & BITMASK(CC_BITS_IN_32BIT_WORD);
    pAesGcmCtx->tempBuf[1] = SWAP_ENDIAN(pAesGcmCtx->tempBuf[1]);
    pAesGcmCtx->tempBuf[0] = 0;
    pAesGcmCtx->tempBuf[3] = (pAesGcmCtx->dataSize << 3) & BITMASK(CC_BITS_IN_32BIT_WORD);
    pAesGcmCtx->tempBuf[3] = SWAP_ENDIAN(pAesGcmCtx->tempBuf[3]);
    pAesGcmCtx->tempBuf[2] = 0;

    /* Set process mode to 'Process_LenA_LenC' */
    pAesGcmCtx->processMode = DRV_AESGCM_Process_LenA_LenC;

   /* set data buffers structures */
    rc = SetDataBuffersInfo((uint8_t*)(pAesGcmCtx->tempBuf), CC_AESGCM_GHASH_DIGEST_SIZE_BYTES, &inBuffInfo,
                            NULL, 0, &outBuffInfo);
    if (rc != 0) {
         CC_PAL_LOG_ERR("illegal data buffers\n");
         return MBEDTLS_ERR_GCM_AUTH_FAILED;
    }

    /* Calculate GHASH(LenA || LenC) */
    rc = ProcessAesGcm(pAesGcmCtx, &inBuffInfo, &outBuffInfo, CC_AESGCM_GHASH_DIGEST_SIZE_BYTES);
    if (rc != AES_DRV_OK) {
        CC_PAL_LOG_ERR("processing Lengths of AAD and Cipher failed with error code %d\n", rc);
        return MBEDTLS_ERR_GCM_AUTH_FAILED;
    }

    return 0;
}

static int gcm_finish(mbedtls_gcm_context *ctx, uint8_t* pTag)
{
    AesGcmContext_t *pAesGcmCtx = NULL;
    int rc;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    /* set pointer to user context */
    pAesGcmCtx = (AesGcmContext_t *)ctx;

    /* Set process mode to 'Process_GctrFinal' */
    pAesGcmCtx->processMode = DRV_AESGCM_Process_GctrFinal;

    /* set data buffers structures */
    rc = SetDataBuffersInfo((uint8_t*)(pAesGcmCtx->tempBuf), CC_AESGCM_GHASH_DIGEST_SIZE_BYTES, &inBuffInfo,
                            pAesGcmCtx->preTagBuf, CC_AESGCM_GHASH_DIGEST_SIZE_BYTES, &outBuffInfo);
    if (rc != 0) {
         CC_PAL_LOG_ERR("illegal data buffers\n");
         return MBEDTLS_ERR_GCM_AUTH_FAILED;
    }

    /* Calculate Encrypt and Calc. Tag */
    rc = ProcessAesGcm(pAesGcmCtx, &inBuffInfo, &outBuffInfo, CC_AESGCM_GHASH_DIGEST_SIZE_BYTES);
    if (rc != AES_DRV_OK) {
        CC_PAL_LOG_ERR("Finish operation failed with error code %d\n", rc);
        return MBEDTLS_ERR_GCM_AUTH_FAILED;
    }

    if (CRYPTO_DIRECTION_ENCRYPT == pAesGcmCtx->dir) {
        CC_PalMemCopy(pTag, pAesGcmCtx->preTagBuf, pAesGcmCtx->tagSize);
        rc = 0;
    } else {
        if (0 == CC_PalMemCmp(pAesGcmCtx->preTagBuf, pTag, pAesGcmCtx->tagSize)) {
            rc = 0;
        } else {
            rc = MBEDTLS_ERR_GCM_AUTH_FAILED;
           }
    }

    return rc;
}

static int gcm_crypt_and_tag(mbedtls_gcm_context *ctx,
                 int mode,
                 size_t length,
                 const unsigned char *iv,
                 size_t iv_len,
                 const unsigned char *aad,
                 size_t aad_len,
                 const unsigned char *input,
                 unsigned char *output,
                 size_t tag_len,
                 unsigned char *tag)
{
    AesGcmContext_t *pAesGcmCtx = NULL;
    int rc;

    /* check for  user context */
    if (NULL == ctx) {
    return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    /* Aes-GCM Initialization function */
    rc = gcm_init(ctx, (cryptoDirection_t)mode,
          iv, iv_len,
          aad, aad_len,
          input, length, output,
          tag, tag_len);

    if (0 != rc) {
    goto gcm_crypt_and_tag_END;
    }

    /* Aes-GCM Process J0 function */
    rc = gcm_process_j0(ctx, iv);
    if (0 != rc) {
    goto gcm_crypt_and_tag_END;
    }

    /* Aes-GCM Process AAD function */
    rc = gcm_process_aad(ctx, aad);
    if (0 != rc) {
    goto gcm_crypt_and_tag_END;
    }

    /* Aes-GCM Process Cipher function */
    rc = gcm_process_cipher(ctx, input, output);
    if (0 != rc) {
    goto gcm_crypt_and_tag_END;
    }

    /* Aes-GCM Process LenA||LenC function */
    rc = gcm_process_lenA_lenC(ctx);
    if (0 != rc) {
    goto gcm_crypt_and_tag_END;
    }

    rc = gcm_finish(ctx, tag);


gcm_crypt_and_tag_END:
    /* set pointer to user context and clear the output in case of failure*/
    pAesGcmCtx = (AesGcmContext_t *)ctx;
    if ((CRYPTO_DIRECTION_DECRYPT == pAesGcmCtx->dir) && (MBEDTLS_ERR_GCM_AUTH_FAILED == rc)) {
        CC_PalMemSetZero(output, pAesGcmCtx->dataSize);
    }

    /* Clear working context */
    CC_PalMemSetZero(ctx->buf, sizeof(mbedtls_gcm_context));

    return rc;
}

int mbedtls_gcm_crypt_and_tag(mbedtls_gcm_context *ctx,
                  int mode,
                  size_t length,
                  const unsigned char *iv,
                  size_t iv_len,
                  const unsigned char *add,
                  size_t add_len,
                  const unsigned char *input,
                  unsigned char *output,
                  size_t tag_len,
                  unsigned char *tag)
{
    int rc;

    rc = gcm_crypt_and_tag(ctx, mode, length,
                   iv, iv_len,
                   add, add_len,
                   input, output,
                   tag_len, tag);
    return rc;
}

int mbedtls_gcm_auth_decrypt( mbedtls_gcm_context *ctx,
                  size_t length,
                  const unsigned char *iv,
                  size_t iv_len,
                  const unsigned char *add,
                  size_t add_len,
                  const unsigned char *tag,
                  size_t tag_len,
                  const unsigned char *input,
                  unsigned char *output )
{
    int rc;

    rc = gcm_crypt_and_tag(ctx, 0, length,
                   iv, iv_len,
                   add, add_len,
                   input, output,
                   tag_len, (unsigned char *)tag);
    return rc;
}

/**************************************************************************************************/
/******                             UN-Supported API's                   **************************/
/**************************************************************************************************/
int mbedtls_gcm_starts(mbedtls_gcm_context *ctx,
                       int mode,
                       const unsigned char *iv,
               size_t iv_len,
               const unsigned char *aad,
               size_t aad_len)
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(mode);
    CC_UNUSED_PARAM(iv);
    CC_UNUSED_PARAM(iv_len);
    CC_UNUSED_PARAM(aad);
    CC_UNUSED_PARAM(aad_len);

    return (MBEDTLS_ERR_GCM_API_IS_NOT_SUPPORTED);
}

int mbedtls_gcm_update(mbedtls_gcm_context *ctx,
               size_t length,
               const unsigned char *input,
               unsigned char *output)
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(length);
    CC_UNUSED_PARAM(input);
    CC_UNUSED_PARAM(output);

    return (MBEDTLS_ERR_GCM_API_IS_NOT_SUPPORTED);
}

int mbedtls_gcm_finish(mbedtls_gcm_context *ctx,
               unsigned char *tag,
               size_t tag_len)
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(tag);
    CC_UNUSED_PARAM(tag_len);

    return (MBEDTLS_ERR_GCM_API_IS_NOT_SUPPORTED);
}
/**************************************************************************************************/
#endif
