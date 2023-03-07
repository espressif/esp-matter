/****************************************************************************
 * @file ccm-star.c
 * @brief implements the CCM* API, providing both software and mbedtls versions
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifdef PLATFORM_HEADER
  #include PLATFORM_HEADER
#else
  #include "hal/micro/unix/compiler/gcc.h"
#endif
#ifdef UC_BUILD
#include "sl_component_catalog.h"
#endif

// TODO: EMZIGBEE-3712
void emStandAloneEncryptBlock(uint8_t* block);

extern void emGetKeyFromCore(uint8_t* key);

#include "stack/include/ember-types.h"
#include "stack/include/ccm-star.h"

#define ENCRYPTION_BLOCK_SIZE 16

// This implements the CCM* encryption algorithm, using AES-128 as the
// underlying block cipher.  AES-128 is essentially a hash function
// controlled by 128-bit (16 byte) keys.  The AES-128 algorithm takes a key
// and a 16-byte block of data and produces a 16-byte block of noise.
//
// The em250 implements AES-128 in hardware.  The hardware can also handle
// some of the other CCM* calculations, but at this point we only use the
// basic AES-128 functionality.
//
// CCM* uses AES-128 for authentication and encryption.  In authentication,
// AES-128 is used create a hash number from a message, called a MIC.
// The MIC is appended to the message before transmission.  The recipient
// hashes the message and verifies that it obtains the same MIC.  Changes
// to either the body of the message or the appended MIC will result in
// the recipient's MIC differing from the appended MIC.
//
// For encryption, AES-128 is used to create a series of pseudo-random
// blocks which are xor-ed with the message to produce the encrypted
// version.  The recipient performs the same operation, which results in
// the original message.  It is important that each message be encrypted
// with a different psuedo-random sequence.  If two messages used the
// same sequence, xor-ing the encrypted messages would produce the xor
// of the unencrypted messages.
//
// Both authentication and encryption begin by encrypting a special block
// of data called a 'nonce' (as in 'for the nonce').  The goal is that each
// use of a particular key will use a different nonce.  The nonce is:
//
// Offset Size
//   0     1    flags
//   1     8    source EUI64
//   9     4    frame counter
//  13     1    control byte
//  14     2    block count
//
// The frame counter is incremented for each message and the block count is
// incremented for each encryption block.  The flags and control byte ensure
// that different operations on the same message, such as MIC generation and
// encryption, do not use the same nonce.

#ifdef UC_BUILD
#if (defined SL_CATALOG_ZIGBEE_CCM_PSA_PRESENT)
#define USE_PSA_CCM
#elif (defined SL_CATALOG_ZIGBEE_CCM_SOFTWARE_PRESENT)
// Default - use software ccm implementation
#else // None of the implementation present, then error
#error Must enable one of the CCM implementation (Software or mbedtls)
#endif // SL_CATALOG_ZIGBEE_
#endif // UC_BUILD

#if defined(USE_PSA_CCM)
#include "psa/crypto.h"

#if defined(SL_CATALOG_SLI_PROTOCOL_CRYPTO_PRESENT) && 0
// -----------------------------------------------------------------
// Protocol Crypto Implementation (opportunistically use radioAES)
// Note: currently always disabled (&& 0 on define guard) until test
// results are acquired to show that BLE can deal with the jitter
// introduced in DMP scenarios with having both Zigbee and BLE target
// the protocol crypto interface/instance.
// -----------------------------------------------------------------
#include "sli_protocol_crypto.h"
static uint8_t aesKey[ENCRYPTION_BLOCK_SIZE];

EmberStatus emberCcmEncryptAndTag(uint8_t *nonce,
                                  uint8_t *authData, uint16_t authlen,
                                  uint8_t *encryptData, uint16_t encryptlen,
                                  uint8_t *micResult)
{
  emGetKeyFromCore(aesKey);
  sl_status_t status = sli_ccm_zigbee(
    true,
    encryptData, encryptData, encryptlen,
    aesKey,
    nonce,
    authData, authlen,
    micResult, MIC_LENGTH);

  if (status != SL_STATUS_OK) {
    return EMBER_BAD_ARGUMENT;
  } else {
    return EMBER_SUCCESS;
  }
}

EmberStatus emberCcmAuthDecrypt(uint8_t *nonce,
                                uint8_t *authData, uint16_t authlen,
                                uint8_t *encryptData, uint16_t encryptlen,
                                uint8_t *mic)
{
  emGetKeyFromCore(aesKey);
  sl_status_t status = sli_ccm_zigbee(
    false,
    encryptData, encryptData, encryptlen,
    aesKey,
    nonce,
    authData, authlen,
    mic, MIC_LENGTH);

  if (status == SL_STATUS_INVALID_SIGNATURE) {
    return EMBER_AUTH_FAILURE;
  } else if (status != SL_STATUS_OK) {
    return EMBER_BAD_ARGUMENT;
  } else {
    return EMBER_SUCCESS;
  }
}

EmberStatus emberCcmEncryptAndTagPacket(uint8_t *nonce,
                                        uint8_t *packet, uint16_t authlen,
                                        uint16_t encryptlen)
{
  emGetKeyFromCore(aesKey);
  sl_status_t status = sli_ccm_zigbee(
    true,
    packet + authlen, packet + authlen, encryptlen,
    aesKey,
    nonce,
    packet, authlen,
    packet + authlen + encryptlen, MIC_LENGTH);

  if (status != SL_STATUS_OK) {
    return EMBER_BAD_ARGUMENT;
  } else {
    return EMBER_SUCCESS;
  }
}

EmberStatus emberCcmAuthDecryptPacket(uint8_t *nonce,
                                      uint8_t *packet, uint16_t authlen,
                                      uint16_t encryptlen)
{
  emGetKeyFromCore(aesKey);
  sl_status_t status = sli_ccm_zigbee(
    false,
    packet + authlen, packet + authlen, encryptlen,
    aesKey,
    nonce,
    packet, authlen,
    packet + authlen + encryptlen, MIC_LENGTH);

  if (status == SL_STATUS_INVALID_SIGNATURE) {
    return EMBER_AUTH_FAILURE;
  } else if (status != SL_STATUS_OK) {
    return EMBER_BAD_ARGUMENT;
  } else {
    return EMBER_SUCCESS;
  }
}

void emberCcmCalculateAndEncryptMic(uint8_t *nonce,
                                    uint8_t *authData, uint16_t authlen,
                                    uint8_t *payload, uint16_t payloadlen,
                                    uint8_t *micResult)
{
  emGetKeyFromCore(aesKey);
  sl_status_t status = sli_ccm_zigbee(
    true,
    payload, NULL, payloadlen,
    aesKey,
    nonce,
    authData, authlen,
    micResult, MIC_LENGTH);
  assert(status == SL_STATUS_OK);
}

void emberCcmEncryptBytes(uint8_t *bytes,
                          uint16_t length,
                          uint8_t* nonce)
{
  emGetKeyFromCore(aesKey);
  sl_status_t status = sli_ccm_zigbee(
    true,
    bytes, bytes, length,
    aesKey,
    nonce,
    NULL, 0,
    NULL, 0);
  assert(status == SL_STATUS_OK);
}

#elif defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_AES) && defined(MBEDTLS_PSA_ACCEL_ALG_CCM) && defined(PSA_WANT_ALG_CCM) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS)
// -----------------------------------------------------------------
// Use SL-modified PSA Crypto driver API for AEAD (which supports
// separate data and tag buffers) when present
// -----------------------------------------------------------------
#include "em_device.h"
#if defined(SEMAILBOX_PRESENT)
#include "sli_se_transparent_functions.h"
#define AEAD_ENCRYPT_TAG_FCT    sli_se_driver_aead_encrypt_tag
#define AEAD_DECRYPT_TAG_FCT    sli_se_driver_aead_decrypt_tag
#elif defined(CRYPTO_PRESENT)
#include "sli_crypto_transparent_functions.h"
#define AEAD_ENCRYPT_TAG_FCT    sli_crypto_transparent_aead_encrypt_tag
#define AEAD_DECRYPT_TAG_FCT    sli_crypto_transparent_aead_decrypt_tag
#elif defined(CRYPTOACC_PRESENT)
#include "sli_cryptoacc_transparent_functions.h"
#define AEAD_ENCRYPT_TAG_FCT    sli_cryptoacc_transparent_aead_encrypt_tag
#define AEAD_DECRYPT_TAG_FCT    sli_cryptoacc_transparent_aead_decrypt_tag
#else
#error "Compiling with PSA drivers for CCM, but not for a Silicon Labs target"
#endif

static uint8_t aesKey[ENCRYPTION_BLOCK_SIZE];
static uint8_t ccmOutput[256];

EmberStatus emberCcmEncryptAndTag(uint8_t *nonce,
                                  uint8_t *authData, uint16_t authlen,
                                  uint8_t *encryptData, uint16_t encryptlen,
                                  uint8_t *micResult)
{
  psa_status_t psa_status;
  size_t tag_len;
  size_t output_length;
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;

  psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&attr, PSA_BYTES_TO_BITS(sizeof(aesKey)));
  emGetKeyFromCore(aesKey);

  psa_status = AEAD_ENCRYPT_TAG_FCT(
    &attr, aesKey, sizeof(aesKey),
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, MIC_LENGTH),
    nonce, NONCE_LENGTH,
    authData, authlen,
    encryptData, encryptlen,
    encryptData, encryptlen, &output_length,
    micResult, MIC_LENGTH, &tag_len);

  psa_reset_key_attributes(&attr);

  if (psa_status != PSA_SUCCESS || output_length != encryptlen || tag_len != MIC_LENGTH) {
    return EMBER_BAD_ARGUMENT;
  } else {
    return EMBER_SUCCESS;
  }
}

EmberStatus emberCcmAuthDecrypt(uint8_t *nonce,
                                uint8_t *authData, uint16_t authlen,
                                uint8_t *encryptData, uint16_t encryptlen,
                                uint8_t *mic)
{
  psa_status_t psa_status;
  size_t output_length;
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;

  psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&attr, PSA_BYTES_TO_BITS(sizeof(aesKey)));
  emGetKeyFromCore(aesKey);

  psa_status = AEAD_DECRYPT_TAG_FCT(
    &attr, aesKey, sizeof(aesKey),
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, MIC_LENGTH),
    nonce, NONCE_LENGTH,
    authData, authlen,
    encryptData, encryptlen,
    mic, MIC_LENGTH,
    encryptData, encryptlen, &output_length);

  psa_reset_key_attributes(&attr);

  if (psa_status == PSA_ERROR_INVALID_SIGNATURE) {
    return EMBER_AUTH_FAILURE;
  } else if (psa_status != PSA_SUCCESS || output_length != encryptlen) {
    return EMBER_BAD_ARGUMENT;
  } else {
    return EMBER_SUCCESS;
  }
}

EmberStatus emberCcmEncryptAndTagPacket(uint8_t *nonce,
                                        uint8_t *packet, uint16_t authlen,
                                        uint16_t encryptlen)
{
  return emberCcmEncryptAndTag(
    nonce,
    packet, authlen,
    packet + authlen, encryptlen,
    packet + authlen + encryptlen);
}

EmberStatus emberCcmAuthDecryptPacket(uint8_t *nonce,
                                      uint8_t *packet, uint16_t authlen,
                                      uint16_t encryptlen)
{
  return emberCcmAuthDecrypt(
    nonce,
    packet, authlen,
    packet + authlen, encryptlen,
    packet + authlen + encryptlen);
}

void emberCcmCalculateAndEncryptMic(uint8_t *nonce,
                                    uint8_t *authData, uint16_t authlen,
                                    uint8_t *payload, uint16_t payloadlen,
                                    uint8_t *micResult)
{
  psa_status_t psa_status;
  size_t tag_len;
  size_t output_length;
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;

  psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&attr, PSA_BYTES_TO_BITS(sizeof(aesKey)));
  emGetKeyFromCore(aesKey);

  psa_status = AEAD_ENCRYPT_TAG_FCT(
    &attr, aesKey, sizeof(aesKey),
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, MIC_LENGTH),
    nonce, NONCE_LENGTH,
    authData, authlen,
    payload, payloadlen,
    ccmOutput, sizeof(ccmOutput), &output_length,
    micResult, MIC_LENGTH, &tag_len);

  psa_reset_key_attributes(&attr);
  assert((psa_status == PSA_SUCCESS)
         && (tag_len == MIC_LENGTH)
         && (output_length == payloadlen));
}

void emberCcmEncryptBytes(uint8_t *bytes,
                          uint16_t length,
                          uint8_t* nonce)
{
  // Convert plaintext to ciphertext according to AES-CCM with a
  // zero-length tag (CCM* in unauthenticated mode)
  psa_status_t psa_status;

  uint8_t tagbuf[MIC_LENGTH];
  size_t tag_len;
  size_t output_length;
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;

  psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&attr, PSA_BYTES_TO_BITS(sizeof(aesKey)));
  emGetKeyFromCore(aesKey);

  psa_status = AEAD_ENCRYPT_TAG_FCT(
    &attr, aesKey, sizeof(aesKey),
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, sizeof(tagbuf)),
    nonce, NONCE_LENGTH,
    NULL, 0,
    bytes, length,
    bytes, length, &output_length,
    tagbuf, sizeof(tagbuf), &tag_len);

  psa_reset_key_attributes(&attr);
  assert((psa_status == PSA_SUCCESS)
         && (tag_len == sizeof(tagbuf))
         && (output_length == length));
}

#elif defined(MBEDTLS_CCM_C)
// -----------------------------------------------------------------
// Use mbedTLS software implementation as fallback if no drivers are
// present (i.e. when compiling for sim / host). If PSA is compiled
// with support for CCM, but no drivers accelerating it, then the
// mbedTLS API would have automatically been included in the build
// since the PSA core still relies on it for software fallback.
//
// The reason this hasn't been moved to PSA-native API calls is due
// to PSA AEAD single-shot APIs requiring that the ciphertext and tag
// are in a contiguous buffer, meaning the way the Zigbee CCM* API
// is built would require us to realign in a temporary buffer with
// all inconveniences related to that.
// -----------------------------------------------------------------
#include "mbedtls/ccm.h"

#if !defined(MBEDTLS_AES_C) || !defined(MBEDTLS_CCM_C)
  #error Must have enabled mbedTLS AES and CCM modules
#endif

#define AES_CIPHER_ID MBEDTLS_CIPHER_ID_AES
#define ENCRYPTION_KEYBITS ENCRYPTION_BLOCK_SIZE * 8
#define OUTPUT_BUFFER_SIZE

static mbedtls_ccm_context ccmCtx;
static uint8_t aesKey[ENCRYPTION_BLOCK_SIZE];
static uint8_t ccmOutput[256];

static void prepCcmContext()
{
  int status;
  emGetKeyFromCore(aesKey);
  status = mbedtls_ccm_setkey(&ccmCtx, AES_CIPHER_ID, aesKey, ENCRYPTION_KEYBITS);
  assert(status == 0);
}

EmberStatus emberCcmEncryptAndTag(uint8_t *nonce,
                                  uint8_t *authData, uint16_t authlen,
                                  uint8_t *encryptData, uint16_t encryptlen,
                                  uint8_t *micResult)
{
  int status;
  prepCcmContext();
  status = mbedtls_ccm_encrypt_and_tag(&ccmCtx,
                                       encryptlen,
                                       nonce,
                                       NONCE_LENGTH,
                                       authData,
                                       authlen,
                                       encryptData,
                                       encryptData,
                                       micResult,
                                       MIC_LENGTH);
  if (status != 0) {
    return EMBER_BAD_ARGUMENT;
  } else {
    return EMBER_SUCCESS;
  }
}

EmberStatus emberCcmAuthDecrypt(uint8_t *nonce,
                                uint8_t *authData, uint16_t authlen,
                                uint8_t *encryptData, uint16_t encryptlen,
                                uint8_t *mic)
{
  int status;
  prepCcmContext();
  status = mbedtls_ccm_auth_decrypt(&ccmCtx,
                                    encryptlen,
                                    nonce,
                                    NONCE_LENGTH,
                                    authData,
                                    authlen,
                                    encryptData,
                                    encryptData,
                                    mic,
                                    MIC_LENGTH);
  if (status != 0) {
    return EMBER_AUTH_FAILURE;
  } else {
    return EMBER_SUCCESS;
  }
}

EmberStatus emberCcmEncryptAndTagPacket(uint8_t *nonce,
                                        uint8_t *packet, uint16_t authlen,
                                        uint16_t encryptlen)
{
  int status;
  prepCcmContext();
  status = mbedtls_ccm_encrypt_and_tag(&ccmCtx,
                                       encryptlen,
                                       nonce,
                                       NONCE_LENGTH,
                                       packet,
                                       authlen,
                                       packet + authlen,
                                       packet + authlen,
                                       packet + authlen + encryptlen,
                                       MIC_LENGTH);
  if (status != 0) {
    return EMBER_BAD_ARGUMENT;
  } else {
    return EMBER_SUCCESS;
  }
}

EmberStatus emberCcmAuthDecryptPacket(uint8_t *nonce,
                                      uint8_t *packet, uint16_t authlen,
                                      uint16_t encryptlen)
{
  int status;
  prepCcmContext();
  status = mbedtls_ccm_auth_decrypt(&ccmCtx,
                                    encryptlen,
                                    nonce,
                                    NONCE_LENGTH,
                                    packet,
                                    authlen,
                                    packet + authlen,
                                    packet + authlen,
                                    packet + authlen + encryptlen,
                                    MIC_LENGTH);
  if (status != 0) {
    return EMBER_AUTH_FAILURE;
  } else {
    return EMBER_SUCCESS;
  }
}

void emberCcmCalculateAndEncryptMic(uint8_t *nonce,
                                    uint8_t *authData, uint16_t authlen,
                                    uint8_t *payload, uint16_t payloadlen,
                                    uint8_t *micResult)
{
  prepCcmContext();
  mbedtls_ccm_encrypt_and_tag(&ccmCtx,
                              payloadlen,
                              nonce,
                              NONCE_LENGTH,
                              authData,
                              authlen,
                              payload,
                              ccmOutput,
                              micResult,
                              MIC_LENGTH);
}

void emberCcmEncryptBytes(uint8_t *bytes,
                          uint16_t length,
                          uint8_t* nonce)
{
  uint8_t ccmMicBuffer[MIC_LENGTH];
  prepCcmContext();
  mbedtls_ccm_encrypt_and_tag(&ccmCtx,
                              length,
                              nonce,
                              NONCE_LENGTH,
                              NULL,
                              0,
                              bytes,
                              bytes,
                              ccmMicBuffer,
                              MIC_LENGTH);
}

#else
#error "PSA CCM implementation configured through SLC, but one of the requirements is missing"
#endif

#else // !USE_PSA_CCM
// -----------------------------------------------------------------
// Software Implementation of CCM on top of the single-block AES API
// -----------------------------------------------------------------

// Handy macro.  This is unsafe because it duplicates its arguments.
#define min(x, y) ((x) < (y) ? (x) : (y))

//----------------------------------------------------------------
// MIC encryption flags

// For authentication, the flags byte has the following form:
//  <reserved:1> <adata:1> <M:3> <L:3>
// Where
//  <reserved:1> = 0   Reserved for future expansion.
//  <adata:1>          0 for zero-length messages, 1 for non-zero-length
//  <M:3>              MIC length, encoded as (micLength - 2)/2
//  <L:3>              Number of bytes used to represent the length
//                     of the message, - 1.

#if MESSAGE_LENGTH_FIELD_SIZE > 0
  #define MIC_FLAG_ADATA_FIELD 0x40
  #define FLAG_L_FIELD ((MESSAGE_LENGTH_FIELD_SIZE - 1) & 0x07)
#else
  #define MIC_FLAG_ADATA_FIELD 0x00
  #define FLAG_L_FIELD 0x00
#endif

#ifdef USE_8_BYTE_MIC
  #define MIC_FLAG_M_FIELD 0x18
#else
  #define MIC_FLAG_M_FIELD 0x08
#endif

#define MIC_FLAG_MASK \
  (MIC_FLAG_ADATA_FIELD | MIC_FLAG_M_FIELD | FLAG_L_FIELD)
#define ENCRYPTION_FLAG_MASK (0x00 | FLAG_L_FIELD)

// Performs an actual nonce encryption, after first setting the fields
// specific to this block.  We do a copy to avoid clobbering the (shared)
// nonce.

#define encryptMicBlock0(nonce, variableField, result) \
  (encryptNonce((nonce), MIC_FLAG_MASK, (variableField), (result)))

#define encryptBlock0(nonce, variableField, result) \
  (encryptNonce((nonce), ENCRYPTION_FLAG_MASK, (variableField), (result)))

static void encryptNonce(uint8_t *nonce,
                         uint8_t flagsMask,
                         uint16_t variableField,
                         uint8_t block[ENCRYPTION_BLOCK_SIZE])
{
  MEMCOPY(block + STANDALONE_NONCE_INDEX, nonce, NONCE_LENGTH);

  block[STANDALONE_FLAGS_INDEX] = flagsMask;
  block[STANDALONE_VARIABLE_FIELD_INDEX_HIGH] = HIGH_BYTE(variableField);
  block[STANDALONE_VARIABLE_FIELD_INDEX_LOW] = LOW_BYTE(variableField);

  emStandAloneEncryptBlock(block);
}

//----------------------------------------------------------------
// This performs the core of the MIC calculation.  'Count' bytes from
// 'bytes' are xor-ed into 'block' and then encrypted.  We start at
// 'blockIndex' in the block.
//
// The final blockIndex is returned.

static uint8_t xorBytesIntoBlock(uint8_t *block,
                                 uint8_t blockIndex,
                                 const uint8_t *bytes,
                                 uint16_t count)
{
  uint16_t i;

  for (i = 0; i < count; ) {
    uint16_t needed = ENCRYPTION_BLOCK_SIZE - blockIndex;
    uint16_t todo = count - i;
    uint16_t copied = min(todo, needed);
    uint16_t j;

    for (j = 0; j < copied; j++, blockIndex++) {
      block[blockIndex] ^= *bytes++;
    }
    i += copied;

    if (blockIndex == ENCRYPTION_BLOCK_SIZE) {
      emStandAloneEncryptBlock(block);
      blockIndex = 0;
    }
  }
  return blockIndex;
}

// static buffer to hold blocks of data for AES encryption
static uint8_t encryptionBlock[ENCRYPTION_BLOCK_SIZE];

// Encrypt an array of bytes by xor-ing with a series of AES encrypted nonces
static void ccmEncryptData(uint8_t *bytes,
                           uint16_t length,
                           uint8_t blockCount,
                           uint8_t *nonce)
{
  while (length > 0) {
    uint8_t todo = min(length, ENCRYPTION_BLOCK_SIZE);
    uint8_t i;
    encryptBlock0(nonce, blockCount, encryptionBlock);
    blockCount += 1;

    for (i = 0; i < todo; i++) {
      *bytes++ ^= encryptionBlock[i];
    }

    length -= todo;
  }
}

// Calculate the MIC by hashing first the authenticated portion of the
// packet and then the encrypted portion (which hasn't been encrypted yet).
//
// The encrypted bytes are processed on a block boundary, so we finish off
// the block at the end of the authenticated bytes.
//
// The 'for' loop goes around two times (authenticated bytes, encrypted bytes).
void emberCcmCalculateAndEncryptMic(uint8_t *nonce,
                                    uint8_t *authData,
                                    uint16_t authlen,
                                    uint8_t *payload,
                                    uint16_t payloadlen,
                                    uint8_t *micResult)
{
  uint8_t blockIndex = 2;     // skip over frame length field
  uint8_t *chunk = authData;
  uint16_t chunklen;
  uint8_t phase;

  chunklen = authlen;
  encryptMicBlock0(nonce, payloadlen, encryptionBlock);

  // first two bytes are the 16-bit representation of the frame length,
  // high byte first
  encryptionBlock[0] ^= HIGH_BYTE(authlen);
  encryptionBlock[1] ^= LOW_BYTE(authlen);

  // phase 0: go over authenticated data
  // phase 1: go over the payload
  for (phase = 0; phase < 2; phase++) {
    blockIndex = xorBytesIntoBlock(encryptionBlock,
                                   blockIndex,
                                   chunk,
                                   chunklen);
    chunk = payload;
    chunklen = payloadlen;

    // finish off authData if not on an encryption block boundary
    if (blockIndex > 0) {
      emStandAloneEncryptBlock(encryptionBlock);
      blockIndex = 0;
    }
  }

  MEMCOPY(micResult, encryptionBlock, MIC_LENGTH);
  // The MIC gets encrypted as block zero of the message.
  ccmEncryptData(micResult, MIC_LENGTH, 0, nonce);
}

//----------------------------------------------------------------
// The core encryption/decrytion functions

EmberStatus emberCcmEncryptAndTag(uint8_t *nonce,
                                  uint8_t *authData,
                                  uint16_t authlen,
                                  uint8_t *encryptData,
                                  uint16_t encryptlen,
                                  uint8_t *micResult)
{
  emberCcmCalculateAndEncryptMic(nonce,
                                 authData,
                                 authlen,
                                 encryptData,
                                 encryptlen,
                                 micResult);

  if (encryptlen > 0) {
    ccmEncryptData(encryptData, encryptlen, 1, nonce);
  }
  return EMBER_SUCCESS;
}

static uint8_t rxMic[MIC_LENGTH];
static uint8_t calcMic[MIC_LENGTH];

EmberStatus emberCcmAuthDecrypt(uint8_t *nonce,
                                uint8_t *authData,
                                uint16_t authlen,
                                uint8_t *encryptData,
                                uint16_t encryptlen,
                                uint8_t *mic)
{
  bool authenticated;
  ccmEncryptData(encryptData, encryptlen, 1, nonce);

  emberCcmCalculateAndEncryptMic(nonce,
                                 authData,
                                 authlen,
                                 encryptData,
                                 encryptlen,
                                 calcMic);
  authenticated = MEMCOMPARE(mic, calcMic, MIC_LENGTH) == 0;
  if (authenticated) {
    return EMBER_SUCCESS;
  } else {
    return EMBER_AUTH_FAILURE;
  }
}

// encryption / decryption on flat packets with an implicit ordering

EmberStatus emberCcmEncryptAndTagPacket(uint8_t *nonce,
                                        uint8_t *packet,
                                        uint16_t authlen,
                                        uint16_t encryptlen)
{
  return emberCcmEncryptAndTag(nonce,
                               packet,
                               authlen,
                               packet + authlen,
                               encryptlen,
                               packet + authlen + encryptlen);
}

EmberStatus emberCcmAuthDecryptPacket(uint8_t *nonce,
                                      uint8_t *packet,
                                      uint16_t authlen,
                                      uint16_t encryptlen)
{
  MEMCOPY(rxMic, packet + authlen + encryptlen, MIC_LENGTH);
  return emberCcmAuthDecrypt(nonce,
                             packet,
                             authlen,
                             packet + authlen,
                             encryptlen,
                             rxMic);
}

void emberCcmEncryptBytes(uint8_t *payload,
                          uint16_t length,
                          uint8_t* nonce)
{
  ccmEncryptData(payload, length, 1, nonce);
}

#endif
