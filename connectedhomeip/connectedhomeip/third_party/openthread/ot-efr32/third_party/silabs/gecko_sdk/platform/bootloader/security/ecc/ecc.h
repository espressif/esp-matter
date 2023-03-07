/***************************************************************************//**
 * @file
 * @brief ECC API definition.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef ECC_H
#define ECC_H

#include "em_device.h"
#include "api/btl_errorcode.h"

#include <stdint.h>
#include <stdbool.h>

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup ECDSA
 * @{
 * @addtogroup ECC ECC Library
 * @brief Elliptic Curve Cryptography Library
 * @details
 *   The Elliptic Curve Cryptography (ECC) API includes ECDSA verification
 *   on one of the elliptic curves recommended by NIST in
 *   csrc.nist.gov/groups/ST/toolkit/documents/dss/NISTReCur.pdf :
 *   * secp256r1: NIST/SECG X9.62 curve over a 256 bit prime field
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   TYPEDEFS   **********************************
 ******************************************************************************/

/** ECC big integer size in bits. */
#define ECC_BIGINT_SIZE_IN_BITS        (256)

/** ECC big integer size in bytes. */
#define ECC_BIGINT_SIZE_IN_BYTES       (ECC_BIGINT_SIZE_IN_BITS / 8)

/** ECC big integer size in words. */
#define ECC_BIGINT_SIZE_IN_32BIT_WORDS (ECC_BIGINT_SIZE_IN_BYTES \
                                        / sizeof(uint32_t))

/** ECC big integer type */
typedef uint32_t ECC_BigInt_t[ECC_BIGINT_SIZE_IN_32BIT_WORDS];

/** Elliptic curve point structure. */
typedef struct {
  ECC_BigInt_t  X;  /**< x coordinate of point. */
  ECC_BigInt_t  Y;  /**< y coordinate of point. */
} ECC_Point_t;

/** ECDSA signature as defined in
 *     FIPS PUB 186-3, Digital Signature Standard (DSS). */
typedef struct {
  ECC_BigInt_t    r;  /**< The r component of the signature. */
  ECC_BigInt_t    s;  /**< The s component of the signature. */
} ECC_EcdsaSignature_t;

/*******************************************************************************
 ******************************   Functions   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Verify an ECDSA signature.
 *
 * @details
 *   TBW
 *
 * @param[in]  crypto
 *   Pointer to CRYPTO peripheral instance
 *
 * @param[in]  msgDigest
 *   The message digest associated with the signature.
 *
 * @param[in]  msgDigestLen
 *   The length of the message digest.
 *
 * @param[in]  publicKey
 *   Public key of entity that generated signature.
 *
 * @param[out] signature
 *   The signature to verify.
 *
 * @return     Error code.
 ******************************************************************************/
int32_t ECC_ECDSA_VerifySignatureP256(CRYPTO_TypeDef         *crypto,
                                      const uint8_t          *msgDigest,
                                      int                    msgDigestLen,
                                      const ECC_Point_t      *publicKey,
                                      ECC_EcdsaSignature_t   *signature);

/***************************************************************************//**
 * @brief
 *  Convert a large integer from a hexadecimal string to the ECC_BigInt_t format.
 *
 * @details
 *  Convert a large integer from a hexadecimal string representation to a
 *  ECC_BigInt_t representation.
 *
 * @param[out] bigint      Pointer to the location where to store the result.
 * @param[in]  hex         The hex representation of the large integer to
 *                         convert.
 ******************************************************************************/
void ECC_HexToBigInt(ECC_BigInt_t bigint, const char* hex);

/***************************************************************************//**
 * @brief
 *  Convert a large integer from the ECC_BigInt_t to a hexadecimal string format.
 *
 * @details
 *  Convert a large integer from a ECC_BigInt_t representation to a
 *  hexadecimal string representation.
 *
 * @param[out] hex         Buffer where to store the hexadecimal result.
 * @param[in]  bigint      The ECC_BigInt_t representation of the large
 *                         integer to convert.
 ******************************************************************************/
void ECC_BigIntToHex(char* hex, ECC_BigInt_t bigint);

/***************************************************************************//**
 * @brief
 *  Convert a big integer from a byte array to the ECC_BigInt_t format.
 *
 * @details
 *  Convert a large integer from a byte array representation to a
 *  ECC_BigInt_t representation.
 *
 * @param[out] bigint      Pointer to the location where to store the result.
 * @param[in]  bytearray   The byte array representation of the large integer to
 *                         convert.
 ******************************************************************************/
void ECC_ByteArrayToBigInt(ECC_BigInt_t bigint, const uint8_t* bytearray);

/***************************************************************************//**
 * @brief
 *  Convert a large integer from the ECC_BigInt_t to the byte array format.
 *
 * @details
 *  Convert a large integer from a ECC_BigInt_t representation to a
 *  byte array representation. Caution: byte array must be big enough
 *  to contain the result.
 *
 *  @param[out] bytearray   Buffer to store the resulting byte array.
 *  @param[in]  bigint      The ECC_BigInt_t representation of the large
 *                          integer to convert.
 ******************************************************************************/
void ECC_BigIntToByteArray(uint8_t* bytearray, ECC_BigInt_t bigint);

/***************************************************************************//**
 * @brief
 *  Convert an integer from uint32_t to ECC_BigInt_t format.
 *
 * @details
 *  Convert a integer from an uint32_t representation to a
 *  ECC_BigInt_t representation.
 *
 * @param[out] bigint      Pointer to the location to store the result.
 * @param[in]  value       The value to convert.
 ******************************************************************************/
void ECC_UnsignedIntToBigInt(ECC_BigInt_t bigint, const uint32_t value);

/** @} (end addtogroup ECC) */
/** @} (end addtogroup ECDSA) */
/** @} (end addtogroup Security) */
/** @} (end addtogroup Components) */

#endif // ECC_H
