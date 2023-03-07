/***************************************************************************//**
 * @file
 * @brief Elliptic Curve Cryptography (ECC) accelerator peripheral API
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

#include "em_device.h"
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "em_crypto.h"
#include "em_assert.h"
#include "em_core.h"
#include "ecc.h"

#include "debug/btl_debug.h"

/***************************************************************************//**
 * @addtogroup CRYPTOLIB
 * @{
 * @addtogroup ECC
 * @brief Elliptic Curve Cryptography API Library
 * @details
 *   This API is intended for use on Silicon Laboratories
 *
 *   TBW
 *
 *   References:
 *   @li Wikipedia - Elliptic curve cryptography,
 *      en.wikipedia.org/wiki/Elliptic_curve_cryptography
 *
 *   @li NIST FIPS 186-4, Digital Signature Standard (DSS), July 2013
 *      nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.186-4.pdf
 * @{
 ******************************************************************************/

/*******************************************************************************
 ******************************   TYPEDEFS   ***********************************
 ******************************************************************************/

typedef struct {
  ECC_BigInt_t  X;  /* x coordinate of point. */
  ECC_BigInt_t  Y;  /* y coordinate of point. */
  ECC_BigInt_t  Z;  /* z coordinate of point. */
} ECC_Projective_Point_t;

/** Structure definintion of NIST GF(p) and GF(2m) curves. */
typedef struct {
  /** The field size in octets. */
  int                size;

  /** The field size in bits. */
  int                bitSize;

  /** Prime (p) modulus identifier defined by the crypto hw.*/
  CRYPTO_ModulusId_TypeDef primeModulusId;

  /** Order (n) modulus identifier defined by the crypto hw.*/
  CRYPTO_ModulusId_TypeDef orderModulusId;

  /** The prime (p) of the curve */
  ECC_BigInt_t prime;

  /** The order (n) of the curve */
  ECC_BigInt_t order;

  /** The base point on the curve */
  ECC_Point_t  G;
} ECC_Curve_Params_t;

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define BIGINT_BYTES_PER_WORD  (sizeof(uint32_t))

/* Evaluates to one if bit number bitno of bn is set to one. */
#define BIGINT_BIT_IS_ONE(bn, bitno) ((bn)[(bitno) / 32] & (1 << ((bitno) % 32)))

#define EC_BIGINT_COPY(X, Y)         memcpy((X), (Y), sizeof(ECC_BigInt_t));

#define ECC_CLEAR_CRYPTO_CTRL crypto->CTRL = 0; \
  crypto->SEQCTRL = 0;                          \
  crypto->SEQCTRLB = 0

// Since we are not doing ECC encryption, but just authentication, there is
//   no danger to leak our private key through side channel attacks.
#undef USE_DUMMY_ADD

#if 1
#define ISSUE_NOP_AFTER_EXEC
#endif

/** @endcond */

/*******************************************************************************
 **************************     STATIC DATA      *******************************
 ******************************************************************************/

static const ECC_Curve_Params_t ECC_Curve_Params =
{  // "secp256r1",
   /* field size in octets */
  32,

  /* field size in bits */
  256,

  /* CRYPTO hw identifier for the Prime modulus (p) */
  cryptoModulusEccP256,

  /* CRYPTO hw identifier for the Order modulus (n) */
  cryptoModulusEccP256Order,

  /* Prime (p) */
  { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
    0x00000000, 0x00000000, 0x00000001, 0xFFFFFFFF },

  /* Order (n) */
  { 0xFC632551, 0xF3B9CAC2, 0xA7179E84, 0xBCE6FAAD,
    0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF },

  /* base point */
  {
    /* x coordinate of base point */
    { 0xD898C296, 0xF4A13945, 0x2DEB33A0, 0x77037D81,
      0x63A440F2, 0xF8BCE6E5, 0xE12C4247, 0x6B17D1F2 },

    /* y coordinate of base point */
    { 0x37BF51F5, 0xCBB64068, 0x6B315ECE, 0x2BCE3357,
      0x7C0F9E16, 0x8EE7EB4A, 0xFE1A7F9B, 0x4FE342E2 }
  }
};

/*******************************************************************************
 ***********************   FORWARD DECLARATIONS    *****************************
 ******************************************************************************/

static void ECC_ProjectiveToAffine(CRYPTO_TypeDef            *crypto,
                                   ECC_Projective_Point_t    *P,
                                   ECC_Point_t               *R);

/*******************************************************************************
 **************************   STATIC FUNCTIONS   *******************************
 ******************************************************************************/

/* Get the prime modulus type associated with the given ecc curve. */
static CRYPTO_ModulusId_TypeDef eccPrimeModIdGet(void)
{
  return ECC_Curve_Params.primeModulusId;
}

/* Get the order modulus type associated with the P256 curve. */
static CRYPTO_ModulusId_TypeDef eccOrderModIdGet(void)
{
  return ECC_Curve_Params.orderModulusId;
}

/* Get the 'modulus' associated with the P256 curve. */
static void eccPrimeGet(ECC_BigInt_t p)
{
  EC_BIGINT_COPY(p, ECC_Curve_Params.prime);
}

/* Get the 'order' associated with the P256 curve. */
static void eccOrderGet(ECC_BigInt_t   order)
{
  EC_BIGINT_COPY(order, ECC_Curve_Params.order);
}

/* Returns the base point for the P256 curve. */
static const ECC_Point_t* eccBasePointGet(void)
{
  return &ECC_Curve_Params.G;
}

/* Returns true if bigint is non-zero. */
static bool bigIntNonZero(ECC_BigInt_t bn)
{
  BTL_ASSERT(bn != NULL);

  uint32_t *pbn = bn;
  int       size = sizeof(ECC_BigInt_t) / sizeof(uint32_t);
  for (; size && (0 == *pbn); size--, pbn++) {
    // Do nothing
  }
  return size ? true : false;
}

/* Returns true if a is larger than or equal b.
 * Size-optimized implementation using CRYPTO HW
 */
static bool CRYPTO_bigIntLargerThanOrEqual(CRYPTO_TypeDef *crypto,
                                           ECC_BigInt_t   a,
                                           ECC_BigInt_t   b)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA2, a);
  CRYPTO_DDataWrite(&crypto->DDATA3, b);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_4(crypto,
                   CRYPTO_CMD_INSTR_CLR,
                   CRYPTO_CMD_INSTR_CCLR,
                   CRYPTO_CMD_INSTR_SELDDATA2DDATA3,
                   CRYPTO_CMD_INSTR_SUB /* DDATA0 = a - b, carry set if a < b */
                   );
  return (crypto->DSTATUS & CRYPTO_DSTATUS_CARRY) != CRYPTO_DSTATUS_CARRY;
}

/***************************************************************************//**
 * @brief
 *   Add two ECC points in GF(p) in projective coordinates
 *
 * @details
 *  This function implements mixed point addition in GF(p) of a point @ref P1
 *  in projective coordinates and a second point @ref P2 in affine coordinates.
 *  The result is stored in @ref R.
 *  See
 *  en.wikibooks.org/wiki/Cryptography/Prime_Curve/
 *    Jacobian_Coordinates#Point_Addition_.2812M_.2B_4S.29
 *
 *  @note With this implementation, it's possible to re-use the same memory for
 *        P1 and R.
 *
 * @param[in]  P1
 *   The point in projective coordinates
 *
 * @param[in]  P2
 *   The point in affine coordinates
 *
 * @param[out] R
 *   The destination of the result
 ******************************************************************************/
static void ECC_AddPrimeMixedProjectiveAffine(CRYPTO_TypeDef         *crypto,
                                              ECC_Projective_Point_t *P1,
                                              const ECC_Point_t      *P2,
                                              ECC_Projective_Point_t *R)
{
  ECC_BigInt_t D;
  CORE_DECLARE_IRQ_STATE;

  /*
   *
   *    Goals: A = P2->X*P1->Z²
   *    B = P2->Y*P1->Z³
   *
   *    Write Operations:
   *
   *    R1 = P1->Z
   *    R3 = P2->X
   *    R4 = P2->Y
   *
   *    Instructions to be executed:
   *
   *    1. R2 = R1 = P1->Z
   *    2. Select R1, R2
   *    2. R0 = R1 * R2 = P1->Z²
   *    3. R1 = R0 = P1->Z²
   *    4. Select R1, R3
   *    5. R0 = R1 * R3 = P2->X * P1->Z²
   *    6. R3 = R0 = P2->X * P1->Z²
   *    7. Select R1, R2
   *    8. R0 = R1 * R2 = P1->Z³
   *    9. R1 = R0 = P1->Z³
   *    10.Select R1, R4
   *    11.R0 = R1 * R4 = P2->Y * P1->Z³
   *
   *    Read Operations:
   *
   *    B = R0 = P2->Y*P1->Z³
   *    A = R3 = P2->X*P1->Z²
   *
   *    STEP 1:
   */

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA1, P1->Z);
  CRYPTO_DDataWrite(&crypto->DDATA3, P2->X);
  CRYPTO_DDataWrite(&crypto->DDATA4, P2->Y);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_12(crypto,
                    CRYPTO_CMD_INSTR_DDATA1TODDATA2,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA2,
                    CRYPTO_CMD_INSTR_MMUL,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA3,
                    CRYPTO_CMD_INSTR_MMUL,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA3,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA2,
                    CRYPTO_CMD_INSTR_MMUL,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA4,
                    CRYPTO_CMD_INSTR_MMUL
                    );

  /*
   *
   *    Goals: C  = A - P1->X
   *    D  = B - P1->Y
   *    R->Z = P1->Z * C
   *
   *    Write Operations:
   *
   *    R0 = B         B is already in R0
   *    R1 = P1->X
   *    R2 = P1->Y
   *    R3 = A         A is already in R3
   *    R4 = P1->Z
   *
   *    Instructions to be executed:
   *
   *    1. Select R0, R2
   *    2. R0 = R0 - R2 = B - P1->Y = D
   *    3. R2 = R0 = D
   *    4. Select R3, R1
   *    5. R0 = R3 - R1 = A - P1->X = C
   *    6. R1 = R0 = C
   *    7. Select R1, R4
   *    8. R0 = R1 * R4 = P1->Z * C = R->Z
   *
   *    Read Operations:
   *
   *    R->Z = R0 = P1->Z * C
   *    C  = R1 = A - P1->X
   *    D  = R2 = B - P1->Y
   *
   *    STEP 2:
   */

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA1, P1->X);
  CRYPTO_DDataWrite(&crypto->DDATA2, P1->Y);
  CRYPTO_DDataWrite(&crypto->DDATA4, P1->Z);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_8(crypto,
                   CRYPTO_CMD_INSTR_SELDDATA0DDATA2,
                   CRYPTO_CMD_INSTR_MSUB,
                   CRYPTO_CMD_INSTR_DDATA0TODDATA2,
                   CRYPTO_CMD_INSTR_SELDDATA3DDATA1,
                   CRYPTO_CMD_INSTR_MSUB,
                   CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                   CRYPTO_CMD_INSTR_SELDDATA1DDATA4,
                   CRYPTO_CMD_INSTR_MMUL
                   );

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataRead(&crypto->DDATA0, R->Z);
  CRYPTO_DDataRead(&crypto->DDATA2, D);
  CORE_EXIT_CRITICAL();

  /*
   *
   *    Goals: X1C2  = P1->X * C²
   *    C3    = C³
   *    D2    = D²
   *
   *    Write Operations:
   *
   *    R1 = C         C is already in R1
   *    R2 = D         D is already in R2
   *    R3 = P1->X
   *
   *    R4 = C
   *
   *    Instructions to be executed:
   *
   *    1. Select R1, R4
   *    2. R0 = R1 * R4 = C²
   *    3. R1 = R0 = C²
   *    4. R0 = R1 * R4 = C³
   *    5. R4 = R0 = C³
   *    6. Select R1, R3
   *    7. R0 = R1 * R3 = P1->X * C^²
   *    8. R3 = R0 = P1->X * C²
   *    9. R1 = R2 = D
   *    10. Select R1, R1
   *    11. R0 = R1 * R1 = D²
   *
   *    Read Operations:
   *
   *    D2   = R0 = D²
   *    X1C2 = R3 = P1->X * C²
   *    C3   = R4 = C³
   *
   *    STEP 3:
   */

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA3, P1->X);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_12(crypto,
                    CRYPTO_CMD_INSTR_DDATA1TODDATA4,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA4,
                    CRYPTO_CMD_INSTR_MMUL,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                    CRYPTO_CMD_INSTR_MMUL,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA4,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA3,
                    CRYPTO_CMD_INSTR_MMUL,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA3,
                    CRYPTO_CMD_INSTR_DDATA2TODDATA1,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA2,
                    CRYPTO_CMD_INSTR_MMUL
                    );

  /*
   *
   *    Goals: R->X   = D2 - (C3 + 2 * X1C2) = D2 - C3 - X1C2- X1C2
   *    Y1C3 = P1->Y * C3
   *
   *    Write Operations:
   *
   *    R0 = D2        D2 is already in R0
   *    R1 = P1->Y
   *    R3 = X1C2      X1C2 is already in R3
   *    R4 = C3        C3 is already in R4
   *
   *    Instructions to be executed:
   *
   *    1. Select R0, R4
   *    2. R0 = R0 - R4 = D2 - C3
   *    3. Select R0, R3
   *    4. R0 = R0 - R3 = D2 - C3 - X1C2
   *    5. R0 = R0 - R3 = D2 - C3 - X1C2 - X1C2 = R->X
   *    6. R2 = R0 = R->X
   *    7. Select R1, R4
   *    8. R0 = R1 * R4 = P1->Y * C3 = Y1C3
   *
   *    Read Operations:
   *
   *    Y1C3 = R0 = P1->Y * C³
   *    R->X   = R2 = D2 - (C3 + 2 * X1C2)
   *
   *    STEP 4:
   */

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA1, P1->Y);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_8(crypto,
                   CRYPTO_CMD_INSTR_SELDDATA0DDATA4,
                   CRYPTO_CMD_INSTR_MSUB,
                   CRYPTO_CMD_INSTR_SELDDATA0DDATA3,
                   CRYPTO_CMD_INSTR_MSUB,
                   CRYPTO_CMD_INSTR_MSUB,
                   CRYPTO_CMD_INSTR_DDATA0TODDATA2,
                   CRYPTO_CMD_INSTR_SELDDATA1DDATA4,
                   CRYPTO_CMD_INSTR_MMUL
                   );

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataRead(&crypto->DDATA2, R->X);
  CORE_EXIT_CRITICAL();

  /*
   *
   *    Goal: R->Y = D * (X1C2 - R->X) - Y1C3
   *
   *    Write Operations:
   *
   *    R1 = D
   *    R2 = R->X        R->X is already in R2
   *    R3 = X1C2      X1C2 is already in R3
   *    R4 = Y1C3
   *
   *    Instructions to be executed:
   *
   *    1. Select R3, R2
   *    2. R0 = R3 - R2 = X1C2 - R->X
   *    3. R2 = R0 = X1C2 - R->X
   *    4. Select R1, R2
   *    5. R0 = R1 * R2 = D *(X1C2 - R->X)
   *    6. Select R0, R4
   *    7. R0 = R0 - R4
   *
   *    Read Operations:
   *
   *    R->Y= R0 = D * (X1C2 - R->X) - Y1C3
   *
   *    STEP 5:
   */

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA1, D);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_8(crypto,
                   CRYPTO_CMD_INSTR_DDATA0TODDATA4,
                   CRYPTO_CMD_INSTR_SELDDATA3DDATA2,
                   CRYPTO_CMD_INSTR_MSUB,
                   CRYPTO_CMD_INSTR_DDATA0TODDATA2,
                   CRYPTO_CMD_INSTR_SELDDATA1DDATA2,
                   CRYPTO_CMD_INSTR_MMUL,
                   CRYPTO_CMD_INSTR_SELDDATA0DDATA4,
                   CRYPTO_CMD_INSTR_MSUB
                   );

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataRead(&crypto->DDATA0, R->Y);
  CORE_EXIT_CRITICAL();
} /* point_add_prime_projective */

/***************************************************************************//**
 * @brief
 *   Double an ECC point in GF(p) in projective coordinates.
 *
 * @details
 *  This function implements point doubling in GF(p) in projective coordinates
 *  on the P256 curve.
 *  The point @ref P1 is doubled and the result is stored in @ref R.
 *
 *  @param[in]  P1       The point to double
 *  @param[out] R        The destination of the result
 ******************************************************************************/
static void ECC_PointDoublePrimeProjective(CRYPTO_TypeDef               *crypto,
                                           const ECC_Projective_Point_t *P1,
                                           ECC_Projective_Point_t       *R)
{
  ECC_BigInt_t A;
  ECC_BigInt_t B;
  ECC_BigInt_t _2A;  /* Represents 2A */
  CORE_DECLARE_IRQ_STATE;

  /*
   *
   *    Goals: B    = 8 * Y1^4
   *    Y1Y1 = Y1²
   *
   *    Write Operations:
   *
   *    R1 = Y1
   *
   *    Instructions to be executed:
   *
   *    1. R2 = R1 = Y1
   *    2. Select R1, R2
   *    3. R0 = R1 * R2 = Y1² = Y1Y1
   *    4. R1 = R0 = Y1²
   *    5. R2 = R0 = Y1²
   *    6. R0 = R1 * R2 = Y1^4
   *    7. Select R0, R0
   *    8. R0 = R0 + R0 = 2 * Y1^4
   *    9. R0 = R0 + R0 = 4 * Y1^4
   *    10 R0 = R0 + R0 = 8 * Y1^4
   *
   *    Read Operations:
   *
   *    B    = R0 = 8 * Y1^4
   *    Y1Y1 = R1 = Y1²
   *
   */

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA1, P1->Y);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_10(crypto,
                    CRYPTO_CMD_INSTR_DDATA1TODDATA2,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA2,
                    CRYPTO_CMD_INSTR_MMUL,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA2,
                    CRYPTO_CMD_INSTR_MMUL,
                    CRYPTO_CMD_INSTR_SELDDATA0DDATA0,
                    CRYPTO_CMD_INSTR_MADD,
                    CRYPTO_CMD_INSTR_MADD,
                    CRYPTO_CMD_INSTR_MADD
                    );

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataRead(&crypto->DDATA0, B);
  CORE_EXIT_CRITICAL();

  /* Goals:
   *    A = 4P1->X * Y1Y1 (According to www.dkrypt.com/home/ecc it must
   *    be 4P1->X+Y1Y1 which is not right.For details see Chapter 3
   *    (Section 3.2.3) of the book "Introduction to Identity-Based Encryption"
   *    by Martin Luther)
   *    _2A  = 2A
   *
   *    Write Operations:
   *
   *    R0 = P1->X
   *    R1 = Y1Y1       R1 already contains Y1Y1
   *
   *    Instructions to be executed:
   *
   *    1.  Select R0, R0
   *    2.  R0 = R0 + R0 = 2P1->X
   *    3.  R0 = R0 + R0 = 4P1->X
   *    4.  R3 = R0 = 4P1->X
   *    5.  Select R1, R3
   *    6.  R0 = R1 * R3 = 4P1->X * Y1Y1 = A
   *    7.  R3 = R0
   *    8.  Select R0, R3
   *    9.  R0 = R0 + R3 = 2A = _2A
   *
   *    Read Operations:
   *
   *    A    = R3 = 4P1->X + Y1Y1
   *    _2A  = R0 = 2A
   *
   */

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA0, P1->X);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_9(crypto,
                   CRYPTO_CMD_INSTR_SELDDATA0DDATA0,
                   CRYPTO_CMD_INSTR_MADD,
                   CRYPTO_CMD_INSTR_MADD,
                   CRYPTO_CMD_INSTR_DDATA0TODDATA3,
                   CRYPTO_CMD_INSTR_SELDDATA1DDATA3,
                   CRYPTO_CMD_INSTR_MMUL,
                   CRYPTO_CMD_INSTR_DDATA0TODDATA3,
                   CRYPTO_CMD_INSTR_SELDDATA0DDATA3,
                   CRYPTO_CMD_INSTR_MADD
                   );

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataRead(&crypto->DDATA3, A);
  CRYPTO_DDataRead(&crypto->DDATA0, _2A);
  CORE_EXIT_CRITICAL();

  /*
   *
   *    Goals: Z1Z1 = P1->Z²
   *
   *    Write Operations:
   *
   *    R1 = P1->Z
   *
   *    Instructions to be executed:
   *
   *    1. R2 = R1 = P1->Z
   *    2. Select R1, R2
   *    3. R0 = R1 * R2 = P1->Z^² = Z1Z1
   *    4. R3 = R0 = Z1Z1
   *
   *    Read Operations:
   *
   *    Z1Z1 = R0 = P1->Z²
   *
   */

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA1, P1->Z);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_4(crypto,
                   CRYPTO_CMD_INSTR_DDATA1TODDATA2,
                   CRYPTO_CMD_INSTR_SELDDATA1DDATA2,
                   CRYPTO_CMD_INSTR_MMUL,
                   CRYPTO_CMD_INSTR_DDATA0TODDATA3
                   );

  /*
   *
   *    Goal: C = 3(P1->X - Z1Z1)(P1->X + Z1Z1)
   *
   *    Write Operations:
   *
   *    R2 = P1->X
   *    R3 = Z1Z1    Z1Z1 is already in R3
   *
   *    Instructions to be executed:
   *
   *    1.  Select R2, R3
   *    2.  R0 = R2 + R3 = P1->X + Z1Z1
   *    3.  R1 = R0 = P1->X + Z1Z1
   *    4.  R0 = R2 - R3 = P1->X - Z1Z1
   *    5.  R2 = R0 = P1->X - Z1Z1
   *    6.  Select R1, R2
   *    7.  R0 = R1 * R2 = (P1->X + Z1Z1)(P1->X - Z1Z1)
   *    8.  R1 = R0 = (P1->X + Z1Z1)(P1->X - Z1Z1)
   *    9.  Select R0, R1
   *    10. R0 = R0 + R1 = 2(P1->X + Z1Z1)(P1->X - Z1Z1)
   *    11. R0 = R0 + R1 = 3(P1->X + Z1Z1)(P1->X - Z1Z1) = C
   *    12. R1 = R0 = C
   *
   *    Read Operations:
   *
   *    C = R1 = 3(P1->X - Z1Z1)(P1->X + Z1Z1)
   *
   */

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA2, P1->X);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_12(crypto,
                    CRYPTO_CMD_INSTR_SELDDATA2DDATA3,
                    CRYPTO_CMD_INSTR_MADD,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                    CRYPTO_CMD_INSTR_MSUB,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA2,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA2,
                    CRYPTO_CMD_INSTR_MMUL,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                    CRYPTO_CMD_INSTR_SELDDATA0DDATA1,
                    CRYPTO_CMD_INSTR_MADD,
                    CRYPTO_CMD_INSTR_MADD,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA1
                    );

  /*
   *
   *    Goals: R->X = C² - _2A
   *    D = C(A - R->X)
   *
   *    Write Operations:
   *
   *    R1 = C          R1 already contains C
   *    R2 = _2A
   *    R3 = A
   *    R4 = C
   *
   *    Instructions to be executed:
   *
   *    1.  R4 = R1 = C
   *    2.  Select R1, R4
   *    3.  R0 = R1 * R4 = C²
   *    4.  Select R0, R2
   *    5.  R0 = R0 - R2 = C² - _2A = R->X
   *    6.  R4 = R0 = R->X
   *    7.  Select R3, R4
   *    8.  R0 = R3 - R4 = A - R->X
   *    9.  R2 = R0 = A - R->X
   *    10  Select R1, R2
   *    11. R0 = R1 * R2 = C(A - R->X) = D
   *
   *    Read Operations:
   *
   *    D  = R0 = C(A - R->X)
   *    R->X = R4 = C² - _2A
   *
   */

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA2, _2A);
  CRYPTO_DDataWrite(&crypto->DDATA3, A);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_11(crypto,
                    CRYPTO_CMD_INSTR_DDATA1TODDATA4,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA4,
                    CRYPTO_CMD_INSTR_MMUL,
                    CRYPTO_CMD_INSTR_SELDDATA0DDATA2,
                    CRYPTO_CMD_INSTR_MSUB,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA4,
                    CRYPTO_CMD_INSTR_SELDDATA3DDATA4,
                    CRYPTO_CMD_INSTR_MSUB,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA2,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA2,
                    CRYPTO_CMD_INSTR_MMUL
                    );

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataRead(&crypto->DDATA4, R->X);
  CORE_EXIT_CRITICAL();

  /*
   *
   *    Goals: R->Y = D - B
   *    R->Z = 2 * Y1 * P1->Z
   *
   *    Write Operations:
   *
   *    R0 = D         R0 already contains D
   *    R1 = Y1
   *    R2 = P1->Z
   *    R3 = B
   *
   *    Instructions to be executed:
   *
   *    1.  Select R0, R3
   *    2.  R0 = R0 - R3 = D - B = R->Y
   *    3.  R3 = R0 = R->Y
   *    4.  Select R1, R2
   *    5.  R0 = R1 * R2 = Y1 * P1->Z
   *    6.  Select R0, R0
   *    7.  R0 = R0 + R0 = 2 * Y1 * P1->Z = R->Z
   *
   *    Read Operations:
   *
   *    R->Z = R0 = 2*Y1*P1->Z
   *    R->Y = R3 = D - B
   *
   */

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA1, P1->Y);
  CRYPTO_DDataWrite(&crypto->DDATA2, P1->Z);
  CRYPTO_DDataWrite(&crypto->DDATA3, B);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_7(crypto,
                   CRYPTO_CMD_INSTR_SELDDATA0DDATA3,
                   CRYPTO_CMD_INSTR_MSUB,
                   CRYPTO_CMD_INSTR_DDATA0TODDATA3,
                   CRYPTO_CMD_INSTR_SELDDATA1DDATA2,
                   CRYPTO_CMD_INSTR_MMUL,
                   CRYPTO_CMD_INSTR_SELDDATA0DDATA0,
                   CRYPTO_CMD_INSTR_MADD
                   );

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataRead(&crypto->DDATA0, R->Z);
  CRYPTO_DDataRead(&crypto->DDATA3, R->Y);
  CORE_EXIT_CRITICAL();
} /* point_dbl_prime_projective */

/***************************************************************************//**
 * @brief
 *   Modular inverse with prime modulus
 *
 * @details
 *   This function computes R = 1/X mod(N).
 *
 *  @param[in]  X        Dividend of modular division operation
 *  @param[in]  N        Modulus
 *  @param[out] R        The destination of the result
 *
 * @return N/A
 ******************************************************************************/
static void ECC_ModularInversePrime(CRYPTO_TypeDef *crypto,
                                    ECC_BigInt_t   X,
                                    ECC_BigInt_t   N,
                                    ECC_BigInt_t   R)
{
  CORE_DECLARE_IRQ_STATE;
  // Prerequisite: crypto modulus is set to order of prime curve we're using

  /* This is based on Fermat's little theorem, see
   * comeoncodeon.wordpress.com/2011/10/09/modular-multiplicative-inverse
   *
   * R = (X ^ (-1)) mod N
   * R = (X ^ (N - 2)) mod N
   */
  memset(R, 0, sizeof(ECC_BigInt_t));
  R[0] = 1;

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA0, N);
  CRYPTO_DDataWrite(&crypto->DDATA1, X);
  CRYPTO_DDataWrite(&crypto->DDATA2, R);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_3(crypto,
                   CRYPTO_CMD_INSTR_DEC,            /* N -= 1 */
                   CRYPTO_CMD_INSTR_DEC,            /* N -= 1 */
                   CRYPTO_CMD_INSTR_DDATA0TODDATA4  /* DDATA4 = N*/
                   );

  while ((crypto->DSTATUS & CRYPTO_DSTATUS_CARRY) != CRYPTO_DSTATUS_CARRY) {
    CRYPTO_EXECUTE_14(crypto,

                      /* N >> 1, carry = LSB(N) */
                      CRYPTO_CMD_INSTR_SELDDATA0DDATA2,
                      CRYPTO_CMD_INSTR_SHR,
                      CRYPTO_CMD_INSTR_DDATA0TODDATA4,

                      /* if LSB(N), R = X * R mod N*/
                      CRYPTO_CMD_INSTR_EXECIFCARRY,
                      CRYPTO_CMD_INSTR_MMUL,
                      CRYPTO_CMD_INSTR_DDATA0TODDATA2,

                      /* X = X*X mod N */
                      CRYPTO_CMD_INSTR_EXECALWAYS,
                      CRYPTO_CMD_INSTR_SELDDATA0DDATA3,
                      CRYPTO_CMD_INSTR_DDATA1TODDATA3,
                      CRYPTO_CMD_INSTR_MMUL,
                      CRYPTO_CMD_INSTR_DDATA0TODDATA1,

                      /* Check for N == 0 */
                      CRYPTO_CMD_INSTR_DDATA4TODDATA0,
                      CRYPTO_CMD_INSTR_DEC,
                      CRYPTO_CMD_INSTR_INC
                      );
  }

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataRead(&crypto->DDATA2, R); /* R = W */
  CORE_EXIT_CRITICAL();
}

/* Returns true if bigint is equal to the given 32bit integer. */
__STATIC_INLINE bool bigIntEqual32bitVal(uint32_t *bn, int size, uint32_t val)
{
  if (*(uint32_t*)bn != val) {
    return 0;
  }

  for (bn++, size -= 4; size > 0 && (0 == *bn); size -= 4, bn++) {
    // Do nothing
  }
  return (size > 0 ? false : true);
}

/***************************************************************************//**
 * @brief
 *   Perform ECC point multiplication.
 *
 * @details
 *   Perform ECC point multiplication using the simple Double-And-Add algorithm:
 *     R := 0
 *     for i from m to 0 do
 *       R := 2R (using point doubling)
 *       if di = 1 then
 *         R := R + P (using point addition)
 *     Return R
 *
 * @param[in]  P
 *   The point to multiply. Has to be affine!
 *
 * @param[in]  n
 *   Multiplicand - scalar to multiply by
 *
 * @param[out] R
 *   The destination of n*P
 ******************************************************************************/
static void ECC_PointMul(CRYPTO_TypeDef            *crypto,
                         const ECC_Point_t         *P,
                         ECC_BigInt_t              n,
                         ECC_Projective_Point_t    *R
                         )
{
  int                  i;
  int                  mulStart;
  ECC_BigInt_t         temp;
  /////////////////////////// Initializations ////////////////////////////

  /* temp = 1 */
  memset(temp, 0, sizeof(temp));
  temp[0] = 1;

  /* R := 0 */
  memset(R->X, 0, sizeof(R->X));
  memset(R->Y, 0, sizeof(R->Y));
  memset(R->Z, 0, sizeof(R->Z));

  /* Set modulus of CRYPTO module corresponding to P256 curve. */
  CRYPTO_ModulusSet(crypto, eccPrimeModIdGet());
  ECC_CLEAR_CRYPTO_CTRL;

  /* Simulation speed optimization: Start at first 1 in multiplicand.
   *    SHOULD NOT BE DONE IN C library because it will make the implementation
   *    vulnerable to timing attacks. */
  // Note (stcoorem): This optimization here is fine since this library will
  // be used for signature verification only. This means we are only handling
  // public data and there are no secrets to leak.
  for (mulStart = ECC_Curve_Params.bitSize - 1;
       (mulStart > 0) && (!BIGINT_BIT_IS_ONE(n, mulStart));
       mulStart--) {
    // Do nothing
  }

  for (i = mulStart; i >= 0; i--) {
    ECC_PointDoublePrimeProjective(crypto, R, R);

    if (BIGINT_BIT_IS_ONE(n, i)) {
      if (!(bigIntEqual32bitVal(R->X, sizeof(R->X), 0)
            && bigIntEqual32bitVal(R->Y, sizeof(R->Y), 0)
            && bigIntEqual32bitVal(R->Z, sizeof(R->Z), 0))) {
        ECC_AddPrimeMixedProjectiveAffine(crypto, R, P, R);
      } else {
        EC_BIGINT_COPY(R->X, P->X);
        EC_BIGINT_COPY(R->Y, P->Y);
        EC_BIGINT_COPY(R->Z, temp);
      }
    }
  } /* for (int i=ECC_Curve_Params.bitSize-1; i>=0; i--) */
} /* ECC_PointMul */

/***************************************************************************//**
 * @brief
 *   Convert projective coordinates to affine coordinates.
 *   note: P and R can point to the same memory
 *
 * @details
 * TBW
 *
 *  @param[in]  P        The X/Y/Z components of the point with projective
 *                       coordinates to convert.
 *  @param[out] R        The destination of the result
 ******************************************************************************/
static void ECC_ProjectiveToAffine(CRYPTO_TypeDef            *crypto,
                                   ECC_Projective_Point_t    *P,
                                   ECC_Point_t               *R)

{
  ECC_BigInt_t    Z_inv;
  ECC_BigInt_t    modulus;
  CORE_DECLARE_IRQ_STATE;

  /* Set modulus of CRYPTO module corresponding to specified curve id. */
  CRYPTO_ModulusSet(crypto, eccPrimeModIdGet());
  ECC_CLEAR_CRYPTO_CTRL;

  eccPrimeGet(modulus);

  /* mod_div_projective(1, P->Z, modType, Z_inv); */
  /* Z_inv = 1 / P->Z mod N*/
  // For prime curves ONLY:
  ECC_ModularInversePrime(crypto, P->Z, modulus, Z_inv);

  // For generic implementation:
  // memset(temp, 0, sizeof(temp));
  // temp[0]=1;
  // ECC_ModularDivision(crypto, temp, P->Z, modulus, Z_inv);

  /*
   *    For prime curve:
   *
   *    Goals:
   *    R->X = P->X * Z_inv ^2
   *    R->Y = P->Y * Z_inv ^3
   *
   *    Write Operations:
   *
   *    R1 = Z_inv
   *    R3 = P->X
   *    R4 = P->Y
   *
   *    Instructions to be executed:
   *
   *    1.  R2 = R1 = Z_inv
   *    2.  Select R1, R2
   *    3.  R0 = R1 * R2 = Z_inv^2
   *    4.  R1 = R0 = Z_inv^2
   *    5.  Select R1, R3
   *    6.  R0 = R1 * R3 = P->X * Z_inv^2 = R->X
   *    7.  R3 = R0
   *    8.  Select R1, R2
   *    9.  R0 = R1 * R2 = Z_inv^3
   *    10. R1 = R0 = Z_inv^3
   *    11. Select R1, R4
   *    12. R0 = R1 * R4 = P->Y * Z_inv^3 = R->Y
   *
   *    Read Operations:
   *
   *    R->Y = R0 = P->Y * P->Z_inv^3
   *    R->X = R3 = P->X * P->Z_inv^2
   *
   */

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA1, Z_inv);
  CRYPTO_DDataWrite(&crypto->DDATA3, P->X);
  CRYPTO_DDataWrite(&crypto->DDATA4, P->Y);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_12(crypto,
                    CRYPTO_CMD_INSTR_DDATA1TODDATA2,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA2,
                    CRYPTO_CMD_INSTR_MMUL,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA3,
                    CRYPTO_CMD_INSTR_MMUL,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA3,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA2,
                    CRYPTO_CMD_INSTR_MMUL,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                    CRYPTO_CMD_INSTR_SELDDATA1DDATA4,
                    CRYPTO_CMD_INSTR_MMUL
                    );

  CORE_ENTER_CRITICAL();
  CRYPTO_DDataRead(&crypto->DDATA0, R->Y);
  CRYPTO_DDataRead(&crypto->DDATA3, R->X);
  CORE_EXIT_CRITICAL();
} /* ECC_ProjectiveoAffine */

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *  Convert big integer from hex string to ECC_BigInt_t format.
 *
 * @details
 *  Convert a large integer from a hexadecimal string representation to a
 *  ECC_BigInt_t representation.
 *
 * @param[out] bigint      Pointer to the location where to store the result.
 * @param[in]  hex         The hex represenation of the large integer to
 *                         convert.
 ******************************************************************************/
void ECC_HexToBigInt(ECC_BigInt_t bigint, const char* hex)
{
  uint32_t* ret;
  uint32_t l = 0;
  int m, i, j, k, c;

  BTL_ASSERT(hex);
  BTL_ASSERT(*hex);

  /* Count number of hex digits. */
  for (i = 0; isxdigit((unsigned char) hex[i]); i++) {
  }

  /* Check number of hex digits is not bigger than can fit in a bigint. */
  BTL_ASSERT(i <= 2 * (int)sizeof(ECC_BigInt_t));

  memset(bigint, 0, sizeof(ECC_BigInt_t));

  ret = bigint;

  /* i is the number of hex digits; */

  j = i; /* least significant 'hex' */
  m = 0;
  while (j > 0) {
    *ret = 0;

    m =
      (((int)BIGINT_BYTES_PER_WORD * 2)
       <= j) ? ((int)BIGINT_BYTES_PER_WORD * 2) : j;
    l = 0;
    for (;; ) {
      c = hex[j - m];
      if ((c >= '0') && (c <= '9')) {
        k = c - '0';
      } else if ((c >= 'a') && (c <= 'f')) {
        k = c - 'a' + 10;
      } else if ((c >= 'A') && (c <= 'F')) {
        k = c - 'A' + 10;
      } else {
        k = 0;  /* paranoia */
      }
      l = (l << 4) | k;

      if (--m <= 0) {
        *ret = l;
        break;
      }
    }
    j -= (BIGINT_BYTES_PER_WORD * 2);
    ret++;
  }
}

/***************************************************************************//**
 * @brief
 *  Convert large integer from ECC_BigInt_t to hex string format.
 *
 * @details
 *  Convert a large integer from a ECC_BigInt_t representation to a
 *  hexadecimal string representation.
 *
 *  @param[out] hex         Buffer where to store the hexadecimal result.
 *  @param[in]  bigint      The ECC_BigInt_t represenation of the large
 *                          integer to convert.
 ******************************************************************************/
void ECC_BigIntToHex(char* hex, ECC_BigInt_t bigint)
{
  int i, j;
  uint8_t nibble;

  BTL_ASSERT(hex);
  BTL_ASSERT(bigint);

  for (i = (sizeof(ECC_BigInt_t) / sizeof(uint32_t)) - 1; i >= 0; i--) {
    for (j = (BIGINT_BYTES_PER_WORD * 2) - 1; j >= 0; j--) {
      nibble = (bigint[i] >> (j * 4)) & 0xf;
      *hex++ = nibble > 9 ? nibble - 10 + 'A' : nibble + '0';
    }
  }

  /* Null terminate at end of string. */
  *hex = 0;
}

/***************************************************************************//**
 * @brief
 *  Convert big integer from byte array to ECC_BigInt_t format.
 *
 * @details
 *  Convert a large integer from a byte array representation to a
 *  ECC_BigInt_t representation.
 *
 * @param[out] bigint      Pointer to the location where to store the result.
 * @param[in]  bytearray   The byte array represenation of the large integer to
 *                         convert.
 ******************************************************************************/
void ECC_ByteArrayToBigInt(ECC_BigInt_t bigint, const uint8_t* bytearray)
{
  uint8_t* bigint_byte;
  int i;

  BTL_ASSERT(bytearray);
  BTL_ASSERT(bigint);

  memset(bigint, 0, sizeof(ECC_BigInt_t));

  bigint_byte = (uint8_t*)bigint;

  for (i = (int)sizeof(ECC_BigInt_t) - 1; i >= 0; i--) {
    bigint_byte[sizeof(ECC_BigInt_t) - i - 1] = bytearray[i];
  }
}

/***************************************************************************//**
 * @brief
 *  Convert large integer from ECC_BigInt_t to byte array format.
 *
 * @details
 *  Convert a large integer from a ECC_BigInt_t representation to a
 *  byte array representation. Caution: byte array must be big enough
 *  to contain the result!
 *
 *  @param[out] bytearray   Buffer where to store the resulting byte array.
 *  @param[in]  bigint      The ECC_BigInt_t represenation of the large
 *                          integer to convert.
 ******************************************************************************/
void ECC_BigIntToByteArray(uint8_t* bytearray, ECC_BigInt_t bigint)
{
  uint8_t* bigint_byte;
  int i;

  BTL_ASSERT(bytearray);
  BTL_ASSERT(bigint);

  memset(bytearray, 0, sizeof(ECC_BigInt_t));

  bigint_byte = (uint8_t*)bigint;

  for (i = (int)sizeof(ECC_BigInt_t) - 1; i >= 0; i--) {
    bytearray[sizeof(ECC_BigInt_t) - i - 1] = bigint_byte[i];
  }
}

/***************************************************************************//**
 * @brief
 *  Convert integer from uint32_t to ECC_BigInt_t format.
 *
 * @details
 *  Convert a integer from an uint32_t representation to a
 *  ECC_BigInt_t representation.
 *
 * @param[out] bigint      Pointer to the location where to store the result.
 * @param[in]  value       The value to convert.
 ******************************************************************************/
void ECC_UnsignedIntToBigInt(ECC_BigInt_t bigint, const uint32_t value)
{
  BTL_ASSERT(bigint);

  bigint[0] = value;
}

/***************************************************************************//**
 * @brief
 *   Verify an ECDSA signature.
 *
 * @details
 *   TBW
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
                                      ECC_EcdsaSignature_t   *signature)
{
  ECC_BigInt_t            w;
  CORE_DECLARE_IRQ_STATE;

  /* P1.Z is in use as 'temp' */
  ECC_Projective_Point_t  P1 = { 0 };

  /* P2.Z is in use as 'z' (digest), P2.Y is in use as 'n' (curve order) */
  ECC_Projective_Point_t  P2 = { 0 };

  if ((msgDigest == NULL) || (publicKey == NULL) || (signature == NULL)) {
    return BOOTLOADER_ERROR_SECURITY_INVALID_PARAM;
  }

  if (msgDigestLen <= 0) {
    return BOOTLOADER_ERROR_SECURITY_INVALID_PARAM;
  }

  /* Crypto module setup. */
  ECC_CLEAR_CRYPTO_CTRL;
  crypto->WAC = (crypto->WAC & (~_CRYPTO_WAC_MODOP_MASK))
                | CRYPTO_WAC_MODOP_REGULAR;
  CRYPTO_ModulusSet(crypto, eccOrderModIdGet());
  CRYPTO_ResultWidthSet(crypto, cryptoResult260Bits);
  CRYPTO_MulOperandWidthSet(crypto, cryptoMulOperandModulusBits);

  eccOrderGet(P2.Y);

  /* Step #1:
   *    Verify that the signature components 'r' and 's' are integers in the
   *    range [1,n-1].
   */
  if (CRYPTO_bigIntLargerThanOrEqual(crypto, signature->r, P2.Y) || !bigIntNonZero(signature->r)) {
    return BOOTLOADER_ERROR_SECURITY_PARAM_OUT_RANGE;
  }

  if (CRYPTO_bigIntLargerThanOrEqual(crypto, signature->s, P2.Y) || !bigIntNonZero(signature->s)) {
    return BOOTLOADER_ERROR_SECURITY_PARAM_OUT_RANGE;
  }

  /* Step #2:
   *    z is the 'size' or 'msgDigestLen' leftmost bits of the digest, whichever
   *    is smallest.
   */
  if (msgDigestLen > ECC_Curve_Params.size) {
    msgDigestLen = ECC_Curve_Params.size;
  }

  /* Optimization: We can use bytewise copy since all P-curves have size%8==0 */
  memset(P2.Z, 0, sizeof(ECC_BigInt_t));
  for (int i = 0; i < msgDigestLen; i++) {
    P2.Z[(msgDigestLen - i - 1) / sizeof(uint32_t)] |=
      msgDigest[i] << 8 * ((msgDigestLen - 1 - i) % sizeof(uint32_t));
  }
  // bin2BigInt(P2.Z, msgDigest, msgDigestLen);

  /* Cap digest at order, since MMUL doesn't support arguments >= modulus */
  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA2, P2.Z);
  CORE_EXIT_CRITICAL();
  CRYPTO_EXECUTE_3(crypto,
                   CRYPTO_CMD_INSTR_CLR,
                   CRYPTO_CMD_INSTR_SELDDATA0DDATA2,
                   CRYPTO_CMD_INSTR_MADD);
  CORE_ENTER_CRITICAL();
  CRYPTO_DDataRead(&crypto->DDATA0, P2.Z);
  CORE_EXIT_CRITICAL();

  /* Step #3:
   *    Calculate u1=z/s mod(n) and u2=r/s mod(n)
   */

  /* Step #3.1:
   *    w = 1 / s mod order(P256)
   */
  // For prime curve:
  ECC_ModularInversePrime(crypto, signature->s, P2.Y, w);

  // For generic implementation:
  // memset(P1.Z, 0, sizeof(P1.Z));
  // P1.Z[0]=1;
  // ECC_ModularDivision(crypto, P1.Z, signature->s, P2.Y, w);

  /* Step #3.2:
   *    u1 = z*w mod(n) and u2 = r*w mod(n)
   */
  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA1, w);
  CRYPTO_DDataWrite(&crypto->DDATA4, P2.Z);
  CRYPTO_DDataWrite(&crypto->DDATA3, signature->r);
  CORE_EXIT_CRITICAL();
  CRYPTO_EXECUTE_7(crypto,
                   CRYPTO_CMD_INSTR_CLR,
                   CRYPTO_CMD_INSTR_SELDDATA1DDATA4,
                   CRYPTO_CMD_INSTR_MMUL,    /* DDATA0 = w*z = u1 */
                   CRYPTO_CMD_INSTR_DDATA0TODDATA4,
                   CRYPTO_CMD_INSTR_CLR,
                   CRYPTO_CMD_INSTR_SELDDATA1DDATA3,
                   CRYPTO_CMD_INSTR_MMUL);   /* DDATA0 = w*r = u2 */
  CORE_ENTER_CRITICAL();
  CRYPTO_DDataRead(&crypto->DDATA4, P2.Z); // z = u1
  CRYPTO_DDataRead(&crypto->DDATA0, w); // w = u2
  CORE_EXIT_CRITICAL();

  /* Step #4:
   *    Calculate P = u1*G + u2*PublicKey
   */

  /* Multiply the base point.
   *    P1 = u1 * G
   */
  ECC_PointMul(crypto,
               eccBasePointGet(),
               P2.Z,
               &P1);

  /* Multiply the public key.
   *    P2 = u2 * publicKey
   */
  ECC_PointMul(crypto,
               publicKey,
               w,
               &P2);

  /* Want to add P1 and P2, but need Affine conversion to deal with
   * addition only taking one projective and one affine argument
   */

  /* P1 = Affine(P1); P2 = Add(P1, P2); P2 = Affine(P2); */
  ECC_ProjectiveToAffine(crypto, &P1, (ECC_Point_t*)&P1);
  ECC_AddPrimeMixedProjectiveAffine(crypto, &P2, (ECC_Point_t*)&P1, &P2);
  ECC_ProjectiveToAffine(crypto, &P2, (ECC_Point_t*)&P2);

  /* Step #4:
   *    The signature is valid if r==P.X mod (n)
   */
  CORE_ENTER_CRITICAL();
  CRYPTO_ModulusSet(crypto, eccOrderModIdGet());
  CRYPTO_DDataWrite(&crypto->DDATA2, P2.X);
  CRYPTO_DDataWrite(&crypto->DDATA3, signature->r);
  CORE_EXIT_CRITICAL();
  CRYPTO_EXECUTE_6(crypto,
                   CRYPTO_CMD_INSTR_CLR,
                   CRYPTO_CMD_INSTR_CCLR,
                   CRYPTO_CMD_INSTR_SELDDATA2DDATA3,
                   CRYPTO_CMD_INSTR_MSUB, /* DDATA0 = P.X - r mod(n) */
                   CRYPTO_CMD_INSTR_DEC,
                   CRYPTO_CMD_INSTR_INC /* if r == P.X, DDATA0 was 0 */
                   );

  if ((crypto->DSTATUS & CRYPTO_DSTATUS_CARRY) == CRYPTO_DSTATUS_CARRY) {
    return BOOTLOADER_OK;
  } else {
    return BOOTLOADER_ERROR_SECURITY_REJECTED; // Signature is invalid.
  }
}

/** @} (end addtogroup ECC) */
/** @} (end addtogroup CRYPTOLIB) */

#endif /* defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) */
