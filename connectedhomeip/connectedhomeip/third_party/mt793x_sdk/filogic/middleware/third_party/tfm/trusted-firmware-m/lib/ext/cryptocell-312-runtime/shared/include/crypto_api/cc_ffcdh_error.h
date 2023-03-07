/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_FFCDH_ERROR_H
#define _CC_FFCDH_ERROR_H


#include "cc_error.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*!
@file
@brief This file contains error codes definitions for CryptoCell FFCDH module.
@defgroup ffccc_dh_error CryptoCell FFCDH specific errors
@{
@ingroup cc_ffcdh
*/
/************************ Defines ******************************/

/* FFCDH module on the CryptoCell layer base address - 0x00F02700 */

/*! The CryptoCell FFCDH module errors */

/*! Invalid input argument pointer. */
#define CC_FFCDH_INVALID_ARGUMENT_POINTER_ERROR         (CC_FFCDH_MODULE_ERROR_BASE + 0x00UL)
/*! Invalid input argument size. */
#define CC_FFCDH_INVALID_ARGUMENT_SIZE_ERROR            (CC_FFCDH_MODULE_ERROR_BASE + 0x01UL)
/*! Invalid pointer to DH Context structure. */
#define CC_FFCDH_INVALID_CONTEXT_PTR_ERROR          (CC_FFCDH_MODULE_ERROR_BASE + 0x02UL)
/*! Invalid DH Context validation Tag.*/
#define CC_FFCDH_CONTEXT_VALIDATION_TAG_ERROR                   (CC_FFCDH_MODULE_ERROR_BASE + 0x03UL)
/*! Invalid FFCDH Scheme ID. */
#define CC_FFCDH_INVALID_SCHEM_ID_ERROR                         (CC_FFCDH_MODULE_ERROR_BASE + 0x04UL)
/*! Invalid FFCDH parameters set ID. */
#define CC_FFCDH_INVALID_DOMAIN_SIZES_SET_ID_ERROR      (CC_FFCDH_MODULE_ERROR_BASE + 0x05UL)
/*! Invalid FFCDH Key confirmation mode. */
#define CC_FFCDH_INVALID_CONFIRM_MODE_ERROR             (CC_FFCDH_MODULE_ERROR_BASE + 0x06UL)
/*! Invalid FFCDH User party ID. */
#define CC_FFCDH_INVALID_USER_PARTY_ID_ERROR                    (CC_FFCDH_MODULE_ERROR_BASE + 0x07UL)
/*! Invalid FFCDH key derivation function mode. */
#define CC_FFCDH_INVALID_KDF_MODE_ERROR                         (CC_FFCDH_MODULE_ERROR_BASE + 0x08UL)
/*! Invalid FFCDH Key validation mode. */
#define CC_FFCDH_INVALID_VALIDAT_MODE_ERROR                     (CC_FFCDH_MODULE_ERROR_BASE + 0x09UL)
/*! Invalid HASH operation mode.*/
#define CC_FFCDH_INVALID_HASH_MODE_ERROR                (CC_FFCDH_MODULE_ERROR_BASE + 0x0AUL)
/*! Invalid HASH operation digest size is too low.*/
#define CC_FFCDH_INVALID_LOW_HASH_SIZE_ERROR                    (CC_FFCDH_MODULE_ERROR_BASE + 0x0BUL)
/*! Invalid HMAC result size is too low.*/
#define CC_FFCDH_INVALID_HMAC_SALT_PARAMS_ERROR             (CC_FFCDH_MODULE_ERROR_BASE + 0x0CUL)
/*! Invalid private key size. */
#define CC_FFCDH_INVALID_PRIVATE_KEY_SIZE_ERROR                 (CC_FFCDH_MODULE_ERROR_BASE + 0x10UL)
/*! Invalid private key value. */
#define CC_FFCDH_INVALID_PRIVATE_KEY_VALUE_ERROR                (CC_FFCDH_MODULE_ERROR_BASE + 0x11UL)
/*! Invalid public key size. */
#define CC_FFCDH_INVALID_PUBLIC_KEY_SIZE_ERROR                  (CC_FFCDH_MODULE_ERROR_BASE + 0x12UL)
/*! Invalid public key value. */
#define CC_FFCDH_INVALID_PUBLIC_KEY_VALUE_ERROR                 (CC_FFCDH_MODULE_ERROR_BASE + 0x13UL)
/*! Invalid key status mode: static or ephemeral. */
#define CC_FFCDH_INVALID_KEY_STATUS_ERROR           (CC_FFCDH_MODULE_ERROR_BASE + 0x14UL)
/*! Invalid rewriting of previously inserted parameter. */
#define CC_FFCDH_ILLEGAL_TRY_REWRITE_PARAM_ERROR                (CC_FFCDH_MODULE_ERROR_BASE + 0x15UL)
/*! Invalid optional data parameters (pointer, size). */
#define CC_FFCDH_OPTIONAL_DATA_ERROR                            (CC_FFCDH_MODULE_ERROR_BASE + 0x16UL)
/*! Invalid parameters of Algorithm ID data  (pointer, size). */
#define CC_FFCDH_ALGORITHM_ID_ERROR                             (CC_FFCDH_MODULE_ERROR_BASE + 0x17UL)
/*! Invalid size of any Party Info entry (too great). */
#define CC_FFCDH_PARTY_INFO_SUB_ENTRY_SIZE_ERROR                (CC_FFCDH_MODULE_ERROR_BASE + 0x18UL)
/*! The User tries to pass Nonce, not required by DH Scheme. */
#define CC_FFCDH_NONCE_IS_NOT_REQUIRED_ERROR                    (CC_FFCDH_MODULE_ERROR_BASE + 0x19UL)
/*! The User tries to pass some Key, not required by DH Scheme. */
#define CC_FFCDH_THE_KEY_IS_NOT_REQUIRED_ERROR                  (CC_FFCDH_MODULE_ERROR_BASE + 0x1AUL)
/*! The output buffer is too low */
#define CC_FFCDH_LOW_OUTPUT_BUFF_SIZE_ERROR                     (CC_FFCDH_MODULE_ERROR_BASE + 0x1BUL)
/*! Invalid size of Partner Info entry. */
#define CC_FFCDH_PARTN_INFO_PARSING_SIZE_ERROR                  (CC_FFCDH_MODULE_ERROR_BASE + 0x20UL)
/*! Error on parsing and comparing of Partner Info data . */
#define CC_FFCDH_PARTN_INFO_PARSING_DATA_ERROR                  (CC_FFCDH_MODULE_ERROR_BASE + 0x21UL)
/*! Invalid output pointer to Keying Data. */
#define CC_FFCDH_KEYING_DATA_PTR_INVALID_ERROR          (CC_FFCDH_MODULE_ERROR_BASE + 0x22UL)
/*! Invalid pointer to Keying Data size. */
#define CC_FFCDH_KEYING_DATA_SIZE_PTR_INVALID_ERROR     (CC_FFCDH_MODULE_ERROR_BASE + 0x23UL)
/*! Invalid size of output Keying Data buffer given by the user. */
#define CC_FFCDH_KEYING_DATA_SIZE_INVALID_ERROR         (CC_FFCDH_MODULE_ERROR_BASE + 0x24UL)
/*! Invalid pointer to MacTag output buffer. */
#define CC_FFCDH_MAC_TAG_PTR_INVALID_ERROR          (CC_FFCDH_MODULE_ERROR_BASE + 0x25UL)
/*! Invalid size of MacTag output buffer. */
#define CC_FFCDH_MAC_TAG_SIZE_INVALID_ERROR         (CC_FFCDH_MODULE_ERROR_BASE + 0x26UL)
/*! Calculated MacTag not matches to value, provided by the partner. */
#define CC_FFCDH_MAC_TAG_DATA_INVALID_ERROR         (CC_FFCDH_MODULE_ERROR_BASE + 0x27UL)
/* Invalid FFC DH Domain pointer. */
#define CC_FFCDH_INVALID_DOMAIN_PTR_ERROR                       (CC_FFCDH_MODULE_ERROR_BASE + 0x30UL)
/*! Invalid validation Tag of user passed FFC Domain. */
#define CC_FFCDH_INVALID_DOMAIN_VALIDAT_TAG_ERROR               (CC_FFCDH_MODULE_ERROR_BASE + 0x31UL)
/*! FFC Domain parameters not meet to required by input FFC sizes set ID or HASH mode. */
#define CC_FFCDH_INVALID_DOMAIN_DATA_ERROR                      (CC_FFCDH_MODULE_ERROR_BASE + 0x32UL)
/*! Invalid pointer to FFC DH Shared Secret Value. */
#define CC_FFCDH_INVALID_SHARED_SECR_VAL_PTR_ERROR              (CC_FFCDH_MODULE_ERROR_BASE + 0x33UL)
/*! Invalid private key data  pointer. */
#define CC_FFCDH_INVALID_PRIV_KEY_PTR_ERROR                     (CC_FFCDH_MODULE_ERROR_BASE + 0x34UL)
/*! invalid public key size. */
#define CC_FFCDH_INVALID_PUBL_KEY_PTR_ERROR                     (CC_FFCDH_MODULE_ERROR_BASE + 0x35UL)

/*! FFC DH is not supported */
#define CC_FFCDH_IS_NOT_SUPPORTED               (CC_FFCDH_MODULE_ERROR_BASE + 0xFFUL)


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

