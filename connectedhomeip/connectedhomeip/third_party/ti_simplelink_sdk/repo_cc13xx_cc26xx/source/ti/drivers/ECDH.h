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
 * @file ECDH.h
 *
 * @brief TI Driver for Elliptic Curve Diffie-Hellman key agreement scheme.
 *
 * @anchor ti_drivers_ECDH_Overview
 * # Overview #
 *
 * Elliptic Curve Diffie-Hellman (ECDH) is a key agreement scheme between
 * two parties based on the Diffie-Hellman key exchange protocol.
 *
 * It provides a means of generating a shared secret and derived symmetric key
 * between the two parties over an insecure channel.
 *
 * It does not provide authentication. As such, it does not guarantee that the
 * party you are exchanging keys with is truly the party you wish to establish a
 * secured channel with.
 *
 * The two parties each generate a private key and a public key. The private key
 * is a random integer in the interval [1, n - 1], where n is the order of a
 * previously agreed upon curve. The public key is generated
 * by multiplying the private key by the generator point of a previously agreed
 * upon elliptic curve such as NISTP256 or Curve 25519. The public key is itself
 * a point upon the elliptic curve. Each public key is then transmitted to the
 * other party over a potentially insecure channel. The other party's public key
 * is then multiplied with the private key, generating a shared secret. This
 * shared secret is also a point on the curve. However, the entropy in the secret
 * is not spread evenly throughout the shared secret. In order to generate one or more
 * shared symmetric keys, the shared secret must be run through a key derivation
 * function (KDF) that was previously agreed upon. Usually, only the X coordinate
 * is processed in this way as it contains all the entropy of the shared secret and
 * some curve implementations only provide the X coordinate. The key derivation function
 * can take many forms, from simply hashing the X coordinate of the shared secret
 * with SHA2 and truncating the result to generating multiple symmetric keys with
 * HKDF, an HMAC based KDF.
 *
 * Key derivation functions in the context of symmetric key generation after
 * elliptic curve based key exchange differ from KDFs used to generate keys from
 * passwords a user provides in a login. Those KDFs such as bcrypt purposefully
 * add additional computation to increase a system's resistance against brute
 * force or dictionary attacks.
 *
 * @anchor ti_drivers_ECDH_Usage
 * # Usage #
 *
 * ## Before starting an ECDH operation #
 *
 * Before starting an ECDH operation, the application must do the following:
 *      - Call ECDH_init() to initialize the driver
 *      - Call ECDH_Params_init() to initialize the ECDH_Params to default values.
 *      - Modify the ECDH_Params as desired
 *      - Call ECDH_open() to open an instance of the driver
 *
 * ## Generating your public-private key pair #
 * To generate a public-private key pair for an agreed upon curve, the application
 * must do the following:
 *      - Generate the keying material for the private key. This keying material must
 *        be an integer in the interval [1, n - 1], where n is the order of the curve.
 *        It should be stored in an array with the least significant byte of the integer
 *        hex representation stored in the highest address of the array (big-endian).
 *        The array should be the same length as the curve parameters of the curve used.
 *        The driver validates private keys against the provided curve by default.
 *      - Initialize the private key CryptoKey. CryptoKeys are opaque data structures and representations
 *        of keying material and its storage. Depending on how the keying material
 *        is stored (RAM or flash, key store), the CryptoKey must be
 *        initialized differently. The ECDH API can handle all types of CryptoKey.
 *        However, not all device-specific implementations support all types of CryptoKey.
 *        Devices without a key store will not support CryptoKeys with keying material
 *        stored in a key store for example.
 *        All devices support plaintext CryptoKeys.
 *      - Initialize a blank CryptoKey for the public key. The CryptoKey will keep track
 *        of where the keying material for the public key should be copied and how
 *        long it is. It should have twice the length of the private key plus one
 *        for SEC 1-based octet string format public keys or the length of the private
 *        key for RFC 7748 Montgomery curve X-only public keys.
 *      - Initialize the ECDH_OperationGeneratePublicKey struct and then populate it. By
 *        default, octet string public keys will be generated.
 *      - If using RFC 7748-style public keys, initialize the operation's public key
 *        data format to be ECDH_PUBLIC_KEY_DATA_FORMAT_MONTGOMERY_X_ONLY.
 *      - Call ECDH_generatePublicKey(). The generated keying material will be copied
 *        according the the CryptoKey passed in as the public key parameter. The CryptoKey
 *        will no longer be considered 'blank' after the operation.
 *
 * ## Calculating a shared secret #
 * After trading public keys with the other party, the application should do the following
 * to calculate the shared secret:
 *      - Initialize a CryptoKey as public key with the keying material received from the other
 *        party.
 *      - Initialize the private key CryptoKey with the key used to generate your
 *        public key. CryptoKeys are opaque data structures and representations
 *        of keying material and its storage. Depending on how the keying material
 *        is stored (RAM or flash, key store), the CryptoKey must be
 *        initialized differently. The ECDH API can handle all types of CryptoKey.
 *        However, not all device-specific implementations support all types of CryptoKey.
 *        Devices without a key store will not support CryptoKeys with keying material
 *        stored in a key store for example.
 *        All devices support plaintext CryptoKeys.
 *      - Initialize a blank CryptoKey with the same size as the previously initialized
 *        public key.
 *      - Initialize the ECDH_OperationComputeSharedSecret struct and then populate it. By
 *        default, octet string public keys will be assumed and octet string shared secrets
 *        will be generated.
 *      - If importing RFC 7748-style public keys, initialize the operation's public key
 *        data format to be ECDH_PUBLIC_KEY_DATA_FORMAT_MONTGOMERY_X_ONLY.
 *      - If generating RFC 7748-style shared secrets, initialize the operation's shared secret
 *        data format to be ECDH_PUBLIC_KEY_DATA_FORMAT_MONTGOMERY_X_ONLY.
 *      - Call ECDH_computeSharedSecret(). The shared secret will be copied to a location
 *        according to the shared secret CryptoKey passed to the function call. The driver
 *        will validate the supplied public key and reject invalid ones.
 *
 * ## Creating one or more symmetric keys from the shared secret #
 * After calculating the shared secret between the application and the other party,
 * the entropy in the shared secret must be evened out and stretched as needed. There are
 * uncountable methods and algorithms to stretch an original seed entropy (the share secret)
 * to generate symmetric keys.
 *      - Run the X coordinate of the resulting entropy through a key derivation function (KDF)
 *
 * ## After a key exchange #
 * After the ECDH key exchange completes, the application should either start another operation
 * or close the driver by calling ECDH_close()
 *
 * ## General usage #
 * The API expects elliptic curves as defined in ti/drivers/cryptoutils/ecc/ECCParams.h.
 * Several commonly used curves are provided. Check the device-specific ECDH documentation
 * for curve type (short Weierstrass, Montgomery, Edwards) support for your device.
 * ECDH support for a curve type on a device does not imply curve-type support for
 * other ECC schemes.
 *
 * ## Key Formatting
 * By default, the ECDH API expects the private and public keys to be formatted in octet
 * string format. The details of octet string formatting can be found in
 * SEC 1: Elliptic Curve Cryptography.
 *
 * Private keys are formatted as big-endian integers of the same length as the
 * curve length.
 *
 * Public keys and shared secrets are points on an elliptic curve. These points can
 * be expressed in several ways. The most common one is in affine coordinates as an
 * X,Y pair.
 * This API uses points expressed in uncompressed affine coordinates by default.
 * The octet string format requires a formatting byte in the first byte of the
 * public key. When using uncompressed affine coordinates, this is the value
 * 0x04.
 * The point itself is stored as a concatenated array of X followed by Y.
 * X and Y are big-endian. Some implementations do not require or yield
 * the Y coordinate for ECDH on certain curves. It is recommended that the full
 * keying material buffer of twice the curve param length is used to facilitate
 * code-reuse. Implementations that do not use the Y coordinate will zero-out
 * the Y-coordinate whenever they write a point to the CryptoKey.
 *
 * If device-supported, Montgomery curves can be stored as their X-only format
 * based on the RFC-7748 specification. Here, only the X coordinate is packed
 * in little-endian integers of the same length as the curve length.
 *
 * This API accepts and returns the keying material of public keys according
 * to the following table:
 *
 * | Curve Type         | Keying Material Array | Array Length               |
 * |--------------------|-----------------------|----------------------------|
 * | Short Weierstrass  | [0x04, X, Y]          | 1 + 2 * Curve Param Length |
 * | Montgomery         | [0x04, X, Y]          | 1 + 2 * Curve Param Length |
 * | Montgomery         | [X]                   | Curve Param Length         |
 * | Edwards            | [0x04, X, Y]          | 1 + 2 * Curve Param Length |
 *
 * Note: This driver will automatically prune the private key according to
 *       RFC 7748 for the following curve:
 *           X25519
 *
 * @anchor ti_drivers_ECDH_Synopsis
 * ## Synopsis
 * @anchor ti_drivers_ECDH_Synopsis_Code
 * @code
 * // Import ECDH Driver definitions
 * #include <ti/drivers/ECDH.h>
 *
 * ECDH_init();
 *
 * // Since we are using default ECDH_Params, we just pass in NULL for that parameter.
 * ecdhHandle = ECDH_open(0, NULL);
 *
 * // Initialize myPrivateKey and myPublicKey
 * CryptoKeyPlaintext_initKey(&myPrivateKey, myPrivateKeyingMaterial, sizeof(myPrivateKeyingMaterial));
 * CryptoKeyPlaintext_initBlankKey(&myPublicKey, myPublicKeyingMaterial, sizeof(myPublicKeyingMaterial));
 *
 * ECDH_OperationGeneratePublicKey_init(&operationGeneratePublicKey);
 * operationGeneratePublicKey.curve                 = &ECCParams_NISTP256;
 * operationGeneratePublicKey.myPrivateKey          = &myPrivateKey;
 * operationGeneratePublicKey.myPublicKey           = &myPublicKey;
 *
 * // Generate the keying material for myPublicKey and store it in myPublicKeyingMaterial
 * operationResult = ECDH_generatePublicKey(ecdhHandle, &operationGeneratePublicKey);
 *
 * // Now send the content of myPublicKeyingMaterial to the other party,
 * // receive their public key, and copy their public keying material to theirPublicKeyingMaterial
 *
 * // Initialize their public CryptoKey and the shared secret CryptoKey
 * CryptoKeyPlaintext_initKey(&theirPublicKey, theirPublicKeyingMaterial, sizeof(theirPublicKeyingMaterial));
 * CryptoKeyPlaintext_initBlankKey(&sharedSecret, sharedSecretKeyingMaterial, sizeof(sharedSecretKeyingMaterial));
 *
 * // The ECC_NISTP256 struct is provided in ti/drivers/types/EccParams.h and the corresponding device-specific implementation
 * ECDH_OperationComputeSharedSecret_init(&operationComputeSharedSecret);
 * operationComputeSharedSecret.curve                      = &ECCParams_NISTP256;
 * operationComputeSharedSecret.myPrivateKey               = &myPrivateKey;
 * operationComputeSharedSecret.theirPublicKey             = &theirPublicKey;
 * operationComputeSharedSecret.sharedSecret               = &sharedSecret;
 *
 * // Compute the shared secret and copy it to sharedSecretKeyingMaterial
 * operationResult = ECDH_computeSharedSecret(ecdhHandle, &operationComputeSharedSecret);
 *
 * // Close the driver
 * ECDH_close(ecdhHandle);
 *
 * @endcode
 *
 * ## Synopsis for X25519 X-only key exchange
 * @anchor ti_drivers_ECDH_X25519_Code
 * @code
 * // Import ECDH Driver definitions
 * #include <ti/drivers/ECDH.h>
 *
 * ECDH_init();
 *
 * // Since we are using default ECDH_Params, we just pass in NULL for that parameter.
 * ecdhHandle = ECDH_open(0, NULL);
 *
 * // Initialize myPrivateKey and myPublicKey
 * CryptoKeyPlaintext_initKey(&myPrivateKey, myPrivateKeyingMaterial, sizeof(myPrivateKeyingMaterial));
 * // Note that the public key size is only 32 bytes
 * CryptoKeyPlaintext_initBlankKey(&myPublicKey, myPublicKeyingMaterial, sizeof(myPublicKeyingMaterial));
 *
 * ECDH_OperationGeneratePublicKey_init(&operationGeneratePublicKey);
 * operationGeneratePublicKey.curve                 = &ECCParams_Curve25519;
 * operationGeneratePublicKey.myPrivateKey          = &myPrivateKey;
 * operationGeneratePublicKey.myPublicKey           = &myPublicKey;
 * // If generating an RFC 7748 X-Only formatted key, we use the following public key data format:
 * operationGeneratePublicKey.publicKeyDataFormat   = ECDH_PUBLIC_KEY_DATA_FORMAT_MONTGOMERY_X_ONLY;
 *
 * // Generate the keying material for myPublicKey and store it in myPublicKeyingMaterial
 * operationResult = ECDH_generatePublicKey(ecdhHandle, &operationGeneratePublicKey);
 *
 * // Now send the content of myPublicKeyingMaterial to the other party,
 * // receive their public key, and copy their public keying material to theirPublicKeyingMaterial
 *
 * // Initialize their public CryptoKey and the shared secret CryptoKey
 * CryptoKeyPlaintext_initKey(&theirPublicKey, theirPublicKeyingMaterial, sizeof(theirPublicKeyingMaterial));
 * CryptoKeyPlaintext_initBlankKey(&sharedSecret, sharedSecretKeyingMaterial, sizeof(sharedSecretKeyingMaterial));
 *
 * // The ECC_NISTP256 struct is provided in ti/drivers/types/EccParams.h and the corresponding device-specific implementation
 * ECDH_OperationComputeSharedSecret_init(&operationComputeSharedSecret);
 * operationComputeSharedSecret.curve                      = &ECCParams_Curve25519;
 * operationComputeSharedSecret.myPrivateKey               = &myPrivateKey;
 * operationComputeSharedSecret.theirPublicKey             = &theirPublicKey;
 * operationComputeSharedSecret.sharedSecret               = &sharedSecret;
 * // If receiving an RFC 7748 X-Only formatted key, we specify the input format:
 * operationComputeSharedSecret.publicKeyDataFormat        = ECDH_PUBLIC_KEY_DATA_FORMAT_MONTGOMERY_X_ONLY;
 * // If agreeing on an RFC 7748 X-Only formatted shared secret, we specify the shared secret format
 * operationComputeSharedSecret.sharedSecretDataFormat     = ECDH_PUBLIC_KEY_DATA_FORMAT_MONTGOMERY_X_ONLY;
 *
 * // Compute the shared secret and copy it to sharedSecretKeyingMaterial
 * operationResult = ECDH_computeSharedSecret(ecdhHandle, &operationComputeSharedSecret);
 *
 * // Close the driver
 * ECDH_close(ecdhHandle);
 *
 * @endcode
 *
 * @anchor ti_drivers_ECDH_Examples
 * # Examples #
 *
 * ## ECDH exchange with plaintext CryptoKeys #
 *
 * @code
 *
 * #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
 * #include <ti/drivers/ECDH.h>
 *
 * #define CURVE_LENGTH 32
 *
 * ...
 *
 * // Our private key is 0x0000000000000000000000000000000000000000000000000000000000000001
 * // In practice, this value should come from a TRNG, PRNG, PUF, or device-specific pre-seeded key
 * uint8_t myPrivateKeyingMaterial[CURVE_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 *                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 *                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 *                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
 * uint8_t myPublicKeyingMaterial[2 * CURVE_LENGTH + 1] = {0};
 * uint8_t theirPublicKeyingMaterial[2 * CURVE_LENGTH + 1] = {0};
 * uint8_t sharedSecretKeyingMaterial[2 * CURVE_LENGTH + 1] = {0};
 * uint8_t symmetricKeyingMaterial[16] = {0};
 *
 * CryptoKey myPrivateKey;
 * CryptoKey myPublicKey;
 * CryptoKey theirPublicKey;
 * CryptoKey sharedSecret;
 * CryptoKey symmetricKey;
 *
 * ECDH_Handle ecdhHandle;
 *
 * int_fast16_t operationResult;
 *
 * ECDH_OperationGeneratePublicKey operationGeneratePublicKey;
 *
 * // Since we are using default ECDH_Params, we just pass in NULL for that parameter.
 * ecdhHandle = ECDH_open(0, NULL);
 *
 * if (!ecdhHandle) {
 *     // Handle error
 * }
 *
 * // Initialize myPrivateKey and myPublicKey
 * CryptoKeyPlaintext_initKey(&myPrivateKey, myPrivateKeyingMaterial, sizeof(myPrivateKeyingMaterial));
 * CryptoKeyPlaintext_initBlankKey(&myPublicKey, myPublicKeyingMaterial, sizeof(myPublicKeyingMaterial));
 *
 * ECDH_OperationGeneratePublicKey_init(&operationGeneratePublicKey);
 * operationGeneratePublicKey.curve                 = &ECCParams_NISTP256;
 * operationGeneratePublicKey.myPrivateKey          = &myPrivateKey;
 * operationGeneratePublicKey.myPublicKey           =  &myPublicKey;
 *
 * // Generate the keying material for myPublicKey and store it in myPublicKeyingMaterial
 * operationResult = ECDH_generatePublicKey(ecdhHandle, &operationGeneratePublicKey);
 *
 * if (operationResult != ECDH_STATUS_SUCCESS) {
 *     // Handle error
 * }
 *
 * // Now send the content of myPublicKeyingMaterial to the other party,
 * // receive their public key, and copy their public keying material and the
 * // 0x04 byte to theirPublicKeyingMaterial
 *
 * // Initialise their public CryptoKey and the shared secret CryptoKey
 * CryptoKeyPlaintext_initKey(&theirPublicKey, theirPublicKeyingMaterial, sizeof(theirPublicKeyingMaterial));
 * CryptoKeyPlaintext_initBlankKey(&sharedSecret, sharedSecretKeyingMaterial, sizeof(sharedSecretKeyingMaterial));
 *
 * // The ECC_NISTP256 struct is provided in ti/drivers/types/EccParams.h and the corresponding device-specific implementation
 * ECDH_OperationComputeSharedSecret_init(&operationComputeSharedSecret);
 * operationComputeSharedSecret.curve                      = &ECCParams_NISTP256;
 * operationComputeSharedSecret.myPrivateKey               = &myPrivateKey;
 * operationComputeSharedSecret.theirPublicKey             = &theirPublicKey;
 * operationComputeSharedSecret.sharedSecret               = &sharedSecret;
 *
 * // Compute the shared secret and copy it to sharedSecretKeyingMaterial
 * operationResult = ECDH_computeSharedSecret(ecdhHandle, &operationComputeSharedSecret);
 *
 * if (operationResult != ECDH_STATUS_SUCCESS) {
 *     // Handle error
 * }
 *
 * CryptoKeyPlaintext_initBlankKey(&symmetricKey, symmetricKeyingMaterial, sizeof(symmetricKeyingMaterial));
 *
 * // Set up a KDF such as HKDF and open the requisite cryptographic primitive driver to implement it
 * // HKDF and SHA2 were chosen as an example and may not be available directly
 *
 * // At this point, you and the other party have both created the content within symmetricKeyingMaterial without
 * // someone else listening to your communication channel being able to do so
 *
 * @endcode
 *
 *
 */


#ifndef ti_drivers_ECDH__include
#define ti_drivers_ECDH__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>
#include <ti/drivers/cryptoutils/ecc/ECCParams.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Common ECDH status code reservation offset.
 * ECC driver implementations should offset status codes with
 * ECDH_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define ECCXYZ_STATUS_ERROR0    ECDH_STATUS_RESERVED - 0
 * #define ECCXYZ_STATUS_ERROR1    ECDH_STATUS_RESERVED - 1
 * #define ECCXYZ_STATUS_ERROR2    ECDH_STATUS_RESERVED - 2
 * @endcode
 */
#define ECDH_STATUS_RESERVED        (-32)

/*!
 * @brief   Successful status code.
 *
 * Functions return ECDH_STATUS_SUCCESS if the function was executed
 * successfully.
 */
#define ECDH_STATUS_SUCCESS         (0)

/*!
 * @brief   Generic error status code.
 *
 * Functions return ECDH_STATUS_ERROR if the function was not executed
 * successfully.
 */
#define ECDH_STATUS_ERROR           (-1)

/*!
 * @brief   An error status code returned if the hardware or software resource
 * is currently unavailable.
 *
 * ECC driver implementations may have hardware or software limitations on how
 * many clients can simultaneously perform operations. This status code is returned
 * if the mutual exclusion mechanism signals that an operation cannot currently be performed.
 */
#define ECDH_STATUS_RESOURCE_UNAVAILABLE (-2)

/*!
 * @brief   The result of the operation is the point at infinity.
 *
 * The operation yielded the point at infinity on this curve. This point is
 * not permitted for further use in ECC operations.
 */
#define ECDH_STATUS_POINT_AT_INFINITY (-3)

/*!
 * @brief   The private key passed in is larger than the order of the curve.
 *
 * Private keys must be integers in the interval [1, n - 1], where n is the
 * order of the curve.
 */
#define ECDH_STATUS_PRIVATE_KEY_LARGER_EQUAL_ORDER (-4)

/*!
 * @brief   The private key passed in is zero.
 *
 * Private keys must be integers in the interval [1, n - 1], where n is the
 * order of the curve.
 */
#define ECDH_STATUS_PRIVATE_KEY_ZERO (-5)

/*!
 * @brief   The public key of the other party does not lie upon the curve.
 *
 * The public key received from the other party does not lie upon the agreed upon
 * curve.
 */
#define ECDH_STATUS_PUBLIC_KEY_NOT_ON_CURVE (-6)

/*!
 * @brief   A coordinate of the public key of the other party is too large.
 *
 * A coordinate of the public key received from the other party is larger than
 * the prime of the curve. This implies that the point was not correctly
 * generated on that curve.
 */
#define ECDH_STATUS_PUBLIC_KEY_LARGER_THAN_PRIME (-7)

/*!
 *  @brief  The ongoing operation was canceled.
 */
#define ECDH_STATUS_CANCELED (-8)

/*!
 *  @brief  The provided CryptoKey does not match the expected size
 *
 *  The driver expects the private key to have the same length as other curve
 *  parameters and the public key to have a length of twice that plus one.
 *  If the provided CryptoKeys for the public and private keys do not match this
 *  scheme, this error will be returned.
 */
#define ECDH_STATUS_INVALID_KEY_SIZE (-9)

/*!
 *  @brief ECC Global configuration
 *
 *  The ECDH_Config structure contains a set of pointers used to characterize
 *  the ECC driver implementation.
 *
 *  This structure needs to be defined before calling ECDH_init() and it must
 *  not be changed thereafter.
 *
 *  @sa     ECDH_init()
 */
typedef struct {
    /*! Pointer to a driver specific data object */
    void               *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void         const *hwAttrs;
} ECDH_Config;

/*!
 *  @brief  A handle that is returned from an ECDH_open() call.
 */
typedef ECDH_Config *ECDH_Handle;

/*!
 * @brief   The way in which ECC function calls return after performing an
 * encryption + authentication or decryption + verification operation.
 *
 * Not all ECC operations exhibit the specified return behavor. Functions that do not
 * require significant computation and cannot offload that computation to a background thread
 * behave like regular functions. Which functions exhibit the specfied return behavior is not
 * implementation dependent. Specifically, a software-backed implementation run on the same
 * CPU as the application will emulate the return behavior while not actually offloading
 * the computation to the background thread.
 *
 * ECC functions exhibiting the specified return behavior have restrictions on the
 * context from which they may be called.
 *
 * |                                | Task  | Hwi   | Swi   |
 * |--------------------------------|-------|-------|-------|
 * |ECDH_RETURN_BEHAVIOR_CALLBACK   | X     | X     | X     |
 * |ECDH_RETURN_BEHAVIOR_BLOCKING   | X     |       |       |
 * |ECDH_RETURN_BEHAVIOR_POLLING    | X     | X     | X     |
 *
 */
typedef enum {
    ECDH_RETURN_BEHAVIOR_CALLBACK = 1,      /*!< The function call will return immediately while the
                                             *   ECC operation goes on in the background. The registered
                                             *   callback function is called after the operation completes.
                                             *   The context the callback function is called (task, HWI, SWI)
                                             *   is implementation-dependent.
                                             */
    ECDH_RETURN_BEHAVIOR_BLOCKING = 2,      /*!< The function call will block while ECC operation goes
                                             *   on in the background. ECC operation results are available
                                             *   after the function returns.
                                             */
    ECDH_RETURN_BEHAVIOR_POLLING  = 4,      /*!< The function call will continuously poll a flag while ECC
                                             *   operation goes on in the background. ECC operation results
                                             *   are available after the function returns.
                                             */
} ECDH_ReturnBehavior;


typedef enum {
    ECDH_PUBLIC_KEY_DATA_FORMAT_OCTET_STRING = 0,       /*!< Use standard NIST public key style (uncompressed) of the form
                                                         *   {0x4 || x || y} in big-endian.
                                                         */
    ECDH_PUBLIC_KEY_DATA_FORMAT_MONTGOMERY_X_ONLY = 1,  /*!< Use X-only public key style from RFC 7748 of the form
                                                         *   {x} in little-endian.
                                                         */
} ECDH_PublicKeyDataFormat;


/*!
 *  @brief  Struct containing the parameters required to generate a public key.
 */
typedef struct {
    const ECCParams_CurveParams     *curve;                   /*!< A pointer to the elliptic curve parameters for myPrivateKey */
    const CryptoKey                 *myPrivateKey;            /*!< A pointer to the private ECC key from which the new public
                                                               *   key will be generated. (maybe your static key)
                                                               */
    CryptoKey                       *myPublicKey;             /*!< A pointer to a public ECC key which has been initialized blank.
                                                               *   Newly generated key will be placed in this location.
                                                               *   The formatting byte will be filled in by the driver if the
                                                               *   publicKeyDataFormat requires it.
                                                               */
    ECDH_PublicKeyDataFormat        publicKeyDataFormat;      /*!< Either standard Octet string type or X-only type public keys */
} ECDH_OperationGeneratePublicKey;

/*!
 *  @brief  Struct containing the parameters required to compute the shared secret.
 */
typedef struct {
    const ECCParams_CurveParams     *curve;                   /*!< A pointer to the elliptic curve parameters for myPrivateKey.
                                                               *   If ECDH_generateKey() was used, this should be the same private key.
                                                               */
    const CryptoKey                 *myPrivateKey;            /*!< A pointer to the private ECC key which will be used in to
                                                               *   compute the shared secret.
                                                               */
    const CryptoKey                 *theirPublicKey;          /*!< A pointer to the public key of the party with whom the
                                                               *   shared secret will be generated.
                                                               */
    CryptoKey                       *sharedSecret;            /*!< A pointer to a CryptoKey which has been initialized blank.
                                                               *   The shared secret will be placed here.
                                                               *   The formatting byte will be filled in by the driver if the
                                                               *   sharedSecretDataFormat requires it.
                                                               */
    ECDH_PublicKeyDataFormat        publicKeyDataFormat;      /*!< Either standard Octet string type or X-only type public keys */
    ECDH_PublicKeyDataFormat        sharedSecretDataFormat;   /*!< Either standard Octet string type or X-only type public keys */
} ECDH_OperationComputeSharedSecret;

/*!
 *  @brief  Union containing pointers to all supported operation structs.
 */
typedef union {
    ECDH_OperationGeneratePublicKey      *generatePublicKey;    /*!< A pointer to an ECDH_OperationGeneratePublicKey struct */
    ECDH_OperationComputeSharedSecret    *computeSharedSecret;  /*!< A pointer to an ECDH_OperationGeneratePublicKey struct */
} ECDH_Operation;

/*!
 *  @brief  Enum for the operation types supported by the driver.
 */
typedef enum {
    ECDH_OPERATION_TYPE_GENERATE_PUBLIC_KEY = 1,
    ECDH_OPERATION_TYPE_COMPUTE_SHARED_SECRET = 2,
} ECDH_OperationType;

/*!
 *  @brief  The definition of a callback function used by the ECDH driver
 *          when used in ::ECDH_RETURN_BEHAVIOR_CALLBACK
 *
 *  @param  handle Handle of the client that started the ECDH operation.
 *
 *  @param  returnStatus The result of the ECDH operation. May contain an error code
 *          if the result is the point at infinity for example.
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
typedef void (*ECDH_CallbackFxn) (ECDH_Handle handle,
                                  int_fast16_t returnStatus,
                                  ECDH_Operation operation,
                                  ECDH_OperationType operationType);

/*!
 *  @brief  ECC Parameters
 *
 *  ECC Parameters are used to with the ECDH_open() call. Default values for
 *  these parameters are set using ECDH_Params_init().
 *
 *  @sa     ECDH_Params_init()
 */
typedef struct {
    ECDH_ReturnBehavior     returnBehavior;             /*!< Blocking, callback, or polling return behavior */
    ECDH_CallbackFxn        callbackFxn;                /*!< Callback function pointer */
    uint32_t                timeout;                    /*!< Timeout of the operation */
    void                   *custom;                     /*!< Custom argument used by driver
                                                         *   implementation
                                                         */
} ECDH_Params;

/*!
 *  @brief Default ECDH_Params structure
 *
 *  @sa     ECDH_Params_init()
 */
extern const ECDH_Params ECDH_defaultParams;

/*!
 *  @brief  This function initializes the ECC module.
 *
 *  @pre    The ECDH_config structure must exist and be persistent before this
 *          function can be called. This function must also be called before
 *          any other ECC driver APIs. This function call does not modify any
 *          peripheral registers.
 */
void ECDH_init(void);

/*!
 *  @brief  Function to initialize the ECDH_Params struct to its defaults
 *
 *  @param  params      An pointer to ECDH_Params structure for
 *                      initialization
 *
 *  Defaults values are:
 *      returnBehavior              = ECDH_RETURN_BEHAVIOR_BLOCKING
 *      callbackFxn                 = NULL
 *      timeout                     = SemaphoreP_WAIT_FOREVER
 *      custom                      = NULL
 */
void ECDH_Params_init(ECDH_Params *params);

/*!
 *  @brief  This function opens a given ECC peripheral.
 *
 *  @pre    ECC controller has been initialized using ECDH_init()
 *
 *  @param  index         Logical peripheral number for the ECC indexed into
 *                        the ECDH_config table
 *
 *  @param  params        Pointer to an parameter block, if NULL it will use
 *                        default values.
 *
 *  @return An ECDH_Handle on success or a NULL on an error or if it has been
 *          opened already.
 *
 *  @sa     ECDH_init()
 *  @sa     ECDH_close()
 */
ECDH_Handle ECDH_open(uint_least8_t index, const ECDH_Params *params);

/*!
 *  @brief  Function to close an ECC peripheral specified by the ECC handle
 *
 *  @pre    ECDH_open() has to be called first.
 *
 *  @param  handle An ECC handle returned from ECDH_open()
 *
 *  @sa     ECDH_open()
 */
void ECDH_close(ECDH_Handle handle);

/*!
 *  @brief  Function to initialize an ECDH_OperationGeneratePublicKey struct to its defaults
 *
 *  @param  operation   A pointer to ECDH_OperationGeneratePublicKey structure for
 *                      initialization
 *
 *  Defaults values are all zeros.
 */
void ECDH_OperationGeneratePublicKey_init(ECDH_OperationGeneratePublicKey *operation);

/*!
 *  @brief  Function to initialize an ECDH_OperationComputeSharedSecret struct to its defaults
 *
 *  @param  operation   A pointer to ECDH_OperationComputeSharedSecret structure for
 *                      initialization
 *
 *  Defaults values are all zeros.
 */
void ECDH_OperationComputeSharedSecret_init(ECDH_OperationComputeSharedSecret *operation);

/*!
 *  @brief Generates a public key for use in key agreement.
 *
 *  This function can be used to generate a public key from a private key.
 *
 *  @param      handle          A ECDH handle returned from ECDH_open()
 *
 *  @param      operation       A pointer to a struct containing the requisite
 *                              parameters to execute the function.
 *
 *  @pre Call ECDH_OperationGeneratePublicKey_init() on @c operation.
 *
 *  @post ECDH_computeSharedSecret()
 *
 *  @retval #ECDH_STATUS_SUCCESS                The operation succeeded.
 *  @retval #ECDH_STATUS_ERROR                  The operation failed.
 *  @retval #ECDH_STATUS_RESOURCE_UNAVAILABLE   The required hardware resource was not available. Try again later.
 *  @retval #ECDH_STATUS_CANCELED               The operation was canceled.
 *  @retval #ECDH_STATUS_POINT_AT_INFINITY      The computed public key is the point at infinity.
 *  @retval #ECDH_STATUS_PRIVATE_KEY_ZERO       The provided private key is zero.
 *
 */
int_fast16_t ECDH_generatePublicKey(ECDH_Handle handle, ECDH_OperationGeneratePublicKey *operation);

/*!
 *  @brief Computes a shared secret
 *
 *  This secret can be used to generate shared keys for encryption and authentication.
 *
 *  @param      handle              A ECDH handle returned from ECDH_open()
 *
 *  @param      operation       A pointer to a struct containing the requisite
 *
 *  @pre Call ECDH_OperationComputeSharedSecret_init() on \c operation.
 *       Generate a shared secret off-chip or using ECDH_generatePublicKey()
 *
 *  @retval #ECDH_STATUS_SUCCESS                        The operation succeeded.
 *  @retval #ECDH_STATUS_ERROR                          The operation failed.
 *  @retval #ECDH_STATUS_RESOURCE_UNAVAILABLE           The required hardware resource was not available. Try again later.
 *  @retval #ECDH_STATUS_CANCELED                       The operation was canceled.
 *  @retval #ECDH_STATUS_PUBLIC_KEY_NOT_ON_CURVE        The foreign public key is not a point on the specified curve.
 *  @retval #ECDH_STATUS_PUBLIC_KEY_LARGER_THAN_PRIME   One of the public key coordinates is larger the the curve's prime.
 */
int_fast16_t ECDH_computeSharedSecret(ECDH_Handle handle, ECDH_OperationComputeSharedSecret *operation);

/*!
 *  @brief Cancels an ongoing ECDH operation.
 *
 *  Asynchronously cancels an ECDH operation. Only available when using
 *  ECDH_RETURN_BEHAVIOR_CALLBACK or ECDH_RETURN_BEHAVIOR_BLOCKING.
 *  The operation will terminate as though an error occured. The
 *  return status code of the operation will be ECDH_STATUS_CANCELED.
 *
 *  @param  handle Handle of the operation to cancel
 *
 *  @retval #ECDH_STATUS_SUCCESS               The operation was canceled.
 *  @retval #ECDH_STATUS_ERROR                 The operation was not canceled. There may be no operation to cancel.
 */
int_fast16_t ECDH_cancelOperation(ECDH_Handle handle);

/**
 *  @brief  Constructs a new ECDH object
 *
 *  Unlike #ECDH_open(), #ECDH_construct() does not require the hwAttrs and
 *  object to be allocated in a #ECDH_Config array that is indexed into.
 *  Instead, the #ECDH_Config, hwAttrs, and object can be allocated at any
 *  location. This allows for relatively simple run-time allocation of temporary
 *  driver instances on the stack or the heap.
 *  The drawback is that this makes it more difficult to write device-agnostic
 *  code. If you use an ifdef with DeviceFamily, you can choose the correct
 *  object and hwAttrs to allocate. That compilation unit will be tied to the
 *  device it was compiled for at this point. To change devices, recompilation
 *  of the application with a different DeviceFamily setting is necessary.
 *
 *  @param  config #ECDH_Config describing the location of the object and hwAttrs.
 *
 *  @param  params #ECDH_Params to configure the driver instance.
 *
 *  @return        Returns a #ECDH_Handle on success or NULL on failure.
 *
 *  @pre    The object struct @c config points to must be zeroed out prior to
 *          calling this function. Otherwise, unexpected behavior may ensue.
 */
ECDH_Handle ECDH_construct(ECDH_Config *config, const ECDH_Params *params);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_ECDH__include */
