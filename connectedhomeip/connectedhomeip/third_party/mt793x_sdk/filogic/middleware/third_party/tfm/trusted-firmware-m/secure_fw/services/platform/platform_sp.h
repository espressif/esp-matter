/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_SP_H__
#define __PLATFORM_SP_H__

#include "tfm_platform_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Initializes the secure partition.
 *
 * \return Returns values as specified by the \ref tfm_platform_err_t
 */
enum tfm_platform_err_t platform_sp_init(void);

/*!
 * \brief Resets the system.
 *
 * \return Returns values as specified by the \ref tfm_platform_err_t
 */
enum tfm_platform_err_t platform_sp_system_reset(void);

/*!
 * \brief register the RTOS slp for TFM reference
 */
enum tfm_platform_err_t platform_slp_internal_sys_init(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

/*!
 * \brief register backup and restore callbacks of sys modules in TFM
 */
void platform_slp_platform_sys_init(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

/*!
 * \brief perform the module CONN BUS suspend flow
 */
void platform_slp_CONN_BUS_backup(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

/*!
 * \brief The platform enter deep sleep & wakeup.
 */
uint32_t platform_sp_system_sleep_wakeup(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

/*!
 * \brief Conninfra bus suspend callback
 */
enum tfm_platform_err_t platform_slp_conn_bus_suspend(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

/*!
 * \brief Conninfra bus resume callback
 */
enum tfm_platform_err_t platform_slp_conn_bus_resume(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

/*!
 * \brief Enter WFI sys part
 */
enum tfm_platform_err_t platform_slp_enter_wfi_sys(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

/*!
 * \brief resource module lock SPM to avoid enter deep sleep
 */
enum tfm_platform_err_t platform_spm_spm_suspend_module_lock(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

/*!
 * \brief control spm module
 */
enum tfm_platform_err_t platform_spm_spm_ctrl(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

/*!
 * \brief module power on/off
 */
enum tfm_platform_err_t platform_spm_spm_mtcmos(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

/*!
 * \brief spm module initialization
 */
enum tfm_platform_err_t platform_spm_hal_spm_init(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

/*!
 * \brief Performs pin services of the platform
 *
 * \param[in]     in_vec     Pointer to in_vec array, which contains input
 *                           arguments for the pin service
 * \param[in]     num_invec  Number of elements in in_vec array
 * \param[in,out] out_vec    Pointer out_vec array, which contains output data
 *                           of the pin service
 * \param[in]     num_outvec Number of elements in out_vec array
 *
 * \return Returns values as specified by the \ref tfm_platform_err_t
 */
enum tfm_platform_err_t
platform_sp_pin_service(const psa_invec  *in_vec,  uint32_t num_invec,
                        const psa_outvec *out_vec, uint32_t num_outvec);

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_SP_H__ */
