/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "platform/include/tfm_platform_system.h"
#include "cmsis.h"
#include "platform/include/tfm_spm_hal.h"
#include "log/tfm_log.h"
#include "services/include/tfm_ioctl_api.h"
#include "tfm_arch.h"
#include "target_cfg.h"
#include "hal_security_api.h"
#include "log/tfm_assert.h"
#include "hal_ls_api.h"
#include "hal_sleep_manager_internal.h"
#include "hal_gpt.h"
#ifdef TFM_ENABLE_PLATFORM_ISOLATION
#include "hal_devapc.h"
#include "pltfm_iso.h"
#endif /* ifdef TFM_ENABLE_PLATFORM_ISOLATION */

enum tfm_platform_err_t
tfm_platform_hal_basic_request(const psa_invec  *in_vec,
                              const psa_outvec *out_vec)
{
    tfm_basic_request_args_t *args;
    tfm_basic_request_out_t *out;

    if (in_vec->len != sizeof(tfm_basic_request_args_t) ||
        out_vec->len != sizeof(tfm_basic_request_out_t)) {
        return TFM_PLATFORM_ERR_INVALID_PARAM;
    }

    args = (tfm_basic_request_args_t *)in_vec->base;
    out = ( tfm_basic_request_out_t *)out_vec->base;
    switch (args->type) {
    case TFM_PLATFORM_IOCTL_BASIC_REQUEST_TYPE_INIT:
        LOG_MSG("basic request init %x\r\n", args->u.basic_config.data);
        out->u.result = BASIC_REQUEST_ERR_NONE;
        out->u.basic_read_result.data = 0x7933aa55;
        break;
    case TFM_PLATFORM_IOCTL_BASIC_REQUEST_TYPE_CONFIG:
        out->u.result = BASIC_REQUEST_ERR_NONE;
        break;
    default:
        out->u.result = BASIC_REQUEST_ERR_INVALID_ARG;
        break;
    }

    return TFM_PLATFORM_ERR_SUCCESS;
}

void tfm_platform_hal_system_reset(void)
{
    /* Reset the system */
    NVIC_SystemReset();
}

enum tfm_platform_err_t tfm_platform_hal_ioctl(tfm_platform_ioctl_req_t request,
                                               psa_invec  *in_vec,
                                               psa_outvec *out_vec)
{
    (void)request;
    (void)in_vec;
    (void)out_vec;
    switch (request){
    case TFM_PLATFORM_IOCTL_BASIC_REQUEST:
        return tfm_platform_hal_basic_request(in_vec, out_vec);
    case TFM_PLATFORM_IOCTL_POWER_REQUEST:
        //return tfm_platform_hal_power_service(in_vec, out_vec);
    default:
        LOG_MSG("Invalid ioctl service\r\n");
        return TFM_PLATFORM_ERR_NOT_SUPPORTED;
    }
    /* Not needed for this platform */
    return TFM_PLATFORM_ERR_SUCCESS;
}

void tfm_platform_hal_resume(void *data)
{
    //devide the RTOS and TFM irq
    tfm_arch_prioritize_secure_exception();

    hal_security_resume(NULL);

    mpc_init_cfg();

    sau_and_idau_cfg();
}

extern void sleep_management_tfm_inetrnal_init(void *slp_ns_ptr);
extern void sleep_management_suspend_callback(sleep_management_backup_restore_module_t module);

void tfm_platform_hal_slp_internal_init(void *slp)
{
    sleep_management_tfm_inetrnal_init(slp);
}

void tfm_platform_hal_slp_platform_sys_init(void)
{
    sleep_management_platform_sys_init();
}

void tfm_platform_hal_slp_suspend_CONN_BUS(void)
{
    sleep_management_suspend_callback(SLEEP_BACKUP_RESTORE_SYS_CONNINFRA);
}

uint32_t tfm_platform_hal_slp_sleep_wakeup(void)
{
    uint32_t ret = 0;
    uint32_t irq_mask = __get_PRIMASK();
    __disable_irq();

    __asm volatile("mrs r3, control");
    __asm volatile("bic r3, r3, #0x2");
    __asm volatile("msr control, r3");

    ret = sleep_management_enter_deep_sleep_sys();

    __asm volatile("mrs r3, control");
    __asm volatile("orr r3, r3, #0x2");
    __asm volatile("msr control, r3");

    hal_gpt_stop_timer(DEEP_SLEEP_GPT);
    if (!irq_mask) {
        __enable_irq();
    }

    return ret;
}

extern void slp_conn_bus_suspend(void);
extern void slp_conn_bus_resume(void);
void tfm_platform_slp_conn_bus_suspend(void)
{
    slp_conn_bus_suspend();
}

void tfm_platform_slp_conn_bus_resume(void)
{
    slp_conn_bus_resume();
}

void tfm_platform_slp_enter_wfi_sys(void)
{
    sleep_management_enter_wfi_sys();
}

void tfm_platform_spm_spm_suspend_module_lock(const void* tfm_mod, const void* tfm_lock)
{
    spm_suspend_lock_module_t module = *(spm_suspend_lock_module_t*)tfm_mod;
    bool lock = *(bool*)tfm_lock;

    spm_suspend_module_lock(module, lock);
}

void tfm_platform_spm_spm_ctrl(const void* tfm_ctrl)
{
    spm_ctrl_t ctrl = *(spm_ctrl_t*)tfm_ctrl;

    return spm_ctrl(ctrl);
}

void tfm_platform_spm_spm_mtcmos(const void* tfm_mod, const void* tfm_onoff, const void* tfm_chain_no)
{
    spm_mtcmos_module_t module = *(spm_suspend_lock_module_t*)tfm_mod;
    spm_onoff_t onoff = *(spm_onoff_t*)tfm_onoff;
    unsigned int chain_no = *(unsigned int*)tfm_chain_no;

    spm_mtcmos(module, onoff, chain_no);
}

void tfm_platform_spm_hal_spm_init(void)
{
    hal_spm_init();
}

// Fix link error in hal_sleep_manager_internal.c
// TODO: a better way to fix this error
typedef enum SWLA_ACTION_T {
    SA_START = 0xe0
} SA_ACTION_t;
void SLA_CustomLogging(const char *pxCustomLabel, SA_ACTION_t xAction){

}

