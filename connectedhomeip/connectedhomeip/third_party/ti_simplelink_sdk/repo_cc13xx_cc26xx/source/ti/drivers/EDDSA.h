/*
 * Copyright (c) 2020-2021, Texas Instruments Incorporated
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
 * @file EDDSA.h
 *
 * @brief TI Driver for Edwards Curve Digital Signature Algorithm.
 *
 *
 * @anchor ti_drivers_EDDSA_Overview
 * # Overview #
 *
 * The Edwards Curve Digital Signature Algorithm (EdDSA) is a message
 * authentication scheme between two parties based on operation on Edwards
 * curves over finite fields. This driver supports the PureEdDSA scheme
 * for Edwards curve Ed25519 as is described in RFC 8032:
 * https://tools.ietf.org/html/rfc8032 . EdDSA variants, such as
 * Ed25519ph or Ed25519ctx are currently not supported.
 *
 * Signing a message with EdDSA proves to the recipient that the sender of
 * the message is in possession of the private key corresponding to the
 * transmitted public key used during verification.
 * For most practical systems, this ensures message authentication and
 * integrity.
 *
 * # Steps involved #
 *  - Public Key Generation. The sender generates an EdDSA private-public
 *    keypair with private key k and public key A. For Ed25519, these are
 *    32 bytes in little endian. The private key k comes from the DRBG.
 *    This private key is hashed, pruned (clear bits 0,1,2,255 and set bit
 *    254), and used as a scalar to generate public key A.
 *  - Signature Generation. The sender hashes part of the private key
 *    k and message to be signed M. This result is used as a scalar to
 *    generate EdDSA signature component R which is a point on Ed25519.
 *    The signature component R, public key A, and message M are hashed
 *    to find a value that is used to generate the EdDSA signature
 *    component S which is a scalar. The signature for message M is {R,S}.
 *  - Signature Verification. The recipient receives signature {R,S}. The
 *    scalar S value is checked to be less than the order of the Edwards curve.
 *    The signature component R, public key A, and message M are hashed to
 *    find a scalar to recalculate R. The recipient accepts the signature
 *    if the received and calculated R match. Otherwise, they reject the
 *    signature.
 *
 * @note From here, we use EDDSA rather than EdDSA to be consistent with
 *       driver naming conventions.
 *
 * @anchor ti_drivers_EDDSA_Usage
 * # Usage #
 *
 * ## Before starting an EDDSA operation #
 *
 * Before starting an EDDSA operation, the application must do the following:
 *      - Call EDDSA_init() to initialize the driver
 *      - Call EDDSA_Params_init() to initialize the EDDSA_Params to default
 *        values.
 *      - Modify the EDDSA_Params as desired
 *      - Call EDDSA_open() to open an instance of the driver
 *
 * ## Generating an Ed25519 Public Key #
 * To generate an Ed25519 public key, the application must do the following:
 *  - Initialize an EDDSA_OperationGeneratePublicKey struct by calling
 *    EDDSA_OperationGeneratePublicKey_init().
 *  - Initialize the public key CryptoKey.
 *    CryptoKeys are opaque data structures and representations of keying
 *    material and its storage.
 *    Depending on how the keying material is stored (RAM or flash, key store),
 *    the CryptoKey must be initialized differently.
 *    The EDDSA API can handle all types of CryptoKey.
 *    However, not all device-specific implementations support all types of
 *    CryptoKey.
 *    Devices without a key store will not support CryptoKeys with keying
 *    material stored in a key store for example.
 *    All devices support plaintext CryptoKeys.
 *  - Initialize the private key CryptoKey and set its value as a random or
 *    selected 32-byte value
 *  - Call EDDSA_operationGeneratePublicKey(). The public key
 *    information will be written to the buffers provided in the function call.
 *    myPublicKey is the public key A generated from part of a hash of the
 *    private key.
 *    Ensure the return value is EDDSA_STATUS_SUCCESS.
 *
 *
 * ## Signing a message #
 * To sign a message using Ed25519, the application must do the following:
 *  - Initialize an EDDSA_OperationSign struct by calling
 *    EDDSA_OperationSign_init().
 *  - Initialize and set the preHashedMessage buffer and corresponding
 *    preHashedMessageLength. Note that this function accepts any length. This
 *    is the message to sign.
 *  - Initialize the signature components R and S as 32 byte arrays.
 *  - Use the CryptoKeys myPrivateKey and myPublicKey generated from
 *    EDDSA_OperationGeneratePublicKey().
 *  - Call EDDSA_sign(). The R and S vectors will be written to the buffers
 *    provided in the function call. Ensure the return value is
 *    EDDSA_STATUS_SUCCESS.
 *
 * ## Verifying a message #
 * After receiving the message, public key, R, and S, the application should
 * do the following to verify the signature:
 *  - Initialize an EDDSA_OperationVerify struct by calling
 *    EDDSA_OperationVerify_init().
 *  - Initialize and set the preHashedMessage buffer and corresponding
 *    preHashedMessageLength. Note that this function accepts any length. This
 *    is the message to verify.
 *  - Initialize a CryptoKey as public key with the keying material received
 *    from the other party.
 *  - Call EDDSA_verify(). Ensure the return value is EDDSA_STATUS_SUCCESS. The
 *    driver will validate the received public key against the provided curve.
 *
 * ## General usage #
 * The API expects Edwards elliptic curves as defined in
 * ti/drivers/cryptoutils/ecc/ECCParams.h.
 * Several commonly used curves are provided. Check the device-specific EDDSA
 * documentation
 * for curve type (short Weierstrass, Montgomery, Edwards) support for your
 * device.
 * EDDSA support for a curve type on a device does not imply curve-type support
 * for other ECC schemes.
 *
 * ## Parameter formatting #
 * Public keys are points on an elliptic curve. For EDDSA, these points are
 * represented in their compressed form.
 * This API uses points expressed in compressed affine coordinates by default
 * and formatted in little endian.
 * The point itself is stored as Y in little endian. If the X-coordinate is
 * odd, the most significant bit of the public key is set (that is 0x80 || y).
 * Otherwise, the public key is simply y and the even choice of x-coordinate
 * is selected.
 *
 * This API accepts and returns the keying material of public keys according
 * to the following table:
 *
 * | Curve Type         | PublicKeying Material Array | Array Length       |
 * |--------------------|-----------------------------|--------------------|
 * | Ed25519            | [{0x80 or 0x00} || Y]       | Curve Param Length |
 *
 * The signature components R and S as well as the hash must be formatted in
 * little endian format. For the hash, this simply means passing the digest of
 * the hash function SHA-512 directly into #EDDSA_sign() or #EDDSA_verify().
 * R and S will be interpreted as little-endian integers.
 *
 * @anchor ti_drivers_EDDSA_Synopsis
 * ## Synopsis
 * @anchor ti_drivers_EDDSA_Synopsis_Code
 * @code
 * // Import EdDSA Driver definitions
 * #include <ti/drivers/EDDSA.h>
 *
 * // Create default parameters
 * EDDSA_Handle eddsaHandle;
 * EDDSA_Params params;
 *
 * EDDSA_init();
 *
 * EDDSA_Params_init(&params);
 * params.returnBehavior = EDDSA_RETURN_BEHAVIOR_BLOCKING;
 *
 * eddsaHandle = EDDSA_open(0, &params);
 *
 * if (!eddsaHandle) {
 *     // Handle error
 * }
 *
 * CryptoKey myPrivateKey;
 * CryptoKey myPublicKey;
 * CryptoKey theirPublicKey;
 *
 * // Initialize myPrivateKey
 * CryptoKeyPlaintext_initKey(&myPrivateKey,
 *                            myPrivateKeyingMaterial,
 *                            sizeof(myPrivateKeyingMaterial));
 *
 * // Initialize myPublicKey
 * CryptoKeyPlaintext_initKey(&myPublicKey,
 *                            myPublicKeyMaterial,
 *                            sizeof(myPublicKeyMaterial));
 *
 * // Initialize the operation
 * EDDSA_OperationGeneratePublicKey_init(&operationGeneratePublicKey);
 * operationGeneratePublicKey.curve             = &ECCParams_Ed25519;
 * operationGeneratePublicKey.myPrivateKey      = &myPrivateKey;
 * operationGeneratePublicKey.myPublicKey       = &myPublicKey;
 *
 * operationResult = EDDSA_generatePublicKey(eddsaHandle,
 *                                           &operationGeneratePublicKey);
 *
 * if (operationResult != EDDSA_STATUS_SUCCESS) {
 *     // Handle error
 * }
 *
 * // Initialize the operation
 * EDDSA_OperationSign_init(&operationSign);
 * operationSign.curve                   = &ECCParams_Ed25519;
 * operationSign.myPrivateKey            = &myPrivateKey;
 * operationSign.myPublicKey             = &myPublicKey;
 * operationSign.preHashedMessage        = preHashedMessage;
 * operationSign.preHashedMessageLength  = preHashedMessageLength;
 * operationSign.R                       = sigR;
 * operationSign.S                       = sigS;
 *
 * // Generate the signature
 * operationResult = EDDSA_sign(eddsaHandle, &operationSign);
 *
 * if (operationResult != EDDSA_STATUS_SUCCESS) {
 *     // Handle error
 * }
 *
 * // Initialize theirPublicKey
 * CryptoKeyPlaintext_initKey(&theirPublicKey,
 *                            theirPublicKeyingMaterial,
 *                            sizeof(theirPublicKeyingMaterial));
 *
 * EDDSA_OperationVerify_init(&operationVerify);
 * operationVerify.curve                  = &ECCParams_Ed25519;
 * operationVerify.theirPublicKey         = &theirPublicKey;
 * operationVerify.preHashedMessage       = preHashedMessage;
 * operationVerify.preHashedMessageLength = preHashedMessageLength;
 * operationVerify.R                      = sigR;
 * operationVerify.S                      = sigS;
 *
 * // Verify the EdDSA signature
 * operationResult = EDDSA_verify(eddsaHandle, &operationVerify);
 *
 * if (operationResult != EDDSA_STATUS_SUCCESS) {
 *     // Handle error
 * }
 *
 * // Close the driver
 * EDDSA_close(eddsaHandle);
 * @endcode
 *
 * @anchor ti_drivers_EDDSA_Examples
 * # Examples #
 * @code
 *
 * #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 * #include <ti/drivers/EDDSA.h>
 *
 * EDDSA_Handle eddsaHandle;
 * EDDSA_Params params;
 * CryptoKey theirPublicKey;
 * int_fast16_t operationResult;
 * EDDSA_OperationVerify operationVerify;
 *
 * // This vector is test vector 2 from Section 7.1 of RFC 8032.
 * uint8_t publicKey[32] =  {0x3D,0x40,0x17,0xC3,0xE8,0x43,0x89,0x5A,
 *                           0x92,0xB7,0x0A,0xA7,0x4D,0x1B,0x7E,0xBC,
 *                           0x9C,0x98,0x2C,0xCF,0x2E,0xC4,0x96,0x8C,
 *                           0xC0,0xCD,0x55,0xF1,0x2A,0xF4,0x66,0x0C};
 * uint8_t sigR[32] = {0x92,0xA0,0x09,0xA9,0xF0,0xD4,0xCA,0xB8,
 *                     0x72,0x0E,0x82,0x0B,0x5F,0x64,0x25,0x40,
 *                     0xA2,0xB2,0x7B,0x54,0x16,0x50,0x3F,0x8F,
 *                     0xB3,0x76,0x22,0x23,0xEB,0xDB,0x69,0xDA};
 * uint8_t sigS[32] = {0x08,0x5A,0xC1,0xE4,0x3E,0x15,0x99,0x6E,
 *                     0x45,0x8F,0x36,0x13,0xD0,0xF1,0x1D,0x8C,
 *                     0x38,0x7B,0x2E,0xAE,0xB4,0x30,0x2A,0xEE,
 *                     0xB0,0x0D,0x29,0x16,0x12,0xBB,0x0C,0x00};
 * uint8_t preHashedMessage[1] = {0x72};
 * uint32_t preHashedMessageLength = 1;
 *
 * // Initialize EDDSA driver
 * EDDSA_init();
 *
 * EDDSA_Params_init(&params);
 * params.returnBehavior = EDDSA_RETURN_BEHAVIOR_BLOCKING;
 *
 * eddsaHandle = EDDSA_open(0, &params);
 *
 * if (!eddsaHandle) {
 *     // Handle error
 * }
 *
 * // Initialize theirPublicKey
 * CryptoKeyPlaintext_initKey(&theirPublicKey,
 *                            publicKey,
 *                            sizeof(publicKey));
 *
 * EDDSA_OperationVerify_init(&operationVerify);
 * operationVerify.curve                  = &ECCParams_Ed25519;
 * operationVerify.theirPublicKey         = &theirPublicKey;
 * operationVerify.preHashedMessage       = preHashedMessage;
 * operationVerify.preHashedMessageLength = preHashedMessageLength;
 * operationVerify.R                      = sigR;
 * operationVerify.S                      = sigS;
 *
 * // Verify the EdDSA signature
 * operationResult = EDDSA_verify(eddsaHandle, &operationVerify);
 *
 * if (operationResult != EDDSA_STATUS_SUCCESS) {
 *     // Handle error
 * }
 * @endcode
 *
 *
 */

#ifndef ti_drivers_EDDSA__include
#define ti_drivers_EDDSA__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>
#include <ti/drivers/cryptoutils/ecc/ECCParams.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Common EDDSA status code reservation offset.
 * EDDSA driver implementations should offset status codes with
 * EDDSA_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define EDDSAXYZ_STATUS_ERROR0    EDDSA_STATUS_RESERVED - 0
 * #define EDDSAXYZ_STATUS_ERROR1    EDDSA_STATUS_RESERVED - 1
 * #define EDDSAXYZ_STATUS_ERROR2    EDDSA_STATUS_RESERVED - 2
 * @endcode
 */
#define EDDSA_STATUS_RESERVED        (-32)

/*!
 * @brief   Successful status code.
 *
 * Functions return EDDSA_STATUS_SUCCESS if the function was executed
 * successfully.
 */
#define EDDSA_STATUS_SUCCESS         (0)

/*!
 * @brief   Generic error status code.
 *
 * Functions return EDDSA_STATUS_ERROR if the function was not executed
 * successfully.
 */
#define EDDSA_STATUS_ERROR           (-1)

 /*!
  *  @brief  The ongoing operation was canceled.
  */
#define EDDSA_STATUS_CANCELED (-2)

 /*!
  * @brief   An error status code returned if the hash hardware or software
  * resource is currently unavailable.
  *
  * EDDSA driver implementations may have hardware or software limitations on
  * how many clients can simultaneously perform operations. This status code is
  * returned if the mutual exclusion mechanism signals that an operation cannot
  * currently be performed because the hash module is unavailable.
  */
#define EDDSA_STATUS_HASH_UNAVAILABLE (-3)

 /*!
  * @brief   An error status code returned if the public-key accelerator
  * hardware or software resource is currently unavailable.
  *
  * EDDSA driver implementations may have hardware or software limitations on
  * how many clients can simultaneously perform operations. This status code is
  * returned if the mutual exclusion mechanism signals that an operation cannot
  * currently be performed because the PKA module is unavailable.
  */
#define EDDSA_STATUS_PKA_UNAVAILABLE (-4)

 /*!
  * @brief   The generated public key was the point at infinity.
  *
  * The point at infinity is not a valid public key, try
  * EDDSA_generatePublicKey again.
  */
#define EDDSA_STATUS_POINT_AT_INFINITY (-5)

 /*!
  * @brief   The private key was not 32 bytes long.
  *
  * Ed25519 expects a 32 byte private key.
  */
#define EDDSA_STATUS_INVALID_PRIVATE_KEY_SIZE (-6)

 /*!
  * @brief   The public key was not 32 bytes long.
  *
  * Ed25519 expects a 32 byte public key. This is the compressed representation
  * of a point on curve Ed25519.
  */
#define EDDSA_STATUS_INVALID_PUBLIC_KEY_SIZE (-7)

 /*!
  * @brief   The public key was not a valid point on curve Ed25519.
  *
  * Ed25519 expects a valid public key. This is the compressed representation
  * of a point on curve Ed25519.
  */
#define EDDSA_STATUS_PUBLIC_KEY_NOT_ON_CURVE (-8)

/*!
 * @brief   The Ed25519 signature component R was not 32 bytes long.
 *
 * Signature component R must be 32 bytes long, representing a point on
 * Ed25519.
 */
#define EDDSA_STATUS_INVALID_R_SIZE (-9)

 /*!
  * @brief   The Ed25519 signature component S was not 32 bytes long.
  *
  * Signature component S must be 32 bytes long, representing a scalar.
  */
#define EDDSA_STATUS_INVALID_S_SIZE (-10)

/*!
 * @brief   The Ed25519 signature component S was larger than the order of
 *          Ed25519.
 *
 * The signature component S must be less than the order of the elliptic curve
 * Ed25519.
 */
#define EDDSA_STATUS_S_LARGER_THAN_ORDER (-11)

/*!
 * @brief   The Ed25519 operation was called with conflicting private and
 *          public key ID parameters.
 *
 * The private and public key params have different ECC parameters.
 */
#define EDDSA_STATUS_KEY_ID_PARAM_MISMATCH (-12)

/*!
 * @brief   The KeyStore module could not find a key with the given ID.
 *
 * The KeyStore was provided an invalid ID#.
 */
#define EDDSA_STATUS_KEYSTORE_INVALID_ID (-13)

/*!
 * @brief   The KeyStore module entered a failure state when retrieving the key
 *          ID.
 *
 * A KeyStore operation failed as a result of an invalid key type, invalid key
 * policy, not enough buffer space, hardware failure, and so on.
 */
#define EDDSA_STATUS_KEYSTORE_GENERIC_FAILURE (-14)

 /*!
  * @brief   The SHA2 module returned an error while hashing.
  *
  * The SHA2 module did not successfully hash a required value.
  */
 #define EDDSA_STATUS_SHA2_HASH_FAILURE (-15)

/*!
 *  @brief EDDSA Global configuration
 *
 *  The EDDSA_Config structure contains a set of pointers used to characterize
 *  the EDDSA driver implementation.
 *
 *  This structure needs to be defined before calling EDDSA_init() and it must
 *  not be changed thereafter.
 *
 *  @sa     EDDSA_init()
 */
typedef struct {
    /*! Pointer to a driver specific data object */
    void               *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void         const *hwAttrs;
} EDDSA_Config;

/*!
 *  @brief  A handle that is returned from an EDDSA_open() call.
 */
typedef EDDSA_Config *EDDSA_Handle;

/*!
 * @brief   The way in which EDDSA function calls return after performing an
 * encryption + authentication or decryption + verification operation.
 *
 * Not all EDDSA operations exhibit the specified return behavior. Functions
 * that do not require significant computation and cannot offload that
 * computation to a background thread behave like regular functions. Which
 * functions exhibit the specified return behavior is implementation dependent.
 * Specifically, a software-backed implementation run on the same CPU as the
 * application will emulate the return behavior while not actually offloading
 * the computation to the background thread.
 *
 * EDDSA functions exhibiting the specified return behavior have restrictions
 * on the context from which they may be called.
 *
 * |                                | Task  | Hwi   | Swi   |
 * |--------------------------------|-------|-------|-------|
 * |EDDSA_RETURN_BEHAVIOR_CALLBACK  | X     | X     | X     |
 * |EDDSA_RETURN_BEHAVIOR_BLOCKING  | X     |       |       |
 * |EDDSA_RETURN_BEHAVIOR_POLLING   | X     | X     | X     |
 *
 */
typedef enum {
    EDDSA_RETURN_BEHAVIOR_CALLBACK = 1,
    /*!< The function call will return immediately while the EDDSA operation
     *   goes on in the background. The registered callback function is called
     *   after the operation completes. The context the callback function is
     *   called (task, HWI, SWI) is implementation-dependent.
     */

    EDDSA_RETURN_BEHAVIOR_BLOCKING = 2,
    /*!< The function call will block while EDDSA operation goes on in the
     *   background. EDDSA operation results are available after the function
     *   returns.
     */

    EDDSA_RETURN_BEHAVIOR_POLLING  = 4,
    /*!< The function call will continuously poll a flag while EDDSA operation
     *   goes on in the background. EDDSA operation results are available after
     *   the function returns.
     */

} EDDSA_ReturnBehavior;

/*!
 *  @brief  Struct containing the parameters required for generating an EdDSA
 *  private-public keypair.
 */
typedef struct {
    const ECCParams_CurveParams     *curve;
    /*!< A pointer to the elliptic curve parameters */

    const CryptoKey                 *myPrivateKey;
    /*!< A pointer to the randomly generated randomly private key "k" in little
     *   endian. Must be 32 bytes for Ed25519.
     */

    CryptoKey                       *myPublicKey;
    /*!< A pointer public EdDSA key A = s*B in compressed public key format.
     *   Must be 32 bytes for Ed25519.
     */

} EDDSA_OperationGeneratePublicKey;

/*!
 *  @brief  Struct containing the parameters required for generating an EdDSA
 *  digital signature.
 */
typedef struct {
    const ECCParams_CurveParams     *curve;
    /*!< A pointer to the elliptic curve parameters */

    const CryptoKey                 *myPrivateKey;
    /*!< A pointer to the randomly generated private key "k" in little endian.
     *   Must be 32 bytes for Ed25519.
     */

    const CryptoKey                 *myPublicKey;
    /*!< A pointer public EdDSA key A = s*B in compressed public key format.
     *   Must be 32 bytes for Ed25519.
     */

    const uint8_t                   *preHashedMessage;
    /*!< A pointer to the  (prehashed) message in little endian. In the
     *   PureEdDSA scheme, the prehash function is the identity, PH(M) = M.
     */

    size_t                          preHashedMessageLength;
    /*!< Length of the message buffer in bytes. */

    uint8_t                         *R;
    /*!< Signature component R = r*B in little endian.
     *   Must be 32 bytes for Ed25519.
     */

    uint8_t                         *S;
    /*!< Signature component S = r + x*s mod n in little endian.
     *   Must be 32 bytes for Ed25519.
     */

} EDDSA_OperationSign;

/*!
 *  @brief  Struct containing the parameters required for verifying an EdDSA
 *          digital signature.
 */
typedef struct {
    const ECCParams_CurveParams     *curve;
    /*!< A pointer to the elliptic curve parameters */

    const CryptoKey                 *theirPublicKey;
    /*!< A pointer to the signer's public EdDSA key A = s*B in compressed
     *   public key format. Must be 32  bytes for Ed25519 and a valid point on
     *   Ed25519.
     */

    const uint8_t                   *preHashedMessage;
    /*!< A pointer to the (prehashed) message in little endian. In the
     *   PureEdDSA scheme, the prehash function is the identity, PH(M) = M.
     */

    size_t                          preHashedMessageLength;
    /*!< Length of the message buffer in bytes. */

    const uint8_t                   *R;
    /*!< Signature component R to verify in little endian.
     *   Must be 32 bytes for Ed25519.
     */

    const uint8_t                   *S;
    /*!< Signature component S to verify in little endian.
     * Must be 32 bytes for Ed25519.
     */

} EDDSA_OperationVerify;

/*!
 *  @brief  Union containing pointers to all supported operation structs.
 */
typedef union {
    /*!< A pointer to an EDDSA_OperationGeneratePublicKey struct */
    EDDSA_OperationGeneratePublicKey   *generatePublicKey;
    /*!< A pointer to an EDDSA_OperationSign struct */
    EDDSA_OperationSign                *sign;
    /*!< A pointer to an EDDSA_OperationVerify struct */
    EDDSA_OperationVerify              *verify;
} EDDSA_Operation;

/*!
 *  @brief  Enum for the operation types supported by the driver.
 */
typedef enum {
    EDDSA_OPERATION_TYPE_GENERATE_PUBLIC_KEY = 1,
    EDDSA_OPERATION_TYPE_SIGN = 2,
    EDDSA_OPERATION_TYPE_VERIFY = 3,
} EDDSA_OperationType;

/*!
 *  @brief  The definition of a callback function used by the EDDSA driver
 *          when used in ::EDDSA_RETURN_BEHAVIOR_CALLBACK
 *
 *  @param  handle Handle of the client that started the EDDSA operation.
 *
 *  @param  returnStatus The result of the EDDSA operation. May contain an
 *                       error code if the result is the point at infinity for
 *                       example.
 *
 *  @param  operation A union of pointers to operation structs. Only one type
 *          of pointer is valid per call to the callback function. Which type
 *          is currently valid is determined by /c operationType. The union
 *          allows easier access to the struct's fields without the need to
 *          typecase the result.
 *
 *  @param  operationType This parameter determined which operation the
 *          callback refers to and which type to access through \c operation.
 */
typedef void (*EDDSA_CallbackFxn) (EDDSA_Handle handle,
                                   int_fast16_t returnStatus,
                                   EDDSA_Operation operation,
                                   EDDSA_OperationType operationType);

/*!
 *  @brief  EDDSA Parameters
 *
 *  EDDSA Parameters are used to with the EDDSA_open() call. Default values for
 *  these parameters are set using EDDSA_Params_init().
 *
 *  @sa     EDDSA_Params_init()
 */
typedef struct {
    EDDSA_ReturnBehavior    returnBehavior;
    /*!< Blocking, callback, or polling return behavior */

    EDDSA_CallbackFxn       callbackFxn;
    /*!< Callback function pointer */

    uint32_t                timeout;
    /*!< Timeout in system ticks before the operation fails and returns */


    void                   *custom;
    /*!< Custom argument used by driver implementation */

} EDDSA_Params;

/*!
 *  @brief  This function initializes the EDDSA module.
 *
 *  @pre    The EDDSA_config structure must exist and be persistent before this
 *          function can be called. This function must also be called before
 *          any other EDDSA driver APIs. This function call does not modify any
 *          peripheral registers.
 */
void EDDSA_init(void);

/*!
 *  @brief  Function to close an EDDSA peripheral specified by the EDDSA handle
 *
 *  @pre    EDDSA_open() has to be called first.
 *
 *  @param  handle An EDDSA handle returned from EDDSA_open()
 *
 *  @sa     EDDSA_open()
 */
void EDDSA_close(EDDSA_Handle handle);

/*!
 *  @brief  This function opens a given EDDSA peripheral.
 *
 *  @pre    EDDSA controller has been initialized using EDDSA_init()
 *
 *  @param  index         Logical peripheral number for the EDDSA indexed into
 *                        the EDDSA_config table
 *
 *  @param  params        Pointer to an parameter block, if NULL it will use
 *                        default values.
 *
 *  @return An EDDSA_Handle on success or a NULL on an error or if it has been
 *          opened already.
 *
 *  @sa     EDDSA_init()
 *  @sa     EDDSA_close()
 */
EDDSA_Handle EDDSA_open(uint_least8_t index, const EDDSA_Params *params);

/*!
 *  @brief  Function to initialize the EDDSA_Params struct to its defaults
 *
 *  @param  params      An pointer to EDDSA_Params structure for
 *                      initialization
 *
 *  Defaults values are:
 *      returnBehavior              = EDDSA_RETURN_BEHAVIOR_BLOCKING
 *      callbackFxn                 = NULL
 *      timeout                     = SemaphoreP_WAIT_FOREVER
 *      custom                      = NULL
 */
void EDDSA_Params_init(EDDSA_Params *params);

/*!
 *  @brief  Function to initialize an EDDSA_OperationGeneratePublicKey struct
 *          to its defaults
 *
 *  @param  operation   A pointer to EDDSA_OperationGeneratePublicKey structure
 *                      for initialization
 *
 *  Defaults to all zeros.
 */
void EDDSA_OperationGeneratePublicKey_init(EDDSA_OperationGeneratePublicKey
                                           *operation);

/*!
 *  @brief  Function to initialize an EDDSA_OperationSign struct to its
 *          defaults
 *
 *  @param  operation   A pointer to EDDSA_OperationSign structure for
 *                      initialization
 *
 *  Defaults to all zeros.
 */
void EDDSA_OperationSign_init(EDDSA_OperationSign *operation);

/*!
 *  @brief  Function to initialize an EDDSA_OperationSign struct to its
 *          defaults
 *
 *  @param  operation   An pointer to EDDSA_OperationSign structure for
 *                      initialization
 *
 *  Defaults to all zeros.
 */
void EDDSA_OperationVerify_init(EDDSA_OperationVerify *operation);

/*!
 *  @brief Generates an EdDSA private-public keypair.
 *
 *  EDDSA_generatePublicKey() generates a private-public keypair (\c k, \c A)
 *  to generate EdDSA signatures with.
 *
 *  @pre    EDDSA_OperationSign_init() must be called on \c operation first.
 *          The driver must have been opened by calling EDDSA_open() first.
 *
 *  @param [in]     handle          An EDDSA handle returned from EDDSA_open()
 *
 *  @param [in]     operation       A struct containing the pointers to the
 *                                  buffers necessary to perform the operation
 *  @sa EDDSA_sign()
 *  @sa EDDSA_verify()
 *
 *  @retval #EDDSA_STATUS_SUCCESS                   The operation succeeded.
 *  @retval #EDDSA_STATUS_ERROR                     The operation failed.
 *  @retval #EDDSA_STATUS_CANCELED                  The operation was canceled.
 *  @retval #EDDSA_STATUS_HASH_UNAVAILABLE          The required hash hardware
 *                                                  resource was not available.
 *                                                  Try again later.
 *  @retval #EDDSA_STATUS_PKA_UNAVAILABLE           The required PKA hardware
 *                                                  resource was not available.
 *                                                  Try again later.
 *  @retval #EDDSA_STATUS_POINT_AT_INFINITY         The computed public key is
 *                                                  the point at infinity.
 *                                                  Try again.
 *  @retval #EDDSA_STATUS_KEY_ID_PARAM_MISMATCH     The ID for the private and
 *                                                  public key parameters do
 *                                                  not match.
 *  @retval #EDDSA_STATUS_KEYSTORE_INVALID_ID       KeyStore could not find a
 *                                                  key with the given key ID#.
 *  @retval #EDDSA_STATUS_KEYSTORE_GENERIC_FAILURE  The KeyStore entered some
 *                                                  error state when storing
 *                                                  the public or private key.
 */
int_fast16_t EDDSA_generatePublicKey(EDDSA_Handle handle,
                                     EDDSA_OperationGeneratePublicKey *operation);

/*!
 *  @brief Generates an EdDSA signature.
 *
 *  EDDSA_sign() generates a signature (\c R, \c S) of a message.
 *
 *  @pre    EDDSA_OperationSign_init() must be called on \c operation first.
 *          The driver must have been opened by calling EDDSA_open() first.
 *
 *  @param [in]     handle          An EDDSA handle returned from EDDSA_open()
 *
 *  @param [in]     operation       A struct containing the pointers to the
 *                                  buffers necessary to perform the operation
 *  @sa EDDSA_generatePublicKey()
 *  @sa EDDSA_verify()
 *
 *  @retval #EDDSA_STATUS_SUCCESS                   The operation succeeded.
 *  @retval #EDDSA_STATUS_ERROR                     The operation failed.
 *  @retval #EDDSA_STATUS_CANCELED                  The operation was canceled.
 *  @retval #EDDSA_STATUS_HASH_UNAVAILABLE          The required hash hardware
 *                                                  resource was not available.
 *                                                  Try again later.
 *  @retval #EDDSA_STATUS_PKA_UNAVAILABLE           The required PKA hardware
 *                                                  resource was not available.
 *                                                  Try again later.
 *  @retval #EDDSA_STATUS_INVALID_PRIVATE_KEY_SIZE  The private key size is an
 *                                                  invalid size.
 *  @retval #EDDSA_STATUS_INVALID_PUBLIC_KEY_SIZE   The public key size is an
 *                                                  invalid size.
 *  @retval #EDDSA_STATUS_KEY_ID_PARAM_MISMATCH     The ID for the private and
 *                                                  public key parameters do
 *                                                  not match.
 *  @retval #EDDSA_STATUS_KEYSTORE_INVALID_ID       KeyStore could not find a
 *                                                  key with the given key ID#.
 *  @retval #EDDSA_STATUS_KEYSTORE_GENERIC_FAILURE  The KeyStore entered some
 *                                                  error state when retrieving
 *                                                  the public or private key.
 */
int_fast16_t EDDSA_sign(EDDSA_Handle handle, EDDSA_OperationSign *operation);

/*!
 *  @brief Verifies a received EdDSA signature matches a hash and public key
 *
 *  @pre    EDDSA_OperationVerify_init() must be called on \c operation first.
 *          The driver must have been opened by calling EDDSA_open() first.
 *
 *  @param [in]     handle          An EDDSA handle returned from EDDSA_open()
 *
 *  @param [in]     operation       A struct containing the pointers to the
 *                                  buffers necessary to perform the operation
 *
 *  @sa EDDSA_sign()
 *
 *  @retval #EDDSA_STATUS_SUCCESS                   The operation succeeded.
 *  @retval #EDDSA_STATUS_ERROR                     The operation failed.
 *  @retval #EDDSA_STATUS_CANCELED                  The operation was canceled.
 *  @retval #EDDSA_STATUS_HASH_UNAVAILABLE          The required hash hardware
 *                                                  resource was not available.
 *                                                  Try again later.
 *  @retval #EDDSA_STATUS_PKA_UNAVAILABLE           The required PKA hardware
 *                                                  resource was not available.
 *                                                  Try again later.
 *  @retval #EDDSA_STATUS_INVALID_PUBLIC_KEY_SIZE   The public key size is an
 *                                                  invalid size.
 *  @retval #EDDSA_STATUS_PUBLIC_KEY_NOT_ON_CURVE   The public key is not a
 *                                                  valid point.
 *  @retval #EDDSA_STATUS_INVALID_R_SIZE            The signature component R
 *                                                  is an invalid size.
 *  @retval #EDDSA_STATUS_INVALID_S_SIZE            The signature component S
 *                                                  is an invalid size.
 *  @retval #EDDSA_STATUS_S_LARGER_THAN_ORDER       The signature component S
 *                                                  is larger than the Edwards
 *                                                  curve order.
 *  @retval #EDDSA_STATUS_KEYSTORE_INVALID_ID       KeyStore could not find a
 *                                                  key with the given key ID#.
 *  @retval #EDDSA_STATUS_KEYSTORE_GENERIC_FAILURE  The KeyStore entered some
 *                                                  error state when retrieving
 *                                                  the public or private key.
 */
int_fast16_t EDDSA_verify(EDDSA_Handle handle,
                          EDDSA_OperationVerify *operation);

/*!
 *  @deprecated This function will be deprecated in the 3Q20 SDK release. The
 *  asynchronicity of the function can not be handled by all accelerators.
 *
 *  @brief Cancels an ongoing EDDSA operation.
 *
 *  Asynchronously cancels an EDDSA operation. Only available when using
 *  EDDSA_RETURN_BEHAVIOR_CALLBACK or EDDSA_RETURN_BEHAVIOR_BLOCKING.
 *  The operation will terminate as though an error occurred. The
 *  return status code of the operation will be EDDSA_STATUS_CANCELED.
 *
 *  @param  handle Handle of the operation to cancel
 *
 *  @retval #EDDSA_STATUS_SUCCESS               The operation was canceled.
 *  @retval #EDDSA_STATUS_ERROR                 The operation was not canceled.
 *                                              There may be no operation to
 *                                              cancel.
 */
int_fast16_t EDDSA_cancelOperation(EDDSA_Handle handle);

/**
 *  @brief  Constructs a new EDDSA object
 *
 *  Unlike #EDDSA_open(), #EDDSA_construct() does not require the hwAttrs and
 *  object to be allocated in a #EDDSA_Config array that is indexed into.
 *  Instead, the #EDDSA_Config, hwAttrs, and object can be allocated at any
 *  location. This allows for relatively simple run-time allocation of
 *  temporary driver instances on the stack or the heap.
 *  The drawback is that this makes it more difficult to write device-agnostic
 *  code. If you use an ifdef with DeviceFamily, you can choose the correct
 *  object and hwAttrs to allocate. That compilation unit will be tied to the
 *  device it was compiled for at this point. To change devices, recompilation
 *  of the application with a different DeviceFamily setting is necessary.
 *
 *  @param  config #EDDSA_Config describing the location of the object
 *                 and hwAttrs.
 *
 *  @param  params #EDDSA_Params to configure the driver instance.
 *
 *  @return        Returns a #EDDSA_Handle on success or NULL on failure.
 *
 *  @pre    The object struct @c config points to must be zeroed out prior to
 *          calling this function. Otherwise, unexpected behavior may ensue.
 */
EDDSA_Handle EDDSA_construct(EDDSA_Config *config, const EDDSA_Params *params);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_EDDSA__include */
