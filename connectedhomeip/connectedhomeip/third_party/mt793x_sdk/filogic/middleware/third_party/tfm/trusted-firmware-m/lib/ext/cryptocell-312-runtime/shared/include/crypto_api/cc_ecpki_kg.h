/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_ECPKI_KG_H
#define _CC_ECPKI_KG_H

/*! @file
@brief This file defines the API for generation of ECC private and public keys.
@defgroup cc_ecpki_kg CryptoCell ECC Key Generation APIs
@{
@ingroup cryptocell_ecpki

*/


#include "cc_error.h"
#include "cc_rnd_common.h"
#include "cc_ecpki_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*****************  CC_EcpkiKeyPairGenerate function   **********************/
/*!
@brief Generates a pair of private and public keys in internal representation according to ANSI X9.62-2005: Public Key Cryptography for the
Financial Services Industry, The Elliptic Curve Digital Signature Algorithm (ECDSA) standard.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_ecpki_error.h or cc_rnd_error.h
*/
CIMPORT_C CCError_t CC_EcpkiKeyPairGenerate(
                        CCRndContext_t         *pRndContext,      /*!< [in/out] Pointer to the RND context buffer. */
                        const CCEcpkiDomain_t  *pDomain,          /*!< [in]  Pointer to EC domain (curve). */
                        CCEcpkiUserPrivKey_t   *pUserPrivKey,     /*!< [out] Pointer to the private key structure. This structure is used as input to the
                                         ECPKI cryptographic primitives. */
                        CCEcpkiUserPublKey_t   *pUserPublKey,     /*!< [out] Pointer to the public key structure. This structure is used as input to the
                                         ECPKI cryptographic primitives. */
            CCEcpkiKgTempData_t   *pTempData,        /*!< [in] Temporary buffers for internal use, defined in ::CCEcpkiKgTempData_t. */
                        CCEcpkiKgFipsContext_t   *pFipsCtx       /*!< [in] Pointer to temporary buffer used in case FIPS certification if required
                                       (may be NULL for all other cases). */
);

/*****************  CC_EcpkiKeyPairGenerateBase function   **********************/
/*!
@brief Generates a pair of private and public keys using a configurable base point
in internal representation according to ANSI X9.62-2005: Public Key Cryptography for the
Financial Services Industry, The Elliptic Curve Digital Signature Algorithm (ECDSA) standard.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_ecpki_error.h or cc_rnd_error.h
*/
CIMPORT_C CCError_t CC_EcpkiKeyPairGenerateBase(
                        CCRndContext_t         *pRndContext, /*!< [in/out] Pointer to RND context. */
                        const CCEcpkiDomain_t  *pDomain,          /*!< [in]  Pointer to EC domain (curve). */
                        const uint32_t         *ecX_ptr, /*!< [in]  The X cordinate of the base point. */
                        const uint32_t         *ecY_ptr, /*!< [in]  The Y cordinate of the base point. */
                        CCEcpkiUserPrivKey_t   *pUserPrivKey,     /*!< [out] Pointer to the private key structure. This structure is used as input to the
                                         ECPKI cryptographic primitives. */
                        CCEcpkiUserPublKey_t   *pUserPublKey,     /*!< [out] Pointer to the public key structure. This structure is used as input to the
                                         ECPKI cryptographic primitives. */
                        CCEcpkiKgTempData_t    *pTempData,        /*!< [in] Temporary buffers for internal use, defined in ::CCEcpkiKgTempData_t. */
                        CCEcpkiKgFipsContext_t *pFipsCtx       /*!< [in] Pointer to temporary buffer used in case FIPS certification if required
                                       (may be NULL for all other cases). */
);



#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif




