/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef _SBROM_BSV_ERROR_H
#define _SBROM_BSV_ERROR_H

#ifdef __cplusplus
extern "C"
{
#endif

/*! @file
@brief This file defines the error code types returned from the Boot Services API's.
*/

/*! Defines BSV base error. */
#define CC_BSV_BASE_ERROR                           0x0B000000
/*! Defines BSV cryptographic base error. */
#define CC_BSV_CRYPTO_ERROR                         0x0C000000

/*! Illegal input parameter error. */
#define CC_BSV_ILLEGAL_INPUT_PARAM_ERR              (CC_BSV_BASE_ERROR + 0x00000001)
/*! Illegal HUK value error. */
#define CC_BSV_ILLEGAL_HUK_VALUE_ERR            (CC_BSV_BASE_ERROR + 0x00000002)
/*! Illegal Kcp value error. */
#define CC_BSV_ILLEGAL_KCP_VALUE_ERR            (CC_BSV_BASE_ERROR + 0x00000003)
/*! Illegal Kce value error. */
#define CC_BSV_ILLEGAL_KCE_VALUE_ERR            (CC_BSV_BASE_ERROR + 0x00000004)
/*! Illegal Kpicv value error. */
#define CC_BSV_ILLEGAL_KPICV_VALUE_ERR          (CC_BSV_BASE_ERROR + 0x00000005)
/*! Illegal Kceicv value error. */
#define CC_BSV_ILLEGAL_KCEICV_VALUE_ERR         (CC_BSV_BASE_ERROR + 0x00000006)
/*! Hash boot key not programmed in the OTP error. */
#define CC_BSV_HASH_NOT_PROGRAMMED_ERR          (CC_BSV_BASE_ERROR + 0x00000007)
/*! Illegal Hash boot key zero count in the OTP error. */
#define CC_BSV_HBK_ZERO_COUNT_ERR           (CC_BSV_BASE_ERROR + 0x00000008)
/*! Illegal LCS error. */
#define CC_BSV_ILLEGAL_LCS_ERR              (CC_BSV_BASE_ERROR + 0x00000009)
/*! OTP write compare failure error. */
#define CC_BSV_OTP_WRITE_CMP_FAIL_ERR           (CC_BSV_BASE_ERROR + 0x0000000A)
/*! Erase key in OTP failed error. */
#define CC_BSV_ERASE_KEY_FAILED_ERR         (CC_BSV_BASE_ERROR + 0x0000000B)
/*! Illegal PIDR error. */
#define CC_BSV_ILLEGAL_PIDR_ERR             (CC_BSV_BASE_ERROR + 0x0000000C)
/*! Illegal CIDR error. */
#define CC_BSV_ILLEGAL_CIDR_ERR             (CC_BSV_BASE_ERROR + 0x0000000D)
/*! Device failed to move to fatal error state. */
#define CC_BSV_FAILED_TO_SET_FATAL_ERR          (CC_BSV_BASE_ERROR + 0x0000000E)
/*! Failed to set RMA LCS error. */
#define CC_BSV_FAILED_TO_SET_RMA_ERR            (CC_BSV_BASE_ERROR + 0x0000000F)
/*! Illegal RMA indication error. */
#define CC_BSV_ILLEGAL_RMA_INDICATION_ERR       (CC_BSV_BASE_ERROR + 0x00000010)
/*! BSV version is not initialized error. */
#define CC_BSV_VER_IS_NOT_INITIALIZED_ERR       (CC_BSV_BASE_ERROR + 0x00000011)
/*! APB secure mode is locked error. */
#define CC_BSV_APB_SECURE_IS_LOCKED_ERR         (CC_BSV_BASE_ERROR + 0x00000012)
/*! APB privilege mode is locked error. */
#define CC_BSV_APB_PRIVILEG_IS_LOCKED_ERR       (CC_BSV_BASE_ERROR + 0x00000013)
/*! Illegal operation error. */
#define CC_BSV_ILLEGAL_OPERATION_ERR            (CC_BSV_BASE_ERROR + 0x00000014)
/*! Illegal asset size error. */
#define CC_BSV_ILLEGAL_ASSET_SIZE_ERR           (CC_BSV_BASE_ERROR + 0x00000015)
/*! Illegal asset value error. */
#define CC_BSV_ILLEGAL_ASSET_VAL_ERR            (CC_BSV_BASE_ERROR + 0x00000016)
/*! Kpicv is locked error. */
#define CC_BSV_KPICV_IS_LOCKED_ERR              (CC_BSV_BASE_ERROR + 0x00000017)
/*! Illegal SW version error. */
#define CC_BSV_ILLEGAL_SW_VERSION_ERR           (CC_BSV_BASE_ERROR + 0x00000018)
/*! AO write operation error. */
#define CC_BSV_AO_WRITE_FAILED_ERR              (CC_BSV_BASE_ERROR + 0x00000019)
/*! Device is locked in fatal error state. */
#define CC_BSV_FATAL_ERR_IS_LOCKED_ERR          (CC_BSV_BASE_ERROR + 0x0000001A)

/*! Illegal data in pointer error. */
#define CC_BSV_INVALID_DATA_IN_POINTER_ERROR        (CC_BSV_CRYPTO_ERROR + 0x00000001)
/*! Illegal data out pointer error. */
#define CC_BSV_INVALID_DATA_OUT_POINTER_ERROR       (CC_BSV_CRYPTO_ERROR + 0x00000002)
/*! Illegal data size error. */
#define CC_BSV_INVALID_DATA_SIZE_ERROR          (CC_BSV_CRYPTO_ERROR + 0x00000003)
/*! Illegal key type error. */
#define CC_BSV_INVALID_KEY_TYPE_ERROR           (CC_BSV_CRYPTO_ERROR + 0x00000004)
/*! Illegal key size error. */
#define CC_BSV_INVALID_KEY_SIZE_ERROR           (CC_BSV_CRYPTO_ERROR + 0x00000005)
/*! Illegal KDF label error. */
#define CC_BSV_ILLEGAL_KDF_LABEL_ERROR          (CC_BSV_CRYPTO_ERROR + 0x00000006)
/*! Illegal KDF context error. */
#define CC_BSV_ILLEGAL_KDF_CONTEXT_ERROR        (CC_BSV_CRYPTO_ERROR + 0x00000007)
/*! Invalid CCM key error. */
#define CC_BSV_CCM_INVALID_KEY_ERROR            (CC_BSV_CRYPTO_ERROR + 0x00000008)
/*! Invalid CCM Nonce error. */
#define CC_BSV_CCM_INVALID_NONCE_ERROR          (CC_BSV_CRYPTO_ERROR + 0x00000009)
/*! Invalid CCM associated data error. */
#define CC_BSV_CCM_INVALID_ASSOC_DATA_ERROR     (CC_BSV_CRYPTO_ERROR + 0x0000000A)
/*! Invalid CCM text data error. */
#define CC_BSV_CCM_INVALID_TEXT_DATA_ERROR      (CC_BSV_CRYPTO_ERROR + 0x0000000B)
/*! Invalid CCM-MAC buffer error. */
#define CC_BSV_CCM_INVALID_MAC_BUF_ERROR        (CC_BSV_CRYPTO_ERROR + 0x0000000C)
/*! Output and input data are overlapping error. */
#define CC_BSV_CCM_DATA_OUT_DATA_IN_OVERLAP_ERROR   (CC_BSV_CRYPTO_ERROR + 0x0000000D)
/*! CCM-MAC comparison failed error. */
#define CC_BSV_CCM_MAC_INVALID_ERROR            (CC_BSV_CRYPTO_ERROR + 0x0000000E)
/*! Invalid CCM mode error. */
#define CC_BSV_CCM_INVALID_MODE_ERROR           (CC_BSV_CRYPTO_ERROR + 0x0000000F)
/*! Invalid out pointer error. */
#define CC_BSV_INVALID_OUT_POINTER_ERROR        (CC_BSV_CRYPTO_ERROR + 0x00000010)
/*! Illegal cryptographic mode error. */
#define CC_BSV_INVALID_CRYPTO_MODE_ERROR        (CC_BSV_CRYPTO_ERROR + 0x00000011)
/*! Illegal IV pointer error. */
#define CC_BSV_INVALID_IV_POINTER_ERROR         (CC_BSV_CRYPTO_ERROR + 0x00000012)
/*! Illegal result buffer pointer error. */
#define CC_BSV_INVALID_RESULT_BUFFER_POINTER_ERROR  (CC_BSV_CRYPTO_ERROR + 0x00000013)

#ifdef __cplusplus
}
#endif

#endif



