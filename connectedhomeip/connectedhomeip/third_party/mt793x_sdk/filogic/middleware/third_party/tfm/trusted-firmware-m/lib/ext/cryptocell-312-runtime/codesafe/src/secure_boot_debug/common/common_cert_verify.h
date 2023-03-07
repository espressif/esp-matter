/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _COMMON_CERT_VERIFY_H
#define _COMMON_CERT_VERIFY_H

#include "common_cert_parser.h"

/**
   @brief This function is used for basic verification of all secure boot/debug certificates.
   it verifies type, size, public key and signature.
   The function returns pointers to certificate proprietary header, and body.
   The function:
   1. calls CCCertFieldsParse() - according to certificate type(x509 or not),
                copy public key, Np and signature to workspace,
                and returns pointers to certificate proprietary header, and body.
   2. Calls CCCertValidateHeader(), and verify cert type (as expected) and size (according to type).
   3. If expected public key hash is NULL, call CC_BsvPubKeyHashGet() with HBK type defined in certificate to get OTP HBK
   4. Calls verifyCertPubKeyAndSign() To verify public key and certificate RSA signature.
 */
CCError_t CCCommonCertVerify(unsigned long   hwBaseAddress,
                             BufferInfo32_t  *pCertInfo,
                             CertFieldsInfo_t  *pCertFields,  // in/out
                             CCSbCertInfo_t  *pSbCertInfo,   //in/out
                             BufferInfo32_t  *pWorkspaceInfo,
                             BufferInfo32_t  *pX509HeaderInfo);


/**
   @brief This function verifies key certificate specific fields.
 */
uint32_t CCCommonKeyCertVerify(unsigned long   hwBaseAddress,
                               uint32_t certFlags,
                               uint8_t  *pCertMain,
                               CCSbCertInfo_t *pCertPkgInfo);

/**
   @brief This function   verifies content certificate specific fields
        Verifies certificate flags, NV counter according to HBK type
        Call CCCertValidateSWComps()
        Call CCSbSetNvCounter()
 */
uint32_t CCCommonContentCertVerify(CCSbFlashReadFunc flashReadFunc,
                                   void *userContext,
                                   unsigned long hwBaseAddress,
                                   CCAddr_t certStoreAddress,
                                   CCSbCertInfo_t *certPkgInfo,
                                   uint32_t certFlags,
                                   uint8_t *pCertMain,
                                   BufferInfo32_t  *pWorkspaceInfo);


#endif /* _COMMON_CERT_VERIFY_H */


