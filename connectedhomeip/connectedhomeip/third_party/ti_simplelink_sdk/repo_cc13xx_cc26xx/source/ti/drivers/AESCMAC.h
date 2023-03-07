/*
 * Copyright (c) 2019-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!
 *  @file       AESCMAC.h
 *
 *  @brief      AESCMAC (CMAC and CBC-MAC) driver header
 *
 *  @anchor ti_drivers_AESCMAC_Overview
 *  # Overview #
 *  The Cipher-based Message Authentication Code (CMAC) and Cipher Block Chaining
 *  Message Authentication Code (CBC-MAC) are generic block cipher modes of
 *  operation. They can be used with any block cipher but this driver
 *  implementation uses AES.
 *
 *  Both CMAC and CBC-MAC create a message authentication code from a message of
 *  any practical length to provide authenticity and integrity assurances.
 *  CMAC is recommended over CBC-MAC because CBC-MAC is not secure for variable
 *  length messages.
 *
 *  ## CBC-MAC Drawbacks #
 *  CBC-MAC is only secure for fixed-length messages. Any single key must only be
 *  used for messages of a fixed and known length. The CMAC algorithm, which
 *  is based on a variation of CBC-MAC at its core, was developed to address that
 *  security deficiency and is the MAC algorithm recommended by NIST.
 *
 *  @anchor ti_drivers_AESCMAC_Usage
 *  # CMAC Usage #
 *  ## Before starting a CMAC operation #
 *
 *  Before starting a CMAC operation, the application must do the following:
 *      - Call #AESCMAC_init() to initialize the driver
 *      - Call #AESCMAC_Params_init() to initialize the #AESCMAC_Params to default values.
 *      - Modify the #AESCMAC_Params as desired
 *      - Call #AESCMAC_open() to open an instance of the driver
 *      - Initialize a CryptoKey. These opaque data structures are representations
 *        of keying material and its storage. Depending on how the keying material
 *        is stored (RAM or flash, key store), the CryptoKey must be
 *        initialized differently. The CMAC API can handle all types of CryptoKey.
 *        However, not all device-specific implementations support all types of CryptoKey.
 *        Devices without a key store will not support CryptoKeys with keying material
 *        stored in a key store for example.
 *        All devices support plaintext CryptoKeys.
 *
 *  ## Starting a CMAC operation #
 *
 *  The #AESCMAC_oneStepSign and #AESCMAC_oneStepVerify functions perform a CMAC operation
 *  in a single call. They will always be the most highly optimized routines with the
 *  least overhead and the fastest runtime. However, they require all plaintext
 *  or ciphertext to be available to the function at the start of the call.
 *  All devices support single call operations.
 *
 *  ## After the CMAC operation completes #
 *
 *  After the CMAC operation completes, the application should either start
 *  another operation or close the driver by calling #AESCMAC_close().
 *
 *  @anchor ti_drivers_AESCMAC_Synopsis
 *  ## Synopsis
 *  @anchor ti_drivers_AESCMAC_Synopsis_Code
 *  @code
 *  // Import CMAC Driver definitions
 *  #include <ti/drivers/CMAC.h>
 *
 *  // Define name for CMAC channel index
 *  #define AESCMAC_INSTANCE 0
 *
 *  AESCMAC_init();
 *
 *  handle = AESCMAC_open(AESCMAC_INSTANCE, NULL);
 *
 *  // Initialize symmetric key
 *  CryptoKeyPlaintext_initKey(&cryptoKey, keyingMaterial, sizeof(keyingMaterial));
 *
 *  // Initialize the operation
 *  AESCMAC_Operation_init(&operation);
 *  operation.input         = input;
 *  operation.inputLength   = sizeof(input);
 *  operation.mac           = mac;
 *  operation.macLength     = sizeof(mac);
 *
 *  signResult = AESCMAC_oneStepSign(handle, &operation, &cryptoKey);
 *
 *  AESCMAC_close(handle);
 *  @endcode
 *
 *  @anchor ti_drivers_AESCMAC_Examples
 *  ## Examples
 *
 *  ### Single call CMAC authentication with plaintext CryptoKey in blocking return mode #
 *  @code
 *
 *  #include <ti/drivers/AESCMAC.h>
 *  #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 *
 *  ...
 *
 *  uint8_t message[16]         = {0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
 *                                 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A};
 *  uint8_t keyingMaterial[16]  = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
 *                                 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
 *  uint8_t mac[16];
 *
 *  ...
 *
 *  CryptoKey cryptoKey;
 *  AESCMAC_Params params;
 *  AESCMAC_Operation operation;
 *
 *  AESCMAC_init();
 *
 *  AESCMAC_Handle handle = AESCMAC_open(0, NULL);
 *
 *  CryptoKeyPlaintext_initKey(&cryptoKey, keyingMaterial, sizeof(keyingMaterial));
 *
 *  AESCMAC_Operation_init(&operation);
 *  operation.input        = input;
 *  operation.inputLength  = sizeof(input);
 *  operation.mac          = mac;
 *  operation.macLength    = sizeof(mac);
 *
 *  int_fast16_t result = AESCMAC_oneStepSign(handle, &operation, &cryptoKey);
 *
 *  if (result != AESCMAC_STATUS_SUCCESS) {
 *      // handle error
 *  }
 *
 *  // The resulting MAC should equal the following after the operation:
 *  // 0x07, 0x0A, 0x16, 0xB4, 0x6B, 0x4D, 0x41, 0x44,
 *  // 0xF7, 0x9B, 0xDD, 0x9D, 0xD0, 0x4A, 0x28, 0x7C
 *
 *  AESCMAC_close(handle);
 *
 *  @endcode
 *
 *  ### Single call CMAC verification with plaintext CryptoKey in callback return mode #
 *  @code
 *
 *  #include <ti/drivers/AESCMAC.h>
 *  #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 *
 *  ...
 *  uint8_t message[16]         = {0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
 *                                 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A};
 *  uint8_t keyingMaterial[16]  = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
 *                                 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
 *  uint8_t expectedMac[16]     = {0x07, 0x0A, 0x16, 0xB4, 0x6B, 0x4D, 0x41, 0x44,
 *                                 0xF7, 0x9B, 0xDD, 0x9D, 0xD0, 0x4A, 0x28, 0x7C};
 *
 *  void cmacCallback(AESCMAC_Handle handle,
 *                    int_fast16_t returnValue) {
 *      if (returnValue != AESCMAC_STATUS_SUCCESS) {
 *          // handle error
 *      }
 *  }
 *
 *  void cmacStartFunction(void) {
 *      AESCMAC_Handle handle;
 *      AESCMAC_Params params;
 *      AESCMAC_Operation operation;
 *      CryptoKey cryptoKey;
 *      int_fast16_t verificationResult;
 *
 *      AESCMAC_Params_init(&params);
 *      params.returnBehavior = AESCMAC_RETURN_BEHAVIOR_CALLBACK;
 *      params.callbackFxn = cmacCallback;
 *      handle = AESCMAC_open(0, &params);
 *      if (handle == NULL) {
 *          // handle error
 *      }
 *
 *      CryptoKeyPlaintext_initKey(&cryptoKey, keyingMaterial, sizeof(keyingMaterial));
 *
 *      AESCMAC_Operation_init(&operation);
 *      operation.input        = input;
 *      operation.inputLength  = sizeof(input);
 *      operation.mac          = expectedMac;
 *      operation.macLength    = sizeof(expectedMac);
 *      verificationResult = AESCMAC_oneStepVerify(handle, &operation, &cryptoKey);
 *
 *      if (verificationResult != AESCMAC_STATUS_SUCCESS) {
 *          // handle error
 *      }
 *      // do other things while CMAC operation completes in the background
 *  }
 *  @endcode
 *
 *  ### Multi-step CMAC signature with plaintext CryptoKey in blocking return mode #
 *  @code
 *
 *  #include <ti/drivers/AESCMAC.h>
 *  #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 *
 *  #define AES_BLOCK_SIZE 16  // bytes
 *  ...
 *
 *  uint8_t keyingMaterial[32]  = {0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
 *                                 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
 *                                 0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
 *                                 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4};
 *  uint8_t message[40]         = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
 *                                 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
 *                                 0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
 *                                 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
 *                                 0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11};
 *  uint8_t mac[16];
 *
 *  CryptoKey cryptoKey;
 *  AESCMAC_Params params;
 *  AESCMAC_Operation operation;
 *
 *  AESCMAC_init();
 *
 *  AESCMAC_Handle handle = AESCMAC_open(0, NULL);
 *
 *  CryptoKeyPlaintext_initKey(&cryptoKey, keyingMaterial, sizeof(keyingMaterial));
 *
 *  AESCMAC_Operation_init(&operation);
 *
 *  // Set up multi-step sign.
 *  int_fast16_t result = AESCMAC_setupSign(handle, &cryptoKey);
 *
 *  if (result != AESCMAC_STATUS_SUCCESS) {
 *      // handle error
 *  }
 *
 *  size_t initialSegmentSize = AES_BLOCK_SIZE;
 *
 *  // Add first segment of data.
 *  operation.input       = input;
 *  operation.inputLength = initialSegmentSize;  // Must be a non-zero multiple of the block size (16-bytes) unless finalizing.
 *  result = AESCMAC_addData(handle, &operation);
 *
 *  if (result != AESCMAC_STATUS_SUCCESS) {
 *      // handle error
 *  }
 *
 *  // Finalize with the last segment of data.
 *  operation.input        = &input[initialSegmentSize];
 *  operation.inputLength  = sizeof(input) - initialSegmentSize;
 *  operation.mac          = mac;
 *  operation.macLength    = sizeof(mac);
 *
 *  result = AESCMAC_finalize(handle, &operation);
 *
 *  if (result != AESCMAC_STATUS_SUCCESS) {
 *      // handle error
 *  }
 *
 *  // The resulting MAC should equal the following after the operation:
 *  // 0xaa, 0xf3, 0xd8, 0xf1, 0xde, 0x56, 0x40, 0xc2,
 *  // 0x32, 0xf5, 0xb1, 0x69, 0xb9, 0xc9, 0x11, 0xe6
 *  ...
 *
 *  AESCMAC_close(handle);
 *
 *  @endcode
 *
 *  @anchor ti_drivers_AESCBCMAC_Usage
 *  # CBC-MAC Usage #
 *  ## Before starting a CBC-MAC operation #
 *
 *  Before starting a CBC-MAC operation, the application must do the following:
 *  - Call #AESCMAC_init() to initialize the driver
 *  - Call #AESCMAC_Params_init() to initialize the #AESCMAC_Params to
 *    default values.
 *  - Set #AESCMAC_Params.operationalMode to #AESCMAC_OPMODE_CBCMAC.
 *  - Modify the #AESCMAC_Params as desired
 *    - Call #AESCMAC_open() to open an instance of the driver
 *    - Initialize a CryptoKey. These opaque data structures are representations
 *      of keying material and its storage. Depending on how the keying material
 *      is stored (RAM or flash, key store, key blob), the CryptoKey must be
 *      initialized differently. The AESCMAC API can handle all types of
 *      CryptoKey. However, not all device-specific implementations support all
 *      types of CryptoKey. Devices without a key store will not support
 *      CryptoKeys with keying material stored in a key store for example. All
 *      devices support plaintext CryptoKeys.
 *  - Initialize the #AESCMAC_Operation using #AESCMAC_Operation_init()
 *    and set all length, key, and buffer fields.
 *
 *  ## Starting a CBC-MAC operation #
 *
 *  The #AESCMAC_oneStepSign and #AESCMAC_oneStepVerify functions perform a
 *  CBC-MAC operation in a single call. They will always be the most highly
 *  optimized routines with the least overhead and the fastest runtime. However,
 *  they require all plaintext or ciphertext to be available to the function at
 *  the start of the call. All devices support single call operations.
 *
 *  ## After the CBC-MAC operation completes #
 *
 *  After the CBC-MAC operation completes, the application should either start
 *  another operation or close the driver by calling #AESCMAC_close().
 *
 *  @anchor ti_drivers_AESCBCMAC_Synopsis
 *  ## Synopsis
 *  @anchor ti_drivers_AESCBCMAC_Synopsis_Code
 *  @code
 *  // Import AESCMAC Driver definitions
 *  #include <ti/drivers/AESCMAC.h>
 *
 *  // Define name for AESCMAC channel index
 *  #define AESCMAC_INSTANCE 0
 *
 *  AESCMAC_init();
 *
 *  AESCMAC_Params params;
 *
 *  AESCMAC_Params_init(&params);
 *  params.operationalMode = AESCMAC_OPMODE_CBCMAC;
 *
 *  handle = AESCMAC_open(AESCMAC_INSTANCE, &params);
 *
 *  // Initialize symmetric key
 *  CryptoKeyPlaintext_initKey(&cryptoKey, keyingMaterial, sizeof(keyingMaterial));
 *
 *  // Set up AESCMAC_Operation
 *  AESCMAC_Operation_init(&operation);
 *  operation.input       = plaintext;
 *  operation.inputLength = sizeof(plaintext);
 *  operation.mac         = mac;
 *  operation.macLength   = sizeof(mac);
 *
 *  signResult = AESCMAC_oneStepSign(handle, &operation);
 *
 *  AESCMAC_close(handle);
 *  @endcode
 *
 *  @anchor ti_drivers_AESCBCMAC_Examples
 *  ## Examples
 *
 *  ### One step AES CBC-MAC signature with plaintext CryptoKey in blocking return mode #
 *  @code
 *
 *  #include <ti/drivers/AESCMAC.h>
 *  #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 *
 *  ...
 *
 *  AESCMAC_Params params;
 *  AESCMAC_Handle handle;
 *  CryptoKey cryptoKey;
 *  int_fast16_t signResult;
 *
 *  // For example purposes only.
 *  // Test vector derived from RFC 3602 Case #2
 *  uint8_t plaintext[32] =       {0x56, 0x2F, 0x15, 0x9A, 0x69, 0x0C, 0x3B, 0x2F,
 *                                 0xD5, 0xBA, 0xB0, 0x62, 0x56, 0x23, 0x61, 0x57,
 *                                 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
 *                                 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
 *  uint8_t mac[16];
 *  uint8_t keyingMaterial[16] =  {0xC2, 0x86, 0x69, 0x6D, 0x88, 0x7C, 0x9A, 0xA0,
 *                                 0x61, 0x1B, 0xBB, 0x3E, 0x20, 0x25, 0xA4, 0x5A};
 *
 *  // The MAC should equal the following after the operation:
 *  //  0x75, 0x86, 0x60, 0x2D, 0x25, 0x3C, 0xFF, 0xF9,
 *  //  0x1B, 0x82, 0x66, 0xBE, 0xA6, 0xD6, 0x1A, 0xB1
 *
 *  AESCMAC_Params_init(&params);
 *  params.operationalMode = AESCMAC_OPMODE_CBCMAC;
 *
 *  handle = AESCMAC_open(0, &params);
 *
 *  if (handle == NULL) {
 *      // handle error
 *  }
 *
 *  CryptoKeyPlaintext_initKey(&cryptoKey, keyingMaterial, sizeof(keyingMaterial));
 *
 *  AESCMAC_Operation operation;
 *  AESCMAC_Operation_init(&operation); // Optional as all struct members will be set before use.
 *
 *  operation.input         = plaintext;
 *  operation.inputLength   = sizeof(plaintext);
 *  operation.mac           = mac;
 *  operation.macLength     = sizeof(mac);
 *
 *  signResult = AESCMAC_oneStepSign(handle, &operation, &cryptoKey);
 *
 *  if (signResult == AESCMAC_STATUS_SUCCESS) {
 *      // signature is available in mac[] buffer.
 *  }
 *  else {
 *      // handle error
 *  }
 *
 *  AESCMAC_close(handle);
 *
 *  @endcode
 *
 *
 *  ### Multi-step AES CBC-MAC verify with plaintext CryptoKey in polling return mode #
 *  @code
 *
 *  #include <ti/drivers/AESCMAC.h>
 *  #include <ti/drivers/types/cryptoKey/CryptoKey_Plaintext.h>
 *
 *  #define AES_BLOCK_SIZE 16  // bytes
 *  ...
 *
 *  AESCMAC_Params params;
 *  AESCMAC_Handle handle;
 *  CryptoKey cryptoKey;
 *  int_fast16_t retVal;
 *
 *  // For example purposes only.
 *  // Test vector derived from RFC 3602 Case #2
 *  uint8_t plaintext[32] =       {0x56, 0x2F, 0x15, 0x9A, 0x69, 0x0C, 0x3B, 0x2F,
 *                                 0xD5, 0xBA, 0xB0, 0x62, 0x56, 0x23, 0x61, 0x57,
 *                                 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
 *                                 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
 *  uint8_t mac[16] =             {0x75, 0x86, 0x60, 0x2D, 0x25, 0x3C, 0xFF, 0xF9,
 *                                 0x1B, 0x82, 0x66, 0xBE, 0xA6, 0xD6, 0x1A, 0xB1}
 *  uint8_t keyingMaterial[16] =  {0xC2, 0x86, 0x69, 0x6D, 0x88, 0x7C, 0x9A, 0xA0,
 *                                 0x61, 0x1B, 0xBB, 0x3E, 0x20, 0x25, 0xA4, 0x5A};
 *
 *  AESCMAC_Params_init(&params)
 *  params.returnBehavior = AESCMAC_RETURN_BEHAVIOR_POLLING;
 *  params.operationalMode = AESCMAC_OPMODE_CBCMAC;
 *
 *  handle = AESCMAC_open(0, &params);
 *
 *  if (handle == NULL) {
 *      // handle error
 *  }
 *
 *  CryptoKeyPlaintext_initKey(&cryptoKey, keyingMaterial, sizeof(keyingMaterial));
 *
 *  AESCMAC_Operation operation;
 *  AESCMAC_Operation_init(&operation); // Optional as all struct members will be set before use.
 *
 *  retVal = AESCMAC_setupVerify(handle, &cryptoKey);
 *
 *  if (retVal != AESCMAC_STATUS_SUCCESS) {
 *      // handle error
 *  }
 *
 *  operation.input        = plaintext;
 *  operation.inputLength  = AES_BLOCK_SIZE;   // Must be a non-zero multiple of the block size (16-bytes) unless finalizing.
 *  // Note: MAC pointer only needs to be set when finalizing operation.
 *
 *  retVal = AESCMAC_addData(handle, &operation);
 *
 *  if (retVal != AESCMAC_STATUS_SUCCESS) {
 *      // handle error
 *  }
 *
 *  operation.input        = plaintext + AES_BLOCK_SIZE;
 *  operation.inputLength  = AES_BLOCK_SIZE;
 *  operation.mac          = mac;
 *  operation.macLength    = sizeof(mac);
 *
 *  retVal = AESCMAC_finalize(handle, &operation);
 *
 *  // retVal should be AESCMAC_STATUS_SUCCESS to indicate that the signature
 *  // verification passed.
 *
 *  if (retVal == AESCMAC_STATUS_MAC_INVALID) {
 *      // handle invalid MAC
 *  }
 *  else if (retVal != AESCMAC_STATUS_SUCCESS) {
 *      // handle error
 *  }
 *
 *  AESCMAC_close(handle);
 *
 *  @endcode
 */

#ifndef ti_drivers_AESCMAC__include
#define ti_drivers_AESCMAC__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Common CMAC status code reservation offset.
 * CMAC driver implementations should offset status codes with
 * #AESCMAC_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define AESCMACXYZ_STATUS_ERROR0    AESCMAC_STATUS_RESERVED - 0
 * #define AESCMACXYZ_STATUS_ERROR1    AESCMAC_STATUS_RESERVED - 1
 * #define AESCMACXYZ_STATUS_ERROR2    AESCMAC_STATUS_RESERVED - 2
 * @endcode
 */
#define AESCMAC_STATUS_RESERVED              (-32)

/*!
 * @brief   Successful status code.
 *
 * Functions return #AESCMAC_STATUS_SUCCESS if the function was executed
 * successfully.
 */
#define AESCMAC_STATUS_SUCCESS               (0)

/*!
 * @brief   Generic error status code.
 *
 * Functions return #AESCMAC_STATUS_ERROR if the function was not executed
 * successfully and no more pertinent error code could be returned.
 */
#define AESCMAC_STATUS_ERROR                 (-1)

/*!
 * @brief   An error status code returned if the hardware or software resource
 * is currently unavailable.
 *
 * CMAC driver implementations may have hardware or software limitations on how
 * many clients can simultaneously perform operations. This status code is returned
 * if the mutual exclusion mechanism signals that an operation cannot currently be performed.
 */
#define AESCMAC_STATUS_RESOURCE_UNAVAILABLE  (-2)

/*!
 * @brief   The MAC verification failed.
 *
 * Functions return #AESCMAC_STATUS_MAC_INVALID if the MAC computed
 * for the provided (key, message) pair did not match the MAC provided.
 */
#define AESCMAC_STATUS_MAC_INVALID           (-3)

/*!
 *  @brief  The ongoing operation was canceled.
 */
#define AESCMAC_STATUS_CANCELED              (-4)

/*!
 *  @brief CMAC Global configuration
 *
 *  The #AESCMAC_Config structure contains a set of pointers used to characterize
 *  the CMAC driver implementation.
 *
 *  This structure needs to be defined before calling #AESCMAC_init() and it must
 *  not be changed thereafter.
 *
 *  @sa     #AESCMAC_init()
 */
typedef struct {
    /*! Pointer to a driver specific data object */
    void               *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void         const *hwAttrs;
} AESCMAC_Config;

/*!
 *  @brief  A handle that is returned from an #AESCMAC_open() call.
 */
typedef AESCMAC_Config *AESCMAC_Handle;

/*!
 * @brief   The return behavior of AESCMAC functions
 *
 * Not all AESCMAC operations exhibit the specified return behavior. Functions that do not
 * require significant computation and cannot offload that computation to a background thread
 * behave like regular functions. Which functions exhibit the specified return behavior is not
 * implementation dependent. Specifically, a software-backed implementation run on the same
 * CPU as the application will emulate the return behavior while not actually offloading
 * the computation to the background thread.
 *
 * AESCMAC functions exhibiting the specified return behavior have restrictions on the
 * context from which they may be called.
 *
 * |                                 | Task  | Hwi   | Swi   |
 * |---------------------------------|-------|-------|-------|
 * |AESCMAC_RETURN_BEHAVIOR_CALLBACK | X     | X     | X     |
 * |AESCMAC_RETURN_BEHAVIOR_BLOCKING | X     |       |       |
 * |AESCMAC_RETURN_BEHAVIOR_POLLING  | X     | X     | X     |
 *
 */
typedef enum {
    AESCMAC_RETURN_BEHAVIOR_CALLBACK = 1,    /*!< The function call will return immediately while the
                                             * MAC operation goes on in the background. The registered
                                             * callback function is called after the operation completes.
                                             * The context the callback function is called (task, HWI, SWI)
                                             * is implementation-dependent.
                                             */
    AESCMAC_RETURN_BEHAVIOR_BLOCKING = 2,    /*!< The function call will block while the MAC operation goes
                                             * on in the background. MAC operation results are available
                                             * after the function returns.
                                             */
    AESCMAC_RETURN_BEHAVIOR_POLLING  = 4,    /*!< The function call will continuously poll a flag while MAC
                                             * operation goes on in the background. MAC operation results
                                             * are available after the function returns.
                                             */
} AESCMAC_ReturnBehavior;


/*!
 *  @brief  Defines the operation modes for the AESCMAC driver.
 *
 *  By default, the driver will use CMAC to sign and verify messages.
 *  To use CBC-MAC instead of CMAC, set the operationalMode in
 *  #AESCMAC_Params accordingly before calling #AESCMAC_open or
 *  #AESCMAC_construct. The operational mode persists throughout
 *  the existance of the driver instance.
 */
typedef enum {
    AESCMAC_OPMODE_CMAC    = 1,    /*!< CMAC operational mode */
    AESCMAC_OPMODE_CBCMAC  = 2,    /*!< CBC-MAC operational mode */
} AESCMAC_OperationalMode;

/*!
 *  @brief  Struct containing the parameters required for signing or verifying a message.
 */
typedef struct {
    uint8_t *input;         /*!< - Sign: Pointer to the input message to
                             *     be authenticated.
                             *   - Verify: Pointer to the input message to be
                             *     verified. */
    uint8_t *mac;           /*!< - Sign: Pointer to the output buffer to write
                             *     the generated MAC. Buffer size must be
                             *     at least equal to @a macLength.
                             *   - Verify: Pointer to the input MAC to be
                             *     used for verification. */
    size_t  inputLength;    /*!< Length of the input message in bytes.
                             *   May be zero for CMAC but must be non-zero for CBC-MAC.
                             *   See function descriptions for further restrictions. */
    size_t  macLength;      /*!< Length of the MAC in bytes.
                             *   Must be <= 16. A length of < 8 is not recommended and
                             *   should severely restrict MAC recomputation attempts.
                             *   See appendix A of NIST SP800-38b for more information. */
} AESCMAC_Operation;

/*!
 *  @brief  Mask for the operation code.
 */
#define AESCMAC_OP_CODE_MASK    0x0F /* bits 0-3 */

/*!
 *  @brief  Enum for the operation codes supported by the driver.
 */
typedef enum {
    AESCMAC_OP_CODE_ONE_STEP = 0,
    AESCMAC_OP_CODE_SEGMENTED,
    AESCMAC_OP_CODE_FINALIZE
} AESCMAC_OperationCode;


/*!
 *  @brief  Flag indicating a sign operation. If this bit is not set, then it
 *          is a verify operation.
 */
#define AESCMAC_OP_FLAG_SIGN    0x10 /* bit 4 */


/*!
 *  @brief  Mask for all valid operation flags.
 */
#define AESCMAC_OP_FLAGS_MASK   (AESCMAC_OP_FLAG_SIGN | AESCMAC_OP_FLAG_VERIFY)

/*!
 *  @brief  Enum for the operation types supported by the driver.
 */
typedef enum {
    AESCMAC_OP_TYPE_SIGN             = AESCMAC_OP_CODE_ONE_STEP | AESCMAC_OP_FLAG_SIGN,
    AESCMAC_OP_TYPE_VERIFY           = AESCMAC_OP_CODE_ONE_STEP,
    AESCMAC_OP_TYPE_SEGMENTED_SIGN   = AESCMAC_OP_CODE_SEGMENTED | AESCMAC_OP_FLAG_SIGN,
    AESCMAC_OP_TYPE_SEGMENTED_VERIFY = AESCMAC_OP_CODE_SEGMENTED,
    AESCMAC_OP_TYPE_FINALIZE_SIGN    = AESCMAC_OP_CODE_FINALIZE | AESCMAC_OP_FLAG_SIGN,
    AESCMAC_OP_TYPE_FINALIZE_VERIFY  = AESCMAC_OP_CODE_FINALIZE
} AESCMAC_OperationType;

/*!
 *  @brief  The definition of a callback function used by the AESCMAC driver
 *          when used in ::AESCMAC_RETURN_BEHAVIOR_CALLBACK
 *
 *  @param  handle        Handle of the client that started the AESCMAC operation.
 *
 *  @param  returnValue   The result of the AESCMAC operation. May contain an error code.
 *                        Informs the application of why the callback function was
 *                        called.
 *
 *  @param  operation     Pointer to an operation struct.
 *
 *  @param  operationType Indicates which operation the callback refers to.
 */
typedef void (*AESCMAC_CallbackFxn)(AESCMAC_Handle handle,
                                    int_fast16_t returnValue,
                                    AESCMAC_Operation *operation,
                                    AESCMAC_OperationType operationType);

/*!
 *  @brief  AESCMAC Parameters
 *
 *  CMAC Parameters are used to with the #AESCMAC_open() or #AESCMAC_construct() call.
 *  Default values for these parameters are set using #AESCMAC_Params_init().
 *
 *  @sa     #AESCMAC_Params_init()
 */
typedef struct {
    AESCMAC_ReturnBehavior  returnBehavior;   /*!< Blocking, callback, or polling return behavior */
    AESCMAC_OperationalMode operationalMode;  /*!< CMAC or CBC-MAC operational mode */
    AESCMAC_CallbackFxn     callbackFxn;      /*!< Callback function pointer */
    uint32_t                timeout;          /*!< Timeout before the driver returns an error in
                                               *   ::AESCMAC_RETURN_BEHAVIOR_BLOCKING
                                               */
    void                    *custom;          /*!< Custom argument used by driver
                                               *   implementation
                                               */
} AESCMAC_Params;



/*!
 *  @brief  Default #AESCMAC_Params structure
 *
 *  @sa     #AESCMAC_Params_init()
 */
extern const AESCMAC_Params AESCMAC_defaultParams;

/*!
 *  @brief  Initializes the CMAC module.
 *
 *  @pre    The AESCMAC_config structure must exist and be persistent before this
 *          function can be called. This function must also be called before
 *          any other CMAC driver APIs. This function call does not modify any
 *          peripheral registers.
 */
void AESCMAC_init(void);

/*!
 *  @brief  Initializes the #AESCMAC_Params struct to its defaults
 *
 *  @param  [in] params  Pointer to #AESCMAC_Params structure for
 *                       initialization
 *
 *  Defaults values are:
 *      returnBehavior              = AESCMAC_RETURN_BEHAVIOR_BLOCKING
 *      operationalMode             = AESCMAC_OPMODE_CMAC
 *      callbackFxn                 = NULL
 *      timeout                     = SemaphoreP_WAIT_FOREVER
 *      custom                      = NULL
 */
void AESCMAC_Params_init(AESCMAC_Params *params);

/*!
 *  @brief  Initializes an #AESCMAC_Operation struct to its defaults
 *
 *  @param  [in] operation  Pointer to an #AESCMAC_Operation structure for
 *                          initialization
 *
 *  Defaults values are all zeros.
 */
void AESCMAC_Operation_init(AESCMAC_Operation *operation);

/*!
 *  @brief  Opens a given AESCMAC peripheral.
 *
 *  @note   #AESCMAC_Params @a operationalMode may be set to enable CBC-MAC
 *          mode but the default is CMAC mode
 *
 *  @pre    AESCMAC driver has been initialized using #AESCMAC_init()
 *
 *  @param  [in] index    Logical peripheral number for the CMAC indexed into
 *                        the AESCMAC_config table
 *
 *  @param  [in] params   Pointer to a parameter block, if NULL it will use
 *                        default values
 *
 *  @return An #AESCMAC_Handle on success or a NULL on an error or if it has
 *          been opened already.
 *
 *  @sa     #AESCMAC_init()
 *  @sa     #AESCMAC_close()
 */
AESCMAC_Handle AESCMAC_open(uint_least8_t index, const AESCMAC_Params *params);

/*!
 *  @brief  Closes a AESCMAC peripheral specified by the CMAC handle
 *
 *  @pre    #AESCMAC_open() or #AESCMAC_construct()
 *
 *  @param  handle  AESCMAC handle
 *
 *  @sa     #AESCMAC_open()
 *  @sa     #AESCMAC_construct()
 */
void AESCMAC_close(AESCMAC_Handle handle);

/*!
 *  @brief  Prepares a segmented AESCMAC sign operation
 *
 *  This function sets up a segmented AESCMAC sign operation.
 *  After a segmented operation is setup, it must be completed
 *  with #AESCMAC_finalize or cancelled with #AESCMAC_cancelOperation
 *  before another operation can be started.
 *
 *  @pre     #AESCMAC_open() or #AESCMAC_construct()
 *
 *  @param  [in] handle   AESCMAC handle
 *
 *  @param  [in] key      Pointer to a previously initialized CryptoKey.
 *
 *  @retval #AESCMAC_STATUS_SUCCESS   The operation succeeded. Segmented
 *                                    data may now be added.
 *  @retval #AESCMAC_STATUS_ERROR     The operation failed.
 *
 *  @post   #AESCMAC_addData() or #AESCMAC_finalize()
 *
 *  @sa     #AESCMAC_setupVerify()
 */
int_fast16_t AESCMAC_setupSign(AESCMAC_Handle handle, const CryptoKey *key);

/*!
 *  @brief  Prepares a segmented AESCMAC verify operation
 *
 *  This function sets up a segmented AESCMAC verify operation.
 *  After a segmented operation is setup, it must be completed
 *  with #AESCMAC_finalize or cancelled with #AESCMAC_cancelOperation
 *  before another operation can be started.
 *
 *  @pre    #AESCMAC_open() or #AESCMAC_construct()
 *
 *  @param  [in] handle   AESCMAC handle
 *
 *  @param  [in] key      Pointer to a previously initialized CryptoKey.
 *
 *  @retval #AESCMAC_STATUS_SUCCESS   The operation succeeded. Segmented
 *                                    data may now be added.
 *  @retval #AESCMAC_STATUS_ERROR     The operation failed.
 *
 *  @post   #AESCMAC_addData() or #AESCMAC_finalize()
 *
 *  @sa     #AESCMAC_setupSign()
 */
int_fast16_t AESCMAC_setupVerify(AESCMAC_Handle handle, const CryptoKey *key);

/*!
 *  @brief  Adds data to the current segmented operation
 *
 *  The @a inputLength must be a non-zero multiple of the block size (16-bytes).
 *  #AESCMAC_addData() may be called an arbitrary number of times before
 *  finishing the operation with #AESCMAC_finalize().
 *
 *  This function blocks until the final MAC been computed.
 *  It returns immediately when ::AESCMAC_RETURN_BEHAVIOR_CALLBACK is set.
 *
 *  @note   None of the buffers provided as arguments may be altered by the application
 *          during an ongoing operation. Doing so can yield corrupted plaintext.
 *
 *  @pre    #AESCMAC_setupSign() or #AESCMAC_setupVerify()
 *
 *  @param  [in] handle      AESCMAC handle
 *
 *  @param  [in] operation   Pointer to CMAC operation structure()
 *
 *  @retval #AESCMAC_STATUS_SUCCESS               The operation succeeded.
 *  @retval #AESCMAC_STATUS_ERROR                 The operation failed.
 *  @retval #AESCMAC_STATUS_RESOURCE_UNAVAILABLE  The required hardware
 *                                                resource was not available.
 *                                                Try again later.

 *
 *  @post   #AESCMAC_addData() or #AESCMAC_finalize()
 */
int_fast16_t AESCMAC_addData(AESCMAC_Handle handle,
                             AESCMAC_Operation *operation);

/*!
 *  @brief  Finalizes the current segmented operation
 *
 *  For sign operations:
 *  This function computes and writes back the final MAC @a mac of length
 *  @a macLength.
 *
 *  For verify operations:
 *  This function uses the provided MAC @a mac to authenticate an input message.
 *  The return value indicates whether the authentication was successful.
 *
 *  @note   Finalizing without additional input data is not supported.
 *          If finalization is attempted with @a inputLength of zero,
 *          #AESCMAC_STATUS_ERROR will be returned and the caller must either
 *          retry finalization with data or terminate the segmented operation
 *          by calling #AESCMAC_cancelOperation.
 *
 *  @note   None of the buffers provided as arguments may be altered by the application
 *          during an ongoing operation. Doing so can yield corrupted plaintext.
 *
 *  @pre    #AESCMAC_addData() or #AESCMAC_setupSign() or #AESCMAC_setupVerify()
 *
 *  @param  [in] handle      AESCMAC handle
 *
 *  @param  [in] operation   Pointer to CMAC operation structure()
 *
 *  @retval #AESCMAC_STATUS_SUCCESS       In ::AESCMAC_RETURN_BEHAVIOR_BLOCKING and
 *                                        ::AESCMAC_RETURN_BEHAVIOR_POLLING, this means the MAC
 *                                        was generated successfully. In ::AESCMAC_RETURN_BEHAVIOR_CALLBACK,
 *                                        this means the operation started successfully.
 *  @retval #AESCMAC_STATUS_ERROR         The operation failed.
 *  @retval #AESCMAC_STATUS_RESOURCE_UNAVAILABLE    The required hardware
 *                                                  resource was not available.
 *                                                  Try again later.
 *  @retval #AESCMAC_STATUS_MAC_INVALID   The provided MAC did not match the generated MAC.
 *                                        This return value is only valid for verify operations.
 */
int_fast16_t AESCMAC_finalize(AESCMAC_Handle handle,
                              AESCMAC_Operation *operation);


/*!
 *  @brief  Performs a AESCMAC signature in one call
 *
 *  This function uses the provided key to authenticate an input message.
 *  The resulting output is a message authentication code.
 *
 *  @note   None of the buffers provided as arguments may be altered by the application
 *          during an ongoing operation. Doing so can yield corrupted plaintext.
 *
 *  @pre    #AESCMAC_open() or #AESCMAC_construct()
 *
 *  @param  [in] handle      AESCMAC handle
 *
 *  @param  [in] operation   Pointer to AESCMAC operation structure
 *
 *  @param  [in] key         Pointer to a previously initialized CryptoKey
 *
 *  @retval #AESCMAC_STATUS_SUCCESS     In ::AESCMAC_RETURN_BEHAVIOR_BLOCKING and
 *                                      ::AESCMAC_RETURN_BEHAVIOR_POLLING, this means the MAC
 *                                      was generated successfully. In ::AESCMAC_RETURN_BEHAVIOR_CALLBACK,
 *                                      this means the operation started successfully.
 *  @retval #AESCMAC_STATUS_ERROR       The operation failed.
 *  @retval #AESCMAC_STATUS_RESOURCE_UNAVAILABLE    The required hardware
 *                                                  resource was not available.
 *                                                  Try again later.
 *
 *  @sa     #AESCMAC_oneStepVerify()
 */
int_fast16_t AESCMAC_oneStepSign(AESCMAC_Handle handle,
                                 AESCMAC_Operation *operation,
                                 CryptoKey *key);

/*!
 *  @brief  Performs a AESCMAC verification in one call
 *
 *  This function verifies that the provided message authentication code
 *  matches the one generated by the provided key and input message.
 *
 *  @note   None of the buffers provided as arguments may be altered by the application
 *          during an ongoing operation. Doing so can yield corrupted plaintext.
 *
 *  @pre    #AESCMAC_open() or #AESCMAC_construct()
 *
 *  @param  [in] handle      AESCMAC handle
 *
 *  @param  [in] operation   Pointer to AESCMAC operation structure
 *
 *  @param  [in] key         Pointer to a previously initialized CryptoKey
 *
 *  @retval #AESCMAC_STATUS_SUCCESS      In ::AESCMAC_RETURN_BEHAVIOR_BLOCKING and
 *                                       ::AESCMAC_RETURN_BEHAVIOR_POLLING, this means the MAC
 *                                       was verified successfully. In ::AESCMAC_RETURN_BEHAVIOR_CALLBACK,
 *                                       this means the operation started successfully.
 *  @retval #AESCMAC_STATUS_ERROR        The operation failed.
 *  @retval #AESCMAC_STATUS_RESOURCE_UNAVAILABLE    The required hardware
 *                                                  resource was not available.
 *                                                  Try again later.
 *  @retval #AESCMAC_STATUS_MAC_INVALID  The provided MAC did not match the generated MAC.
 *                                       This return value is only valid for verify operations.
 *
 *  @sa     AESCMAC_oneStepSign()
 */
int_fast16_t AESCMAC_oneStepVerify(AESCMAC_Handle handle,
                                   AESCMAC_Operation *operation,
                                   CryptoKey *key);

/*!
 *  @brief Cancels an ongoing AESCMAC operation.
 *
 *  Asynchronously cancels an AESCMAC operation. Only available when using
 *  AESCMAC_RETURN_BEHAVIOR_CALLBACK.
 *  The operation will terminate as though an error occurred. The
 *  return status code of the operation will be AESCMAC_STATUS_CANCELED.
 *
 *  @note Only the same thread that started an operation is permitted to cancel it.
 *        This function cannot be be called from an interrupt context or callback.
 *
 *  @param  [in] handle Handle of the operation to cancel
 *
 *  @retval #AESCMAC_STATUS_SUCCESS     The operation was canceled or the operation had already completed.
 */
int_fast16_t AESCMAC_cancelOperation(AESCMAC_Handle handle);

/**
 *  @brief  Constructs a new AESCMAC object
 *
 *  Unlike #AESCMAC_open(), #AESCMAC_construct() does not require the hwAttrs and
 *  object to be allocated in a #AESCMAC_Config array that is indexed into.
 *  Instead, the #AESCMAC_Config, hwAttrs, and object can be allocated at any
 *  location. This allows for relatively simple run-time allocation of temporary
 *  driver instances on the stack or the heap.
 *  The drawback is that this makes it more difficult to write device-agnostic
 *  code. If you use an ifdef with DeviceFamily, you can choose the correct
 *  object and hwAttrs to allocate. That compilation unit will be tied to the
 *  device it was compiled for at this point. To change devices, recompilation
 *  of the application with a different DeviceFamily setting is necessary.
 *
 *  @note   #AESCMAC_Params @a operationalMode may be set to
 *          enable CBC-MAC mode but the default is CMAC mode
 *
 *  @pre    The object struct @c config points to must be zeroed out prior to
 *          calling this function. Otherwise, unexpected behavior may ensue.
 *
 *  @param [in] config #AESCMAC_Config describing the location of the object and hwAttrs.
 *
 *  @param [in] params #AESCMAC_Params to configure the driver instance.
 *
 *  @return     Returns a #AESCMAC_Handle on success or NULL on failure.
 */
AESCMAC_Handle AESCMAC_construct(AESCMAC_Config *config, const AESCMAC_Params *params);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_AESCMAC__include */
