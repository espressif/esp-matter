/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_ECPKI_BUILD_H
#define _CC_ECPKI_BUILD_H

/*!
@file
@brief This file defines functions for building key structures used in Elliptic Curves Cryptography (ECC).
@defgroup cryptocell_ecpki CryptoCell ECC APIs
@{
@ingroup cryptocell_api

*/


#include "cc_error.h"
#include "cc_ecpki_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**********************************************************************************
 *                    CC_EcpkiPrivKeyBuild function                            *
 **********************************************************************************/
/*!
@brief Builds (imports) the user private key structure from an existing private key so
that this structure can be used by other EC primitives.
This function should be called before using of the private key. Input
domain structure must be initialized by EC parameters and auxiliary
values, using CC_EcpkiGetDomain or CC_EcpkiSetDomain functions.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_ecpki_error.h.
*/
CIMPORT_C CCError_t CC_EcpkiPrivKeyBuild(
                     const CCEcpkiDomain_t *pDomain,            /*!< [in] The EC domain (curve). */
                     const uint8_t         *pPrivKeyIn,         /*!< [in] Pointer to private key data. */
                     size_t                 PrivKeySizeInBytes, /*!< [in] Size of private key data (in bytes). */
                     CCEcpkiUserPrivKey_t  *pUserPrivKey        /*!< [out] Pointer to the private key structure.
                                                   This structure is used as input to the ECPKI cryptographic primitives. */
                     );

/**********************************************************************************
 *                CC_EcpkiPublKeyBuildAndCheck function                             *
 **********************************************************************************/
/*!
@brief Builds a user public key structure from an imported public key,
so it can be used by other EC primitives.
When operating the EC cryptographic algorithms with imported EC public
key, this function should be called before using of the public key.

\note The Incoming public key PublKeyIn structure is big endian bytes array, containing
concatenation of PC||X||Y. \par
\note PC - point control single byte, defining the type of point: 0x4 - uncompressed,
06,07 - hybrid, 2,3 - compressed. \par
\note X,Y - EC point coordinates of public key (y is omitted in compressed form),
size of X and Y must be equal to size of EC modulus.

The user may call this function by appropriate macros, according to the necessary validation level in section SEC1. ECC standard: 3.2 of Standards for
Efficient Cryptography Group (SECG): SEC1 Elliptic Curve Cryptography and ANSI X9.62-2005: Public Key Cryptography for the Financial Services Industry,
The Elliptic Curve Digital Signature Algorithm (ECDSA):
<ul><li>Checking the input pointers and sizes only - ::CC_EcpkiPubKeyBuild.</li>
<li>Partially checking of public key - ::CC_EcpkiPubKeyBuildAndPartlyCheck. </li>
<li>Full checking of public key - ::CC_EcpkiPubKeyBuildAndFullCheck. </li></ul>

\note Full check mode takes long time and should be used only when it is actually needed.

@return CC_OK on success.
@return A non-zero value on failure as defined cc_ecpki_error.h.
*/
/*
The function performs the following operations:
- Checks validity of incoming variables and pointers;
- Converts incoming key data from big endian into little endian;
- If public key is given in compressed form (i.e. byte[0] = 2 or 3 and
  coordinate Y is omitted), then the function uncompress it;
- Performs checking of input key according to CheckMode parameter.
- Initializes variables and structures.
*/
CIMPORT_C CCError_t CC_EcpkiPublKeyBuildAndCheck(
            const CCEcpkiDomain_t       *pDomain,               /*!< [in]  The EC domain (curve). */
            uint8_t                     *pPubKeyIn,         /*!< [in]  Pointer to the input public key data, in compressed or
                                           uncompressed or hybrid form:
                                           [PC||X||Y] Big-Endian representation, structured according to
                                           [IEEE1363], where:
                                           <ul><li>X and Y are the public key's EC point coordinates.
                                           In compressed form, Y is omitted.</li>
                                           <li> The sizes of X and Y are equal to the size of the EC modulus.</li>
                                           <li> PC is a one-byte point control that defines the type of point
                                           compression. </li></ul>*/
            size_t                      PublKeySizeInBytes,    /*!< [in]  The size of public key data (in bytes). */
            ECPublKeyCheckMode_t        CheckMode,             /*!< [in]  The required level of public key verification
                                    (higher verification level means longer verification time):
                                    <ul><li> 0 = preliminary validation. </li>
                                    <li> 1 = partial validation. </li>
                                    <li> 2 = full validation. </li></ul>*/
            CCEcpkiUserPublKey_t        *pUserPublKey,          /*!< [out] Pointer to the output public key structure.
                                        This structure is used as input to the ECPKI cryptographic primitives. */
            CCEcpkiBuildTempData_t      *pTempBuff              /*!< [in]  Pointer for a temporary buffer required for the build function. */
            );


/**********************************************************************************
 *                 CC_EcpkiPubKeyBuild macro                              *
 **********************************************************************************/
/*!
@brief This macro calls CC_EcpkiPublKeyBuildAndCheck function for building the public key
while checking input pointers and sizes. For a description of the parameters see ::CC_EcpkiPublKeyBuildAndCheck.
*/
#define  CC_EcpkiPubKeyBuild(pDomain, pPubKeyIn, PublKeySizeInBytes, pUserPublKey) \
         CC_EcpkiPublKeyBuildAndCheck((pDomain), (pPubKeyIn), (PublKeySizeInBytes), CheckPointersAndSizesOnly, (pUserPublKey), NULL)


/**********************************************************************************
 *                 CC_EcpkiPubKeyBuildAndPartlyCheck macro                         *
 **********************************************************************************/
/*!
@brief This macro calls CC_EcpkiPublKeyBuildAndCheck function for building the public key with partial validation of the key [SEC1] - 3.2.3.
For a description of the parameters see ::CC_EcpkiPublKeyBuildAndCheck.
*/
#define  CC_EcpkiPubKeyBuildAndPartlyCheck(pDomain, pPubKeyIn, PublKeySizeInBytes, pUserPublKey, pTempBuff) \
         CC_EcpkiPublKeyBuildAndCheck((pDomain), (pPubKeyIn), (PublKeySizeInBytes), ECpublKeyPartlyCheck, (pUserPublKey), (pTempBuff))


/**********************************************************************************
 *                 CC_EcpkiPubKeyBuildAndFullCheck macro                     *
 **********************************************************************************/
/*!
@brief This macro calls CC_EcpkiPublKeyBuildAndCheck function for building the public key with full validation of the key [SEC1] - 3.2.2.
For a description of the parameters and return values see CC_EcpkiPublKeyBuildAndCheck.
*/
#define  CC_EcpkiPubKeyBuildAndFullCheck(pDomain, pPubKeyIn, PublKeySizeInBytes, pUserPublKey,  pTempBuff) \
     CC_EcpkiPublKeyBuildAndCheck((pDomain), (pPubKeyIn), (PublKeySizeInBytes), (ECpublKeyFullCheck), (pUserPublKey),  (pTempBuff))


/***********************************************************************************
 *                     CC_EcpkiPubKeyExport function                           *
 ***********************************************************************************/
/*!
@brief Converts an existing public key from internal representation to Big-Endian export representation.
The function converts the X,Y coordinates of public key EC point to big endianness,
and sets the public key as follows:
<ul><li>In case "Uncompressed" point:  PubKey = PC||X||Y, PC = 0x4 - single byte;</li>
<li>In case of "Hybrid" key PC = 0x6.</li>
<li>In case of "Compressed" key PC = 0x2.</li></ul>
\note Size of output X and Y coordinates is equal to ModSizeInBytes.
@return CC_OK on success.
@return A non-zero value on failure as defined cc_ecpki_error.h.
*/
CIMPORT_C CCError_t CC_EcpkiPubKeyExport(
                  CCEcpkiUserPublKey_t           *pUserPublKey,        /*!< [in]  Pointer to the input public key structure (in Little-Endian form). */
                  CCEcpkiPointCompression_t      compression,         /*!< [in]  Compression mode: Compressed, Uncompressed or Hybrid. */
                  uint8_t                        *pExternPublKey,      /*!< [out] Pointer to the exported public key array, in compressed or uncompressed
                                               or hybrid form:
                                            [PC||X||Y] Big-Endian representation, structured according to [IEEE1363].
                                            In compressed form, Y is omitted. */
                  size_t                         *pPublKeySizeBytes    /*!< [in/out] Pointer used for the input of the user public key buffer size
                                               (in bytes), and the output of the size of the converted public key in bytes. */
                  );



#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif
