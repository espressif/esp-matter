/*
 *   Copyright (c) 2016, GreenPeak Technologies
 *   Copyright (c) 2017, Qorvo Inc
 *
 *   Elliptic Curve Cryptography
 *   Declarations of the public functions and enumerations of gpECC.
 *
 *   This software is owned by Qorvo Inc
 *   and protected under applicable copyright laws.
 *   It is delivered under the terms of the license
 *   and is intended and supplied for use solely and
 *   exclusively with products manufactured by
 *   Qorvo Inc.
 *
 *
 *   THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *   CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *   IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *   LIMITED TO, IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A
 *   PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *   QORVO INC. SHALL NOT, IN ANY
 *   CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *   INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *   FOR ANY REASON WHATSOEVER.
 *
 *   $Header$
 *   $Change$
 *   $DateTime$
 */


#ifndef _GPECC_H_
#define _GPECC_H_

/// @file "gpECC.h"
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#ifdef GP_ECC_DIVERSITY_USE_SLICING
/** @macro GP_ECC_NUM_CTX */
#define GP_ECC_NUM_CTX                               4

/** @macro GP_ECC_API_CTX_SIZE */
#define GP_ECC_API_CTX_SIZE                          150

/** @macro GP_ECC_API_CTX_NUM */
#define GP_ECC_API_CTX_NUM                           0

/** @macro GP_ECC_SLICE_CTX_SIZE */
#define GP_ECC_SLICE_CTX_SIZE                        190

/** @macro GP_ECC_SLICE_CTX_NUM */
#define GP_ECC_SLICE_CTX_NUM                         1

/** @macro GP_ECC_API_MAKE_KEY_CTX_SIZE */
#define GP_ECC_API_MAKE_KEY_CTX_SIZE                 190

/** @macro GP_ECC_API_MAKE_KEY_CTX_NUM */
#define GP_ECC_API_MAKE_KEY_CTX_NUM                   2

/** @macro GP_ECC_SLICE_MAKE_KEY_CTX_SIZE */
#define GP_ECC_SLICE_MAKE_KEY_CTX_SIZE               190

/** @macro GP_ECC_SLICE_MAKE_KEY_CTX_NUM */
#define GP_ECC_SLICE_MAKE_KEY_CTX_NUM                3

#endif /*GP_ECC_DIVERSITY_USE_SLICING*/

/** @macro GP_ECC_SECP256R1_SIZE */
#define GP_ECC_SECP256R1_SIZE                        32


/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#ifdef GP_ECC_DIVERSITY_USE_SLICING
typedef void (*gpECC_SharedSecret_SliceCb)(uint8_t **pBuffs);
typedef void (*gpECC_MakeKey_SliceCb)(uint8_t **pBuffs);
#endif /*GP_ECC_DIVERSITY_USE_SLICING*/
#ifdef GP_ECC_DIVERSITY_USE_EXT_LIB
typedef const void * gpECC_Curve;
#else
struct gpECC_Curve_t;
typedef const struct gpECC_Curve_t * gpECC_Curve;
#endif /*GP_ECC_DIVERSITY_USE_EXT_LIB*/

typedef int (*gpECC_RNG_Function)(UInt8 *dest, unsigned size);


/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//Requests
/** @brief Return a handle for the ECC curve secp256r1 (NIST-P256). */
gpECC_Curve gpECC_secp256r1(void);

/** @brief Generate a new (random) public/private key pair.
 *
 *  @param[out] public_key   Pointer to (2*N)-byte buffer to store the generated public key.
 *  @param[out] private_key  Pointer to N-byte buffer to store the generated private key.
 *  @param[in]  curve        Curve handle.
 *
 *  @return 1 when successful, 0 in case of failure.
 *
 *  The size of the private key depends on the curve, for example GP_ECC_SECP256R1_SIZE for curve secp256r1.
 *  The size of the public key is two times the size of the private key.
 */
Int32 gpECC_make_key(UInt8 *public_key, UInt8 *private_key, gpECC_Curve curve);

#if defined(GP_ECC_DIVERSITY_USE_SLICING)
void gpECC_make_key_sliced(const UInt8 *public_key,
                                const UInt8 *private_key,
                                gpECC_Curve curve,
                                gpECC_MakeKey_SliceCb CbFunction,
                                UInt8 **ContextPtrs,
                                UInt16 *ContextSizes);
#endif //defined(GP_ECC_DIVERSITY_USE_SLICING)

/** @brief Derive a shared secret from a given public key and private key using the ECDH algorithm.
 *
 *  @param[in]  public_key   Pointer to public key (2*N bytes).
 *  @param[in]  private_key  Pointer to private key (N bytes).
 *  @param[out] secret       Pointer to N-byte buffer to store the derived shared secret.
 *
 *  @return 1 when successful, 0 in case of failure.
 *
 *  The size of the private key depends on the curve, for example GP_ECC_SECP256R1_SIZE for curve secp256r1.
 *  The size of the public key is two times the size of the private key.
 *  The size of the shared secret is the same as the size of the private key.
 */
Int32 gpECC_shared_secret(const UInt8 *public_key, const UInt8 *private_key, UInt8* secret, gpECC_Curve curve);

#if defined(GP_ECC_DIVERSITY_USE_SLICING)
void gpECC_shared_secret_sliced(const UInt8 *public_key, const UInt8 *private_key, UInt8* secret, gpECC_Curve curve, gpECC_SharedSecret_SliceCb CbFunction, UInt8 **ContextPtrs, UInt16 *ContextSizes);
#endif //defined(GP_ECC_DIVERSITY_USE_SLICING)

/** @brief Set the random number generator to be used when generating a key pair.
 *
 *  When using the uECC implementation of gpECC, @a gpECC_set_rng must be called
 *  before calling @a gpECC_make_key.
 *
 *  The CryptoSoc implementation of gpECC uses its own random number generator;
 *  in this case calling @a gpECC_set_rng is not necessary and will be ignored.
 */
void gpECC_set_rng(gpECC_RNG_Function rng_function);

/** @brief Check validity of the public key.
 *
 *  @param[in]  public_key   Pointer to public key (2*N bytes).
 *
 *   @return 1 when public key is valid, 0 in case of invalid public key.
 */
Int32 gpECC_valid_public_key(const UInt8 *public_key);


//Indications

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPECC_H_

