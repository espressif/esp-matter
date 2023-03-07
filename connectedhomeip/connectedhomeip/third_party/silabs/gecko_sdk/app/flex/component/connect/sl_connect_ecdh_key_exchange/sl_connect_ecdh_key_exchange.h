/***************************************************************************//**
 * @file
 * @brief Connect ECDH key exchange component related header.
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

#ifndef SL_CONNECT_ECDH_KEY_EXCHANGE_H_
#define SL_CONNECT_ECDH_KEY_EXCHANGE_H_

#include "psa/crypto.h"
#include "sl_connect_ecdh_key_exchange_config.h"

#define SL_CONNECT_ECDH_KEY_EXCHANGE_MAX_MESSAGE_LENGTH 120

#define SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_SECPXXXR1_SIZE_192 (192U)
#define SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_SECPXXXR1_SIZE_256 (256U)
#define SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_SECPXXXR1_SIZE_384 (384U)
#define SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_SECPXXXR1_SIZE_521 (521U)

#define SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_MONTGOMERY_SIZE_CURVE25519 (255U)
#define SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_MONTGOMERY_SIZE_CURVE448 (448U)

#define SL_CONNECT_ECDH_KEY_EXCHANGE_HKDF_SALT "Silabs"
#define SL_CONNECT_ECDH_KEY_EXCHANGE_HKDF_INFO "Connect ECDH"

#define SL_CONNECT_ECDH_KEY_EXCHANGE_SHARED_KEY_TYPE PSA_KEY_TYPE_AES
#define SL_CONNECT_ECDH_KEY_EXCHANGE_SHARED_KEY_SIZE 16

/// Key buffer size = maximum public key size + 1
#define SL_CONNECT_ECDH_KEY_EXCHANGE_PUBLIC_KEY_SIZE (133)

#define SL_CONNECT_ECDH_KEY_EXCHANGE_DEFAULT_PAN_ID (0x1ff)
#define SL_CONNECT_ECDH_KEY_EXCHANGE_DEFAULT_TX_POWER (0)

typedef uint16_t sl_connect_ecdh_state;

enum {
  SL_CONNECT_ECDH_KEY_EXCHANGE_STATE_UNDEFINED,
  SL_CONNECT_ECDH_KEY_EXCHANGE_STATE_REQUEST_KEY,
  SL_CONNECT_ECDH_KEY_EXCHANGE_STATE_SEND_KEY,
};

/**************************************************************************//**
 * Initializes the PSA crypto system
 *
 * @return PSA_SUCCESS on success, otherwise PSA error code
 *****************************************************************************/
psa_status_t sl_connect_ecdh_key_exchange_init(void);

/**************************************************************************//**
 * Generates the public/private key pair and provides the ID for the generated
 * key
 *
 * @param[out] key_id             ID of the generated key pair
 *
 * @return If success PSA_SUCCESS, otherwise PSA error code
 *****************************************************************************/
psa_status_t sl_connect_ecdh_key_exchange_generate_key_pair(psa_key_id_t *key_id);

/**************************************************************************//**
 * Exports the public key for the specified key ID of a previously generated
 * key.
 *
 * @param[in]  key_id             ID of the generated key pair
 * @param[out] public_key         Buffer for the public key to export
 * @param[in]  public_key_size    Buffer size of the public key to export
 * @param[out] public_key_length  Length of the exported public key
 *
 * @return If success PSA_SUCCESS, otherwise PSA error code
 *****************************************************************************/
psa_status_t sl_connect_ecdh_key_exchange_export_public_key(
  psa_key_id_t key_id,
  uint8_t *public_key,
  size_t public_key_size,
  size_t *public_key_length);

/**************************************************************************//**
 * Generates shared key from the private key and the peer public key.
 *
 * @param[in]  private_key_id           ID of the previously generated key pair
 * @param[in]  peer_public_key          Peer public key
 * @param[in]  peer_public_key_length   Length of the peer public key
 * @param[out] shared_key_id            Shared key ID
 *
 * @return If success PSA_SUCCESS, otherwise PSA error code
 *****************************************************************************/
psa_status_t sl_connect_ecdh_key_exchange_generate_shared_key(
  psa_key_id_t key_id,
  const uint8_t *peer_public_key,
  size_t peer_public_key_length,
  psa_key_id_t *hkdf_key_id);

/**************************************************************************//**
 * Encrypts a message with the provided key and generates the initial vector to
 * apply on the decrypt side.
 *
 * Note:  AES supports only 16 bytes of data to encrypt/decrypt. If the data to
 *        encrypt is not multiple of 16 bytes the encryption will fail.
 *
 * @param[in]  key_id               Shared key ID used to encrypt the data
 * @param[in]  plain_text           Data to encrypt
 * @param[in]  plain_text_length    Length of the data to encrypt
 * @param[out] cipher_text          Encrypted data
 * @param[in]  cipher_text_size     Buffer size of the encrypted data
 * @param[out] cipher_text_length   Length of the encrypted data
 * @param[out] iv_text              Initial vector
 * @param[in]  iv_size              Buffer size of the initial vector
 * @param[out] iv_length            Length of the initial vector
 *
 * @return If success PSA_SUCCESS, otherwise PSA error code
 *****************************************************************************/
psa_status_t sl_connect_ecdh_key_exchange_encrypt_message(
  psa_key_id_t key_id,
  const uint8_t *plain_text,
  size_t plain_text_length,
  uint8_t *cipher_text,
  size_t cipher_text_size,
  size_t *cipher_text_length,
  uint8_t *iv,
  size_t iv_size,
  size_t *iv_length);

/**************************************************************************//**
 * Decrypts a message with the provided key and initial vector.
 *
 * @param[in]  key_id               Shared key ID used to encrypt the data
 * @param[in]  iv_text              Initial vector
 * @param[in]  iv_length            Length of the initial vector
 * @param[in]  cipher_text          Encrypted data
 * @param[in]  cipher_text_length   Length of the encrypted data
 * @param[out] plain_text           Decrypted data
 * @param[in]  plain_text_size      Buffer size of the data to decrypt
 * @param[out] plain_text_length    Length of the decrypted data
 *
 * @return If success PSA_SUCCESS, otherwise PSA error code
 *****************************************************************************/
psa_status_t sl_connect_ecdh_key_exchange_decrypt_message(
  psa_key_id_t key_id,
  const uint8_t *iv,
  size_t iv_length,
  const uint8_t *cipher_text,
  size_t cipher_text_length,
  uint8_t *plain_text,
  size_t plain_text_size,
  size_t *plain_text_length);

/**************************************************************************//**
 * Destroy the previously created key and frees the allocated resources
 *
 * @param[in]  key_id ID of the key to destroy
 *
 * @return If success PSA_SUCCESS, otherwise PSA error code
 *****************************************************************************/
psa_status_t sl_connect_ecdh_key_exchange_destroy_key(psa_key_id_t key_id);

/**************************************************************************//**
 * Set the device's PAN ID
 *
 * @param PAN ID
 * @returns None
 *****************************************************************************/
void sl_connect_ecdh_key_exchange_set_pan_id(uint16_t new_id);

/**************************************************************************//**
 * Get the device's PAN ID
 *
 * @param None
 * @returns PAN_ID
 *****************************************************************************/
uint16_t sl_connect_ecdh_key_exchange_get_pan_id(void);

#endif /* SL_CONNECT_ECDH_KEY_EXCHANGE_H_ */
