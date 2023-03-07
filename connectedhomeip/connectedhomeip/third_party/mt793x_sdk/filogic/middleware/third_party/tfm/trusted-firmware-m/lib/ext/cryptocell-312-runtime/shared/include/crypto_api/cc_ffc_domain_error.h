/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_FFC_DOMAIN_ERROR_H
#define _CC_FFC_DOMAIN_ERROR_H


#include "cc_error.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*!
@file
@brief This file contains error codes definitions for CryptoCell FFC_DOMAIN module.
@defgroup ffccc_dh_error CryptoCell FFC_DOMAIN specific errors
@{
@ingroup cc_ffc_domain
*/
/************************ Defines ******************************/

/* FFC_DOMAIN module on the CryptoCell layer base address - 0x00F02C00 */

/*! Invalid input pointer.*/
#define CC_FFC_DOMAIN_INVALID_ARGUMENT_PTR_ERROR        (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x00UL)
/*! Invalid input size.*/
#define CC_FFC_DOMAIN_INVALID_ARGUMENT_SIZE_ERROR       (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x01UL)
/*! A pointer and size of optional parameter not meets one to other: one is zero, but other - not. */
#define CC_FFC_DOMAIN_INVALID_OPTIONAL_PARAM_ERROR              (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x02UL)
/*! Invalid pointer to domain structure. */
#define CC_FFC_DOMAIN_INVALID_DOMAIN_PTR_ERROR                  (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x03UL)
/*! Invalid pointer to random function Context. */
#define CC_FFC_DOMAIN_INVALID_RND_CTX_PTR_ERROR                 (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x04UL)
/*! Invalid pointer to random function inside the Context. */
#define CC_FFC_DOMAIN_INVALID_RND_FUNCTION_PTR_ERROR            (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x05UL)
/*! Invalid Domain validation Tag. */
#define CC_FFC_DOMAIN_VALIDATION_TAG_ERROR                      (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x06UL)
/*! Invalid enumerator of FFC sizes standard set ID. */
#define CC_FFC_DOMAIN_INVALID_SIZES_SET_ID_ERROR        (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x07UL)
/*! Invalid enumerator of hash mode. */
#define CC_FFC_DOMAIN_INVALID_HASH_MODE_ERROR               (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x08UL)
/*! Invalid enumerator of seed generation mode. */
#define CC_FFC_DOMAIN_INVALID_SEED_GENERATION_MODE_ERROR    (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x09UL)
/*! Invalid enumerator of Domain validation mode. */
#define CC_FFC_DOMAIN_INVALID_VALIDAT_MODE_ERROR                (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x0AUL)
/*! Invalid Hash mode: size of Hash too low for required security. */
#define CC_FFC_DOMAIN_INVALID_LOW_HASH_SIZE_ERROR               (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x0BUL)
/*! Invalid prime modulus size. */
#define CC_FFC_DOMAIN_INVALID_PRIME_SIZE_ERROR              (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x10UL)
/*! Invalid FFC sub-group Order size. */
#define CC_FFC_DOMAIN_INVALID_ORDER_SIZE_ERROR          (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x11UL)
/*! Invalid Domain generation Seed pointer. */
#define CC_FFC_DOMAIN_INVALID_SEED_PTR_ERROR                    (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x12UL)
/*! Invalid Domain generation Seed size. */
#define CC_FFC_DOMAIN_INVALID_SEED_SIZE_ERROR           (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x13UL)
/*! Invalid FFC Domain Prime value. */
#define CC_FFC_DOMAIN_PRIME_NOT_VALID_ERROR                     (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x14UL)
/*! Invalid FFC Domain Order value. */
#define CC_FFC_DOMAIN_ORDER_NOT_VALID_ERROR                 (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x15UL)
/*! Invalid FFC Domain sub-group Generator. */
#define CC_FFC_DOMAIN_GENERATOR_NOT_VALID_ERROR                 (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x16UL)
/*! Invalid FFC Domain generation Counter. */
#define CC_FFC_DOMAIN_GEN_COUNTER_NOT_VALID_ERROR               (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x17UL)
/*! Seed is not required by given DH Scheme. */
#define CC_FFC_DOMAIN_SEED_IS_NOT_REQUIRED_ERROR                (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x18UL)
/*! Domain generation is failed. */
#define CC_FFC_DOMAIN_GENERATION_FAILURE_ERROR                  (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0x19UL)

#define CC_FFC_DOMAIN_IS_NOT_SUPPORTED              (CC_FFC_DOMAIN_MODULE_ERROR_BASE + 0xFFUL)


#ifdef __cplusplus
}
#endif
/**
@}
 */

#endif

