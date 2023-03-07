/***************************************************************************//**
 * @file
 * @brief Accelerated mbed TLS AES block cipher
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
#ifndef AES_ALT_H
#define AES_ALT_H

/***************************************************************************//**
 * \addtogroup sl_crypto
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_crypto_aes Accelerated AES Block Cipher
 * \brief Accelerated AES block cipher for the mbed TLS API using the AES, CRYPTO,
 *        CRYPTOACC or SE peripheral
 *
 * \{
 ******************************************************************************/

#if defined(MBEDTLS_AES_ALT)
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          AES context structure
 */
typedef struct {
  unsigned int                    keybits;     /*!<  size of key */
  unsigned char                   key[32];     /*!<  AES key 128, 192 or 256 bits */
}
mbedtls_aes_context;

#if defined(MBEDTLS_CIPHER_MODE_XTS)
/**
 * \brief The AES XTS context-type definition.
 */
typedef struct mbedtls_aes_xts_context{
  mbedtls_aes_context crypt;   /*!< The AES context to use for AES block
                                        encryption or decryption. */
  mbedtls_aes_context tweak;   /*!< The AES context used for tweak
                                        computation. */
} mbedtls_aes_xts_context;
#endif /* MBEDTLS_CIPHER_MODE_XTS */

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_AES_ALT */

/** \} (end addtogroup sl_crypto_aes) */
/** \} (end addtogroup sl_crypto) */

#endif /* AES_ALT_H */
