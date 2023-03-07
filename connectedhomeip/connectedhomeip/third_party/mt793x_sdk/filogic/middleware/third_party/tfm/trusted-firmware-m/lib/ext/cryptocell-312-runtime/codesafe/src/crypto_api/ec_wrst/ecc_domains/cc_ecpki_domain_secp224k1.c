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
 *   Data base of CC_ECPKI_DomainID_secp224k1: structure of type  CCEcpkiDomain_t    *
 *       All data is given in little endian order of words in arrays               *
 ***********************************************************************************/
static const CCEcpkiDomain_t ecpki_domain_secp224k1 = {
    /* Field modulus :  GF_Modulus =  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFE56D - big end*/
    {0xFFFFE56D,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
    /* EC equation parameters a, b  */
    /* a = -0  - big end  from SEC2 */
    {0},
    /* b = 5 - big end  from SEC2 */
    {5},
    /* Order of generator: 10000000000000000000000000001DCE8D2EC6184CAF0A971769FB1F7 big end  from SEC2 */
    {0x769FB1F7,0xCAF0A971,0xD2EC6184,0x0001DCE8,0x00000000,0x00000000,0x00000000,0x00000001},

    /* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
    /* A1455B334DF099DF30FC28A169A467E9E47075A90F7E650EB6B7A45C   X - big end  from SEC2 */
    {0xB6B7A45C,0x0F7E650E,0xE47075A9,0x69A467E9,0x30FC28A1,0x4DF099DF,0xA1455B33},
    /* 7E089FED7FBA344282CAFBD6F7E319F7C0B0BD59E2CA4BDB556D61A5  Y - big end  from SEC2 */
    {0x556D61A5,0xE2CA4BDB,0xC0B0BD59,0xF7E319F7,0x82CAFBD6,0x7FBA3442,0x7E089FED},

    1, /* EC cofactor K */

    /* Barrett tags NP,RP */
    #ifdef CC_SUPPORT_PKA_128_32
    {0x00000000,0x00000000,0x00000000,0x00000000,0x00000080,
        0xFE23172D,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x000000FF},
    #else  // CC_SUPPORT_PKA_64_16
    {0x00000000,0x00000000,0x00000080,0x00000000, 0x00000000,
        0xFFFFFFFF,0xFFFFFFFF,0x000000FF,0x00000000,0x00000000},
    #endif

    224, /* Size of field modulus in bits */
    225, /* Size of order of generator in bits */
    5,   /* Size of each inserted Barret tag in words; 0 - if not inserted */

    CC_ECPKI_DomainID_secp224k1,    /* EC Domain identifier - enum */
    "SECG_PRIME_224K1"
};





/**
 @brief    the function returns the domain pointer id the domain is supported for the product;
        otherwise return NULL
 @return   return domain pointer or NULL

*/
const CCEcpkiDomain_t *CC_EcpkiGetSecp224k1DomainP(void)
{
    return &ecpki_domain_secp224k1;
}

