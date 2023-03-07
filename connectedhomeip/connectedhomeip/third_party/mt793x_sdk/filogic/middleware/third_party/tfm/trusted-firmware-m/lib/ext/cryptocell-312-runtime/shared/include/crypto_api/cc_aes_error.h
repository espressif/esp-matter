/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
@file
@brief This file contains the definitions of the CryptoCell AES errors.
@defgroup cc_aes_error CryptoCell AES specific errors
@{
@ingroup cc_aes
*/

#ifndef CC_AES_ERROR_H
#define CC_AES_ERROR_H

#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/

/*! CC_AES_MODULE_ERROR_BASE - 0x00F00000 */
/*! Illegal user context. */
#define CC_AES_INVALID_USER_CONTEXT_POINTER_ERROR     (CC_AES_MODULE_ERROR_BASE + 0x00UL)
/*! Illegal IV or tweak pointer. */
#define CC_AES_INVALID_IV_OR_TWEAK_PTR_ERROR          (CC_AES_MODULE_ERROR_BASE + 0x01UL)
/*! Illegal operation. */
#define CC_AES_ILLEGAL_OPERATION_MODE_ERROR           (CC_AES_MODULE_ERROR_BASE + 0x02UL)
/*! Illegal key size. */
#define CC_AES_ILLEGAL_KEY_SIZE_ERROR                 (CC_AES_MODULE_ERROR_BASE + 0x03UL)
/*! Illegal key pointer. */
#define CC_AES_INVALID_KEY_POINTER_ERROR              (CC_AES_MODULE_ERROR_BASE + 0x04UL)
/*! Unsupported key type. */
#define CC_AES_KEY_TYPE_NOT_SUPPORTED_ERROR           (CC_AES_MODULE_ERROR_BASE + 0x05UL)
/*! Illegal operation. */
#define CC_AES_INVALID_ENCRYPT_MODE_ERROR             (CC_AES_MODULE_ERROR_BASE + 0x06UL)
/*! User context corrupted. */
#define CC_AES_USER_CONTEXT_CORRUPTED_ERROR           (CC_AES_MODULE_ERROR_BASE + 0x07UL)
/*! Illegal data in pointer. */
#define CC_AES_DATA_IN_POINTER_INVALID_ERROR          (CC_AES_MODULE_ERROR_BASE + 0x08UL)
/*! Illegal data out pointer. */
#define CC_AES_DATA_OUT_POINTER_INVALID_ERROR         (CC_AES_MODULE_ERROR_BASE + 0x09UL)
/*! Illegal data in size. */
#define CC_AES_DATA_IN_SIZE_ILLEGAL                   (CC_AES_MODULE_ERROR_BASE + 0x0AUL)
/*! Illegal data out address. */
#define CC_AES_DATA_OUT_DATA_IN_OVERLAP_ERROR         (CC_AES_MODULE_ERROR_BASE + 0x0BUL)
/*! Illegal data in buffer size. */
#define CC_AES_DATA_IN_BUFFER_SIZE_ERROR              (CC_AES_MODULE_ERROR_BASE + 0x0CUL)
/*! Illegal data out buffer size. */
#define CC_AES_DATA_OUT_BUFFER_SIZE_ERROR             (CC_AES_MODULE_ERROR_BASE + 0x0DUL)
/*! Illegal padding type. */
#define CC_AES_ILLEGAL_PADDING_TYPE_ERROR             (CC_AES_MODULE_ERROR_BASE + 0x0EUL)
/*! Incorrect padding. */
#define CC_AES_INCORRECT_PADDING_ERROR                (CC_AES_MODULE_ERROR_BASE + 0x0FUL)
/*! Output is corrupted. */
#define CC_AES_CORRUPTED_OUTPUT_ERROR                 (CC_AES_MODULE_ERROR_BASE + 0x10UL)
/*! Illegal output size. */
#define CC_AES_DATA_OUT_SIZE_POINTER_INVALID_ERROR    (CC_AES_MODULE_ERROR_BASE + 0x11UL)
/*! Decryption operation is not permitted in this mode. */
#define CC_AES_DECRYPTION_NOT_ALLOWED_ON_THIS_MODE    (CC_AES_MODULE_ERROR_BASE + 0x12UL)
/*! Additional block operation is not permitted. */
#define CC_AES_ADDITIONAL_BLOCK_NOT_PERMITTED_ERROR   (CC_AES_MODULE_ERROR_BASE + 0x15UL)
/*! Illegal context size. */
#define CC_AES_CTX_SIZES_ERROR                      (CC_AES_MODULE_ERROR_BASE + 0x16UL)
/*! Illegal parameters. */
#define CC_AES_ILLEGAL_PARAMS_ERROR               (CC_AES_MODULE_ERROR_BASE + 0x60UL)
/*! Illegal CTR block offset. */
#define CC_AES_CTR_ILLEGAL_BLOCK_OFFSET_ERROR     (CC_AES_MODULE_ERROR_BASE + 0x70UL)
/*! Illegal counter (in CTR mode). */
#define CC_AES_CTR_ILLEGAL_COUNTER_ERROR          (CC_AES_MODULE_ERROR_BASE + 0x71UL)
/*! AES is not supported. */
#define CC_AES_IS_NOT_SUPPORTED                   (CC_AES_MODULE_ERROR_BASE + 0xFFUL)

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/

#ifdef __cplusplus
}
#endif
/**
@}
 */

#endif /* #ifndef CC_AES_ERROR_H */
