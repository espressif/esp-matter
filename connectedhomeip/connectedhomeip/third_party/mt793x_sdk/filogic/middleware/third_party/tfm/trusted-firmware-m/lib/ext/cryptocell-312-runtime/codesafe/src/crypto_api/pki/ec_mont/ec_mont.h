/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef EC_MONT_H_H
#define EC_MONT_H_H


#ifdef __cplusplus
extern "C"
{
#endif

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "cc_pal_types.h"
#include "cc_ec_mont_api.h"
#include "ec_mont_local.h"


/*****************   Definitions    ***********************/

/*********************************************************************/
/*!
@brief The function performs EC Montgomery (Curve25519) scalar multiplication:
       resPoint = scalar * point.

       Libsodium analog: crypto_scalarmult_curve25519() function

 @return CCError_t
*/
CCError_t EcMontScalarmult(
                                uint32_t *resPoint,       /* [out] pointer to result point (compressed,
                                                                   the size = ec modulus size) */
                                uint32_t *scalar,         /* [in] pointer to the scalar, the size = ec order size) */
                                uint32_t *inPoint,        /* [in] pointer to the input point (compressed, the
                                                                  size = ec modulus size)  */
                                const CCEcMontDomain_t *pEcDomain /* [in] pointer to EC domain (curve). */);

/*********************************************************************/
/*!
 * The function performs input/output parameters in/from PKA to perform scalar
 * multiplication of EC_MONT point (using ladder algorithm).
 *
 *         resPoint(X,_) = k*inPoint(X,_), where:
 *         both points are given in compressed form (only X coordinates ) with
 *         LE order of words.
 *
 *         Assuming: the PKA HW is turned on and initialized yet.
 *
 * \param resPointX -  the pointer to result EC_MONT point.
 * \param scalar -     the pointer to scalar (LE order of words).
 * \param scalarSizeBits - the size of scalar in bits.
 * \param inPointX -   the pointer to input ECMONT point (coord. X only).
 *
 * \return CCError_t
 */
CCError_t EcMontPkaScalMultWithLadder(
                        uint32_t  *resPoint,  /*!< [out] pointer to result EC point (coordinate X). */
                        uint32_t  *scalar,    /*!< [in] pointer to scalar. */
                        uint32_t   scalarSizeBits, /*!< [in] scalar size in bits. */
                        uint32_t  *inPoint,   /*!< [in] pointer to input ECMONT point (coordinate X). */
                        const CCEcMontDomain_t *pEcDomain); /*!< [in] pointer to EC domain (curve). */



#ifdef __cplusplus
}

#endif

#endif


