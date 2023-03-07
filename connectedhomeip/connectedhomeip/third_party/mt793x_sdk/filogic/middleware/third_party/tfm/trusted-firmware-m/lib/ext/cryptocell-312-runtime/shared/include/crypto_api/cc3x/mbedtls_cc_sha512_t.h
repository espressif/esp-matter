/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_sha512_t_h
 @{
 */


/*!
 @file
 @brief This file contains all of the CryptoCell SHA-512 truncated APIs, their
 enums and definitions.
 */

#ifndef _MBEDTLS_CC_SHA512_T_H
#define _MBEDTLS_CC_SHA512_T_H

#include <sha512.h>

/*!
   @brief This function initializes the SHA-512_t context.
 */
void mbedtls_sha512_t_init(
    /*! The SHA-512_t context to initialize. */
    mbedtls_sha512_context *ctx
      );

/*!
   @brief This function clears the SHA-512_t context.
 */
void mbedtls_sha512_t_free(
    /*! The SHA-512_t context to clear. */
    mbedtls_sha512_context *ctx
       );

/*!
   @brief This function starts a SHA-512_t checksum calculation.
 */
void mbedtls_sha512_t_starts(
    /*! The SHA-512_t context to initialize. */
    mbedtls_sha512_context *ctx,
    /*! Determines which function to use: 0: Use SHA-512/256, or 1:
    Use SHA-512/224. */
    int is224
        );

/*!
   @brief This function feeds an input buffer into an ongoing SHA-512_t
   checksum calculation.
 */
void mbedtls_sha512_t_update(
    /*! The SHA-512_t context. */
    mbedtls_sha512_context *ctx,
    /*! The buffer holding the input data. */
    const unsigned char *input,
    /*! The length of the input data. */
    size_t ilen
          );

/*!
   @brief   This function finishes the SHA-512_t operation, and writes
    the result to the output buffer.

    <ul><li>For SHA512/224, the output buffer will include
    the 28 leftmost bytes of the SHA-512 digest.</li>
    <li>For SHA512/256, the output buffer will include
    the 32 leftmost bytes of the SHA-512 digest.</li></ul>
    */
void mbedtls_sha512_t_finish(
    /*! The SHA-512_t context. */
    mbedtls_sha512_context *ctx,
    /*! The SHA-512/256 or SHA-512/224 checksum result. */
    unsigned char output[32],
    /*! Determines which function to use: 0: Use SHA-512/256, or 1:
    Use SHA-512/224. */
    int is224
          );

/*!
   @brief      This function calculates the SHA-512 checksum of a buffer.

    The function performs the following operations:
    <ul><li>Allocates the context.<li><li>Calculates
    the checksum.</li><li>Frees the context.</li></ul>
    The SHA-512 result is calculated as
    output = SHA-512(input buffer).
*/
void mbedtls_sha512_t(
    /*! The buffer holding the input data. */
    const unsigned char *input,
    /*! The length of the input data. */
    size_t ilen,
    /*! The SHA-512/256 or SHA-512/224 checksum result. */
    unsigned char output[32],
    /*! Determines which function to use: 0: Use SHA-512/256, or 1:
    Use SHA-512/224. */
    int is224
      );

/*!
 @}
 */
#endif
