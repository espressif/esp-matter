/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_RSA_TYPES_H
#define _CC_RSA_TYPES_H

#include "cc_hash_defs.h"
#include "cc_bitops.h"
#include "cc_pka_defs_hw.h"
#include "cc_pal_types.h"
#include "cc_pal_compiler.h"

#ifdef CC_SOFT_KEYGEN
#include "ccsw_rsa_shared_types.h"
#endif
#ifdef USE_MBEDTLS_CRYPTOCELL
#include "md.h"
#else
#include "cc_hash.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif
/*!
@file
@@brief This file contains all the enums and definitions that are used for the CryptoCell RSA APIs.
@defgroup cc_rsa_types CryptoCell RSA used definitions and enums
@{
@ingroup cc_rsa
*/

/************************ Defines ******************************/

/*! Definition of HASH context size. */
#define CC_PKA_RSA_HASH_CTX_SIZE_IN_WORDS CC_HASH_USER_CTX_SIZE_IN_WORDS

/*! Maximal key size in bytes. */
#define CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES    (CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS / CC_BITS_IN_BYTE)

/*! Minimal key size in bits. */
#define CC_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS              512
/*! Valid key size multiplications in RSA. */
#define CC_RSA_VALID_KEY_SIZE_MULTIPLE_VALUE_IN_BITS         256

/*! Maximal RSA generated key size in bits. */
#define CC_RSA_MAX_KEY_GENERATION_SIZE_BITS   CC_RSA_MAX_KEY_GENERATION_HW_SIZE_BITS

/* FIPS 184-4 definitions for allowed RSA and FFC DH key sizes */
/*! FIPS 184-4 allowed key size - 1024 bits. */
#define CC_RSA_FIPS_KEY_SIZE_1024_BITS   1024
/*! FIPS 184-4 allowed key size - 2048 bits. */
#define CC_RSA_FIPS_KEY_SIZE_2048_BITS   2048
/*! FIPS 184-4 allowed key size - 3072 bits. */
#define CC_RSA_FIPS_KEY_SIZE_3072_BITS   3072
/*! FIPS 184-4 allowed modulus size in bits. */
#define CC_RSA_FIPS_MODULUS_SIZE_BITS    CC_RSA_FIPS_KEY_SIZE_2048_BITS

/*! FIPS 184-4 DH key size - 1024 bits. */
#define CC_DH_FIPS_KEY_SIZE_1024_BITS    1024
/*! FIPS 184-4 DH key size - 2048 bits. */
#define CC_DH_FIPS_KEY_SIZE_2048_BITS    2048


/*! Salt length definition - if the salt length is not available in verify operation, the user can use this define and the algorithm will
    calculate the salt length alone*/
/*!\note Security wise: it is not recommended to use this flag.*/
#define CC_RSA_VERIFY_SALT_LENGTH_UNKNOWN                     0xFFFF

/*! Minimal public exponent value */
#define CC_RSA_MIN_PUB_EXP_VALUE  3
/*! Minimal private exponent value */
#define CC_RSA_MIN_PRIV_EXP_VALUE 1

/* The maximum buffer size for the 'H' value */
/*! Temporary buffer size definition.*/
#define CC_RSA_TMP_BUFF_SIZE (CC_RSA_OAEP_ENCODE_MAX_MASKDB_SIZE + CC_RSA_OAEP_ENCODE_MAX_SEEDMASK_SIZE + CC_PKA_RSA_HASH_CTX_SIZE_IN_WORDS*sizeof(uint32_t) + sizeof(CCHashResultBuf_t))

/*! Hash structure definition.*/
#define CCPkcs1HashFunc_t CCHashOperationMode_t

/*! OAEP maximal H length.*/
#define CC_RSA_OAEP_MAX_HLEN                            CC_HASH_SHA512_DIGEST_SIZE_IN_BYTES

/*! MGF1 definitions */
#define CC_RSA_MGF_2_POWER_32                       65535 /*!< \internal 0xFFFF This is the 2^32 of the 2^32*hLen boundary check */
/*! MGF1 definitions */
#define CC_RSA_SIZE_OF_T_STRING_BYTES                   (CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*sizeof(uint32_t))

/***********************************************************
 *
 * RSA PKCS#1 v2.1 DEFINES
 *
 ***********************************************************/
 /*! Size of OEAP seed. */
#define CC_RSA_OAEP_ENCODE_MAX_SEEDMASK_SIZE            CC_RSA_OAEP_MAX_HLEN
/*! Maximal PSS salt size. */
#define CC_RSA_PSS_SALT_LENGTH                  CC_RSA_OAEP_MAX_HLEN
/*! PSS padding length. */
#define CC_RSA_PSS_PAD1_LEN                 8
/*! OAEP encode mask size. */
#define CC_RSA_OAEP_ENCODE_MAX_MASKDB_SIZE              (CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*sizeof(uint32_t)) /*!< \internal For OAEP Encode; the max size is emLen */
/*! OAEP decode mask size. */
#define CC_RSA_OAEP_DECODE_MAX_DBMASK_SIZE              (CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS*sizeof(uint32_t)) /*!< \internal For OAEP Decode; the max size is emLen */

/************************ Enums ********************************/

/*! Defines the enum for the HASH operation mode. */
typedef enum
{
    CC_RSA_HASH_MD5_mode  = 0,  /*!< For Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version 1.5 only.
                         The input data will be hashed with MD5 */
    CC_RSA_HASH_SHA1_mode = 1,  /*!< The input data will be hashed with SHA1. */
    CC_RSA_HASH_SHA224_mode = 2,  /*!< The input data will be hashed with SHA224. */
    CC_RSA_HASH_SHA256_mode = 3,  /*!< The input data will be hashed with SHA256. */
    CC_RSA_HASH_SHA384_mode = 4,  /*!< The input data will be hashed with SHA384. */
    CC_RSA_HASH_SHA512_mode = 5,    /*!< The input data will be hashed with SHA512. */
    CC_RSA_After_MD5_mode = 6,      /*!< For PKCS1 v1.5 only. The input data is a digest of MD5 and will not be hashed. */
    CC_RSA_After_SHA1_mode = 7, /*!< The input data is a digest of SHA1 and will not be hashed. */
    CC_RSA_After_SHA224_mode = 8,   /*!< The input data is a digest of SHA224 and will not be hashed. */
    CC_RSA_After_SHA256_mode = 9,   /*!< The input data is a digest of SHA256 and will not be hashed. */
    CC_RSA_After_SHA384_mode = 10,  /*!< The input data is a digest of SHA384 and will not be hashed. */
    CC_RSA_After_SHA512_mode = 11,  /*!< The input data is a digest of SHA512 and will not be hashed. */
    CC_RSA_After_HASH_NOT_KNOWN_mode = 12,    /*!< \internal used only for Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version 1.5 -
                            possible to perform verify operation without hash mode input,
                        the hash mode is derived from the signature.*/
    CC_RSA_HASH_NO_HASH_mode = 13,  /*!< Used for Public-Key Cryptography Standards (PKCS) #1 RSA Cryptography Specifications Version 1.5 Encrypt and Decrypt.*/
    CC_RSA_HASH_NumOfModes,     /*!< Maximal number of hash operations modes. */

    CC_RSA_HASH_OpModeLast  = 0x7FFFFFFF, /*! Reserved.*/

}CCRsaHashOpMode_t;


/*! Defines the enum of the RSA decryption mode. */
typedef enum
{
    CC_RSA_NoCrt = 10, /*!< Decryption no CRT mode.*/
    CC_RSA_Crt   = 11, /*!< Decryption CRT mode.*/

    CC_RSADecryptionNumOfOptions, /*! Reserved.*/

    CC_RSA_DecryptionModeLast= 0x7FFFFFFF, /*! Reserved.*/

}CCRsaDecryptionMode_t;

/*! RSA Key source definition. */
typedef enum
{
    CC_RSA_ExternalKey = 1, /*!< External key.*/
    CC_RSA_InternalKey = 2, /*!< Internal key.*/

    CC_RSA_KeySourceLast= 0x7FFFFFFF, /*!< Reserved. */

}CCRsaKeySource_t;

/*! MGF values. */
typedef enum
{
    CC_PKCS1_MGF1 = 0,   /*! MGF1. */
    CC_PKCS1_NO_MGF = 1, /*! No MGF. */
    CC_RSA_NumOfMGFFunctions, /*! Maximal number of MGF options. */

    CC_PKCS1_MGFLast= 0x7FFFFFFF, /*! Reserved.*/

}CCPkcs1Mgf_t;

/*! Defines the enum of the various PKCS1 versions. */
typedef enum
{
    CC_PKCS1_VER15 = 0, /*! PKCS1 version 15. */
    CC_PKCS1_VER21 = 1, /*! PKCS1 version 21. */

    CC_RSA_NumOf_PKCS1_versions, /*! Maximal number of PKCS versions. */

    CC_PKCS1_versionLast= 0x7FFFFFFF, /*! Reserved.*/

}CCPkcs1Version_t;


/*! Enum defining primality testing mode in Rabin-Miller
   and Lucas-Lehmer tests (internal tests). */
typedef enum
{
        /* P and Q primes */
    CC_RSA_PRIME_TEST_MODE     = 0, /*!< PRIME test. */

        /* FFC (DH, DSA) primes */
    CC_DH_PRIME_TEST_MODE      = 1, /*!< DH Prime test. */

    CC_RSA_DH_PRIME_TEST_OFF_MODE /*!< Reserved.*/

}CCRsaDhPrimeTestMode_t;

/************************ Public and private key database Structs ******************************/

/* .................. The public key definitions ...................... */
/* --------------------------------------------------------------------- */

/*! Public key data structure (used internally). */
typedef struct
{
    /*! RSA modulus buffer. */
    uint32_t n[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    /*! RSA modulus size in bits. */
    uint32_t nSizeInBits;

    /*! RSA public exponent buffer. */
    uint32_t e[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    /*! RSA public exponent buffer. */
    uint32_t eSizeInBits;

    /*! Buffer for internal usage.*/
    uint32_t ccRSAIntBuff[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];

}CCRsaPubKey_t;

/*! The public key's user structure prototype. This structure must be saved by the user, and is used as input to the RSA functions
(such as ::CC_RsaSchemesEncrypt etc.) */
typedef struct CCRsaUserPubKey_t
{
    /*! Validation tag. */
    uint32_t valid_tag;
    /*! Public key data. */
    uint32_t  PublicKeyDbBuff[ sizeof(CCRsaPubKey_t)/sizeof(uint32_t) + 1 ];


}CCRsaUserPubKey_t;

/* .................. The private key definitions ...................... */
/* --------------------------------------------------------------------- */

/*! Private key on non-CRT mode data structure (used internally). */
typedef struct
{
    /*! RSA private exponent buffer. */
    uint32_t d[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    /*! RSA private exponent size in bits. */
    uint32_t dSizeInBits;

    /*! RSA public exponent buffer. */
    uint32_t e[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    /*! RSA public exponent size in bits. */
    uint32_t eSizeInBits;

}CCRsaPrivNonCrtKey_t;

/*! Private key on CRT mode data structure (used internally). */
/* use small CRT buffers */
typedef struct
{
    /*! First factor buffer. */
    uint32_t P[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS/2];
    /*! First factor size in bits. */
    uint32_t PSizeInBits;

    /*! Second factor buffer. */
    uint32_t Q[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS/2];
    /*! Second factor size in bits. */
    uint32_t QSizeInBits;

    /*! First CRT exponent buffer. */
    uint32_t dP[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS/2];
    /*! First CRT exponent size in bits. */
    uint32_t dPSizeInBits;

    /*! Second CRT exponent buffer. */
    uint32_t dQ[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS/2];
    /*! Second CRT exponent size in bits. */
    uint32_t dQSizeInBits;

    /*! First CRT coefficient buffer. */
    uint32_t qInv[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS/2];
    /*! First CRT coefficient size in bits. */
    uint32_t qInvSizeInBits;

}CCRsaPrivCrtKey_t;


/*! Private key data structure (used internally). */
typedef struct
{
    /*! RSA modulus buffer. */
    uint32_t n[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    /*! RSA modulus size in bits. */
    uint32_t nSizeInBits;

    /*! Decryption operation mode. */
    CCRsaDecryptionMode_t OperationMode;

    /*! Key source ( internal or external ). */
    CCRsaKeySource_t KeySource;


    /*! Union between the CRT and non-CRT data structures. */
    union
    {
        CCRsaPrivNonCrtKey_t NonCrt; /*!< Non CRT data structure. */
        CCRsaPrivCrtKey_t    Crt;    /*!< CRT data structure. */
    }PriveKeyDb;

    /*! Internal buffer. */
    uint32_t ccRSAPrivKeyIntBuff[CC_PKA_PRIV_KEY_BUFF_SIZE_IN_WORDS];

}CCRsaPrivKey_t;

/*! The private key's user structure prototype. This structure must be saved by the user, and is used as input to the RSA functions
(such as ::CC_RsaSchemesDecrypt etc.). */
typedef struct CCRsaUserPrivKey_t
{
    /*! Validation tag.*/
    uint32_t valid_tag;
    /*! Private key data. */
    uint32_t  PrivateKeyDbBuff[ sizeof(CCRsaPrivKey_t)/sizeof(uint32_t) + 1 ] ;

}CCRsaUserPrivKey_t;

/*! Temporary buffers for RSA usage. */
typedef struct CCRsaPrimeData_t
{
   /* The aligned input and output data buffers */
    uint32_t DataIn[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];   /*!< Temporary buffer for data in.*/
    uint32_t DataOut[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];  /*!< Temporary buffer for data out.*/
    /*! Temporary buffer for internal data.*/
    uint8_t  InternalBuff[CC_RSA_TMP_BUFF_SIZE] CC_PAL_COMPILER_ALIGN (4);

}CCRsaPrimeData_t;

/*! KG data type. */
typedef union CCRsaKgData_t
{
    /*! RSA Key Generation buffers definitions. */
    struct
    {
        /* The aligned input and output data buffers */
        /*! First factor buffer. */
        uint32_t p[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS / 2];
        /*! Second factor buffer. */
        uint32_t q[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS / 2];
        union {
            /*! Internal buffer. */
            uint32_t ccRSAKGDataIntBuff[CC_PKA_KGDATA_BUFF_SIZE_IN_WORDS];
            #ifdef CC_SOFT_KEYGEN
            /* # added for compatibility with size of KGData SW type */
            uint32_t TempbuffExp[PKI_KEY_GEN_TEMP_BUFF_SIZE_WORDS];
            #endif
        }kg_buf;
    }KGData;

    /*! Buffers for internal usage. */
    union {
        /*! Internally used buffer.*/
            struct {
            CCRsaPrimeData_t    PrimData;
        }primExt;
        #ifdef CC_SOFT_KEYGEN
        /* # added for compatibility with size of SW CCRsaPrivKey_t type */
            SwSharedRSAPrimeData_t SW_Shared_PrimData;
        #endif
    }prim;
}CCRsaKgData_t;

 /*************
 *  RSA contexts
 **************/
/************************ CryptoCell RSA struct for Private Key ******************************/


/*! Context definition for operations that use the RSA private key. */
typedef struct
{

    /*! Private key data.  */
    CCRsaUserPrivKey_t PrivUserKey;

    /*! RSA PKCS#1 Version 1.5 or 2.1 */
    uint8_t   PKCS1_Version;

    /*! MGF to be used for the PKCS1 Ver 2.1 sign or verify operations. */
    uint8_t MGF_2use;

    /*! Salt random length for PKCS#1 PSS Ver 2.1*/
    uint16_t SaltLen;

    /*! Internal buffer. */
    CCRsaPrimeData_t  PrimeData;

    /*! HASH context buffer. */
#ifdef USE_MBEDTLS_CRYPTOCELL
    mbedtls_md_context_t RsaHashCtx;
#else
    uint32_t RsaHashCtxBuff[CC_PKA_RSA_HASH_CTX_SIZE_IN_WORDS];
#endif

    /*! HASH result buffer. */
    CCHashResultBuf_t        HASH_Result;
    /*! HASH result size in words. */
    uint16_t                  HASH_Result_Size;
    /*! RSA HASH operation mode (all modes RSA supports).*/
    CCRsaHashOpMode_t    RsaHashOperationMode;
    /*! HASH operation mode.*/
    CCHashOperationMode_t HashOperationMode;
    /*! HASH block size (in words).*/
    uint16_t                  HashBlockSize;
    /*! HASH flag. */
    bool doHash;

    /* Used for sensitive data manipulation in the context space, which is safer and which saves stack space */
    /*! Internal buffer.*/
    uint32_t EBD[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    /*! Internal bufffer used size in bits. */
    uint32_t EBDSizeInBits;

    /* Used for sensitive data manipulation in the context space, which is safer and which saves stack space */
    /*! Internal buffer.*/
    uint8_t  T_Buf[CC_RSA_SIZE_OF_T_STRING_BYTES];
    /*! Internal buffer used size.*/
    uint16_t T_BufSize;

    /*! Buffer for the use of the Ber encoder in the case of PKCS#1 Ver 1.5. */
    uint32_t  BER[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    /*! Ber encoder buffer size.*/
    uint16_t  BERSize;

    /*! Internal buffer.*/
    uint8_t DummyBufAESBlockSize[16];

}RSAPrivContext_t;

/*! The user's context prototype - the argument type that is passed by the user
   to the RSA APIs. The context saves the state of the operation and must be saved by the user
   till the end of the APIs flow . */
typedef struct CCRsaPrivUserContext_t
{
    /*! Validation tag. */
    uint32_t valid_tag;
    /*! Internally used value.*/
    uint32_t AES_iv;
    /*! Private data context buffer. */
    uint8_t  context_buff[ sizeof(RSAPrivContext_t) + sizeof(uint32_t)] CC_PAL_COMPILER_ALIGN (4);  /* must be aligned to 4 */

}CCRsaPrivUserContext_t;


/************************ CryptoCell RSA struct for Public Key ******************************/

/*! Context definition for operations that use the RSA public key. */
typedef struct
{

    /*! RSA public key structure. */
    CCRsaUserPubKey_t PubUserKey;

    /*! Public key size in bytes */
    uint32_t nSizeInBytes;

    /*! RSA PKCS#1 Version 1.5 or 2.1 */
    uint8_t   PKCS1_Version;

    /*! MGF to be used for the PKCS1 Ver 2.1 Sign or Verify operations */
    uint8_t MGF_2use;

    /*! Salt random length for PKCS#1 PSS Ver 2.1*/
    uint16_t SaltLen;

    /*! Internal buffer. */
    CCRsaPrimeData_t  PrimeData;

    /*! HASH context. */
#ifdef USE_MBEDTLS_CRYPTOCELL
    mbedtls_md_context_t RsaHashCtx;
#else
    uint32_t RsaHashCtxBuff[CC_PKA_RSA_HASH_CTX_SIZE_IN_WORDS];
#endif
    /*! HASH result buffer. */
    CCHashResultBuf_t        HASH_Result;
    /*! HASH result size. */
    uint16_t                  HASH_Result_Size; /* denotes the length, in words, of the hash function output */
    /*! RSA HASH operation mode (all modes RSA supports). */
    CCRsaHashOpMode_t    RsaHashOperationMode;
    /*! HASH operation mode. */
    CCHashOperationMode_t HashOperationMode;
    /*! HASH block size. */
    uint16_t                  HashBlockSize; /*in words*/
    /*! HASH flag.*/
    bool doHash;

    /* Used for sensitive data manipulation in the context space, which is safer and which saves stack space */
    /*! Internal buffer.*/
    uint32_t EBD[CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    /*! Internal bufffer used size in bits. */
    uint32_t EBDSizeInBits;

    /* Used for sensitive data manipulation in the context space, which is safer and which saves stack space */
    /*! Internal buffer.*/
    uint8_t T_Buf[CC_RSA_SIZE_OF_T_STRING_BYTES];
    /*! Internal buffer used size.*/
    uint16_t T_BufSize;

    /*! Internal buffer.*/
    uint8_t DummyBufAESBlockSize[16];

}RSAPubContext_t;


/*! Temporary buffers for the RSA usage. */
typedef struct CCRsaPubUserContext_t
{
    /*! Validation tag. */
    uint32_t valid_tag;
    /*! Internally used value.*/
    uint32_t AES_iv;
    /*! Public data context buffer. */
    uint32_t  context_buff[ sizeof(RSAPubContext_t)/sizeof(uint32_t) + 1] ;

}CCRsaPubUserContext_t;



/*! Required for internal FIPS verification for RSA key generation. */
typedef struct CCRsaKgFipsContext_t{
    /*! Internal buffer. */
    CCRsaPrimeData_t    primData;
    /*! Buffer used for decryption. */
    uint8_t         decBuff[((CC_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS/CC_BITS_IN_BYTE) - 2*(CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES) -2)];
    /*! Buffer used for encryption. */
    uint8_t         encBuff[CC_RSA_FIPS_MODULUS_SIZE_BITS/CC_BITS_IN_BYTE];
}CCRsaKgFipsContext_t;

/*! Required for internal FIPS verification for RSA KAT. The RSA KAT tests are defined for Public-Key Cryptography Standards (PKCS) #1 RSA*
    Cryptography Specifications Version 2.1 with modulus key size of 2048.      */
typedef struct CCRsaFipsKatContext_t{
    /*! RSA user's key (either public or private).*/
    union {
        /*! RSA user's public key. */
        CCRsaUserPubKey_t   userPubKey;     // used for RsaEnc and RsaVerify
        /*! RSA user's private key. */
        CCRsaUserPrivKey_t   userPrivKey;  // used for RsaDec and RsaSign
    }userKey;
    /*! RSA user's context (either public or private).*/
    union {
        /*! RSA user's private context. */
        CCRsaPrivUserContext_t userPrivContext;  // used for RsaSign
        /*! RSA public user's context. */
        CCRsaPubUserContext_t userPubContext;  // used for RsaVerify
        /*! Internal buffers. */
        CCRsaPrimeData_t    primData;  // used for RsaEnc and RsaDec
    }userContext;
    /*! RSA user's data. */
    union {
        struct {   // used for RsaEnc and RsaDec
            /*! Buffer for encrypted data. */
            uint8_t     encBuff[CC_RSA_FIPS_MODULUS_SIZE_BITS/CC_BITS_IN_BYTE];
            /*! Buffer for decrypted data. */
            uint8_t     decBuff[((CC_RSA_FIPS_MODULUS_SIZE_BITS/CC_BITS_IN_BYTE) - 2*(CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES) -2)];
        }userOaepData;
        /*! Buffer for Signed data. */
        uint8_t         signBuff[CC_RSA_FIPS_MODULUS_SIZE_BITS/CC_BITS_IN_BYTE]; // used for RsaSign and RsaVerify
    }userData;
}CCRsaFipsKatContext_t;


#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif
