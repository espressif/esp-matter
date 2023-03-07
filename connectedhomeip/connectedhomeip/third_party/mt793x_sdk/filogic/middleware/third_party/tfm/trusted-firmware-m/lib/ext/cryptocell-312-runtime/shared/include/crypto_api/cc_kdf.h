/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_KDF_H
#define _CC_KDF_H



#ifdef __cplusplus
extern "C"
{
#endif

/*!
@file
@brief This file defines the API that supports Key derivation function in modes
       as defined in Public-Key Cryptography Standards (PKCS) #3: Diffie-Hellman Key Agreement Standard,
       ANSI X9.42-2003: Public Key Cryptography for the Financial Services Industry: Agreement of Symmetric Keys Using Discrete Logarithm Cryptography,
       and ANSI X9.63-2011: Public Key Cryptography for the Financial Services Industry - Key Agreement and Key Transport Using Elliptic Curve
       Cryptography.
@defgroup cc_kdf CryptoCell Key Derivation APIs
@{
@ingroup cryptocell_api

*/

#include "cc_hash_defs.h"

/************************ Defines ******************************/

/*! Shared secret value max size in bytes */
#define  CC_KDF_MAX_SIZE_OF_SHARED_SECRET_VALUE  1024

/* Count and max. sizeof OtherInfo entries (pointers to data buffers) */
/*! Number of other info entries. */
#define  CC_KDF_COUNT_OF_OTHER_INFO_ENTRIES   5

/*! Maximal size of keying data in bytes. */
#define  CC_KDF_MAX_SIZE_OF_KEYING_DATA  2048
/*! Size of KDF counter in bytes */
#define CC_KDF_COUNTER_SIZE_IN_BYTES  4

/************************ Enums ********************************/

/*! HASH operation modes */
typedef enum
{
    /*! SHA1 mode.*/
    CC_KDF_HASH_SHA1_mode    = 0,
    /*! SHA224 mode.*/
    CC_KDF_HASH_SHA224_mode  = 1,
    /*! SHA256 mode.*/
    CC_KDF_HASH_SHA256_mode  = 2,
    /*! SHA384 mode.*/
    CC_KDF_HASH_SHA384_mode  = 3,
    /*! SHA512 mode.*/
    CC_KDF_HASH_SHA512_mode  = 4,
    /*! Maximal number of HASH modes. */
    CC_KDF_HASH_NumOfModes,
    /*! Reserved.*/
    CC_KDF_HASH_OpModeLast = 0x7FFFFFFF,

}CCKdfHashOpMode_t;

/*! Key derivation modes. */
typedef enum
{
    /*! ASN1 key derivation mode.*/
    CC_KDF_ASN1_DerivMode    = 0,
    /*! Concatination key derivation mode.*/
    CC_KDF_ConcatDerivMode   = 1,
    /*! X963 key derivation mode.*/
    CC_KDF_X963_DerivMode    = CC_KDF_ConcatDerivMode,
    /*! ISO 18033 KDF1 key derivation mode.*/
    CC_KDF_ISO18033_KDF1_DerivMode = 3,
    /*! ISO 18033 KDF2 key derivation mode.*/
    CC_KDF_ISO18033_KDF2_DerivMode = 4,
    /*! Maximal number of key derivation modes. */
    CC_KDF_DerivFunc_NumOfModes = 5,
    /*! Reserved.*/
    CC_KDF_DerivFuncModeLast= 0x7FFFFFFF,

}CCKdfDerivFuncMode_t;

/*! Enumerator for the additional information given to the KDF. */
typedef enum
{
    CC_KDF_ALGORITHM_ID     = 0, /*! An identifier (OID), indicating algorithm for which the keying data is used. */
    CC_KDF_PARTY_U_INFO     = 1, /*! Optional data of party U .*/
    CC_KDF_PARTY_V_INFO     = 2, /*! Optional data of party V. */
    CC_KDF_SUPP_PRIV_INFO   = 3, /*! Optional supplied private shared data. */
    CC_KDF_SUPP_PUB_INFO    = 4, /*! Optional supplied public shared data. */

    CC_KDF_MAX_COUNT_OF_ENTRIES,  /*! Maximal allowed number of entries in Other Info structure. */
    /*! Reserved.*/
    CC_KDF_ENTRYS_MAX_VAL  = 0x7FFFFFFF,

}CCKdfOtherInfoEntries_t;
/************************ Typedefs  ****************************/

/*! KDF structure, containing pointers to OtherInfo data entries and sizes.

   The structure contains two arrays: one for data pointers and one for sizes, placed according
   to the order given in the the ANSI X9.42-2003: Public Key Cryptography for the Financial Services
   Industry: Agreement of Symmetric Keys Using Discrete Logarithm Cryptography standard
   and defined in CCKdfOtherInfoEntries_t enumerator.
   On KDF ASN1 mode this order is mandatory. On other KDF modes the user may insert
   optional OtherInfo simply in one (preferably the first) or in some entries.
   If any data entry is not used, then the pointer value and the size must be set to NULL. */
typedef struct
{
        /*! Pointers to data entries. */
        uint8_t  *dataPointers[CC_KDF_MAX_COUNT_OF_ENTRIES];
        /*! Sizes of data entries. */
        uint32_t  dataSizes[CC_KDF_MAX_COUNT_OF_ENTRIES];
}CCKdfOtherInfo_t;


/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions **********************/

/****************************************************************/

/*********************************************************************************************************/
/*!
 @brief CC_KdfKeyDerivFunc performs key derivation according to one of the modes defined in standards:
    ANSI X9.42-2003: Public Key Cryptography for the Financial Services Industry: Agreement of Symmetric Keys Using Discrete Logarithm Cryptography,
    ANSI X9.63-2011: Public Key Cryptography for the Financial Services Industry - Key Agreement and Key Transport Using Elliptic Curve Cryptography,
    ISO/IEC 18033-2:2006: Information technology -- Security techniques -- Encryption algorithms -- Part 2: Asymmetric ciphers.

The present implementation of the function allows the following operation modes:
<ul><li> CC_KDF_ASN1_DerivMode - mode based on  ASN.1 DER encoding; </li>
<li> CC_KDF_ConcatDerivMode - mode based on concatenation;</li>
<li> CC_KDF_X963_DerivMode = CC_KDF_ConcatDerivMode;</li>
<li> CC_KDF_ISO18033_KDF1_DerivMode, CC_KDF_ISO18033_KDF2_DerivMode - specific modes according to
ISO/IEC 18033-2 standard.</li></ul>

The purpose of this function is to derive a keying data from the shared secret value and some
other optional shared information, included in OtherInfo (SharedInfo).

\note All buffers arguments are represented in Big-Endian format.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_kdf_error.h or cc_hash_error.h.
*/
CCError_t  CC_KdfKeyDerivFunc(
                    uint8_t              *pZzSecret,            /*!< [in]  A pointer to shared secret value octet string. */
                    size_t                zzSecretSize,         /*!< [in]  The size of the shared secret value in bytes.
                                                                           The maximal size is defined as: ::CC_KDF_MAX_SIZE_OF_SHARED_SECRET_VALUE. */
                    CCKdfOtherInfo_t     *pOtherInfo,           /*!< [in]  A pointer to the structure, containing pointers to the data, shared by
                                       two entities of agreement, depending on KDF mode:
                                                                           <ul><li> In KDF ASN1 mode OtherInfo includes ASN1 DER encoding of AlgorithmID (mandatory),
                                                                             and some optional data entries as described in section 7.7.1 of the ANSI X9.42-2003:
                                         Public Key Cryptography for the Financial Services Industry: Agreement of Symmetric Keys Using
                                         Discrete Logarithm Cryptography standard.</li>
                                                                           <li> In both ISO/IEC 18033-2:2006: Information technology -- Security techniques -- Encryption algorithms -- Part 2:
                                        Asymmetric ciphers standard: KDF1 and KDF2 modes this parameter is ignored and may be set to NULL. </li>
                                                                           <li> In other modes it is optional and may be set to NULL. </li></ul>*/
                    CCKdfHashOpMode_t     kdfHashMode,          /*!< [in]  The KDF identifier of hash function to be used. The hash function output
                                       must be at least 160 bits. */
                    CCKdfDerivFuncMode_t  derivMode,            /*!< [in]  The enum value, specifies one of above described derivation modes. */
                    uint8_t              *pKeyingData,          /*!< [out] A pointer to the buffer for derived keying data. */
                    size_t                keyingDataSize        /*!< [in]  The size in bytes of the keying data to be derived.
                                                                           The maximal size is defined as :: CC_KDF_MAX_SIZE_OF_KEYING_DATA. */ );

/*********************************************************************************************************/
/*!
 CC_KdfAsn1KeyDerivFunc is a macro that performs key derivation according to ASN1 DER encoding method defined
 in section 7.2.1 of ANSI X9.42-2003: Public Key Cryptography for the Financial Services Industry: Agreement of Symmetric Keys Using Discrete Logarithm Cryptography standard.
 For a description of the parameters see ::CC_KdfKeyDerivFunc.
*/
#define CC_KdfAsn1KeyDerivFunc(ZZSecret_ptr,ZZSecretSize,OtherInfo_ptr,kdfHashMode,KeyingData_ptr,KeyLenInBytes)\
        CC_KdfKeyDerivFunc((ZZSecret_ptr),(ZZSecretSize),(OtherInfo_ptr),(kdfHashMode),CC_KDF_ASN1_DerivMode,(KeyingData_ptr),(KeyLenInBytes))


/*********************************************************************************************************/
/*!
 CC_KdfConcatKeyDerivFunc is a macro that performs key derivation according to concatenation mode defined
 in section 7.2.2 of ANSI X9.42-2003: Public Key Cryptography for the Financial Services Industry: Agreement of Symmetric Keys Using Discrete Logarithm Cryptography
 standard and also meets ANSI X9.63-2011: Public Key Cryptography for the Financial Services Industry - Key Agreement and Key Transport Using Elliptic Curve
 Cryptography standard. For a description of the parameters see ::CC_KdfKeyDerivFunc.
*/
#define CC_KdfConcatKeyDerivFunc(ZZSecret_ptr,ZZSecretSize,OtherInfo_ptr,kdfHashMode,KeyingData_ptr,KeyLenInBytes)\
        CC_KdfKeyDerivFunc((ZZSecret_ptr),(ZZSecretSize),(OtherInfo_ptr),(kdfHashMode),CC_KDF_ConcatDerivMode,(KeyingData_ptr),(KeyLenInBytes))


#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif

