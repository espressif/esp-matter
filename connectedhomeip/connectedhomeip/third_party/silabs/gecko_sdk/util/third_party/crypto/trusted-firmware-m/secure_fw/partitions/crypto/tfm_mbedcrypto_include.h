/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_MBEDCRYPTO_INCLUDE_H__
#define __TFM_MBEDCRYPTO_INCLUDE_H__

/* Define PSA_CRYPTO_SECURE to signal that we are compiling for the SPE */
#define PSA_CRYPTO_SECURE 1

#define SUPPRESS_UNUSED_IOVEC_PARAM_WARNING() \
  (void)in_vec;                               \
  (void)in_len;                               \
  (void)out_vec;                              \
  (void)out_len;

/* Include the crypto_spe.h header before including the PSA Crypto header from
 * Mbed Crypto
 */
#if !defined(TFM_CONFIG_SL_SECURE_LIBRARY)
#include "crypto_spe.h"
#endif
#include "psa/crypto.h"

#if defined(TFM_CONFIG_SL_SECURE_LIBRARY)
#include "psa_crypto_config_autogen.h"
#endif

#endif /* __TFM_MBEDCRYPTO_INCLUDE_H__ */
