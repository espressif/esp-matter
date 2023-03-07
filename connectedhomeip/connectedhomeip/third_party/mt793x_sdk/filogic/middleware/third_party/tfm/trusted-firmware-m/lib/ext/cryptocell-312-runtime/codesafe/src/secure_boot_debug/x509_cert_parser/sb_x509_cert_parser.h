/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SB_X509_CERT_PARSER_H
#define _SB_X509_CERT_PARSER_H

#include "cc_pal_x509_defs.h"
#include "secureboot_parser_gen_defs.h"
#include "secureboot_gen_defs.h"


#define CC_X509_CERT_TYPE_KEY_MAX_SIZE      0x400
#define CC_X509_CERT_TYPE_PRIM_DBG_MAX_SIZE 0x400
#define CC_X509_CERT_TYPE_SCND_DBG_MAX_SIZE 0x400

#ifdef CC_SB_CERT_USER_DATA_EXT
/* user's data */
typedef uint8_t CCSbUserData_t[X509_USER_DATA_MAX_SIZE_BYTES];
#endif

/* max Num of components in the header certificate arameter */
#define CC_X509_CERT_LEN_NUM_OF_COMPS_MAX_SIZE             0x0000FFFFL

typedef struct {
    CCSbNParams_t       certPubKey;
    CCSbSwVersion_t     nvCounter;
    CCSbCertPubKeyHash_t    pubKeyHash;
#ifdef CC_SB_CERT_USER_DATA_EXT
/* user's data */
    CCSbUserData_t      userData;
#endif
    CCSbSignature_t     certSign; /* address must be word aligned */
}KeyCertInfo_t;

typedef struct {
    CCSbSignature_t     certSign; /* address must be word aligned */
    CCSbNParams_t       certPubKey;
    CCSbSwVersion_t     nvCounter;
    CCSbNonce_t     nonce;
    uint32_t        numOfImages;
#ifdef CC_SB_CERT_USER_DATA_EXT
/* user's data */
    CCSbUserData_t      userData;
#endif
}ContentCertInfo_t;

typedef struct {
    CCSbNParams_t       certPubKey;
    uint32_t        validLcs;
    uint32_t        socSpecific;
    uint32_t        rmaMode;
    CCSbCertPubKeyHash_t    pubKeyHash;
#ifdef CC_SB_CERT_USER_DATA_EXT
/* user's data */
    CCSbUserData_t      userData;
#endif
    CCSbSignature_t     certSign;  /* address must be word aligned */
}primDbgCertInfo_t;

typedef struct {
    CCSbNParams_t       certPubKey;
    CCSbCertSocId_t     socId;
    uint32_t        socSpecific;
#ifdef CC_SB_CERT_USER_DATA_EXT
/* user's data */
    CCSbUserData_t      userData;
#endif
    CCSbSignature_t     certSign; /* address must be word aligned */
}scndDbgCertInfo_t;


/*!
 * @brief Parse and validate debug primary certificate
 *
 * @param[in] ppAsn1Cert    - pointer to X509 certificate as ASN.1 byte array
 * @param[in] certMaxSize   - certificate max size (according to certificate type)
 * @param[out] pSignCertSize    - certificate TBS size, used for signature
 * @param[out] pTbsStartOffset - certificate TBS start offset, used for signature
 * @param[out] pOutPubKey       - certificate public key modulus (exponent is constant)
 * @param[out] pOutCertHeaderInfo  - certificate specfic header info
 * @param[out] startAddress     - startAddress of certificate pointer
 * @param[out] endAddress       - endAddress of certificate pointer (max address that certificate ponter may reach)
 *
 * @return uint32_t         - On success: the value CC_OK is returned,
 *                    On failure: a value from sbrom_bsv_error.h
 */
CCError_t SB_X509_VerifyCertTbsHeader(uint8_t       **ppAsn1Cert,
                      uint32_t      certMaxSize,
                      uint32_t      *pSignCertSize,
                      uint32_t      *pTbsStartOffset,
                      CCSbNParams_t     *pOutPubKey,
                      CCX509CertHeaderInfo_t *pOutCertHeaderInfo,
                      unsigned long startAddress,
                      unsigned long endAddress);

/*!
 * @brief Parse and validate certificate package header
 *
 * @param[in] pPkgHeader    - pointer to debug certificate package
 * @param[in] pkgType       - certificate package type
 * @param[out] pDbg2CertInfo    - secondary debug certificate offset and size
 * @param[out] pHbkFormat   - HBK format for comparison with OTP
 *
 * @return uint32_t         - On success: the value CC_OK is returned,
 *                    On failure: a value from sbrom_bsv_error.h
 */
CCError_t SB_X509_ParseCertPkg(CCX509PkgHeader_t    *pPkgHeader,
                  CCX509CertType_t      pkgType,
                  CCX509CertInfo_t      *pDbg2CertInfo,
                  CCSbPubKeyIndexType_t     *pHbkFormat);


#endif
