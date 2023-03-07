/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup chacha_ext_dma

 @{
 */

/*!
 @file
 @brief This file contains all the CryptoCell ChaCha external DMA APIs, their
 enums and definitions.
 */

#ifndef _MBEDTLS_CHACHA_EXT_DMA_H
#define _MBEDTLS_CHACHA_EXT_DMA_H

#include "cc_pal_types.h"
#include "mbedtls_cc_chacha.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*!
  @brief This function initializes the external DMA control.

  It configures the ChaCha mode, the initial hash value, and other
  configurations in the ChaCha engine.

  @return \c 0 on success.
  @return A non-zero value from mbedtls_ext_dma_error.h on failure.
 */
int mbedtls_ext_dma_chacha_init(
            /*! [in] The nonce buffer.  */
            uint8_t *  pNonce,
            /*! [in] The nonce size flag.  */
            mbedtls_chacha_nonce_size_t         nonceSizeFlag,
            /*! [in] The key buffer.  */
            uint8_t *  pKey,
            /*! [in] The size of the key buffer. Must be 32 bytes.  */
            uint32_t    keySizeBytes,
            /*! [in] Initial counter value.  */
            uint32_t    initialCounter,
            /*! [in] The ChaCha operation: Encrypt or Decrypt. */
            mbedtls_chacha_encrypt_mode_t  EncryptDecryptFlag,
            /*! [in] Input data length in bytes */
            uint32_t    dataSize
            );


/*!
  @brief This function frees used resources.

  @return \c CC_OK on success.
  @return A non-zero value from mbedtls_ext_dma_error.h on failure.
 */
int mbedtls_chacha_ext_dma_finish(void);



#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif /* #ifndef _MBEDTLS_CHACHA_EXT_DMA_H */


