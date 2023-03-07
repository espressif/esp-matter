/***************************************************************************//**
 * @file
 * @brief ECDH Key exchange component related source.
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
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <string.h>
#include "sl_gsdk_version.h"
#include "sl_connect_ecdh_key_exchange.h"
#include "ember-types.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static uint8_t data[SL_CONNECT_ECDH_KEY_EXCHANGE_MAX_MESSAGE_LENGTH];
// PAN ID of the device
static uint16_t pan_id;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
psa_status_t sl_connect_ecdh_key_exchange_init(void)
{
  return psa_crypto_init();
}

psa_status_t sl_connect_ecdh_key_exchange_generate_key_pair(psa_key_id_t *key_id)
{
  if (key_id == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t psa_status;
  psa_key_attributes_t key_attr;

  size_t key_size = (SL_CONNECT_ECDH_KEY_EXCHANGE_ECC_FAMILY == PSA_ECC_FAMILY_SECP_R1) ? SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_SECPXXXR1_SIZE : SL_CONNECT_ECDH_KEY_EXCHANGE_CURVE_MONTGOMERY_SIZE;
  key_attr = psa_key_attributes_init();
  psa_set_key_type(&key_attr, PSA_KEY_TYPE_ECC_KEY_PAIR(SL_CONNECT_ECDH_KEY_EXCHANGE_ECC_FAMILY));
  psa_set_key_bits(&key_attr, key_size);
  psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_DERIVE);
  psa_set_key_algorithm(&key_attr, PSA_ALG_ECDH);

#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  psa_set_key_lifetime(&key_attr, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_VOLATILE, 0x01));
#else
  psa_set_key_lifetime(&key_attr, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_VOLATILE, 0x00));
#endif

  psa_status = psa_generate_key(&key_attr, key_id);
  psa_reset_key_attributes(&key_attr);

  return psa_status;
}

psa_status_t sl_connect_ecdh_key_exchange_export_public_key(
  psa_key_id_t key_id,
  uint8_t *public_key,
  size_t public_key_size,
  size_t *public_key_length)
{
  if ((key_id == 0)
      || (public_key == NULL)
      || (public_key_size == 0)
      || (public_key_length == NULL)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  return psa_export_public_key(key_id, public_key, public_key_size, public_key_length);
}

psa_status_t sl_connect_ecdh_key_exchange_generate_shared_key(
  psa_key_id_t key_id,
  const uint8_t *peer_public_key,
  size_t peer_public_key_length,
  psa_key_id_t *hkdf_key_id)
{
  if ((key_id == 0)
      || (peer_public_key == NULL)
      || (peer_public_key_length == 0)
      || (hkdf_key_id == NULL)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  psa_key_attributes_t key_attr;
  psa_status_t psa_status;
  uint8_t hkdf_salt[] = SL_CONNECT_ECDH_KEY_EXCHANGE_HKDF_SALT;
  uint8_t hkdf_info[] = SL_CONNECT_ECDH_KEY_EXCHANGE_HKDF_INFO;

  key_attr = psa_key_attributes_init();
  psa_set_key_type(&key_attr, SL_CONNECT_ECDH_KEY_EXCHANGE_SHARED_KEY_TYPE);
  psa_set_key_bits(&key_attr, PSA_BYTES_TO_BITS(SL_CONNECT_ECDH_KEY_EXCHANGE_SHARED_KEY_SIZE));

  // Add PSA_KEY_USAGE_EXPORT to permit export of the key for testing purposes
  psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);

  psa_set_key_algorithm(&key_attr, PSA_ALG_CBC_NO_PADDING);

  psa_key_derivation_operation_t kdf_op;
  kdf_op = psa_key_derivation_operation_init();

  psa_status = psa_key_derivation_setup(&kdf_op, PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(SL_CONNECT_ECDH_KEY_EXCHANGE_HASH_ALG)));
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = psa_key_derivation_set_capacity(&kdf_op, SL_CONNECT_ECDH_KEY_EXCHANGE_SHARED_KEY_SIZE);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = psa_key_derivation_input_bytes(&kdf_op, PSA_KEY_DERIVATION_INPUT_SALT, hkdf_salt, sizeof(hkdf_salt));
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = psa_key_derivation_input_bytes(&kdf_op, PSA_KEY_DERIVATION_INPUT_INFO, hkdf_info, sizeof(hkdf_info));
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = psa_key_derivation_key_agreement(&kdf_op, PSA_KEY_DERIVATION_INPUT_SECRET, key_id, peer_public_key, peer_public_key_length);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = psa_key_derivation_output_key(&key_attr, &kdf_op, hkdf_key_id);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = psa_key_derivation_abort(&kdf_op);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  return PSA_SUCCESS;
}

psa_status_t sl_connect_ecdh_key_exchange_encrypt_message(
  psa_key_id_t key_id,
  const uint8_t *plain_text,
  size_t plain_text_length,
  uint8_t *cipher_text,
  size_t cipher_text_size,
  size_t *cipher_text_length,
  uint8_t *iv,
  size_t iv_size,
  size_t *iv_length)
{
  if ((key_id == 0)
      || (plain_text == NULL)
      || (plain_text_length == 0)
      || (cipher_text == NULL)
      || (cipher_text_length == NULL)
      || (iv == NULL)
      || (iv_size == 0)
      || (iv_length == NULL)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t psa_status;

#if SL_GSDK_VERSION >= 0x400

  // single part operation is only supported in GSDK 4.0+
  (void) cipher_text_size;
  psa_status = psa_cipher_encrypt(
    key_id,
    PSA_ALG_CBC_NO_PADDING,
    plain_text,
    plain_text_length,
    data,
    sizeof(data),
    cipher_text_length);

  if (psa_status == PSA_SUCCESS) {
    memcpy(cipher_text, data, *cipher_text_length - iv_size);
    memcpy(iv, data + *cipher_text_length - iv_size, iv_size);
    *cipher_text_length -= iv_size;
    *iv_length = iv_size;
  }
  return psa_status;

#else
  if (cipher_text_size == 0) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  size_t cipher_text_length_finish = 0;

  psa_cipher_operation_t cipher_op;
  cipher_op = psa_cipher_operation_init();

  psa_status = psa_cipher_encrypt_setup(&cipher_op, key_id, PSA_ALG_CBC_NO_PADDING);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = psa_cipher_generate_iv(&cipher_op, iv, iv_size, iv_length);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = psa_cipher_update(
    &cipher_op,
    plain_text,
    plain_text_length,
    cipher_text,
    cipher_text_size,
    cipher_text_length);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = psa_cipher_finish(
    &cipher_op,
    cipher_text + *cipher_text_length,
    cipher_text_size - *cipher_text_length,
    &cipher_text_length_finish);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  return PSA_SUCCESS;

#endif
}

psa_status_t sl_connect_ecdh_key_exchange_decrypt_message(
  psa_key_id_t key_id,
  const uint8_t *iv,
  size_t iv_length,
  const uint8_t *cipher_text,
  size_t cipher_text_length,
  uint8_t *plain_text,
  size_t plain_text_size,
  size_t *plain_text_length)
{
  if ((key_id == 0)
      || (iv == NULL)
      || (iv_length == 0)
      || (cipher_text == NULL)
      || (cipher_text_length == 0)
      || (plain_text == NULL)
      || (plain_text_size == 0)
      || (plain_text_length == NULL)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  psa_status_t psa_status;

#if SL_GSDK_VERSION >= 0x400

  // single part operation is only supported in GSDK 4.0+
  memcpy(data, cipher_text, cipher_text_length);
  memcpy(data + cipher_text_length, iv, iv_length);

  psa_status = psa_cipher_decrypt(
    key_id,
    PSA_ALG_CBC_NO_PADDING,
    data,
    cipher_text_length + iv_length,
    plain_text,
    plain_text_size,
    plain_text_length);

  return psa_status;

#else

  size_t plain_text_length_finish = 0;

  psa_cipher_operation_t cipher_op;
  cipher_op = psa_cipher_operation_init();

  psa_status = psa_cipher_decrypt_setup(&cipher_op, key_id, PSA_ALG_CBC_NO_PADDING);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = psa_cipher_set_iv(&cipher_op, iv, iv_length);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = psa_cipher_update(
    &cipher_op,
    cipher_text,
    cipher_text_length,
    plain_text,
    plain_text_size,
    plain_text_length);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = psa_cipher_finish(
    &cipher_op,
    plain_text + *plain_text_length,
    plain_text_size - *plain_text_length,
    &plain_text_length_finish);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  return PSA_SUCCESS;

#endif
}

psa_status_t sl_connect_ecdh_key_exchange_destroy_key(psa_key_id_t key_id)
{
  if (key_id == 0) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  return psa_destroy_key(key_id);
}

/**************************************************************************//**
 * Set the device's PAN ID
 *****************************************************************************/
void sl_connect_ecdh_key_exchange_set_pan_id(uint16_t new_id)
{
  pan_id = new_id;
}

/**************************************************************************//**
 * Get the device's PAN ID
 *****************************************************************************/
uint16_t sl_connect_ecdh_key_exchange_get_pan_id(void)
{
  return pan_id;
}
