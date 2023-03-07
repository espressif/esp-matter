/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_ECPKI_LOCAL_H
#define _CC_ECPKI_LOCAL_H

 /** @file
   *  @brief this file contains the prototype of the service functions for
   *         the CryptoCell ECPKI module that are intendet for internaly usage.  */


#include "cc_error.h"
#include "cc_ecpki_types.h"
#include "cc_rnd_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/

/*-------------------------------------------------*/
/*  User passed structures validation tags         */
/*-------------------------------------------------*/

/* the ECPKI user public key user validity TAG */
#define CC_ECPKI_PUBL_KEY_VALIDATION_TAG  0xEC000001
/* the ECPKI user private key user validity TAG */
#define CC_ECPKI_PRIV_KEY_VALIDATION_TAG 0xEC000002

/* the ECDSA signing user context validity TAG */
#define CC_ECDSA_SIGN_CONTEXT_VALIDATION_TAG   0xEC000003
/* the ECDSA verifying user context validity TAG */
#define CC_ECDSA_VERIFY_CONTEXT_VALIDATION_TAG 0xEC000004

typedef struct {
    uint16_t hashResultSize;
    CCHashOperationMode_t hashMode;
}CCEcpkiHash_t;




/************************ macros ********************************/

/************************ Typedefs  *****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions **********************/
/**************************************************************************
 *                EcdsaSignInit function
 **************************************************************************/
/*!
@brief
The EcdsaSignInit functions user shall call first to perform the EC DSA Signing operation.

   The function performs the following steps:
   -# Validates all the inputs of the function. If one of the received
      parameters is not valid, the function returns an error.
   -# Calls the CC_HashInit() function.
   -# Exits the handler with the OK code.

   This function does not do ECDSA cryptographic processing. Rather, it
   prepares a context that is used by the Update() and Finish() functions.

@note
Using of HASH functions with HASH size great, than EC modulus size, is not recommended!

@return CC_OK on success.
@return a non-zero value on failure as defined cc_ecpki_error.h.
*/
CIMPORT_C CCError_t EcdsaSignInit(
             CCEcdsaSignUserContext_t  *pSignUserContext,    /*!< [in/out] A pointer to the user buffer for signing data. */
             CCEcpkiUserPrivKey_t      *pSignerPrivKey,      /*!< [in]  A pointer to the private key that is used to sign the data. */
             CCEcpkiHashOpMode_t       hashMode             /*!< [in]  Defines the hash mode used for DSA. */
             );

/**************************************************************************
 *                EcdsaSignUpdate function
 **************************************************************************/
/*!
@brief Performs a hash  operation on data allocated by the user
before finally signing it.

In case user divides signing data by block, he must call the Update function
continuously a number of times until processing of the entire data block is complete.

@note
Using of HASH functions with HASH size great, than EC modulus size, is not recommended!

@return CC_OK on success.
@return a non-zero value on failure as defined cc_ecpki_error.h.
 */
CIMPORT_C CCError_t EcdsaSignUpdate(
               CCEcdsaSignUserContext_t    *pSignUserContext,  /*!< [in/out] The pointer to the user buffer for signing the database. */
               uint8_t                         *pMessageDataIn,    /*!< [in]  The pointer to the message data block for calculating the HASH. */
               size_t                          dataInSize         /*!< [in]  The size of the message data block, in bytes.
                                              The data size, passed on each call of the function, besides the last call,
                                              must be a multiple of the HASH block size according to used HASH mode. */
               );

/**************************************************************************
 *                _DX_ECDSA_Sign_Finish function
 **************************************************************************/
/*!
@brief Performs initialization of variables and structures, calls the hash function
for the last block of data (if necessary) and then calculates digital signature.
Algorithm according ANS X9.62 standard.

@note
Using of HASH functions with HASH size great, than EC modulus size, is not recommended!

@return CC_OK on success.
@return a non-zero value on failure as defined cc_ecpki_error.h.
**/
CIMPORT_C  CCError_t EcdsaSignFinishInt(
               CCEcdsaSignUserContext_t   *pSignUserContext,       /*!< [in]  A pointer to the user buffer for signing database. */
               CCRndContext_t             *pRndContext,            /*!< [in/out] A pointer to the random generation function context. */
               uint8_t                        *pSignOut,               /*!< [out] A pointer to a buffer for output of signature. */
               size_t                         *pSignOutSize,           /*!< [in/out] A pointer to the size of a user passed buffer for
                                                 signature (in), be not less than 2*orderSizeInBytes. */
               uint32_t                        isEphemerKeyInternal,   /*!< [in] A parameter defining whether the ephemeral key is
                                                 internal or external (1 or 0). */
               uint32_t                       *pEphemerKeyData         /*!< [in] A pointer to external ephemeral key data.
                                           If it is given (case isEphemerKeyInternal=0), then the buffer
                                           must containing the ephemeral private key of size equal to
                                           EC generator order size, where LS-word is left most and MS-word
                                           is right most one. */
               );

/**************************************************************************
 *                EcdsaSignFinish function
 **************************************************************************/
/*!
@brief The macro definition for calling the ::EcdsaSignFinishInt function with internal generation of ephemeral keys.

@note
Using of HASH functions with HASH size great, than EC modulus size, is not recommended!

The macro calls the function with the following arguments as constant:
isEphemerKeyInternal = 1 and pEphemerKeyData = NULL.
*/
#define EcdsaSignFinish(pSignUserContext, pRndContext, pSignatureOut, pSignatureOutSize) \
EcdsaSignFinishInt((pSignUserContext), (pRndContext), (pSignatureOut), (pSignatureOutSize), 1, NULL)

/**************************************************************************
 *                EcdsaVerifyInit  function
 **************************************************************************/
/*!
@brief Prepares a context that is used by the Update and Finish functions
but does not perform elliptic curve cryptographic processing

The function:
- Receives and decrypts user data (working context).
- Checks input parameters of  ECDSA Verifying primitive.
- Calls hash init function.
- Initializes variables and structures for calling next functions.
- Encrypts and releases working context.

@note
Using of HASH functions with HASH size great, than EC modulus size, is not recommended!

@return CC_OK on success.
@return a non-zero value on failure as defined cc_ecpki_error.h.
*/
CIMPORT_C CCError_t EcdsaVerifyInit(
                       CCEcdsaVerifyUserContext_t  *pVerifyUserContext,    /*!< [in/out] A pointer to the user buffer for verifying database. */
                       CCEcpkiUserPublKey_t        *pSignerPublKey,        /*!< [in]  A pointer to a Signer public key structure. */
                       CCEcpkiHashOpMode_t         hashMode               /*!< [in]  The enumerator variable defines the hash function to be used. */
                       );


/**************************************************************************
 *                EcdsaVerifyUpdate function
 **************************************************************************/
/*!
@brief Performs a hash  operation on data allocated by the user
before finally verifying its signature.

In case user divides signing data by block, he must call the Update function
continuously a number of times until processing of the entire data block is complete.

@note
Using of HASH functions with HASH size greater, than EC modulus size, is not recommended.

@return CC_OK on success.
@return a non-zero value on failure as defined cc_ecpki_error.h.
*/
CIMPORT_C CCError_t EcdsaVerifyUpdate(
                         CCEcdsaVerifyUserContext_t *pVerifyUserContext, /*!< [in/out] The pointer to the user buffer for verifying database. */
                         uint8_t                        *pMessageDataIn,     /*!< [in]  The message data for calculating Hash. */
                         size_t                         dataInSize          /*!< [in]  The size of the message data block, in bytes.
                                                         The data size, passed on each call of the function, besides the last call,
                                                         must be a multiple of the HASH block size according to used HASH mode. */
                         );


/**************************************************************************
 *                EcdsaVerifyFinish function
 **************************************************************************/

/*!
@brief Performs initialization of variables and structures,
calls the hash function for the last block of data (if necessary),
than calls EcWrstDsaVerify function for verifying signature according to
ANS X9.62 standard.

@note
Using of HASH functions with HASH size greater, than EC modulus size, is not recommended!

@return CC_OK on success.
@return a non-zero value on failure as defined cc_ecpki_error.h.
**/
CIMPORT_C CCError_t EcdsaVerifyFinish(
                         CCEcdsaVerifyUserContext_t  *pVerifyUserContext,    /*!< [in]  A pointer to the user buffer for verifying the database. */
                         uint8_t                         *pSignatureIn,          /*!< [in]  A pointer to a buffer for the signature to be compared. */
                         size_t                          SignatureSizeBytes /*!< [in]  The size of a user passed signature (must be 2*orderSizeInBytes). */
                         );





#ifdef __cplusplus
}
#endif

#endif /* #ifndef _CC_ECPKI_LOCAL_H */

