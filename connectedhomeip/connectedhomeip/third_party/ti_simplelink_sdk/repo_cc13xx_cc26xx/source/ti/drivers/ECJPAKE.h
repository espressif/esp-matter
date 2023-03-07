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
 * @file ECJPAKE.h
 *
 * @brief   TI Driver for Elliptic Curve Password Authenticated Key Exchange
 *          by Juggling.
 *
 * @anchor ti_drivers_ECJPAKE_Overview
 * # Overview #
 * Elliptic Curve Password Authenticated Key Exchange by Juggling (EC-JPAKE)
 * is a key agreement scheme that establishes a secure channel over an insecure
 * network. It only requires sharing a password offline and does not require
 * public key infrastructure or trusted third parties such as certificate
 * authorities.
 *
 * At the end of the EC-JPAKE scheme, both parties derive a shared secret
 * from which a session key is derived.
 *
 * The scheme is symmetric. Both parties perform the exact same operations
 * to end up with the shared secret.
 *
 * # Steps involved #
 * Since the scheme is symmetric, the steps involved will be illustrated
 * using Alice and Bob as relevant parties.
 *
 *  -# Alice and Bob decide on some pre-shared secret, the password, and
 *     negotiate this through some offline means such as during commissioning.
 *
 *  -# Alice generates private keys x1, x2, v1, and v2 uniformly at random from [1, n - 1],
 *     where n is the order of the curve.
 *  -# Alice generates public keys X1 = x1 * G, X2 = x2 * G, V1 = v1 * G, and V2 = v2 * G.
 *  -# Alice generates Zero-Knowledge Proofs (ZKPs) for (X1, x1) and (X2, x2).
 *     The required hash is computed by concatenating G, V, the public key
 *     the ZKP authenticates, and the UserID of the authenticator and hashing
 *     the new bitstring. The exact order and formatting of all parameters and
 *     any extra information such as length words must be agreed upon by both
 *     parties to yield the same result.
 *  -# Alice sends X1, X2, V1, V2, r1, r2, and her UserID to Bob.
 *
 *  -# Bob generates private keys x3, x4, v3, and v4 uniformly at random from [1, n - 1],
 *     where n is the order of the curve.
 *  -# Bob generates public keys X3 = x3 * G, X4 = x4 * G, V3 = v3 * G, and V4 = v4 * G.
 *  -# Bob generates Zero-Knowledge Proofs (ZKPs) for (X3, x3) and (X4, x4).
 *  -# Bob sends X3, X4, V3, V4, r3, r4, and his UserID to Bob.
 *
 *  -# Alice and Bob verify the other parties ZKPs and break off the scheme if they
 *     do not check out.
 *
 *  -# Alice computes the new generator point G2 = (X1 + X3 + X4).
 *  -# Alice computes the combined private key x5 = x2 * s, where s is the pre-shared
 *     secret.
 *  -# Alice computes the combined public key X5 = x5 * G2.
 *  -# Alice computes a ZKP for (X5, x5) using G2 as the generator point of the ZKP.
 *  -# Alice sends X5, V5, r5, and her UserID to Bob.
 *
 *  -# Bob computes the new generator point G3 = (X3 + X1 + X2).
 *  -# Bob computes the combined private key x6 = x4 * s, where s is the pre-shared
 *     secret.
 *  -# Bob computes the combined public key X6 = x6 * G3.
 *  -# Bob computes a ZKP for (X6, x6) using G3 as the generator point of the ZKP.
 *  -# Bob sends X6, V6, r6, and his UserID to Alice.
 *
 *  -# Alice and Bob verify the other parties ZKPs and break off the scheme if they
 *     do not check out. This involves computing the other parties generator point.
 *
 *  -# Alice computes shared secret K = (X6 - (X4 * x5)) * x2.
 *
 *  -# Bob computes shared secret K = (X5 - (X2 * x6)) * x4.
 *
 *  -# Alice and Bob each run K through a mutually agreed upon key derivation
 *    function to compute the symmetric session key.
 *
 * @anchor ti_drivers_ECJPAKE_Usage
 * # Usage #
 *
 * ## Before starting an ECJPAKE operation #
 *
 * Before starting an ECJPAKE operation, the application must do the following:
 * -# Call ECJPAKE_init() to initialize the driver
 * -# Call ECJPAKE_Params_init() to initialize the ECJPAKE_Params to default values.
 * -# Modify the ECJPAKE_Params as desired
 * -# Call ECJPAKE_open() to open an instance of the driver
 *
 * ## Round one #
 * -# Initialize the following private key CryptoKeys.
 *    Seed them with keying material uniformly drawn from [1, n - 1]
 *      - myPrivateKey1
 *      - myPrivateKey2
 *      - myPrivateV1
 *      - myPrivateV2
 * -# Initialize the following blank public key CryptoKeys:
 *      - myPublicKey1
 *      - myPublicKey2
 *      - myPublicV1
 *      - myPublicV2
 *      - theirPublicKey1
 *      - theirPublicKey2
 *      - theirPublicV1
 *      - theirPublicV2
 * -# Call ECJPAKE_roundOneGenerateKeys() to generate all round one keys as needed.
 * -# Generate the hashes for the ZKPs using previously agreed upon formatting.
 *    Use the default generator point of the curve in the first round.
 * -# Generate your two ZKPs by calling ECJPAKE_generateZKP() once per ZKP.
 * -# Exchange public keys, UserIDs, and ZKP signatures. Write the received keying
 *    material into the relevant buffers or load them into key stores as specified
 *    by the CryptoKeys initialised earlier.
 * -# Verify the other party's ZKPs after computing their ZKP hash by calling
 *    ECJPAKE_verifyZKP() once per ZKP.
 * -# You can now let all V keys, myPrivateKey1, and all ZKP signatures go out of scope
 *    and re-use their memory.
 *
 * ## Round two #
 *  -# Initialize the following private key CryptoKeys.
 *     Seed myPrivateV with keying material uniformly drawn from [1, n - 1].
 *     Initialise the preSharedSecret with the common keying material previously
 *     shared between you and the other party.
 *      - preSharedSecret
 *      - myCombinedPrivateKey
 *  -# Initialize the following blank public key CryptoKeys:
 *      - theirNewGenerator
 *      - myNewGenerator
 *      - myCombinedPublicKey
 *      - myPublicV
 *  -# Call ECJPAKE_roundTwoGenerateKeys() to generate the remaining round two keys.
 *  -# Generate the hash for your ZKP use myNewGenerator as your generator point.
 *  -# Exchange public keys, UserIDs, and ZKP signatures. Write the received keying
 *     material into the relevant buffers or load them into key stores as specified
 *     by the CryptoKeys initialised earlier.
 *  -# Verify the other party's ZKP after computing their ZKP hash b calling
 *     ECJPAKE_verifyZKP(). Use theirNewGenerator as the generator point for this
 *     ZKP.
 *  -# You can now let all keys and keying material but myCombinedPrivateKey,
 *     theirCombinedPublicKey, theirPublicKey2, and myPrivateKey2 go out of scope.
 *
 * ## Computing the shared secret #
 *  -#  Initialize the following blank public key CryptoKey:
 *      - sharedSecret
 *  -#  Call ECJPAKE_computeSharedSecret().
 *  -#  Run sharedSecret through a key derivation function to compute the shared
 *     symmetric session key.
 *
 * ## Key Formatting
 * The ECJPAKE API expects the private and public keys to be formatted in octet
 * string format. The details of octet string formatting can be found in
 * SEC 1: Elliptic Curve Cryptography.
 *
 * Private keys and V's are formatted as big-endian integers of the same length
 * as the curve length.
 *
 * Public keys, public V's, generator points, and shared secrets are points on
 * an elliptic curve. These points can be expressed in several ways.
 * This API uses points expressed in uncompressed affine coordinates by default.
 * The octet string format requires a formatting byte in the first byte of the
 * public key. When using uncompressed affine coordinates, this is the value
 * 0x04.
 * The point itself is stored as a concatenated array of X followed by Y.
 * X and Y are big-endian. Some implementations do not require or yield
 * the Y coordinate for ECJPAKE on certain curves. It is recommended that the full
 * keying material buffer of twice the curve param length is used to facilitate
 * code-reuse. Implementations that do not use the Y coordinate will zero-out
 * the Y-coordinate whenever they write a point to the CryptoKey.
 *
 * This API accepts and returns the keying material of public keys according
 * to the following table:
 *
 * | Curve Type         | Keying Material Array | Array Length               |
 * |--------------------|-----------------------|----------------------------|
 * | Short Weierstrass  | [0x04, X, Y]          | 1 + 2 * Curve Param Length |
 * | Montgomery         | [0x04, X, Y]          | 1 + 2 * Curve Param Length |
 * | Edwards            | [0x04, X, Y]          | 1 + 2 * Curve Param Length |
 *
 * The r component of the ZKP signature, hash, and preSharedSecret also all
 * use the octet string format. They are interpreted as big-endian integers.
 *
 * @anchor ti_drivers_ECJPAKE_Synopsis
 * ## Synopsis
 *
 * @anchor ti_drivers_ECJPAKE_Synopsis_Code
 * @code
 * // Import ECJPAKE Driver definitions
 * #include <ti/drivers/ECJPAKE.h>
 *
 * ECJPAKE_init();
 *
 * // Since we are using default ECJPAKE_Params, we just pass in NULL for that parameter.
 * ecjpakeHandle = ECJPAKE_open(0, NULL);

 * ECJPAKE_Handle handle = ECJPAKE_open(0, &params);
 *
 * ECJPAKE_OperationRoundOneGenerateKeys   operationRoundOneGenerateKeys;
 * ECJPAKE_OperationRoundTwoGenerateKeys   operationRoundTwoGenerateKeys;
 * ECJPAKE_OperationGenerateZKP            operationGenerateZKP;
 * ECJPAKE_OperationVerifyZKP              operationVerifyZKP;
 * ECJPAKE_OperationComputeSharedSecret    operationComputeSharedSecret;
 *
 * // Generate my round one keys
 * ECJPAKE_OperationRoundOneGenerateKeys_init(&operationRoundOneGenerateKeys);
 * operationRoundOneGenerateKeys.curve             = &ECCParams_NISTP256;
 * operationRoundOneGenerateKeys.myPrivateKey1     = &myPrivateCryptoKey1;
 * operationRoundOneGenerateKeys.myPrivateKey2     = &myPrivateCryptoKey2;
 * operationRoundOneGenerateKeys.myPublicKey1      = &myPublicCryptoKey1;
 * operationRoundOneGenerateKeys.myPublicKey2      = &myPublicCryptoKey2;
 * operationRoundOneGenerateKeys.myPrivateV1       = &myPrivateCryptoV1;
 * operationRoundOneGenerateKeys.myPrivateV2       = &myPrivateCryptoV2;
 * operationRoundOneGenerateKeys.myPublicV1        = &myPublicCryptoV1;
 * operationRoundOneGenerateKeys.myPublicV2        = &myPublicCryptoV2;
 *
 * result = ECJPAKE_roundOneGenerateKeys(handle, &operationRoundOneGenerateKeys);
 *
 * // Generate hashes here
 *
 * // generate my round one ZKPs
 * ECJPAKE_OperationGenerateZKP_init(&operationGenerateZKP);
 * operationGenerateZKP.curve              = &ECCParams_NISTP256;
 * operationGenerateZKP.myPrivateKey       = &myPrivateCryptoKey1;
 * operationGenerateZKP.myPrivateV         = &myPrivateCryptoV1;
 * operationGenerateZKP.hash               = myHash1;
 * operationGenerateZKP.r                  = myR1;
 *
 * result = ECJPAKE_generateZKP(handle, &operationGenerateZKP);
 *
 * ECJPAKE_OperationGenerateZKP_init(&operationGenerateZKP);
 * operationGenerateZKP.curve              = &ECCParams_NISTP256;
 * operationGenerateZKP.myPrivateKey       = &myPrivateCryptoKey2;
 * operationGenerateZKP.myPrivateV         = &myPrivateCryptoV2;
 * operationGenerateZKP.hash               = myHash2;
 * operationGenerateZKP.r                  = myR2;
 *
 * result = ECJPAKE_generateZKP(handle, &operationGenerateZKP);
 *
 * // Do ZKP and key transmission here
 *
 * // Verify their round one ZKPs
 * // Generate their hashes here
 *
 * ECJPAKE_OperationVerifyZKP_init(&operationVerifyZKP);
 * operationVerifyZKP.curve                = &ECCParams_NISTP256;
 * operationVerifyZKP.theirGenerator       = NULL;
 * operationVerifyZKP.theirPublicKey       = &theirPublicCryptoKey1;
 * operationVerifyZKP.theirPublicV         = &theirPublicCryptoV1;
 * operationVerifyZKP.hash                 = theirHash1;
 * operationVerifyZKP.r                    = theirR1;
 *
 * result = ECJPAKE_verifyZKP(handle, &operationVerifyZKP);
 *
 * ECJPAKE_OperationVerifyZKP_init(&operationVerifyZKP);
 * operationVerifyZKP.curve                = &ECCParams_NISTP256;
 * operationVerifyZKP.theirGenerator       = NULL;
 * operationVerifyZKP.theirPublicKey       = &theirPublicCryptoKey2;
 * operationVerifyZKP.theirPublicV         = &theirPublicCryptoV2;
 * operationVerifyZKP.hash                 = theirHash2;
 * operationVerifyZKP.r                    = theirR2;
 *
 * result = ECJPAKE_verifyZKP(handle,&operationVerifyZKP);
 *
 * // Round two starts now
 *
 * // Generate my round two keys
 * ECJPAKE_OperationRoundTwoGenerateKeys_init(&operationRoundTwoGenerateKeys);
 * operationRoundTwoGenerateKeys.curve                 = &ECCParams_NISTP256;
 * operationRoundTwoGenerateKeys.myPrivateKey2         = &myPrivateCryptoKey2;
 * operationRoundTwoGenerateKeys.myPublicKey1          = &myPublicCryptoKey1;
 * operationRoundTwoGenerateKeys.myPublicKey2          = &myPublicCryptoKey2;
 * operationRoundTwoGenerateKeys.theirPublicKey1       = &theirPublicCryptoKey1;
 * operationRoundTwoGenerateKeys.theirPublicKey2       = &theirPublicCryptoKey2;
 * operationRoundTwoGenerateKeys.preSharedSecret       = &preSharedSecretCryptoKey;
 * operationRoundTwoGenerateKeys.theirNewGenerator     = &theirGeneratorKey;
 * operationRoundTwoGenerateKeys.myNewGenerator        = &myGeneratorKey;
 * operationRoundTwoGenerateKeys.myCombinedPrivateKey  = &myCombinedPrivateKey;
 * operationRoundTwoGenerateKeys.myCombinedPublicKey   = &myCombinedPublicKey;
 * operationRoundTwoGenerateKeys.myPrivateV            = &myPrivateCryptoV3;
 * operationRoundTwoGenerateKeys.myPublicV             = &myPublicCryptoV3;
 *
 * result = ECJPAKE_roundTwoGenerateKeys(handle, &operationRoundTwoGenerateKeys);
 *
 * // Generate my round  two ZKP
 * // Generate the round two hash here
 *
 * ECJPAKE_OperationGenerateZKP_init(&operationGenerateZKP);
 * operationGenerateZKP.curve              = &ECCParams_NISTP256;
 * operationGenerateZKP.myPrivateKey       = &myCombinedPrivateKey;
 * operationGenerateZKP.myPrivateV         = &myPrivateCryptoV3;
 * operationGenerateZKP.hash               = myHash3;
 * operationGenerateZKP.r                  = myR3;
 *
 * result = ECJPAKE_generateZKP(handle, &operationGenerateZKP);
 *
 * // Exchange keys and ZKPs again
 *
 * // Verify their second round ZKP
 * // Generate their round two hash here
 *
 * ECJPAKE_OperationVerifyZKP_init(&operationVerifyZKP);
 * operationVerifyZKP.curve                = &ECCParams_NISTP256;
 * operationVerifyZKP.theirGenerator       = &theirGeneratorKey;
 * operationVerifyZKP.theirPublicKey       = &theirCombinedPublicKey;
 * operationVerifyZKP.theirPublicV         = &theirPublicCryptoV3;
 * operationVerifyZKP.hash                 = theirHash3;
 * operationVerifyZKP.r                    = theirR3;
 *
 * result = ECJPAKE_verifyZKP(handle, &operationVerifyZKP);
 *
 * // Generate shared secret
 * ECJPAKE_OperationComputeSharedSecret_init(&operationComputeSharedSecret);
 * operationComputeSharedSecret.curve                      = &ECCParams_NISTP256;
 * operationComputeSharedSecret.myCombinedPrivateKey       = &myCombinedPrivateKey;
 * operationComputeSharedSecret.theirCombinedPublicKey     = &theirCombinedPublicKey;
 * operationComputeSharedSecret.theirPublicKey2            = &theirPublicCryptoKey2;
 * operationComputeSharedSecret.myPrivateKey2              = &myPrivateCryptoKey2;
 * operationComputeSharedSecret.sharedSecret               = &sharedSecretCryptoKey;
 *
 * result =  ECJPAKE_computeSharedSecret(handle, &operationComputeSharedSecret);
 *
 * // Close the driver
 * ECJPAKE_close(handle);
 * @endcode
 *
 * @anchor ti_drivers_ECJPAKE_Examples
 * # Examples #
 *
 * ## Basic ECJPAKE exchange #
 *
 * @code
 *
 * #define NISTP256_CURVE_LENGTH_BYTES 32
 * #define OCTET_STRING_OFFSET 1
 * #define NISTP256_PRIVATE_KEY_LENGTH_BYTES NISTP256_CURVE_LENGTH_BYTES
 * #define NISTP256_PUBLIC_KEY_LENGTH_BYTES (NISTP256_CURVE_LENGTH_BYTES * 2 + OCTET_STRING_OFFSET)
 *
 * // My fixed keying material
 * uint8_t myPrivateKeyMaterial1[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t myPrivateKeyMaterial2[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t myPrivateVMaterial1[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t myPrivateVMaterial2[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t myPrivateVMaterial3[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t myHash1[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t myHash2[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t myHash3[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * // My derived keying material
 * uint8_t myR1[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t myR2[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t myR3[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t myPublicKeyMaterial1[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 * uint8_t myPublicKeyMaterial2[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 * uint8_t myPublicVMaterial1[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 * uint8_t myPublicVMaterial2[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 * uint8_t myPublicVMaterial3[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 * uint8_t myCombinedPublicKeyMaterial1[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 * uint8_t myCombinedPrivateKeyMaterial1[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t myGenerator[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 *
 * // Their fixed keying material
 * uint8_t theirHash1[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t theirHash2[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t theirHash3[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 *
 * // Their derived keying material
 * uint8_t theirR1[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t theirR2[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t theirR3[NISTP256_PRIVATE_KEY_LENGTH_BYTES];
 * uint8_t theirPublicKeyMaterial1[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 * uint8_t theirPublicKeyMaterial2[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 * uint8_t theirPublicVMaterial1[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 * uint8_t theirPublicVMaterial2[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 * uint8_t theirPublicVMaterial3[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 * uint8_t theirCombinedPublicKeyMaterial1[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 * uint8_t theirGenerator[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 *
 * //  Shared secrets
 * uint8_t preSharedSecretKeyingMaterial[NISTP256_PRIVATE_KEY_LENGTH_BYTES] = "This is our password";
 * uint8_t sharedSecretKeyingMaterial1[NISTP256_PUBLIC_KEY_LENGTH_BYTES];
 *
 * // Pre-Shared Secret Key
 * CryptoKey preSharedSecretCryptoKey;
 *
 * // Final shared secret keys
 * CryptoKey sharedSecretCryptoKey;
 *
 * // My's keys
 * CryptoKey myPrivateCryptoKey1;
 * CryptoKey myPrivateCryptoKey2;
 * CryptoKey myPrivateCryptoV1;
 * CryptoKey myPrivateCryptoV2;
 * CryptoKey myPrivateCryptoV3;
 * CryptoKey myCombinedPrivateKey;
 *
 * CryptoKey myPublicCryptoKey1;
 * CryptoKey myPublicCryptoKey2;
 * CryptoKey myPublicCryptoV1;
 * CryptoKey myPublicCryptoV2;
 * CryptoKey myPublicCryptoV3;
 * CryptoKey myCombinedPublicKey;
 * CryptoKey myGeneratorKey;
 *
 * // Their's Keys
 * CryptoKey theirPublicCryptoKey1;
 * CryptoKey theirPublicCryptoKey2;
 * CryptoKey theirPublicCryptoV1;
 * CryptoKey theirPublicCryptoV2;
 * CryptoKey theirPublicCryptoV3;
 * CryptoKey theirCombinedPublicKey;
 * CryptoKey theirGeneratorKey;
 *
 * // NISTP256 generator
 * CryptoKeyPlaintext_initKey(NULL, ECCParams_NISTP256.generatorX, sizeof(ECCParams_NISTP256.length * 2));
 *
 * // Pre-shared secret
 * CryptoKeyPlaintext_initKey(&preSharedSecretCryptoKey, preSharedSecretKeyingMaterial, sizeof(preSharedSecretKeyingMaterial));
 *
 * // Final shared secret key
 * CryptoKeyPlaintext_initKey(&sharedSecretCryptoKey, sharedSecretKeyingMaterial1, sizeof(sharedSecretKeyingMaterial1));
 * CryptoKeyPlaintext_initKey(&sharedSecretCryptoKey2, sharedSecretKeyingMaterial2, sizeof(sharedSecretKeyingMaterial2));
 *
 *
 * // My keys
 *
 * // This example assumes that the private keying material buffers already
 * // contains random bytes. Otherwise, we need to use a TRNG or DRBG to fill
 * // them after initialising the CryptoKeys.
 * CryptoKeyPlaintext_initKey(&myPrivateCryptoKey1, myPrivateKeyMaterial1, sizeof(myPrivateKeyMaterial1));
 * CryptoKeyPlaintext_initKey(&myPrivateCryptoKey2, myPrivateKeyMaterial2, sizeof(myPrivateKeyMaterial2));
 * CryptoKeyPlaintext_initKey(&myPrivateCryptoV1, myPrivateVMaterial1, sizeof(myPrivateVMaterial1));
 * CryptoKeyPlaintext_initKey(&myPrivateCryptoV2, myPrivateVMaterial2, sizeof(myPrivateVMaterial2));
 * CryptoKeyPlaintext_initKey(&myPrivateCryptoV3, myPrivateVMaterial3, sizeof(myPrivateVMaterial3));
 *
 * CryptoKeyPlaintext_initBlankKey(&myPublicCryptoKey1, myPublicKeyMaterial1, sizeof(myPublicKeyMaterial1));
 * CryptoKeyPlaintext_initBlankKey(&myPublicCryptoKey2, myPublicKeyMaterial2, sizeof(myPublicKeyMaterial2));
 * CryptoKeyPlaintext_initBlankKey(&myPublicCryptoV1, myPublicVMaterial1, sizeof(myPublicVMaterial1));
 * CryptoKeyPlaintext_initBlankKey(&myPublicCryptoV2, myPublicVMaterial2, sizeof(myPublicVMaterial2));
 * CryptoKeyPlaintext_initBlankKey(&myPublicCryptoV3, myPublicVMaterial3, sizeof(myPublicVMaterial3));
 * CryptoKeyPlaintext_initBlankKey(&myCombinedPrivateKey, myCombinedPrivateKeyMaterial1, sizeof(myCombinedPrivateKeyMaterial1));
 * CryptoKeyPlaintext_initBlankKey(&myCombinedPublicKey, myCombinedPublicKeyMaterial1, sizeof(myCombinedPublicKeyMaterial1));
 * CryptoKeyPlaintext_initBlankKey(&myGeneratorKey, myGenerator, sizeof(myGenerator));
 *
 * // Their keys
 * CryptoKeyPlaintext_initBlankKey(&theirPublicCryptoKey1, theirPublicKeyMaterial1, sizeof(theirPublicKeyMaterial1));
 * CryptoKeyPlaintext_initBlankKey(&theirPublicCryptoKey2, theirPublicKeyMaterial2, sizeof(theirPublicKeyMaterial2));
 * CryptoKeyPlaintext_initBlankKey(&theirPublicCryptoV1, theirPublicVMaterial1, sizeof(theirPublicVMaterial1));
 * CryptoKeyPlaintext_initBlankKey(&theirPublicCryptoV2, theirPublicVMaterial2, sizeof(theirPublicVMaterial2));
 * CryptoKeyPlaintext_initBlankKey(&theirPublicCryptoV3, theirPublicVMaterial3, sizeof(theirPublicVMaterial3));
 * CryptoKeyPlaintext_initBlankKey(&theirCombinedPublicKey, theirCombinedPublicKeyMaterial1, sizeof(theirCombinedPublicKeyMaterial1));
 * CryptoKeyPlaintext_initBlankKey(&theirGeneratorKey, theirGenerator, sizeof(theirGenerator));
 *
 * // Initial driver setup
 * ECJPAKE_Params params;
 * ECJPAKE_Params_init(&params);
 *
 *
 * ECJPAKE_Handle handle = ECJPAKE_open(0, &params);
 *
 * ECJPAKE_OperationRoundOneGenerateKeys   operationRoundOneGenerateKeys;
 * ECJPAKE_OperationRoundTwoGenerateKeys   operationRoundTwoGenerateKeys;
 * ECJPAKE_OperationGenerateZKP            operationGenerateZKP;
 * ECJPAKE_OperationVerifyZKP              operationVerifyZKP;
 * ECJPAKE_OperationComputeSharedSecret    operationComputeSharedSecret;
 *
 * // Generate my round one keys
 * ECJPAKE_OperationRoundOneGenerateKeys_init(&operationRoundOneGenerateKeys);
 * operationRoundOneGenerateKeys.curve             = &ECCParams_NISTP256;
 * operationRoundOneGenerateKeys.myPrivateKey1     = &myPrivateCryptoKey1;
 * operationRoundOneGenerateKeys.myPrivateKey2     = &myPrivateCryptoKey2;
 * operationRoundOneGenerateKeys.myPublicKey1      = &myPublicCryptoKey1;
 * operationRoundOneGenerateKeys.myPublicKey2      = &myPublicCryptoKey2;
 * operationRoundOneGenerateKeys.myPrivateV1       = &myPrivateCryptoV1;
 * operationRoundOneGenerateKeys.myPrivateV2       = &myPrivateCryptoV2;
 * operationRoundOneGenerateKeys.myPublicV1        = &myPublicCryptoV1;
 * operationRoundOneGenerateKeys.myPublicV2        = &myPublicCryptoV2;
 *
 * int_fast16_t result = ECJPAKE_roundOneGenerateKeys(handle, &operationRoundOneGenerateKeys);
 *
 * if (result != ECJPAKE_STATUS_SUCCESS) {
 *     while(1);
 * }
 *
 * // Generate hashes here
 *
 * // generate my round one ZKPs
 * ECJPAKE_OperationGenerateZKP_init(&operationGenerateZKP);
 * operationGenerateZKP.curve              = &ECCParams_NISTP256;
 * operationGenerateZKP.myPrivateKey       = &myPrivateCryptoKey1;
 * operationGenerateZKP.myPrivateV         = &myPrivateCryptoV1;
 * operationGenerateZKP.hash               = myHash1;
 * operationGenerateZKP.r                  = myR1;
 *
 * result = ECJPAKE_generateZKP(handle, &operationGenerateZKP);
 *
 * if (result != ECJPAKE_STATUS_SUCCESS) {
 *     while(1);
 * }
 *
 * ECJPAKE_OperationGenerateZKP_init(&operationGenerateZKP);
 * operationGenerateZKP.curve              = &ECCParams_NISTP256;
 * operationGenerateZKP.myPrivateKey       = &myPrivateCryptoKey2;
 * operationGenerateZKP.myPrivateV         = &myPrivateCryptoV2;
 * operationGenerateZKP.hash               = myHash2;
 * operationGenerateZKP.r                  = myR2;
 *
 * result = ECJPAKE_generateZKP(handle, &operationGenerateZKP);
 *
 * if (result != ECJPAKE_STATUS_SUCCESS) {
 *     while(1);
 * }
 *
 * // Do ZKP and key transmission here
 *
 * // Verify their round one ZKPs
 * // Generate their hashes here
 *
 * ECJPAKE_OperationVerifyZKP_init(&operationVerifyZKP);
 * operationVerifyZKP.curve                = &ECCParams_NISTP256;
 * operationVerifyZKP.theirGenerator       = NULL;
 * operationVerifyZKP.theirPublicKey       = &theirPublicCryptoKey1;
 * operationVerifyZKP.theirPublicV         = &theirPublicCryptoV1;
 * operationVerifyZKP.hash                 = theirHash1;
 * operationVerifyZKP.r                    = theirR1;
 *
 * result = ECJPAKE_verifyZKP(handle, &operationVerifyZKP);
 *
 * if (result != ECJPAKE_STATUS_SUCCESS) {
 *     while(1);
 * }
 *
 * ECJPAKE_OperationVerifyZKP_init(&operationVerifyZKP);
 * operationVerifyZKP.curve                = &ECCParams_NISTP256;
 * operationVerifyZKP.theirGenerator       = NULL;
 * operationVerifyZKP.theirPublicKey       = &theirPublicCryptoKey2;
 * operationVerifyZKP.theirPublicV         = &theirPublicCryptoV2;
 * operationVerifyZKP.hash                 = theirHash2;
 * operationVerifyZKP.r                    = theirR2;
 *
 * result = ECJPAKE_verifyZKP(handle,&operationVerifyZKP);
 *
 * if (result != ECJPAKE_STATUS_SUCCESS) {
 *     while(1);
 * }
 *
 * // Round two starts now
 *
 * // Generate my round two keys
 * ECJPAKE_OperationRoundTwoGenerateKeys_init(&operationRoundTwoGenerateKeys);
 * operationRoundTwoGenerateKeys.curve                 = &ECCParams_NISTP256;
 * operationRoundTwoGenerateKeys.myPrivateKey2         = &myPrivateCryptoKey2;
 * operationRoundTwoGenerateKeys.myPublicKey1          = &myPublicCryptoKey1;
 * operationRoundTwoGenerateKeys.myPublicKey2          = &myPublicCryptoKey2;
 * operationRoundTwoGenerateKeys.theirPublicKey1       = &theirPublicCryptoKey1;
 * operationRoundTwoGenerateKeys.theirPublicKey2       = &theirPublicCryptoKey2;
 * operationRoundTwoGenerateKeys.preSharedSecret       = &preSharedSecretCryptoKey;
 * operationRoundTwoGenerateKeys.theirNewGenerator     = &theirGeneratorKey;
 * operationRoundTwoGenerateKeys.myNewGenerator        = &myGeneratorKey;
 * operationRoundTwoGenerateKeys.myCombinedPrivateKey  = &myCombinedPrivateKey;
 * operationRoundTwoGenerateKeys.myCombinedPublicKey   = &myCombinedPublicKey;
 * operationRoundTwoGenerateKeys.myPrivateV            = &myPrivateCryptoV3;
 * operationRoundTwoGenerateKeys.myPublicV             = &myPublicCryptoV3;
 *
 * result = ECJPAKE_roundTwoGenerateKeys(handle, &operationRoundTwoGenerateKeys);
 *
 * // Generate my round  two ZKP
 * // Generate the round two hash here
 *
 * ECJPAKE_OperationGenerateZKP_init(&operationGenerateZKP);
 * operationGenerateZKP.curve              = &ECCParams_NISTP256;
 * operationGenerateZKP.myPrivateKey       = &myCombinedPrivateKey;
 * operationGenerateZKP.myPrivateV         = &myPrivateCryptoV3;
 * operationGenerateZKP.hash               = myHash3;
 * operationGenerateZKP.r                  = myR3;
 *
 * result = ECJPAKE_generateZKP(handle, &operationGenerateZKP);
 *
 * if (result != ECJPAKE_STATUS_SUCCESS) {
 *     while(1);
 * }
 *
 * // Exchange keys and ZKPs again
 *
 * // Verify their second round ZKP
 * // Generate their round two hash here
 *
 * ECJPAKE_OperationVerifyZKP_init(&operationVerifyZKP);
 * operationVerifyZKP.curve                = &ECCParams_NISTP256;
 * operationVerifyZKP.theirGenerator       = &theirGeneratorKey;
 * operationVerifyZKP.theirPublicKey       = &theirCombinedPublicKey;
 * operationVerifyZKP.theirPublicV         = &theirPublicCryptoV3;
 * operationVerifyZKP.hash                 = theirHash3;
 * operationVerifyZKP.r                    = theirR3;
 *
 * result = ECJPAKE_verifyZKP(handle, &operationVerifyZKP);
 *
 * if (result != ECJPAKE_STATUS_SUCCESS) {
 *     while(1);
 * }
 *
 *
 * // Generate shared secret
 * ECJPAKE_OperationComputeSharedSecret_init(&operationComputeSharedSecret);
 * operationComputeSharedSecret.curve                      = &ECCParams_NISTP256;
 * operationComputeSharedSecret.myCombinedPrivateKey       = &myCombinedPrivateKey;
 * operationComputeSharedSecret.theirCombinedPublicKey     = &theirCombinedPublicKey;
 * operationComputeSharedSecret.theirPublicKey2            = &theirPublicCryptoKey2;
 * operationComputeSharedSecret.myPrivateKey2              = &myPrivateCryptoKey2;
 * operationComputeSharedSecret.sharedSecret               = &sharedSecretCryptoKey;
 *
 * result =  ECJPAKE_computeSharedSecret(handle, &operationComputeSharedSecret);
 *
 * if (result != ECJPAKE_STATUS_SUCCESS) {
 *     while(1);
 * }
 *
 * // Run sharedSecretCryptoKey through a key derivation function and
 * // confirm to the other party that we have derived the same key
 *
 *
 * @endcode
 *
 *
 */


#ifndef ti_drivers_ECJPAKE__include
#define ti_drivers_ECJPAKE__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>
#include <ti/drivers/cryptoutils/ecc/ECCParams.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Common ECJPAKE status code reservation offset.
 * ECJPAKE driver implementations should offset status codes with
 * ECJPAKE_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define ECJPAKEXYZ_STATUS_ERROR0    ECJPAKE_STATUS_RESERVED - 0
 * #define ECJPAKEXYZ_STATUS_ERROR1    ECJPAKE_STATUS_RESERVED - 1
 * #define ECJPAKEXYZ_STATUS_ERROR2    ECJPAKE_STATUS_RESERVED - 2
 * @endcode
 */
#define ECJPAKE_STATUS_RESERVED        (-32)

/*!
 * @brief   Successful status code.
 *
 * Functions return ECJPAKE_STATUS_SUCCESS if the function was executed
 * successfully.
 */
#define ECJPAKE_STATUS_SUCCESS         (0)

/*!
 * @brief   Generic error status code.
 *
 * Functions return ECJPAKE_STATUS_ERROR if the function was not executed
 * successfully.
 */
#define ECJPAKE_STATUS_ERROR           (-1)

/*!
 * @brief   An error status code returned if the hardware or software resource
 * is currently unavailable.
 *
 * ECJPAKE driver implementations may have hardware or software limitations on how
 * many clients can simultaneously perform operations. This status code is returned
 * if the mutual exclusion mechanism signals that an operation cannot currently be performed.
 */
#define ECJPAKE_STATUS_RESOURCE_UNAVAILABLE (-2)

/*!
 * @brief   The public key of the other party is not valid.
 *
 * The public key received from the other party is not valid.
 */
#define ECJPAKE_STATUS_INVALID_PUBLIC_KEY (-3)

/*!
 * @brief   The public key of the other party does not lie upon the curve.
 *
 * The public key received from the other party does not lie upon the agreed upon
 * curve.
 */
#define ECJPAKE_STATUS_PUBLIC_KEY_NOT_ON_CURVE (-4)

/*!
 * @brief   A coordinate of the public key of the other party is too large.
 *
 * A coordinate of the public key received from the other party is larger than
 * the prime of the curve. This implies that the point was not correctly
 * generated on that curve.
 */
#define ECJPAKE_STATUS_PUBLIC_KEY_LARGER_THAN_PRIME (-5)

/*!
 * @brief   The result of the operation is the point at infinity.
 *
 * The operation yielded the point at infinity on this curve. This point is
 * not permitted for further use in ECC operations.
 */
#define ECJPAKE_STATUS_POINT_AT_INFINITY (-6)

/*!
 * @brief   The private key passed into the the call is invalid.
 *
 * Private keys must be integers in the interval [1, n - 1], where n is the
 * order of the curve.
 */
#define ECJPAKE_STATUS_INVALID_PRIVATE_KEY (-7)

/*!
 * @brief   The private v passed into the the call is invalid.
 *
 * Private v must be integers in the interval [1, n - 1], where n is the
 * order of the curve.
 */
#define ECJPAKE_STATUS_INVALID_PRIVATE_V (-8)

/*!
 *  @brief  The ongoing operation was canceled.
 */
#define ECJPAKE_STATUS_CANCELED (-9)

/*!
 *  @brief ECJPAKE Global configuration
 *
 *  The ECJPAKE_Config structure contains a set of pointers used to characterize
 *  the ECJPAKE driver implementation.
 *
 *  This structure needs to be defined before calling ECJPAKE_init() and it must
 *  not be changed thereafter.
 *
 *  @sa     ECJPAKE_init()
 */
typedef struct {
    /*! Pointer to a driver specific data object */
    void               *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void         const *hwAttrs;
} ECJPAKE_Config;

/*!
 *  @brief  A handle that is returned from an ECJPAKE_open() call.
 */
typedef ECJPAKE_Config *ECJPAKE_Handle;

/*!
 * @brief   The way in which ECJPAKE function calls return after performing an
 * encryption + authentication or decryption + verification operation.
 *
 * Not all ECJPAKE operations exhibit the specified return behavor. Functions that do not
 * require significant computation and cannot offload that computation to a background thread
 * behave like regular functions. Which functions exhibit the specfied return behavior is
 * implementation dependent. Specifically, a software-backed implementation run on the same
 * CPU as the application will emulate the return behavior while not actually offloading
 * the computation to the background thread.
 *
 * ECJPAKE functions exhibiting the specified return behavior have restrictions on the
 * context from which they may be called.
 *
 * |                                  | Task  | Hwi   | Swi   |
 * |----------------------------------|-------|-------|-------|
 * |ECJPAKE_RETURN_BEHAVIOR_CALLBACK  | X     | X     | X     |
 * |ECJPAKE_RETURN_BEHAVIOR_BLOCKING  | X     |       |       |
 * |ECJPAKE_RETURN_BEHAVIOR_POLLING   | X     | X     | X     |
 *
 */
typedef enum {
    ECJPAKE_RETURN_BEHAVIOR_CALLBACK = 1,   /*!< The function call will return immediately while the
                                             *   ECJPAKE operation goes on in the background. The registered
                                             *   callback function is called after the operation completes.
                                             *   The context the callback function is called (task, HWI, SWI)
                                             *   is implementation-dependent.
                                             */
    ECJPAKE_RETURN_BEHAVIOR_BLOCKING = 2,   /*!< The function call will block while ECJPAKE operation goes
                                             *   on in the background. ECJPAKE operation results are available
                                             *   after the function returns.
                                             */
    ECJPAKE_RETURN_BEHAVIOR_POLLING  = 4,   /*!< The function call will continuously poll a flag while ECJPAKE
                                             *   operation goes on in the background. ECJPAKE operation results
                                             *   are available after the function returns.
                                             */
} ECJPAKE_ReturnBehavior;

/*!
 *  @brief  Struct containing the parameters required to generate the first round of keys.
 */
typedef struct {
    const ECCParams_CurveParams     *curve;                     /*!< A pointer to the elliptic curve parameters
                                                                 *   used in the operation.
                                                                 */
    CryptoKey                       *myPrivateKey1;             /*!< A pointer to a private ECC key. Must
                                                                 *   be of the same length as other params
                                                                 *   of the curve used.
                                                                 */
    CryptoKey                       *myPrivateKey2;             /*!< A pointer to a private ECC key. Must
                                                                 *   be of the same length as other params
                                                                 *   of the curve used.
                                                                 */
    CryptoKey                       *myPublicKey1;              /*!< A pointer to the blank public key of \c
                                                                 *   myPrivateKey1. The keying material will be
                                                                 *   written to the buffer specified in the
                                                                 *   CryptoKey.
                                                                 */
    CryptoKey                       *myPublicKey2;              /*!< A pointer to the blank public key of \c
                                                                 *   myPrivateKey2. The keying material will be
                                                                 *   written to the buffer specified in the
                                                                 *   CryptoKey.
                                                                 */
    CryptoKey                       *myPrivateV1;               /*!< A pointer to a private ECC key used in the
                                                                 *   first Schnorr ZKP.
                                                                 *   Must be of the same length as other params
                                                                 *   of the curve used. The CryptoKey and keying material
                                                                 *   may be deleted or go out of scope after
                                                                 *   generating the ZKP.
                                                                 */
    CryptoKey                       *myPrivateV2;                /*!< A pointer to a private ECC key used in the
                                                                 *   second Schnorr ZKP.
                                                                 *   Must be of the same length as other params
                                                                 *   of the curve used. The CryptoKey and keying material
                                                                 *   may be deleted or go out of scope after
                                                                 *   generating the ZKP.
                                                                 */
    CryptoKey                       *myPublicV1;                /*!< A pointer to the blank public key of \c
                                                                 *   myPrivateV1. The keying material will be
                                                                 *   written to the buffer specified in the
                                                                 *   CryptoKey. The CryptoKey and keying material
                                                                 *   may be deleted or go out of scope after
                                                                 *   generating the hash and sending \c myPublicV2
                                                                 *   to the other party with the rest of the
                                                                 *   parameters.
                                                                 */
    CryptoKey                       *myPublicV2;                /*!< A pointer to the blank public key of \c
                                                                 *   myPrivateV2. The keying material will be
                                                                 *   written to the buffer specified in the
                                                                 *   CryptoKey. The CryptoKey and keying material
                                                                 *   may be deleted or go out of scope after
                                                                 *   generating the hash and sending \c myPublicV2
                                                                 *   to the other party with the rest of the
                                                                 *   parameters.
                                                                 */
} ECJPAKE_OperationRoundOneGenerateKeys;

/*!
 *  @brief  Struct containing the parameters required to generate a ZKP.
 */
typedef struct {
    const ECCParams_CurveParams     *curve;                     /*!< A pointer to the elliptic curve parameters
                                                                 *   used in the operation.
                                                                 */
    const CryptoKey                 *myPrivateKey;              /*!< A pointer to a private ECC key to be signed. Must
                                                                 *   be of the same length as other params
                                                                 *   of the curve used.
                                                                 */
    const CryptoKey                 *myPrivateV;                /*!< A pointer to a private ECC key that will be
                                                                 *   used only to generate a ZKP signature.
                                                                 *   Must be of the same length as other params
                                                                 *   of the curve used.
                                                                 */
    const uint8_t                   *hash;                      /*!< A pointer to the hash of the message.
                                                                 *   Must be of the same length as other params
                                                                 *   of the curve used.
                                                                 */
    uint8_t                         *r;                         /*!< A pointer to where the r component of the
                                                                 *   ZKP will be written to.
                                                                 */
} ECJPAKE_OperationGenerateZKP;

/*!
 *  @brief  Struct containing the parameters required to verify a ZKP.
 */
typedef struct {
    const ECCParams_CurveParams     *curve;                     /*!< A pointer to the elliptic curve parameters
                                                                 *   used in the operation.
                                                                 */
    const CryptoKey                 *theirGenerator;            /*!< A CryptoKey describing the generator point
                                                                 *   to be used. In the first round, this will
                                                                 *   be the default generator of the curve.
                                                                 *   In the second round, this parameter is
                                                                 *   computed by ECJPAKE_roundTwoGenerateKeys().
                                                                 *
                                                                 *   Formatted as a public key. If  NULL, default
                                                                 *   generator point from @c curve is used.
                                                                 */
    const CryptoKey                 *theirPublicKey;            /*!< A CryptoKey describing the public key
                                                                 *   received from the other party that the
                                                                 *   ZKP to be verified supposedly signed.
                                                                 */
    const CryptoKey                 *theirPublicV;              /*!< A CryptoKey describing the public V of the
                                                                 *   ZKP. Received from the other party.
                                                                 */
    const uint8_t                   *hash;                      /*!< The hash of the ZKP generated as the
                                                                 *   other party generated it to compute r.
                                                                 */
    const uint8_t                   *r;                         /*!< R component of the ZKP signature. Received
                                                                 *   from the other party.
                                                                 */
} ECJPAKE_OperationVerifyZKP;

/*!
 *  @brief  Struct containing the parameters required to generate the second round keys.
 */
typedef struct {
    const ECCParams_CurveParams     *curve;                     /*!< A pointer to the elliptic curve parameters
                                                                 *   used in the operation.
                                                                 */
    const CryptoKey                 *myPrivateKey2;             /*!< A pointer to a private ECC key. Must
                                                                 *   be of the same length as other params
                                                                 *   of the curve used. Generated in round one.
                                                                 */
    const CryptoKey                 *myPublicKey1;              /*!< A pointer to the public key of
                                                                 *   myPrivateKey1. Generated in round one.
                                                                 */
    const CryptoKey                 *myPublicKey2;              /*!< A pointer to the second public key.
                                                                 *   Generated in round one.
                                                                 */
    const CryptoKey                 *theirPublicKey1;           /*!< A CryptoKey describing the first public key
                                                                 *   received from the other party.
                                                                 */
    const CryptoKey                 *theirPublicKey2;           /*!< A CryptoKey describing the second public key
                                                                 *   received from the other party.
                                                                 */
    const CryptoKey                 *preSharedSecret;           /*!< A CryptoKey describing the secret shared between
                                                                 *   the two parties prior to starting the scheme.
                                                                 *   This exchange would have happened through some
                                                                 *   offline commissioning scheme most likely.
                                                                 *   The driver expects an integer of the same length
                                                                 *   as the curve parameters of the curve in use as
                                                                 *   keying material even if the original pre-shared
                                                                 *   secret is shorter than this length.
                                                                 */
    CryptoKey                       *theirNewGenerator;         /*!< A blank CryptoKey describing the generator point
                                                                 *   used by the other party in the second round.
                                                                 *   After it is computed, the keying material will
                                                                 *   be written to the location described in the
                                                                 *   CryptoKey.
                                                                 *   Formatted as a public key.
                                                                 */
    CryptoKey                       *myNewGenerator;            /*!< A blank CryptoKey describing the generator point
                                                                 *   used by the application in the second round.
                                                                 *   After it is computed, the keying material will
                                                                 *   be written to the location described in the
                                                                 *   CryptoKey.
                                                                 *   Formatted as a public key.
                                                                 */
    CryptoKey                       *myCombinedPrivateKey;      /*!< A pointer to a public ECC key. Must
                                                                 *   be of the same length as other params
                                                                 *   of the curve used. Result of multiplying
                                                                 *   \c myCombinedPrivateKey by \c myNewGenerator.
                                                                 */
    CryptoKey                       *myCombinedPublicKey;       /*!< A pointer to a public ECC key. Result of multiplying
                                                                 *   \c myCombinedPrivateKey by \c myNewGenerator.
                                                                 */
    CryptoKey                       *myPrivateV;                /*!< A pointer to a private ECC key used in the
                                                                 *   only second-round Schnorr ZKP.
                                                                 *   Must be of the same length as other params
                                                                 *   of the curve used. The CryptoKey and keying material
                                                                 *   may be deleted or go out of scope after
                                                                 *   generating the ZKP.
                                                                 */
    CryptoKey                       *myPublicV;                 /*!< A pointer to the blank public key of \c
                                                                 *   myPrivateV. The keying material will be
                                                                 *   written to the buffer specified in the
                                                                 *   CryptoKey. The CryptoKey and keying material
                                                                 *   may be deleted or go out of scope after
                                                                 *   generating the hash and sending \c myPublicV2
                                                                 *   to the other party with the rest of the
                                                                 *   parameters.
                                                                 */
} ECJPAKE_OperationRoundTwoGenerateKeys;

/*!
 *  @brief  Struct containing the parameters required to compute the shared secret.
 */
typedef struct {
    const ECCParams_CurveParams     *curve;                     /*!< A pointer to the elliptic curve parameters
                                                                 *   used in the operation.
                                                                 */
    const CryptoKey                 *myCombinedPrivateKey;      /*!< A pointer to a private ECC key. Must
                                                                 *   be of the same length as other params
                                                                 *   of the curve used. Generated in round one.
                                                                 */
    const CryptoKey                 *theirCombinedPublicKey;    /*!< A CryptoKey describing the second public key
                                                                 *   received from the other party.
                                                                 */
    const CryptoKey                 *theirPublicKey2;           /*!< A pointer to a private ECC key. Must
                                                                 *   be of the same length as other params
                                                                 *   of the curve used. Result of multiplying
                                                                 *   \c myPrivateKey2 by \c preSharedSecret.
                                                                 */
    const CryptoKey                 *myPrivateKey2;             /*!< Combined public key received in the second
                                                                 *   round and verified by the application against
                                                                 *   the second round ZKP signature.
                                                                 */
    CryptoKey                       *sharedSecret;              /*!< The shared secret that is identical between both
                                                                 *   parties. Formatted as a public key.
                                                                 */
} ECJPAKE_OperationComputeSharedSecret;


/*!
 *  @brief  Union containing pointers to all supported operation structs.
 */
typedef union {
    ECJPAKE_OperationRoundOneGenerateKeys   *generateRoundOneKeys;  /*!< A pointer to an ECJPAKE_OperationRoundOneGenerateKeys struct */
    ECJPAKE_OperationGenerateZKP            *generateZKP;           /*!< A pointer to an ECJPAKE_OperationGenerateZKP struct */
    ECJPAKE_OperationVerifyZKP              *verifyZKP;             /*!< A pointer to an ECJPAKE_OperationVerifyZKP struct */
    ECJPAKE_OperationRoundTwoGenerateKeys   *generateRoundTwoKeys;  /*!< A pointer to an ECJPAKE_OperationRoundTwoGenerateKeys struct */
    ECJPAKE_OperationComputeSharedSecret    *computeSharedSecret;   /*!< A pointer to an ECJPAKE_OperationComputeSharedSecret struct */
} ECJPAKE_Operation;

/*!
 *  @brief  Enum for the operation types supported by the driver.
 */
typedef enum {
    ECJPAKE_OPERATION_TYPE_ROUND_ONE_GENERATE_KEYS = 1,
    ECJPAKE_OPERATION_TYPE_GENERATE_ZKP = 2,
    ECJPAKE_OPERATION_TYPE_VERIFY_ZKP = 3,
    ECJPAKE_OPERATION_TYPE_ROUND_TWO_GENERATE_KEYS = 4,
    ECJPAKE_OPERATION_TYPE_COMPUTE_SHARED_SECRET = 5,
} ECJPAKE_OperationType;

/*!
 *  @brief  The definition of a callback function used by the ECJPAKE driver
 *          when used in ::ECJPAKE_RETURN_BEHAVIOR_CALLBACK
 *
 *  @param  handle Handle of the client that started the ECJPAKE operation.
 *
 *  @param  returnStatus The result of the ECJPAKE operation. May contain an error code
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
typedef void (*ECJPAKE_CallbackFxn) (ECJPAKE_Handle handle,
                                     int_fast16_t returnStatus,
                                     ECJPAKE_Operation operation,
                                     ECJPAKE_OperationType operationType);

/*!
 *  @brief  ECJPAKE Parameters
 *
 *  ECJPAKE Parameters are used to with the ECJPAKE_open() call. Default values for
 *  these parameters are set using ECJPAKE_Params_init().
 *
 *  @sa     ECJPAKE_Params_init()
 */
typedef struct {
    ECJPAKE_ReturnBehavior  returnBehavior;                     /*!< Blocking, callback, or polling return behavior */
    ECJPAKE_CallbackFxn     callbackFxn;                        /*!< Callback function pointer */
    uint32_t                timeout;                            /*!< Timeout in system ticks before the operation fails
                                                                 *   and returns
                                                                 */
    void                   *custom;                             /*!< Custom argument used by driver
                                                                 *   implementation
                                                                 */
} ECJPAKE_Params;

/*!
 *  @brief  This function initializes the ECJPAKE module.
 *
 *  @pre    The ECJPAKE_config structure must exist and be persistent before this
 *          function can be called. This function must also be called before
 *          any other ECJPAKE driver APIs. This function call does not modify any
 *          peripheral registers.
 */
void ECJPAKE_init(void);

/*!
 *  @brief  Function to initialize an ECJPAKE_OperationRoundOneGenerateKeys struct to its defaults
 *
 *  @param  operation   A pointer to ECJPAKE_OperationRoundOneGenerateKeys structure for
 *                      initialization
 *
 *  Defaults values are all zeros.
 */
void ECJPAKE_OperationRoundOneGenerateKeys_init(ECJPAKE_OperationRoundOneGenerateKeys *operation);

/*!
 *  @brief  Function to initialize an ECJPAKE_OperationGenerateZKP struct to its defaults
 *
 *  @param  operation   A pointer to ECJPAKE_OperationGenerateZKP structure for
 *                      initialization
 *
 *  Defaults values are all zeros.
 */
void ECJPAKE_OperationGenerateZKP_init(ECJPAKE_OperationGenerateZKP *operation);

/*!
 *  @brief  Function to initialize an ECJPAKE_OperationVerifyZKP struct to its defaults
 *
 *  @param  operation   A pointer to ECJPAKE_OperationVerifyZKP structure for
 *                      initialization
 *
 *  Defaults values are all zeros.
 */
void ECJPAKE_OperationVerifyZKP_init(ECJPAKE_OperationVerifyZKP *operation);

/*!
 *  @brief  Function to initialize an ECJPAKE_OperationRoundTwoGenerateKeys struct to its defaults
 *
 *  @param  operation   A pointer to ECJPAKE_OperationRoundTwoGenerateKeys structure for
 *                      initialization
 *
 *  Defaults values are all zeros.
 */
void ECJPAKE_OperationRoundTwoGenerateKeys_init(ECJPAKE_OperationRoundTwoGenerateKeys *operation);


/*!
 *  @brief  Function to initialize an ECJPAKE_OperationComputeSharedSecret struct to its defaults
 *
 *  @param  operation   A pointer to ECJPAKE_OperationComputeSharedSecret structure for
 *                      initialization
 *
 *  Defaults values are all zeros.
 */
void ECJPAKE_OperationComputeSharedSecret_init(ECJPAKE_OperationComputeSharedSecret *operation);

/*!
 *  @brief  Function to close an ECJPAKE peripheral specified by the ECJPAKE handle
 *
 *  @pre    ECJPAKE_open() has to be called first.
 *
 *  @param  handle An ECJPAKE handle returned from ECJPAKE_open()
 *
 *  @sa     ECJPAKE_open()
 */
void ECJPAKE_close(ECJPAKE_Handle handle);

/*!
 *  @brief  This function opens a given ECJPAKE peripheral.
 *
 *  @pre    ECJPAKE controller has been initialized using ECJPAKE_init()
 *
 *  @param  index         Logical peripheral number for the ECJPAKE indexed into
 *                        the ECJPAKE_config table
 *
 *  @param  params        Pointer to an parameter block, if NULL it will use
 *                        default values.
 *
 *  @return An ECJPAKE_Handle on success or a NULL on an error or if it has
 *          been opened already.
 *
 *  @sa     ECJPAKE_init()
 *  @sa     ECJPAKE_close()
 */
ECJPAKE_Handle ECJPAKE_open(uint_least8_t index, ECJPAKE_Params *params);

/*!
 *  @brief  Function to initialize the ECJPAKE_Params struct to its defaults
 *
 *  @param  params      An pointer to ECJPAKE_Params structure for
 *                      initialization
 *
 *  Defaults values are:
 *      returnBehavior              = ECJPAKE_RETURN_BEHAVIOR_BLOCKING
 *      callbackFxn                 = NULL
 *      timeout                     = SemaphoreP_WAIT_FOREVER
 *      custom                      = NULL
 */
void ECJPAKE_Params_init(ECJPAKE_Params *params);

/*!
 *  @brief Generates all public and private keying material for the first round of
 *         the EC-JPAKE scheme.
 *
 *  This function generates all public and private keying material required for
 *  the first round of the EC-JPAKE scheme.
 *
 *  @param [in]     handle          An ECJPAKE handle returned from ECJPAKE_open()
 *
 *  @param [in]     operation       A pointer to a struct containing the requisite
 *                                  parameters to execute the function.
 *
 *  @pre Call ECJPAKE_OperationRoundOneGenerateKeys_init() on /c operation.
 *
 *  @post Generate the two sets of hashes and ZKPs for the two public/private key pairs.
 *
 *  @retval #ECJPAKE_STATUS_SUCCESS                The operation succeeded.
 *  @retval #ECJPAKE_STATUS_ERROR                  The operation failed.
 *  @retval #ECJPAKE_STATUS_RESOURCE_UNAVAILABLE   The required hardware resource was not available. Try again later.
 *  @retval #ECJPAKE_STATUS_CANCELED               The operation was canceled.
 *  @retval #ECJPAKE_STATUS_POINT_AT_INFINITY      The computed public key is the point at infinity.
 *  @retval #ECJPAKE_STATUS_INVALID_PRIVATE_KEY    The private key passed into the the call is invalid.
 *  @retval #ECJPAKE_STATUS_INVALID_PRIVATE_V      The private v passed into the the call is invalid.
 *
 */
int_fast16_t ECJPAKE_roundOneGenerateKeys(ECJPAKE_Handle handle, ECJPAKE_OperationRoundOneGenerateKeys *operation);

/*!
 *  @brief Generates the \c r component of a Schnorr Zero-Knowledge Proof (ZKP) signature.
 *
 *  This function generates the \c r component of a ZKP using the hash and private
 *  keys. The hash must be computed prior.
 *  This function does not compute the hash for the application. There is no strictly
 *  defined bit-level implementation guideline for generating the hash in the EC-JPAKE
 *  scheme. Hence, interoperability could not be guaranteed between different EC-JPAKE
 *  implementations. Usually, the hash will be a concatenation of the public V, public
 *  key, generator point, and user ID. There may be other components such as length
 *  fields mixed in.
 *
 *  @param [in]     handle          An ECJPAKE handle returned from ECJPAKE_open()
 *
 *  @param [in]     operation       A pointer to a struct containing the requisite
 *                                  parameters to execute the function.
 *
 *  @pre    If in round one, ECJPAKE_roundOneGenerateKeys() must be called prior.
 *          Else, ECJPAKE_roundTwoGenerateKeys() must be called prior. The hash
 *          must also have been computed prior to calling this function.
 *          Call ECJPAKE_OperationGenerateZKP_init() on /c operation.
 *
 *
 *  @post   Send all ZKP signatures (\c r, public V, user ID) together with the
 *          public keys to the other party.
 *
 *
 *  @retval #ECJPAKE_STATUS_SUCCESS                The operation succeeded.
 *  @retval #ECJPAKE_STATUS_ERROR                  The operation failed.
 *  @retval #ECJPAKE_STATUS_RESOURCE_UNAVAILABLE   The required hardware resource was not available. Try again later.
 *  @retval #ECJPAKE_STATUS_CANCELED               The operation was canceled.
 */
int_fast16_t ECJPAKE_generateZKP(ECJPAKE_Handle handle, ECJPAKE_OperationGenerateZKP *operation);

 /*!
 *  @brief Verifies a Schnorr Zero-Knowledge Proof (ZKP) signature.
 *
 *  This function computes if a received Schnorr ZKP correctly verifies
 *  a received public key.
 *
 *  @param [in]     handle          An ECJPAKE handle returned from ECJPAKE_open()
 *
 *  @param [in]     operation       A pointer to a struct containing the requisite
 *                                  parameters to execute the function.
 *
 *  @pre    Receive the relevant ZKP signature parameters. Compute the hash.
 *          If in the second round, compute the generator first by calling
 *          ECJPAKE_roundTwoGenerateKeys().
 *          Call ECJPAKE_OperationVerifyZKP_init() on /c operation.
 *
 *  @retval #ECJPAKE_STATUS_SUCCESS                         The operation succeeded.
 *  @retval #ECJPAKE_STATUS_ERROR                           The operation failed. Signature did not verify correctly.
 *  @retval #ECJPAKE_STATUS_RESOURCE_UNAVAILABLE            The required hardware resource was not available. Try again later.
 *  @retval #ECJPAKE_STATUS_CANCELED                        The operation was canceled.
 *  @retval #ECJPAKE_STATUS_PUBLIC_KEY_NOT_ON_CURVE         The public key of the other party does not lie upon the curve.
 *  @retval #ECJPAKE_STATUS_PUBLIC_KEY_LARGER_THAN_PRIME    A coordinate of the public key of the other party is too large.
 */
int_fast16_t ECJPAKE_verifyZKP(ECJPAKE_Handle handle, ECJPAKE_OperationVerifyZKP *operation);

/*!
 *  @brief Generates all public and private keying material for the first round of
 *         the EC-JPAKE scheme.
 *
 *  This function generates all public and private keying material required for
 *  the first round of the EC-JPAKE scheme.
 *
 *  @param [in]     handle          An ECJPAKE handle returned from ECJPAKE_open()
 *
 *  @param [in]     operation       A pointer to a struct containing the requisite
 *                                  parameters to execute the function.
 *
 *  @pre Call ECJPAKE_OperationRoundTwoGenerateKeys_init() on /c operation.
 *
 *  @post Generate the hash and ZKP signature for the second round public/private key.
 *
 *  @retval #ECJPAKE_STATUS_SUCCESS                The operation succeeded.
 *  @retval #ECJPAKE_STATUS_ERROR                  The operation failed.
 *  @retval #ECJPAKE_STATUS_RESOURCE_UNAVAILABLE   The required hardware resource was not available. Try again later.
 *  @retval #ECJPAKE_STATUS_CANCELED               The operation was canceled.
 *  @retval #ECJPAKE_STATUS_INVALID_PRIVATE_KEY    The private key passed into the the call is invalid.
 *  @retval #ECJPAKE_STATUS_INVALID_PRIVATE_V      The private v passed into the the call is invalid.
 */
int_fast16_t ECJPAKE_roundTwoGenerateKeys(ECJPAKE_Handle handle, ECJPAKE_OperationRoundTwoGenerateKeys *operation);

/*!
 *  @brief Computes the shared secret.
 *
 *  This function computes the shared secret between both parties. The shared
 *  secret is a point on the elliptic curve and is used to further derive the
 *  symmetric session key via a key derivation function.
 *
 *  @param [in]     handle          An ECJPAKE handle returned from ECJPAKE_open()
 *
 *  @param [in]     operation       A pointer to a struct containing the requisite
 *                                  parameters to execute the function.
 *
 *  @pre    Call ECJPAKE_OperationComputeSharedSecret_init() on /c operation.
 *
 *  @post   The shared secret must be processed by a key derivation function to
 *          compute the symmetric session key. It is recommended that the two parties
 *          prove to each other that they are in posession of the symmetric session
 *          key. While this should be implied by the successful verification of
 *          the three ZKPs in the scheme, it is nonetheless good practice.
 *
 *  @retval #ECJPAKE_STATUS_SUCCESS                The operation succeeded.
 *  @retval #ECJPAKE_STATUS_ERROR                  The operation failed.
 *  @retval #ECJPAKE_STATUS_RESOURCE_UNAVAILABLE   The required hardware resource was not available. Try again later.
 *  @retval #ECJPAKE_STATUS_CANCELED               The operation was canceled.
 */
int_fast16_t ECJPAKE_computeSharedSecret(ECJPAKE_Handle handle, ECJPAKE_OperationComputeSharedSecret *operation);

/*!
 *  @brief Cancels an ongoing ECJPAKE operation.
 *
 *  Asynchronously cancels an ECJPAKE operation. Only available when using
 *  ECJPAKE_RETURN_BEHAVIOR_CALLBACK or ECJPAKE_RETURN_BEHAVIOR_BLOCKING.
 *  The operation will terminate as though an error occured. The
 *  return status code of the operation will be ECJPAKE_STATUS_CANCELED.
 *
 *  @param  handle Handle of the operation to cancel
 *
 *  @retval #ECJPAKE_STATUS_SUCCESS               The operation was canceled.
 *  @retval #ECJPAKE_STATUS_ERROR                 The operation was not canceled. There may be no operation to cancel.
 */
int_fast16_t ECJPAKE_cancelOperation(ECJPAKE_Handle handle);

/**
 *  @brief  Constructs a new ECJPAKE object
 *
 *  Unlike #ECJPAKE_open(), #ECJPAKE_construct() does not require the hwAttrs and
 *  object to be allocated in a #ECJPAKE_Config array that is indexed into.
 *  Instead, the #ECJPAKE_Config, hwAttrs, and object can be allocated at any
 *  location. This allows for relatively simple run-time allocation of temporary
 *  driver instances on the stack or the heap.
 *  The drawback is that this makes it more difficult to write device-agnostic
 *  code. If you use an ifdef with DeviceFamily, you can choose the correct
 *  object and hwAttrs to allocate. That compilation unit will be tied to the
 *  device it was compiled for at this point. To change devices, recompilation
 *  of the application with a different DeviceFamily setting is necessary.
 *
 *  @param  config #ECJPAKE_Config describing the location of the object and hwAttrs.
 *
 *  @param  params #ECJPAKE_Params to configure the driver instance.
 *
 *  @return        Returns a #ECJPAKE_Handle on success or NULL on failure.
 *
 *  @pre    The object struct @c config points to must be zeroed out prior to
 *          calling this function. Otherwise, unexpected behavior may ensue.
 */
ECJPAKE_Handle ECJPAKE_construct(ECJPAKE_Config *config, const ECJPAKE_Params *params);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_ECJPAKE__include */
