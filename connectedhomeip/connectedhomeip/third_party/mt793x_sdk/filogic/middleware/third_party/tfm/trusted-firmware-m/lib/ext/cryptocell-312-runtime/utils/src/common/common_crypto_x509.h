/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _COMMON_CRYPTO_X509_H
#define _COMMON_CRYPTO_X509_H

#include <stdint.h>
#include "cc_crypto_x509_defs.h"
#include "cc_crypto_x509_common_defs.h"


typedef struct {
    uint8_t   setSerialNum;
    uint32_t  serialNum;
    uint8_t   setNotBefore;
    long      notBefore;
    uint8_t   setNotAfter;
    long      notAfter;
    uint8_t   setIssuerName;
    char      IssuerName[X509_ISSUER_NAME_MAX_STRING_SIZE+1];
    uint8_t   setSubjectName;
    char      SubjectName[X509_SUBJECT_NAME_MAX_STRING_SIZE+1];
}CCX509CertHeaderParamsIn_t;

/**
* @brief free X509 certificate
*
* @param[in/out] ppCertBuff          - x.509 certificate
*/
/*********************************************************/
void CC_CommonX509Free(uint8_t **ppCertBuff);


/**
* @brief Creates X509 certificate and set its header fields
*
* @param[in/out] ppCertBuff     - x.509 certificate
* @param[in] certType           - certificate type
*/
/*********************************************************/
int32_t CC_CommonX509CreateAndSetHeader(uint8_t **ppCertBuff,
                CCX509CertType_t  certType,CCX509CertHeaderParamsIn_t *pCertHeaderParams);


/**
* @brief Add ASN.1 critical integer extension to X.509V3 certificate
*
* @param[in/out] pCertBuff          - x.509 certificate
* @param[in] certType                 - certificate type
* @param[in] extType              - extension type
* @param[in] val              - Extension value
*/
/*********************************************************/
int32_t CC_CommonX509AddIntegerExtension(uint8_t *pCertBuff,
                       CCX509CertType_t certType,
                       CCX509ExtType_t  extType,
                       int32_t val);


/**
* @brief Add critical DER extension to X.509V3 certificate
*
* @param[in/out] pCertBuff          - x.509 certificate
* @param[in] certType                 - certificate tyoes
* @param[in] extType              - extension type
* @param[in] pVal                 - Extension data
* @param[in] valLen               - extension data length
*/
/*********************************************************/
int32_t CC_CommonX509AddStringExtension(uint8_t *pCertBuff,
                      CCX509CertType_t  certType,
                      CCX509ExtType_t  extType,
                      uint8_t *pVal,
                      uint32_t valLen);

/**
* @brief Add subject public key to the X509 certificate
*   and sign the certificate
*
* @param[in/out] pCertBuff      - x.509 certificate
* @param[in] pKeyPairFileName   - key pair file name in PEM format
* @param[in] pKeyPairPwd    - passphrase of key pair
*/
/*********************************************************/
int32_t CC_CommonX509SetKeyAndSign(uint8_t *pCertBuff,
                uint8_t *pKeyPairFileName,
                uint8_t *pKeyPairPwd);



/**
* @brief convert the x.509 certificate to DER format
*
* @param[in/out] ppCertBuff      - x.509 certificate
* @param[out] pOutCertSize  - certificate size in DER format
/*********************************************************/
int32_t CC_CommonX509ToDer(uint8_t **pCertBuff,
                uint32_t *pOutCertSize);


/**
* @brief build package for the certificate
*
* @param[in] ppCertBuff          - the x509 certificate  in PEM format
* @param[in] certSize       - certificate size
* @param[in] certType           - certificate type
* @param[in] encFlag        - indicates whether images were encrypted
* @param[in] hbkType        - hbk type to use by target, in the verification
* @param[in] pAddData       - additional data to add to package
* @param[in] addDataSize        - length of additional data
* @param[in] outPkgFile     - package file name to write the package to
*/
/*********************************************************/
#ifdef CC_SB_SUPPORT_IOT
int32_t CC_CommonX509BuildCertPkg(uint8_t **ppCertBuff,
                uint32_t certSize,
                uint8_t *pAddData,
                uint32_t addDataSize,
                uint8_t *outPkgFile);

#else
int32_t CC_CommonX509BuildCertPkg(uint8_t **ppCertBuff,
                uint32_t certSize,
                CCX509CertType_t certType,
                uint8_t encFlag,
                uint8_t hbkType,
                uint8_t *pAddData,
                uint32_t addDataSize,
                uint8_t *outPkgFile);

#endif

#endif
