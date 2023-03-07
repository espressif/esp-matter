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
 *   Data base of CC_ECPKI_DomainID_secp224r1: structure of type  CCEcpkiDomain_t    *
 *       All data is given in little endian order of words in arrays               *
 ***********************************************************************************/
static const CCEcpkiDomain_t ecpki_domain_secp224r1 = {
    /* Field modulus :  GF_Modulus =  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000001  - big end*/
    {0x00000001,0x00000000,0x00000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
    /* EC equation parameters a, b  */
    /* a = -3 = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFE  - big end  from SEC2 */
    {0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
    /* b = B4050A850C04B3ABF54132565044B0B7D7BFD8BA270B39432355FFB4 - big end  from SEC2 */
    {0x2355FFB4,0x270B3943,0xD7BFD8BA,0x5044B0B7,0xF5413256,0x0C04B3AB,0xB4050A85},

    /* Order of generator: ord= FFFFFFFFFFFFFFFFFFFFFFFFFFFF16A2E0B8F03E13DD29455C5C2A3D big end  from SEC2 */
    {0x5C5C2A3D,0x13DD2945,0xE0B8F03E,0xFFFF16A2,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},

    /* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
    /* B70E0CBD6BB4BF7F321390B94A03C1D356C21122343280D6115C1D21   X - big end  from SEC2 */
    {0x115C1D21,0x343280D6,0x56C21122,0x4A03C1D3,0x321390B9,0x6BB4BF7F,0xB70E0CBD},
    /* BD376388B5F723FB4C22DFE6CD4375A05A07476444D5819985007E34  Y - big end  from SEC2 */
    {0x85007E34,0x44D58199,0x5A074764,0xCD4375A0,0x4C22DFE6,0xB5F723FB,0xBD376388},

    1, /* EC cofactor K */

    /* Barrett tags NP,RP */
    #ifdef CC_SUPPORT_PKA_128_32
    {0x0000007F,0x00000000,0x00000000,0x00000000,0x00000080,
        0x0074AE8F,0x00000000,0x00000000,0x00000000,0x00000080},
    #else  // CC_SUPPORT_PKA_64_16
    {0x00000000,0x00000000,0x00000080,0x00000000, 0x00000000,
        0x00000000,0x00000000,0x00000080,0x00000000,0x00000000},
    #endif

    224, /* Size of field modulus in bits */
    224, /* Size of order of generator in bits */
    5,   /* Size of each inserted Barret tag in words; 0 - if not inserted */

    CC_ECPKI_DomainID_secp224r1,    /* EC Domain identifier - enum */
    "SECG_PRIME_224R1" /*NIST_P224*/
};




/**
 @brief    the function returns the domain pointer id the domain is supported for the product;
        otherwise return NULL
 @return   return domain pointer or NULL

*/
const CCEcpkiDomain_t *CC_EcpkiGetSecp224r1DomainP(void)
{
    return &ecpki_domain_secp224r1;
}

