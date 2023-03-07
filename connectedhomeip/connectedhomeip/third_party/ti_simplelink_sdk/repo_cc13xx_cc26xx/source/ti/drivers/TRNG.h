/*
 * Copyright (c) 2018-2021, Texas Instruments Incorporated
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
 *  @file       TRNG.h
 *
 *  @brief      TRNG driver header
 *
 *  @anchor ti_drivers_TRNG_Overview
 *  # Overview #
 *  The True Random Number Generator (TRNG) module generates random data of variable
 *  lengths from a source of entropy. The output is suitable for applications
 *  requiring cryptographically random data such as keying material for
 *  private or symmetric keys.
 *
 *  @anchor ti_drivers_TRNG_Usage
 *  # Usage #
 *
 *  ## Before starting a TRNG operation #
 *
 *  Before starting a TRNG operation, the application must do the following:
 *      - Call TRNG_init() to initialize the driver.
 *      - Call TRNG_Params_init() to initialize the TRNG_Params to default values.
 *      - Modify the TRNG_Params as desired.
 *      - Call TRNG_open() to open an instance of the driver.
 *      - Option 1: Use TRNG_generateKey() that writes random bytes to a CryptoKey. <br>
 *        Initialize a blank CryptoKey. These opaque data structures are representations
 *        of keying material and its storage. Depending on how the keying material
 *        is stored (RAM or flash, key store), the CryptoKey must be
 *        initialized differently. The TRNG API can handle all types of CryptoKey.
 *        However, not all device-specific implementations support all types of CryptoKey.
 *        Devices without a key store will not support CryptoKeys with keying material
 *        stored in a key store for example.
 *        All devices support plaintext CryptoKeys.
 *      - Option 2: Use TRNG_getRandomBytes() that writes random bytes to a buffer. <br>
 *        Allocate memory sufficient to hold the number of bytes of random data requested.
 *
 *  ## TRNG operations #
 *
 *  TRNG_generateKey() provides the most basic functionality. Use it to
 *  generate key-material of a specified size. An example use-case would be generating
 *  a symmetric key for AES encryption and / or authentication. If entropy data is needed
 *  for anything other than a key-material, use TRNG_getRandomBytes() that
 *  writes random bytes from the entropy source to a buffer/array.
 *
 *  To generate an ECC private key, you should use rejection sampling to ensure
 *  that the keying material is in the interval [1, n - 1]. The ECDH public key
 *  generation APIs will reject private keys that are outside of this interval.
 *  This information may be used to generate keying material until a suitable
 *  key is generated. For most curves, it is improbable to generate a random number
 *  outside of this interval because n is a large number close to the maximum
 *  number that would fit in the k-byte keying material array. An example
 *  of how to do this is given below.
 *
 *  ## After the TRNG operation completes #
 *
 *  After the TRNG operation completes, the application should either start another operation
 *  or close the driver by calling TRNG_close().
 *
 *  @anchor ti_drivers_TRNG_Synopsis
 *  ## Synopsis
 *  @anchor ti_drivers_TRNG_Synopsis_Code
 *  @code
 *  // Import TRNG Driver definitions
 *  #include <ti/drivers/TRNG.h>
 *  #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 *
 *  // Define name for TRNG channel index
 *  #define TRNG_INSTANCE 0
 *
 *  #define KEY_LENGTH_BYTES 16
 *
 *  TRNG_init();
 *
 *  handle = TRNG_open(TRNG_INSTANCE, NULL);
 *
 *  CryptoKeyPlaintext_initBlankKey(&entropyKey, entropyBuffer, KEY_LENGTH_BYTES);
 *
 *  result = TRNG_generateKey(handle, &entropyKey);
 *
 *  TRNG_close(handle);
 *
 *  @endcode
 *
 *  @anchor ti_drivers_TRNG_Examples
 *  ## Examples
 *
 *  ### Generate symmetric encryption key #
 *
 *  @code
 *
 *  #include <ti/drivers/TRNG.h>
 *  #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 *
 *  #define KEY_LENGTH_BYTES 16
 *
 *  TRNG_Handle handle;
 *  int_fast16_t result;
 *
 *  CryptoKey entropyKey;
 *  uint8_t entropyBuffer[KEY_LENGTH_BYTES] = {0};
 *
 *  handle = TRNG_open(0, NULL);
 *
 *  if (!handle) {
 *      // Handle error
 *      while(1);
 *  }
 *
 *  CryptoKeyPlaintext_initBlankKey(&entropyKey, entropyBuffer, KEY_LENGTH_BYTES);
 *
 *  result = TRNG_generateKey(handle, &entropyKey);
 *
 *  if (result != TRNG_STATUS_SUCCESS) {
 *      // Handle error
 *      while(1);
 *  }
 *
 *  TRNG_close(handle);
 *
 *  @endcode
 *
 *  ### Generate ECC private and public key using rejection sampling #
 *
 *  @code
 *
 *  #include <ti/drivers/TRNG.h>
 *  #include <ti/drivers/ECDH.h>
 *  #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 *  #include <ti/drivers/cryptoutils/ecc/ECCParams.h>
 *
 *  TRNG_Handle trngHandle;
 *  ECDH_Handle ecdhHandle;
 *
 *  CryptoKey privateKey;
 *  CryptoKey publicKey;
 *
 *  int_fast16_t trngResult;
 *  int_fast16_t ecdhResult;
 *
 *  uint8_t privateKeyingMaterial[32];
 *  uint8_t publicKeyingMaterial[64];
 *
 *  ECDH_OperationGeneratePublicKey genPubKeyOperation;
 *
 *  trngHandle = TRNG_open(0, NULL);
 *  if (!trngHandle) {
 *      while(1);
 *  }
 *
 *  ecdhHandle = ECDH_open(0, NULL);
 *  if (!ecdhHandle) {
 *      while(1);
 *  }
 *
 *  // Repeatedly generate random numbers until they are in the range [1, n - 1].
 *  // Since the NIST-P256 order is so close to 2^256, the probability of needing
 *  // to generate more than one random number is incredibly low but not non-zero.
 *  do {
 *
 *      CryptoKeyPlaintext_initBlankKey(&privateKey, privateKeyingMaterial, ECCParams_NISTP256.length);
 *      CryptoKeyPlaintext_initBlankKey(&publicKey, publicKeyingMaterial, 2 * ECCParams_NISTP256.length);
 *
 *      trngResult = TRNG_generateKey(trngHandle, &privateKey);
 *
 *      if (trngResult != TRNG_STATUS_SUCCESS) {
 *          while(1);
 *      }
 *
 *      ECDH_OperationGeneratePublicKey_init(&genPubKeyOperation);
 *      genPubKeyOperation.curve = &ECCParams_NISTP256;
 *      genPubKeyOperation.myPrivateKey = &privateKey;
 *      genPubKeyOperation.myPublicKey = &publicKey;
 *
 *      ecdhResult = ECDH_generatePublicKey(ecdhHandle, &genPubKeyOperation);
 *
 *  } while(ecdhResult == ECDH_STATUS_PRIVATE_KEY_LARGER_EQUAL_ORDER || ecdhResult == ECDH_STATUS_PRIVATE_KEY_ZERO);
 *
 *  TRNG_close(trngHandle);
 *  ECDH_close(ecdhHandle);
 *
 *  @endcode
 *
 *  ### Generate random bytes to a user provided buffer #
 *
 *  @code
 *
 *  #include <ti/drivers/TRNG.h>
 *
 *  #define RANDOM_BYTES_SIZE 16
 *
 *  TRNG_Handle handle;
 *  int_fast16_t result;
 *
 *  uint8_t randomBytesArray[RANDOM_BYTES_SIZE] = {0};
 *
 *  handle = TRNG_open(0, NULL);
 *
 *  if (!handle) {
 *      // Handle error
 *      while(1);
 *  }
 *
 *  result = TRNG_getRandomBytes(handle, randomBytesArray, RANDOM_BYTES_SIZE);
 *
 *  if (result != TRNG_STATUS_SUCCESS) {
 *      // Handle error
 *      while(1);
 *  }
 *
 *  TRNG_close(handle);
 *
 *  @endcode
 */

#ifndef ti_drivers_TRNG__include
#define ti_drivers_TRNG__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Common TRNG status code reservation offset.
 * TRNG driver implementations should offset status codes with
 * TRNG_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define TRNGXYZ_STATUS_ERROR0    TRNG_STATUS_RESERVED - 0
 * #define TRNGXYZ_STATUS_ERROR1    TRNG_STATUS_RESERVED - 1
 * #define TRNGXYZ_STATUS_ERROR2    TRNG_STATUS_RESERVED - 2
 * @endcode
 */
#define TRNG_STATUS_RESERVED        (-32)

/*!
 * @brief   Successful status code.
 *
 * Functions return TRNG_STATUS_SUCCESS if the function was executed
 * successfully.
 */
#define TRNG_STATUS_SUCCESS         (0)

/*!
 * @brief   Generic error status code.
 *
 * Functions return TRNG_STATUS_ERROR if the function was not executed
 * successfully.
 */
#define TRNG_STATUS_ERROR           (-1)

/*!
 * @brief   An error status code returned if the hardware or software resource
 * is currently unavailable.
 *
 * TRNG driver implementations may have hardware or software limitations on how
 * many clients can simultaneously perform operations. This status code is returned
 * if the mutual exclusion mechanism signals that an operation cannot currently be performed.
 */
#define TRNG_STATUS_RESOURCE_UNAVAILABLE (-2)

/*!
* @brief   Operation failed due to invalid inputs.
*
* Functions return TRNG_STATUS_INVALID_INPUTS if input validation fails.
*/
#define TRNG_STATUS_INVALID_INPUTS       (-3)

/*!
* @brief  The ongoing operation was canceled.
*/
#define TRNG_STATUS_CANCELED             (-4)

/*!
 *  @brief TRNG Global configuration
 *
 *  The TRNG_Config structure contains a set of pointers used to characterize
 *  the TRNG driver implementation.
 *
 *  This structure needs to be defined before calling TRNG_init() and it must
 *  not be changed thereafter.
 *
 *  @sa     TRNG_init()
 */
typedef struct {
    /*! Pointer to a driver specific data object */
    void               *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void         const *hwAttrs;
} TRNG_Config;

/*!
 *  @brief  A handle that is returned from a TRNG_open() call.
 */
typedef TRNG_Config  *TRNG_Handle;

/*!
 * @brief   The way in which TRNG function calls return after generating
 *          the requested entropy.
 *
 * Not all TRNG operations exhibit the specified return behavior. Functions that do not
 * require significant computation and cannot offload that computation to a background thread
 * behave like regular functions. Which functions exhibit the specified return behavior is not
 * implementation dependent. Specifically, a software-backed implementation run on the same
 * CPU as the application will emulate the return behavior while not actually offloading
 * the computation to the background thread.
 *
 * TRNG functions exhibiting the specified return behavior have restrictions on the
 * context from which they may be called.
 *
 * |                              | Task  | Hwi   | Swi   |
 * |------------------------------|-------|-------|-------|
 * |TRNG_RETURN_BEHAVIOR_CALLBACK | X     | X     | X     |
 * |TRNG_RETURN_BEHAVIOR_BLOCKING | X     |       |       |
 * |TRNG_RETURN_BEHAVIOR_POLLING  | X     | X     | X     |
 *
 */
typedef enum {
    TRNG_RETURN_BEHAVIOR_CALLBACK = 1,    /*!< The function call will return immediately while the
                                             *   TRNG operation goes on in the background. The registered
                                             *   callback function is called after the operation completes.
                                             *   The context the callback function is called (task, HWI, SWI)
                                             *   is implementation-dependent.
                                             */
    TRNG_RETURN_BEHAVIOR_BLOCKING = 2,    /*!< The function call will block while TRNG operation goes
                                             *   on in the background. TRNG operation results are available
                                             *   after the function returns.
                                             */
    TRNG_RETURN_BEHAVIOR_POLLING  = 4,    /*!< The function call will continuously poll a flag while TRNG
                                             *   operation goes on in the background. TRNG operation results
                                             *   are available after the function returns.
                                             */
} TRNG_ReturnBehavior;

/*!
 *  @brief  The definition of a callback function used by the TRNG driver
 *          when TRNG_generateKey() is called with ::TRNG_RETURN_BEHAVIOR_CALLBACK
 *
 *  @attention This will replace #TRNG_CallbackFxn, which is currently deprecated.
 *
 *  @param  handle  Handle of the client that started the TRNG operation.
 *
 *  @param  returnValue Return status code describing the outcome of the operation.
 *
 *  @param  entropy     The CryptoKey that describes the location the generated
 *                      entropy will be copied to.
 */
typedef void (*TRNG_CryptoKeyCallbackFxn) (TRNG_Handle handle,
                                           int_fast16_t returnValue,
                                           CryptoKey *entropy);

/*!
 *  @brief  The definition of a callback function used by the TRNG driver
 *          when TRNG_generateKey() is called with ::TRNG_RETURN_BEHAVIOR_CALLBACK
 *
 *  @param  handle  Handle of the client that started the TRNG operation.
 *
 *  @param  returnValue Return status code describing the outcome of the operation.
 *
 *  @param  randomBytes Pointer to an array that stores the random bytes
 *                      output by this function.
 *
 *  @param  randomBytesSize The size of the random data required.
 */
typedef void (*TRNG_RandomBytesCallbackFxn) (TRNG_Handle handle,
                                             int_fast16_t returnValue,
                                             uint8_t *randomBytes,
                                             size_t randomBytesSize);

/*!
 *  @brief  The definition of a callback function used by the TRNG driver
 *          when used in ::TRNG_RETURN_BEHAVIOR_CALLBACK
 *
 *  @deprecated #TRNG_CallbackFxn will be replaced by #TRNG_CryptoKeyCallbackFxn
 */
typedef TRNG_CryptoKeyCallbackFxn TRNG_CallbackFxn;


/*!
 *  @brief  TRNG Parameters
 *
 *  TRNG Parameters are used to with the TRNG_open() call. Default values for
 *  these parameters are set using TRNG_Params_init().
 *
 *  @attention When using the driver in #TRNG_RETURN_BEHAVIOR_CALLBACK,
 *             set the appropriate callback function field to point to a
 *             valid callback function and set the other one to NULL.
 *
 *  @sa     TRNG_Params_init()
 */
typedef struct {
    TRNG_ReturnBehavior         returnBehavior;         /*!< Blocking, callback, or polling return behavior */
    TRNG_CryptoKeyCallbackFxn   cryptoKeyCallbackFxn;   /*!< Callback function to use with TRNG_generateKey().
                                                         *  Set randomBytesCallbackFxn to NULL if using this.
                                                         */
    TRNG_RandomBytesCallbackFxn randomBytesCallbackFxn; /*!< Callback function to use with TRNG_getRandomBytes()
                                                         *  Set cryptoKeyCallbackFxn to NULL if using this.
                                                         */
    uint32_t                    timeout;                /*!< Timeout before the driver returns an error in
                                                         *   ::TRNG_RETURN_BEHAVIOR_BLOCKING
                                                         */
    void                        *custom;                /*!< Custom argument used by driver implementation */
} TRNG_Params;

/*!
 *  @brief Default TRNG_Params structure
 *
 *  @sa     TRNG_Params_init()
 */
extern const TRNG_Params TRNG_defaultParams;

/*!
 *  @brief  This function initializes the TRNG module.
 *
 *  @pre    The TRNG_config structure must exist and be persistent before this
 *          function can be called. This function must also be called before
 *          any other TRNG driver APIs. This function call does not modify any
 *          peripheral registers.
 */
void TRNG_init(void);

/*!
 *  @brief  Function to initialize the TRNG_Params struct to its defaults
 *
 *  @param  params      An pointer to TRNG_Params structure for
 *                      initialization
 *
 *  Default values are:    <br>
 *      returnBehavior              = TRNG_RETURN_BEHAVIOR_BLOCKING <br>
 *      callbackFxn                 = NULL                          <br>
 *      timeout                     = SemaphoreP_WAIT_FOREVER       <br>
 *      custom                      = NULL                          <br>
 */
void TRNG_Params_init(TRNG_Params *params);

/*!
 *  @brief  This function opens a given TRNG peripheral.
 *
 *  @pre    TRNG controller has been initialized using TRNG_init()
 *
 *  @param  index         Logical peripheral number for the TRNG indexed into
 *                        the TRNG_config table
 *
 *  @param  params        Pointer to an parameter block, if NULL it will use
 *                        default values.
 *
 *  @return A TRNG_Handle on success or a NULL on an error or if it has been
 *          opened already.
 *
 *  @sa     TRNG_init()
 *  @sa     TRNG_close()
 */
TRNG_Handle TRNG_open(uint_least8_t index, TRNG_Params *params);

/*!
 *  @brief  Function to close a TRNG peripheral specified by the TRNG handle
 *
 *  @pre    TRNG_open() has to be called first.
 *
 *  @param  handle A TRNG handle returned from TRNG_open()
 *
 *  @sa     TRNG_open()
 */
void TRNG_close(TRNG_Handle handle);

/*!
 *  @brief  Generate random bytes and output to the given \c CryptoKey object.
 *
 *  Generates a random bitstream of the size defined in the \c entropy
 *  CryptoKey in the range 0 <= \c entropy buffer < 2 ^ (entropy length * 8).
 *  The entropy will be generated and stored according to the storage requirements
 *  defined in the CryptoKey.
 *
 *  @deprecated This function has been replaced by a pair of new functions.
 *              See #TRNG_generateKey() and #TRNG_getRandomBytes().
 *
 *  @pre    TRNG_open() has to be called first.
 *
 *  @param  handle A TRNG handle returned from TRNG_open().
 *
 *  @param  entropy Pointer to a \c CryptoKey object that should already be initialized
 *                  to hold a plaintext key, provided with the length and the address
 *                  of the plaintext key-material where the generated entropy will be populated.
 *
 *  @retval #TRNG_STATUS_SUCCESS               The operation succeeded.
 *  @retval #TRNG_STATUS_ERROR                 The operation failed.
 *  @retval #TRNG_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource was not available. Try again later.
 *  @retval #TRNG_STATUS_INVALID_INPUTS        Inputs provided are not valid.
 */
int_fast16_t TRNG_generateEntropy(TRNG_Handle handle, CryptoKey *entropy);

/*!
 *  @brief  Generate random bytes and output to the given \c CryptoKey object.
 *
 *  Generates a random bitstream of the size defined in the \c entropy
 *  CryptoKey in the range 0 <= \c entropy buffer < 2 ^ (entropy length * 8).
 *  The entropy will be generated and stored according to the storage requirements
 *  defined in the CryptoKey.
 *
 *  @note This function replaces #TRNG_generateEntropy().
 *        See #TRNG_getRandomBytes() to output random bytes to an array instead.
 *
 *  @attention When called with ::TRNG_RETURN_BEHAVIOR_CALLBACK, provide a callback
 *              function of type #TRNG_CryptoKeyCallbackFxn.
 *
 *  @pre    TRNG_open() has to be called first.
 *
 *  @param  handle A TRNG handle returned from TRNG_open().
 *
 *  @param  entropy Pointer to a \c CryptoKey object that should already be initialized
 *                  to hold a plaintext key, provided with the length and the address
 *                  of the plaintext key-material where the generated entropy will be populated.
 *
 *  @retval #TRNG_STATUS_SUCCESS               The operation succeeded.
 *  @retval #TRNG_STATUS_ERROR                 The operation failed.
 *  @retval #TRNG_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource was not available. Try again later.
 *  @retval #TRNG_STATUS_INVALID_INPUTS        Inputs provided are not valid.
*/
int_fast16_t TRNG_generateKey(TRNG_Handle handle, CryptoKey *entropy);

/*!
 *  @brief  Generate random bytes and output to the given array.
 *
 *  Generates random bytes of size given by \c randomBytesSize and stores it
 *  in the array pointed at by \c randomBytes. The user shall be responsible for allocating
 *  \c randomBytesSize long memory starting at the address pointed at by \c randomBytes.
 *
 *  @attention When called with ::TRNG_RETURN_BEHAVIOR_CALLBACK, provide a callback
 *              function of type #TRNG_RandomBytesCallbackFxn.
 *
 *  @note See #TRNG_generateKey() to output random bytes to a \c CryptoKey instead.
 *
 *  @pre    TRNG_open() has to be called first.
 *
 *  @param  handle A TRNG handle returned from TRNG_open().
 *
 *  @param  randomBytes Pointer to an array that stores the random bytes
 *                      output by this function.
 *
 *  @param  randomBytesSize The size of the random data required.
 *
 *  @retval #TRNG_STATUS_SUCCESS               The operation succeeded.
 *  @retval #TRNG_STATUS_ERROR                 The operation failed.
 *  @retval #TRNG_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource was not available. Try again later.
 *  @retval #TRNG_STATUS_INVALID_INPUTS        Inputs provided are not valid.
 */
int_fast16_t TRNG_getRandomBytes(TRNG_Handle handle, void *randomBytes, size_t randomBytesSize);

/**
 *  @brief  Constructs a new TRNG object
 *
 *  Unlike #TRNG_open(), #TRNG_construct() does not require the hwAttrs and
 *  object to be allocated in a #TRNG_Config array that is indexed into.
 *  Instead, the #TRNG_Config, hwAttrs, and object can be allocated at any
 *  location. This allows for relatively simple run-time allocation of temporary
 *  driver instances on the stack or the heap.
 *  The drawback is that this makes it more difficult to write device-agnostic
 *  code. If you use an ifdef with DeviceFamily, you can choose the correct
 *  object and hwAttrs to allocate. That compilation unit will be tied to the
 *  device it was compiled for at this point. To change devices, recompilation
 *  of the application with a different DeviceFamily setting is necessary.
 *
 *  @param  config #TRNG_Config describing the location of the object and hwAttrs.
 *
 *  @param  params #TRNG_Params to configure the driver instance.
 *
 *  @return        Returns a #TRNG_Handle on success or NULL on failure.
 *
 *  @pre    The object struct @c config points to must be zeroed out prior to
 *          calling this function. Otherwise, unexpected behavior may ensue.
 */
TRNG_Handle TRNG_construct(TRNG_Config *config, const TRNG_Params *params);

/*!
 *  @brief Aborts an ongoing TRNG operation and clears internal buffers.
 *
 *  Aborts an operation to generate random bytes/entropy. The operation will
 *  terminate as though an error occurred and the status code of the operation will be
 *  #TRNG_STATUS_CANCELED in this case.
 *
 *  @param  handle      A #TRNG_Handle returned from #TRNG_open()
 *
 *  @retval #TRNG_STATUS_SUCCESS    The operation was canceled or there was no
 *                                  operation in progress to be canceled.
 */
int_fast16_t TRNG_cancelOperation(TRNG_Handle handle);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_TRNG__include */
