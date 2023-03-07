/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_aesccm_star_common
 @{
 */

/*!
 @file
 @brief This file contains the common definitions of the CryptoCell AES-CCM star APIs.
 */



#ifndef _MBEDTLS_CCM_COMMON_H
#define _MBEDTLS_CCM_COMMON_H

#ifdef __cplusplus
extern "C"
{
#endif

/*! The size of the AES CCM star nonce in bytes. */
#define MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES               13
/*! The size of source address of the AES CCM star in bytes. */
#define MBEDTLS_AESCCM_STAR_SOURCE_ADDRESS_SIZE_BYTES      8

/*! AES CCM mode: CCM. */
#define MBEDTLS_AESCCM_MODE_CCM             0
/*! AES CCM mode: CCM star. */
#define MBEDTLS_AESCCM_MODE_STAR            1

#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif /* _MBEDTLS_CCM_COMMON_H */

