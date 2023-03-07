/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup eddsa
 @{
 */

/*!
 @file

 @brief This file contains the CryptoCell EDDSA Edwards curve APIs.

 This API supports EDDSA Edwards for generating, signing and verifying keys.
 This is implemented based on <em>Ed25519: High-speed high-security
 signatures</em>.
 */

#ifndef _MBEDTLS_ECDSA_EDWARDS_H
#define _MBEDTLS_ECDSA_EDWARDS_H


#include "cc_pal_types.h"
#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*************************** Defines *****************************************/

/*************************** Typedefs  ***************************************/

/*************************** Enums *******************************************/

/*************************** Structs  ****************************************/

/*************************** context Structs  ********************************/

/*!
 @brief This function generates an EDDSA keypair on the Edwards 25519 curve.

 @return    \c 0 on success.
 @return    An \c MBEDTLS_ERR_ECP_XXX code on failure.
 */
int mbedtls_ecdsa_genkey_edwards(
        /*! The EDDSA context to store the keypair in. */
        mbedtls_ecdsa_context *ctx,
        /*! The elliptic curve to use. Currently only 25519 curve is
        supported. */
        mbedtls_ecp_group_id gid,
        /*! The RNG function. */
        int (*f_rng)(void *, unsigned char *, size_t),
        /*! The RNG context. */
        void *p_rng
        );

/*!
   @brief           This function computes the EDDSA signature of a
                    previously-hashed message.

   @note            If the bitlength of the message hash is larger than the
                    bitlength of the group order, then the hash is truncated
                    as defined in <em>Standards for Efficient Cryptography Group
                    (SECG): SEC1 Elliptic Curve Cryptography</em>, section
                    4.1.3, step 5.

   @return          \c 0 on success.
   @return          An \c MBEDTLS_ERR_ECP_XXX or \c MBEDTLS_MPI_XXX error code
                    on failure.
 */
int mbedtls_ecdsa_sign_edwards(
            /*! The ECP group. */
            mbedtls_ecp_group *grp,
            /*! The first output integer. */
            mbedtls_mpi *r,
            /*! The second output integer. */
            mbedtls_mpi *s,
            /*! The private signing key. */
            const mbedtls_mpi *d,
            /*! The message hash. */
            const unsigned char *buf,
            /*! The length of \p buf. */
            size_t blen
            );


/*!
   @brief           This function verifies the EDDSA signature of a
                    previously-hashed message.

   @note            If the bitlength of the message hash is larger than the
                    bitlength of the group order, then the hash is truncated as
                    defined in <em>Standards for Efficient Cryptography Group
                    (SECG): SEC1 Elliptic Curve Cryptography</em>, section
                    4.1.4, step 3.

   @return          \c 0 on success.
   @return          \c MBEDTLS_ERR_ECP_BAD_INPUT_DATA if signature is invalid.
   @return          An \c MBEDTLS_ERR_ECP_XXX or \c MBEDTLS_MPI_XXX
                    error code on failure for any other reason.
  */
int mbedtls_ecdsa_verify_edwards(
            /*! The ECP group. */
            mbedtls_ecp_group *grp,
            /*!The message hash . */
            const unsigned char *buf,
            /*! The length of \p buf. */
            size_t blen,
            /*! The public key to use for verification. */
            const mbedtls_ecp_point *Q,
            /*! The first integer of the signature. */
            const mbedtls_mpi *r,
            /*! The second integer of the signature. */
            const mbedtls_mpi *s
            );

/**
   @brief           This function imports an EC Edwards public key.

   @return          \c 0 on success.
   @return          \c MBEDTLS_ERR_ECP_BAD_INPUT_DATA
                    or \c MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE on failure.
 */
int mbedtls_ecdsa_public_key_read_edwards(
            /*! [out] The public key to import. */
            mbedtls_ecp_point *Q,
            /*! [in] The buffer to read the public key from. */
            unsigned char *buf,
            /*! [in] The length of the buffer in bytes. */
            size_t blen
            );

/**
   @brief           This function exports an EC Edwards public key.

   @return          \c 0 on success.
   @return          \c MBEDTLS_ERR_ECP_BAD_INPUT_DATA
                    or \c MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL on failure.
 */
int mbedtls_ecdsa_public_key_write_edwards(
            /*! [in] The public key to export. */
            const mbedtls_ecp_point *Q,
            /*! [out] The length of the data written in bytes. */
            size_t *olen,
            /*! [out] The buffer to write the public key to. */
            unsigned char *buf,
            /*! [in] The length of the buffer in bytes. */
            size_t blen
            );


 #ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif /* _MBEDTLS_ECDSA_EDWARDS_H */

