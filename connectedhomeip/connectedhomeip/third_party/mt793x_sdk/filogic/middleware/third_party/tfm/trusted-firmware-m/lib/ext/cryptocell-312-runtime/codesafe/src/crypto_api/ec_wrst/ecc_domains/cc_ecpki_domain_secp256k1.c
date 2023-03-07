/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "cc_pal_types.h"
#include "cc_ecpki_types.h"


/**************** The domain structure describing *************/
/**
// The structure containing EC domain parameters in little-endian form.
// Elliptic curve: Y^2 = X^3 + A*X + B over prime fild GFp

typedef  struct {

    // Field modulus:  GF_Modulus = P
    uint32_t    ecP [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
    // EC equation parameters a, b
    uint32_t    ecA [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
    uint32_t    ecB [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
    // Order of generator: EC_GenerOrder
    uint32_t    ecOrd [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 1];
    // Generator (EC base point) coordinates in projective form
    uint32_t    ecGx [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
    uint32_t    ecGy [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
    // EC cofactor EC_Cofactor_K
    uint32_t    ecH;
    // include the specific fields that are used by the low level
    uint32_t      barrTagBuff[CC_PKA_DOMAIN_BUFF_SIZE_IN_WORDS];
    // Size of fields in bits
    uint32_t    modSizeInBits;
    uint32_t    ordSizeInBits;
    // Size of each inserted Barret tag in words; 0 - if not inserted
    uint32_t    barrTagSizeInWords;
    CCEcpkiDomainID_t   DomainID;
    int8_t  name[20];

} CCEcpkiDomain_t;

*/


/***********************************************************************************
 *   Data base of CC_ECPKI_DomainID_secp256k1: structure of type  CCEcpkiDomain_t    *
 *       All data is given in little endian order of words in arrays               *
 ***********************************************************************************/
static const CCEcpkiDomain_t ecpki_domain_secp256k1 = {
    /* Field modulus :  GF_Modulus =  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F - big end*/
    {0xFFFFFC2F,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
    /* EC equation parameters a, b  */
    /* a = 0  - big end  from SEC2 */
    {0x00000000},
    /* b = 7 - big end  from SEC2 */
    {0x00000007},

    /* Order of generator: FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141 big end  from SEC2 */
    {0xD0364141,0xBFD25E8C,0xAF48A03B,0xBAAEDCE6,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},

    /* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
    /* 79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798   X - big end  from SEC2 */
    {0x16F81798,0x59F2815B,0x2DCE28D9,0x029BFCDB,0xCE870B07,0x55A06295,0xF9DCBBAC,0x79BE667E},
    /* 483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8  Y - big end  from SEC2 */
    {0xFB10D4B8,0x9C47D08F,0xA6855419,0xFD17B448,0x0E1108A8,0x5DA4FBFC,0x26A3C465,0x483ADA77},

    1, /* EC cofactor K */

    /* Barrett tags NP,RP */
    #ifdef CC_SUPPORT_PKA_128_32
    {0x00000000,0x00000000,0x00000000,0x00000000,0x00000080,
        0x000000A2,0x00000000,0x00000000,0x00000000,0x00000080},
    #else  // CC_SUPPORT_PKA_64_16
    {0x00000000,0x00000000,0x00000080,0x00000000, 0x00000000,
        0x00000000,0x00000000,0x00000080,0x00000000,0x00000000},
    #endif

    256, /* Size of field modulus in bits */
    256, /* Size of order of generator in bits */
    5,   /* Size of each inserted Barret tag in words; 0 - if not inserted */

    CC_ECPKI_DomainID_secp256k1,    /* EC Domain identifier - enum */
    "SECG_PRIME_256K1"
};




/**
 @brief    the function returns the domain pointer id the domain is supported for the product;
        otherwise return NULL
 @return   return domain pointer or NULL

*/
const CCEcpkiDomain_t *CC_EcpkiGetSecp256k1DomainP(void)
{
    return &ecpki_domain_secp256k1;
}

