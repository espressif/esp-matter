/***************************************************************************//**
 * @file
 * @brief Implementation of the Cipher-based Message Authentication Code (CMAC)
 * specified in the IETF memo "The AES-CMAC Algorithm".
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

#include "aes-cmac.h"

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#endif

#if defined(EMBER_SCRIPTED_TEST)
#define HIDDEN
#else
#define HIDDEN static
#endif

// If PSA is already in use for CCM, we can pull in the PSA headers
// to get access to the configuration symbols, and figure out
// whether the build is capable of CMAC too.
#if defined(SL_CATALOG_ZIGBEE_CCM_PSA_PRESENT)
#include "psa/crypto.h"
#endif

#if defined(SL_CATALOG_SLI_PROTOCOL_CRYPTO_PRESENT) && defined(RADIOAES_PRESENT) && 0
#include "sli_protocol_crypto.h"
// Hardware implementation on top of the protocol crypto API (runs on RADIOAES,
// so it is time critical and not meant for long-running operations).
// Currently disabled (&& 0 in define guard) pending investigation on whether it is OK
// for BLE and Zigbee to both rely on the protocol crypto instance in DMP scenarios
void emberAfPluginAesMacAuthenticate(const uint8_t *key,
                                     const uint8_t *message,
                                     uint8_t messageLength,
                                     uint8_t *out)
{
  sl_status_t status = sli_aes_cmac_radio(key, 128, message, messageLength, (unsigned char[16])out);
  assert(status == SL_STATUS_OK);
}

#elif defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_AES) && defined(MBEDTLS_PSA_ACCEL_ALG_CMAC) && defined(PSA_WANT_ALG_CMAC) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS)
#include "em_device.h"
#if defined(SEMAILBOX_PRESENT)
#include "sli_se_transparent_functions.h"
#define SLI_MAC_FCT    sli_se_transparent_mac_compute
#elif defined(CRYPTO_PRESENT)
#include "sli_crypto_transparent_functions.h"
#define SLI_MAC_FCT    sli_crypto_transparent_mac_compute
#elif defined(CRYPTOACC_PRESENT)
#include "sli_cryptoacc_transparent_functions.h"
#define SLI_MAC_FCT    sli_cryptoacc_transparent_mac_compute
#else
#error "Compiling with PSA drivers for CMAC, but not for a Silicon Labs target"
#endif

void emberAfPluginAesMacAuthenticate(const uint8_t *key,
                                     const uint8_t *message,
                                     uint8_t messageLength,
                                     uint8_t *out)
{
  psa_status_t psa_status;
  size_t output_length;
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;

  psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&attr, PSA_BYTES_TO_BITS(sizeof(aesKey)));

  psa_status = SLI_MAC_FCT(
    &attr, key, 16,
    PSA_ALG_CMAC,
    message, messageLength,
    out, 16, &output_length);

  psa_reset_key_attributes(&attr);

  assert(status == PSA_SUCCESS);
  assert(output_length == 16);
}

#else
// Software CMAC implementation on top of emStandaloneEncrypt API

// Exclusive-Or operation. For two equal length strings, x and y, x XOR y is
// their bit-wise exclusive-OR.
HIDDEN void xor128(const uint8_t *x, const uint8_t *y, uint8_t *out)
{
  uint8_t i;

  for (i = 0; i < 16; i++) {
    out[i] = x[i] ^ y[i];
  }
}

// Left-shift of the string x by 1 but. The most significant bit disappears, and
// a zero comes into the least significant bit.
HIDDEN void oneBitLeftShift(const uint8_t *x, uint8_t *out)
{
  int8_t i;
  uint8_t overflow = 0x00;

  for (i = 15; i >= 0; i--) {
    out[i] = x[i] << 1;
    out[i] |= overflow;
    overflow = (x[i] & 0x80) ? 0x01 : 0x00;
  }
}

// 10^i padded output of input x. Is the concatenation of x and a single '1'
// followed by the minimum number of '0's, so that the total length is equal
// to 128 bits.
HIDDEN void padding(const uint8_t *x, uint8_t length, uint8_t *out)
{
  uint8_t i;

  for (i = 0; i < 16; i++) {
    if (i < length) {
      out[i] = x[i];
    } else if (i == length) {
      out[i] = 0x80;
    } else {
      out[i] = 0x00;
    }
  }
}

extern void emGetKeyFromCore(uint8_t* key);
extern void emLoadKeyIntoCore(const uint8_t* key);
extern void emStandAloneEncryptBlock(uint8_t *block);

HIDDEN void aesEncrypt(uint8_t *block, const uint8_t *key)
{
  uint8_t temp[16];

  ATOMIC(
    emGetKeyFromCore(temp);
    emLoadKeyIntoCore((uint8_t*)key);
    emStandAloneEncryptBlock(block);
    emLoadKeyIntoCore(temp);
    )
}

HIDDEN void generateSubKey(const uint8_t *key, uint8_t *outKey1, uint8_t *outKey2)
{
  uint8_t L[16];
  uint8_t constRb[16];

  initToConstRb(constRb);

  // Step 1
  initToConstZero(L);
  aesEncrypt(L, key);

  // Step 2
  oneBitLeftShift(L, outKey1); // // K1:= L << 1;

  if (MSB(L)) { // K1:= (L << 1) XOR const_Rb;
    xor128(outKey1, constRb, outKey1);
  }
  // Step 3
  oneBitLeftShift(outKey1, outKey2); // // K2 := K1 << 1;

  if (MSB(outKey1)) { // K2 := (K1 << 1) XOR const_Rb;
    xor128(outKey2, constRb, outKey2);
  }
}

void emberAfPluginAesMacAuthenticate(const uint8_t *key,
                                     const uint8_t *message,
                                     uint8_t messageLength,
                                     uint8_t *out)
{
  uint8_t key1[16];
  uint8_t key2[16];
  uint8_t lastBlock[16];
  uint8_t blockNum;
  bool isLastBlockComplete;
  uint8_t i;

  // Step 1
  generateSubKey(key, key1, key2);

  // Step 2 (we perform ceil(x/y) by doing: (x + y - 1) / y).
  blockNum = (messageLength + 15) / 16;

  // Step 3
  if (blockNum == 0) {
    blockNum = 1;
    isLastBlockComplete = false;
  } else {
    isLastBlockComplete = ((messageLength % 16) == 0);
  }

  // Step 4
  if (isLastBlockComplete) {
    xor128(message + (blockNum - 1) * 16, key1, lastBlock);
  } else {
    padding(message + (blockNum - 1) * 16, messageLength % 16, lastBlock);
    xor128(lastBlock, key2, lastBlock);
  }

  // Step 5
  initToConstZero(out);

  // Step 6
  for (i = 0; i < blockNum - 1; i++) {
    xor128(out, message + i * 16, out);
    aesEncrypt(out, key);
  }

  xor128(out, lastBlock, out);
  aesEncrypt(out, key);
}
#endif
