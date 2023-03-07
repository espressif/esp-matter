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
 *   Data base of CC_ECPKI_DomainID_secp521r1: structure of type  CCEcpkiDomain_t    *
 *       All data is given in little endian order of words in arrays               *
 ***********************************************************************************/
static const CCEcpkiDomain_t ecpki_domain_secp521r1 = {
    /* Field modulus :                            *
    *  GF_Modulus =  P= 1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF */
    {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
     0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x000001FF},
    /* EC equation parameters a, b  */
    /* a = 1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC  - big end  from SEC2 */
    {0xFFFFFFFC,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
     0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x000001FF},
    /* b = Bec= 51953EB9618E1C9A1F929A21A0B68540EEA2DA725B99B315F3B8B489918EF109E156193951EC7E937B1652C0BD3BB1BF073573DF883D2C34F1EF451FD46B503F00  */
    {0x6B503F00,0xEF451FD4,0x3D2C34F1,0x3573DF88,0x3BB1BF07,0x1652C0BD,0xEC7E937B,0x56193951,
     0x8EF109E1,0xB8B48991,0x99B315F3,0xA2DA725B,0xB68540EE,0x929A21A0,0x8E1C9A1F,0x953EB961,0x00000051},

    /* Order of generator: 1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFA51868783BF2F966B7FCC0148F709A5D03BB5C9B8899C47AEBB6FB71E91386409 big end  from SEC2 */
    {0x91386409,0xBB6FB71E,0x899C47AE,0x3BB5C9B8,0xF709A5D0,0x7FCC0148,0xBF2F966B,0x51868783,
     0xFFFFFFFA,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x000001FF},

    /* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
    /* Gx= C6858E06B70404E9CD9E3ECB662395B4429C648139053FB521F828AF606B4D3DBAA14B5E77EFE75928FE1DC127A2FFA8DE3348B3C1856A429BF97E7E31C2E5BD66  */
    {0xC2E5BD66,0xF97E7E31,0x856A429B,0x3348B3C1,0xA2FFA8DE,0xFE1DC127,0xEFE75928,0xA14B5E77,
     0x6B4D3DBA,0xF828AF60,0x053FB521,0x9C648139,0x2395B442,0x9E3ECB66,0x0404E9CD,0x858E06B7,0x000000C6},
    /* Gy= 11839296A789A3BC0045C8A5FB42C7D1BD998F54449579B446817AFBD17273E662C97EE72995EF42640C550B9013FAD0761353C7086A272C24088BE94769FD16650  Y - big end  from SEC2 */
    {0x9FD16650,0x88BE9476,0xA272C240,0x353C7086,0x3FAD0761,0xC550B901,0x5EF42640,0x97EE7299,
     0x273E662C,0x17AFBD17,0x579B4468,0x98F54449,0x2C7D1BD9,0x5C8A5FB4,0x9A3BC004,0x39296A78,0x00000118},
    1, /* EC cofactor K */

    /* Barrett tags NP,RP */
    #ifdef CC_SUPPORT_PKA_128_32
    {0x00000000,0x00000000,0x00000000,0x00000000,0x00000080,
        0x00000000,0x00000000,0x00000000,0x00000000,0x00000080},
    #else  // CC_SUPPORT_PKA_64_16
    {0x00000000,0x00000000,0x00000080,0x00000000, 0x00000000,
        0x00000000,0x00000000,0x00000080,0x00000000,0x00000000},
    #endif

    521, /* Size of field modulus in bits */
    521, /* Size of order of generator in bits */
    5,   /* Size of each inserted Barret tag in words; 0 - if not inserted */

    CC_ECPKI_DomainID_secp521r1,    /* EC Domain identifier - enum */
    "SECG_PRIME_521R1" /*NIST_P521*/
};




/**
 @brief    the function returns the domain pointer id the domain is supported for the product;
        otherwise return NULL
 @return   return domain pointer or NULL

*/
const CCEcpkiDomain_t *CC_EcpkiGetSecp521r1DomainP(void)
{
    return &ecpki_domain_secp521r1;
}

