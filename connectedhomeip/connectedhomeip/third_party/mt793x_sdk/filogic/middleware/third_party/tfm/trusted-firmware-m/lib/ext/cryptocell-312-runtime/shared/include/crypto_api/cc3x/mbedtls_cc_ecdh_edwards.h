/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup ecdh_edwards
 @}
 */

/*!
 @file

 @brief This file contains the CryptoCell ECDH Edwards curve APIs.
 */

#ifndef ECDH_EDWARDS_H
#define ECDH_EDWARDS_H

#ifdef __cplusplus
extern "C"
{
#endif


#include "mbedtls/ecp.h"

/*************************** Defines *****************************************/

/*************************** Typedefs  ***************************************/

/*************************** Enums *******************************************/

/*************************** Structs  ****************************************/

/*************************** context Structs  ********************************/

/*!
   @brief   This function generates a public key and a TLS ServerKeyExchange
   payload.

   This is the first function used by a TLS server for ECDHE ciphersuites.

   @note    This function can be used only for curve 25519.

   @note    This function assumes that the ECP group (\c grp) of the
            \p ctx context has already been properly set,
            for example, using mbedtls_ecp_group_load().

   @see     ecp.h

   @return  \c 0 on success.
   @return  An \c MBEDTLS_ERR_ECP_XXX error code on failure.
 */

int mbedtls_ecdh_make_params_edwards(
        /*! The ECDH context. */
        mbedtls_ecdh_context *ctx,
        /*! The number of characters written. */
        size_t *olen,
        /*! The destination buffer. */
        unsigned char *buf,
        /*! The length of the destination buffer. */
        size_t blen,
        /*! The RNG function. */
        int (*f_rng)(void *, unsigned char *, size_t),
        /*! The RNG context. */
        void *p_rng
                      );

/*!
   @brief   This function parses and processes a TLS ServerKeyExhange
            payload.

            This is the first function used by a TLS client for ECDHE ciphersuites.

   @note    This function can be used only for curve 25519.

   @see     ecp.h

   @return  \c 0 on success.
   @return  An \c MBEDTLS_ERR_ECP_XXX error code on failure.
 */
int mbedtls_ecdh_read_params_edwards(
        /*! The ECDH context. */
        mbedtls_ecdh_context *ctx,
        /*! The pointer to the start of the input buffer. */
        const unsigned char **buf,
        /*! The address for one byte past the end of the buffer. */
        const unsigned char *end
        );


#ifdef __cplusplus
}
#endif

/*!
@}
*/
#endif  /* MBEDTLS_ECDH_EDWARDS */
