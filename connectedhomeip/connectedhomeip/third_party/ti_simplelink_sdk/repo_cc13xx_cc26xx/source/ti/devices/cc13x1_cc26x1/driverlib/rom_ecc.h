/******************************************************************************
*  Filename:       ECC.h
*  Revised:        2016-09-19 10:36:17 +0200 (Mon, 19 Sep 2016)
*  Revision:       47179
*
*  Description:    Defines and prototypes for the NIST-P256 curve constants to
*                  be used with the ECC SW implementation.
*
*  Copyright (c) 2015 - 2021, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#ifndef __ROM_ECC_H__
#define __ROM_ECC_H__

#include <stdint.h>
#include "rom.h"

/******************************************************************************/
/***                              DEFINES                                   ***/
/******************************************************************************/

/*! \brief Length in bytes of NISTP256 curve parameters excluding the prepended
 *  length word.
 */
#define ECC_NISTP256_PARAM_LENGTH_BYTES 32

/*! \brief Number of bytes for the length word prepended before all parameters
 *  passed into the ECC functions.
 */
#define ECC_LENGTH_OFFSET_BYTES 4

/*! \brief Length in bytes of NISTP256 curve parameters including the prepended
 *  length word.
 */
#define ECC_NISTP256_PARAM_LENGTH_WITH_OFFSET_BYTES (ECC_NISTP256_PARAM_LENGTH_BYTES + ECC_LENGTH_OFFSET_BYTES)

/******************************************************************************/
/***                               STATUS                                   ***/
/******************************************************************************/

// Scalar multiplication status

#define STATUS_MODULUS_EVEN                     0xDC
#define STATUS_MODULUS_LARGER_THAN_255_WORDS    0xD2
#define STATUS_MODULUS_LENGTH_ZERO              0x08
#define STATUS_MODULUS_MSW_IS_ZERO              0x30
#define STATUS_SCALAR_TOO_LONG                  0x35
#define STATUS_SCALAR_LENGTH_ZERO               0x53
#define STATUS_ORDER_TOO_LONG                   0xC6
#define STATUS_ORDER_LENGTH_ZERO                0x6C
#define STATUS_X_COORD_TOO_LONG                 0x3C
#define STATUS_X_COORD_LENGTH_ZERO              0xC3
#define STATUS_Y_COORD_TOO_LONG                 0x65
#define STATUS_Y_COORD_LENGTH_ZERO              0x56
#define STATUS_A_COEF_TOO_LONG                  0x5C
#define STATUS_A_COEF_LENGTH_ZERO               0xC5
#define STATUS_BAD_WINDOW_SIZE                  0x66
#define STATUS_SCALAR_MUL_OK                    0x99

// ECDSA and ECDH status

#define STATUS_ORDER_LARGER_THAN_255_WORDS      0x28
#define STATUS_ORDER_EVEN                       0x82
#define STATUS_ORDER_MSW_IS_ZERO                0x23
#define STATUS_ECC_KEY_TOO_LONG                 0x25
#define STATUS_ECC_KEY_LENGTH_ZERO              0x52
#define STATUS_ECC_X_LARGER_THAN_PRIME          0x11
#define STATUS_ECC_Y_LARGER_THAN_PRIME          0x12
#define STATUS_ECC_X_ZERO                       0x13
#define STATUS_ECC_Y_ZERO                       0x14
#define STATUS_ECC_POINT_NOT_ON_CURVE           0x15
#define STATUS_ECC_POINT_ON_CURVE               0x16
#define STATUS_PRIVATE_KEY_ZERO                 0x17
#define STATUS_PRIVATE_KEY_LARGER_EQUAL_ORDER   0x18
#define STATUS_PRIVATE_VALID                    0x19
#define STATUS_DIGEST_TOO_LONG                  0x27
#define STATUS_DIGEST_LENGTH_ZERO               0x72
#define STATUS_ECDSA_SIGN_OK                    0x32
#define STATUS_ECDSA_INVALID_SIGNATURE          0x5A
#define STATUS_ECDSA_VALID_SIGNATURE            0xA5
#define STATUS_SIG_P1_TOO_LONG                  0x11
#define STATUS_SIG_P1_LENGTH_ZERO               0x12
#define STATUS_SIG_P2_TOO_LONG                  0x22
#define STATUS_SIG_P2_LENGTH_ZERO               0x21

#define STATUS_ECDSA_KEYGEN_OK                  STATUS_SCALAR_MUL_OK
#define STATUS_ECDH_KEYGEN_OK                   STATUS_SCALAR_MUL_OK
#define STATUS_ECDH_COMMON_KEY_OK               STATUS_SCALAR_MUL_OK

/******************************************************************************/
/***                              TYPES                                     ***/
/******************************************************************************/

typedef union {
    uint8_t     byte[ECC_NISTP256_PARAM_LENGTH_WITH_OFFSET_BYTES];
    uint32_t    word[ECC_NISTP256_PARAM_LENGTH_WITH_OFFSET_BYTES / sizeof(uint32_t)];
} ECC_NISTP256_Param;


/******************************************************************************/
/***                        GLOBAL VARIABLES                                ***/
/******************************************************************************/
//*****************************************************************************
//
//! \brief X coordinate of the generator point of the ECC_NISTP256 curve.
//
//*****************************************************************************
extern const ECC_NISTP256_Param ECC_NISTP256_generatorX;

//*****************************************************************************
//
//! \brief Y coordinate of the generator point of the ECC_NISTP256 curve.
//
//*****************************************************************************
extern const ECC_NISTP256_Param ECC_NISTP256_generatorY;

//*****************************************************************************
//
//! \brief prime of the ECC_NISTP256 curve.
//
//*****************************************************************************
extern const ECC_NISTP256_Param ECC_NISTP256_prime;

//*****************************************************************************
//
//! \brief a constant of the ECC_NISTP256 curve when expressed in short
//! Weierstrass form (y^3 = x^2 + a*x + b).
//
//*****************************************************************************
extern const ECC_NISTP256_Param ECC_NISTP256_a;

//*****************************************************************************
//
//! \brief b constant of the ECC_NISTP256 curve when expressed in short
//! Weierstrass form (y^3 = x^2 + a*x + b).
//
//*****************************************************************************
extern const ECC_NISTP256_Param ECC_NISTP256_b;

//*****************************************************************************
//
//! \brief order of the ECC_NISTP256 curve.
//
//*****************************************************************************
extern const ECC_NISTP256_Param ECC_NISTP256_order;

//*****************************************************************************
//
//! \brief k in Montgomery domain of the ECC_NISTP256 curve.
//
//*****************************************************************************
extern const ECC_NISTP256_Param ECC_NISTP256_k_mont;

//*****************************************************************************
//
//! \brief a in Montgomery domain of the ECC_NISTP256 curve.
//
//*****************************************************************************
extern const ECC_NISTP256_Param ECC_NISTP256_a_mont;

//*****************************************************************************
//
//! \brief b in Montgomery domain of the ECC_NISTP256 curve.
//
//*****************************************************************************
extern const ECC_NISTP256_Param ECC_NISTP256_b_mont;

//*****************************************************************************
//
//! \brief X coordinate of the generator point in Montgomery domain
//! of the ECC_NISTP256 curve.
//
//*****************************************************************************
extern const ECC_NISTP256_Param ECC_NISTP256_gx_mont;

//*****************************************************************************
/*!
 * \brief Initialize elliptic curve parameters to default values and specify workzone.
 *
 * This function initializes the elliptic curve parameters to default values.
 * The default elliptic curve used is NIST-P256.
 *
 * The workzone defaults to an expected window size of 3.
 *
 * This function can be called again to point the ECC workzone at
 * a different memory buffer.
 *
 * @param state     Struct to keep track of the state of the operation.
 * @param workzone  Pointer to memory allocated for computations, input.
 *                  See description at beginning of ECC section for
 *                  memory requirements.
 *
 * \return None
 */
//*****************************************************************************
extern void ECC_initialize(ECC_State *state, uint32_t *workzone);

//*****************************************************************************
/*!
 * \brief Initialize elliptic curve parameters to specified values and specify workzone.
 *
 * This function may be used to explicitly specify the curve parameters used
 * by the ECC in ROM implementation.
 *
 * All curve parameters must be prepended with a length word specifying the
 * length of the parameter in 32-bit words excluding the length word itself.
 * For NIST-P256, the length word is 8.
 *
 * @param state         Struct to keep track of the state of the operation.
 * @param workzone      Pointer to memory allocated for computations, input.
 *                      See description at beginning of ECC section for
 *                      memory requirements.
 * @param windowSize    Window size of \c workzone. Default value is 3.
 * @param prime         Curve prime
 * @param order         Curve order
 * @param a             Curve a value
 * @param b             Curve b value
 * @param generatorX    X coordinate of generator point
 * @param generatorY    Y coordinate of generator point
 */
extern void ECC_init(ECC_State *state,
                     uint32_t *workzone,
                     uint8_t   windowSize,
                     const uint32_t *prime,
                     const uint32_t *order,
                     const uint32_t *a,
                     const uint32_t *b,
                     const uint32_t *generatorX,
                     const uint32_t *generatorY);

//*****************************************************************************
 /*!
 * \brief Generate a public key.
 *
 * This is used for both ECDH and ECDSA.
 *
 * \param [in,out] state    Struct to keep track of the state of the operation
 * \param [in] randEntropy  Pointer to random string
 * \param [out] privateKey  Pointer to the private key. May be the same location
 *                          as @c randEntropy.
 * \param [out] publicKey_x Pointer to public key X-coordinate
 * \param [out] publicKey_y Pointer to public key Y-coordinate
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t ECC_generateKey(ECC_State *state,
                               uint32_t *randEntropy,
                               uint32_t *privateKey,
                               uint32_t *publicKey_x,
                               uint32_t *publicKey_y);

//*****************************************************************************
/*!
 * \brief Sign message digest.
 *
 * \param [in,out] state    Struct to keep track of the state of the operation
 * \param [in] privateKey   Pointer to the secret key
 * \param [in] hash         Pointer to the message
 * \param [in] pmsn         Pointer to random string
 * \param [out] r           Pointer to r component of signature
 * \param [out] s           Pointer to s component of signature
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t ECC_ECDSA_sign(ECC_State *state,
                              uint32_t *privateKey,
                              uint32_t *hash,
                              uint32_t *pmsn,
                              uint32_t *r,
                              uint32_t *s);

//*****************************************************************************
/*!
 * \brief Verify signature.
 *
 * \param [in,out] state   Struct to keep track of the state of the operation
 * \param [in] publicKey_x Pointer to public key X-coordinate
 * \param [in] publicKey_y Pointer to public key Y-coordinate
 * \param [in] hash        Pointer to hash of message digest
 * \param [in] r       Pointer to r component of signature
 * \param [in] s       Pointer to s component of signature
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t ECC_ECDSA_verify(ECC_State *state,
                                uint32_t *publicKey_x,
                                uint32_t *publicKey_y,
                                uint32_t *hash,
                                uint32_t *r,
                                uint32_t *s);

//*****************************************************************************
/*!
 * \brief Compute the shared secret.
 *
 * \param [in,out] state      Struct to keep track of the state of the operation
 * \param [in] privateKey     Pointer to private key
 * \param [in] publicKey_x    Pointer to public key X-coordinate
 * \param [in] publicKey_y    Pointer to public key Y-coordinate
 * \param [out] sharedSecret_x Pointer to shared secret X-coordinate
 * \param [out] sharedSecret_y Pointer to shared secret Y-coordinate
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t ECC_ECDH_computeSharedSecret(ECC_State *state,
                                            uint32_t *privateKey,
                                            uint32_t *publicKey_x,
                                            uint32_t *publicKey_y,
                                            uint32_t *sharedSecret_x,
                                            uint32_t *sharedSecret_y);

//*****************************************************************************
/*!
 * \brief Validate a private key against the currently set elliptic curve
 *
 *  Validates that \c private key is within [1, n-1] where n is the order of
 *  the elliptic curve.
 *
 * \param [in,out] state Struct to keep track of the state of the operation
 * \param privateKey    Pointer to public key X-coordinate, input.
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t ECC_validatePrivateKey(ECC_State *state,
                                      uint32_t *privateKey);

//*****************************************************************************
/*!
 * \brief Validate a public key against the currently set elliptic curve
 *
 * \param [in,out] state Struct to keep track of the state of the operation
 * \param publicKey_x    Pointer to public key X-coordinate, input.
 * \param publicKey_y    Pointer to public key Y-coordinate, input.
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t ECC_validatePublicKey(ECC_State *state,
                                     uint32_t *publicKey_x,
                                     uint32_t *publicKey_y);


#endif // __ROM_ECC_H__
