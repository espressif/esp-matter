/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_ecpki_types
 @{
 */

/*!
 @file
 @brief This file contains all the type definitions that are used for the
 CryptoCell ECPKI APIs.
 */

#ifndef _CC_ECPKI_TYPES_H
#define _CC_ECPKI_TYPES_H


#include "cc_bitops.h"
#include "cc_pal_types_plat.h"
#include "cc_hash_defs.h"
#include "cc_pka_defs_hw.h"
#include "cc_pal_compiler.h"
#ifdef USE_MBEDTLS_CRYPTOCELL
#include "mbedtls/md.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/
/*! The size of the internal buffer in words. */
#define CC_PKA_DOMAIN_LLF_BUFF_SIZE_IN_WORDS (10 + 3*CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS)

/**************************************************************************************
 *                Enumerators
 ***************************************************************************************/

/*------------------------------------------------------------------*/
/*! @brief EC domain idetifiers.

   For more information, see <em>Standards for Efficient Cryptography Group
   (SECG): SEC2 Recommended Elliptic Curve Domain Parameters, Version 1.0</em>.
*/
typedef enum
{
    /* For prime field */
    /*! EC secp192k1. */
    CC_ECPKI_DomainID_secp192k1,
    /*! EC secp192r1. */
    CC_ECPKI_DomainID_secp192r1,
    /*! EC secp224k1. */
    CC_ECPKI_DomainID_secp224k1,
    /*! EC secp224r1. */
    CC_ECPKI_DomainID_secp224r1,
    /*! EC secp256k1. */
    CC_ECPKI_DomainID_secp256k1,
    /*! EC secp256r1. */
    CC_ECPKI_DomainID_secp256r1,
    /*! EC secp384r1. */
    CC_ECPKI_DomainID_secp384r1,
    /*! EC secp521r1. */
    CC_ECPKI_DomainID_secp521r1,
    /*! Reserved.*/
    CC_ECPKI_DomainID_OffMode,
    /*! Reserved.*/
    CC_ECPKI_DomainIDLast      = 0x7FFFFFFF,

}CCEcpkiDomainID_t;


/*------------------------------------------------------------------*/
/*!
  @brief Hash operation mode.

  Defines hash modes according to <em>IEEE 1363-2000: IEEE Standard for
  Standard Specifications for Public-Key Cryptography</em>.
 */
typedef enum
{
    /*! The message data will be hashed with SHA-1. */
    CC_ECPKI_HASH_SHA1_mode    = 0,
    /*! The message data will be hashed with SHA-224. */
    CC_ECPKI_HASH_SHA224_mode  = 1,
    /*! The message data will be hashed with SHA-256. */
    CC_ECPKI_HASH_SHA256_mode  = 2,
    /*! The message data will be hashed with SHA-384. */
    CC_ECPKI_HASH_SHA384_mode  = 3,
    /*! The message data will be hashed with SHA-512. */
    CC_ECPKI_HASH_SHA512_mode  = 4,
    /*! The message data is a digest of SHA-1 and will not be hashed. */
    CC_ECPKI_AFTER_HASH_SHA1_mode    = 5,
    /*! The message data is a digest of SHA-224 and will not be hashed. */
    CC_ECPKI_AFTER_HASH_SHA224_mode  = 6,
    /*! The message data is a digest of SHA-256 and will not be hashed. */
    CC_ECPKI_AFTER_HASH_SHA256_mode  = 7,
    /*! The message data is a digest of SHA-384 and will not be hashed. */
    CC_ECPKI_AFTER_HASH_SHA384_mode  = 8,
    /*! The message data is a digest of SHA-512 and will not be hashed. */
    CC_ECPKI_AFTER_HASH_SHA512_mode  = 9,
    /*! The maximal number of hash modes. */
    CC_ECPKI_HASH_NumOfModes,
    /*! Reserved. */
    CC_ECPKI_HASH_OpModeLast        = 0x7FFFFFFF,

}CCEcpkiHashOpMode_t;


/*---------------------------------------------------*/
/*! EC point-compression identifiers.
*/
typedef enum
{
    /*! A compressed point. */
    CC_EC_PointCompressed     = 2,
    /*! An uncompressed point. */
    CC_EC_PointUncompressed   = 4,
    /*! An incorrect point-control value. */
    CC_EC_PointContWrong      = 5,
    /*! A hybrid point. */
    CC_EC_PointHybrid         = 6,
    /*! Reserved. */
    CC_EC_PointCompresOffMode = 8,
    /*! Reserved. */
    CC_ECPKI_PointCompressionLast= 0x7FFFFFFF,
}CCEcpkiPointCompression_t;

/*----------------------------------------------------*/
/*! EC key checks. */
typedef enum {
    /*! Check only preliminary input parameters. */
    CheckPointersAndSizesOnly = 0,
    /*! Check preliminary input parameters and verify that the EC public-key
    point is on the curve. */
    ECpublKeyPartlyCheck      = 1,
    /*! Check preliminary input parameters, verify that the EC public-key
    point is on the curve, and verify that \c EC_GeneratorOrder*PubKey = 0 */
    ECpublKeyFullCheck        = 2,
    /*! Reserved. */
    PublKeyChecingOffMode,
    /*! Reserved. */
    EC_PublKeyCheckModeLast  = 0x7FFFFFFF,
}ECPublKeyCheckMode_t;

/*----------------------------------------------------*/
/*! SW SCA protection type. */
typedef enum {
    /*! SCA protection inactive. */
    SCAP_Inactive,
    /*! SCA protection active. */
    SCAP_Active,
    /*! Reserved. */
    SCAP_OFF_MODE,
    /*! Reserved. */
    SCAP_LAST = 0x7FFFFFFF
}CCEcpkiScaProtection_t;

/**************************************************************************************
 *               EC Domain structure definition
 ***************************************************************************************/

/*!
 @brief The structure containing the EC domain parameters in little-endian
 form.

 EC equation: \c Y^2 = \c X^3 + \c A*X + \c B over prime field \p GFp.
 */
typedef  struct {
    /*! EC modulus: P. */
    uint32_t    ecP [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
    /*! EC equation parameter A. */
    uint32_t    ecA [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
    /*! EC equation parameter B. */
    uint32_t    ecB [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
    /*! Order of generator. */
    uint32_t    ecR [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 1];
    /*! EC cofactor EC_Cofactor_K. The coordinates of the EC base point
    generator in projective form. */
    uint32_t    ecGx [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
    /*! EC cofactor EC_Cofactor_K. The coordinates of the EC base point
    generator in projective form. */
    uint32_t    ecGy [CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
    /*! EC cofactor EC_Cofactor_K. The coordinates of the EC base point
    generator in projective form. */
    uint32_t    ecH;
    /*! Specific fields that are used by the low-level functions.*/
    uint32_t      llfBuff[CC_PKA_DOMAIN_LLF_BUFF_SIZE_IN_WORDS];
    /*! The size of fields in bits. */
    uint32_t    modSizeInBits;
    /*! The size of the order in bits. */
    uint32_t    ordSizeInBits;
    /*! The size of each inserted Barret tag in words. Zero if not inserted.*/
    uint32_t    barrTagSizeInWords;
    /*! The EC Domain identifier. */
    CCEcpkiDomainID_t   DomainID;
    /*! Internal buffer. */
    int8_t name[20];
}CCEcpkiDomain_t;



/**************************************************************************************
 *               EC  point structures definitions
 ***************************************************************************************/

/*! The structure containing the EC point in affine coordinates
   and little endian form. */
typedef  struct
{
    /*! The X coordinate of the point. */
    uint32_t x[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
    /*! The Y coordinate of the point. */
    uint32_t y[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];

}CCEcpkiPointAffine_t;


/**************************************************************************************
 *                ECPKI public and private key  Structures
 ***************************************************************************************/

/* --------------------------------------------------------------------- */
/* .................. The public key structures definitions ............ */
/* --------------------------------------------------------------------- */

/*! The structure containing the public key in affine coordinates.*/
typedef  struct
{
    /*! The X coordinate of the public key.*/
    uint32_t x[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
    /*! The Y coordinate of the public key.*/
    uint32_t y[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
    /*! The EC Domain.*/
    CCEcpkiDomain_t  domain;
    /*! The point type.*/
    uint32_t pointType;
} CCEcpkiPublKey_t;


/*!
@brief The user structure prototype of the EC public key.

This structure must be saved by the user. It is used as input to ECC functions,
for example, CC_EcdsaVerify().
*/
typedef struct   CCEcpkiUserPublKey_t
{
    /*! The validation tag. */
    uint32_t    valid_tag;
    /*! The data of the public key. */
    uint32_t    PublKeyDbBuff[(sizeof(CCEcpkiPublKey_t)+3)/4];
} CCEcpkiUserPublKey_t;


/* --------------------------------------------------------------------- */
/* .................. The private key structures definitions ........... */
/* --------------------------------------------------------------------- */

/*! The structure containing the data of the private key. */
typedef  struct
{
    /*! The data of the private key. */
    uint32_t  PrivKey[CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 1];
    /*! The EC domain. */
    CCEcpkiDomain_t  domain;
    /*! The SCA protection mode. */
    CCEcpkiScaProtection_t  scaProtection;
}CCEcpkiPrivKey_t;


/*!
 @brief The user structure prototype of the EC private key.

 This structure must be saved by the user. It is used as input to ECC functions,
 for example, CC_EcdsaSign().
 */
typedef struct   CCEcpkiUserPrivKey_t
{
    /*! The validation tag. */
    uint32_t    valid_tag;
    /*! The data of the private key. */
    uint32_t    PrivKeyDbBuff[(sizeof(CCEcpkiPrivKey_t)+3)/4];
}  CCEcpkiUserPrivKey_t;

/*! The type of the ECDH temporary data. */
typedef struct CCEcdhTempData_t
{
    /*! Temporary buffers. */
    uint32_t ccEcdhIntBuff[CC_PKA_ECDH_BUFF_MAX_LENGTH_IN_WORDS];
}CCEcdhTempData_t;

/*! EC build temporary data. */
typedef struct CCEcpkiBuildTempData_t
{
    /*! Temporary buffers. */
    uint32_t  ccBuildTmpIntBuff[CC_PKA_ECPKI_BUILD_TMP_BUFF_MAX_LENGTH_IN_WORDS];
}CCEcpkiBuildTempData_t;



/**************************************************************************
 *                CryptoCell ECDSA context structures
 **************************************************************************/

/* --------------------------------------------------------------------- */
/*                CryptoCell ECDSA Signing context structure                   */
/* --------------------------------------------------------------------- */
/*! The internal buffer used in the signing process. */
typedef uint32_t CCEcdsaSignIntBuff_t[CC_PKA_ECDSA_SIGN_BUFF_MAX_LENGTH_IN_WORDS];

/*! The context definition for the signing operation. */
typedef  struct
{
    /*! The data of the private key. */
    CCEcpkiUserPrivKey_t     ECDSA_SignerPrivKey;

#ifdef USE_MBEDTLS_CRYPTOCELL
    /*! The hash context. */
    mbedtls_md_context_t     hash_ctx;
#else
    /*! The hash context. */
    CCHashUserContext_t      hashUserCtxBuff;
#endif
    /*! The hash result buffer. */
    CCHashResultBuf_t        hashResult;
    /*! The size of the hash result in words. */
    uint32_t                 hashResultSizeWords;
    /*! The hash mode. */
    CCEcpkiHashOpMode_t  hashMode;
    /*! Internal buffer. */
    CCEcdsaSignIntBuff_t     ecdsaSignIntBuff;
}EcdsaSignContext_t;


/* --------------------------------------------------------------------- */
/*                ECDSA  Signing User context database              */
/* --------------------------------------------------------------------- */

/*!
 @brief The context definition of the user for the signing operation.

 This context saves the state of the operation, and must be saved by the user
 until the end of the API flow.
 */
typedef struct  CCEcdsaSignUserContext_t
{
    /*! The data of the signing process. */
    uint32_t  context_buff [(sizeof(EcdsaSignContext_t)+3)/4];
    /*! The validation tag. */
    uint32_t  valid_tag;
} CCEcdsaSignUserContext_t;



/****************************************************************************/

/* --------------------------------------------------------------------- */
/*                ECDSA Verifying context structure                 */
/* --------------------------------------------------------------------- */
/*! The internal buffer used in the verification process. */
typedef uint32_t CCEcdsaVerifyIntBuff_t[CC_PKA_ECDSA_VERIFY_BUFF_MAX_LENGTH_IN_WORDS];

/*! The context definition for verification operation. */
typedef  struct
{
    /*! The data of the public key. */
    CCEcpkiUserPublKey_t        ECDSA_SignerPublKey;

#ifdef USE_MBEDTLS_CRYPTOCELL
    /*! The hash context. */
    mbedtls_md_context_t        hash_ctx;
#else
    /*! The hash context. */
    CCHashUserContext_t         hashUserCtxBuff;
#endif
    /*! The hash result. */
    CCHashResultBuf_t           hashResult;
    /*! The size of the hash result in words. */
    uint32_t                    hashResultSizeWords;
    /*! The hash mode. */
    CCEcpkiHashOpMode_t         hashMode;
    /*! Internal buffer. */
    CCEcdsaVerifyIntBuff_t      ccEcdsaVerIntBuff;
}EcdsaVerifyContext_t;


/* --------------------------------------------------------------------- */
/*                ECDSA Verifying User context database             */
/* --------------------------------------------------------------------- */
/*!
 @brief The context definition of the user for the verification operation.

 The context saves the state of the operation, and must be saved by the user
 until the end of the API flow.
 */
typedef struct  CCEcdsaVerifyUserContext_t
{
    /*! The data of the verification process. */
    uint32_t    context_buff[(sizeof(EcdsaVerifyContext_t)+3)/4];
    /*! The validation tag. */
    uint32_t    valid_tag;
}CCEcdsaVerifyUserContext_t;


/* --------------------------------------------------------------------- */
/* .................. key generation temp buffer   ........... */
/* --------------------------------------------------------------------- */

/*! The temporary data type of the ECPKI KG. */
typedef struct CCEcpkiKgTempData_t
{
    /*! Internal buffer. */
    uint32_t ccKGIntBuff[CC_PKA_KG_BUFF_MAX_LENGTH_IN_WORDS];
}CCEcpkiKgTempData_t;

/*! The temporary data definition of the ECIES. */
typedef struct CCEciesTempData_t {
    /*! The data of the private key. */
    CCEcpkiUserPrivKey_t   PrivKey;
    /*! The data of the public key. */
    CCEcpkiUserPublKey_t   PublKey;
    /*! The public-key data used by conversion from Mbed TLS to CryptoCell. */
    CCEcpkiUserPublKey_t   ConvPublKey;
    /*! Internal buffer. */
    uint32_t  zz[3*CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 1];
    /*! Internal buffers. */
    union {
        CCEcpkiBuildTempData_t buildTempbuff;
        CCEcpkiKgTempData_t    KgTempBuff;
        CCEcdhTempData_t       DhTempBuff;
    } tmp;
}CCEciesTempData_t;


/* --------------------------------------------------------------------- */
/* .................. defines for FIPS      ........... */
/* --------------------------------------------------------------------- */

/*! The order length for FIPS ECC tests. */
#define CC_ECPKI_FIPS_ORDER_LENGTH (256/CC_BITS_IN_BYTE)  // the order of secp256r1 in bytes

/*! ECPKI data structures for FIPS certification. */
typedef struct CCEcpkiKgFipsContext_t
{
    /*! Signing and verification data. */
    union {
        CCEcdsaSignUserContext_t    signCtx;
        CCEcdsaVerifyUserContext_t  verifyCtx;
    }operationCtx;
    /*! Internal buffer. */
    uint32_t    signBuff[2*CC_ECPKI_ORDER_MAX_LENGTH_IN_WORDS] ;
}CCEcpkiKgFipsContext_t;

/*! ECDSA KAT data structures for FIPS certification.
    The ECDSA KAT tests are defined for domain 256r1.     */
typedef struct CCEcdsaFipsKatContext_t{
    /*! The key data. */
    union {
        /*! The private key data. */
        struct {
            CCEcpkiUserPrivKey_t    PrivKey;
            CCEcdsaSignUserContext_t    signCtx;
        }userSignData;
        /*! The public key data. */
        struct {
            CCEcpkiUserPublKey_t    PublKey;
            union {
                CCEcdsaVerifyUserContext_t  verifyCtx;
                CCEcpkiBuildTempData_t  tempData;
            }buildOrVerify;
        }userVerifyData;
    }keyContextData;
    /*! Internal buffer. */
    uint8_t         signBuff[2*CC_ECPKI_FIPS_ORDER_LENGTH];
}CCEcdsaFipsKatContext_t;

/*! ECDH KAT data structures for FIPS certification. */
typedef struct CCEcdhFipsKatContext_t{
    /*! The public key data. */
    CCEcpkiUserPublKey_t  pubKey;
    /*! The private key data. */
    CCEcpkiUserPrivKey_t  privKey;
    /*! Internal buffers. */
    union {
        CCEcpkiBuildTempData_t  ecpkiTempData;
        CCEcdhTempData_t      ecdhTempBuff;
    }tmpData;
    /*! The buffer for the secret key. */
    uint8_t secretBuff[CC_ECPKI_FIPS_ORDER_LENGTH];
}CCEcdhFipsKatContext_t;

#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif

