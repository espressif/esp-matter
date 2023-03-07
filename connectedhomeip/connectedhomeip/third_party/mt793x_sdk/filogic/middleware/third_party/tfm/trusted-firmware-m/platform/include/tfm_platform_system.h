/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PLATFORM_SYSTEM_H__
#define __TFM_PLATFORM_SYSTEM_H__
/**
 * \note The interfaces defined in this file must be implemented for each
 *       target.
 */

#include "psa/client.h"
#include "tfm_plat_defs.h"
#include "tfm_platform_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Resets the system.
 *
 * \details Requests a system reset to reset the MCU.
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_hal_system_reset(void);

/*!
 * \brief Performs a platform-specific service
 *
 * \param[in]  request      Request identifier (valid values vary
 *                          based on the platform)
 * \param[in]  in_vec       Input buffer to the requested service (or NULL)
 * \param[out] out_vec      Output buffer to the requested service (or NULL)
 *
 * \return Returns values as specified by the \ref tfm_platform_err_t
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
enum tfm_platform_err_t tfm_platform_hal_ioctl(tfm_platform_ioctl_req_t request,
                                               psa_invec *in_vec,
                                               psa_outvec *out_vec);

/**
 * \brief Resume the tfm
 *
 * \details Restore the tfm environment after platform wakeup
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_hal_resume(void *data);

/**
 * \brief Set the slp from NS to the global structure
 *
 * \details Set the slp addr from NS to get the NS status
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_hal_slp_internal_init(void *slp);

/**
 * \brief Register suspend & resume callback to TFM
 *
 * \details Register backup & restore callback of SYS modules to TFM
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_hal_slp_platform_sys_init(void);

/**
 * \brief Suspend SYS module CONN BUS
 *
 * \details SYS module CONN BUS performs backup
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_hal_slp_suspend_CONN_BUS(void);

/**
 * \brief TFM suspend and resume main flow
 *
 * \details TFM suspend and resume main flow
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
uint32_t tfm_platform_hal_slp_sleep_wakeup(void);

/**
 * \brief callback function of conninfra suspend
 *
 * \details conninfra suspend callback
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_slp_conn_bus_suspend(void);

/**
 * \brief callback function of conninfra resume
 *
 * \details conninfra resume callback
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_slp_conn_bus_resume(void);

/**
 * \brief System enter WFI mode
 *
 * \details Platform enter wfi
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_slp_enter_wfi_sys(void);

/**
 * \brief resource module lock SPM to avoid enter deep sleep
 *
 * \details set SPM lock to avoid platform sleep
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_spm_spm_suspend_module_lock(const void* tfm_mod, const void* tfm_lock);

/**
 * \brief control spm module
 *
 * \details control spm module
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_spm_spm_ctrl(const void* tfm_ctrl);

/**
 * \brief module power on/off
 *
 * \details module power on/off
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_spm_spm_mtcmos(const void* tfm_mod, const void* tfm_onoff, const void* tfm_chain_no);

/**
 * \brief spm module initialization
 *
 * \details spm module initialization
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_spm_hal_spm_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PLATFORM_SYSTEM_H__ */
