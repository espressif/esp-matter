/***************************************************************************//**
 * @file
 * @brief Silicon Labs Series-1 PSA Crypto TRNG driver functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef SLI_CRYPTO_TRNG_DRIVER_H
#define SLI_CRYPTO_TRNG_DRIVER_H

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#include "em_device.h"
#include "psa/crypto.h"

#if defined(TRNG_PRESENT) \
  && (defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG) || defined(MBEDTLS_ENTROPY_HARDWARE_ALT))

psa_status_t sli_crypto_trng_get_random(unsigned char *output,
                                        size_t len,
                                        size_t *out_len);

#endif /* TRNG_PRESENT && (MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG || MBEDTLS_ENTROPY_HARDWARE_ALT) */

#ifdef __cplusplus
}
#endif

/// @endcond

#endif // SLI_CRYPTO_TRNG_DRIVER_H
