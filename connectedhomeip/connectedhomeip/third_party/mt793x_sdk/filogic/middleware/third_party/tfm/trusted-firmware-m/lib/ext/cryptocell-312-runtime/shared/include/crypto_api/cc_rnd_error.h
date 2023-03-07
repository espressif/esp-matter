/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_RND_ERROR_H
#define _CC_RND_ERROR_H

#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*!
@file
@brief This file contains the definitions of the CryptoCell RND errors.
@defgroup cc_rnd_error CryptoCell RND specific errors
@{
@ingroup cc_rnd
*/


/************************ Defines ******************************/
/*! RND module on the CryptoCell layer base address - 0x00F00C00 */
/*! Illegal output pointer.*/
#define CC_RND_DATA_OUT_POINTER_INVALID_ERROR           (CC_RND_MODULE_ERROR_BASE + 0x0UL)
/*! Random generation in range failed .*/
#define CC_RND_CAN_NOT_GENERATE_RAND_IN_RANGE           (CC_RND_MODULE_ERROR_BASE + 0x1UL)
/*! CPRNGT test failed.*/
#define CC_RND_CPRNG_TEST_FAIL_ERROR                (CC_RND_MODULE_ERROR_BASE + 0x2UL)
/*! Illegal additional data buffer. */
#define CC_RND_ADDITIONAL_INPUT_BUFFER_NULL         (CC_RND_MODULE_ERROR_BASE + 0x3UL)
/*! Illegal additional data size. */
#define CC_RND_ADDITIONAL_INPUT_SIZE_ERROR          (CC_RND_MODULE_ERROR_BASE + 0x4UL)
/*! Data size overflow. */
#define CC_RND_DATA_SIZE_OVERFLOW_ERROR             (CC_RND_MODULE_ERROR_BASE + 0x5UL)
/*! Illegal vector size. */
#define CC_RND_VECTOR_SIZE_ERROR            (CC_RND_MODULE_ERROR_BASE + 0x6UL)
/*! Reseed counter overflow - in case this error was returned instantiation or reseeding operation must be called. */
#define CC_RND_RESEED_COUNTER_OVERFLOW_ERROR        (CC_RND_MODULE_ERROR_BASE + 0x7UL)
/*! Instantiation was not yet called. */
#define CC_RND_INSTANTIATION_NOT_DONE_ERROR     (CC_RND_MODULE_ERROR_BASE + 0x8UL)
/*! TRNG loss of samples. */
#define CC_RND_TRNG_LOSS_SAMPLES_ERROR                  (CC_RND_MODULE_ERROR_BASE + 0x9UL)
/*! TRNG Time exceeded limitations. */
#define CC_RND_TRNG_TIME_EXCEED_ERROR               (CC_RND_MODULE_ERROR_BASE + 0xAUL)
/*! TRNG loss of samples and time exceeded limitations. */
#define CC_RND_TRNG_LOSS_SAMPLES_AND_TIME_EXCEED_ERROR  (CC_RND_MODULE_ERROR_BASE + 0xBUL)
/*! RND is in Known Answer Test mode. */
#define CC_RND_IS_KAT_MODE_ERROR                        (CC_RND_MODULE_ERROR_BASE + 0xCUL)
/*! RND operation not supported. */
#define CC_RND_OPERATION_IS_NOT_SUPPORTED_ERROR         (CC_RND_MODULE_ERROR_BASE + 0xDUL)
/*! RND validity check failed. */
#define CC_RND_STATE_VALIDATION_TAG_ERROR               (CC_RND_MODULE_ERROR_BASE + 0xEUL)
/*! RND is not supported. */
#define CC_RND_IS_NOT_SUPPORTED                         (CC_RND_MODULE_ERROR_BASE + 0xFUL)

/*! Illegal generate vector function pointer. */
#define CC_RND_GEN_VECTOR_FUNC_ERROR                (CC_RND_MODULE_ERROR_BASE + 0x14UL)

/*! Illegal work buffer pointer. */
#define CC_RND_WORK_BUFFER_PTR_INVALID_ERROR            (CC_RND_MODULE_ERROR_BASE + 0x20UL)
/*! Illegal AES key size. */
#define CC_RND_ILLEGAL_AES_KEY_SIZE_ERROR               (CC_RND_MODULE_ERROR_BASE + 0x21UL)
/*! Illegal data pointer. */
#define CC_RND_ILLEGAL_DATA_PTR_ERROR                   (CC_RND_MODULE_ERROR_BASE + 0x22UL)
/*! Illegal data size. */
#define CC_RND_ILLEGAL_DATA_SIZE_ERROR                  (CC_RND_MODULE_ERROR_BASE + 0x23UL)
/*! Illegal parameter. */
#define CC_RND_ILLEGAL_PARAMETER_ERROR                  (CC_RND_MODULE_ERROR_BASE + 0x24UL)
/*! Illegal RND state pointer. */
#define CC_RND_STATE_PTR_INVALID_ERROR                  (CC_RND_MODULE_ERROR_BASE + 0x25UL)
/*! TRNG errors. */
#define CC_RND_TRNG_ERRORS_ERROR                        (CC_RND_MODULE_ERROR_BASE + 0x26UL)
/*! Illegal context pointer. */
#define CC_RND_CONTEXT_PTR_INVALID_ERROR                (CC_RND_MODULE_ERROR_BASE + 0x27UL)
/*! Illegal output vector pointer. */
#define CC_RND_VECTOR_OUT_PTR_ERROR                     (CC_RND_MODULE_ERROR_BASE + 0x30UL)
/*! Illegal output vector size. */
#define CC_RND_VECTOR_OUT_SIZE_ERROR            (CC_RND_MODULE_ERROR_BASE + 0x31UL)
/*! Maximal vector size is too small. */
#define CC_RND_MAX_VECTOR_IS_TOO_SMALL_ERROR        (CC_RND_MODULE_ERROR_BASE + 0x32UL)
/*! Illegal Known Answer Tests parameters. */
#define CC_RND_KAT_DATA_PARAMS_ERROR                    (CC_RND_MODULE_ERROR_BASE + 0x33UL)
/*! TRNG Known Answer Test not supported. */
#define CC_RND_TRNG_KAT_NOT_SUPPORTED_ERROR             (CC_RND_MODULE_ERROR_BASE + 0x34UL)
/*! SRAM memory is not defined. */
#define CC_RND_SRAM_NOT_SUPPORTED_ERROR                 (CC_RND_MODULE_ERROR_BASE + 0x35UL)
/*! AES operation failure. */
#define CC_RND_AES_ERROR                                (CC_RND_MODULE_ERROR_BASE + 0x36UL)
/*! TRNG mode mismatch between PAL and lib */
#define CC_RND_MODE_MISMATCH_ERROR                      (CC_RND_MODULE_ERROR_BASE + 0x37UL)


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


