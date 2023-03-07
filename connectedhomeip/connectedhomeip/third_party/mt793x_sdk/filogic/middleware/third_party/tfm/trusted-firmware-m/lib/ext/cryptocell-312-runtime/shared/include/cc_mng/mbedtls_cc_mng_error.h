/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/*!
 @addtogroup cc_management_error
 @{
*/

/*!
 @file
 @brief This file contains the error definitions of the CryptoCell management APIs.
 */


#ifndef _MBEDTLS_CC_MNG_ERROR_H
#define _MBEDTLS_CC_MNG_ERROR_H

#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/* CryptoCell Management module errors. CC_MNG_MODULE_ERROR_BASE = 0x00F02900 */

/*! Illegal input parameter. */
#define CC_MNG_ILLEGAL_INPUT_PARAM_ERR      (CC_MNG_MODULE_ERROR_BASE + 0x00UL)
/*! Illegal operation. */
#define CC_MNG_ILLEGAL_OPERATION_ERR        (CC_MNG_MODULE_ERROR_BASE + 0x01UL)
/*! Illegal Peripheral ID. */
#define CC_MNG_ILLEGAL_PIDR_ERR             (CC_MNG_MODULE_ERROR_BASE + 0x02UL)
/*! Illegal Component ID. */
#define CC_MNG_ILLEGAL_CIDR_ERR             (CC_MNG_MODULE_ERROR_BASE + 0x03UL)
/*! APB Secure is locked. */
#define CC_MNG_APB_SECURE_IS_LOCKED_ERR         (CC_MNG_MODULE_ERROR_BASE + 0x04UL)
/*! APB Privilege is locked. */
#define CC_MNG_APB_PRIVILEGE_IS_LOCKED_ERR  (CC_MNG_MODULE_ERROR_BASE + 0x05UL)
/*! APBC Secure is locked. */
#define CC_MNG_APBC_SECURE_IS_LOCKED_ERR    (CC_MNG_MODULE_ERROR_BASE + 0x06UL)
/*! APBC Privilege is locked. */
#define CC_MNG_APBC_PRIVILEGE_IS_LOCKED_ERR (CC_MNG_MODULE_ERROR_BASE + 0x07UL)
/*! APBC Instruction is locked. */
#define CC_MNG_APBC_INSTRUCTION_IS_LOCKED_ERR   (CC_MNG_MODULE_ERROR_BASE + 0x08UL)
/*! Invalid Key type. */
#define CC_MNG_INVALID_KEY_TYPE_ERROR       (CC_MNG_MODULE_ERROR_BASE + 0x09UL)
/*! Illegal size of HUK. */
#define CC_MNG_ILLEGAL_HUK_SIZE_ERR     (CC_MNG_MODULE_ERROR_BASE + 0x0AUL)
/*! Illegal size for any HW key other than HUK. */
#define CC_MNG_ILLEGAL_HW_KEY_SIZE_ERR      (CC_MNG_MODULE_ERROR_BASE + 0x0BUL)
/*! HW key is locked. */
#define CC_MNG_HW_KEY_IS_LOCKED_ERR         (CC_MNG_MODULE_ERROR_BASE + 0x0CUL)
/*! Kcp is locked. */
#define CC_MNG_KCP_IS_LOCKED_ERR            (CC_MNG_MODULE_ERROR_BASE + 0x0DUL)
/*! Kce is locked. */
#define CC_MNG_KCE_IS_LOCKED_ERR        (CC_MNG_MODULE_ERROR_BASE + 0x0EUL)
/*! RMA Illegal state. */
#define CC_MNG_RMA_ILLEGAL_STATE_ERR        (CC_MNG_MODULE_ERROR_BASE + 0x0FUL)
/*! Error returned from AO_APB_FILTERING write operation. */
#define CC_MNG_AO_APB_WRITE_FAILED_ERR      (CC_MNG_MODULE_ERROR_BASE + 0x10UL)
/*! APBC access failure. */
#define CC_MNG_APBC_ACCESS_FAILED_ERR       (CC_MNG_MODULE_ERROR_BASE + 0x11UL)
/*! APBC already-off failure. */
#define CC_MNG_APBC_ACCESS_ALREADY_OFF_ERR  (CC_MNG_MODULE_ERROR_BASE + 0x12UL)
/*! APBC access is on failure. */
#define CC_MNG_APBC_ACCESS_IS_ON_ERR        (CC_MNG_MODULE_ERROR_BASE + 0x13UL)
/*! PM SUSPEND/RESUME failure. */
#define CC_MNG_PM_SUSPEND_RESUME_FAILED_ERR (CC_MNG_MODULE_ERROR_BASE + 0x14UL)
/*! SW version failure. */
#define CC_MNG_ILLEGAL_SW_VERSION_ERR       (CC_MNG_MODULE_ERROR_BASE + 0x15UL)
/*! Hash Public Key NA. */
#define CC_MNG_HASH_NOT_PROGRAMMED_ERR      (CC_MNG_MODULE_ERROR_BASE + 0x16UL)
/*! Illegal hash boot key zero count in the OTP error. */
#define CC_MNG_HBK_ZERO_COUNT_ERR           (CC_MNG_MODULE_ERROR_BASE + 0x17UL)

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/

#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif // _MBEDTLS_CC_MNG_ERROR_H

