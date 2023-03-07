/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <openssl/evp.h>
#include <openssl/bio.h>
#include "common_crypto_encode.h"
#include "common_util_log.h"


/* header and footer of the PEM */
#define CC_UTIL_COMMON_PEM_HEADER       "-----BEGIN CERTIFICATE-----"
#define CC_UTIL_COMMON_PEM_FOOTER       "-----END CERTIFICATE-----"


#define IS_BASE64_ENCODE_VAL(val) \
        ((((val >= '0') && (val <= '9')) || \
         ((val >= 'A') && (val <= 'Z')) || \
         ((val >= 'a') && (val <= 'z')) || \
         (val == '+') || (val == '/'))?1:0)

/**
* @brief Encodes data into base64 format
*
* @param[in] pBuff             - the buffer to encode
* @param[in] buffLen           - input buffer length
* @param[out] pEncBuff         - encoded buffer
* @param[in/out] pEncBuffLen   - encoded buffer length
*/
/*********************************************************/
int32_t CC_CommonBase64Encode(uint8_t *pBuff,
                uint32_t  buffLen,
                uint8_t *pEncBuff,
                uint32_t *pEecBuffLen)
{
    BIO *bio = NULL;
    BIO *b64 = NULL;;
    size_t size;
    FILE *stream = NULL;
    int32_t actualWritten = 0;
    int32_t rc = 0;

    if ((NULL == pBuff) ||
        (NULL == pEncBuff) ||
        (NULL == pEecBuffLen) ||
        (0 == buffLen)) {
        UTIL_LOG_ERR("ilegal inputs\n");
        return 1;
    }
    if (*pEecBuffLen < ((((buffLen*4)+2)/3)+1)) {
        UTIL_LOG_ERR("ilegal outBuffLen %d \n", *pEecBuffLen);
        return 1;
    }

    memset(pEncBuff, 0, *pEecBuffLen);

    stream = fmemopen(pEncBuff, *pEecBuffLen, "w");
    if (NULL == stream) {
        UTIL_LOG_ERR("failed to open mem\n");
        return 1;
    }
    b64 = BIO_new(BIO_f_base64());
    if (NULL == b64) {
        UTIL_LOG_ERR("failed to BIO_new\n");
        rc = 1;
        goto End_Base64Encode;
    }
    bio = BIO_new_fp(stream, BIO_NOCLOSE);
    if (NULL == bio) {
        UTIL_LOG_ERR("failed to BIO_new_fp\n");
        rc = 1;
        goto End_Base64Encode;
    }
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
    actualWritten = BIO_write(bio, pBuff, buffLen);
    BIO_flush(bio);
    if (actualWritten != buffLen) {
        UTIL_LOG_ERR("failed to BIO_write buffLen %d = actualWritten %d\n", buffLen, actualWritten);
        rc = 1;
        goto End_Base64Encode;
    }
    rc = 0;

    End_Base64Encode:
    if (bio != NULL) { /* clears bio and b64, b64 mustn't clear explicitly*/
        BIO_free_all(bio);
    }
    if (stream != NULL) {
        fclose(stream);
    }
    return rc;
}


/**
* @brief Decode base64-encoded data
*
* @param[in] pEncBuff          - base64-encoded buffer
* @param[in] encBuffLen        - input buffer length
* @param[out] pDecBuff         - decoded buffer
* @param[in/out] pDecBuffLen   - decoded buffer length
*/
/*********************************************************/
int32_t CC_CommonBase64Decode(uint8_t *pEncBuff,
                uint32_t  encBuffLen,
                uint8_t *pDecBuff,
                uint32_t *pDecBuffLen)
{
    BIO *bio = NULL;
    BIO *b64 = NULL;
    int32_t actualRead = 0;
    int32_t expDecBuffLen = 0;
    FILE* stream = NULL;
    int32_t padding = 0;
    int32_t rc = 0;
    int32_t i = 0;

    if ((NULL == pEncBuff) ||
        (NULL == pDecBuff) ||
        (NULL == pDecBuffLen) ||
        (0 == encBuffLen)) {
        UTIL_LOG_ERR("ilegal inputs\n");
        return 1;
    }

    UTIL_LOG_INFO("started. encBuffLen %d\n", encBuffLen);
    /* check validity of decoded buffer length */
    i = encBuffLen-1;
    while (!IS_BASE64_ENCODE_VAL(pEncBuff[i])) {
        UTIL_LOG_INFO("pEncBuff[%d] 0x%x\n", i, pEncBuff[i]);
        padding++;
        i--;
    }
    expDecBuffLen = (((encBuffLen-padding)*3)/4);
    if (*pDecBuffLen < expDecBuffLen) {
        UTIL_LOG_ERR("ilegal inputs outBuffLen %d expDecBuffLen %d\n", *pDecBuffLen, expDecBuffLen);
        rc = 1;
        goto End_Base64Decode;
    }

    memset(pDecBuff, 0, *pDecBuffLen);

    stream = fmemopen(pEncBuff, encBuffLen, "r");
    if (NULL == stream) {
        UTIL_LOG_ERR("failed to open mem\n");
        rc = 1;
        goto End_Base64Decode;
    }

    b64 = BIO_new(BIO_f_base64());
    if (NULL == b64) {
        UTIL_LOG_ERR("failed to BIO_new\n");
        rc = 1;
        goto End_Base64Decode;
    }
    bio = BIO_new_fp(stream, BIO_NOCLOSE);
    if (NULL == bio) {
        UTIL_LOG_ERR("failed to BIO_new_fp\n");
        rc = 1;
        goto End_Base64Decode;
    }
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
    actualRead = BIO_read(bio, pDecBuff, encBuffLen);
    if (actualRead != expDecBuffLen) {
        UTIL_LOG_ERR("failed to BIO_read expDecBuffLen %d = actualRead %d\n", expDecBuffLen, actualRead);
        rc = 1;
        goto End_Base64Decode;
    }
    *pDecBuffLen = actualRead;
    rc = 0;

    End_Base64Decode:
    if (bio != NULL) { /* clears bio and b64, b64 mustn't clear explicitly*/
        BIO_free_all(bio);
    }
    if (stream != NULL) {
        fclose(stream);
    }
    UTIL_LOG_INFO("rc %d\n", rc);
    return rc;
}

