/***************************************************************************//**
 * @file
 * @brief Accelerated cryptographic primitives using the CRYPTO and RADIOAES
 *        peripherals, for series-1 and series-2 respectively.
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
#ifndef SLI_PROTOCOL_CRYPTO_H
#define SLI_PROTOCOL_CRYPTO_H

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/***************************************************************************//**
 * @addtogroup sli_protocol_crypto
 * @brief Accelerated cryptographic primitives using the CRYPTO and RADIOAES
 *        peripherals, for series-1 and series-2 respectively.
 * @{
 ******************************************************************************/

#include "sl_status.h"
#include "em_device.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @brief          AES-CTR block encryption/decryption optimized for radio
 *
 * @param key      AES key
 * @param keybits  must be 128 or 256
 * @param input    16-byte input block
 * @param iv_in    16-byte counter/IV starting value
 * @param iv_out   16-byte counter/IV output after block round
 * @param output   16-byte output block
 *
 * @return         SL_STATUS_OK if successful, relevant status code on error
 ******************************************************************************/
sl_status_t sli_aes_crypt_ctr_radio(const unsigned char    *key,
                                    unsigned int           keybits,
                                    const unsigned char    input[16],
                                    const unsigned char    iv_in[16],
                                    volatile unsigned char iv_out[16],
                                    volatile unsigned char output[16]);

/***************************************************************************//**
 * @brief          AES-ECB block encryption/decryption optimized for radio
 *
 * @param encrypt  true for encryption, false for decryption
 * @param key      AES key
 * @param keybits  must be 128 or 256
 * @param input    16-byte input block
 * @param output   16-byte output block
 *
 * @return         SL_STATUS_OK if successful, relevant status code on error
 ******************************************************************************/
sl_status_t sli_aes_crypt_ecb_radio(bool                   encrypt,
                                    const unsigned char    *key,
                                    unsigned int           keybits,
                                    const unsigned char    input[16],
                                    volatile unsigned char output[16]);

#if defined(RADIOAES_PRESENT)
/***************************************************************************//**
 * @brief          AES-CMAC calculation optimized for radio
 *
 * @param key      AES key
 * @param keybits  Must be 128 or 256
 * @param input    Input buffer containing the message to be signed
 * @param length   Amount of bytes in the input buffer
 * @param output   16-byte output block for calculated CMAC
 *
 * @return         SL_STATUS_OK if successful, relevant status code on error
 ******************************************************************************/
sl_status_t sli_aes_cmac_radio(const unsigned char    *key,
                               unsigned int           keybits,
                               const unsigned char    *input,
                               unsigned int           length,
                               volatile unsigned char output[16]);

/***************************************************************************//**
 * @brief         Seeds the AES mask. It is recommended to call this function
                  during initialization in order to avoid taking the potential
                  hit of requesting RNG output in an IRQ context.
 ******************************************************************************/
void sli_aes_seed_mask(void);
#endif

/***************************************************************************//**
 * @brief          CCM buffer authenticated decryption optimized for BLE
 *
 * @param data     Input/output buffer of payload data of BLE packet
 * @param length   length of input data
 * @param iv       nonce (initialization vector)
 *                 must be 13 bytes
 * @param header   header of BLE packet (1 byte)
 * @param tag      authentication tag of BLE packet (4 bytes)
 *
 * @return         SL_STATUS_OK if successful and authenticated,
 *                 SL_STATUS_INVALID_SIGNATURE if tag does not match payload,
 *                 relevant status code on other error
 ******************************************************************************/
sl_status_t sli_ccm_auth_decrypt_ble(unsigned char       *data,
                                     size_t              length,
                                     const unsigned char *key,
                                     const unsigned char *iv,
                                     unsigned char       header,
                                     unsigned char       *tag);

/***************************************************************************//**
 * @brief          CCM buffer encryption optimized for BLE
 *
 * @param data     Input/output buffer of payload data of BLE packet
 * @param length   length of input data
 * @param iv       nonce (initialization vector)
 *                 must be 13 bytes
 * @param header   header of BLE packet (1 byte)
 * @param tag      buffer where the BLE packet tag (4 bytes) will be written
 *
 * @return         SL_STATUS_OK if successful, relevant status code on error
 ******************************************************************************/
sl_status_t sli_ccm_encrypt_and_tag_ble(unsigned char       *data,
                                        size_t              length,
                                        const unsigned char *key,
                                        const unsigned char *iv,
                                        unsigned char       header,
                                        unsigned char       *tag);

/***************************************************************************//**
 * @brief          CCM buffer authenticated decryption optimized for Zigbee
 *
 * @param data     Input/output buffer of payload data (decrypt-in-place)
 * @param length   length of input data
 * @param iv       nonce (initialization vector)
 *                 must be 13 bytes
 * @param aad      Input buffer of Additional Authenticated Data
 * @param aad_len  Length of buffer @p aad
 * @param tag      authentication tag
 * @param tag_len  Length of authentication tag
 *
 * @return         SL_STATUS_OK if successful and authenticated,
 *                 SL_STATUS_INVALID_SIGNATURE if tag does not match payload,
 *                 relevant status code on other error
 ******************************************************************************/
sl_status_t sli_ccm_zigbee(bool encrypt,
                           const unsigned char *data_in,
                           unsigned char       *data_out,
                           size_t              length,
                           const unsigned char *key,
                           const unsigned char *iv,
                           const unsigned char *aad,
                           size_t              aad_len,
                           unsigned char       *tag,
                           size_t              tag_len);

/***************************************************************************//**
 * @brief          Process a table of BLE RPA device keys and look for a
 *                 match against the supplied hash
 *
 * @param keytable Pointer to an array of AES-128 keys, corresponding to the
 *                 per-device key in the BLE RPA process
 * @param keymask  Bitmask indicating with key indices in keytable are valid
 * @param prand    24-bit BLE nonce to encrypt with each key and match against hash
 * @param hash     BLE RPA hash to match against (last 24 bits of AES result)
 *
 * @return         0-based index of matching key if a match is found, -1 for no match.
 ******************************************************************************/
int sli_process_ble_rpa(const unsigned char keytable[],
                        uint32_t            keymask,
                        uint32_t            prand,
                        uint32_t            hash);

#ifdef __cplusplus
}
#endif

/// @} (end addtogroup sli_protocol_crypto)
/// @endcond
#endif // SLI_PROTOCOL_CRYPTO_H
