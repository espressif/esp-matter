/*
 * Copyright (c) 2017-2020, Texas Instruments Incorporated
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
 *  @file       SHA2.h
 *
 *  @brief      SHA2 driver header
 *
 *  @anchor ti_drivers_SHA2_Overview
 *  # Overview #
 *
 *  SHA2 (Secure Hash Algorithm 2) is a cryptographic hashing algorithm that
 *  maps an input of arbitrary length to a fixed-length output with negligible
 *  probability of collision. A collision would occur when two different inputs
 *  map to the same output.
 *
 *  It is not currently technologically feasible to derive an input from
 *  the hash digest (output) itself.
 *
 *  Hashes are often used to ensure the integrity of messages. They are also
 *  used to as constituent parts of more complicated cryptographic schemes.
 *  HMAC is a message authentication code that is based on hash functions such
 *  as SHA2 rather than a block cipher.
 *  Hashes may themselves be used as or form a part of key derivation functions
 *  used to derive symmetric keys from sources of entropy such as an Elliptic
 *  Curve Diffie-Helman key exchange (ECDH).
 *
 *  SHA2 is not actually a single algorithm, but a suite of similar algorithms
 *  that produce hash digests of different lengths. 224, 256, 384, and 512-bit
 *  outputs are available.
 *
 *  "Hash" may refer to either the process of hashing when used as a verb and
 *  the output digest when used as a noun.
 *
 *  @anchor ti_drivers_SHA2_Usage
 *  # Usage #
 *
 *  Before starting a SHA2 operation, the application must do the following:
 *      - Call #SHA2_init() to initialize the driver
 *      - Call #SHA2_Params_init() to initialize the SHA2_Params to default values.
 *      - Modify the #SHA2_Params as desired
 *      - Call #SHA2_open() to open an instance of the driver
 *
 *  There are two general ways to execute a SHA2 operation:
 *
 *  - one-step (in one operation)
 *  - multi-step (multiple partial operations)

 *  @anchor ti_drivers_SHA2_Synopsis
 *  # Synopsis
 *
 *  @anchor ti_drivers_SHA2_Synopsis_Code
 *  @code
 *
 *  // Import SHA2 Driver definitions
 *  #include <ti/drivers/SHA2.h>
 *
 *  // Define name for SHA2 channel index
 *  #define SHA2_INSTANCE 0
 *
 *  SHA2_init();
 *
 *  handle = SHA2_open(SHA2_INSTANCE, NULL);
 *
 *  result = SHA2_hashData(handle, message, strlen(message), actualDigest);
 *
 *  SHA2_close(handle);
 *  @endcode
 *
 *  @anchor ti_drivers_SHA2_Examples
 *  # Examples #
 *
 *  ## One-step hash operation #
 *
 *  The #SHA2_hashData() function can perform a SHA2 operation in a single call.
 *  It will always use the most highly optimized routine with the least overhead and
 *  the fastest runtime. However, it requires that the entire input message is
 *  available to the function in a contiguous location at the start of the call.
 *  The single call operation is required when hashing a message with a length smaller
 *  than or equal to one hash-block length. All devices support single call operations.
 *
 *  After a SHA2 operation completes, the application may either start
 *  another operation or close the driver by calling #SHA2_close().
 *
 *  @code
 *  SHA2_Params params;
 *  SHA2_Handle handle;
 *  int_fast16_t result;
 *
 *  char message[] = "A Ferengi without profit is no Ferengi at all.";
 *
 *  uint8_t actualDigest[SHA2_DIGEST_LENGTH_BYTES_256];
 *  uint8_t expectedDigest[] = {
 *      0x93, 0xD6, 0x5C, 0x07,
 *      0xA6, 0x26, 0x88, 0x9C,
 *      0x87, 0xCC, 0x82, 0x24,
 *      0x47, 0xC6, 0xE4, 0x28,
 *      0xC0, 0xBD, 0xC6, 0xED,
 *      0xAA, 0x8C, 0xD2, 0x53,
 *      0x77, 0xAA, 0x73, 0x14,
 *      0xA3, 0xE2, 0xDE, 0x43
 *  };
 *
 *  SHA2_init();
 *
 *  SHA2_Params_init(&params);
 *  params.returnBehavior = SHA2_RETURN_BEHAVIOR_BLOCKING;
 *  handle = SHA2_open(0, &params);
 *  assert(handle != NULL);
 *
 *  result = SHA2_hashData(handle, message, strlen(message), actualDigest);
 *  assert(result == SHA2_STATUS_SUCCESS);
 *
 *  result = memcmp(actualDigest, expectedDigest, SHA2_DIGEST_LENGTH_BYTES_256);
 *  assert(result == 0);
 *
 *  SHA2_close(handle);
 *  @endcode
 *
 *  ## Partial hash operation #
 *
 *  When trying to operate on data that is too large to fit into available memory,
 *  partial processing is more advisable. The segments are processed with
 *  #SHA2_addData() whereas the final digest is computed by #SHA2_finalize().
 *
 *  @code
 *  SHA2_Handle handle;
 *  int_fast16_t result;
 *  SHA2_Params params;
 *
 *  const char message[] =
 *      "Premature optimization is the root of all evil (or at least most of it) in programming.";
 *
 *  uint8_t actualDigest[SHA2_DIGEST_LENGTH_BYTES_256];
 *  uint8_t expectedDigest[] = {
 *      0xF2, 0x6A, 0xFF, 0x01,
 *      0x11, 0x6B, 0xF6, 0x77,
 *      0x63, 0x91, 0xFE, 0xD9,
 *      0x47, 0x56, 0x99, 0xB2,
 *      0xAD, 0x7D, 0x64, 0x16,
 *      0xF7, 0x40, 0x1A, 0x5B,
 *      0xCC, 0xC7, 0x08, 0x3D,
 *      0xE8, 0x6B, 0x35, 0x6D,
 *  };
 *
 *  SHA2_init();
 *
 *  SHA2_Params_init(&params);
 *  params.returnBehavior = SHA2_RETURN_BEHAVIOR_BLOCKING;
 *  handle = SHA2_open(0, &params);
 *  assert(handle != NULL);
 *
 *  // We can configure the driver even after SHA2_open()
 *  result = SHA2_setHashType(handle, SHA2_HASH_TYPE_256);
 *  assert(result == SHA2_STATUS_SUCCESS);
 *
 *  // Process data in chunks. The driver buffers incomplete blocks internally.
 *  result = SHA2_addData(handle, &message[0], 17);
 *  assert(result == SHA2_STATUS_SUCCESS);
 *
 *  result = SHA2_addData(handle, &message[17], strlen(message) - 17);
 *  assert(result == SHA2_STATUS_SUCCESS);
 *
 *  // Compute the resulting digest
 *  result = SHA2_finalize(handle, actualDigest);
 *  assert(result == SHA2_STATUS_SUCCESS);
 *
 *  // Verify
 *  result = memcmp(actualDigest, expectedDigest, SHA2_DIGEST_LENGTH_BYTES_256);
 *  assert(result == 0);
 *
 *  SHA2_close(handle);
 *  @endcode
 *
 *  ## One-step HMAC operation #
 *
 *  The #SHA2_hmac() function can perform a SHA2 operation in a single call.
 *  It will always use the most highly optimized routine with the least overhead
 *  and the fastest runtime. It requires that the entire input message is
 *  available to the function in a contiguous location at the start of the call.
 *
 *  After a SHA2 operation completes, the application may either start
 *  another operation or close the driver by calling #SHA2_close().
 *
 *  @code
 *  SHA2_Params params;
 *  SHA2_Handle handle;
 *  int_fast16_t result;
 *  CryptoKey hmacKey;
 *
 *  uint8_t message[] = {
 *          0xb1, 0x68, 0x9c, 0x25, 0x91, 0xea, 0xf3, 0xc9,
 *          0xe6, 0x60, 0x70, 0xf8, 0xa7, 0x79, 0x54, 0xff,
 *          0xb8, 0x17, 0x49, 0xf1, 0xb0, 0x03, 0x46, 0xf9,
 *          0xdf, 0xe0, 0xb2, 0xee, 0x90, 0x5d, 0xcc, 0x28,
 *          0x8b, 0xaf, 0x4a, 0x92, 0xde, 0x3f, 0x40, 0x01,
 *          0xdd, 0x9f, 0x44, 0xc4, 0x68, 0xc3, 0xd0, 0x7d,
 *          0x6c, 0x6e, 0xe8, 0x2f, 0xac, 0xea, 0xfc, 0x97,
 *          0xc2, 0xfc, 0x0f, 0xc0, 0x60, 0x17, 0x19, 0xd2,
 *          0xdc, 0xd0, 0xaa, 0x2a, 0xec, 0x92, 0xd1, 0xb0,
 *          0xae, 0x93, 0x3c, 0x65, 0xeb, 0x06, 0xa0, 0x3c,
 *          0x9c, 0x93, 0x5c, 0x2b, 0xad, 0x04, 0x59, 0x81,
 *          0x02, 0x41, 0x34, 0x7a, 0xb8, 0x7e, 0x9f, 0x11,
 *          0xad, 0xb3, 0x04, 0x15, 0x42, 0x4c, 0x6c, 0x7f,
 *          0x5f, 0x22, 0xa0, 0x03, 0xb8, 0xab, 0x8d, 0xe5,
 *          0x4f, 0x6d, 0xed, 0x0e, 0x3a, 0xb9, 0x24, 0x5f,
 *          0xa7, 0x95, 0x68, 0x45, 0x1d, 0xfa, 0x25, 0x8e};
 *
 *  // In this case, keyingMaterial is 40 bytes long. It could also be
 *  // any other length.
 *  uint8_t keyingMaterial[] = {
 *          0x97, 0x79, 0xd9, 0x12, 0x06, 0x42, 0x79, 0x7f,
 *          0x17, 0x47, 0x02, 0x5d, 0x5b, 0x22, 0xb7, 0xac,
 *          0x60, 0x7c, 0xab, 0x08, 0xe1, 0x75, 0x8f, 0x2f,
 *          0x3a, 0x46, 0xc8, 0xbe, 0x1e, 0x25, 0xc5, 0x3b,
 *          0x8c, 0x6a, 0x8f, 0x58, 0xff, 0xef, 0xa1, 0x76};
 *
 *
 *
 *  uint8_t actualHmac[SHA2_DIGEST_LENGTH_BYTES_256];
 *  uint8_t expectedHmac[] = {
 *      0x76, 0x9f, 0x00, 0xd3, 0xe6, 0xa6, 0xcc, 0x1f,
 *      0xb4, 0x26, 0xa1, 0x4a, 0x4f, 0x76, 0xc6, 0x46,
 *      0x2e, 0x61, 0x49, 0x72, 0x6e, 0x0d, 0xee, 0x0e,
 *      0xc0, 0xcf, 0x97, 0xa1, 0x66, 0x05, 0xac, 0x8b
 *  };
 *
 *  SHA2_init();
 *
 *  SHA2_Params_init(&params);
 *  params.returnBehavior = SHA2_RETURN_BEHAVIOR_BLOCKING;
 *  handle = SHA2_open(0, &params);
 *  assert(handle != NULL);
 *
 *  CryptoKeyPlaintext_initKey(&hmacKey,
 *                             keyingMaterial,
 *                             sizeof(keyingMaterial));
 *
 *  result = SHA2_hmac(handle,
 *                     &hmacKey,
 *                     message,
 *                     sizeof(message),
 *                     actualHmac);
 *  assert(result == SHA2_STATUS_SUCCESS);
 *
 *  result = memcmp(actualHmac, expectedHmac, SHA2_DIGEST_LENGTH_BYTES_256);
 *  assert(result == 0);
 *
 *  SHA2_close(handle);
 *  @endcode
 */

#ifndef ti_drivers_SHA2__include
#define ti_drivers_SHA2__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Common SHA2 status code reservation offset.
 * SHA2 driver implementations should offset status codes with
 * SHA2_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define SHA2XYZ_STATUS_ERROR0    SHA2_STATUS_RESERVED - 0
 * #define SHA2XYZ_STATUS_ERROR1    SHA2_STATUS_RESERVED - 1
 * #define SHA2XYZ_STATUS_ERROR2    SHA2_STATUS_RESERVED - 2
 * @endcode
 */
#define SHA2_STATUS_RESERVED        (-32)

/*!
 * @brief   Successful status code.
 *
 * Functions return SHA2_STATUS_SUCCESS if the function was executed
 * successfully.
 */
#define SHA2_STATUS_SUCCESS         (0)

/*!
 * @brief   Generic error status code.
 *
 * Functions return SHA2_STATUS_ERROR if the function was not executed
 * successfully and no more specific error is applicable.
 */
#define SHA2_STATUS_ERROR           (-1)

/*!
 * @brief   An error status code returned if the hardware or software resource
 * is currently unavailable.
 *
 * SHA2 driver implementations may have hardware or software limitations on how
 * many clients can simultaneously perform operations. This status code is returned
 * if the mutual exclusion mechanism signals that an operation cannot currently be performed.
 */
#define SHA2_STATUS_RESOURCE_UNAVAILABLE (-2)

/*!
 *  @brief  The ongoing operation was canceled.
 */
#define SHA2_STATUS_CANCELED (-3)

/*!
 * @brief   The way in which SHA2 function calls return after performing an
 * operation.
 *
 * Not all SHA2 operations exhibit the specified return behavior. Functions that do not
 * require significant computation and cannot offload that computation to a background thread
 * behave like regular functions. Which functions exhibit the specified return behavior is not
 * implementation dependent. Specifically, a software-backed implementation run on the same
 * CPU as the application will emulate the return behavior while not actually offloading
 * the computation to the background thread.
 *
 * SHA2 functions exhibiting the specified return behavior have restrictions on the
 * context from which they may be called.
 *
 * |                                | Task  | Hwi   | Swi   |
 * |--------------------------------|-------|-------|-------|
 * |SHA2_RETURN_BEHAVIOR_CALLBACK   | X     | X     | X     |
 * |SHA2_RETURN_BEHAVIOR_BLOCKING   | X     |       |       |
 * |SHA2_RETURN_BEHAVIOR_POLLING    | X     | X     | X     |
 *
 */
typedef enum {
    SHA2_RETURN_BEHAVIOR_CALLBACK = 1,      /*!< The function call will return immediately while the
                                             *   SHA2 operation goes on in the background. The registered
                                             *   callback function is called after the operation completes.
                                             *   The context the callback function is called (task, HWI, SWI)
                                             *   is implementation-dependent.
                                             */
    SHA2_RETURN_BEHAVIOR_BLOCKING = 2,      /*!< The function call will block while the SHA2 operation goes
                                             *   on in the background. SHA2 operation results are available
                                             *   after the function returns.
                                             */
    SHA2_RETURN_BEHAVIOR_POLLING  = 4,      /*!< The function call will continuously poll a flag while the SHA2
                                             *   operation goes on in the background. SHA2 operation results
                                             *   are available after the function returns.
                                             */
} SHA2_ReturnBehavior;


/*!
 *  @brief  Enum for the hash types supported by the driver.
 */
typedef enum {
    SHA2_HASH_TYPE_224 = 0,
    SHA2_HASH_TYPE_256 = 1,
    SHA2_HASH_TYPE_384 = 2,
    SHA2_HASH_TYPE_512 = 3,
} SHA2_HashType;

/*!
 *  @brief  Enum for the hash digest lengths in bytes supported by the driver.
 */
typedef enum {
    SHA2_DIGEST_LENGTH_BYTES_224 = 28,
    SHA2_DIGEST_LENGTH_BYTES_256 = 32,
    SHA2_DIGEST_LENGTH_BYTES_384 = 48,
    SHA2_DIGEST_LENGTH_BYTES_512 = 64,
} SHA2_DigestLengthBytes;

/*!
 *  @brief  Enum for the block sizes of the algorithms.
 *
 *  SHA2 iteratively consumes segments of the block
 *  size and computes intermediate digests which are
 *  fed back into the algorithm together with the next
 *  segment to compute the next intermediate or final
 *  digest.
 *  The block sizes of the algorithms differ from their
 *  digest lengths. When performing partial hashes,
 *  the segment lengths for all but the last segment
 *  must be multiples of the relevant block size.
 */
typedef enum {
    SHA2_BLOCK_SIZE_BYTES_224 = 64,
    SHA2_BLOCK_SIZE_BYTES_256 = 64,
    SHA2_BLOCK_SIZE_BYTES_384 = 128,
    SHA2_BLOCK_SIZE_BYTES_512 = 128,
} SHA2_BlockSizeBytes;

/*!
 *  @brief SHA2 Global configuration
 *
 *  The %SHA2_Config structure contains a set of pointers used to characterize
 *  the SHA2 driver implementation.
 *
 *  This structure needs to be defined before calling #SHA2_init() and it must
 *  not be changed thereafter.
 *
 *  @sa     SHA2_init()
 */
typedef struct {
    /*! Pointer to a driver specific data object */
    void               *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void         const *hwAttrs;
} SHA2_Config;

/*!
 *  @brief  A handle that is returned from an SHA2_open() call.
 */
typedef SHA2_Config* SHA2_Handle;

/*!
 *  @brief  The definition of a callback function used by the SHA2 driver
 *          when used in ::SHA2_RETURN_BEHAVIOR_CALLBACK
 *
 *  @param  handle Handle of the client that started the SHA2 operation.
 *
 *  @param  returnStatus The result of the SHA2 operation. May contain an error code.
 *                       Informs the application of why the callback function was
 *                       called.
 */
typedef void (*SHA2_CallbackFxn) (SHA2_Handle handle, int_fast16_t returnStatus);

/*!
 *  @brief  SHA2 Parameters
 *
 *  SHA2 Parameters are used to with the SHA2_open() call. Default values for
 *  these parameters are set using SHA2_Params_init().
 *
 *  @sa     SHA2_Params_init()
 */
typedef struct {
    SHA2_HashType           hashType;                   /*!< SHA2 variant to use. This determines the output digest
                                                         *   length.
                                                         */
    SHA2_ReturnBehavior     returnBehavior;             /*!< Blocking, callback, or polling return behavior */
    SHA2_CallbackFxn        callbackFxn;                /*!< Callback function pointer */
    uint32_t                timeout;                    /*!< Timeout before the driver returns an error in
                                                         *   ::SHA2_RETURN_BEHAVIOR_BLOCKING
                                                         */
} SHA2_Params;

/*!
 * @brief Global SHA2 configuration struct.
 *
 * Specifies context objects and hardware attributes for every
 * driver instance.
 *
 * This variable is supposed to be defined in the board file.
 */
extern const SHA2_Config SHA2_config[];

/*!
 * @brief Global SHA2 configuration count.
 *
 * Specifies the amount of available SHA2 driver instances.
 *
 * This variable is supposed to be defined in the board file.
 */
extern const uint_least8_t SHA2_count;

/*!
 *  @brief  Default SHA2_Params structure
 *
 *  @sa     #SHA2_Params_init()
 */
extern const SHA2_Params SHA2_defaultParams;

/*!
 *  @brief  Initializes the SHA2 driver module.
 *
 *  @pre    The #SHA2_config structure must exist and be persistent before this
 *          function can be called. This function must also be called before
 *          any other SHA2 driver APIs. This function call does not modify any
 *          peripheral registers.
 */
void SHA2_init(void);

/*!
 *  @brief  Initializes \a params with default values.
 *
 *  @param  params      A pointer to #SHA2_Params structure for
 *                      initialization
 *
 *  Defaults values are:
 *      returnBehavior              = SHA2_RETURN_BEHAVIOR_BLOCKING
 *      callbackFxn                 = NULL
 *      timeout                     = SemaphoreP_WAIT_FOREVER
 *      custom                      = NULL
 */
void SHA2_Params_init(SHA2_Params *params);

/*!
 *  @brief  Initializes a SHA2 driver instance and returns a handle.
 *
 *  @pre    SHA2 controller has been initialized using #SHA2_init()
 *
 *  @param  index         Logical peripheral number for the SHA2 indexed into
 *                        the #SHA2_config table
 *
 *  @param  params        Pointer to a parameter block, if NULL it will use
 *                        default values.
 *
 *  @return A #SHA2_Handle on success or a NULL on an error or if it has been
 *          opened already.
 *
 *  @sa     #SHA2_init(), #SHA2_close()
 */
SHA2_Handle SHA2_open(uint_least8_t index, const SHA2_Params *params);

/*!
 *  @brief  Closes a SHA2 peripheral specified by \a handle.
 *
 *  @pre    #SHA2_open() has to be called first.
 *
 *  @param  handle A #SHA2_Handle returned from SHA2_open()
 *
 *  @sa     #SHA2_open()
 */
void SHA2_close(SHA2_Handle handle);

/*!
 *  @brief  Starts an HMAC operation on segmented data
 *
 *  This function uses @c key to compute the all intermediate results involving
 *  @c key as specified in FIPS 198-1.
 *
 *  This function blocks until the final digest hash been computed.
 *  It returns immediately when ::SHA2_RETURN_BEHAVIOR_CALLBACK is set.
 *
 *  @pre    #SHA2_open() has to be called first.
 *
 *  @post   Call #SHA2_addData() and #SHA2_finalizeHmac()
 *
 *  @param  handle  A #SHA2_Handle returned from #SHA2_open()
 *
 *  @param  key     The key with which to sign the message with
 *
 *  @retval #SHA2_STATUS_SUCCESS               The hash operation succeeded.
 *  @retval #SHA2_STATUS_ERROR                 The hash operation failed.
 *  @retval #SHA2_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource
 *                                             was not available. Try again
 *                                             later.
 *  @retval #SHA2_STATUS_CANCELED              The hash operation was canceled.
 *
 *  @sa     #SHA2_reset()
 */
int_fast16_t SHA2_setupHmac(SHA2_Handle handle, CryptoKey *key);

/*!
 *  @brief  Adds a segment of @c data with a @c length in bytes to the
 *          cryptographic hash or HMAC.
 *
 *  %SHA2_addData() may be called arbitrary times before finishing the
 *  operation with #SHA2_finalize().
 *
 *  This function blocks until the final digest hash been computed.
 *  It returns immediately when ::SHA2_RETURN_BEHAVIOR_CALLBACK is set.
 *
 *
 *  @pre    #SHA2_open() has to be called first.
 *  @pre    If computing an HMAC, #SHA2_setupHmac() must be called first.
 *
 *  @param  handle   A #SHA2_Handle returned from #SHA2_open()
 *
 *  @param  data     Pointer to the location to read from.
 *                   There might be alignment restrictions on different
 *                   platforms.
 *
 *  @param  length   Length of the message segment to hash, in bytes.
 *
 *  @retval #SHA2_STATUS_SUCCESS               The hash operation succeeded.
 *  @retval #SHA2_STATUS_ERROR                 The hash operation failed.
 *  @retval #SHA2_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource
 *                                             was not available. Try again
 *                                             later.
 *  @retval #SHA2_STATUS_CANCELED              The hash operation was canceled.
 *
 *  @sa     #SHA2_open(), #SHA2_reset(), #SHA2_finalize()
 */
int_fast16_t SHA2_addData(SHA2_Handle handle, const void* data, size_t length);

/*!
 *  @brief  Finishes a hash operation and writes the result to \a digest.
 *
 *  This function finishes a hash operation that has been previously started
 *  by #SHA2_addData().
 *
 *  This function blocks until the final digest hash been computed.
 *  It returns immediately when ::SHA2_RETURN_BEHAVIOR_CALLBACK is set.
 *
 *  @pre    #SHA2_addData() has to be called first.
 *
 *  @param  handle      A #SHA2_Handle returned from #SHA2_open()
 *
 *  @param  digest      Pointer to the location to write the digest to.
 *
 *  @retval #SHA2_STATUS_SUCCESS               The hash operation succeeded.
 *  @retval #SHA2_STATUS_ERROR                 The hash operation failed.
 *  @retval #SHA2_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource
 *                                             was not available. Try again
 *                                             later.
 *  @retval #SHA2_STATUS_CANCELED              The hash operation was canceled.
 *
 *  @sa     #SHA2_open(), #SHA2_addData()
 */
int_fast16_t SHA2_finalize(SHA2_Handle handle, void *digest);

/*!
 *  @brief  Finishes an HMAC operation and writes the result to @c hmac.
 *
 *  This function finishes a an HMAC operation that has been previously started
 *  by #SHA2_setupHmac() and #SHA2_addData().
 *
 *  This function blocks until the final digest hash been computed.
 *  It returns immediately when ::SHA2_RETURN_BEHAVIOR_CALLBACK is set.
 *
 *  @pre    #SHA2_setupHmac() must be called prior.
 *  @pre    #SHA2_addData() should be called after #SHA2_setupHmac().
 *
 *  @param  handle      A #SHA2_Handle returned from #SHA2_open()
 *
 *  @param  hmac        Pointer to the location to write the digest to.
 *
 *  @retval #SHA2_STATUS_SUCCESS               The hash operation succeeded.
 *  @retval #SHA2_STATUS_ERROR                 The hash operation failed.
 *  @retval #SHA2_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource
 *                                             was not available. Try again
 *                                             later.
 *  @retval #SHA2_STATUS_CANCELED              The hash operation was canceled.
 *
 *  @sa     #SHA2_open(), #SHA2_setupHmac() #SHA2_addData()
 */
int_fast16_t SHA2_finalizeHmac(SHA2_Handle handle, void *hmac);

/*!
 *  @brief  Hashes a segment of \a data with a \a size in bytes and writes the
 *          resulting hash to \a digest.
 *
 *  The digest content is computed in one step. Intermediate data from a
 *  previous partial operation started with #SHA2_addData() is discarded.
 *
 *  This function blocks until the final digest hash been computed.
 *  It returns immediately when ::SHA2_RETURN_BEHAVIOR_CALLBACK is set.
 *
 *  @pre    #SHA2_open() has to be called first.
 *
 *  @param  handle   A #SHA2_Handle returned from #SHA2_open()
 *
 *  @param  data     Pointer to the location to read from.
 *                   There might be alignment restrictions on different
 *                   platforms.
 *
 *  @param  dataLength Length of the message @c data, in bytes.
 *
 *  @param  digest   Pointer to the location to write the digest to.
 *                   There might be alignment restrictions on different
 *                   platforms.
 *
 *  @retval #SHA2_STATUS_SUCCESS               The hash operation succeeded.
 *  @retval #SHA2_STATUS_ERROR                 The hash operation failed.
 *  @retval #SHA2_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource
 *                                             was not available. Try again
 *                                             later.
 *  @retval #SHA2_STATUS_CANCELED              The hash operation was canceled.
 *
 *  @sa     #SHA2_open()
 */
int_fast16_t SHA2_hashData(SHA2_Handle handle,
                           const void* data,
                           size_t dataLength,
                           void *digest);

/*!
 *  @brief  Creates a keyed hash of @c data with @c key.
 *
 *  This function signs @c data using @c key using the keyed-hash message
 *  authentication code (HMAC) algorithm specified in FIPS 198-1.
 *
 *  This function expects all of @c data to be available in contiguous memory.
 *
 *  Intermediate data from a previous
 *  partial operation started with #SHA2_addData() is discarded.
 *
 *  This function blocks until the final digest hash been computed.
 *  It returns immediately when ::SHA2_RETURN_BEHAVIOR_CALLBACK is set.
 *
 *  @pre    #SHA2_open() has to be called first.
 *
 *  @param  handle  A #SHA2_Handle returned from #SHA2_open()
 *
 *  @param  key     The key with which @c data is signed
 *
 *  @param  data    Pointer to the location to read from.
 *                  There might be alignment restrictions on different platforms.
 *
 *  @param  dataLength Length of the message @c data, in bytes.
 *
 *  @param  hmac    Pointer to the location to write the HMAC to.
 *                  There might be alignment restrictions on different platforms.
 *
 *  @retval #SHA2_STATUS_SUCCESS               The hash operation succeeded.
 *  @retval #SHA2_STATUS_ERROR                 The hash operation failed.
 *  @retval #SHA2_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource
 *                                             was not available. Try again
 *                                             later.
 *
 *  @sa     #SHA2_open()
 */
int_fast16_t SHA2_hmac(SHA2_Handle handle,
                       CryptoKey *key,
                       const void *data,
                       size_t dataLength,
                       void *hmac);

/*!
 *  @brief Clears internal buffers and aborts an ongoing SHA2 operation.
 *
 *  Clears all internal buffers and the intermediate digest of this driver instance.
 *  If an asynchronous operation is ongoing, the behavior is the same as for
 *  #SHA2_cancelOperation().
 *
 *  @param  handle      A #SHA2_Handle returned from #SHA2_open()
 *
 *  @sa     #SHA2_cancelOperation()
 */
void SHA2_reset(SHA2_Handle handle);

/*!
 *  @brief Aborts an ongoing SHA2 operation and clears internal buffers.
 *
 *  Aborts an ongoing hash operation of this driver instance. The operation will
 *  terminate as though an error occurred and the status code of the operation will be
 *  #SHA2_STATUS_CANCELED in this case.
 *
 *  @param  handle      A #SHA2_Handle returned from #SHA2_open()
 *
 *  @retval #SHA2_STATUS_SUCCESS               The operation was canceled or there was no
 *                                             operation in progress to be canceled.
 */
int_fast16_t SHA2_cancelOperation(SHA2_Handle handle);

/*!
 *  @brief  Selects a new hash algorithm @a type.
 *
 *  This function changes the hash algorithm type of the hash digest at
 *  run-time. The hash type is usually specified during #SHA2_open().
 *
 *  Neither is it allowed to call this function during a running hash operation
 *  nor during an incomplete multi-step hash operation. In this case
 *  #SHA2_STATUS_ERROR would be returned.
 *
 *  @pre    #SHA2_open() has to be called first.
 *
 *  @param  handle      A #SHA2_Handle returned from #SHA2_open()
 *
 *  @param  type        New hash algorithm type
 *
 *  @retval #SHA2_STATUS_SUCCESS               Hash type set correctly.
 *  @retval #SHA2_STATUS_ERROR                 Error. Platform may not support this hash type.
 */
int_fast16_t SHA2_setHashType(SHA2_Handle handle, SHA2_HashType type);

/**
 *  @brief  Constructs a new SHA2 object
 *
 *  Unlike #SHA2_open(), #SHA2_construct() does not require the hwAttrs and
 *  object to be allocated in a #SHA2_Config array that is indexed into.
 *  Instead, the #SHA2_Config, hwAttrs, and object can be allocated at any
 *  location. This allows for relatively simple run-time allocation of temporary
 *  driver instances on the stack or the heap.
 *  The drawback is that this makes it more difficult to write device-agnostic
 *  code. If you use an ifdef with DeviceFamily, you can choose the correct
 *  object and hwAttrs to allocate. That compilation unit will be tied to the
 *  device it was compiled for at this point. To change devices, recompilation
 *  of the application with a different DeviceFamily setting is necessary.
 *
 *  @param  config #SHA2_Config describing the location of the object and hwAttrs.
 *
 *  @param  params #SHA2_Params to configure the driver instance.
 *
 *  @return        Returns a #SHA2_Handle on success or NULL on failure.
 *
 *  @pre    The object struct @c config points to must be zeroed out prior to
 *          calling this function. Otherwise, unexpected behavior may ensue.
 */
SHA2_Handle SHA2_construct(SHA2_Config *config, const SHA2_Params *params);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_SHA2__include */
