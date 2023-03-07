/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef CC_EC_MONT_API_TW_H
#define CC_EC_MONT_API_TW_H

#include "cc_pal_types.h"
#include "cc_rnd_common.h"
#include "cc_pka_defs_hw.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
@file
@brief This file contains the CryptoCell APIs used for EC MONT (Montgomery Curve25519) algorithms.
@defgroup cc_ec_mont CryptoCell EC Montgomery APIs
@{
@ingroup cryptocell_ec



\note  Implemented algorithms according to Montgomery elliptic curves cryptography,
       developed by Daniel J.Bernstein etc.
*/

/*! EC Montgomery curve25519 modulus size in bits, words and bytes  */
/*! EC Montgomery modulus size in bits. */
#define CC_EC_MONT_MOD_SIZE_IN_BITS  255U
/*! EC Montgomery modulus size in words. */
#define CC_EC_MONT_MOD_SIZE_IN_32BIT_WORDS ((CC_EC_MONT_MOD_SIZE_IN_BITS + CC_BITS_IN_32BIT_WORD - 1) / CC_BITS_IN_32BIT_WORD)
/*! EC Montgomery modulus size in bytes. */
#define CC_EC_MONT_MOD_SIZE_IN_BYTES       ((CC_EC_MONT_MOD_SIZE_IN_BITS + CC_BITS_IN_BYTE - 1) / CC_BITS_IN_BYTE)

/*! Constant sizes of special EC_MONT buffers and arrays  */
/*! EC Montgomery scalar size in bytes. */
#define CC_EC_MONT_SCALARBYTES     (CC_EC_MONT_MOD_SIZE_IN_32BIT_WORDS * CC_32BIT_WORD_SIZE)
/*! EC Montgomery scalar multiplication size in bytes. */
#define CC_EC_MONT_SCALARMULTBYTES (CC_EC_MONT_MOD_SIZE_IN_32BIT_WORDS * CC_32BIT_WORD_SIZE)
/*! EC Montgomery scalar seed size in bytes. */
#define CC_EC_MONT_SEEDBYTES       (CC_EC_MONT_MOD_SIZE_IN_32BIT_WORDS * CC_32BIT_WORD_SIZE)

/*! EC Montgomery domains ID-s enumerator. */
typedef enum
{
    CC_EC_MONT_DOMAIN_CURVE_25519,  /*!< EC Curve25519 */
    /*! EC Montgomery last domain. */
    CC_EC_MONT_DOMAIN_OFF_MODE,
    /*! Reserved. */
    CC_EC_MONT_DOMAIN_LAST = 0x7FFFFFFF
}CCEcMontDomainId_t;


/*! EC_MONT scalar mult temp buffer type definition */
typedef struct {
    /*! Internal temporary buffer. */
        uint32_t ecMontScalarMultTempBuff[CC_EC_MONT_TEMP_BUFF_SIZE_IN_32BIT_WORDS]; //! ! Change as needed
} CCEcMontScalrMultTempBuff_t;

/*! EC_MONT temp buffer type definition */
typedef struct {
        /* Don't change sequence order of the buffers */
    /*! Internal temporary buffer. */
        uint32_t ecMontScalar[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
    /*! Internal temporary buffer. */
        uint32_t ecMontResPoint[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
    /*! Internal temporary buffer. */
        uint32_t ecMontInPoint[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
    /*! Internal temporary buffer. */
        CCEcMontScalrMultTempBuff_t  ecMontScalrMultTempBuff;// if needed ?
} CCEcMontTempBuff_t;


/*********************************************************************/
/*!
@brief The function performs EC Montgomery (Curve25519) scalar multiplication:
       resPoint = scalar * point.

@return CC_OK on success,
@return A non-zero value on failure as defined cc_ec_mont_edw_error.h.
*/
CIMPORT_C CCError_t CC_EcMontScalarmult(
                                uint8_t       *pResPoint,       /*!< [out] Pointer to the public (secret) key. */
                                size_t        *pResPointSize,   /*!< [in/out] Pointer to the size of the public key in bytes.
                                                                       In  - the size of the buffer. must be at least EC modulus
                                                                             size (for curve25519 - 32 bytes).
                                                                       Out - the actual size. */
                                const uint8_t *pScalar,         /*!< [in] Pointer to the secret (private) key. */
                                size_t         scalarSize,      /*!< [in] Pointer to the size of the secret key in bytes;
                                                                     must be equal to EC order size (for curve25519 - 32 bytes). */
                                const uint8_t *pInPoint,        /*!< [in] Pointer to the input point (compressed). */
                                size_t         inPointSize,     /*!< [in] Size of the point - must be equal to CC_EC_MONT_MOD_SIZE_IN_BYTES. */
                                CCEcMontTempBuff_t *ecMontTempBuff  /*!< [in] Pointer to temp buffer, for internal use. */);



/*********************************************************************/
/*!
@brief The function performs EC Montgomery (Curve25519) scalar multiplication of base point:
       res = scalar * base_point.

       Note: all byte arrays have LE order of bytes, i.e. LS byte is on left most place.

@return CC_OK on success,
@return A non-zero value on failure as defined cc_ec_mont_edw_error.h.
*/
CIMPORT_C CCError_t CC_EcMontScalarmultBase(
                                uint8_t       *pResPoint,      /*!< [out] Pointer to the public (secret) key. */
                                size_t        *pResPointSize,  /*!< [in/out] Pointer to the size of the public key in bytes.
                                                                      In  - the size of buffer must be at least EC modulus size
                                     (for curve25519 - 32 bytes);
                                                                      Out - the actual size. */
                                const uint8_t *pScalar,        /*!< [in] Pointer to the secret (private) key. */
                                size_t         scalarSize,     /*!< [in] Pointer to the size of the scalar in bytes -
                                                                    must be equal to EC order size (for curve25519 - 32 bytes). */
                                CCEcMontTempBuff_t *pEcMontTempBuff /*!< [in] Pointer to temp buffer, for internal use. */);


/*******************************************************************/
/*!
@brief The function randomly generates  private and public keys for Montgomery
       Curve25519. it uses CC_EcMontKeyPair with the Generator point of the Curve


\note All byte arrays are in LE order of bytes, i.e. LS byte is on the left most place.\par
\note LS and MS bits of the Secret key are set according to EC Montgomery scalar mult. algorithm:
      secrKey[0] &= 248; secrKey[31] &= 127; secrKey[31] |= 64;

@return CC_OK on success,
@return A non-zero value on failure as defined cc_ec_mont_edw_error.h or cc_rnd_error.h.

*/
CIMPORT_C CCError_t CC_EcMontKeyPair (
                      uint8_t *pPublKey,                    /*!< [out] Pointer to the public key. */
                      size_t  *pPublKeySize,                /*!< [in/out] Pointer to the size of the public key in bytes.
                                                                  In  - the size of the buffer must be at least EC order size
                                                                        (for curve25519 - 32 bytes);
                                                                  Out - the actual size. */
                      uint8_t *pSecrKey,                    /*!< [out] Pointer to the secret key, including. */
                      size_t  *pSecrKeySize,                /*!< [in/out] Pointer to the size of buffer for the secret key in bytes -
                                                                 must be at least EC order size (for curve25519 - 32 bytes). */
                      CCRndContext_t *pRndContext,      /*!< [in/out] Pointer to the RND context buffer. */
                      CCEcMontTempBuff_t *pEcMontTempBuff /*!< [in] Pointer to the temp buffer, for internal use. */);


/*******************************************************************/

/*!
@brief The function randomly generates  private and public keys for Montgomery
       Curve25519, using a configurable base point


\note All byte arrays are in LE order of bytes, i.e. LS byte is on the left most place.\par
\note LS and MS bits of the Secret key are set according to EC Montgomery scalar mult. algorithm:
      secrKey[0] &= 248; secrKey[31] &= 127; secrKey[31] |= 64;

@return CC_OK on success,
@return A non-zero value on failure as defined cc_ec_mont_edw_error.h or cc_rnd_error.h.

*/
CIMPORT_C CCError_t CC_EcMontKeyPairBase (
                      uint8_t *pPublKey,                    /*!< [out] Pointer to the public key. */
                      size_t  *pPublKeySize,                /*!< [in/out] Pointer to the size of the public key in bytes.
                                                                  In  - the size of the buffer must be at least EC order size
                                                                        (for curve25519 - 32 bytes);
                                                                  Out - the actual size. */
                      uint8_t *pSecrKey,                    /*!< [out] Pointer to the secret key, including. */
                      size_t  *pSecrKeySize,                /*!< [in/out] Pointer to the size of buffer for the secret key in bytes -
                                                                 must be at least EC order size (for curve25519 - 32 bytes). */
                      const uint8_t  *pInPoint,          /*!< [in] Pointer to the input point (compressed). */
                      size_t         inPointSize,        /*!< [in] Size of the point - must be equal to CC_EC_MONT_MOD_SIZE_IN_BYTES. */
                      CCRndContext_t *pRndContext,      /*!< [in/out] Pointer to the RND context buffer. */
                      CCEcMontTempBuff_t *pEcMontTempBuff /*!< [in] Pointer to the temp buffer, for internal use. */);


/*******************************************************************/

/*!
@brief The function generates private and public keys for Montgomery algorithms.

       The generation performed using given seed.


@return CC_OK on success,
@return A non-zero value on failure as defined cc_ec_mont_edw_error.h or cc_hash_error.h.
*/
CIMPORT_C CCError_t CC_EcMontSeedKeyPair (
                          uint8_t       *pPublKey,       /*!< [out] Pointer to the public (secret) key. */
                          size_t        *pPublKeySize,   /*!< [in/out] Pointer to the size of the public key in bytes.
                                                                 In  - the size of buffer must be at least EC order size
                                                                       (for curve25519 - 32 bytes);
                                                                 Out - the actual size. */
                          uint8_t       *pSecrKey,       /*!< [out] Pointer to the secret (private) key. */
                          size_t        *pSecrKeySize,   /*!< [in/out] Pointer to the size of the secret key in bytes
                                                                  In  - the size of buffer must be at least EC order size
                                                                        (for curve25519 - 32 bytes);
                                                                  Out - the actual size. */
                          const uint8_t *pSeed,          /*!< [in] Pointer to the given seed - 32 bytes. */
                          size_t         seedSize,       /*!< [in/] Size of the seed in bytes (must be equal to CC_EC_MONT_SEEDBYTES). */
                          CCEcMontTempBuff_t *pEcMontTempBuff  /*!< [in] Pointer to a temp buffer, for internal use. */);



#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif



