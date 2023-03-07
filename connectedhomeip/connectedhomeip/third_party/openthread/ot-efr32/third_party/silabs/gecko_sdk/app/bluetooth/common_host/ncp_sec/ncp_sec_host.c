/***************************************************************************//**
 * @file
 * @brief Secure NCP host program
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdio.h>
#include <stdint.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ec.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include "app_log.h"

#include "sl_bt_api.h"
#include "sl_common.h"
#include "ncp_sec_host.h"

typedef struct conn_nonce{
  uint32_t counter;
  uint8_t counter_hi;
  uint8_t host_iv[IV_SIZE];
  uint8_t target_iv[IV_SIZE];
}conn_nonce_t;

typedef struct ec_keypair{
  uint8_t pub[PUBLIC_KEYPAIR_SIZE];
  uint8_t priv[ECDH_PRIVATE_KEY_SIZE];
}ec_keypair_t;

static uint8_t ccm_key[AES_CCM_KEY_SIZE];
static ec_keypair_t local_ec_key;
static conn_nonce_t sec_counter_in;
static conn_nonce_t sec_counter_out;

security_state_t security_state = SECURITY_STATE_UNDEFINED;

extern int sl_bgapi_user_cmd_increase_security(uint8_t *public_key,
                                               uint8_t *host_iv_to_target,
                                               uint8_t *host_iv_to_host);

SL_WEAK int sl_bgapi_user_cmd_increase_security(uint8_t *public_key,
                                                uint8_t *host_iv_to_target,
                                                uint8_t *host_iv_to_host)
{
  (void)(public_key);
  (void)(host_iv_to_target);
  (void)(host_iv_to_host);
  return 0;
}

static sl_status_t ec_ephemeral_key(ec_keypair_t *key)
{
  // NOTE: OpenSSL random number generator is not thread safe
  uint8_t priv_buf[ECDH_PRIVATE_KEY_SIZE] = { 0 };
  uint8_t pub_buf[1 + PUBLIC_KEYPAIR_SIZE] = { 0 };
  EC_KEY *ec_key = NULL;
  EC_GROUP *group = NULL;
  const BIGNUM *priv_bn = NULL;
  const EC_POINT *pub_point = NULL;
  sl_status_t e;
  size_t s;

  group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
  if (!group) {
    e = SL_STATUS_FAIL;
    goto out;
  }

  ec_key = EC_KEY_new();
  if (!ec_key) {
    e = SL_STATUS_NO_MORE_RESOURCE;
    goto out;
  }

  if (EC_KEY_set_group(ec_key, group) < 1
      || EC_KEY_generate_key(ec_key) < 1) {
    e = SL_STATUS_FAIL;
    goto out;
  }

  priv_bn = EC_KEY_get0_private_key(ec_key);
  if (!priv_bn || BN_num_bytes(priv_bn) != ECDH_PRIVATE_KEY_SIZE) {
    e = SL_STATUS_FAIL;
    goto out;
  }
  BN_bn2bin(priv_bn, priv_buf);

  pub_point = EC_KEY_get0_public_key(ec_key);
  if (!pub_point) {
    e = SL_STATUS_FAIL;
    goto out;
  }

  s = EC_POINT_point2oct(group, pub_point, POINT_CONVERSION_UNCOMPRESSED,
                         NULL, 0, NULL);
  if (s != sizeof(pub_buf)) {
    e = SL_STATUS_FAIL;
    goto out;
  }

  EC_POINT_point2oct(group, pub_point, POINT_CONVERSION_UNCOMPRESSED,
                     pub_buf, sizeof(pub_buf), NULL);

  memcpy(key->priv, priv_buf, ECDH_PRIVATE_KEY_SIZE);
  memcpy(key->pub, pub_buf + 1, PUBLIC_KEYPAIR_SIZE);
  e = SL_STATUS_OK;

  out:
  EC_KEY_free(ec_key);
  EC_GROUP_free(group);
  return e;
}

static EVP_PKEY *ec_key(const ec_keypair_t *key,
                        int both_parts)
{
  uint8_t tmp[1 + PUBLIC_KEYPAIR_SIZE];
  EVP_PKEY *result = NULL;
  EVP_PKEY *evp_key = NULL;
  BIGNUM *priv_bn = NULL;
  EC_POINT *pub_point = NULL;
  EC_KEY *ec_key = NULL;
  EC_GROUP *group = NULL;

  tmp[0] = POINT_CONVERSION_UNCOMPRESSED;
  memcpy(tmp + 1, key->pub, PUBLIC_KEYPAIR_SIZE);

  group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
  if (!group) {
    goto out;
  }

  ec_key = EC_KEY_new();
  if (!ec_key) {
    goto out;
  }
  if (EC_KEY_set_group(ec_key, group) < 1) {
    goto out;
  }

  if (both_parts) {
    priv_bn = BN_bin2bn(key->priv, ECDH_PRIVATE_KEY_SIZE, NULL);
    if (!priv_bn) {
      goto out;
    }
    if (EC_KEY_set_private_key(ec_key, priv_bn) != 1) {
      goto out;
    }
  }

  pub_point = EC_POINT_new(group);
  if (!pub_point) {
    goto out;
  }
  if (EC_POINT_oct2point(group, pub_point, tmp, sizeof(tmp), NULL) < 1) {
    goto out;
  }
  if (EC_KEY_set_public_key(ec_key, pub_point) != 1) {
    goto out;
  }

  evp_key = EVP_PKEY_new();
  if (!evp_key) {
    goto out;
  }

  if (EVP_PKEY_set1_EC_KEY(evp_key, ec_key) != 1) {
    goto out;
  }

  result = evp_key;
  evp_key = NULL;

  out:
  EVP_PKEY_free(evp_key);
  EC_KEY_free(ec_key);
  BN_free(priv_bn);
  EC_POINT_free(pub_point);
  EC_GROUP_free(group);

  return result;
}

static sl_status_t ecdh_secret(const ec_keypair_t *remote_ec_key)
{
  EVP_PKEY_CTX *ctxt = NULL;
  unsigned char *secret_ptr = NULL;
  size_t secret_len;
  EVP_PKEY *local_pkey = NULL;
  EVP_PKEY *remote_pkey = NULL;
  sl_status_t e;

  // Set up keys
  local_pkey = ec_key(&local_ec_key, 1);
  if (!local_pkey) {
    e = SL_STATUS_FAIL;
    goto out;
  }

  remote_pkey = ec_key(remote_ec_key, 0);
  if (!remote_pkey) {
    e = SL_STATUS_FAIL;
    goto out;
  }

  ctxt = EVP_PKEY_CTX_new(local_pkey, NULL);
  if (!ctxt) {
    e = SL_STATUS_FAIL;
    goto out;
  }

  if (EVP_PKEY_derive_init(ctxt) < 1
      || EVP_PKEY_derive_set_peer(ctxt, remote_pkey) < 1
      || EVP_PKEY_derive(ctxt, NULL, &secret_len) < 1
      || secret_len != 32) {
    e = SL_STATUS_FAIL;
    goto out;
  }

  secret_ptr = OPENSSL_malloc(secret_len);
  if (!secret_ptr) {
    e = SL_STATUS_NO_MORE_RESOURCE;
    goto out;
  }

  if (EVP_PKEY_derive(ctxt, secret_ptr, &secret_len) < 1) {
    e = SL_STATUS_FAIL;
    goto out;
  }

  // Use sha256 to derive the AES CCM key.
  // NOTE: This is not thread safe
  uint8_t *hash = SHA256(secret_ptr, secret_len, NULL);
  if (!hash) {
    e = SL_STATUS_FAIL;
    goto out;
  }

  memcpy(ccm_key, hash, AES_CCM_KEY_SIZE);
  e = SL_STATUS_OK;

  out:
  if (secret_ptr) {
    OPENSSL_free(secret_ptr);
  }
  if (local_pkey) {
    EVP_PKEY_free(local_pkey);
  }
  if (remote_pkey) {
    EVP_PKEY_free(remote_pkey);
  }
  if (ctxt) {
    EVP_PKEY_CTX_free(ctxt);
  }

  return e;
}

static sl_status_t aes_ccm_encrypt(const uint8_t *key, const uint8_t *nonce,
                                   const uint8_t *plain_text,
                                   const size_t text_len,
                                   const uint8_t *additional,
                                   const size_t additional_len,
                                   uint8_t *cipher_text, uint8_t *mac)
{
  EVP_CIPHER_CTX *ccm = NULL;
  int len;
  sl_status_t r;

  ccm = EVP_CIPHER_CTX_new();
  if (!ccm) {
    r = SL_STATUS_FAIL;
    goto out;
  }

  if (EVP_EncryptInit_ex(ccm, EVP_aes_128_ccm(), NULL, NULL, NULL) != 1
      || EVP_CIPHER_CTX_ctrl(ccm, EVP_CTRL_CCM_SET_IVLEN,
                             NONCE_SIZE, NULL) != 1
      || EVP_CIPHER_CTX_ctrl(ccm, EVP_CTRL_CCM_SET_TAG,
                             MAC_LEN, NULL) != 1
      || EVP_EncryptInit_ex(ccm, NULL, NULL, key, nonce) != 1) {
    r = SL_STATUS_FAIL;
    goto out;
  }

  // Provide the total plain text length
  if (EVP_EncryptUpdate(ccm,
                        NULL, &len,
                        NULL, text_len) != 1) {
    r = SL_STATUS_FAIL;
    goto out;
  }

  // Provide any AAD data. This can be called zero or one times as required
  if (additional) {
    if (EVP_EncryptUpdate(ccm,
                          NULL, &len,
                          additional, additional_len) != 1) {
      r = SL_STATUS_FAIL;
      goto out;
    }
  }
  if (len != additional_len) {
    r = SL_STATUS_FAIL;
    goto out;
  }

  /* Provide the message to be encrypted, and obtain the encrypted output.
     EVP_EncryptUpdate can only be called once for this */
  if (EVP_EncryptUpdate(ccm,
                        cipher_text, &len,
                        plain_text, text_len) != 1) {
    r = SL_STATUS_FAIL;
    goto out;
  }
  if (len != text_len) {
    r = SL_STATUS_FAIL;
    goto out;
  }

  // Get the tag
  if (EVP_CIPHER_CTX_ctrl(ccm, EVP_CTRL_CCM_GET_TAG,
                          MAC_LEN, mac) != 1) {
    r = SL_STATUS_FAIL;
    goto out;
  }

  r = SL_STATUS_OK;

  out:
  if (ccm) {
    EVP_CIPHER_CTX_free(ccm);
  }

  return r;
}

static sl_status_t aes_ccm_decrypt(const uint8_t *key, const uint8_t *nonce,
                                   const uint8_t *cipher_text,
                                   const size_t text_len,
                                   const uint8_t *additional,
                                   const size_t additional_len,
                                   uint8_t *plain_text, const uint8_t *mac)
{
  EVP_CIPHER_CTX *ccm = NULL;
  int len;
  sl_status_t r;

  ccm = EVP_CIPHER_CTX_new();
  if (!ccm) {
    r = SL_STATUS_FAIL;
    goto out;
  }

  if (EVP_DecryptInit_ex(ccm, EVP_aes_128_ccm(), NULL, NULL, NULL) != 1
      || EVP_CIPHER_CTX_ctrl(ccm, EVP_CTRL_CCM_SET_IVLEN,
                             NONCE_SIZE, NULL) != 1
      || EVP_CIPHER_CTX_ctrl(ccm, EVP_CTRL_CCM_SET_TAG,
                             MAC_LEN, (void *)mac) != 1
      || EVP_DecryptInit_ex(ccm, NULL, NULL, key, nonce) != 1) {
    r = SL_STATUS_FAIL;
    goto out;
  }

  // Provide the total plain text length
  if (EVP_DecryptUpdate(ccm,
                        NULL, &len,
                        NULL, text_len) != 1) {
    r = SL_STATUS_FAIL;
    goto out;
  }

  // Provide any AAD data. This can be called zero or one times as required
  if (additional) {
    if (EVP_DecryptUpdate(ccm,
                          NULL, &len,
                          additional, additional_len) != 1) {
      r = SL_STATUS_FAIL;
      goto out;
    }
  }
  if (len != additional_len) {
    r = SL_STATUS_FAIL;
    goto out;
  }

  /* Provide the message to be decrypted, and obtain the decrypted output.
     EVP_DecryptUpdate can be called multiple times if necessary */
  if (EVP_DecryptUpdate(ccm,
                        plain_text, &len,
                        cipher_text, text_len) != 1) {
    r = SL_STATUS_BT_CTRL_AUTHENTICATION_FAILURE;
    goto out;
  }
  if (len != text_len) {
    r = SL_STATUS_FAIL;
    goto out;
  }

  r = SL_STATUS_OK;

  out:
  if (ccm) {
    EVP_CIPHER_CTX_free(ccm);
  }

  return r;
}

static void increase_counter(conn_nonce_t *counter)
{
  if (counter->counter == UINT32_MAX) {
    counter->counter_hi++;
  }
  counter->counter++;
}

static void change_state(security_state_t ns)
{
  if (security_state == ns) {
    return;
  }
  security_state = ns;
  security_state_change_cb(security_state);
}

sl_status_t security_init()
{
  security_state = SECURITY_STATE_UNDEFINED;
  return SL_STATUS_OK;
}

void security_reset()
{
  change_state(SECURITY_STATE_UNENCRYPTED);
}

security_state_t  get_security_state()
{
  return security_state;
}

void security_start()
{
  switch (security_state) {
    case SECURITY_STATE_UNENCRYPTED: {
      app_log_info("Start encryption" APP_LOG_NL);
      sec_counter_in.counter = 0;
      sec_counter_in.counter_hi = 0;
      sec_counter_out.counter = 0;
      sec_counter_out.counter_hi = 0;

      sl_status_t err = ec_ephemeral_key(&local_ec_key);
      if (err) {
        app_log_warning("EC keypair generation failed 0x%x" APP_LOG_NL,
                        err);
        return;
      }
      int ret = RAND_bytes(sec_counter_out.host_iv,
                           sizeof(sec_counter_out.host_iv));
      if (ret != 1) {
        app_log_warning("Error generating random bytes 0x%lx" APP_LOG_NL,
                        ERR_get_error());
        return;
      }
      ret = RAND_bytes(sec_counter_in.host_iv, sizeof(sec_counter_in.host_iv));
      if (ret != 1) {
        app_log_warning("Error generating random bytes 0x%lx" APP_LOG_NL,
                        ERR_get_error());
        return;
      }
      security_state = SECURITY_STATE_INCREASE_SECURITY;
      sl_bgapi_user_cmd_increase_security(local_ec_key.pub,
                                          sec_counter_out.host_iv,
                                          sec_counter_in.host_iv);
      break;
    }
    default:
      break;
  }
}

/**************************************************************************//**
 * Callback which is called when security state changes.
 *
 * @note Weak implementation
 *****************************************************************************/
SL_WEAK void security_state_change_cb(security_state_t state)
{
  (void)state;
}

void security_increase_security_rsp(uint8_t *public_key,
                                    uint8_t *target_iv_to_target,
                                    uint8_t *target_iv_to_host)
{
  switch (security_state) {
    case SECURITY_STATE_INCREASE_SECURITY: {
      ec_keypair_t remote_ec_key;
      memcpy(remote_ec_key.pub, public_key, PUBLIC_KEYPAIR_SIZE);
      memcpy(sec_counter_out.target_iv, target_iv_to_target,
             sizeof(sec_counter_out.target_iv));
      memcpy(sec_counter_in.target_iv, target_iv_to_host,
             sizeof(sec_counter_in.target_iv));
      sl_status_t err = ecdh_secret(&remote_ec_key);
      if (err) {
        app_log_warning("AES CCM key generation failed 0x%x" APP_LOG_NL,
                        err);
        change_state(SECURITY_STATE_UNENCRYPTED);
        return;
      }
      change_state(SECURITY_STATE_ENCRYPTED);
      break;
    }
    default:
      break;
  }
}

void security_decrypt(char *src, char *dst, unsigned *len)
{
  if (security_state != SECURITY_STATE_ENCRYPTED) {
    memcpy(dst, src, *len);
  } else {
    security_decrypt_packet(src, dst, len);
  }
}

void security_decrypt_packet(char *src, char *dst, unsigned *len)
{
  *dst++ = src[0] & ~(1 << 6);//clr encrypted bit
  *dst++ = src[1] - 9;

  // remove tag and counter value
  *len = *len - 9;

  //verify counter to prevent replay attacks
  conn_nonce_t nonce;
  memcpy(&nonce.counter, src + *len + 4, 4);
  memcpy(&nonce.counter_hi, src + *len + 8, 1);
  memcpy(nonce.host_iv, sec_counter_in.host_iv,
         sizeof(sec_counter_in.host_iv));
  memcpy(nonce.target_iv, sec_counter_in.target_iv,
         sizeof(sec_counter_in.target_iv));
  if ((nonce.counter_hi > sec_counter_in.counter_hi
       || (nonce.counter_hi == sec_counter_in.counter_hi
           && nonce.counter >= sec_counter_in.counter)) == 0) {
    app_log_warning("Error packet counter not valid" APP_LOG_NL);
    *len = 0;
    return;
  }

  uint8_t auth_data[7];
  memcpy(auth_data, src, 2);
  memcpy(auth_data + 2, &nonce.counter, 4);
  auth_data[6] = nonce.counter_hi;

  sl_status_t err = aes_ccm_decrypt(ccm_key, (uint8_t *)&nonce,
                                    (uint8_t *)src + 2, *len - 2,
                                    auth_data, 7,
                                    (uint8_t *)dst, (uint8_t *)src + *len);
  if (err) {
    app_log_warning("Packet decryption failed 0x%x" APP_LOG_NL, err);
    *len = 0;
    return;
  }

  // Update counter
  if (sec_counter_in.counter != nonce.counter
      || sec_counter_in.counter_hi != nonce.counter_hi) {
    sec_counter_in.counter = nonce.counter;
    sec_counter_in.counter_hi = nonce.counter_hi;
  }
  increase_counter(&sec_counter_in);
}

void security_encrypt(char *src, char *dst, unsigned *len)
{
  if (security_state != SECURITY_STATE_ENCRYPTED) {
    memcpy(dst, src, *len);
  } else {
    security_encrypt_packet(src, dst, len);
  }
}

void security_encrypt_packet(char *src, char *dst, unsigned *len)
{
  *dst++ = src[0] | (1 << 6);//set encrypted bit
  *dst++ = src[1] + 9;

  uint8_t auth_data[7];
  memcpy(auth_data, dst - 2, 2);
  memcpy(auth_data + 2, &sec_counter_out.counter, 4);
  auth_data[6] = sec_counter_out.counter_hi;

  sl_status_t err = aes_ccm_encrypt(ccm_key, (uint8_t *)&sec_counter_out,
                                    (uint8_t *)src + 2, *len - 2,
                                    auth_data, 7,
                                    (uint8_t *)dst, (uint8_t *)dst + *len - 2);
  if (err) {
    app_log_warning("Packet encryption failed 0x%x" APP_LOG_NL, err);
    *len = 0;
    return;
  }

  // message + tag
  dst += *len - 2 + MAC_LEN;

  // add counter to end
  memcpy(dst, auth_data + 2, 5);

  *len = *len + 9;

  increase_counter(&sec_counter_out);
}
