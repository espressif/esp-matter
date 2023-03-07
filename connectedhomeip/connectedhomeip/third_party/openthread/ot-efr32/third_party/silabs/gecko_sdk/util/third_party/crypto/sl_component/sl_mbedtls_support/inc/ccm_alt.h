/***************************************************************************//**
 * @file
 * @brief Accelerated mbed TLS AES-CCM AEAD cipher
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
#ifndef CCM_ALT_H
#define CCM_ALT_H

/***************************************************************************//**
 * \addtogroup sl_crypto
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_crypto_ccm Accelerated AES-CCM AEAD Cipher
 * \brief Accelerated AES-CCM AEAD cipher for the mbed TLS API using the CRYPTOACC
 *         or SE peripheral
 *
 * \{
 ******************************************************************************/
#if defined(MBEDTLS_CCM_ALT)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief    The CCM context-type definition. The CCM context is passed
 *           to the APIs called.
 */
typedef struct {
  unsigned char key[32];      /*!< The key in use. */
  unsigned int keybits;
}
mbedtls_ccm_context;

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_CCM_ALT */

/** \} (end addtogroup sl_crypto_ccm) */
/** \} (end addtogroup sl_crypto) */

#endif /* CCM_ALT_H */
