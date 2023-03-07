/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_ECPKI_ERROR_H
#define _CC_ECPKI_ERROR_H


/*!
@file
@brief This file contains the definitions of the CryptoCell ECPKI errors.
@defgroup cc_ecpki_error CryptoCell ECC specific errors
@{
@ingroup cryptocell_ecpki

*/

#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/* CC_ECPKI_MODULE_ERROR_BASE = 0x00F00800  */

/*********************************************************************************************
 * CryptoCell ECPKI MODULE ERRORS                                                                  *
 *********************************************************************************************/
/*! Illegal domain ID. */
#define CC_ECPKI_ILLEGAL_DOMAIN_ID_ERROR                    (CC_ECPKI_MODULE_ERROR_BASE + 0x1UL)
/*! Illegal domain pointer. */
#define CC_ECPKI_DOMAIN_PTR_ERROR               (CC_ECPKI_MODULE_ERROR_BASE + 0x2UL)
/* The CryptoCell ECPKI GEN KEY PAIR module errors */
/*! Illegal private key pointer. */
#define CC_ECPKI_GEN_KEY_INVALID_PRIVATE_KEY_PTR_ERROR          (CC_ECPKI_MODULE_ERROR_BASE + 0x3UL)
/*! Illegal public key pointer. */
#define CC_ECPKI_GEN_KEY_INVALID_PUBLIC_KEY_PTR_ERROR           (CC_ECPKI_MODULE_ERROR_BASE + 0x4UL)
/*! Illegal temporary buffer pointer. */
#define CC_ECPKI_GEN_KEY_INVALID_TEMP_DATA_PTR_ERROR            (CC_ECPKI_MODULE_ERROR_BASE + 0x5UL)
/*! Illegal RND context pointer. */
#define CC_ECPKI_RND_CONTEXT_PTR_ERROR              (CC_ECPKI_MODULE_ERROR_BASE + 0x6UL)

/************************************************************************************************************
* The CryptoCell ECPKI BUILD KEYS MODULE ERRORS                                                                   *
*************************************************************************************************************/
/*! Illegal compression mode. */
#define CC_ECPKI_BUILD_KEY_INVALID_COMPRESSION_MODE_ERROR     (CC_ECPKI_MODULE_ERROR_BASE + 0x07UL)
/*! Illegal domain ID. */
#define CC_ECPKI_BUILD_KEY_ILLEGAL_DOMAIN_ID_ERROR            (CC_ECPKI_MODULE_ERROR_BASE + 0x08UL)
/*! Illegal private key pointer. */
#define CC_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_IN_PTR_ERROR      (CC_ECPKI_MODULE_ERROR_BASE + 0x09UL)
/*! Illegal private key structure pointer. */
#define CC_ECPKI_BUILD_KEY_INVALID_USER_PRIV_KEY_PTR_ERROR    (CC_ECPKI_MODULE_ERROR_BASE + 0x0AUL)
/*! Illegal private key size. */
#define CC_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_SIZE_ERROR        (CC_ECPKI_MODULE_ERROR_BASE + 0x0BUL)
/*! Illegal private key data. */
#define CC_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_DATA_ERROR        (CC_ECPKI_MODULE_ERROR_BASE + 0x0CUL)
/*! Illegal public key pointer. */
#define CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_IN_PTR_ERROR      (CC_ECPKI_MODULE_ERROR_BASE + 0x0DUL)
/*! Illegal public key structure pointer. */
#define CC_ECPKI_BUILD_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR    (CC_ECPKI_MODULE_ERROR_BASE + 0x0EUL)
/*! Illegal public key size. */
#define CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_SIZE_ERROR        (CC_ECPKI_MODULE_ERROR_BASE + 0x0FUL)
/*! Illegal public key data. */
#define CC_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_DATA_ERROR        (CC_ECPKI_MODULE_ERROR_BASE + 0x10UL)
/*! Illegal EC build check mode option. */
#define CC_ECPKI_BUILD_KEY_INVALID_CHECK_MODE_ERROR           (CC_ECPKI_MODULE_ERROR_BASE + 0x11UL)
/*! Illegal temporary buffer pointer. */
#define CC_ECPKI_BUILD_KEY_INVALID_TEMP_BUFF_PTR_ERROR        (CC_ECPKI_MODULE_ERROR_BASE + 0x12UL)



/* The CryptoCell ECPKI EXPORT PUBLIC KEY MODULE ERRORS */
/*! Illegal public key structure pointer. */
#define CC_ECPKI_EXPORT_PUBL_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR      (CC_ECPKI_MODULE_ERROR_BASE + 0x14UL)
/*! Illegal public key compression mode. */
#define CC_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_COMPRESSION_MODE_ERROR       (CC_ECPKI_MODULE_ERROR_BASE + 0x15UL)
/*! Illegal output public key pointer. */
#define CC_ECPKI_EXPORT_PUBL_KEY_INVALID_EXTERN_PUBL_KEY_PTR_ERROR    (CC_ECPKI_MODULE_ERROR_BASE + 0x16UL)
/*! Illegal output public key size pointer. */
#define CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_PTR_ERROR      (CC_ECPKI_MODULE_ERROR_BASE + 0x17UL)
/*! Illegal output public key size. */
#define CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_ERROR          (CC_ECPKI_MODULE_ERROR_BASE + 0x18UL)
/*! Illegal domain ID. */
#define CC_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_DOMAIN_ID_ERROR              (CC_ECPKI_MODULE_ERROR_BASE + 0x19UL)
/*! Validation of public key failed. */
#define CC_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_VALIDATION_TAG_ERROR         (CC_ECPKI_MODULE_ERROR_BASE + 0x1AUL)
/*! Validation of public key failed. */
#define CC_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_DATA_ERROR          (CC_ECPKI_MODULE_ERROR_BASE + 0x1BUL)

/* The CryptoCell ECPKI BUILD ECC DOMAIN ERRORS */
/*! Illegal domain ID. */
#define CC_ECPKI_BUILD_DOMAIN_ID_IS_NOT_VALID_ERROR               (CC_ECPKI_MODULE_ERROR_BASE + 0x20UL)
/*! Illegal domain ID pointer. */
#define CC_ECPKI_BUILD_DOMAIN_DOMAIN_PTR_ERROR                        (CC_ECPKI_MODULE_ERROR_BASE + 0x21UL)
/*! Illegal domain parameter pointer. */
#define CC_ECPKI_BUILD_DOMAIN_EC_PARAMETR_PTR_ERROR                   (CC_ECPKI_MODULE_ERROR_BASE + 0x22UL)
/*! Illegal domain parameter size. */
#define CC_ECPKI_BUILD_DOMAIN_EC_PARAMETR_SIZE_ERROR                  (CC_ECPKI_MODULE_ERROR_BASE + 0x23UL)
/*! Illegal domain cofactor parameters. */
#define CC_ECPKI_BUILD_DOMAIN_COFACTOR_PARAMS_ERROR                   (CC_ECPKI_MODULE_ERROR_BASE + 0x24UL)
/*! Insufficient strength. */
#define CC_ECPKI_BUILD_DOMAIN_SECURITY_STRENGTH_ERROR                 (CC_ECPKI_MODULE_ERROR_BASE + 0x25UL)
/*! SCA resistance error. */
#define CC_ECPKI_BUILD_SCA_RESIST_ILLEGAL_MODE_ERROR                  (CC_ECPKI_MODULE_ERROR_BASE + 0x26UL)


/*! Internal error */
#define CC_ECPKI_INTERNAL_ERROR                                       (CC_ECPKI_MODULE_ERROR_BASE + 0x30UL)
/************************************************************************************************************
 * CryptoCell EC DIFFIE-HELLMAN MODULE ERRORS
*************************************************************************************************************/
/* The CryptoCell EC SVDP_DH Function errors */
/*! Illegal partner's public key pointer. */
#define CC_ECDH_SVDP_DH_INVALID_PARTNER_PUBL_KEY_PTR_ERROR                (CC_ECPKI_MODULE_ERROR_BASE + 0x31UL)
/*! Partner's public key validation failed. */
#define CC_ECDH_SVDP_DH_PARTNER_PUBL_KEY_VALID_TAG_ERROR                  (CC_ECPKI_MODULE_ERROR_BASE + 0x32UL)
/*! Illegal user private key pointer. */
#define CC_ECDH_SVDP_DH_INVALID_USER_PRIV_KEY_PTR_ERROR                   (CC_ECPKI_MODULE_ERROR_BASE + 0x33UL)
/*! Private key validation failed. */
#define CC_ECDH_SVDP_DH_USER_PRIV_KEY_VALID_TAG_ERROR                     (CC_ECPKI_MODULE_ERROR_BASE + 0x34UL)
/*! Illegal shared secret pointer. */
#define CC_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_PTR_ERROR                 (CC_ECPKI_MODULE_ERROR_BASE + 0x35UL)
/*! Illegal temporary buffer pointer. */
#define CC_ECDH_SVDP_DH_INVALID_TEMP_DATA_PTR_ERROR                           (CC_ECPKI_MODULE_ERROR_BASE + 0x36UL)
/*! Illegal shared secret size pointer. */
#define CC_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_PTR_ERROR            (CC_ECPKI_MODULE_ERROR_BASE + 0x37UL)
/*! Illegal shared secret size. */
#define CC_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_ERROR                (CC_ECPKI_MODULE_ERROR_BASE + 0x38UL)
/*! Illegal domain ID. */
#define CC_ECDH_SVDP_DH_ILLEGAL_DOMAIN_ID_ERROR                               (CC_ECPKI_MODULE_ERROR_BASE + 0x39UL)
/*! Illegal private and public domain ID are different. */
#define CC_ECDH_SVDP_DH_NOT_CONCENT_PUBL_AND_PRIV_DOMAIN_ID_ERROR             (CC_ECPKI_MODULE_ERROR_BASE + 0x3AUL)


/************************************************************************************************************
 * CryptoCell ECDSA  MODULE ERRORS
 ************************************************************************************************************/
/* The CryptoCell ECDSA Signing  errors */
/*! Illegal domain ID. */
#define CC_ECDSA_SIGN_INVALID_DOMAIN_ID_ERROR           (CC_ECPKI_MODULE_ERROR_BASE + 0x50UL)
/*! Illegal context pointer. */
#define CC_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR            (CC_ECPKI_MODULE_ERROR_BASE + 0x51UL)
/*! Illegal private key pointer. */
#define CC_ECDSA_SIGN_INVALID_USER_PRIV_KEY_PTR_ERROR           (CC_ECPKI_MODULE_ERROR_BASE + 0x52UL)
/*! Illegal hash operation mode. */
#define CC_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR                (CC_ECPKI_MODULE_ERROR_BASE + 0x53UL)
/*! Illegal data in pointer. */
#define CC_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_PTR_ERROR         (CC_ECPKI_MODULE_ERROR_BASE + 0x54UL)
/*! Illegal data in size. */
#define CC_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR        (CC_ECPKI_MODULE_ERROR_BASE + 0x55UL)
/*! Context validation failed. */
#define CC_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR         (CC_ECPKI_MODULE_ERROR_BASE + 0x57UL)
/*! User's private key validation failed. */
#define CC_ECDSA_SIGN_USER_PRIV_KEY_VALIDATION_TAG_ERROR        (CC_ECPKI_MODULE_ERROR_BASE + 0x58UL)
/*! Illegal signature pointer. */
#define CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_PTR_ERROR           (CC_ECPKI_MODULE_ERROR_BASE + 0x60UL)
/*! Illegal signature size pointer. */
#define CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_PTR_ERROR      (CC_ECPKI_MODULE_ERROR_BASE + 0x61UL)
/*! Illegal signature size. */
#define CC_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_ERROR          (CC_ECPKI_MODULE_ERROR_BASE + 0x62UL)
/*! Ephemeral key error. */
#define CC_ECDSA_SIGN_INVALID_IS_EPHEMER_KEY_INTERNAL_ERROR     (CC_ECPKI_MODULE_ERROR_BASE + 0x63UL)
/*! Illegal ephemeral key pointer. */
#define CC_ECDSA_SIGN_INVALID_EPHEMERAL_KEY_PTR_ERROR           (CC_ECPKI_MODULE_ERROR_BASE + 0x64UL)
/*! Illegal RND context pointer. */
#define CC_ECDSA_SIGN_INVALID_RND_CONTEXT_PTR_ERROR             (CC_ECPKI_MODULE_ERROR_BASE + 0x65UL)
/*! Illegal RND function pointer. */
#define CC_ECDSA_SIGN_INVALID_RND_FUNCTION_PTR_ERROR            (CC_ECPKI_MODULE_ERROR_BASE + 0x66UL)
/*! Signature calculation failed. */
#define CC_ECDSA_SIGN_SIGNING_ERROR                             (CC_ECPKI_MODULE_ERROR_BASE + 0x67UL)

/* The CryptoCell ECDSA Verifying  errors */
/*! Illegal domain ID. */
#define CC_ECDSA_VERIFY_INVALID_DOMAIN_ID_ERROR         (CC_ECPKI_MODULE_ERROR_BASE + 0x70UL)
/*! Illegal user's context pointer. */
#define CC_ECDSA_VERIFY_INVALID_USER_CONTEXT_PTR_ERROR      (CC_ECPKI_MODULE_ERROR_BASE + 0x71UL)
/*! Illegal public key pointer. */
#define CC_ECDSA_VERIFY_INVALID_SIGNER_PUBL_KEY_PTR_ERROR       (CC_ECPKI_MODULE_ERROR_BASE + 0x72UL)
/*! Illegal hash operation mode. */
#define CC_ECDSA_VERIFY_ILLEGAL_HASH_OP_MODE_ERROR              (CC_ECPKI_MODULE_ERROR_BASE + 0x73UL)
/*! Illegal signature pointer. */
#define CC_ECDSA_VERIFY_INVALID_SIGNATURE_IN_PTR_ERROR          (CC_ECPKI_MODULE_ERROR_BASE + 0x76UL)
/*! Illegal signature size. */
#define CC_ECDSA_VERIFY_INVALID_SIGNATURE_SIZE_ERROR        (CC_ECPKI_MODULE_ERROR_BASE + 0x77UL)
/*! Illegal data in pointer. */
#define CC_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_PTR_ERROR       (CC_ECPKI_MODULE_ERROR_BASE + 0x80UL)
/*! Illegal data in size. */
#define CC_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_SIZE_ERROR      (CC_ECPKI_MODULE_ERROR_BASE + 0x81UL)
/*! Context validation failed. */
#define CC_ECDSA_VERIFY_USER_CONTEXT_VALIDATION_TAG_ERROR       (CC_ECPKI_MODULE_ERROR_BASE + 0x82UL)
/*! public key validation failed. */
#define CC_ECDSA_VERIFY_SIGNER_PUBL_KEY_VALIDATION_TAG_ERROR    (CC_ECPKI_MODULE_ERROR_BASE + 0x83UL)
/*! Verification failed. */
#define CC_ECDSA_VERIFY_INCONSISTENT_VERIFY_ERROR               (CC_ECPKI_MODULE_ERROR_BASE + 0x84UL)



/*! Illegal hash mode. */
#define CC_ECC_ILLEGAL_HASH_MODE_ERROR                         (CC_ECPKI_MODULE_ERROR_BASE + 0x85UL)


/************************************************************************************************************
 * CryptoCell ECPKI MODULE  COMMON ERRORS
*************************************************************************************************************/
/*! Illegal RND function pointer. */
#define CC_ECPKI_INVALID_RND_FUNC_PTR_ERROR                   (CC_ECPKI_MODULE_ERROR_BASE + 0x90UL)
/*! Illegal RND context pointer. */
#define CC_ECPKI_INVALID_RND_CTX_PTR_ERROR                    (CC_ECPKI_MODULE_ERROR_BASE + 0x91UL)
/*! Illegal domain ID. */
#define CC_ECPKI_INVALID_DOMAIN_ID_ERROR                      (CC_ECPKI_MODULE_ERROR_BASE + 0x92UL)
/*! Private key validation failed. */
#define CC_ECPKI_INVALID_PRIV_KEY_TAG_ERROR                   (CC_ECPKI_MODULE_ERROR_BASE + 0x93UL)
/*! Public key validation failed. */
#define CC_ECPKI_INVALID_PUBL_KEY_TAG_ERROR                   (CC_ECPKI_MODULE_ERROR_BASE + 0x94UL)
/*! Illegal data in. */
#define CC_ECPKI_INVALID_DATA_IN_PASSED_STRUCT_ERROR          (CC_ECPKI_MODULE_ERROR_BASE + 0x95UL)
/*! Illegal Base point pointer. */
#define CC_ECPKI_INVALID_BASE_POINT_PTR_ERROR          (CC_ECPKI_MODULE_ERROR_BASE + 0x96UL)

/************************************************************************************************************
 * CryptoCell ECIES MODULE ERRORS
*************************************************************************************************************/
/*! Illegal public key pointer. */
#define CC_ECIES_INVALID_PUBL_KEY_PTR_ERROR                     (CC_ECPKI_MODULE_ERROR_BASE + 0xE0UL)
/*! Public key validation failed. */
#define CC_ECIES_INVALID_PUBL_KEY_TAG_ERROR         (CC_ECPKI_MODULE_ERROR_BASE + 0xE1UL)
/*! Illegal private key pointer. */
#define CC_ECIES_INVALID_PRIV_KEY_PTR_ERROR                     (CC_ECPKI_MODULE_ERROR_BASE + 0xE2UL)
/*! Private key validation failed. */
#define CC_ECIES_INVALID_PRIV_KEY_TAG_ERROR         (CC_ECPKI_MODULE_ERROR_BASE + 0xE3UL)
/*! Illegal private key value. */
#define CC_ECIES_INVALID_PRIV_KEY_VALUE_ERROR           (CC_ECPKI_MODULE_ERROR_BASE + 0xE4UL)
/*! Illegal KDF derivation mode. */
#define CC_ECIES_INVALID_KDF_DERIV_MODE_ERROR               (CC_ECPKI_MODULE_ERROR_BASE + 0xE5UL)
/*! Illegal KDF hash mode. */
#define CC_ECIES_INVALID_KDF_HASH_MODE_ERROR            (CC_ECPKI_MODULE_ERROR_BASE + 0xE6UL)
/*! Illegal secret key pointer. */
#define CC_ECIES_INVALID_SECRET_KEY_PTR_ERROR           (CC_ECPKI_MODULE_ERROR_BASE + 0xE7UL)
/*! Illegal secret key size. */
#define CC_ECIES_INVALID_SECRET_KEY_SIZE_ERROR          (CC_ECPKI_MODULE_ERROR_BASE + 0xE8UL)
/*! Illegal cipher data pointer. */
#define CC_ECIES_INVALID_CIPHER_DATA_PTR_ERROR          (CC_ECPKI_MODULE_ERROR_BASE + 0xE9UL)
/*! Illegal cipher data size pointer. */
#define CC_ECIES_INVALID_CIPHER_DATA_SIZE_PTR_ERROR         (CC_ECPKI_MODULE_ERROR_BASE + 0xEAUL)
/*! Illegal cipher data size. */
#define CC_ECIES_INVALID_CIPHER_DATA_SIZE_ERROR         (CC_ECPKI_MODULE_ERROR_BASE + 0xEBUL)
/*! Illegal temporary buffer pointer. */
#define CC_ECIES_INVALID_TEMP_DATA_PTR_ERROR            (CC_ECPKI_MODULE_ERROR_BASE + 0xECUL)
/*! Illegal temporary buffe size */
#define CC_ECIES_INVALID_TEMP_DATA_SIZE_ERROR           (CC_ECPKI_MODULE_ERROR_BASE + 0xEDUL)
/*! Illegal ephemeral key pointer */
#define CC_ECIES_INVALID_EPHEM_KEY_PAIR_PTR_ERROR               (CC_ECPKI_MODULE_ERROR_BASE + 0xEEUL)
/*! NULL ptr */
#define CC_ECIES_INVALID_PTR                                   (CC_ECPKI_MODULE_ERROR_BASE + 0xEFUL)

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif
/**
@}
 */

#endif


