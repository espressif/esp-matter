/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PKA_ECC_H_H
#define PKA_ECC_H_H


#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types.h"
#include "cc_ecpki_types.h"
#include "pka_hw_defs.h"
#include "pka.h"
#include "ec_wrst.h"


#define PKA_ECC_MAX_OPERATION_SIZE_BITS        640  /*for EC 521-bit*/

/* maximal size of extended register in "big PKA words" and in 32-bit words:  *
   the size defined according to RSA as more large, and used to define some   *
*  auxiliary buffers sizes                                */
#define PKA_ECC_MAX_REGISTER_SIZE_IN_PKA_WORDS ((PKA_ECC_MAX_OPERATION_SIZE_BITS+PKA_EXTRA_BITS+CC_PKA_WORD_SIZE_IN_BITS-1)/CC_PKA_WORD_SIZE_IN_BITS)
#define PKA_ECC_MAX_REGISTER_SIZE_WORDS  (PKA_ECC_MAX_REGISTER_SIZE_IN_PKA_WORDS*(CC_PKA_WORD_SIZE_IN_BITS/32))


/* affine ec-point (in PKA format) */
typedef struct{
  uint32_t x;
  uint32_t y;
} PkaRegAffPoint_t;

/* jacobian ec-point: X:x/z^2, Y:y/z^3, t:a*z^4 (in PKA format) */
typedef struct{
  uint32_t x;
  uint32_t y;
  uint32_t z;
} PkaRegJcbPoint_t;


/* modified jacobian ec-point: X:x/z^2, Y:y/z^3, t:a*z^4 (in PKA format) */
typedef struct{
  uint32_t x;
  uint32_t y;
  uint32_t z;
  uint32_t t;
} PkaRegMdfPoint_t;


/* EC double: modified-modified */
void PkaDoubleMdf2Mdf(
    const uint32_t x,  const uint32_t y,  const uint32_t z,  const uint32_t t,
    const uint32_t x1, const uint32_t y1, const uint32_t z1, const uint32_t t1);

/* EC double: modified-jacobi */
void PkaDoubleMdf2Jcb(
    const uint32_t x,  const uint32_t y,  const uint32_t z,
    const uint32_t x1, const uint32_t y1, const uint32_t z1, const uint32_t t1);

/* EC add: affine-jacobi-modified */
void PkaAddJcbAfn2Mdf(
    const uint32_t x,  const uint32_t y,  const uint32_t z, const uint32_t t,
    const uint32_t x1, const uint32_t y1, const uint32_t z1,
    const uint32_t x2, const uint32_t y2);

/* convert to affine */
void PkaJcb2Afn(
    CCEcpkiScaProtection_t fr,
    const uint32_t x, const uint32_t y, const uint32_t z);

/* EC add: affine-affine-affine */
void PkaAddAff(
    const uint32_t x,  const uint32_t y,
    const uint32_t x1, const uint32_t y1,
    const uint32_t x2, const uint32_t y2);

/* double EC scalar multiplication: R = a*p + b*q */
uint32_t PkaSum2ScalarMullt(
    const uint32_t xr, const uint32_t yr, const uint32_t a,
    const uint32_t xp, const uint32_t yp, const uint32_t b,
    const uint32_t xq, const uint32_t yq);


CCError_t PkaEcWrstScalarMult(const CCEcpkiDomain_t *pDomain,
                             const uint32_t       *scalar,
                             uint32_t             scalSizeInWords,
                             const uint32_t       *inPointX,
                             const uint32_t       *inPointY,
                             uint32_t             *outPointX,
                             uint32_t             *outPointY,
                             uint32_t             *tmpBuff);

CCError_t PkaEcdsaVerify(void);


#ifdef __cplusplus
}

#endif

#endif


