/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_ECPKI_DH_H
#define _CC_ECPKI_DH_H

/*! @file
@brief This file defines the API that supports EC Diffie-Hellman shared secret value derivation primitives.
@defgroup cc_ecpki_dh CryptoCell ECC Diffie-Hellman APIs
@{
@ingroup cryptocell_ecpki

*/


#include "cc_ecpki_types.h"
#include "cc_ecpki_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
 *               CC_EcdhSvdpDh function                    *
 ***********************************************************************/
/*!
@brief Creates the shared secret value according to IEEE 1363-2000: IEEE Standard for Standard Specifications for Public-Key Cryptography standard
and ANSI X9.63-2011: Public Key Cryptography for the Financial Services Industry - Key Agreement and Key Transport Using
Elliptic Curve Cryptography standard:
<ol><li> Checks input-parameter pointers and EC Domain in public and private
keys.</li>
<li> Derives the partner public key and calls the EcWrstDhDeriveSharedSecret
function, which performs EC SVDP operations.</li></ol>
\note The term "User"
refers to any party that calculates a shared secret value using this primitive.
The term "Partner" refers to any other party of shared secret value calculation.
Partner's public key shall be validated before using in this primitive.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_ecpki_error.h.
*/
CIMPORT_C CCError_t CC_EcdhSvdpDh(
                        CCEcpkiUserPublKey_t *PartnerPublKey_ptr,           /*!< [in]  Pointer to a partner public key. */
                        CCEcpkiUserPrivKey_t *UserPrivKey_ptr,              /*!< [in]  Pointer to a user private key. */
                        uint8_t              *SharedSecretValue_ptr,    /*!< [out] Pointer to an output buffer that contains the shared
                                               secret value. */
                        size_t                   *SharedSecrValSize_ptr,    /*!< [in/out] Pointer to the size of user-passed buffer (in) and
                                                                                          actual size of output of calculated shared secret value
                                              (out). */
                        CCEcdhTempData_t     *TempBuff_ptr              /*!< [in]  Pointer to a temporary buffer. */);

#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif
