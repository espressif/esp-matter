/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "cmsis.h"
#include "platform/include/tfm_spm_hal.h"
#include "spm_api.h"
#include "spm_db.h"
#include "tfm_platform_core_api.h"
#include "target_cfg.h"
#include "Driver_MPC.h"
#include "mpu_armv8m_drv.h"
#include "region_defs.h"
#include "secure_utilities.h"
#include "region.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

/* Import MPC driver */
extern ARM_DRIVER_MPC Driver_SSRAM1_MPC;

/* Get address of memory regions to configure MPU */
extern const struct memory_region_limits memory_regions;

struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };

enum tfm_plat_err_t tfm_spm_hal_init_isolation_hw(void)
{
    int32_t ret = ARM_DRIVER_OK;
    /* Configures non-secure memory spaces in the target */
    sau_and_idau_cfg();
    ret = mpc_init_cfg();
    if (ret != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    ppc_init_cfg();
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_configure_default_isolation(
                  uint32_t partition_idx,
                  const struct tfm_spm_partition_platform_data_t *platform_data)
{
    bool privileged = tfm_is_partition_privileged(partition_idx);

    if (!platform_data) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    if (platform_data->periph_ppc_bank != PPC_SP_DO_NOT_CONFIGURE) {
        ppc_configure_to_secure(platform_data->periph_ppc_bank,
                                platform_data->periph_ppc_loc);
        if (privileged) {
            ppc_clr_secure_unpriv(platform_data->periph_ppc_bank,
                                  platform_data->periph_ppc_loc);
        } else {
            ppc_en_secure_unpriv(platform_data->periph_ppc_bank,
                                 platform_data->periph_ppc_loc);
        }
    }
    return TFM_PLAT_ERR_SUCCESS;
}

#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT

#define MPU_REGION_VENEERS           0
#define MPU_REGION_TFM_UNPRIV_CODE   1
#define MPU_REGION_TFM_UNPRIV_DATA   2
#define MPU_REGION_NS_STACK          3
#define PARTITION_REGION_RO          4
#define PARTITION_REGION_RW_STACK    5
#define PARTITION_REGION_PERIPH      6

REGION_DECLARE(Load$$LR$$, LR_VENEER, $$Base);
REGION_DECLARE(Load$$LR$$, LR_VENEER, $$Limit);
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_UNPRIV_DATA, $$RW$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_DATA, $$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_APP_CODE_START, $$Base);
REGION_DECLARE(Image$$, TFM_APP_CODE_END, $$Base);
REGION_DECLARE(Image$$, TFM_APP_RW_STACK_START, $$Base);
REGION_DECLARE(Image$$, TFM_APP_RW_STACK_END, $$Base);
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Base);
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Limit);

const struct mpu_armv8m_region_cfg_t region_cfg[] = {
           /* Veneer region */
           {
               MPU_REGION_VENEERS,
               (uint32_t)&REGION_NAME(Load$$LR$$, LR_VENEER, $$Base),
               (uint32_t)&REGION_NAME(Load$$LR$$, LR_VENEER, $$Limit),
               MPU_ARMV8M_MAIR_ATTR_CODE_IDX,
               MPU_ARMV8M_XN_EXEC_OK,
               MPU_ARMV8M_AP_RO_PRIV_UNPRIV,
               MPU_ARMV8M_SH_NONE
           },
           /* TFM Core unprivileged code region */
           {
               MPU_REGION_TFM_UNPRIV_CODE,
               (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Base),
               (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit),
               MPU_ARMV8M_MAIR_ATTR_CODE_IDX,
               MPU_ARMV8M_XN_EXEC_OK,
               MPU_ARMV8M_AP_RO_PRIV_UNPRIV,
               MPU_ARMV8M_SH_NONE
           },
           /* TFM Core unprivileged data region */
           {
               MPU_REGION_TFM_UNPRIV_DATA,
               (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_DATA, $$RW$$Base),
               (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_DATA, $$ZI$$Limit),
               MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
               MPU_ARMV8M_XN_EXEC_NEVER,
               MPU_ARMV8M_AP_RW_PRIV_UNPRIV,
               MPU_ARMV8M_SH_NONE
           },
           /* NSPM PSP */
           {
               MPU_REGION_NS_STACK,
               (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Base),
               (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Limit),
               MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
               MPU_ARMV8M_XN_EXEC_NEVER,
               MPU_ARMV8M_AP_RW_PRIV_UNPRIV,
               MPU_ARMV8M_SH_NONE
           },
           /* RO region */
           {
               PARTITION_REGION_RO,
               (uint32_t)&REGION_NAME(Image$$, TFM_APP_CODE_START, $$Base),
               (uint32_t)&REGION_NAME(Image$$, TFM_APP_CODE_END, $$Base),
               MPU_ARMV8M_MAIR_ATTR_CODE_IDX,
               MPU_ARMV8M_XN_EXEC_OK,
               MPU_ARMV8M_AP_RO_PRIV_UNPRIV,
               MPU_ARMV8M_SH_NONE
           },
           /* RW, ZI and stack as one region */
           {
               PARTITION_REGION_RW_STACK,
               (uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_START, $$Base),
               (uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_END, $$Base),
               MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
               MPU_ARMV8M_XN_EXEC_NEVER,
               MPU_ARMV8M_AP_RW_PRIV_UNPRIV,
               MPU_ARMV8M_SH_NONE
           }
       };

static enum spm_err_t tfm_spm_mpu_init(void)
{
    int32_t i;

    mpu_armv8m_clean(&dev_mpu_s);

    for (i = 0; i < ARRAY_SIZE(region_cfg); i++) {
        if (mpu_armv8m_region_enable(&dev_mpu_s,
            (struct mpu_armv8m_region_cfg_t *)&region_cfg[i])
            != MPU_ARMV8M_OK) {
            return SPM_ERR_INVALID_CONFIG;
        }
    }

    mpu_armv8m_enable(&dev_mpu_s, PRIVILEGED_DEFAULT_ENABLE,
                      HARDFAULT_NMI_ENABLE);

    return SPM_ERR_OK;
}

enum tfm_plat_err_t tfm_spm_hal_setup_isolation_hw(void)
{
    if (tfm_spm_mpu_init() != SPM_ERR_OK) {
        ERROR_MSG("Failed to set up initial MPU configuration! Halting.");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    return TFM_PLAT_ERR_SUCCESS;
}
#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

void MPC_Handler(void)
{
    /* Clear MPC interrupt flag and pending MPC IRQ */
    Driver_SSRAM1_MPC.ClearInterrupt();
    NVIC_ClearPendingIRQ(MPC_IRQn);

    /* Print fault message and block execution */
    ERROR_MSG("Oops... MPC fault!!!");

    /* Inform TF-M core that isolation boundary has been violated */
    tfm_access_violation_handler();
}

void PPC_Handler(void)
{
    /*
     * Due to an issue on the FVP, the PPC fault doesn't trigger a
     * PPC IRQ which is handled by the PPC_handler.
     * In the FVP execution, this code is not execute.
     */

    /* Clear PPC interrupt flag and pending PPC IRQ */
    ppc_clear_irq();
    NVIC_ClearPendingIRQ(PPC_IRQn);

    /* Print fault message*/
    ERROR_MSG("Oops... PPC fault!!!");

    /* Inform TF-M core that isolation boundary has been violated */
    tfm_access_violation_handler();
}

uint32_t tfm_spm_hal_get_ns_VTOR(void)
{
    return memory_regions.non_secure_code_start;
}

uint32_t tfm_spm_hal_get_ns_MSP(void)
{
    return *((uint32_t *)memory_regions.non_secure_code_start);
}

uint32_t tfm_spm_hal_get_ns_entry_point(void)
{
    return *((uint32_t *)(memory_regions.non_secure_code_start+ 4));
}

enum tfm_plat_err_t tfm_spm_hal_set_secure_irq_priority(IRQn_Type irq_line,
                                                        uint32_t priority)
{
    uint32_t quantized_priority = priority >> (8U - __NVIC_PRIO_BITS);
    NVIC_SetPriority(irq_line, quantized_priority);
    return TFM_PLAT_ERR_SUCCESS;
}

void tfm_spm_hal_clear_pending_irq(IRQn_Type irq_line)
{
    NVIC_ClearPendingIRQ(irq_line);
}

void tfm_spm_hal_enable_irq(IRQn_Type irq_line)
{
    NVIC_EnableIRQ(irq_line);
}

void tfm_spm_hal_disable_irq(IRQn_Type irq_line)
{
    NVIC_DisableIRQ(irq_line);
}

enum irq_target_state_t tfm_spm_hal_set_irq_target_state(
                                           IRQn_Type irq_line,
                                           enum irq_target_state_t target_state)
{
    uint32_t result;

    if (target_state == TFM_IRQ_TARGET_STATE_SECURE) {
        result = NVIC_ClearTargetState(irq_line);
    } else {
        result = NVIC_SetTargetState(irq_line);
    }

    if (result) {
        return TFM_IRQ_TARGET_STATE_NON_SECURE;
    } else {
        return TFM_IRQ_TARGET_STATE_SECURE;
    }
}

enum tfm_plat_err_t tfm_spm_hal_enable_fault_handlers(void)
{
    return enable_fault_handlers();
}

enum tfm_plat_err_t tfm_spm_hal_system_reset_cfg(void)
{
    return system_reset_cfg();
}

enum tfm_plat_err_t tfm_spm_hal_init_debug(void)
{
    return init_debug();
}

enum tfm_plat_err_t tfm_spm_hal_nvic_interrupt_target_state_cfg(void)
{
    return nvic_interrupt_target_state_cfg();
}

enum tfm_plat_err_t tfm_spm_hal_nvic_interrupt_enable(void)
{
    return nvic_interrupt_enable();
}
