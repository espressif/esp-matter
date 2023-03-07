/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <string.h>
#include <stdio.h>
#include "cmsis.h"
#include "platform/include/tfm_spm_hal.h"
#include "spm_api.h"
#include "spm_db.h"
#include "tfm_platform_core_api.h"
#include "Driver_MPC.h"
#include "mpu_armv8m_drv.h"
#include "region_defs.h"
#include "hal_spm.h"
#include "hal_clk.h"
#include "log/tfm_assert.h"
#include "hal_psram.h"
#include "hal_boot.h"
#include "sys_init.h"
#include "hal_asic_mpu.h"
#include "hal_sys.h"
#ifdef TFM_ENABLE_PLATFORM_ISOLATION
#include "hal_devapc.h"
#include "pltfm_iso.h"
#include "ASIC_MPU_config.h"
#include "DAPC_config.h"
#include "Domain_config.h"
#include "memory_map.h"
#endif /* ifdef TFM_ENABLE_PLATFORM_ISOLATION */

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

/* Import MPC driver */
extern ARM_DRIVER_MPC Driver_SRAM1_MPC;
extern ARM_DRIVER_MPC Driver_SRAM2_MPC;

/* Get address of memory regions to configure MPU */
extern const struct memory_region_limits memory_regions;

struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };

void open_audio_bus()
{
    uint32_t index;

    /* open audio power domain */
    MTCMOS_PWR_ON_AUDIO_AO;
    MTCMOS_PWR_ON_AUDIO_AFE;
    hal_clock_enable(HAL_CLOCK_CG_AUDIO_XTAL);

    for (index = 0; index < 4; index++) {
        SRAM_PWR_ON_AUDIO(index);
    }

    hal_clock_enable(HAL_CLOCK_CG_XPLL);
    hal_clock_enable(HAL_CLOCK_CG_AUDSYS_BUS);
}

static void platform_preinit()
{
    system_preinit_hw();
}

enum tfm_plat_err_t tfm_spm_hal_init_isolation_hw(void)
{
    int32_t ret = ARM_DRIVER_OK;

    platform_preinit();

    ret = mpc_init_cfg();
    if (ret != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    sau_and_idau_cfg();
    //ppc_init_cfg();
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_configure_default_isolation(
                  uint32_t partition_idx,
                  const struct tfm_spm_partition_platform_data_t *platform_data)
{
    return TFM_PLAT_ERR_SUCCESS;
}

#if TFM_LVL != 1

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
#endif /* TFM_LVL != 1 */

void MPC_Handler(void)
{
#if 0
    /* Clear MPC interrupt flag and pending MPC IRQ */
    Driver_SRAM1_MPC.ClearInterrupt();
    NVIC_ClearPendingIRQ(MPC_IRQn);

    /* Print fault message and block execution */
    ERROR_MSG("Oops... MPC fault!!!");

    /* Inform TF-M core that isolation boundary has been violated */
    tfm_access_violation_handler();
#endif
}

void PPC_Handler(void)
{
#if 0
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
#endif
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
    printf("TFM ns entry %x\r\n", *((uint32_t *)(memory_regions.non_secure_code_start+ 4)));
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

void tfm_spm_hal_system_reset(void)
{
    hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
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

enum tfm_plat_err_t tfm_spm_hal_post_isolation_hw(void)
{
#ifdef TFM_ENABLE_PLATFORM_ISOLATION
    uint32_t ns_vtor_addr = tfm_spm_hal_get_ns_VTOR();
    if(!IS_ADDR_IN_FLASH(ns_vtor_addr))
        TFM_ASSERT(0);

    /* Configures non-secure memory spaces in the target */
    platform_isolation_init();
    pltfm_iso_cfg_table_register(master_domain, INFRA_Devices, AUD_Devices, ASIC_MPU_Devices, ns_vtor_addr);
    master_domain_cfg();
    dapc_cfg();
    asic_mpu_region_cfg();
    asic_mpu_apc_cfg();
#else
    //disable all protection
    asic_mpu_disable_all_protect();
#endif /* TFM_ENABLE_PLATFORM_ISOLATION */

    return TFM_PLAT_ERR_SUCCESS;
}

/* FIXME:
 * Instead of TFM-customized mcuboot, at this moment psoc64 uses
 * Cypress version of it - CypressBootloader (CYBL). CYBL doesn't
 * populate BOOT_TFM_SHARED_DATA.
 * As a temp workaround, mock mcuboot shared data to pass
 * initialization checks.
*/
#if 0
void mock_tfm_shared_data(void)
{
    const uint32_t mock_data[] = {
        0x00D92016,    0x00071103,     0x00455053,     0x30000911,
        0x302E302E,    0x00081102,     0x00000000,     0x00241108,
        0x6C170A97,    0x5645665E,     0xDB6E2BA6,     0xA4FF4D74,
        0xFD34D7DB,    0x67449A82,     0x75FD0930,     0xAA15A9F9,
        0x000A1109,    0x32414853,     0x11013635,     0xE6BF0024,
        0x26886FD8,    0xFB97FFF4,     0xFBE6C496,     0x463E99C4,
        0x5D56FC19,    0x34DF6AA2,     0x9A4829C3,     0x114338DC,
        0x534E0008,    0x11404550,     0x2E300009,     0x42302E30,
        0x00000811,    0x48000000,     0x7E002411,     0x5FD9229A,
        0xE9672A5F,    0x31AAE1EA,     0x8514D772,     0x7F3B26BC,
        0x2C7EF27A,    0x9C6047D2,     0x4937BB9F,     0x53000A11,
        0x35324148,    0x24114136,     0xCA60B300,     0x6B8CC9F5,
        0x82482A94,    0x23489DFA,     0xA966B1EF,     0x4A6E6AEF,
        0x19197CA3,    0xC0CC1FED,     0x00000049,     0x00000000
    };
    uint32_t *boot_data = (uint32_t*)BOOT_TFM_SHARED_DATA_BASE;
    memcpy(boot_data, mock_data, sizeof(mock_data));
}


enum tfm_plat_err_t tfm_spm_hal_post_init_platform(void)
{
     // FIXME: Use the actual data from mcuboot
    mock_tfm_shared_data();

    return TFM_PLAT_ERR_SUCCESS;
}
#endif
