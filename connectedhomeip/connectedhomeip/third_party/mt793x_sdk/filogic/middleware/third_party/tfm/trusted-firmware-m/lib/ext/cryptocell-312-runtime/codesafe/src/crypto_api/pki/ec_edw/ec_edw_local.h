/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef EC_EDW_LOCAL_H_H
#define EC_EDW_LOCAL_H_H

#include "cc_pal_types.h"
#include "cc_pka_defs_hw.h"
#include "cc_ec_edw_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
@file
@brief This file contains the  APIs used for EC MONT (Montgomery Curve25519) algorithms.

@note  Algorithms of Montgomery and Edwards elliptic curves cryptography are developed by
       Daniel.J.Bernstein and described in SW library "NaCl" (Networking and
       Cryptographic Library).
*/


/******************************************************************************/
/**          EC Montgomery domain APIs:                                       */
/******************************************************************************/

/*!< EC Edwards curve domain structure type:
     Elliptic curve over prime fild GFp: x^2 + y^2 = 1 + d*x^2*y^2 */
typedef struct {
        /*----   Common parameters for Mont-Edw curves ----*/
        /*!< EC prime modulus P */
        uint32_t ecModP[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        /*!< modulus size in bits */
        uint32_t ecModSizeInBits;
        /*!< modulus size in words */
        uint32_t ecModSizeInWords;
        /*!< EC generator G aff. coordinate X */
        uint32_t ecGenX[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        /*!< EC generator G aff. coordinate Y */
        uint32_t ecGenY[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        /*!< EC generator order.  */
        uint32_t ecOrdN[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        /*!< EC generator order size in bits */
        uint32_t ecOrdSizeInBits;
        /*!< EC generator order size in words */
        uint32_t ecOrdSizeInWords;
        /*!< EC generator order's cofactor */
        uint32_t ecOrdCofactor;
        /*!< EC equation parameter D */
        uint32_t ecParamD[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];

        /*!< EC generator G proective coordinates:
             X=ecGenX, Y=ecGenY, Z = 1, T=X*Y = ecGenT */
        uint32_t ecGenT[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];

        /*!< Precalculated cordinates (s,d,p,mp) of EC points G,4G,8G,16G */
        /* used only in special scal.mult */
        uint32_t sg[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t dg[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t pg[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t mpg[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        /* used in both scal.mult */
        uint32_t sg2[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t dg2[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t pg2[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t mpg2[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t sg4[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t dg4[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t pg4[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t mpg4[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        /* used only in special scal.mult */
        uint32_t sg8[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t dg8[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t pg8[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t mpg8[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t sg16[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t dg16[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t pg16[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t mpg16[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        /*!< Modified cordinates (X,Y,Z=1,T) of precalculated EC points 2G, 4G, 32G */
        /* used only in common scal.mult */
        uint32_t xg2[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t yg2[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t tg2[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t xg4[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t yg4[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t tg4[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        /* used only in special scal.mult */
        uint32_t xg32[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t yg32[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        uint32_t tg32[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];

        /*!< EC auxiliary value  d2 = 2*d */
        uint32_t ecAuxValD2[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        /*!< EC auxiliary value  q58 = = (P - 5)/8 */
        uint32_t ecAuxValQ58[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        /*!< EC auxiliary value sqrt_1 = square_root(-1) */
        uint32_t ecAuxValSqrt_1[CC_EC_MONT_EDW_MODULUS_MAX_SIZE_IN_WORDS];
        /*!< Barrett tag for EC modulus */
        uint32_t ecModBarrTag[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];
        /*!< Barrett tag for EC generator order */
        uint32_t ecOrdBarrTag[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];
        /*!< masks for bits setting in scalar multiplication LS/MS words */
        uint32_t scalarLsWordAndValue;
        uint32_t scalarMsWordAndValue;
        uint32_t scalarMsWordOrValue;

} CCEcEdwDomain_t;


/******************************************************************************/
/******************************************************************************/

const CCEcEdwDomain_t *EcEdwGetDomain25519(void);  /*!< The function returns pointer to EC Edwards domain 25519. */


#ifdef __cplusplus
}
#endif

#endif



