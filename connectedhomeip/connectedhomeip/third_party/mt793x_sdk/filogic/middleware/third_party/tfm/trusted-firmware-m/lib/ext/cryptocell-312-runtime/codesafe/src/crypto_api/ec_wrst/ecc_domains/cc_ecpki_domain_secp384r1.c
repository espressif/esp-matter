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
 *   Data base of CC_ECPKI_DomainID_secp384r1: structure of type  CCEcpkiDomain_t    *
 *       All data is given in little endian order of words in arrays               *
 ***********************************************************************************/
static const CCEcpkiDomain_t ecpki_domain_secp384r1 = {
    /* Field modulus :                            *
    *  GF_Modulus =  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFF - big end*/
    {0xFFFFFFFF,0x00000000,0x00000000,0xFFFFFFFF,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
    /* EC equation parameters a, b  */
    /* a = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFC   - big end  from SEC2 */
    {0xFFFFFFFC,0x00000000,0x00000000,0xFFFFFFFF,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
    /* b = B3312FA7E23EE7E4988E056BE3F82D19181D9C6EFE8141120314088F5013875AC656398D8A2ED19D2A85C8EDD3EC2AEF  - big end  from SEC2 */
    {0xD3EC2AEF,0x2A85C8ED,0x8A2ED19D,0xC656398D,0x5013875A,0x0314088F,0xFE814112,0x181D9C6E,
        0xE3F82D19,0x988E056B,0xE23EE7E4,0xB3312FA7},

    /* Order of generator: FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC7634D81F4372DDF581A0DB248B0A77AECEC196ACCC52973  big end  from SEC2 */
    {0xCCC52973,0xECEC196A,0x48B0A77A,0x581A0DB2,0xF4372DDF,0xC7634D81,0xFFFFFFFF,0xFFFFFFFF,
        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},

    /* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
    /* AA87CA22BE8B05378EB1C71EF320AD746E1D3B628BA79B9859F741E082542A385502F25DBF55296C3A545E3872760AB7    X - big end  from SEC2 */
    {0x72760AB7,0x3A545E38,0xBF55296C,0x5502F25D,0x82542A38,0x59F741E0,0x8BA79B98,0x6E1D3B62,
        0xF320AD74,0x8EB1C71E,0xBE8B0537,0xAA87CA22},
    /* 3617DE4A96262C6F5D9E98BF9292DC29F8F41DBD289A147CE9DA3113B5F0B8C00A60B1CE1D7E819D7A431D7C90EA0E5F   Y - big end  from SEC2 */
    {0x90EA0E5F,0x7A431D7C,0x1D7E819D,0x0A60B1CE,0xB5F0B8C0,0xE9DA3113,0x289A147C,0xF8F41DBD,
        0x9292DC29,0x5D9E98BF,0x96262C6F,0x3617DE4A},

    1, /* EC cofactor K */

    /* Barrett tags NP,RP */
    #ifdef CC_SUPPORT_PKA_128_32
    {0x00000000,0x00000000,0x00000000,0x00000000,0x00000080,
        0x00000000,0x00000000,0x00000000,0x00000000,0x00000080},
    #else  // CC_SUPPORT_PKA_64_16
    {0x00000000,0x00000000,0x00000080,0x00000000, 0x00000000,
        0x00000000,0x00000000,0x00000080,0x00000000,0x00000000},
    #endif

    384, /* Size of field modulus in bits */
    384, /* Size of order of generator in bits */
    5,   /* Size of each inserted Barret tag in words; 0 - if not inserted */

    CC_ECPKI_DomainID_secp384r1,    /* EC Domain identifier - enum */
    "SECG_PRIME_384R1"

};


/**
 @brief    the function returns the domain pointer id the domain is supported for the product;
        otherwise return NULL
 @return   return domain pointer or NULL

*/
const CCEcpkiDomain_t *CC_EcpkiGetSecp384r1DomainP(void)
{
    return &ecpki_domain_secp384r1;
}

