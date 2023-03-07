/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_types.h"
#include "cc_ec_mont_api.h"
#include "ec_mont_local.h"


/*!
@file
@brief The file contains Curve25519 domain parameters and get-function.
*/


/* EC Montgomery curve domain structure type:
   Elliptic curve: y^2 = x^3 + Ax^2 + x over prime fild GFp
typedef struct {

        // EC prime modulus P
        uint32_t ecModP[CC_ECMONT_EDW_MODULUS_MAX_SIZE_IN_BYTES];
        // modulus size in bits
        uint32 ecModSizeInBits;
        uint32_t ecModSizeInBits;
        // EC generator coordinates X, Y
        uint32_t ecGenX[CC_ECMONT_EDW_MODULUS_MAX_SIZE_IN_BYTES];
        uint32_t ecGenY[CC_ECMONT_EDW_MODULUS_MAX_SIZE_IN_BYTES];
        // EC generator order
        uint32_t ecOrdN[CC_ECMONT_EDW_MODULUS_MAX_SIZE_IN_BYTES];
        // EC generator order size in bits
        uint32_t ecOrdSizeInBits;
        uint32_t ecOrdSizeInWords;
        // EC generator order's cofactor
        uint32_t ecOrdCofactor;
        // EC equation parameter; (A+2)/4 - for Curve25519
        uint32_t ecParam[CC_ECMONT_EDW_MODULUS_MAX_SIZE_IN_BYTES];
        // Barrett tags for EC modulus and generator order
        uint32_t ecModBarrTag[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];
        uint32_t ecOrdBarrTag[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];
        // parameters for bits setting in scalar multiplication LS/MS words
        uint32_t scalarLsWordAndValue;
        uint32_t scalarMsWordAndValue;
        uint32_t scalarMsWordOrValue;
        // EC Domain ID - enum
        CCEcMontDomainId_t domainId;
        // EC Domain name
        int8_t  name[20];

} CCEcEdwDomain_t;
*/


/*!> EC Montgomery curve25519 domain parameters.                              *
*    The data is in little endian order of words: LS-Word is most left one    */
static const CCEcMontDomain_t  EcMontDomainCurve25519 = {
        /* Prime modulus P = (2^255 - 19) =                                   *
        *  0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFED */
        {0xffffffed,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0x7fffffff},
        /* modulus size in bits and words */
        255, 8,
        /* EC generator G coordinate: X = 0x9  */
        {0x00000009},
        /* EC generator G coordinate: Y=                           *
        *  0x20AE19A1B8A086B4E01EDD2C7748D14C923D4D7E6D7C61B229E9C5A27ECED3D9 */
    {0x7eced3d9,0x29e9c5a2,0x6d7c61b2,0x923d4d7e,0x7748d14c,0xe01edd2c,0xb8a086b4,0x20ae19a1},
        /* EC_MONT generator order with cofactor 8:  *
        *  0x1000000000000000000000000000000014DEF9DEA2F79CD65812631A5CF5D3ED */
        {0x5cf5d3ed,0x5812631a,0xa2f79cd6,0x14def9de,0x00000000,0x00000000,0x00000000,0x10000000},
        253, 8, /* EC_MONT generator order size in bits and IN words */
        8, /* EC order cofactor */
        {0x0001db42}, /* parameter (a+2)/4 = 0x1DB42 */

        /*---------------------------------------------------*/
        /*Barrett tags for EC modulus and order */
#ifdef CC_SUPPORT_PKA_64_16
        {0x00000000,0x00000000,0x00000080}, /*0x800000000000000000 - for modulus*/
        {0xFFFFFFFF,0xFFFFFFFF,0x0000003F}, /*0x3FFFFFFFFFFFFFFFFF - for EC order*/
#else  // CC_SUPPORT_PKA_128_32
        {0x00000000,0x00000000,0x00000000,0x00000000,0x00000080}, /*0x8000000000000000000000000000000000  - for modulus*/
        {0x000003FF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFAC8}, /*0x3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFAC8 - for EC order*/
#endif
        CC_EC_MONT_DOMAIN_CURVE_25519, /* EC Domain Identifier - enum */
        "Curve25519",  /* EC Domain name */
        /* scalar bit setting parameters */
        0xF8,  /* SCALAR_LSB_AND_VALUE (248)*/
        0x7F,  /* SCALAR_MSB_AND_VALUE (127)*/
        0x40  /* SCALAR_MSB_OR_VALUE (64)*/
};

/*!<
 @brief    the function returns the domain pointer if the domain is supported for the product,
       otherwise return NULL
 @return   return domain pointer or NULL

*/
const CCEcMontDomain_t *EcMontGetCurve25519Domain(void)
{
    return &EcMontDomainCurve25519;
}

