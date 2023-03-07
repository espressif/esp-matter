/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_RSA_LOCAL_H
#define _CC_RSA_LOCAL_H

#include "cc_error.h"
#include "cc_rsa_types.h"
#include "cc_rnd_common.h"


#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/* the RSA public key user validity TAG */
#define CC_RSA_PUB_KEY_VALIDATION_TAG 0x13579BDF

/* the RSA private key user validity TAG */
#define CC_RSA_PRIV_KEY_VALIDATION_TAG 0x2468ACE0

/* the RSA sign Context user validity TAG */
#define CC_RSA_SIGN_CONTEXT_VALIDATION_TAG   0x98765432
#define CC_RSA_VERIFY_CONTEXT_VALIDATION_TAG 0x45678901


typedef struct {
    uint16_t hashResultSize;
    CCHashOperationMode_t hashMode;
}RsaHash_t;

extern const RsaHash_t RsaHashInfo_t[CC_RSA_HASH_NumOfModes];
extern const uint8_t RsaSupportedHashModes_t[CC_RSA_HASH_NumOfModes];
#ifdef USE_MBEDTLS_CRYPTOCELL
extern const mbedtls_md_type_t RsaHash_CC_mbedtls_Info[CC_HASH_NumOfModes];
#endif

/***************

  ASN1 types - for BER Parser - used for PKCS#1 Ver 1.5

***************/

#define ASN1_BOOLEAN        1
#define ASN1_INTEGER        2
#define ASN1_BIT_STRING     3
#define ASN1_OCTET_STRING   4
#define ASN1_NULL       5
#define ASN1_OBJECT_IDENTIFIER  6
#define ASN1_SEQUENCE       16 /* 0x10 */
#define ASN1_SET        17 /* 0x11 */
#define ASN1_PRINTABLE_STRING   19 /* 0x13 */
#define ASN1_TELETEX_STRING 20 /* 0x14 */
#define ASN1_IA5STRING      22 /* 0x16 */
#define ASN1_UTC_TIME           23 /* 0x17 */

/**********************
 structures definition
 **********************/
#define TEST_MSB_BIT 0x80

/* Hash algorithm ID (DER code) structure type */
#define HASH_DER_CODE_MAX_SIZE_BYTES 24
typedef struct HashDerCode_t {
    uint32_t algIdSizeBytes;
    CCHashOperationMode_t hashMode;
    uint8_t algId[HASH_DER_CODE_MAX_SIZE_BYTES];
}HashDerCode_t;

/* For security goal the padding string PS in EME-PKCS1-v1_5 encodding method
   must be at least eight octets long */
#define PS_MIN_LEN 8


/************************ macros ********************************/

/************************ Typedefs  ****************************/


/************************ Structs  ******************************/
typedef struct CCRsaOaepData_t{
    uint8_t  MaskDB[CC_RSA_OAEP_ENCODE_MAX_MASKDB_SIZE];
    uint8_t  SeedMask[CC_RSA_OAEP_ENCODE_MAX_SEEDMASK_SIZE];
#ifdef USE_MBEDTLS_CRYPTOCELL
    mbedtls_md_context_t hash_ctx;
#else
    CCHashUserContext_t HashUsercontext;
#endif
    CCHashResultBuf_t         HashResultBuff;
}CC_PAL_COMPILER_TYPE_MAY_ALIAS CCRsaOaepData_t;

/************************ Public Variables **********************/


/************************ Public Functions **********************/

/*********************************************************************************/
/**
 * @brief The function implements PKCS#1 v1.5 (9.2) EMSA Encoding
 *        algorithm used in Sign/Verify operations.
 *
 * @author reuvenl (9/14/2014)
 *
 * @param K - The size of encoded message in octets.
 * @param hashMode - hash mode ID (enum).
 * @param pM - The Pointer to the Message M. In case of Sign it is a hash (H).
 * @param MSize - Denotes the Message size: for Sig/Ver = hashSize,
 *                for Enc/Dec <= K-hashAlgIdSize-PSS_MIN_LEN-3.
 * @param pOut - The pointer to a buffer which is at least K octets long.
 *
 * @return CCError_t
 */
CCError_t RsaEmsaPkcs1v15Encode(
        uint32_t K,
            CCHashOperationMode_t hashMode,
        uint8_t     *pM, /*mess.digest*/
        uint32_t     MSize,
        uint8_t     *pOut);



/**********************************************************************************************************/

CCError_t RsaOaepMGF1( uint16_t hLen, /*size in Bytes*/
                uint8_t * Z_ptr,
                uint16_t ZSize,/*size in Bytes*/
                uint32_t L,
                uint8_t  *Mask_ptr,
                CCPkcs1HashFunc_t hashFunc,
                uint8_t  *T_Buf,    /*T_Buf is a buffer used for data manipulation for the function to use instead of allocating the space on stack*/
                uint8_t  *T_TMP_Buf);/*T_TMP_Buf is a buffer used for data manipulation for the function to use instead of allocating the space on stack*/

/**********************************************************************************************************/
/**
   @brief
   RsaPssOaepEncode implements the the Encoding operation according to the PKCS#1 as defined
   in PKCS#1 v2.1 7.1.1 (2) and PKCS#1 v2.0
*/
CCError_t RsaPssOaepEncode(CCRndContext_t *rndContext_ptr, /* random functions context */
                             CCPkcs1HashFunc_t hashFunc,     /* PKCS1 hash mode enum */
                             CCPkcs1Mgf_t MGF,               /* MGF function type enum */
                             uint8_t *M_ptr,                     /* a pointer to the message to be encoded */
                             uint16_t MSize,                     /* the message size in bytes */
                             uint8_t *P_ptr,                     /* a pointer to the label; can be empty string */
                             size_t PSize,                     /* the size of the label in bytes */
                             uint16_t emLen, /* The value is set before the call */
                             CCRsaPrimeData_t  *PrimeData_ptr,/* temp buffer */
                             uint8_t  *EMInput_ptr,              /* encoded message output */
                             CCPkcs1Version_t PKCS1_ver);

/**********************************************************************************************************/
/**
   @brief
   RsaPssOaepDecode implements the the De operation according to the PKCS#1 as defined
   in PKCS#1 v2.1 7.1.1 (2) and PKCS#1 v2.0
*/
CCError_t RsaPssOaepDecode(CCPkcs1HashFunc_t hashFunc,
                             CCPkcs1Mgf_t MGF,
                             uint8_t  *EM_ptr,
                             uint16_t EMSize,
                             uint8_t *P_ptr,
                             size_t  PSize,
                             CCRsaPrimeData_t  *PrimeData_ptr, /*Only for stack memory save*/
                             uint8_t *M_ptr,
                             size_t  *MSize_ptr);

/**********************************************************************************************************/
/**
 * @brief
 * This function does implements the functionality of PKCS1 Ver 2.1 Sign
 *  operation after the Hash operation
 *
 * Before using that function a Hash must be completed on the Data.
 * The function is called after the call to Hash_Finish
 *
 * @param[in/out] rndContext_ptr  - Pointer to the RND context buffer.
 * @param[in] Context_ptr - Pointer to a valid context as
 *                           given from CC_RsaSign
 *
 * @param[out] Output_ptr - A buffer allocated for the output which is at least the size of the MOdulus N
 *
 *
 * @return CCError_t - On success CC_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CCError_t RsaPssSign21(CCRndContext_t *rndContext_ptr,
                                RSAPrivContext_t *Context_ptr,
                                uint8_t     *Output_ptr);


/**********************************************************************************************************/
/**
    Function Name: RsaPssVerify21
    Date:   06-12-2004
    Author: Ohad Shperling


    \brief RsaPssVerify21 implements EMSA-PSS-Verify algorithm
   as defined in PKCS#1 v2.1 Sec 9.1.2

   @param[in] Context_ptr - Pointer to a valid context as
                            given from the VerifyFinish function.

   The field HASH_Result inside the Context_ptr is initialized with the Hashed digested message.
   The field HASH_Result_Size inside the Context_ptr is initialized with the Hash digested message size

   @return CCError_t - CC_OK,or error
*/
CCError_t RsaPssVerify21(RSAPubContext_t *Context_ptr);

/**********************************************************************************************************/
/**
 * @brief The CC_RsaGenerateVectorInRangeX931 function generates a random vector in range:
 *            MinVect < RandVect < MaxVect, where:
 *            MinVect = sqwRoot(2) * 2^(RndSizeInBits-1),  MaxVect = 2^RndSizeInBits.
 *
 *            Note: 1. MSBit of RandVect must be set to 1.
 *                  2. Words order of output vector is set from LS word to MS
 *                 word.
 *
 *        This function is used in PKI RSA for random generation according to ANS X9.31 standard.
 *        If PKI_RSA is not supported, the function does nothing.
 *
 *        Functions algorithm::
 *
 *        1.  Calls the CC_RndGenerateVector() function for generating random vector
 *            RndVect of size RndSizeInWords, rounded up to bytes. Set index i
 *            to high word i = SizeInWords-1.
 *        2.  Check and adust candidate for msWord inside the random vector
 *            starting from msWord himselv, if msWord > high word of MinVect,
 *            goto step 3, else try next word i--; if no words to try, then goto
 *            step 1.
 *        3.  Set the found msWord to high position in array and generate new
 *            random words instead all checked and rejected words.
 *
 * @rndContext_ptr[in/out]  - Pointer to the RND context buffer.
 * @rndSizeWords[in]  - The size of random vectore that is required.
 * @rnd_ptr[out]      - The output buffer of size not less, than rndSizeWords.
 *
 * @return CCError_t - On success CC_OK is returned, on failure a
 *                       value MODULE_* as defined in ...
 */
CCError_t CC_RsaGenerateVectorInRangeX931(CCRndContext_t *rndContext_ptr,
                                               uint32_t   rndSizeWords,
                                               uint32_t  *rnd_ptr);

/*****************************************************************************/
/**
 * The function generates vector of non zero octets.
 *
 * @author reuvenl (9/14/2014)
 *
 * @param [in/out] rndContext_ptr  - Pointer to the RND context buffer.
 * @param pVect - The pointer to output buffer.
 * @param size  - The size of vector in bytes.
 *
 * @return CCError_t
 */
CCError_t  RsaGenRndNonZeroVect(CCRndContext_t *rndContext_ptr, uint8_t *pVect, uint32_t size);


/**********************************************************************************************************/
/*!
@brief RSA_SignInit initializes the Signing multi-call algorithm as defined in PKCS#1 2.1 standard, including v1.5.

NOTE:
-# In PSS_Sign v2.1 MD5 is not supported, since it is not recommended by the PKCS#1 v2.1.
-# According to the said standard, implementation of the function for version v1.5 is based on DER encoding of the algorithm info.

This function does not do cryptographic processing. Rather, it
prepares a context that is used by the Update and Finish functions.

@return CC_OK on success.
@return error on failure
*/
CIMPORT_C CCError_t CC_RsaSignInit(
                            CCRsaPrivUserContext_t *UserContext_ptr,     /*!< [in/out] A pointer to a Context. The value returned here must be passed to the Update and Finish functions. */
                            CCRsaUserPrivKey_t *UserPrivKey_ptr,         /*!< [in]  A pointer to the private key data structure.
                                                                                       \note The representation (pair or quintuple) and hence the algorithm (CRT or not) is determined by the Private Key data structure.
                                                                                       Using of the CC_BuildPrivKey or CC_BuildPrivKeyCRT determines which algorithm is used. */
                            CCRsaHashOpMode_t rsaHashMode,             /*!< [in]  The enumerator value, defining the hash function to be used: SHA-1,SHA224/256/384/512, MD5 (MD5 allowed only in v1.5).
                                                                                        The hash functions recommended by PKCS#1 v2.1 are: 256/384/512. Also allowed "After" HASH modes for said functions. */
                            CCPkcs1Mgf_t MGF,                           /*!< [in]  The mask generation function. PKCS#1 v2.1 defines MGF1, so the only value allowed here is CC_PKCS1_MGF1. */
                            size_t SaltLen,                 /*!< [in]  The Length of the Salt buffer (relevant for PKCS#1 Ver 2.1 only, typically lengths is 0 or hLen). FIPS 186-4 requires,
                                                                                       that SaltLen <= hlen. If SaltLen > KeySize - hLen - 2, the function returns an error. */
                            CCPkcs1Version_t PKCS1_ver          /*!< [in]  Ver 1.5 or 2.1, according to the functionality required. */
);

/**********************************************************************************************************/
/*!
@brief CC_RsaSignUpdate processes the data to be signed in a given context.

@note CC_RsaSignUpdate can be called multiple times with data

@return CC_OK on success.
@return Error on failure
*/
CIMPORT_C CCError_t CC_RsaSignUpdate(
                                CCRsaPrivUserContext_t *UserContext_ptr,     /*!< [in] A pointer to a valid context, as returned by CC_RsaSignInit. */
                                uint8_t     *DataIn_ptr,                        /*!< [in] A pointer to the data to sign. */
                                size_t      DataInSize                         /*!< [in] The size, in bytes, of the data to sign. */
);

/**********************************************************************************************************/
/*!
@brief CC_RsaSignFinish calculates the signature on the data passed to one or more calls to CC_RsaSignUpdate,
and releases the context.

@return CC_OK on success.
@return Error on failure:\n
            CC_RSA_INVALID_USER_CONTEXT_POINTER_ERROR,\n
            CC_RSA_USER_CONTEXT_VALIDATION_TAG_ERROR,\n
            CC_RSA_INVALID_OUTPUT_POINTER_ERROR,\n
            CC_RSA_INVALID_SIGNATURE_BUFFER_SIZE,\n
            CC_RSA_HASH_ILLEGAL_OPERATION_MODE_ERROR,\n
            CC_RSA_PKCS1_VER_ARG_ERROR
*/

CIMPORT_C CCError_t CC_RsaSignFinish(
                                CCRndContext_t *rndContext_ptr,             /*!< [in/out] Pointer to the RND context buffer. */
                                CCRsaPrivUserContext_t *UserContext_ptr,     /*!< [in/out] A pointer to the Context initialized by the SignInit function and used by the SignUpdate function. */
                                uint8_t     *Output_ptr,                        /*!< [out] A pointer to the signature.
                                                                                            The buffer must be at least PrivKey_ptr->N.len bytes long (that is, the size of the modulus, in bytes). */
                                size_t      *OutputSize_ptr                     /*!< [in/out] A pointer to the Signature Size value -
                                                                                                the input value is the signature buffer size allocated, the output value is the signature size used.
                                                                                                The buffer must be at least PrivKey_ptr->N.len bytes long (that is, the size of the modulus, in bytes). */
);

/**********************************************************************************************************/
/*!
@brief  RSA_VerifyInit initializes the Verify multi-call algorithm as defined in PKCS#1 v1.5 and 2.1

note:
-# In PSS_Sign v2.1 MD5 is not supported, since it is not recommended by the PKCS#1 v2.1.
-# According to the said standard, implementation of the function for version v1.5 is based on DER encoding of the algorithm info.

@return CC_OK on success.
@return Error on failure
*/
CIMPORT_C CCError_t CC_RsaVerifyInit(
                            CCRsaPubUserContext_t *UserContext_ptr,      /*!< [in]  A pointer to the public Context structure of the User. */
                            CCRsaUserPubKey_t *UserPubKey_ptr,           /*!< [in]  A pointer to the public key data structure. */
                            CCRsaHashOpMode_t rsaHashMode,             /*!< [in]  The hash function to be used. Currently available HASH functions: SHA1/SHA-256/384/512/MD5 (MD5 - allowed only for PKCS#1 v1.5).
                                                                                        Also allowed "After HASH" modes for said functions. */
                            CCPkcs1Mgf_t MGF,                           /*!< [in]  The mask generation function, relevant only for PKCS#1 v2.1. The currently allowed value for v2.1 is CC_PKCS1_MGF1. */
                            size_t  SaltLen,                               /*!< [in]  The Length of the Salt buffer. Relevant for PKCS#1 Ver 2.1 only. Typical lengths are 0 and hashLen (20 for SHA1).
                                                                                        The maximum length allowed is NSize - hLen - 2.
                                                                                        If the salt length is not available in this process, the user can use the define: CC_RSA_VERIFY_SALT_LENGTH_UNKNOWN.
                                                                                        Security Note: This mode is not FIPS approved and it is recommended not to use this flag and provide the Salt length
                                                                                        on each verification. */
                            CCPkcs1Version_t PKCS1_ver                    /*!< [in]  Ver 1.5 or 2.1, according to the functionality required. */
);

/**********************************************************************************************************/
/*!
@brief RSA_VerifyUpdate processes the data to be verified in a given context, according to PKCS1 v1.5 and 2.1
@brief RSA_VerifyUpdate can be called multiple times with data

@return CC_OK on success.
@return Error on failure
*/

CIMPORT_C CCError_t CC_RsaVerifyUpdate(
                                CCRsaPubUserContext_t *UserContext_ptr,      /*!< [in]  A pointer to the public Context structure of the User. */
                                uint8_t     *DataIn_ptr,                        /*!< [in]  A pointer to the data whose signature is to be verified. */
                                size_t       DataInSize                         /*!< [in]  The size, in bytes, of the data whose signature is to be verified. */
);

/**********************************************************************************************************/
/*!
@brief RSA_VerifyFinish implements the Finish Phase of the Verify algorithm as defined in PKCS#1 v2.1 or PKCS#1 v1.5

@return CC_OK on success.
@return Error on failure
*/

CIMPORT_C CCError_t CC_RsaVerifyFinish(
                            CCRsaPubUserContext_t *UserContext_ptr,      /*!< [in]  A pointer to the public Context structure of the User. */
                            uint8_t *Sig_ptr                                /*!< [in]  A pointer to the signature to be verified.
                                                                                        The length of the signature is PubKey_ptr->N.len bytes (that is, the size of the modulus, in bytes). */
);


#ifdef __cplusplus
}
#endif

#endif

