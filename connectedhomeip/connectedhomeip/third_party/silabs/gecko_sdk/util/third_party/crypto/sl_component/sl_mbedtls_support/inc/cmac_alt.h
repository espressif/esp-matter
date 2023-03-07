/***************************************************************************//**
 * @file
 * @brief Accelerated mbed TLS AES-CMAC cipher
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
#ifndef CMAC_ALT_H
#define CMAC_ALT_H

/***************************************************************************//**
 * \addtogroup sl_crypto
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_crypto_cmac Accelerated AES-CMAC Cipher
 * \brief Accelerated AES-CMAC cipher for the mbed TLS API using the CRYPTOACC or
 *        SE peripheral. This implementation builds on the PSA Crypto drivers
 *        (\ref sl_psa_drivers).
 *
 * \{
 ******************************************************************************/
#if defined(MBEDTLS_CMAC_ALT)

#ifdef __cplusplus
extern "C" {
#endif

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)
#include "sli_se_transparent_types.h"
#define SL_MAC_OPERATION_CTX_TYPE sli_se_transparent_mac_operation_t
#elif defined(CRYPTO_PRESENT)
#include "sli_crypto_transparent_types.h"
#define SL_MAC_OPERATION_CTX_TYPE sli_crypto_transparent_mac_operation_t
#elif defined(CRYPTOACC_PRESENT)
#include "sli_cryptoacc_transparent_types.h"
#define SL_MAC_OPERATION_CTX_TYPE sli_cryptoacc_transparent_mac_operation_t
#endif

struct mbedtls_cmac_context_t {
  SL_MAC_OPERATION_CTX_TYPE ctx;
};

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_CMAC_ALT */

/** \} (end addtogroup sl_crypto_cmac) */
/** \} (end addtogroup sl_crypto) */

#endif /* CMAC_ALT_H */
