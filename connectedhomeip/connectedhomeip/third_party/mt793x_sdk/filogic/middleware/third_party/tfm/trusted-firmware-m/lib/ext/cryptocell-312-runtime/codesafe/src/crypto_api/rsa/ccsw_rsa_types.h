/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CCSW_RSA_TYPES_H
#define CCSW_RSA_TYPES_H


#include "cc_pal_types.h"
#include "cc_hash.h"
#include "cc_rsa_types.h"
#include "ccsw_rsa_shared_types.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

#define   PLS_FALSE  0UL
#define   PLS_TRUE   1UL

/************************************************************************/
/* the following definitions are only relevant for RSA code on SW */
/************************************************************************/
/* Define the maximal allowed width of the exponentiation sliding window
in range 2...6. This define is actual for projects on soft platform.
To minimize code size use the minimum value. To optimize performance
choose the maximum value */
/* Define the size of the exponentiation temp buffer, used in LLF_PKI and NON DEPENDED on
width of the sliding window. The size defined in units equaled to maximal RSA modulus size */
#define PKI_CONV_CRT_CONST_TEMP_BUFF_SIZE_IN_MODULUS_UNITS  16

/**************  Calculation of buffers sizes in words *******************************/

/* Size of buffers for sliding window exponents */
#if (PKI_EXP_SLIDING_WINDOW_MAX_VALUE == 6)
#define PKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS  34
#else
#define PKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS  (3 + (1 << (PKI_EXP_SLIDING_WINDOW_MAX_VALUE-1)))
#endif




/* Define the size of the temp buffer, used in LLF_PKI_CONVERT_TO_CRT and DEPENDED on
   width of the sliding window in words */
#if (PKI_CONV_CRT_CONST_TEMP_BUFF_SIZE_IN_MODULUS_UNITS > PKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS )
#define PKI_CONV_CRT_TEMP_BUFFER_SIZE_IN_WORDS  \
    (PKI_CONV_CRT_CONST_TEMP_BUFF_SIZE_IN_MODULUS_UNITS * SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS + 2 )
#else
#define PKI_CONV_CRT_TEMP_BUFFER_SIZE_IN_WORDS  \
    (PKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS * SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS + 2 )
#endif

#define SW_CC_RSA_VALID_KEY_SIZE_MULTIPLE_VALUE_IN_BITS     256

/* maximal allowed key size in words */
#define SW_CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BYTES    (SW_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS / 8)




/************************ Public and private key database Structs ******************************/

/* .................. The public key definitions ...................... */
/* --------------------------------------------------------------------- */



/* The public key data structure */
typedef struct {
    /* The RSA modulus buffer and its size in bits */
    uint32_t n[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    uint32_t nSizeInBits;

    /* The RSA public exponent buffer and its size in bits */
    uint32_t e[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    uint32_t eSizeInBits;
    /* # added for compatibility with size of CC CCRsaPubKey_t type */
    uint32_t ccRSAIntBuff[CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS];

}SwRsaPubKey_t;

/* The user structure prototype used as an input to the CC_RsaPrimEncrypt */
typedef struct CCSwRsaUserPubKey_t {

        uint32_t valid_tag;
        uint32_t  PublicKeyDbBuff[sizeof(SwRsaPubKey_t)/sizeof(uint32_t)+1];
}CCSwRsaUserPubKey_t;

/* .................. The private key definitions ...................... */
/* --------------------------------------------------------------------- */

/* The private key on non-CRT mode data structure */
typedef struct {
    /* The RSA private exponent buffer and its size in bits */
    uint32_t d[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    uint32_t dSizeInBits;

    /* The RSA public exponent buffer and its size in bits */
    uint32_t e[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    uint32_t eSizeInBits;

}CCSwRsaPrivNonCRTKey_t;

/* The private key on CRT mode data structure */
#ifndef CC_NO_RSA_SMALL_CRT_BUFFERS_SUPPORT
/* use small CRT buffers */
typedef struct {
    /* The first factor buffer and size in bits */
    uint32_t P[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS/2];
    uint32_t PSizeInBits;

    /* The second factor buffer and its size in bits */
    uint32_t Q[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS/2];
    uint32_t QSizeInBits;

    /* The first CRT exponent buffer and its size in bits */
    uint32_t dP[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS/2];
    uint32_t dPSizeInBits;

    /* The second CRT exponent buffer and its size in bits */
    uint32_t dQ[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS/2];
    uint32_t dQSizeInBits;

    /* The first CRT coefficient buffer and its size in bits */
    uint32_t qInv[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS/2];
    uint32_t qInvSizeInBits;

}CCSwRsaPrivCrtKey_t;


#else /* use large CRT buffers */
typedef struct {
    /* The first factor buffer and size in bits */
    uint32_t P[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    uint32_t PSizeInBits;

    /* The second factor buffer and its size in bits */
    uint32_t Q[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    uint32_t QSizeInBits;

    /* The first CRT exponent buffer and its size in bits */
    uint32_t dP[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    uint32_t dPSizeInBits;

    /* The second CRT exponent buffer and its size in bits */
    uint32_t dQ[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    uint32_t dQSizeInBits;

    /* The first CRT coefficient buffer and its size in bits */
    uint32_t qInv[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    uint32_t qInvSizeInBits;

}CCSwRsaPrivCrtKey_t;

#endif

/* The private key data structure: */
typedef struct {
    /* The RSA modulus buffer and its size in bits */
    uint32_t n[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    uint32_t nSizeInBits;

    /* The decryption operation mode */
    CCRsaDecryptionMode_t OperationMode;

    /* the source flag: 1 - External;  2 - Internal generation */
    CCRsaKeySource_t KeySource;

    /* The union between the CRT and non-CRT data structures */
    union {
        CCSwRsaPrivNonCRTKey_t NonCrt;
        CCSwRsaPrivCrtKey_t    Crt;
    }PriveKeyDb;

       /* # added for compatibility with size of CC CCRsaPrivKey_t type */
       uint32_t ccRSAPrivKeyIntBuff[CC_PKA_PRIV_KEY_BUFF_SIZE_IN_WORDS];

}SwRsaPrivKey_t;

/* Define the size of SwRsaPrivKey_t structure for using in temp buffers allocation */

/* The users Key structure prototype, used as an input to the
CC_RsaPrimDecrypt*/
typedef struct CCSwRsaUserPrivKey_t {

    uint32_t valid_tag;
    uint32_t  PrivateKeyDbBuff[sizeof(SwRsaPrivKey_t)/sizeof(uint32_t)+1];
}CCSwRsaUserPrivKey_t;

/* the RSA data type */
typedef struct CCSwRsaPrimeData_t {
    /* The aligned input and output data buffers */
    uint32_t DataIn[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
    uint32_t DataOut[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];

    /* #include specific fields that are used by the low level */
    struct {
        union {
            struct { /* Temporary buffers used for the exponent calculation */
                uint32_t Tempbuff1[PKI_EXP_TEMP_BUFFER_SIZE_IN_WORDS];
                uint32_t Tempbuff2[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS * 2];
                /* Temporary buffer for self-test support */
                uint32_t TempBuffer[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS];
            }NonCrt;

            struct { /* Temporary buffers used for the exponent calculation */
                uint32_t Tempbuff1[PKI_EXP_TEMP_BUFFER_SIZE_IN_WORDS];
                uint32_t Tempbuff2[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS * 2];
            }Crt;
        }Data;
    }LLF;

}CCSwRsaPrimeData_t;

/* the KG data type */
typedef union CCSwRsaKgData_t {
    struct {
        /* The aligned input and output data buffers */
        uint32_t p[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS / 2];
        uint32_t q[SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS / 2];
        /* Temporary buffers used for the exponent calculation */
        uint32_t TempbuffExp[PKI_KEY_GEN_TEMP_BUFF_SIZE_WORDS];

    }KGData;

    CCSwRsaPrimeData_t PrimData;

}CCSwRsaKgData_t;

/* .......................... Temp buff definition  ........................ */
/* ------------------------------------------------------------------------- */

/* the RSA Convert Key to CRT data type */
typedef struct CCSwRsaConvertKeyToCrtBuffers_t {
    /* #include specific fields that are used by the low level */
    struct {
        uint32_t TempBuffers[ 7*SW_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS + PKI_EXP_TEMP_BUFFER_SIZE_IN_WORDS ];

    } LLF;
}CCSwRsaConvertKeyToCrtBuffers_t;


#ifdef __cplusplus
}
#endif
#endif
