/*
 * Copyright (c) 2017-2021, Texas Instruments Incorporated
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
/*!****************************************************************************
 *  @file       AESECB.h
 *
 *  @brief      AESECB driver header
 *
 *  @anchor ti_drivers_AESECB_Overview
 *  # Overview #
 *  The Electronic Code Book (ECB) mode of operation is a generic
 *  encryption block cipher mode. It can be used with any block cipher.
 *  AESECB encrypts or decrypts one or multiple blocks of plaintext or ciphertext
 *  using the Advanced Encryption Standard (AES) block cipher.
 *  Each input block is individually encrypted or decrypted. This means that
 *  blocks of ciphertext can be decrypted individually and out of order.
 *  Encrypting the same plaintext using the same key yields identical ciphertext.
 *  This raises several security issues. For this reason, it is not recommended
 *  that ECB be used unless interfacing with legacy systems that can't be updated
 *  or where a standard specifies its use. Better alternatives would be an
 *  authenticated encryption with associated data (AEAD) mode such as
 *  CCM or GCM.
 *
 *  The AES key is a shared secret between the two parties and has a length
 *  of 128, 192, or 256 bits.
 *
 *  @anchor ti_drivers_AESECB_Usage
 *  # Usage #
 *
 *  ## Before starting an ECB operation #
 *
 *  Before starting an ECB operation, the application must do the following:
 *      - Call AESECB_init() to initialize the driver
 *      - Call AESECB_Params_init() to initialize the AESECB_Params to default values.
 *      - Modify the AESECB_Params as desired
 *      - Call AESECB_open() to open an instance of the driver
 *      - Initialize a CryptoKey. These opaque datastructures are representations
 *        of keying material and its storage. Depending on how the keying material
 *        is stored (RAM or flash, key store), the CryptoKey must be
 *        initialized differently. The AESECB API can handle all types of CryptoKey.
 *        However, not all device-specific implementations support all types of CryptoKey.
 *        Devices without a key store will not support CryptoKeys with keying material
 *        stored in a key store for example.
 *        All devices support plaintext CryptoKeys.
 *      - Initialize the AESECB_Operation using AESECB_Operation_init() and set all
 *        length, key, and buffer fields.
 *
 *  ## Starting an ECB operation #
 *
 *  The AESECB_oneStepEncrypt and AESECB_oneStepDecrypt functions do an ECB operation in a single call.
 *  They will always be the most highly optimized routines with the least overhead and the fastest
 *  runtime. Since ECB plaintext blocks are simply encrypted with the block cipher block by block,
 *  there is no difference in the ciphertext between encrypting two blocks in one go or encrypting
 *  each block individually.
 *
 *  ## After the ECB operation completes #
 *
 *  After the ECB operation completes, the application should either start another operation
 *  or close the driver by calling AESECB_close()
 *
 *  @anchor ti_drivers_AESECB_Synopsis
 *  ## Synopsis
 *  @anchor ti_drivers_AESECB_Synopsis_Code
 *  @code
 *  // Import AESECB Driver definitions
 *  #include <ti/drivers/AESECB.h>
 *
 *  AESECB_init();
 *
 *  // Define name for AESECB channel index
 *  #define AESECB_INSTANCE 0
 *
 *  handle = AESECB_open(AESECB_INSTANCE, NULL);
 *
 *  // Initialize symmetric key
 *  CryptoKeyPlaintext_initKey(&cryptoKey, keyingMaterial, sizeof(keyingMaterial));
 *
 *  // Set up AESECB_Operation
 *  AESECB_Operation_init(&operation);
 *  operation.key               = &cryptoKey;
 *  operation.input             = plaintext;
 *  operation.output            = ciphertext;
 *  // Input length must be a non-zero multiple of block-size (16 bytes)
 *  // for one-step operations. The user or application should take care of
 *  // necessary padding.
 *  operation.inputLength       = sizeof(plaintext);
 *
 *  encryptionResult = AESECB_oneStepEncrypt(handle, &operation);
 *
 *  AESECB_close(handle);
 *  @endcode
 *
 *  @anchor ti_drivers_AESECB_Examples
 *
 *  ## Examples
 *
 *  ### Encryption of multiple plaintext blocks in blocking mode #
 *  @code
 *
 *  #include <ti/drivers/AESECB.h>
 *  #include <ti/drivers/cryptouils/cryptokey/CryptoKeyPlaintext.h>
 *
 *  ...
 *
 *  AESECB_Handle handle;
 *  CryptoKey cryptoKey;
 *  int_fast16_t encryptionResult;
 *  uint8_t plaintext[] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
 *                         0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
 *                         0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
 *                         0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51};
 *  uint8_t ciphertext[sizeof(plaintext)];
 *  uint8_t keyingMaterial[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
 *                                0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c}
 *
 *  handle = AESECB_open(0, NULL);
 *
 *  if (handle == NULL) {
 *      // handle error
 *  }
 *
 *  CryptoKeyPlaintext_initKey(&cryptoKey, keyingMaterial, sizeof(keyingMaterial));
 *
 *      AESECB_Operation operation;
 *      AESECB_Operation_init(&operation);
 *
 *      operation.key               = &cryptoKey;
 *      operation.input             = plaintext;
 *      operation.output            = ciphertext;
 *      // Input length must be a non-zero multiple of block-size (16 bytes)
 *      // for one-step operations. The user or application should take care of
 *      // necessary padding.
 *      operation.inputLength       = sizeof(plaintext);
 *
 *  encryptionResult = AESECB_oneStepEncrypt(handle, &operation);
 *
 *  if (encryptionResult != AESECB_STATUS_SUCCESS) {
 *      // handle error
 *  }
 *
 *  // The resultant ciphertext should be:
 *  // 0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
 *  // 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
 *  // 0xf5, 0xd3, 0xd5, 0x85, 0x03, 0xb9, 0x69, 0x9d,
 *  // 0xe7, 0x85, 0x89, 0x5a, 0x96, 0xfd, 0xba, 0xaf
 *
 *
 *  AESECB_close(handle);
 *
 *  @endcode
 *
 *  ### Single call ECB decryption in callback mode #
 *  @code
 *
 *  #include <ti/drivers/AESECB.h>
 *  #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 *
 *  ...
 *
 *  uint8_t ciphertext[]                    = {0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c,
 *                                             0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8};
 *  uint8_t keyingMaterial[32]              = {0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
 *                                             0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
 *                                             0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
 *                                             0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4};
 *  uint8_t plaintext[sizeof(ciphertext)];
 *
 *  // The plaintext should be the following after the decryption operation:
 *  // 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
 *  // 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
 *
 *
 *  void ecbCallback(AESECB_Handle handle,
 *                   int_fast16_t returnValue,
 *                   AESECB_Operation *operation,
 *                   AESECB_OperationType operationType) {
 *
 *      if (returnValue != AESECB_STATUS_SUCCESS) {
 *          // handle error
 *      }
 *  }
 *
 *  AESECB_Operation operation;
 *
 *  void ecbStartFunction(void) {
 *      AESECB_Handle handle;
 *      AESECB_Params params;
 *      CryptoKey cryptoKey;
 *      int_fast16_t decryptionResult;
 *
 *      AESECB_Params_init(&params);
 *      params.returnBehavior = AESECB_RETURN_BEHAVIOR_CALLBACK;
 *      params.callbackFxn = ecbCallback;
 *
 *      handle = AESECB_open(0, &params);
 *
 *      if (handle == NULL) {
 *          // handle error
 *      }
 *
 *      CryptoKeyPlaintext_initKey(&cryptoKey, keyingMaterial, sizeof(keyingMaterial));
 *
 *      AESECB_Operation_init(&operation);
 *
 *      operation.key               = &cryptoKey;
 *      operation.input             = plaintext;
 *      operation.output            = ciphertext;
 *      // Input length must be a non-zero multiple of block-size (16 bytes)
 *      // for one-step operations. The user or application should take care of
 *      // necessary padding.
 *      operation.inputLength       = sizeof(plaintext);
 *
 *      decryptionResult = AESECB_oneStepDecrypt(handle, &operation);
 *
 *      if (decryptionResult != AESECB_STATUS_SUCCESS) {
 *          // handle error
 *      }
 *
 *      // do other things while ECB operation completes in the background
 *
 *  }
 *
 *  @endcode
 *
 *  ### Multi-step ECB encryption in blocking mode #
 *  @code
 *
 *  #include <ti/drivers/AESECB.h>
 *  #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 *
 *  #define AES_BLOCK_SIZE  16      // bytes
 *
 *  ...
 *
 *  AESECB_Handle handle;
 *  CryptoKey cryptoKey;
 *  int_fast16_t encryptionResult;
 *  int_fast16_t setupEncryptionResult;
 *  int_fast16_t finalizeEncryptionResult;
 *  uint8_t plaintext[] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
 *                         0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
 *                         0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
 *                         0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51};
 *  uint8_t ciphertext[sizeof(plaintext)];
 *  uint8_t keyingMaterial[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
 *                                0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c}
 *
 *  handle = AESECB_open(0, NULL);
 *
 *  if (handle == NULL) {
 *      // handle error
 *  }
 *
 *  CryptoKeyPlaintext_initKey(&cryptoKey, keyingMaterial, sizeof(keyingMaterial));
 *
 *  setupEncryptionResult = AESECB_setupEncrypt(handle, &cryptoKey);
 *  if (setupEncryptionResult != AESECB_STATUS_SUCCESS) {
 *      // handle error
 *  }
 *
 *  AESECB_Operation operation;
 *  AESECB_Operation_init(&operation);
 *
 *  operation.key               = &cryptoKey;
 *  operation.input             = plaintext;
 *  operation.output            = ciphertext;
 *  // Input length must be a non-zero multiple of block-size (16 bytes) for calling
 *  // #AESECB_addData(). The user or application should take care of necessary padding
 *  // if the final block of data is being added for the entire segmented operation.
 *  operation.inputLength       = AES_BLOCK_SIZE;
 *
 *  encryptionResult = AESECB_addData(handle, &operation);
 *  if (encryptionResult != AESECB_STATUS_SUCCESS) {
 *      // handle error
 *  }
 *
 *  operation.input             = plaintext + AES_BLOCK_SIZE;
 *  operation.output            = ciphertext + AES_BLOCK_SIZE;
 *  // Input length must either be a non-zero multiple of block-size (16 bytes)
 *  // for calling #AESECB_finalize(), or it could be zero in case of finalizing without
 *  // any more data. The user or application should take care of necessary padding
 *  // for the last block of data.
 *  operation.inputLength       = AES_BLOCK_SIZE;
 *
 *  finalizeEncryptionResult = AESECB_finalize(handle, &operation);
 *  if (finalizeEncryptionResult != AESECB_STATUS_SUCCESS) {
 *      // handle error
 *  }
 *
 *  // The resultant ciphertext should be:
 *  // 0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
 *  // 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
 *  // 0xf5, 0xd3, 0xd5, 0x85, 0x03, 0xb9, 0x69, 0x9d,
 *  // 0xe7, 0x85, 0x89, 0x5a, 0x96, 0xfd, 0xba, 0xaf
 *
 *
 *  AESECB_close(handle);
 *
 *  }
 *
 *  @endcode
 *
 *  ### Multi-step ECB decryption in callback mode #
 *  @code
 *
 *  #include <ti/drivers/AESECB.h>
 *  #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 *
 *  #define AES_BLOCK_SIZE  16      // bytes
 *
 *  ...
 *  uint8_t ciphertext[]                    = {0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c,
 *                                             0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8};
 *  uint8_t keyingMaterial[32]              = {0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
 *                                             0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
 *                                             0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
 *                                             0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4};
 *  uint8_t plaintext[sizeof(ciphertext)];
 *
 *  // The plaintext should be the following after the decryption operation:
 *  // 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
 *  // 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
 *
 *
 *  void ecbCallback(AESECB_Handle handle,
 *                   int_fast16_t returnValue,
 *                   AESECB_Operation *operation,
 *                   AESECB_OperationType operationType) {
 *
 *      if (returnValue != AESECB_STATUS_SUCCESS) {
 *          // handle error
 *      }
 *  }
 *
 *  AESECB_Operation operation;
 *
 *  void ecbStartFunction(void) {
 *      AESECB_Handle handle;
 *      AESECB_Params params;
 *      CryptoKey cryptoKey;
 *      int_fast16_t decryptionResult;
 *      int_fast16_t setupDecryptionResult;
 *      int_fast16_t finalizeDecryptionResult;
 *
 *      AESECB_Params_init(&params);
 *      params.returnBehavior = AESECB_RETURN_BEHAVIOR_CALLBACK;
 *      params.callbackFxn = ecbCallback;
 *
 *      handle = AESECB_open(0, &params);
 *
 *      if (handle == NULL) {
 *          // handle error
 *      }
 *
 *      CryptoKeyPlaintext_initKey(&cryptoKey, keyingMaterial, sizeof(keyingMaterial));
 *
 *      setupDecryptionResult = AESECB_setupDecrypt(handle, &cryptoKey);
 *      if (setupDecryptionResult != AESECB_STATUS_SUCCESS) {
 *          // handle error
 *      }
 *
 *      AESECB_Operation_init(&operation);
 *
 *      operation.key               = &cryptoKey;
 *      operation.input             = plaintext;
 *      operation.output            = ciphertext;
 *      // Input length must be a non-zero multiple of block-size (16 bytes) for calling
 *      // #AESECB_addData(). The user or application should take care of necessary padding
 *      // if the final block of data is being added for the entire segmented operation.
 *      operation.inputLength       = AES_BLOCK_SIZE;
 *
 *      decryptionResult = AESECB_addData(handle, &operation);
 *      if (decryptionResult != AESECB_STATUS_SUCCESS) {
 *          // handle error
 *      }
 *
 *      // do other things while ECB operation completes in the background
 *
 *      // Input length must either be a non-zero multiple of block-size (16 bytes)
 *      // for calling #AESECB_finalize(), or it could be zero in case of finalizing without
 *      // any more data as shown in this example. There's no more data involved and padding
 *      // is not applicable for this finalization operation.
 *      operation.inputLength       = 0;
 *
 *      finalizeDecryptionResult = AESECB_finalize(handle, &operation);
 *      if (finalizeDecryptionResult != AESECB_STATUS_SUCCESS) {
 *          // handle error
 *      }
 *
 *  }
 *
 *  @endcode
 */

#ifndef ti_drivers_AESECB__include
#define ti_drivers_AESECB__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Common AESECB status code reservation offset.
 * AESECB driver implementations should offset status codes with
 * AESECB_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define AESECBXYZ_STATUS_ERROR0    AESECB_STATUS_RESERVED - 0
 * #define AESECBXYZ_STATUS_ERROR1    AESECB_STATUS_RESERVED - 1
 * #define AESECBXYZ_STATUS_ERROR2    AESECB_STATUS_RESERVED - 2
 * @endcode
 */
#define AESECB_STATUS_RESERVED        (-32)

/*!
 * @brief   Successful status code.
 *
 * Functions return AESECB_STATUS_SUCCESS if the function was executed
 * successfully.
 */
#define AESECB_STATUS_SUCCESS         (0)

/*!
 * @brief   Generic error status code.
 *
 * Functions return AESECB_STATUS_ERROR if the function was not executed
 * successfully and no more pertinent error code could be returned.
 */
#define AESECB_STATUS_ERROR           (-1)

/*!
 * @brief   An error status code returned if the hardware or software resource
 * is currently unavailable.
 *
 * AESECB driver implementations may have hardware or software limitations on how
 * many clients can simultaneously perform operations. This status code is returned
 * if the mutual exclusion mechanism signals that an operation cannot currently be performed.
 */
#define AESECB_STATUS_RESOURCE_UNAVAILABLE (-2)

/*!
 *  @brief  The ongoing operation was canceled.
 */
#define AESECB_STATUS_CANCELED (-3)

/*!
 *  @brief AESECB Global configuration
 *
 *  The AESECB_Config structure contains a set of pointers used to characterize
 *  the AESECB driver implementation.
 *
 *  This structure needs to be defined before calling AESECB_init() and it must
 *  not be changed thereafter.
 *
 *  @sa     AESECB_init()
 */
typedef struct {
    /*! Pointer to a driver specific data object */
    void               *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void         const *hwAttrs;
} AESECB_Config;

/*!
 *  @brief  A handle that is returned from an AESECB_open() call.
 */
typedef AESECB_Config *AESECB_Handle;

/*!
 * @brief   The way in which ECB function calls return after performing an
 * encryption + authentication or decryption + verification operation.
 *
 * Not all ECB operations exhibit the specified return behavior. Functions that do not
 * require significant computation and cannot offload that computation to a background thread
 * behave like regular functions. Which functions exhibit the specified return behavior is not
 * implementation dependent. Specifically, a software-backed implementation run on the same
 * CPU as the application will emulate the return behavior while not actually offloading
 * the computation to the background thread.
 *
 * AESECB functions exhibiting the specified return behavior have restrictions on the
 * context from which they may be called.
 *
 * |                                | Task  | Hwi   | Swi   |
 * |--------------------------------|-------|-------|-------|
 * |AESECB_RETURN_BEHAVIOR_CALLBACK | X     | X     | X     |
 * |AESECB_RETURN_BEHAVIOR_BLOCKING | X     |       |       |
 * |AESECB_RETURN_BEHAVIOR_POLLING  | X     | X     | X     |
 *
 */
typedef enum {
    AESECB_RETURN_BEHAVIOR_CALLBACK = 1,    /*!< The function call will return immediately while the
                                             *   ECB operation goes on in the background. The registered
                                             *   callback function is called after the operation completes.
                                             *   The context the callback function is called (task, HWI, SWI)
                                             *   is implementation-dependent.
                                             */
    AESECB_RETURN_BEHAVIOR_BLOCKING = 2,    /*!< The function call will block while ECB operation goes
                                             *   on in the background. ECB operation results are available
                                             *   after the function returns.
                                             */
    AESECB_RETURN_BEHAVIOR_POLLING  = 4,    /*!< The function call will continuously poll a flag while ECB
                                             *   operation goes on in the background. ECB operation results
                                             *   are available after the function returns.
                                             */
} AESECB_ReturnBehavior;

/*!
 *  @brief  Enum for the direction of the ECB operation.
 */
typedef enum {
    AESECB_MODE_ENCRYPT = 1,
    AESECB_MODE_DECRYPT = 2,
} AESECB_Mode;

/*!
 *  @brief  Struct containing the parameters required for encrypting/decrypting
 *          and a message.
 */
typedef struct {
   CryptoKey                *key;                        /*!< A previously initialized CryptoKey */
   uint8_t                  *input;                      /*!<
                                                          *   - Encryption: A pointer to the plaintext buffer.
                                                          *   - Decryption: A pointer to the ciphertext buffer.
                                                          *
                                                          *   Both input and output buffers should be of the size
                                                          *   \c inputLength in bytes each.
                                                          */
   uint8_t                  *output;                     /*!<
                                                          *   - Encryption: A pointer to the buffer to store the
                                                          *     resulting ciphertext.
                                                          *   - Decryption: A pointer to the buffer to store the
                                                          *   resulting plaintext.
                                                          *
                                                          *   Both input and output buffers should be of the size
                                                          *   \c inputLength in bytes each.
                                                          */
   size_t                   inputLength;                 /*!<
                                                          *   - One-step operation: Total length of the input in
                                                          *     bytes.
                                                          *   - Multi-step / Segmented operation: Length of the
                                                          *     input in bytes for that #AESECB_addData()
                                                          *     or #AESECB_finalize() call.
                                                          *
                                                          *   The output will be the same length as the input.
                                                          *
                                                          *   Must be a non-zero multiple of block-size (16 bytes).
                                                          *   May be 0 only when calling #AESECB_finalize() to
                                                          *   finalize a multi-step operation without additional
                                                          *   data.
                                                          *   The user or application should take care of
                                                          *   necessary padding.
                                                          */
} AESECB_Operation;

/*!
 *  @brief  Enum for the operation types supported by the driver.
 */
typedef enum {
    AESECB_OPERATION_TYPE_ENCRYPT = 1,
    AESECB_OPERATION_TYPE_DECRYPT = 2,
    AESECB_OPERATION_TYPE_ENCRYPT_SEGMENTED = 3,
    AESECB_OPERATION_TYPE_DECRYPT_SEGMENTED = 4,
    AESECB_OPERATION_TYPE_FINALIZE_ENCRYPT_SEGMENTED = 5,
    AESECB_OPERATION_TYPE_FINALIZE_DECRYPT_SEGMENTED = 6
} AESECB_OperationType;

/*!
 *  @brief  The definition of a callback function used by the AESECB driver
 *          when used in ::AESECB_RETURN_BEHAVIOR_CALLBACK
 *
 *  @param  handle        Handle of the client that started the ECB operation.
 *
 *  @param  returnValue   The result of the ECB operation. May contain an error code.
 *                        Informs the application of why the callback function was
 *                        called.
 *
 *  @param  operation     A pointer to an operation struct.
 *
 *  @param  operationType This parameter determines which operation the
 *                        callback refers to.
 */
typedef void (*AESECB_CallbackFxn) (AESECB_Handle handle,
                                    int_fast16_t returnValue,
                                    AESECB_Operation *operation,
                                    AESECB_OperationType operationType);

/*!
 *  @brief  ECB Parameters
 *
 *  ECB Parameters are used with the #AESECB_open() call. Default values for
 *  these parameters are set using #AESECB_Params_init().
 *
 *  @sa     #AESECB_Params_init()
 */
typedef struct {
    AESECB_ReturnBehavior   returnBehavior;             /*!< Blocking, callback, or polling return behavior */
    AESECB_CallbackFxn      callbackFxn;                /*!< Callback function pointer */
    uint32_t                timeout;                    /*!< Timeout before the driver returns an error in
                                                         *   ::AESECB_RETURN_BEHAVIOR_BLOCKING
                                                         */
    void                   *custom;                     /*!< Custom argument used by driver
                                                         *   implementation
                                                         */
} AESECB_Params;

/*!
 *  @brief Default #AESECB_Params structure
 *
 *  @sa     #AESECB_Params_init()
 */
extern const AESECB_Params AESECB_defaultParams;

/*!
 *  @brief  This function initializes the ECB module.
 *
 *  @pre    The AESECB_config structure must exist and be persistent before this
 *          function can be called. This function must also be called before
 *          any other ECB driver APIs. This function call does not modify any
 *          peripheral registers.
 */
void AESECB_init(void);

/*!
 *  @brief  Function to initialize the #AESECB_Params struct to its defaults
 *
 *  @param  params      An pointer to #AESECB_Params structure for
 *                      initialization
 *
 *  Defaults values are:
 *      returnBehavior              = AESECB_RETURN_BEHAVIOR_BLOCKING
 *      callbackFxn                 = NULL
 *      timeout                     = SemaphoreP_WAIT_FOREVER
 *      custom                      = NULL
 */
void AESECB_Params_init(AESECB_Params *params);

/*!
 *  @brief  This function opens a given ECB peripheral.
 *
 *  @pre    ECB controller has been initialized using #AESECB_init()
 *
 *  @param  [in] index    Logical peripheral number for the ECB indexed into
 *                        the AESECB_config table
 *
 *  @param  [in] params   Pointer to an parameter block, if NULL it will use
 *                        default values.
 *
 *  @return An #AESECB_Handle on success or a NULL on an error or if it has been
 *          opened already.
 *
 *  @sa     #AESECB_init()
 *  @sa     #AESECB_close()
 */
AESECB_Handle AESECB_open(uint_least8_t index, const AESECB_Params *params);

/*!
 *  @brief  Function to close an ECB peripheral specified by the ECB handle
 *
 *  @pre    #AESECB_open() or #AESECB_construct()
 *
 *  @param  [in] handle An ECB handle returned from #AESECB_open() or #AESECB_construct()
 *
 *  @sa     AESECB_open()
 */
void AESECB_close(AESECB_Handle handle);

/*!
 *  @brief  Function to initialize an #AESECB_Operation struct to its defaults
 *
 *  @param  [in] operationStruct     An pointer to #AESECB_Operation structure for
 *                                  initialization
 *
 *  Defaults values are all zeros.
 */
void AESECB_Operation_init(AESECB_Operation *operationStruct);

/*!
 *  @brief  Function to perform an AESECB encryption operation in one call.
 *
 *  @note   None of the buffers provided as arguments may be altered by the application during an ongoing operation.
 *          Doing so can yield corrupted ciphertext or incorrect authentication.
 *
 *  @pre    #AESECB_open() or #AESECB_construct(), and AESECB_Operation_init() have to be called first.
 *
 *  @param  [in] handle                 An ECB handle returned from #AESECB_open() or #AESECB_construct()
 *
 *  @param  [in] operation              A pointer to a struct containing the parameters required to perform the operation.
 *
 *  @retval #AESECB_STATUS_SUCCESS               The operation succeeded.
 *  @retval #AESECB_STATUS_ERROR                 The operation failed.
 *  @retval #AESECB_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource was not available. Try again later.
 *
 *  @sa     AESECB_oneStepDecrypt()
 */
int_fast16_t AESECB_oneStepEncrypt(AESECB_Handle handle, AESECB_Operation *operation);

/*!
 *  @brief  Function to perform an AESECB decryption in one call.
 *
 *  @note   None of the buffers provided as arguments may be altered by the application during an ongoing operation.
 *          Doing so can yield corrupted plaintext or incorrectly failed verification.
 *
 *  @pre    #AESECB_open() or #AESECB_construct(), and AESECB_Operation_init() have to be called first.
 *
 *  @param  [in] handle                 An ECB handle returned from #AESECB_open() or #AESECB_construct()
 *
 *  @param  [in] operation              A pointer to a struct containing the parameters required to perform the operation.
 *
 *  @retval #AESECB_STATUS_SUCCESS               The operation succeeded.
 *  @retval #AESECB_STATUS_ERROR                 The operation failed.
 *  @retval #AESECB_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource was not available. Try again later.
 *
 *  @sa     AESECB_oneStepEncrypt()
 */
int_fast16_t AESECB_oneStepDecrypt(AESECB_Handle handle, AESECB_Operation *operation);

/*!
 *  @brief  Function to prepare a segmented AESECB encryption operation.
 *
 *  This functions sets up a segmented AESECB encryption operation.
 *
 *  @pre    #AESECB_open() or #AESECB_construct()
 *
 *  @param  [in] handle     An ECB handle returned from #AESECB_open()
 *                          or #AESECB_construct()
 *
 *  @param  [in] key        A previously initialized CryptoKey.
 *
 *  @retval #AESECB_STATUS_SUCCESS                  The operation succeeded.
 *  @retval #AESECB_STATUS_ERROR                    The operation failed.
 *
 *  @post   #AESECB_addData()
 */
int_fast16_t AESECB_setupEncrypt(AESECB_Handle handle, const CryptoKey *key);

/*!
 *  @brief  Function to prepare a segmented AESECB decryption operation.
 *
 *  This functions sets up a segmented AESECB decryption operation.
 *
 *  @pre    #AESECB_open() or #AESECB_construct()
 *
 *  @param  [in] handle     An ECB handle returned from #AESECB_open()
 *                          or #AESECB_construct()
 *
 *  @param  [in] key        A previously initialized CryptoKey.
 *
 *  @retval #AESECB_STATUS_SUCCESS                  The operation succeeded.
 *  @retval #AESECB_STATUS_ERROR                    The operation failed.
 *
 *  @post   #AESECB_addData()
 */
int_fast16_t AESECB_setupDecrypt(AESECB_Handle handle, const CryptoKey *key);

/*!
 *  @brief  Encrypts or decrypts segment of @a data with a @a length
 *
 *  #AESECB_addData() may be called an arbitrary number times before finishing the operation with
 *  #AESECB_finalize(). Note that this function is called for use with segmented operations. For
 *  segmented operations, @c inputLength will govern the input/output lengths and
 *  must be a AES block size multiple (16-bytes).
 *
 *  @pre    #AESECB_setupEncrypt() or #AESECB_setupDecrypt()
 *
 *  @param  [in] handle         An ECB handle returned from #AESECB_open() or #AESECB_construct()
 *
 *  @param  [in] operation      Pointer to ECB operation structure()
 *
 *  @retval #AESECB_STATUS_SUCCESS               The operation succeeded.
 *  @retval #AESECB_STATUS_ERROR                 The operation failed.
 *  @retval #AESECB_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource was not available. Try again later.
 *
 *  @post   #AESECB_addData() or #AESECB_finalize()
 */
int_fast16_t AESECB_addData(AESECB_Handle handle,
                            AESECB_Operation * operation);

/*!
 *  @brief  Finalize the AES transaction. If new data needs to be added,
 *  @c inputLength will be used to govern how many bytes will be written.
 *
 *  @pre    #AESECB_addData()
 *
 *  @param  [in] handle         An ECB handle returned from #AESECB_open() or #AESECB_construct()
 *
 *  @param  [in] operation      Pointer to ECB operation structure()
 *
 *  @retval #AESECB_STATUS_SUCCESS              In ::AESECB_RETURN_BEHAVIOR_BLOCKING and
 *                                              ::AESECB_RETURN_BEHAVIOR_POLLING, this means the ECB
 *                                              was generated successfully. In ::AESECB_RETURN_BEHAVIOR_CALLBACK,
 *                                              this means the operation started successfully.
 *  @retval #AESECB_STATUS_ERROR                The operation failed.
 *  @retval #AESECB_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource was not available. Try again later.
 */
int_fast16_t AESECB_finalize(AESECB_Handle handle,
                             AESECB_Operation * operation);

/*!
 *  @brief Cancels an ongoing AESECB operation.
 *
 *  Asynchronously cancels an AESECB operation. Only available when using
 *  AESECB_RETURN_BEHAVIOR_CALLBACK.
 *  The operation will terminate as though an error occurred. The
 *  return status code of the operation will be AESECB_STATUS_CANCELED.
 *
 *  @param  [in] handle Handle of the operation to cancel
 *
 *  @retval #AESECB_STATUS_SUCCESS               The operation was canceled, or the requested operation had already completed.
 */
int_fast16_t AESECB_cancelOperation(AESECB_Handle handle);

/**
 *  @brief  Constructs a new AESECB object
 *
 *  Unlike #AESECB_open(), #AESECB_construct() does not require the hwAttrs and
 *  object to be allocated in a #AESECB_Config array that is indexed into.
 *  Instead, the #AESECB_Config, hwAttrs, and object can be allocated at any
 *  location. This allows for relatively simple run-time allocation of temporary
 *  driver instances on the stack or the heap.
 *  The drawback is that this makes it more difficult to write device-agnostic
 *  code. If you use an ifdef with DeviceFamily, you can choose the correct
 *  object and hwAttrs to allocate. That compilation unit will be tied to the
 *  device it was compiled for at this point. To change devices, recompilation
 *  of the application with a different DeviceFamily setting is necessary.
 *
 *  @param  config #AESECB_Config describing the location of the object and hwAttrs.
 *
 *  @param  params #AESECB_Params to configure the driver instance.
 *
 *  @return        Returns a #AESECB_Handle on success or NULL on failure.
 *
 *  @pre    The object struct @c config points to must be zeroed out prior to
 *          calling this function. Otherwise, unexpected behavior may ensue.
 */
AESECB_Handle AESECB_construct(AESECB_Config *config, const AESECB_Params *params);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_AESECB__include */
