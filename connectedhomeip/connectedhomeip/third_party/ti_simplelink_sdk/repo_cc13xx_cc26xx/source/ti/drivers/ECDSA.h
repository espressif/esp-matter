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
/*!
 * @file ECDSA.h
 *
 * @brief TI Driver for Elliptic Curve Digital Signature Algorithm.
 *
 *
 * @anchor ti_drivers_ECDSA_Overview
 * # Overview #
 *
 * The Elliptic Curve Digital Signature Algorithm (ECDSA) is a message
 * authentication scheme between two parties based on operation on elliptic
 * curves over finite fields.
 *
 * Signing a message with ECDSA proves to the recipient that the sender of
 * the message is in possession of the private key corresponding to the
 * transmitted public key used during verification.
 * For most practical systems, this ensures message authentication and
 * integrity.
 *
 * # Steps involved #
 *  - The sender hashes the message they wish to authenticate and
 *    truncates it to the length of the curve parameters of the
 *    elliptic curve used by both parties.
 *  - The sender generates r and s where 0 < r, s < N. These two integers
 *    constitute the actual signature of the message.
 *  - The sender transmits the message, r, s, and the public key to the
 *    recipient.
 *  - The recipient calculates the hash of the message using an agreed
 *    upon hash function and truncates it to the length of the curve
 *    parameters of the elliptic curve used by both parties
 *  - The recipient uses the hash, s, and the sender's public key to
 *    recalculate r.
 *  - The recipient accepts the signature if the received and calculated r
 *    match. Otherwise, they reject the signature.
 *
 * @anchor ti_drivers_ECDSA_Usage
 * # Usage #
 *
 * ## Before starting an ECDSA operation #
 *
 * Before starting an ECDSA operation, the application must do the following:
 *      - Call ECDSA_init() to initialize the driver
 *      - Call ECDSA_Params_init() to initialize the ECDSA_Params to default
 *        values.
 *      - Modify the ECDSA_Params as desired
 *      - Call ECDSA_open() to open an instance of the driver
 *
 * ## Signing a message #
 * To sign a message using an agreed upon hash function and elliptic curve, the
 * application must do the following:
 *  - Initialize an ECDSA_OperationSign struct by calling
 *    ECDSA_OperationSign_init().
 *  - Generate the keying material for the private key. This keying material
 *    must be an integer in the interval [1, n - 1], where n is the order of the
 *    curve. It should be stored in an array with the least significant byte of
 *    the integer hex representation stored in the highest address of the array
 *    (big-endian).
 *    The array should be the same length as the curve parameters of the curve
 *    used. The driver validates public and private keys against the provided
 *    curve.
 *  - Initialize the private key CryptoKey. CryptoKeys are opaque datastructures
 *    and representations of keying material and its storage.
 *    Depending on how the keying material is stored (RAM or flash, key store),
 *    the CryptoKey must be initialized differently.
 *    The ECDSA API can handle all types of CryptoKey.
 *    However, not all device-specific implementions support all types of
 *    CryptoKey.
 *    Devices without a key store will not support CryptoKeys with keying
 *    material stored in a key store for example.
 *    All devices support plaintext CryptoKeys.
 *  - Hash the message using a previously agreed upon hash function and truncate
 *    the hash to the length of the curve parameters of the agreed upon curve.
 *  - Call  ECDSA_sign(). The r and s vectors will be written to the buffers
 *    provided in the function call. Ensure the return value is
 *    ECDSA_STATUS_SUCCESS.
 *
 * ## Verifying a message #
 * After receiving the message, public key, r, and s, the application should
 * do the following to verify the signature:
 *  - Initialize an ECDSA_OperationVerify struct by calling
 *    ECDSA_OperationVerify_init().
 *  - Hash the message using a previously agreed upon hash function and truncate
 *    the hash to the length of the curve parameters of the agreed upon curve.
 *  - Initialize a CryptoKey as public key with the keying material received
 *    from the other party.
 *  - Call ECDSA_verify(). Ensure the return value is ECDSA_STATUS_SUCCESS. The
 *    driver will validate the received public key against the provided curve.
 *
 * ## General usage #
 * The API expects elliptic curves as defined in
 * ti/drivers/cryptoutils/ecc/ECCParams.h.
 * Several commonly used curves are provided. Check the device-specific ECDSA
 * documentation
 * for curve type (short Weierstrass, Montgomery, Edwards) support for your
 * device.
 * ECDSA support for a curve type on a device does not imply curve-type support
 * for other ECC schemes.
 *
 * ## Parameter formatting #
 * Public keys are points on an elliptic curve. These points can
 * be expressed in several ways. The most common one is in affine coordinates as
 * an X,Y pair.
 * This API uses points expressed in uncompressed affine coordinates by default
 * and formatted in octet string format.
 * The octet string format requires a formatting byte in the first byte of the
 * public key. When using uncompressed coordinates, this is the value
 * 0x04.
 * The point itself is stored as a concatenated array of X followed by Y.
 * X and Y are big-endian.
 *
 * This API accepts and returns the keying material of public keys according
 * to the following table:
 *
 * | Curve Type         | PublicKeying Material Array | Array Length         |
 * |--------------------|-----------------------|----------------------------|
 * | Short Weierstrass  | [0x04, X, Y]          | 1 + 2 * Curve Param Length |
 * | Montgomery         | [0x04, X, Y]          | 1 + 2 * Curve Param Length |
 * | Edwards            | [0x04, X, Y]          | 1 + 2 * Curve Param Length |
 *
 * The signature components r and s as well as the hash must be formatted in
 * octet string format. For the hash, this simply means passing the digest of
 * the hash function such as SHA-256 directly into #ECDSA_sign() or
 * #ECDSA_verify(). r and s will be interpreted as big-endian integers.
 *
 * @anchor ti_drivers_ECDSA_Synopsis
 * ## Synopsis
 * @anchor ti_drivers_ECDSA_Synopsis_Code
 * @code
 * // Import ECDSA Driver definitions
 * #include <ti/drivers/ECDSA.h>
 *
 * // Since we are using default ECDSA_Params,
 * // we just pass in NULL for that parameter.
 * ecdsaHandle = ECDSA_open(0, NULL);
 *
 * if (!ecdsaHandle) {
 *     // Handle error
 * }
 *
 * // Initialize myPrivateKey
 * CryptoKeyPlaintext_initKey(&myPrivateKey,
 *                            myPrivateKeyingMaterial,
 *                            sizeof(myPrivateKeyingMaterial));
 *
 * // Initialize the operation
 * ECDSA_OperationSign_init(&operationSign);
 * operationSign.curve             = &ECCParams_NISTP256;
 * operationSign.myPrivateKey      = &myPrivateKey;
 * operationSign.hash              = messageHash;
 * operationSign.r                 = r;
 * operationSign.s                 = s;
 *
 * // Generate the signature
 * operationResult = ECDSA_sign(ecdsaHandle, &operationSign);
 *
 * // Initialize theirPublicKey
 * CryptoKeyPlaintext_initKey(&theirPublicKey,
 *                            theirPublicKeyingMaterial,
 *                            sizeof(theirPublicKeyingMaterial));
 *
 * ECDSA_OperationVerify_init(&operationVerify);
 * operationVerify.curve           = &ECCParams_NISTP256;
 * operationVerify.theirPublicKey  = &theirPublicKey;
 * operationVerify.hash            = messageHash;
 * operationVerify.r               = r;
 * operationVerify.s               = s;
 *
 * // Generate the keying material for myPublicKey and store it in
 * // myPublicKeyingMaterial
 * operationResult = ECDSA_verify(ecdsaHandle, &operationVerify);
 *
 * // Close the driver
 * ECDSA_close(ecdsaHandle);
 * @endcode
 *
 * @anchor ti_drivers_ECDSA_Examples
 * # Examples #
 *
 * ## ECDSA sign with plaintext CryptoKeys #
 *
 * @code
 *
 * #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 * #include <ti/drivers/ECDSA.h>
 *
 * ...
 *
 * // This vector is taken from the NIST ST toolkit examples from ECDSA_Prime.pdf
 * uint8_t myPrivateKeyingMaterial[32] = {0x96, 0xBF, 0x85, 0x49, 0xC3, 0x79, 0xE4, 0x04,
 *                                        0xED, 0xA1, 0x08, 0xA5, 0x51, 0xF8, 0x36, 0x23,
 *                                        0x12, 0xD8, 0xD1, 0xB2, 0xA5, 0xFA, 0x57, 0x06,
 *                                        0xE2, 0xCC, 0x22, 0x5C, 0xF6, 0xF9, 0x77, 0xC4};
 * uint8_t messageHash[32] = {0xA4,0x1A,0x41,0xA1,0x2A,0x79,0x95,0x48,
 *                            0x21,0x1C,0x41,0x0C,0x65,0xD8,0x13,0x3A,
 *                            0xFD,0xE3,0x4D,0x28,0xBD,0xD5,0x42,0xE4,
 *                            0xB6,0x80,0xCF,0x28,0x99,0xC8,0xA8,0xC4};
 * uint8_t r[32] = {0};
 * uint8_t s[32] = {0};
 *
 *
 * CryptoKey myPrivateKey;
 *
 * ECDSA_Handle ecdsaHandle;
 *
 * int_fast16_t operationResult;
 *
 * // Since we are using default ECDSA_Params,
 * // we just pass in NULL for that parameter.
 * ecdsaHandle = ECDSA_open(0, NULL);
 *
 * if (!ecdsaHandle) {
 *     // Handle error
 * }
 *
 * // Initialize myPrivateKey
 * CryptoKeyPlaintext_initKey(&myPrivateKey,
 *                            myPrivateKeyingMaterial,
 *                            sizeof(myPrivateKeyingMaterial));
 *
 * // Initialize the operation
 * ECDSA_OperationSign_init(&operationSign);
 * operationSign.curve             = &ECCParams_NISTP256;
 * operationSign.myPrivateKey      = &myPrivateKey;
 * operationSign.hash              = messageHash;
 * operationSign.r                 = r;
 * operationSign.s                 = s;
 *
 * // Generate the signature
 * operationResult = ECDSA_sign(ecdsaHandle, &operationSign);
 *
 * if (operationResult != ECDSA_STATUS_SUCCESS) {
 *     // Handle error
 * }
 *
 * @endcode
 *
 *
 * ## ECDSA verify with plaintext CryotoKeys #
 *
 * @code
 *
 * #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 * #include <ti/drivers/ECDSA.h>
 *
 * ...
 *
 * // This vector is taken from the NIST ST toolkit examples from
 * // ECDSA_Prime.pdf
 * // messageHash is the direct result of using SHA-256 on the string
 * // 'Example of ECDSA with P-256' without the terminating zero.
 * uint8_t theirPublicKeyingMaterial[65] =  {0x04,
 *                                           // X
 *                                           0xB7,0xE0,0x8A,0xFD,0xFE,0x94,0xBA,0xD3,
 *                                           0xF1,0xDC,0x8C,0x73,0x47,0x98,0xBA,0x1C,
 *                                           0x62,0xB3,0xA0,0xAD,0x1E,0x9E,0xA2,0xA3,
 *                                           0x82,0x01,0xCD,0x08,0x89,0xBC,0x7A,0x19,
 *                                           //Y
 *                                           0x36,0x03,0xF7,0x47,0x95,0x9D,0xBF,0x7A,
 *                                           0x4B,0xB2,0x26,0xE4,0x19,0x28,0x72,0x90,
 *                                           0x63,0xAD,0xC7,0xAE,0x43,0x52,0x9E,0x61,
 *                                           0xB5,0x63,0xBB,0xC6,0x06,0xCC,0x5E,0x09};
 * uint8_t messageHash[32] = {0xA4,0x1A,0x41,0xA1,0x2A,0x79,0x95,0x48,
 *                            0x21,0x1C,0x41,0x0C,0x65,0xD8,0x13,0x3A,
 *                            0xFD,0xE3,0x4D,0x28,0xBD,0xD5,0x42,0xE4,
 *                            0xB6,0x80,0xCF,0x28,0x99,0xC8,0xA8,0xC4};
 * uint8_t r[32] = {0x2B,0x42,0xF5,0x76,0xD0,0x7F,0x41,0x65,
 *                  0xFF,0x65,0xD1,0xF3,0xB1,0x50,0x0F,0x81,
 *                  0xE4,0x4C,0x31,0x6F,0x1F,0x0B,0x3E,0xF5,
 *                  0x73,0x25,0xB6,0x9A,0xCA,0x46,0x10,0x4F};
 * uint8_t s[32] = {0xDC,0x42,0xC2,0x12,0x2D,0x63,0x92,0xCD,
 *                  0x3E,0x3A,0x99,0x3A,0x89,0x50,0x2A,0x81,
 *                  0x98,0xC1,0x88,0x6F,0xE6,0x9D,0x26,0x2C,
 *                  0x4B,0x32,0x9B,0xDB,0x6B,0x63,0xFA,0xF1};
 *
 *
 * CryptoKey theirPublicKey;
 *
 * ECDSA_Handle ecdsaHandle;
 *
 * int_fast16_t operationResult;
 *
 * ECDSA_OperationVerify operationVerify;
 *
 * // Since we are using default ECDSA_Params, we just pass in NULL for that
 * // parameter.
 * ecdsaHandle = ECDSA_open(0, NULL);
 *
 * if (!ecdsaHandle) {
 *     // Handle error
 * }
 *
 * // Initialize theirPublicKey
 * CryptoKeyPlaintext_initKey(&theirPublicKey,
 *                            theirPublicKeyingMaterial,
 *                            sizeof(theirPublicKeyingMaterial));
 *
 * ECDSA_OperationVerify_init(&operationVerify);
 * operationVerify.curve           = &ECCParams_NISTP256;
 * operationVerify.theirPublicKey  = &theirPublicKey;
 * operationVerify.hash            = messageHash;
 * operationVerify.r               = r;
 * operationVerify.s               = s;
 *
 * // Generate the keying material for myPublicKey and store it in
 * // myPublicKeyingMaterial
 * operationResult = ECDSA_verify(ecdsaHandle, &operationVerify);
 *
 * if (operationResult != ECDSA_STATUS_SUCCESS) {
 *     // Handle error
 * }
 *
 * @endcode
 *
 *
 */


#ifndef ti_drivers_ECDSA__include
#define ti_drivers_ECDSA__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>
#include <ti/drivers/cryptoutils/ecc/ECCParams.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Common ECDSA status code reservation offset.
 * ECDSA driver implementations should offset status codes with
 * ECDSA_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define ECDSAXYZ_STATUS_ERROR0    ECDSA_STATUS_RESERVED - 0
 * #define ECDSAXYZ_STATUS_ERROR1    ECDSA_STATUS_RESERVED - 1
 * #define ECDSAXYZ_STATUS_ERROR2    ECDSA_STATUS_RESERVED - 2
 * @endcode
 */
#define ECDSA_STATUS_RESERVED        (-32)

/*!
 * @brief   Successful status code.
 *
 * Functions return ECDSA_STATUS_SUCCESS if the function was executed
 * successfully.
 */
#define ECDSA_STATUS_SUCCESS         (0)

/*!
 * @brief   Generic error status code.
 *
 * Functions return ECDSA_STATUS_ERROR if the function was not executed
 * successfully.
 */
#define ECDSA_STATUS_ERROR           (-1)

/*!
 * @brief   An error status code returned if the hardware or software resource
 * is currently unavailable.
 *
 * ECDSA driver implementations may have hardware or software limitations on how
 * many clients can simultaneously perform operations. This status code is
 * returned if the mutual exclusion mechanism signals that an operation cannot
 * currently be performed.
 */
#define ECDSA_STATUS_RESOURCE_UNAVAILABLE (-2)

/*!
 * @brief   The r value passed in is larger than the order of the curve.
 *
 * Signature components (r and s) must be integers in the interval [1, n - 1],
 * where n is the order of the curve.
 */
#define ECDSA_STATUS_R_LARGER_THAN_ORDER (-3)

/*!
 * @brief   The s value passed in is larger than the order of the curve.
 *
 * Signature components (r and s) must be integers in the interval [1, n - 1],
 * where n is the order of the curve.
 */
#define ECDSA_STATUS_S_LARGER_THAN_ORDER (-4)

/*!
 * @brief   The public key of the other party does not lie upon the curve.
 *
 * The public key received from the other party does not lie upon the agreed
 * upon curve.
 */
#define ECDSA_STATUS_PUBLIC_KEY_NOT_ON_CURVE (-5)

/*!
 * @brief   A coordinate of the public key of the other party is too large.
 *
 * A coordinate of the public key received from the other party is larger than
 * the prime of the curve. This implies that the point was not correctly
 * generated on that curve.
 */
#define ECDSA_STATUS_PUBLIC_KEY_LARGER_THAN_PRIME (-6)

/*!
 * @brief   The public key to verify against is the point at infinity.
 *
 * The point at infinity is not a valid input.
 */
#define ECDSA_STATUS_POINT_AT_INFINITY (-7)

/*!
 *  @brief  The ongoing operation was canceled.
 */
#define ECDSA_STATUS_CANCELED (-8)

/*!
 *  @brief  The provided CryptoKey does not match the expected size
 *
 *  The driver expects the private key to have the same length as other curve
 *  parameters and the public key to have a length of twice that plus one.
 *  If the provided CryptoKeys for the public and private keys do not match this
 *  scheme, this error will be returned.
 */
#define ECDSA_STATUS_INVALID_KEY_SIZE (-10)

/*!
 *  @brief ECDSA Global configuration
 *
 *  The ECDSA_Config structure contains a set of pointers used to characterize
 *  the ECDSA driver implementation.
 *
 *  This structure needs to be defined before calling ECDSA_init() and it must
 *  not be changed thereafter.
 *
 *  @sa     ECDSA_init()
 */
typedef struct {
    /*! Pointer to a driver specific data object */
    void               *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void         const *hwAttrs;
} ECDSA_Config;

/*!
 *  @brief  A handle that is returned from an ECDSA_open() call.
 */
typedef ECDSA_Config *ECDSA_Handle;

/*!
 * @brief   The way in which ECDSA function calls return after performing an
 * encryption + authentication or decryption + verification operation.
 *
 * Not all ECDSA operations exhibit the specified return behavor. Functions that
 * do not require significant computation and cannot offload that computation to
 * a background threadbehave like regular functions. Which functions exhibit the
 * specfied return behavior is implementation dependent. Specifically, a
 * software-backed implementation run on the same CPU as the application will
 * emulate the return behavior while not actually offloading the computation to
 * the background thread.
 *
 * ECDSA functions exhibiting the specified return behavior have restrictions on
 * the context from which they may be called.
 *
 * |                                | Task  | Hwi   | Swi   |
 * |--------------------------------|-------|-------|-------|
 * |ECDSA_RETURN_BEHAVIOR_CALLBACK  | X     | X     | X     |
 * |ECDSA_RETURN_BEHAVIOR_BLOCKING  | X     |       |       |
 * |ECDSA_RETURN_BEHAVIOR_POLLING   | X     | X     | X     |
 *
 */
typedef enum {
    ECDSA_RETURN_BEHAVIOR_CALLBACK = 1,     /*!< The function call will return immediately while the
                                             *   ECDSA operation goes on in the background. The registered
                                             *   callback function is called after the operation completes.
                                             *   The context the callback function is called (task, HWI, SWI)
                                             *   is implementation-dependent.
                                             */
    ECDSA_RETURN_BEHAVIOR_BLOCKING = 2,     /*!< The function call will block while ECDSA operation goes
                                             *   on in the background. ECDSA operation results are available
                                             *   after the function returns.
                                             */
    ECDSA_RETURN_BEHAVIOR_POLLING  = 4,     /*!< The function call will continuously poll a flag while ECDSA
                                             *   operation goes on in the background. ECDSA operation results
                                             *   are available after the function returns.
                                             */
} ECDSA_ReturnBehavior;

/*!
 *  @brief  Struct containing the parameters required for signing a message.
 */
typedef struct {
    const ECCParams_CurveParams     *curve;         /*!< A pointer to the elliptic curve parameters */
    const CryptoKey                 *myPrivateKey;  /*!< A pointer to the private ECC key that will
                                                     *   sign the hash of the message
                                                     */
    const uint8_t                   *hash;          /*!< A pointer to the hash of the message in
                                                     *   octet string format.
                                                     *   Must be the same length as the other curve parameters.
                                                     */
    uint8_t                         *r;             /*!< A pointer to the buffer the r component of
                                                     *   the signature will be written to.
                                                     *   Formatted in octet string format.
                                                     *   Must be of the same length as other
                                                     *   params of the curve used.
                                                     */
    uint8_t                         *s;             /*!< A pointer to the buffer the s component of
                                                     *   the signature will be written to.
                                                     *   Must be of the same length as other
                                                     *   params of the curve used.
                                                     */
} ECDSA_OperationSign;

/*!
 *  @brief  Struct containing the parameters required for verifying a message.
 */
typedef struct {
    const ECCParams_CurveParams     *curve;             /*!< A pointer to the elliptic curve parameters */
    const CryptoKey                 *theirPublicKey;    /*!< A pointer to the public key of the party
                                                         *   that signed the hash of the message
                                                         */
    const uint8_t                   *hash;              /*!< A pointer to the hash of the message in
                                                         *   octet string format.
                                                         *   Must be the same length as the other curve parameters.
                                                         */
    const uint8_t                   *r;                 /*!< A pointer to the r component of the received
                                                         *   signature.
                                                         *   Formatted in octet string format.
                                                         *   Must be of the same length
                                                         *   as other params of the curve used.
                                                         */
    const uint8_t                   *s;                 /*!< A pointer to the s component of the received
                                                         *   signature.
                                                         *   Formatted in octet string format.
                                                         *   Must be of the same length
                                                         *   as other params of the curve used.
                                                         */
} ECDSA_OperationVerify;

/*!
 *  @brief  Union containing pointers to all supported operation structs.
 */
typedef union {
    ECDSA_OperationSign     *sign;      /*!< A pointer to an ECDSA_OperationSign struct */
    ECDSA_OperationVerify   *verify;    /*!< A pointer to an ECDSA_OperationVerify struct */
} ECDSA_Operation;

/*!
 *  @brief  Enum for the operation types supported by the driver.
 */
typedef enum {
    ECDSA_OPERATION_TYPE_SIGN = 1,
    ECDSA_OPERATION_TYPE_VERIFY = 2,
} ECDSA_OperationType;

/*!
 *  @brief  The definition of a callback function used by the ECDSA driver
 *          when used in ::ECDSA_RETURN_BEHAVIOR_CALLBACK
 *
 *  @param  handle Handle of the client that started the ECDSA operation.
 *
 *  @param  returnStatus The result of the ECDSA operation. May contain an error
 *                       code if the result is the point at infinity for
 *                       example.
 *
 *  @param  operation A union of pointers to operation structs. Only one type
 *          of pointer is valid per call to the callback function. Which type
 *          is currently valid is determined by /c operationType. The union
 *          allows easier access to the struct's fields without the need to
 *          typecase the result.
 *
 *  @param  operationType This parameter determined which operation the
 *          callback refers to and which type to access through /c operation.
 */
typedef void (*ECDSA_CallbackFxn) (ECDSA_Handle handle,
                                   int_fast16_t returnStatus,
                                   ECDSA_Operation operation,
                                   ECDSA_OperationType operationType);

/*!
 *  @brief  ECDSA Parameters
 *
 *  ECDSA Parameters are used to with the ECDSA_open() call. Default values for
 *  these parameters are set using ECDSA_Params_init().
 *
 *  @sa     ECDSA_Params_init()
 */
typedef struct {
    ECDSA_ReturnBehavior    returnBehavior; /*!< Blocking, callback, or polling
                                             *   return behavior
                                             */
    ECDSA_CallbackFxn       callbackFxn;    /*!< Callback function pointer */
    uint32_t                timeout;        /*!< Timeout in system ticks before
                                             *   the operation fails and returns
                                             */
    void                   *custom;         /*!< Custom argument used by driver
                                             *   implementation
                                             */
} ECDSA_Params;

/*!
 *  @brief  This function initializes the ECDSA module.
 *
 *  @pre    The ECDSA_config structure must exist and be persistent before this
 *          function can be called. This function must also be called before
 *          any other ECDSA driver APIs. This function call does not modify any
 *          peripheral registers.
 */
void ECDSA_init(void);

/*!
 *  @brief  Function to close an ECDSA peripheral specified by the ECDSA handle
 *
 *  @pre    ECDSA_open() has to be called first.
 *
 *  @param  handle An ECDSA handle returned from ECDSA_open()
 *
 *  @sa     ECDSA_open()
 */
void ECDSA_close(ECDSA_Handle handle);

/*!
 *  @brief  This function opens a given ECDSA peripheral.
 *
 *  @pre    ECDSA controller has been initialized using ECDSA_init()
 *
 *  @param  index         Logical peripheral number for the ECDSA indexed into
 *                        the ECDSA_config table
 *
 *  @param  params        Pointer to an parameter block, if NULL it will use
 *                        default values.
 *
 *  @return An ECDSA_Handle on success or a NULL on an error or if it has been
 *          opened already.
 *
 *  @sa     ECDSA_init()
 *  @sa     ECDSA_close()
 */
ECDSA_Handle ECDSA_open(uint_least8_t index, const ECDSA_Params *params);

/*!
 *  @brief  Function to initialize the ECDSA_Params struct to its defaults
 *
 *  @param  params      An pointer to ECDSA_Params structure for
 *                      initialization
 *
 *  Defaults values are:
 *      returnBehavior              = ECDSA_RETURN_BEHAVIOR_BLOCKING
 *      callbackFxn                 = NULL
 *      timeout                     = SemaphoreP_WAIT_FOREVER
 *      custom                      = NULL
 */
void ECDSA_Params_init(ECDSA_Params *params);

/*!
 *  @brief  Function to initialize an ECDSA_OperationSign struct to its defaults
 *
 *  @param  operation   A pointer to ECDSA_OperationSign structure for
 *                      initialization
 *
 *  Defaults values are all zeros.
 */
void ECDSA_OperationSign_init(ECDSA_OperationSign *operation);

/*!
 *  @brief  Function to initialize an ECDSA_OperationSign struct to its defaults
 *
 *  @param  operation   An pointer to ECDSA_OperationSign structure for
 *                      initialization
 *
 *  Defaults values are all zeros.
 */
void ECDSA_OperationVerify_init(ECDSA_OperationVerify *operation);

/*!
 *  @brief Signs a hashed message.
 *
 *  ECDSA_sign() generates a signature (\c r, \c s) of a \c hash of a message.
 *
 *  @pre    ECDSA_OperationSign_init() must be called on \c operation first.
 *          The driver must have been opened by calling ECDSA_open() first.
 *
 *  @param [in]     handle          An ECDSA handle returned from ECDSA_open()
 *
 *  @param [in]     operation       A struct containing the pointers to the
 *                                  buffers necessary to perform the operation
 *  @sa ECDSA_verify()
 *
 *  @retval #ECDSA_STATUS_SUCCESS               The operation succeeded.
 *  @retval #ECDSA_STATUS_ERROR                 The operation failed.
 *  @retval #ECDSA_STATUS_RESOURCE_UNAVAILABLE  The required hardware resource was not available. Try again later.
 *  @retval #ECDSA_STATUS_CANCELED              The operation was canceled.
 */
int_fast16_t ECDSA_sign(ECDSA_Handle handle, ECDSA_OperationSign *operation);

/*!
 *  @brief Verifies a received signature matches a hash and public key
 *
 *  @pre    ECDSA_OperationVerify_init() must be called on \c operation first.
 *          The driver must have been opened by calling ECDSA_open() first.
 *
 *  @param [in]     handle          An ECDSA handle returned from ECDSA_open()
 *
 *  @param [in]     operation       A struct containing the pointers to the
 *                                  buffers necessary to perform the operation
 *
 *  @sa ECDSA_sign()
 *
 *  @retval #ECDSA_STATUS_SUCCESS                       The operation succeeded.
 *  @retval #ECDSA_STATUS_ERROR                         The operation failed. This is the return status if the signature did not match.
 *  @retval #ECDSA_STATUS_RESOURCE_UNAVAILABLE          The required hardware resource was not available. Try again later.
 *  @retval #ECDSA_STATUS_CANCELED                      The operation was canceled.
 *  @retval #ECDSA_STATUS_R_LARGER_THAN_ORDER           The r value passed in is larger than the order of the curve.
 *  @retval #ECDSA_STATUS_S_LARGER_THAN_ORDER           The s value passed in is larger than the order of the curve.
 *  @retval #ECDSA_STATUS_PUBLIC_KEY_NOT_ON_CURVE       The public key of the other party does not lie upon the curve.
 *  @retval #ECDSA_STATUS_PUBLIC_KEY_LARGER_THAN_PRIME  One of the public key coordinates is larger the the curve's prime.
 *  @retval #ECDSA_STATUS_POINT_AT_INFINITY             The public key to verify against is the point at infinity.
 */
int_fast16_t ECDSA_verify(ECDSA_Handle handle, ECDSA_OperationVerify *operation);

/*!
 *  @brief Cancels an ongoing ECDSA operation.
 *
 *  Asynchronously cancels an ECDSA operation. Only available when using
 *  ECDSA_RETURN_BEHAVIOR_CALLBACK or ECDSA_RETURN_BEHAVIOR_BLOCKING.
 *  The operation will terminate as though an error occured. The
 *  return status code of the operation will be ECDSA_STATUS_CANCELED.
 *
 *  @param  handle Handle of the operation to cancel
 *
 *  @retval #ECDSA_STATUS_SUCCESS               The operation was canceled.
 *  @retval #ECDSA_STATUS_ERROR                 The operation was not canceled. There may be no operation to cancel.
 */
int_fast16_t ECDSA_cancelOperation(ECDSA_Handle handle);

/**
 *  @brief  Constructs a new ECDSA object
 *
 *  Unlike #ECDSA_open(), #ECDSA_construct() does not require the hwAttrs and
 *  object to be allocated in a #ECDSA_Config array that is indexed into.
 *  Instead, the #ECDSA_Config, hwAttrs, and object can be allocated at any
 *  location. This allows for relatively simple run-time allocation of temporary
 *  driver instances on the stack or the heap.
 *  The drawback is that this makes it more difficult to write device-agnostic
 *  code. If you use an ifdef with DeviceFamily, you can choose the correct
 *  object and hwAttrs to allocate. That compilation unit will be tied to the
 *  device it was compiled for at this point. To change devices, recompilation
 *  of the application with a different DeviceFamily setting is necessary.
 *
 *  @param  config #ECDSA_Config describing the location of the object
 *                 and hwAttrs.
 *
 *  @param  params #ECDSA_Params to configure the driver instance.
 *
 *  @return        Returns a #ECDSA_Handle on success or NULL on failure.
 *
 *  @pre    The object struct @c config points to must be zeroed out prior to
 *          calling this function. Otherwise, unexpected behavior may ensue.
 */
ECDSA_Handle ECDSA_construct(ECDSA_Config *config, const ECDSA_Params *params);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_ECDSA__include */
