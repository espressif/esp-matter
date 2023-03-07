/***************************************************************************//**
 * @file
 * @brief Accelerated mbed TLS Galois/Counter Mode (GCM) for AES-128-bit block ciphers
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
#ifndef GCM_ALT_H
#define GCM_ALT_H

/***************************************************************************//**
 * \addtogroup sl_crypto
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_crypto_gcm Accelerated GCM AES-128 Cipher
 * \brief Accelerated AES-GCM-128 cipher for the mbed TLS API using the CRYPTOACC
 *        or SE peripheral
 *
 * \{
 * This module implements the GCM AES-128 cipher, as defined in
 * <em>D. McGrew, J. Viega, The Galois/Counter Mode of Operation
 *        (GCM), Natl. Inst. Stand. Technol.</em>
 * For more information on GCM, see <em>NIST SP 800-38D: Recommendation for
 * Block Cipher Modes of Operation: Galois/Counter Mode (GCM) and GMAC</em>.
 *
 ******************************************************************************/

#if defined(MBEDTLS_GCM_ALT)
/* SiliconLabs CRYPTO hardware acceleration implementation */

#include "em_device.h"

#if defined(CRYPTO_PRESENT)
#include "em_crypto.h"
#elif defined(SEMAILBOX_PRESENT)
#include "em_se.h"
#elif defined(CRYPTOACC_PRESENT)
#include "sx_aes.h"
#include "sl_enum.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CRYPTOACC_PRESENT)
SL_ENUM(sli_gcm_mode_t) {
  SLI_GCM_ENC = 1,
  SLI_GCM_DEC = 2,
};
#endif

/**
 * \brief          The GCM context structure.
 */
typedef struct {
  unsigned int         keybits;       /*!< Size of key */
  uint64_t             len;           /*!< Total length of encrypted data. */
  uint64_t             add_len;       /*!< Total length of additional data. */

#if defined(CRYPTO_PRESENT)

  CRYPTO_DData_TypeDef key;              /*!< AES key, 128 or 256 bits */
  int                  mode;             /*!< Encryption or decryption */
  CRYPTO_TypeDef*      device;           /*!< CRYPTO device to use */
  CRYPTO_Data_TypeDef  ghash_state;      /*!< GHASH state */
  CRYPTO_Data_TypeDef  gctr_state;       /*!< GCTR counter value */
  CRYPTO_Data_TypeDef  ghash_key;        /*!< GHASH key (is a constant value
                                              which is faster to restore than
                                              to reconstruct each time). */
#elif defined(SEMAILBOX_PRESENT)
  unsigned char        key[32];          /*!< AES key 128, 192 or 256 bits */
  int                  mode;             /*!< Encryption or decryption */
  size_t               iv_len;           /*!< IV length */
  bool                 last_op;          /*!< Last streaming block identified */
  uint8_t              tagbuf[16];       /*!< Buffer for storing tag */
  uint8_t              se_ctx_enc[32];   /*!< SE GCM encryption state */
  uint8_t              se_ctx_dec[32];   /*!< SE GCM decryption state */

#elif defined(CRYPTOACC_PRESENT)
  unsigned char        key[32];          /*!< AES key 128, 192 or 256 bits */
  sli_gcm_mode_t       dir;              /*!< Encryption or decryption */
  uint8_t              sx_ctx[AES_CTX_xCM_SIZE];   /*!< CRYPTOACC GCM state */
#endif
}
mbedtls_gcm_context;

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_GCM_ALT */

/** \} (end addtogroup sl_crypto_gcm) */
/** \} (end addtogroup sl_crypto) */

#endif /* GCM_ALT_H */
