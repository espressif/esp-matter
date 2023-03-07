/***************************************************************************//**
 * @file
 * @brief Implementation of AES.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include PLATFORM_HEADER
#include "hal/hal.h"
#include "include/error.h"
#include "stack/platform/micro/aes.h"
#include <stdbool.h>
#include "include/security.h"

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#endif

#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(SL_CATALOG_SLI_PROTOCOL_CRYPTO_PRESENT)
// Quick-and-dirty implementation on top of RADIOAES / CRYPTO2 for increased speed
#include "sli_protocol_crypto.h"

// PSA Crypto exposes stateless AES hooks. Use these with a static key buffer
// until we can fully migrate to PSA Crypto / Vault key management, at which point
// these calls would become PSA key handling calls with the correct key ID.
static uint8_t loadedKey[EMBER_ENCRYPTION_KEY_SIZE] = { 0 };

// Load the passed key into the encryption core.
void emLoadKeyIntoCore(const uint8_t* key)
{
  MEMCOPY(loadedKey, key, sizeof(loadedKey));
}

void emGetKeyFromCore(uint8_t* key)
{
  MEMCOPY(key, loadedKey, sizeof(loadedKey));
}

void emSecurityHardwareInit(void)
{
  return;
}

void emStandAloneEncryptBlock(uint8_t* block)
{
  // Encrypt this block in place with the current key
  sl_status_t status = sli_aes_crypt_ecb_radio(
    true,
    loadedKey,
    EMBER_ENCRYPTION_KEY_SIZE * 8,
    block,
    block);
  assert(status == SL_STATUS_OK);
}

//----------------------------------------------------------------
// Wrapper for those that just want access to AES.

void emAesEncrypt(uint8_t* block, const uint8_t* key)
{
  // Encrypt this block in place with the current key
  sl_status_t status = sli_aes_crypt_ecb_radio(
    true,
    key,
    EMBER_ENCRYPTION_KEY_SIZE * 8,
    block,
    block);
  assert(status == SL_STATUS_OK);
}

void emAesDecrypt(uint8_t* block, const uint8_t* key)
{
  // Encrypt this block in place with the current key
  sl_status_t status = sli_aes_crypt_ecb_radio(
    false,
    key,
    EMBER_ENCRYPTION_KEY_SIZE * 8,
    block,
    block);
  assert(status == SL_STATUS_OK);
}

#elif defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_AES) && defined(MBEDTLS_PSA_ACCEL_ALG_ECB_NO_PADDING) && defined(PSA_WANT_ALG_ECB_NO_PADDING) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS)
// PSA Crypto driver implementation
#include "psa/crypto.h"

#if defined(SEMAILBOX_PRESENT)
#include "sli_se_transparent_types.h"
#include "sli_se_transparent_functions.h"
#define CIPHER_SINGLE_SHOT_ENC_FCT sli_se_transparent_cipher_encrypt
#define CIPHER_SINGLE_SHOT_DEC_FCT sli_se_transparent_cipher_decrypt
#elif defined(CRYPTOACC_PRESENT)
#include "sli_cryptoacc_transparent_types.h"
#include "sli_cryptoacc_transparent_functions.h"
#define CIPHER_SINGLE_SHOT_ENC_FCT sli_cryptoacc_transparent_cipher_encrypt
#define CIPHER_SINGLE_SHOT_DEC_FCT sli_cryptoacc_transparent_cipher_decrypt
#elif defined(CRYPTO_PRESENT)
#include "sli_crypto_transparent_types.h"
#include "sli_crypto_transparent_functions.h"
#define CIPHER_SINGLE_SHOT_ENC_FCT sli_crypto_transparent_cipher_encrypt
#define CIPHER_SINGLE_SHOT_DEC_FCT sli_crypto_transparent_cipher_decrypt
#else
#error "Compiling with PSA drivers, but not for a target part"
#endif

// PSA Crypto exposes stateless AES hooks. Use these with a static key buffer
// until we can fully migrate to PSA Crypto / Vault key management, at which point
// these calls would become PSA key handling calls with the correct key ID.
static uint8_t loadedKey[EMBER_ENCRYPTION_KEY_SIZE] = { 0 };

// Load the passed key into the encryption core.
void emLoadKeyIntoCore(const uint8_t* key)
{
  MEMCOPY(loadedKey, key, sizeof(loadedKey));
}

void emGetKeyFromCore(uint8_t* key)
{
  MEMCOPY(key, loadedKey, sizeof(loadedKey));
}

void emSecurityHardwareInit(void)
{
}

void emStandAloneEncryptBlock(uint8_t* block)
{
  // Encrypt this block in place with the current key
  size_t output_size = 0;
  psa_key_attributes_t key_attr = PSA_KEY_ATTRIBUTES_INIT;

  psa_set_key_type(&key_attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&key_attr, PSA_BYTES_TO_BITS(EMBER_ENCRYPTION_KEY_SIZE));

  psa_status_t status = CIPHER_SINGLE_SHOT_ENC_FCT(
    &key_attr,
    loadedKey,
    sizeof(loadedKey),
    PSA_ALG_ECB_NO_PADDING,
    NULL, 0,
    block, SECURITY_BLOCK_SIZE,
    block, SECURITY_BLOCK_SIZE,
    &output_size);

  psa_reset_key_attributes(&key_attr);

  assert(status == PSA_SUCCESS);
}

//----------------------------------------------------------------
// Wrapper for those that just want access to AES.

void emAesEncrypt(uint8_t* block, const uint8_t* key)
{
  size_t output_size = 0;
  psa_key_attributes_t key_attr = PSA_KEY_ATTRIBUTES_INIT;

  psa_set_key_type(&key_attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&key_attr, PSA_BYTES_TO_BITS(EMBER_ENCRYPTION_KEY_SIZE));

  psa_status_t status = CIPHER_SINGLE_SHOT_ENC_FCT(
    &key_attr,
    key,
    EMBER_ENCRYPTION_KEY_SIZE,
    PSA_ALG_ECB_NO_PADDING,
    NULL, 0,
    block, SECURITY_BLOCK_SIZE,
    block, SECURITY_BLOCK_SIZE,
    &output_size);

  psa_reset_key_attributes(&key_attr);

  assert(status == PSA_SUCCESS);
  assert(output_size == SECURITY_BLOCK_SIZE);
}

void emAesDecrypt(uint8_t* block, const uint8_t* key)
{
  size_t output_size = 0;
  psa_key_attributes_t key_attr = PSA_KEY_ATTRIBUTES_INIT;

  psa_set_key_type(&key_attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&key_attr, PSA_BYTES_TO_BITS(EMBER_ENCRYPTION_KEY_SIZE));

  psa_status_t status = CIPHER_SINGLE_SHOT_DEC_FCT(
    &key_attr,
    key,
    EMBER_ENCRYPTION_KEY_SIZE,
    PSA_ALG_ECB_NO_PADDING,
    block, SECURITY_BLOCK_SIZE,
    block, SECURITY_BLOCK_SIZE,
    &output_size);

  psa_reset_key_attributes(&key_attr);

  assert(status == PSA_SUCCESS);
  assert(output_size == SECURITY_BLOCK_SIZE);
}

#elif defined(MBEDTLS_AES_C)

#include "mbedtls/aes.h"

// mbed TLS implementation
static mbedtls_aes_context aesContext;

#if defined(EMBER_TEST) || defined(SL_CATALOG_ZIGBEE_AES_SOFTWARE_PRESENT)
// Mbed TLS doesn't seem to provide an easy way to get the key for platforms other than EFR32.
// Ie. mbedtls_aes_context in mbedtls/aes.h vs. sl_crypto/include/aes_alt.h.
// For ember test (or host apps) let's just store the key locally to easily provide emGetKeyFromCore.
static uint8_t loadedKey[EMBER_ENCRYPTION_KEY_SIZE] = { 0 };

// Load the passed key into the encryption core.
void emLoadKeyIntoCore(const uint8_t* key)
{
  int status = mbedtls_aes_setkey_enc(&aesContext,
                                      key,
                                      SECURITY_BLOCK_SIZE * 8U);

  assert(status == 0);

  MEMCOPY(loadedKey, key, sizeof(loadedKey));
}

void emGetKeyFromCore(uint8_t* key)
{
  MEMCOPY(key, loadedKey, sizeof(loadedKey));
}

#else //EMBER_TEST
// Load the passed key into the encryption core.
void emLoadKeyIntoCore(const uint8_t* key)
{
  int status = mbedtls_aes_setkey_enc(&aesContext,
                                      key,
                                      SECURITY_BLOCK_SIZE * 8U);

  assert(status == 0);
}

void emGetKeyFromCore(uint8_t* key)
{
  MEMCOPY(key, aesContext.key, SECURITY_BLOCK_SIZE * sizeof(key[0]));
}
#endif

void emSecurityHardwareInit(void)
{
  mbedtls_aes_init(&aesContext);
}

void emStandAloneEncryptBlock(uint8_t* block)
{
  // Encrypt this block in place with the current key
  int status = mbedtls_aes_crypt_ecb(&aesContext,
                                     MBEDTLS_AES_ENCRYPT,
                                     block,
                                     block);

  assert(status == 0);
}

//----------------------------------------------------------------
// Wrapper for those that just want access to AES.

void emAesEncrypt(uint8_t* block, const uint8_t* key)
{
  int status = mbedtls_aes_setkey_enc(&aesContext,
                                      key,
                                      SECURITY_BLOCK_SIZE * 8U);

  assert(status == 0);

  status = mbedtls_aes_crypt_ecb(&aesContext,
                                 MBEDTLS_AES_ENCRYPT,
                                 block,
                                 block);

  assert(status == 0);
}

void emAesDecrypt(uint8_t* block, const uint8_t* key)
{
  int status = mbedtls_aes_setkey_dec(&aesContext,
                                      key,
                                      SECURITY_BLOCK_SIZE * 8U);

  assert(status == 0);

  status = mbedtls_aes_crypt_ecb(&aesContext,
                                 MBEDTLS_AES_DECRYPT,
                                 block,
                                 block);

  assert(status == 0);
}

#else
#error "Stack AES needs either PSA Crypto or MbedTLS AES. Check your crypto configuration."
#endif
