/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/*!
 @addtogroup hash_ext_dma
 @{
 */

/*!
 @file
 @brief This file contains all the CryptoCell hash external DMA APIs, their
 enums and definitions.
 */

#ifndef _MBEDTLS_HASH_EXT_DMA_H
#define _MBEDTLS_HASH_EXT_DMA_H

#include "cc_pal_types.h"
#include "cc_hash_defs.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*!
 @brief This function initializes the External DMA Control.

 It configures the hash mode, the initial hash value, and other configurations.

 @return \c CC_OK on success.
 @return A non-zero value on failure.
 */
int mbedtls_hash_ext_dma_init(
    /*! [in] The hash mode. Supported modes are: SHA1, SHA224 or SHA256. */
    CCHashOperationMode_t  operationMode,
    /*! [in] Input data size in bytes. */
    uint32_t                dataSize
    );

/*!
  @brief This function returns the digest after the hash operation, and frees
  used resources.

  @return \c CC_OK on success.
  @return A non-zero value on failure.
 */
int mbedtls_hash_ext_dma_finish(
    /*! [in] The hash mode. Supported modes are: SHA1, SHA224 or SHA256. */
    CCHashOperationMode_t  operationMode,
    /*! [in] The size of the hash digest in bytes. */
    uint32_t digestBufferSize,
    /*! [out] The output digest buffer. */
    uint32_t *digestBuffer
    );

#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif /* #ifndef MBEDTLS_HASH_EXT_DMA_H_ */
