/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_AESCCM_ERROR_H
#define _CC_AESCCM_ERROR_H


#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*!
@file
@brief This file contains the definitions of the CryptoCell AESCCM errors.
@defgroup cc_aesccm_error CryptoCell AES-CCM specific errors
@{
@ingroup cc_aesccm

*/

/************************ Defines ******************************/

/*! CryptoCell AESCCM module errors. CC_AESCCM_MODULE_ERROR_BASE = 0x00F01500 */
/*! Invalid context pointer. */
#define CC_AESCCM_INVALID_USER_CONTEXT_POINTER_ERROR     (CC_AESCCM_MODULE_ERROR_BASE + 0x00UL)
/*! Illegal key size. */
#define CC_AESCCM_ILLEGAL_KEY_SIZE_ERROR                 (CC_AESCCM_MODULE_ERROR_BASE + 0x01UL)
/*! Invalid key pointer. */
#define CC_AESCCM_INVALID_KEY_POINTER_ERROR              (CC_AESCCM_MODULE_ERROR_BASE + 0x02UL)
/*! Invalid encryption mode. */
#define CC_AESCCM_INVALID_ENCRYPT_MODE_ERROR             (CC_AESCCM_MODULE_ERROR_BASE + 0x03UL)
/*! Context is corrupted. */
#define CC_AESCCM_USER_CONTEXT_CORRUPTED_ERROR           (CC_AESCCM_MODULE_ERROR_BASE + 0x04UL)
/*! Invalid data in pointer. */
#define CC_AESCCM_DATA_IN_POINTER_INVALID_ERROR          (CC_AESCCM_MODULE_ERROR_BASE + 0x05UL)
/*! Invalid data out pointer. */
#define CC_AESCCM_DATA_OUT_POINTER_INVALID_ERROR         (CC_AESCCM_MODULE_ERROR_BASE + 0x06UL)
/*! Illegal data in size. */
#define CC_AESCCM_DATA_IN_SIZE_ILLEGAL                   (CC_AESCCM_MODULE_ERROR_BASE + 0x07UL)
/*! Illegal data in or data out address. */
#define CC_AESCCM_DATA_OUT_DATA_IN_OVERLAP_ERROR         (CC_AESCCM_MODULE_ERROR_BASE + 0x08UL)
/*! Illegal data out size. */
#define CC_AESCCM_DATA_OUT_SIZE_INVALID_ERROR            (CC_AESCCM_MODULE_ERROR_BASE + 0x09UL)
/*! Illegal call to process additional data. */
#define CC_AESCCM_ADDITIONAL_BLOCK_NOT_PERMITTED_ERROR   (CC_AESCCM_MODULE_ERROR_BASE + 0x0AUL)
/*! Illegal dma buffer type. */
#define CC_AESCCM_ILLEGAL_DMA_BUFF_TYPE_ERROR            (CC_AESCCM_MODULE_ERROR_BASE + 0x0BUL)
/*! Illegal parameter size. */
#define CC_AESCCM_ILLEGAL_PARAMETER_SIZE_ERROR           (CC_AESCCM_MODULE_ERROR_BASE + 0x0CUL)
/*! Invalid parameter pointer. */
#define CC_AESCCM_ILLEGAL_PARAMETER_PTR_ERROR            (CC_AESCCM_MODULE_ERROR_BASE + 0x0DUL)
/*! Invalid data type. */
#define CC_AESCCM_ILLEGAL_DATA_TYPE_ERROR                (CC_AESCCM_MODULE_ERROR_BASE + 0x0EUL)
/*! CCM MAC compare failure. */
#define CC_AESCCM_CCM_MAC_INVALID_ERROR                  (CC_AESCCM_MODULE_ERROR_BASE + 0x0FUL)
/*! Illegal operation. */
#define CC_AESCCM_LAST_BLOCK_NOT_PERMITTED_ERROR         (CC_AESCCM_MODULE_ERROR_BASE + 0x10UL)
/*! Illegal parameter. */
#define CC_AESCCM_ILLEGAL_PARAMETER_ERROR                (CC_AESCCM_MODULE_ERROR_BASE + 0x11UL)
/*! Additional data input size is incorrect. */
#define CC_AESCCM_NOT_ALL_ADATA_WAS_PROCESSED_ERROR      (CC_AESCCM_MODULE_ERROR_BASE + 0x13UL)
/*! Text data input size is incorrect. */
#define CC_AESCCM_NOT_ALL_DATA_WAS_PROCESSED_ERROR       (CC_AESCCM_MODULE_ERROR_BASE + 0x14UL)
/*! Additional data was already processed (must be processed only once). */
#define CC_AESCCM_ADATA_WAS_PROCESSED_ERROR          (CC_AESCCM_MODULE_ERROR_BASE + 0x15UL)
/*! Illegal Nonce size. */
#define CC_AESCCM_ILLEGAL_NONCE_SIZE_ERROR       (CC_AESCCM_MODULE_ERROR_BASE + 0x16UL)
/*! Illegal tag (MAC) size. */
#define CC_AESCCM_ILLEGAL_TAG_SIZE_ERROR         (CC_AESCCM_MODULE_ERROR_BASE + 0x17UL)
/*! Illegal context size. */
#define CC_AESCCM_CTX_SIZES_ERROR            (CC_AESCCM_MODULE_ERROR_BASE + 0x28UL)
/*! Illegal parameters. */
#define CC_AESCCM_ILLEGAL_PARAMS_ERROR           (CC_AESCCM_MODULE_ERROR_BASE + 0x29UL)
/*! AESCCM is not supported. */
#define CC_AESCCM_IS_NOT_SUPPORTED                       (CC_AESCCM_MODULE_ERROR_BASE + 0xFFUL)

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


#endif /* _CC_AESCCM_ERROR_H */


