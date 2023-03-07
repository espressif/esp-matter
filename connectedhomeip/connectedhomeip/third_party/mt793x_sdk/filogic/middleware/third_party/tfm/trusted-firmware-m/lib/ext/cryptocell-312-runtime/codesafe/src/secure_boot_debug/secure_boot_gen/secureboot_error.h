/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _SECUREBOOT_ERROR_H
#define _SECUREBOOT_ERROR_H

/*! @file
@brief This file defines the error code types returned from the secure boot code.
*/

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/
/*! Defines the error number space used for the Secure Boot different modules. */
#define CC_SECUREBOOT_BASE_ERROR                 0xF0000000

/*! Defines the secure boot's base layer's error number. */
#define CC_SECUREBOOT_LAYER_BASE_ERROR           0x01000000

/*! Defines the Secure boot's verifier layer's error prefix number. */
#define CC_SB_VERIFIER_LAYER_PREFIX         1
/*! Defines the Secure boot's driver layer's error prefix number. */
#define CC_SB_DRV_LAYER_PREFIX          2
/*! Defines the Secure boot's revocation layer's error prefix number. */
#define CC_SB_SW_REVOCATION_LAYER_PREFIX    3
/*! Defines the Secure boot's HAL layer's error prefix number. */
#define CC_SB_HAL_LAYER_PREFIX                  6
/*! Defines the Secure boot's RSA layer's error prefix number. */
#define CC_SB_RSA_LAYER_PREFIX              7
/*! Defines the Secure boot's certificate verifier layer's error prefix number. */
#define CC_SB_VERIFIER_CERT_LAYER_PREFIX    8
/*! Defines the Secure boot's X509 certificate layer's error prefix number. */
#define CC_SB_X509_CERT_LAYER_PREFIX        9


/*! Defines the boot images verifier base error = 0xF1000000. */
#define CC_BOOT_IMG_VERIFIER_BASE_ERROR          (CC_SECUREBOOT_BASE_ERROR + CC_SB_VERIFIER_LAYER_PREFIX*CC_SECUREBOOT_LAYER_BASE_ERROR)
/*! Defines the NVM base error = 0xF4000000. */
#define CC_SB_HAL_BASE_ERROR                     (CC_SECUREBOOT_BASE_ERROR + CC_SB_HAL_LAYER_PREFIX*CC_SECUREBOOT_LAYER_BASE_ERROR)
/*! Defines the RSA's base error = 0xF7000000. */
#define CC_SB_RSA_BASE_ERROR                     (CC_SECUREBOOT_BASE_ERROR + CC_SB_RSA_LAYER_PREFIX*CC_SECUREBOOT_LAYER_BASE_ERROR)

/*! Defines the boot images verifier certificates base error = 0xF8000000. */
#define CC_BOOT_IMG_VERIFIER_CERT_BASE_ERROR     (CC_SECUREBOOT_BASE_ERROR + CC_SB_VERIFIER_CERT_LAYER_PREFIX*CC_SECUREBOOT_LAYER_BASE_ERROR)

/*! Defines the X.509's base error = 0xF9000000. */
#define CC_SB_X509_CERT_BASE_ERROR               (CC_SECUREBOOT_BASE_ERROR + CC_SB_X509_CERT_LAYER_PREFIX*CC_SECUREBOOT_LAYER_BASE_ERROR)

/*! Defines the cryptographic driver base error = 0xF2000000. */
#define CC_SB_DRV_BASE_ERROR             (CC_SECUREBOOT_BASE_ERROR + CC_SB_DRV_LAYER_PREFIX*CC_SECUREBOOT_LAYER_BASE_ERROR)

/*! Defines a HAL fatal error. */
#define CC_SB_HAL_FATAL_ERROR_ERR            (CC_SB_HAL_BASE_ERROR + 0x00000001)
/*! Illegal input error. */
#define CC_SB_DRV_ILLEGAL_INPUT_ERR      (CC_SB_DRV_BASE_ERROR + 0x00000001)
/*! Illegal key error. */
#define CC_SB_DRV_ILLEGAL_KEY_ERR        (CC_SB_DRV_BASE_ERROR + 0x00000002)
/*! Illegal size error. */
#define CC_SB_DRV_ILLEGAL_SIZE_ERR       (CC_SB_DRV_BASE_ERROR + 0x00000003)


#ifdef __cplusplus
}
#endif

#endif

