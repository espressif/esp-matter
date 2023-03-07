/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_EC_EDW_API_H
#define _CC_EC_EDW_API_H

#include "cc_pal_types.h"
#include "cc_hash_defs.h"
#include "cc_rnd_common.h"
#include "cc_pka_defs_hw.h"
#include "cc_bitops.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
@file
@brief This file contains the CryptoCell APIs used for EC EDW (Edwards) ed25519 algorithms.
@defgroup cryptocell_ec CryptoCell EC 25519 curve APIs
@{
@ingroup cryptocell_api


@note  Algorithms of Montgomery and Edwards elliptic curves cryptography were developed by
       Daniel.J.Bernstein.
*/

/*! EC Edwards ed25519 modulus and order sizes in bits, words and bytes.  */
/*! EC Edwards modulus size in bits. */
#define CC_EC_EDW_MOD_SIZE_IN_BITS  255U   /*!<\internal MOD - EC Edw modulus size*/
/*! EC Edwards order size in bits. */
#define CC_EC_EDW_ORD_SIZE_IN_BITS  255U   /*!<\internal ORD - EC Edw generator order size*/
/*! EC Edwards modulus size in words. */
#define CC_EC_EDW_MOD_SIZE_IN_32BIT_WORDS ((CC_EC_EDW_MOD_SIZE_IN_BITS + CC_BITS_IN_32BIT_WORD - 1) / CC_BITS_IN_32BIT_WORD)
/*! EC Edwards modulus size in bytes. */
#define CC_EC_EDW_MOD_SIZE_IN_BYTES  (CC_EC_EDW_MOD_SIZE_IN_32BIT_WORDS * CC_32BIT_WORD_SIZE)
/*! EC Edwards order size in words. */
#define CC_EC_EDW_ORD_SIZE_IN_32BIT_WORDS ((CC_EC_EDW_MOD_SIZE_IN_BITS + CC_BITS_IN_32BIT_WORD - 1) / CC_BITS_IN_32BIT_WORD)
/*! EC Edwards order size in bytes. */
#define CC_EC_EDW_ORD_SIZE_IN_BYTES  (CC_EC_EDW_ORD_SIZE_IN_32BIT_WORDS * CC_32BIT_WORD_SIZE)

/*! Constant sizes of special EC_MONT buffers and arrays  */
/*! EC Edwards seed size in bytes. */
#define CC_EC_EDW_SEED_BYTES       CC_EC_EDW_MOD_SIZE_IN_BYTES
/*! EC Edwards secret key size in bytes. */
#define CC_EC_EDW_SECRET_KEY_BYTES (2 * CC_EC_EDW_MOD_SIZE_IN_BYTES)
/*! EC Edwards signatue size in bytes. */
#define CC_EC_EDW_SIGNATURE_BYTES  (2 * CC_EC_EDW_ORD_SIZE_IN_BYTES)
/*! EC Edwards scalar size in bytes. */
#define CC_EC_EDW_SCALARBYTES     CC_EC_EDW_ORD_SIZE_IN_BYTES
/*! EC Edwards scalar multiplication size in bytes. */
#define CC_EC_EDW_SCALARMULTBYTES CC_EC_EDW_MOD_SIZE_IN_BYTES

/*! EC_EDW temp buffer size definition. */
#define CC_EC_EDW_TEMP_BUFF_SIZE_IN_32BIT_WORD   (10*CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS + (sizeof(CCHashUserContext_t)+CC_32BIT_WORD_SIZE-1)/CC_32BIT_WORD_SIZE)

/*! EC_EDW temp buffer type definition. */
typedef struct {
    /*! Internal buffer. */
        uint32_t buff[CC_EC_EDW_TEMP_BUFF_SIZE_IN_32BIT_WORD];
 } CCEcEdwTempBuff_t;



/******************************************************************************/
/*!
@brief The function creates EC Edwards signature on the message.
\note Used detached form of signature, separated from the message.
      Implemented algorithm of Bernstein D. etc. sign ed25519.

@return CC_OK on success,
@return A non-zero value on failure as defined cc_ec_mont_edw_error.h or cc_hash_error.h.
*/
CIMPORT_C CCError_t CC_EcEdwSign (
                      uint8_t       *pSign,                /*!< [out] Pointer to the detached signature. */
                      size_t        *pSignSize,            /*!< [in/out] Pointer to the total size of the signature ;
                                            In  - the buffer size, which (must be at least 2*EC order size);
                                            Out - the actual size of output data. */
                      const uint8_t *pMsg,                 /*!< [in] Pointer to the message. */
                      size_t         msgSize,              /*!< [in] Message size in bytes: must be less, than
                                            (CC_HASH_UPDATE_DATA_MAX_SIZE_IN_BYTES - 2*(EC_EDW modulus size)). */
                      const uint8_t *pSignSecrKey,         /*!< [in] Pointer to the signer secret key (seed || pulKey) */
                      size_t         secrKeySize,          /*!< [in] Size of signer secret key in bytes: (must be 2*EC order size). */
                      CCEcEdwTempBuff_t *pTempBuff     /*!< [in] pointer to the temp buffer. */);



/******************************************************************************/
/*!
@brief The function verifies the EC Edwards ed25519 signature on the message.
\note The input signature is in detached form, i.e. separated from the message.

@return CC_OK on success,
@return A non-zero value on failure as defined cc_ec_mont_edw_error.h or cc_hash_error.h.
*/
CIMPORT_C CCError_t CC_EcEdwVerify(
                       const uint8_t *pSign,                /*!< [in] Pointer to detached signature, i.e. the
                                         signature is separated from the message. */
                       size_t         signSize,             /*!< [in] Size of the signature in bytes, it must be
                                         equal to two EC Order size in bytes. */
                       const uint8_t *pSignPublKey,         /*!< [in] Pointer to signer public key. */
                       size_t         publKeySize,          /*!< [in] Size of the signer public key in bytes; must be
                                         equal to EC modulus size. */
                       uint8_t       *pMsg,                 /*!< [in] Pointer to the message. */
                       size_t         msgSize,              /*!< [in] Pointer to the message size in bytes. Must be less than
                                         (CC_HASH_UPDATE_DATA_MAX_SIZE_IN_BYTES - 2*(EC_EDW modulus size)). */
                       CCEcEdwTempBuff_t *pEcEdwTempBuff    /*!< [in] Pointer to temp buffer. */);



/*******************************************************************/
/*!
@brief The function randomly generates Ec ed25519 private and public keys
       using given seed.
       The generation is performed using EC Edwards ed25519 algorithm.

@return CC_OK on success,
@return A non-zero value on failure as defined cc_ec_mont_edw_error.h or cc_hash_error.h.
*/
CIMPORT_C CCError_t CC_EcEdwSeedKeyPair (
                         const uint8_t *pSeed,                  /*!< [in] Pointer to the given seed. */
                         size_t         seedSize,               /*!< [in] Size of the seed in bytes, must be equal the EC order size
                                             in bytes. */
                         uint8_t       *pSecrKey,               /*!< [out] Pointer to the secret key, including the seed, concatenated
                                             with the public key. */
                         size_t        *pSecrKeySize,           /*!< [in/out] Pointer to the size of the secret key buffer in bytes
                                             (must be at least 2*EC order size). */
                         uint8_t       *pPublKey,               /*!< [out] Pointer to the public key. */
                         size_t        *pPublKeySize,           /*!< [in/out] Pointer to the size of the public key in bytes.
                                                In  - the size of buffer must be at least EC modulus size;
                                                Out - the actual size. */
                         CCEcEdwTempBuff_t *pTempBuff       /*!< [in] Pointer to the temp buffer, for internal use. */);

/*******************************************************************/
/*!
 @brief The function randomly generates the EC Edwards ed25519 private and
    public keys.
    The generation is performed using EC Edwards ed25519 algorithm.

@return CC_OK on success,
@return A non-zero value on failure as defined cc_ec_mont_edw_error.h, cc_hash_error.h or cc_rnd_error.
*/
CIMPORT_C CCError_t CC_EcEdwKeyPair (
                     uint8_t       *pSecrKey,               /*!< [out] Pointer to the secret key (including seed and public key). */
                     size_t        *pSecrKeySize,           /*!< [in/out] Pointer to the size of the secret key in bytes,
                                              (must be at least 2*EC order size). */
                     uint8_t       *pPublKey,               /*!< [out] Pointer to the public key. */
                     size_t        *pPublKeySize,           /*!< [in/out] - Pointer to the size of the public key in bytes.
                                            In  - the size of buffer must be at least EC modulus size;
                                            Out - the actual size. */
                     CCRndContext_t *pRndContext,           /*!< [in/out] Pointer to the RND context buffer. */
                     CCEcEdwTempBuff_t *pTempBuff           /*!< [in] Pointer to the temp buffer. */);


#ifdef __cplusplus
}
#endif
/**
@}
 */

#endif



