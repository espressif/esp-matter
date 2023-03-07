/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*!
 @addtogroup aes_ext_dma
 @{
*/

/*!
 @file
 @brief This file contains all the CryptoCell AES external DMA APIs, their
 enums and definitions.
 */


#ifndef _MBEDTLS_AES_EXT_DMA_H
#define _MBEDTLS_AES_EXT_DMA_H

#include "cc_aes_defs_proj.h"
#include "cc_pal_types.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*!
  @brief This function initializes the external DMA Control. It configures the
  AES mode, the direction (encryption or decryption), and the data size.

  @return \c CC_OK on success.
  @return A non-zero value from cc_aes_error.h on failure.
 */
int mbedtls_aes_ext_dma_init(
    /*! [in] AES key size. Valid values are: 128 bits, 192 bits, or 256 bits. */
    unsigned int keybits,
    /*! [in] 0: Encrypt. 1: Decrypt. */
    int   encryptDecryptFlag,
    /*! [in] AES mode. Supported modes are: ECB, CBC, CTR, CBC_MAC, CMAC,
    or OFB. */
    CCAesOperationMode_t operationMode
    );


/*!
  @brief This function configures the key.

  @return \c CC_OK on success.
  @return A non-zero value from cc_aes_error.h on failure.
 */
int  mbedtls_aes_ext_dma_set_key(
    /*! [in] AES mode. Supported modes are: ECB, CBC, CTR, CBC_MAC, CMAC or
    OFB. */
    CCAesOperationMode_t operationMode,
    /*! [in] The AES key buffer. */
    const unsigned char *key,
    /*! [in] The size of the AES Key. Valid values are: 128 bits, 192 bits, or
    256 bits. */
    unsigned int keybits
    );


/*!
  @brief This function configures the IV.

  @return \c CC_OK on success.
  @return A non-zero value from cc_aes_error.h on failure.
 */
int mbedtls_aes_ext_dma_set_iv(
    /*! [in] AES mode. Supported modes are: ECB, CBC, CTR, CBC_MAC, CMAC or
    OFB. */
    CCAesOperationMode_t operationMode,
    /*! [in] The AES IV buffer. */
    unsigned char       *iv,
    /*! [in] The size of the IV. Must be 16 bytes. */
    unsigned int         iv_size
    );

/*!
  @brief This function configures data size which will be written to external
  DMA interface.

  @return \c CC_OK on success.
  @return A non-zero value from cc_aes_error.h on failure.
 */
int mbedtls_aes_ext_dma_set_data_size(
    /*! [in] Size of input data in bytes. */
    uint32_t                dataSize,
    /*! [in] The AES mode. Supported modes are: ECB, CBC, CTR, CBC_MAC, CMAC
    or OFB. */
    CCAesOperationMode_t    operationMode
);


/*!
  @brief This function returns the IV after an AES CMAC or a CBCMAC operation.

  @return \c CC_OK on success.
  @return A non-zero value from cc_aes_error.h on failure.
 */
int mbedtls_aes_ext_dma_finish(
    /*! [in] The AES mode. Supported modes are: ECB, CBC, CTR, CBC_MAC, CMAC or OFB. */
    CCAesOperationMode_t operationMode,
    /*! [out] The AES IV buffer. */
    unsigned char       *iv,
    /*! [in] The size of the IV. Must be 16 bytes. */
    unsigned int         iv_size
    );

#ifdef __cplusplus
}
#endif

/*!
 @}
*/

#endif /* #ifndef MBEDTLS_AES_EXT_DMA_H */
