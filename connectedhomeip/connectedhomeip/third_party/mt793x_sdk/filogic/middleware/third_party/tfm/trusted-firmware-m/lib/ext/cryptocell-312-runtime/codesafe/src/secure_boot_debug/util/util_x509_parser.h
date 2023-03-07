/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _UTIL_X509_PARSER_H_
#define _UTIL_X509_PARSER_H_


#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_crypto_x509_defs.h"
#include "secureboot_defs.h"
#include  "secureboot_parser_gen_defs.h"


#define X509_RSA_E_VAL_IN_BYTES     {0x01,0x00,0x01}

/* TBS structure */
#define CC_X509_CERT_SEQ_TAG_ID         0x30
#define CC_X509_CERT_INT_TAG_ID         0x02
#define CC_X509_CERT_OBJ_IDENTIFIER_TAG_ID      0x06
#define CC_X509_CERT_SET_OF_TAG_ID          0x31
#define CC_X509_CERT_PRNT_STR_TAG_ID            0x13
#define CC_X509_CERT_UTF8_TAG_ID            0x0C
#define CC_X509_CERT_BIT_STR_TAG_ID         0x03
#define CC_X509_CERT_BOOL_TAG_ID            0x01
#define CC_X509_CERT_CTX_SPEC_TAG_ID            0xA0
#define CC_X509_CERT_CTX_SPEC_TAG1_ID           0xA1
#define CC_X509_CERT_CTX_SPEC_TAG2_ID           0xA2
#define CC_X509_CERT_NULL_TAG_ID            0x05
#define CC_X509_CERT_CTX_EXT_TAG_ID         0xA3
#define CC_X509_CERT_OCT_STR_TAG_ID         0x04

#define CC_X509_CERT_UTC_TIME_TAG_ID            0x17
#define CC_X509_CERT_GENERALIZED_TIME_TAG_ID    0x18

/* x509 definitions */
#define CC_X509_CERT_VERSION        0x02
#define CC_X509_CERT_RSASSAENC_ID   {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01}  // 1.2.840.113549.1.1.1
#define CC_X509_CERT_SHA256RSAENC_ID    {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0B}  // 1.2.840.113549.1.1.11
#define CC_X509_CERT_SHA256RSAPSS_ID    {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0A}  // 1.2.840.113549.1.1.10
#define CC_X509_CERT_SHA256_ID      {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01}  // 2.16.840.1.101.3.4.2.1
#define CC_X509_CERT_MGF1_ID        {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x08}  // 1.2.840.113549.1.1.8

#define CC_X509_CERT_CNT_NUM_OF_OBJ_IN_EXT  5
#define CC_X509_CERT_KEY_NUM_OF_OBJ_IN_EXT  4

#define CC_X509_CERT_CTX_SPEC_TAG2_SIZE     0x3

/**
 * @brief This function verifies certificate's issuer name according to predefined name
 *
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from bootimagesverifierx509_error.h
 */
CCError_t UTIL_X509VerifyIssuerName(uint8_t *pCert, uint32_t size);

/**
 * @brief This function follows x509 string structure, should be called before
 *    reading the issuer name or the subject name
 *
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from bootimagesverifierx509_error.h
 */
CCError_t UTIL_X509VerifyStr(uint8_t **pCert, uint32_t *dataSize, unsigned long startAddress, unsigned long endAddress);

/**
 * @brief This function verify the subject name according to fixed data
 *
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from bootimagesverifierx509_error.h
 */
CCError_t UTIL_X509VerifySubjectName(uint8_t *pCert, CCX509CertType_t certType, uint32_t subNameSize);

/**
 * @brief This function gets the public key from the certificate and copy it to the external buffer
 *
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from bootimagesverifierx509_error.h
 */
CCError_t UTIL_X509VerifyPubKey(uint8_t **pCert, CCSbNParams_t *pParamsN, unsigned long startAddress, unsigned long endAddress);

/**
 * @brief This function gets the signature from the certificate and copy it to the external buffer
 *
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from bootimagesverifierx509_error.h
 */
CCError_t UTIL_X509GetSignature(uint8_t **pCert, CCSbSignature_t *signatureP, unsigned long startAddress, unsigned long endAddress);

#ifdef __cplusplus
}
#endif

#endif



