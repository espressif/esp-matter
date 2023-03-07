/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PLATFORM_API__
#define __TFM_PLATFORM_API__

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "tfm_api.h"
#include "hal_spm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief TFM secure partition platform API version
 */
#define TFM_PLATFORM_API_VERSION_MAJOR (0)
#define TFM_PLATFORM_API_VERSION_MINOR (3)

#define TFM_PLATFORM_API_ID_NV_READ       (1010)
#define TFM_PLATFORM_API_ID_NV_INCREMENT  (1011)

/*!
 * \enum tfm_platform_err_t
 *
 * \brief Platform service error types
 *
 */
enum tfm_platform_err_t {
    TFM_PLATFORM_ERR_SUCCESS = 0,
    TFM_PLATFORM_ERR_SYSTEM_ERROR,
    TFM_PLATFORM_ERR_INVALID_PARAM,
    TFM_PLATFORM_ERR_NOT_SUPPORTED,

    /* Following entry is only to ensure the error code of int size */
    TFM_PLATFORM_ERR_FORCE_INT_SIZE = INT_MAX
};

typedef int32_t tfm_platform_ioctl_req_t;

/*!
 * \brief Resets the system.
 *
 * \return Returns values as specified by the \ref tfm_platform_err_t
 */
enum tfm_platform_err_t tfm_platform_system_reset(void);

/*!
 * \brief Performs a platform-specific service
 *
 * \param[in]  request      Request identifier (valid values vary
 *                          based on the platform)
 * \param[in]  input        Input buffer to the requested service (or NULL)
 * \param[in,out] output    Output buffer to the requested service (or NULL)
 *
 * \return Returns values as specified by the \ref tfm_platform_err_t
 */
enum tfm_platform_err_t tfm_platform_ioctl(tfm_platform_ioctl_req_t request,
                                           psa_invec *input,
                                           psa_outvec *output);

/*!
 * \brief register the RTOS slp for TFM reference.
 */
void tfm_sleep_management_internal_sys_init(void *slp);

/*!
 * \brief register backup and restore callbacks of sys modules in TFM.
 */
void tfm_sleep_management_platform_sys_init(void);

/*!
 *  * \brief backup SLEEP_BACKUP_RESTORE_SYS_CONN_BUS
 *   */
void tfm_suspend_callback_SYS_CONN_BUS(void *data);

/*!
 * \brief The platform enter deep sleep & wakeup.
 */
uint32_t tfm_sleep_management_enter_deep_sleep_sys(void);

/*!
 * \brief conninfra bus suspend callback.
 */
void tfm_slp_conn_bus_suspend(void *data);

/*!
 * \brief conninfra bus resume callback.
 */
void tfm_slp_conn_bus_resume(void *data);

/*!
 * \brief enter wfi sys
 */
void tfm_sleep_management_enter_wfi_sys(void);

/*!
 * \brief resource module lock SPM to avoid enter deep sleep
 */
void tfm_spm_suspend_module_lock(spm_suspend_lock_module_t module, bool lock);

/*!
 * \brief control spm module
 */
void tfm_spm_ctrl(spm_ctrl_t ctrl);

/*!
 * \brief module power on/off
 */
void tfm_spm_mtcmos(spm_module_t module, spm_onoff_t onoff, unsigned int chain_no);

/*!
 * \brief spm module initialization
 */
void tfm_hal_spm_init(void);

/*!
 * \brief Increments the given non-volatile (NV) counter by one
 *
 * \param[in]  counter_id  NV counter ID.
 *
 * \return  TFM_PLATFORM_ERR_SUCCESS if the value is read correctly. Otherwise,
 *          it returns TFM_PLATFORM_ERR_SYSTEM_ERROR.
 */
enum tfm_platform_err_t
tfm_platform_nv_counter_increment(uint32_t counter_id);

/*!
 * \brief Reads the given non-volatile (NV) counter
 *
 * \param[in]  counter_id  NV counter ID.
 * \param[in]  size        Size of the buffer to store NV counter value
 *                         in bytes.
 * \param[out] val         Pointer to store the current NV counter value.
 *
 * \return  TFM_PLATFORM_ERR_SUCCESS if the value is read correctly. Otherwise,
 *          it returns TFM_PLATFORM_ERR_SYSTEM_ERROR.
 */
enum tfm_platform_err_t
tfm_platform_nv_counter_read(uint32_t counter_id,
                             uint32_t size, uint8_t *val);

/*!
 * \brief Encrypts plaintext with AES-ECB by GCPU
 *
 * \param[out] out_EncTxt  Pointer to store output encrypted data.
 *
 * \param[in]  in_PlnTxt   Pointer to input plaintext data
 * \param[in]  in_Key      Pointer to encryption key data
 *
 * \return  PSA_SUCCESS if the operation is read correctly. Otherwise,
 *          it returns error number.
 */
int32_t tfm_aes_ecb_encrypt(void *out_EncTxt, void *in_PlnTxt, void *in_Key);

/*!
 * \brief Generates random number with mbedtls by TRNG
 *
 * \param[in]  ctr_drbg    Pointer to DRBG context
 *
 * \param[out] output      Pointer to store output random data.
 * \param[out] output_len  Pointer to store output random data length.
 *
 * \return  PSA_SUCCESS if the operation is read correctly. Otherwise,
 *          it returns error number.
 */
int tfm_generate_random(void *ctr_drbg, unsigned char *output, size_t output_len);
#ifdef __cplusplus
}
#endif

#endif /* __TFM_PLATFORM_API__ */
