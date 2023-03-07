/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_management
 @{
 */

/*!
 @file
 @brief This file contains all CryptoCell Management APIs and definitions.

 The following terms, used throughout this module, are defined in
 <em>Arm® v8-M Architecture Reference Manual</em>:
 <ul><li>Privileged and unprivileged modes.</li>
 <li>Secure and Non-secure modes.</li></ul>
 */



#ifndef _MBEDTLS_CC_MNG_H
#define _MBEDTLS_CC_MNG_H

/* *********************** Includes ***************************** */
#include "cc_pal_types_plat.h"

#ifdef __cplusplus
extern "C"
{
#endif


/* *********************** Defines ***************************** */
/* *********************** Macros ***************************** */


/* *********************** Enums ***************************** */
/*! RMA statuses. */
typedef enum  {
    /*! Non-RMA: bit [30] = 0, bit [31] = 0. */
    CC_MNG_NON_RMA              = 0,
    /*! Pending RMA: bit [30] = 1, bit [31] = 0. */
    CC_MNG_PENDING_RMA          = 1,
    /*! Illegal state: bit [30] = 0, bit [31] = 1. */
    CC_MNG_ILLEGAL_STATE        = 2,
    /*! RMA: bit [30] = 1, bit [31] = 1. */
    CC_MNG_RMA                  = 3,
    /*! Reserved. */
    CC_MNG_END_OF_RMA_STATUS    = 0x7FFFFFFF
}mbedtls_mng_rmastatus;

/*! AES HW key types. */
typedef enum  {
    /*! Device root key (HUK). */
    CC_MNG_HUK_KEY         = 0,
    /*! Platform key (Krtl). */
    CC_MNG_RTL_KEY         = 1,
    /*! ICV provisioning key (Kcp). */
    CC_MNG_PROV_KEY        = 2,
    /*! OEM code-encryption key (Kce). */
    CC_MNG_CE_KEY          = 3,
    /*! OEM provisioning key (Kpicv). */
    CC_MNG_ICV_PROV_KEY    = 4,
    /*! ICV code-encryption key (Kceicv). */
    CC_MNG_ICV_CE_KEY      = 5,
    /*! Total number of HW Keys. */
    CC_MNG_TOTAL_HW_KEYS   = 6,
    /*! Reserved. */
    CC_MNG_END_OF_KEY_TYPE = 0x7FFFFFFF
}mbedtls_mng_keytype;

/*! APB-C only part IDs. */
typedef enum  {
    /*! Secure accesses. */
    CC_MNG_APBC_SEC_ID      = 0,
    /*! Privileged accesses. */
    CC_MNG_APBC_PRIV_ID     = 1,
    /*! Instruction accesses. */
    CC_MNG_APBC_INST_ID     = 2,
    /*! Total part IDs. */
    CC_MNG_APBC_TOTAL_ID    = 3,
    /*! Reserved. */
    CC_MNG_APBC_END_OF_ID   = 0x7FFFFFFF
}mbedtls_mng_apbc_parts;

/*! APB-C part configuration. */
typedef enum  {
    /*! Use APB-C as an input when there is no need to change bits.
    Modify bit = 0. */
    CC_MNG_APBC_NO_CHANGE           = 0,
    /*! Use APB-C as an input when you need to set the 'Allow' bit to '0' and
    leave this part unlocked. Modify bit = 1, Allow bit = 0, Allow Lock
    bit = 0. */
    CC_MNG_APBC_ALLOW_0_ALLOWLOCK_0 = 1,
    /*! Use APB-C as an input when you need to set the 'Allow' bit to '0' and
    lock this part. Modify bit = 1, Allow bit = 0, Allow Lock bit = 1. */
    CC_MNG_APBC_ALLOW_0_ALLOWLOCK_1 = 2,
    /*! Use APB-C as an input when you need to set the 'Allow' bit to '1' and
    leave this part unlocked. Modify bit = 1, Allow bit = 1, Allow Lock
    bit = 0. */
    CC_MNG_APBC_ALLOW_1_ALLOWLOCK_0 = 3,
    /*! Use APB-C as an input when you need to set the 'Allow' bit to '1' and
    lock this part. Modify bit = 1, Allow bit = 1, Allow Lock bit = 1. */
    CC_MNG_APBC_ALLOW_1_ALLOWLOCK_1 = 4,
    /*! Total parts. */
    CC_MNG_APBC_TOTAL_PARTS_CONFIG  = 5,
    /*! Reserved. */
    CC_MNG_APBC_END_OF_PARTS_CONFIG = 0x7FFFFFFF
}mbedtls_mng_apbc_parts_config;

/************************ Typedefs  ****************************/

/*! A uint8_t representation for the APB-C parts in the AO_APB_FILTERING
register. */
typedef union mbedtls_mng_apbc_part{
    /*! A representation of the APB-C value in the AO_APB_FILTERING register.*/
    uint8_t apbcPartVal;
    /*! A representation of the APB-C parts in the AO_APB_FILTERING register.*/
    struct {
        /*! APB-C accepts only 'mbedtls_mng_apbc_parts' accesses. */
        uint8_t accessAllow       :   1;
        /*! APB-C \p accessAllow cannot be modified. */
        uint8_t accessAllowLock   :   1;
        /*! User decided to modify the upper couple. */
        uint8_t accessModify      :   1;
        /*! APB-C part access bits. */
        uint8_t rfu               :   5;
    }apbcPartBits;
}mbedtls_mng_apbc_part;

/*! Input to the mbedtls_mng_apbc_config_set() function. */
typedef union mbedtls_mng_apbcconfig{
    /*! APB-C configuration values. */
    uint32_t apbcConfigVal;
    /*! An array of the configuration bits for the Secure, Privileged, and
    Instruction parts. */
    mbedtls_mng_apbc_part apbcPart[CC_MNG_APBC_TOTAL_ID + 1];
}mbedtls_mng_apbcconfig;


/* ****************************************** Public Functions **************************************** */
/*
Management APIs enable to set, get or obtain device status by reading or writing the
appropriate registers or the OTP.
*/
/* ********************************************************************************************** */
/*!
  @brief This function reads the OTP word of the OEM flags,
         and returns the OEM RMA flag status: TRUE or FALSE.

  The function returns the value only in DM LCS or Secure LCS.
  It validates the device RoT configuration, and returns the
  value only if both HBK0 and HBK1 are supported.
  Otherwise, it returns FALSE regardless of the OTP status.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_pending_rma_status_get(
        /*! [out] The RMA status. */
        uint32_t *rmaStatus
                                      );

/*!
  @brief This function verifies and returns the CryptoCell HW version.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_hw_version_get(
        /*! [out] The part number. */
        uint32_t *partNumber,
        /*! [out] The HW version. */
        uint32_t *revision
);

/*!
  @brief This function sets CryptoCell to Secured mode.

  Setting CryptoCell to Secured mode can only be done while CryptoCell is idle.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_cc_sec_mode_set(
        /*! [in] True: Set CryptoCell to Secured mode. False: Set CryptoCell
        to non-Secured mode. */
        CCBool_t isSecAccessMode,
        /*! [in] True: Lock CryptoCell to current mode. False: Do not lock
        CryptoCell to current mode. Allows calling this function again. */
        CCBool_t isSecModeLock
);

/*!
  @brief This function sets CryptoCell to Privileged mode.

  Setting CryptoCell to Privileged mode can only be done while CryptoCell is idle.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_cc_priv_mode_set(
        /*! [in] True: Set CryptoCell to privileged mode. False: Set
        CryptoCell to unprivileged mode. */
        CCBool_t isPrivAccessMode,
        /*! [in] True: Lock CryptoCell to current mode. False: Do not lock
        CryptoCell to current mode. Allows calling this function again. */
        CCBool_t isPrivModeLock
);

/*!
  @brief This function sets the shadow register of one of the
  HW Keys when the device is in CM LCS or DM LCS.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_debug_key_set(
        /*! [in] The type of the HW key. One of the following values: HUK,
        Kcp, Kce, Kpicv, or Kceicv. */
        mbedtls_mng_keytype keyType,
        /*! [in] A pointer to the buffer holding the HW key. */
        uint32_t *pHwKey,
        /*! [in] The size of the HW key in bytes. */
        size_t keySize
);

/*!
  @brief This function retrieves the general configuration from the OTP.
         See <em>Arm CryptoCell-312 Software Integrators Manual</em>.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_gen_config_get(
        /*! [out] The OTP configuration word. */
        uint32_t *pOtpWord
                               );

/*!
  @brief This function locks the usage of either Kcp, Kce, or both during runtime,
         in either Secure LCS or RMA LCS.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_oem_key_lock(
        /*! [in] The flag for locking Kcp usage. */
        CCBool_t kcpLock,
        /*! [in] The flag for locking Kce usage. */
        CCBool_t kceLock
);

/*!
  @brief This function sets CryptoCell APB-C into one of the following modes:
         Secured access mode, Privileged access mode, or Instruction access
         mode.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_apbc_config_set(
        /*! Secured access mode. */
        mbedtls_mng_apbc_parts_config securePartCfg,
        /*! Privileged access mode.*/
        mbedtls_mng_apbc_parts_config privPartCfg,
        /*! Instruction access mode. */
        mbedtls_mng_apbc_parts_config instPartCfg
);
/*!
  @brief This function requests usage of, or releases, the APB-C.

  @note This function must be called before and after each use of APB-C.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_apbc_access(
        /*! [in] TRUE: Request usage of APB-C. FALSE: Free APB-C. */
        CCBool_t isApbcAccessUsed
                           );

/*!
  @brief This function is called once the external PMU decides to power-down
  CryptoCell.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_suspend(
        /*! [in] A pointer to a buffer that can be used for backup. */
        uint8_t *pBackupBuffer,
        /*! [in] The size of the backup buffer. Must be at least
        \c CC_MNG_MIN_BACKUP_SIZE_IN_BYTES. */
        size_t backupSize
);

/*!
  @brief This function is called once the external PMU decides to power-up
  CryptoCell.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_resume(
        /*! [in] A pointer to a buffer that can be used for backup. */
        uint8_t *pBackupBuffer,
        /*! [in] The size of the backup buffer. Must be at least
        \c CC_MNG_MIN_BACKUP_SIZE_IN_BYTES. */
        size_t backupSize
);
#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif // _MBEDTLS_CC_MNG_H

