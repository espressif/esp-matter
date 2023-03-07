/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_MBEDCRYPTO_INCLUDE_H__
#define __TFM_MBEDCRYPTO_INCLUDE_H__

/* Define PSA_CRYPTO_SECURE to signal that we are compiling for the SPE */
#define PSA_CRYPTO_SECURE 1
/* Include the crypto_spe.h header before including the PSA Crypto header from
 * Mbed Crypto
 */
#include "crypto_spe.h"
#include "mbedcrypto/psa/crypto.h"

#endif /* __TFM_MBEDCRYPTO_INCLUDE_H__ */
