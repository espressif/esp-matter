/*
 * Copyright (c) 2019-2020 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "target_cfg.h"
#include "Driver_MPC.h"
#include "Driver_SSE123_PPC.h"
#include "device_definition.h"
#include "platform_description.h"
#include "region_defs.h"
#include "mpu_armv8m_drv.h"
#include "secure_utilities.h"
#include "tfm_plat_defs.h"
#include "region.h"

/* Throw out bus error when an access causes security violation */
#define CMSDK_SECRESPCFG_BUS_ERR_MASK   (1UL << 0)

/* The section names come from the scatter file */
REGION_DECLARE(Load$$LR$$, LR_NS_PARTITION, $$Base);
REGION_DECLARE(Load$$LR$$, LR_VENEER, $$Base);
REGION_DECLARE(Load$$LR$$, LR_VENEER, $$Limit);
#ifdef BL2
REGION_DECLARE(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base);
#endif /* BL2 */

const struct memory_region_limits memory_regions = {
    .non_secure_code_start =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base) +
        BL2_HEADER_SIZE,

    .non_secure_partition_base =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base),

    .non_secure_partition_limit =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base) +
        NS_PARTITION_SIZE - 1,

    .veneer_base =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_VENEER, $$Base),

    .veneer_limit =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_VENEER, $$Limit),

#ifdef BL2
    .secondary_partition_base =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base),

    .secondary_partition_limit =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base) +
        SECONDARY_PARTITION_SIZE - 1,
#endif /* BL2 */
};

static struct mpu_armv8m_dev_t dev_mpu = { MPU_BASE };

/* Allows software, via SAU, to define the code region as a NSC */
#define NSCCFG_CODENSC  1

/* Import MPC drivers */
extern ARM_DRIVER_MPC Driver_ISRAM0_MPC;
extern ARM_DRIVER_MPC Driver_SRAM1_MPC;
extern ARM_DRIVER_MPC Driver_SRAM2_MPC;
extern ARM_DRIVER_MPC Driver_SSRAM3_MPC;

/* Import PPC drivers */
extern DRIVER_PPC_SSE123 Driver_PPC_SSE123_AHB_EXP0;
extern DRIVER_PPC_SSE123 Driver_PPC_SSE123_APB;
extern DRIVER_PPC_SSE123 Driver_PPC_SSE123_APB_EXP0;
extern DRIVER_PPC_SSE123 Driver_PPC_SSE123_APB_EXP1;
extern DRIVER_PPC_SSE123 Driver_PPC_SSE123_APB_EXP2;
extern DRIVER_PPC_SSE123 Driver_PPC_SSE123_APB_EXP3;

/* Define Peripherals NS address range for the platform */
#define PERIPHERALS_BASE_NS_START      (0x40000000)
#define PERIPHERALS_BASE_NS_END        (0x4FFFFFFF)

/* Enable system reset request for CPU 0 */
#define ENABLE_CPU0_SYSTEM_RESET_REQUEST (1U << 4U)

/* To write into AIRCR register, 0x5FA value must be write to the VECTKEY field,
 * otherwise the processor ignores the write.
 */
#define SCB_AIRCR_WRITE_MASK ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos))

struct tfm_spm_partition_platform_data_t tfm_peripheral_std_uart = {
        UART0_BASE_NS,
        UART0_BASE_NS + 0xFFF,
        PPC_SP_DO_NOT_CONFIGURE,
        -1
};

struct tfm_spm_partition_platform_data_t tfm_peripheral_fpga_io = {
        MPS2_IO_FPGAIO_BASE_S,
        MPS2_IO_FPGAIO_BASE_S + 0xFFF,
        PPC_SP_APB_PPC_EXP3,
        FPGA_IO_APB_PPCEXP3_POS
};

struct tfm_spm_partition_platform_data_t tfm_peripheral_timer0 = {
        SYSTEM_TIMER0_BASE_S,
        SYSTEM_TIMER1_BASE_S - 1,
        PPC_SP_APB_PPC,
        SYSTEM_TIMER0_APB_PPC_POS
};

static DRIVER_PPC_SSE123 *const ppc_bank_drivers[] = {
    &Driver_PPC_SSE123_AHB_EXP0,    /* AHB PPCEXP0 */
    &Driver_PPC_SSE123_APB,         /* APB PPC0 */
    &Driver_PPC_SSE123_APB_EXP0,    /* APB PPCEXP0 */
    &Driver_PPC_SSE123_APB_EXP1,    /* APB PPCEXP1 */
    &Driver_PPC_SSE123_APB_EXP2,    /* APB PPCEXP2 */
    &Driver_PPC_SSE123_APB_EXP3,    /* APB PPCEXP3 */
};

#define PPC_BANK_COUNT (sizeof(ppc_bank_drivers)/sizeof(ppc_bank_drivers[0]))

enum tfm_plat_err_t enable_fault_handlers(void)
{
    /* Secure fault is not present in the Baseline implementation. */
    /* Fault handler enable registers are not present in a Baseline
     * implementation.
     */

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t system_reset_cfg(void)
{
    struct sse123_sysctrl_t *sysctrl =
                            (struct sse123_sysctrl_t *)SSE123_SYSCTRL_BASE_S;
    uint32_t reg_value = SCB->AIRCR;

    /* Enable system reset request for CPU 0, to be triggered via
     * NVIC_SystemReset function.
     */
    sysctrl->resetmask |= ENABLE_CPU0_SYSTEM_RESET_REQUEST;

    /* Clear SCB_AIRCR_VECTKEY value */
    reg_value &= ~(uint32_t)(SCB_AIRCR_VECTKEY_Msk);

    /* Enable system reset request only to the secure world */
    reg_value |= (uint32_t)(SCB_AIRCR_WRITE_MASK | SCB_AIRCR_SYSRESETREQS_Msk);

    SCB->AIRCR = reg_value;

    return TFM_PLAT_ERR_SUCCESS;
}

/*--------------------- NVIC interrupt NS/S configuration --------------------*/
enum tfm_plat_err_t nvic_interrupt_target_state_cfg(void)
{
    uint8_t i;

    /* Target every interrupt to NS; unimplemented interrupts will be WI */
    for (i = 0; i < (sizeof(NVIC->ITNS) / sizeof(NVIC->ITNS[0])); i++) {
        NVIC->ITNS[i] = 0xFFFFFFFF;
    }

    /* Make sure that MPC and PPC are targeted to S state */
    NVIC_ClearTargetState(MPC_IRQn);
    NVIC_ClearTargetState(PPC_IRQn);

    return TFM_PLAT_ERR_SUCCESS;
}

enum mpu_armv8m_error_t mpu_enable(uint32_t privdef_en, uint32_t hfnmi_en)
{
    return mpu_armv8m_enable(&dev_mpu, privdef_en, hfnmi_en);
}

enum mpu_armv8m_error_t mpu_disable(void)
{
    return mpu_armv8m_disable(&dev_mpu);
}

enum mpu_armv8m_error_t mpu_region_enable(
                                     struct mpu_armv8m_region_cfg_t *region_cfg)
{
    if (!region_cfg) {
        return MPU_ARMV8M_ERROR;
    }

    return mpu_armv8m_region_enable(&dev_mpu, region_cfg);
}

enum mpu_armv8m_error_t mpu_region_disable(uint32_t region_nr)
{
    return mpu_armv8m_region_disable(&dev_mpu, region_nr);
}

enum mpu_armv8m_error_t mpu_clean(void)
{
    return mpu_armv8m_clean(&dev_mpu);
}

/*----------------- NVIC interrupt enabling for S peripherals ----------------*/
enum tfm_plat_err_t nvic_interrupt_enable(void)
{
    int32_t ret = ARM_DRIVER_OK;

    /* MPC interrupt enabling */
    mpc_clear_irq();
    ret = Driver_SRAM1_MPC.EnableInterrupt();
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Enable MPC interrupt for SSRAM1!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    ret = Driver_SRAM2_MPC.EnableInterrupt();
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Enable MPC interrupt for SSRAM2!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    NVIC_ClearPendingIRQ(MPC_IRQn);
    NVIC_EnableIRQ(MPC_IRQn);

    /* PPC interrupt enabling */
    /* Clear pending PPC interrupts */
    /* In the PPC configuration function, we have used the Non-Secure
     * Privilege Control Block to grant unprivilged NS access to some
     * peripherals used by NS. That triggers a PPC0 exception as that
     * register is meant for NS privileged access only. Clear it here
     */
    NVIC_ClearPendingIRQ(PPC_IRQn);
    Driver_PPC_SSE123_APB.ClearInterrupt();

    /* Enable PPC interrupts for APB PPC */
    ret = Driver_PPC_SSE123_APB.EnableInterrupt();
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Enable PPC APB interrupt!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    ret = Driver_PPC_SSE123_APB_EXP2.EnableInterrupt();
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Enable PPC APB EXP2 interrupt!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    ret = Driver_PPC_SSE123_APB_EXP3.EnableInterrupt();
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Enable PPC APB EXP3 interrupt!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    NVIC_EnableIRQ(PPC_IRQn);

    return TFM_PLAT_ERR_SUCCESS;
}

/*------------------- SAU/IDAU configuration functions -----------------------*/
void sau_and_idau_cfg(void)
{
    struct sse123_spctrl_t *spctrl = (struct sse123_spctrl_t*)SSE123_SPCTRL_BASE_S;

    /* Enables SAU */
    TZ_SAU_Enable();

    /* Configures SAU regions to be non-secure */
    SAU->RNR  = 0U;
    SAU->RBAR = (memory_regions.non_secure_partition_base
                 & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = (memory_regions.non_secure_partition_limit
                  & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;

    SAU->RNR  = 1U;
    SAU->RBAR = (NS_DATA_START & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = (NS_DATA_LIMIT & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;

    /* Configures veneers region to be non-secure callable */
    SAU->RNR  = 2U;
    SAU->RBAR = (memory_regions.veneer_base & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = (memory_regions.veneer_limit & SAU_RLAR_LADDR_Msk)
                 | SAU_RLAR_ENABLE_Msk | SAU_RLAR_NSC_Msk;

    /* Configure the peripherals space */
    SAU->RNR  = 3U;
    SAU->RBAR = (PERIPHERALS_BASE_NS_START & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = (PERIPHERALS_BASE_NS_END & SAU_RLAR_LADDR_Msk)
                  | SAU_RLAR_ENABLE_Msk;
#ifdef BL2
    /* Secondary image partition */
    SAU->RNR  = 4U;
    SAU->RBAR = (memory_regions.secondary_partition_base
                 & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = (memory_regions.secondary_partition_limit
                 & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;
#endif /* BL2 */

    /* Allows SAU to define the code region as a NSC */
    spctrl->nsccfg |= NSCCFG_CODENSC;
}

/*------------------- Memory configuration functions -------------------------*/
enum tfm_plat_err_t mpc_init_cfg(void)
{
    int32_t ret = ARM_DRIVER_OK;

    ret = Driver_SRAM1_MPC.Initialize();
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Initialize MPC for SSRAM1!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    ret = Driver_SRAM1_MPC.ConfigRegion(
                                    memory_regions.non_secure_partition_base,
                                    memory_regions.non_secure_partition_limit,
                                    ARM_MPC_ATTR_NONSECURE);
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Configure MPC for SSRAM1!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

#ifdef BL2
    ret = Driver_SRAM1_MPC.ConfigRegion(
                                    memory_regions.secondary_partition_base,
                                    memory_regions.secondary_partition_limit,
                                    ARM_MPC_ATTR_NONSECURE);
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Configure MPC for SSRAM1!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
#endif

    ret = Driver_SRAM2_MPC.Initialize();
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Initialize MPC for SSRAM2!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    ret = Driver_SRAM2_MPC.ConfigRegion(NS_DATA_START, NS_DATA_LIMIT,
                                  ARM_MPC_ATTR_NONSECURE);
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Configure MPC for SSRAM2!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Lock down the MPC configuration */
    ret = Driver_SRAM1_MPC.LockDown();
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Lock down MPC for SSRAM1!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    ret = Driver_SRAM2_MPC.LockDown();
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Lock down MPC for SSRAM2!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Initialize and lock down not used MPC drivers. */
    ret = Driver_ISRAM0_MPC.Initialize();
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Initialize MPC for ISRAM0!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    ret = Driver_SSRAM3_MPC.Initialize();
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Initialize MPC for SSRAM3!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    ret = Driver_ISRAM0_MPC.LockDown();
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Lock down MPC for ISRAM0!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    ret = Driver_SSRAM3_MPC.LockDown();
    if (ret != ARM_DRIVER_OK) {
        ERROR_MSG("Failed to Lock down MPC for SSRAM3!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Add barriers to assure the MPC configuration is done before continue
     * the execution.
     */
    __DSB();
    __ISB();

    return TFM_PLAT_ERR_SUCCESS;
}

void mpc_clear_irq(void)
{
    Driver_SRAM1_MPC.ClearInterrupt();
    Driver_SRAM2_MPC.ClearInterrupt();
}

/*------------------- PPC configuration functions -------------------------*/
enum tfm_plat_err_t ppc_init_cfg(void)
{
    struct sse123_spctrl_t *spctrl =
                                (struct sse123_spctrl_t*)SSE123_SPCTRL_BASE_S;
    int32_t err = ARM_DRIVER_OK;

    /* Grant non-secure access to peripherals in the PPC0
     * (timer0 and 1, dualtimer, watchdog, mhu 0 and 1)
     */
    err |= Driver_PPC_SSE123_APB.Initialize();
    err |= Driver_PPC_SSE123_APB.ConfigSecurity(SYSTEM_TIMER0_APB_PPC_POS,
                                         PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB.ConfigSecurity(SYSTEM_TIMER1_APB_PPC_POS,
                                         PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB.ConfigSecurity(SYSTEM_WATCHDOG_APB_PPC_POS,
                                         PPC_SSE123_NONSECURE_CONFIG);

    /* Grant non-secure access for APB peripherals on EXP2 */
    err |= Driver_PPC_SSE123_APB_EXP2.Initialize();
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(SPI0_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(SPI1_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(SPI2_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(SPI3_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(SPI4_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(UART0_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(UART1_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(UART2_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(UART3_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(UART4_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(I2C0_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(I2C1_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(I2C2_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigSecurity(I2C3_APB_PPCEXP2_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);

    /* Grant un-privileged access for UART0 in NS domain */
    err |= Driver_PPC_SSE123_APB_EXP2.ConfigPrivilege(UART0_APB_PPCEXP2_POS,
                                          PPC_SSE123_NONSECURE_CONFIG,
                                          PPC_SSE123_PRIV_AND_NONPRIV_CONFIG);

    /* Grant non-secure access for APB peripherals on EXP3 */
    err |= Driver_PPC_SSE123_APB_EXP3.Initialize();
    err |= Driver_PPC_SSE123_APB_EXP3.ConfigSecurity(FPGA_SCC_APB_PPCEXP3_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);

    err |= Driver_PPC_SSE123_APB_EXP3.ConfigSecurity(FPGA_AUDIO_APB_PPCEXP3_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);

    err |= Driver_PPC_SSE123_APB_EXP3.ConfigSecurity(FPGA_IO_APB_PPCEXP3_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);

    /* Grant un-privileged access for LEDs in NS domain */
    err |= Driver_PPC_SSE123_APB_EXP3.ConfigPrivilege(FPGA_SCC_APB_PPCEXP3_POS,
                                            PPC_SSE123_NONSECURE_CONFIG,
                                            PPC_SSE123_PRIV_AND_NONPRIV_CONFIG);

    err |= Driver_PPC_SSE123_APB_EXP3.ConfigPrivilege(FPGA_IO_APB_PPCEXP3_POS,
                                            PPC_SSE123_NONSECURE_CONFIG,
                                            PPC_SSE123_PRIV_AND_NONPRIV_CONFIG);

    /*
     * Grant non-secure access to all peripherals on AHB EXP0:
     * Make sure that all possible peripherals are enabled by default
     */
    err |= Driver_PPC_SSE123_AHB_EXP0.Initialize();
    err |= Driver_PPC_SSE123_AHB_EXP0.ConfigSecurity(VGA_AHB_PPCEXP_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_AHB_EXP0.ConfigSecurity(GPIO0_AHB_PPCEXP_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_AHB_EXP0.ConfigSecurity(GPIO1_AHB_PPCEXP_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_AHB_EXP0.ConfigSecurity(GPIO2_AHB_PPCEXP_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_AHB_EXP0.ConfigSecurity(GPIO3_AHB_PPCEXP_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_AHB_EXP0.ConfigSecurity(PSRAM_ETH_AHB_PPCEXP_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_AHB_EXP0.ConfigSecurity(DMA0_AHB_PPCEXP_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);
    err |= Driver_PPC_SSE123_AHB_EXP0.ConfigSecurity(DMA1_AHB_PPCEXP_POS,
                                              PPC_SSE123_NONSECURE_CONFIG);

    /* Initialize not used PPC drivers */
    err |= Driver_PPC_SSE123_APB_EXP0.Initialize();
    err |= Driver_PPC_SSE123_APB_EXP1.Initialize();

    /*
     * Configure the response to a security violation as a
     * bus error instead of RAZ/WI
     */
    spctrl->secrespcfg |= CMSDK_SECRESPCFG_BUS_ERR_MASK;

    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

void ppc_configure_to_secure(enum ppc_bank_e bank, uint32_t pos)
{
    DRIVER_PPC_SSE123 *ppc_driver;

    if (bank >= PPC_BANK_COUNT) {
        return;
    }

    ppc_driver = ppc_bank_drivers[bank];
    if (ppc_driver) {
        ppc_driver->ConfigSecurity(pos, PPC_SSE123_SECURE_CONFIG);
    }
}

void ppc_configure_to_non_secure(enum ppc_bank_e bank, uint32_t pos)
{
    DRIVER_PPC_SSE123 *ppc_driver;

    if (bank >= PPC_BANK_COUNT) {
        return;
    }

    ppc_driver = ppc_bank_drivers[bank];
    if (ppc_driver) {
        ppc_driver->ConfigSecurity(pos, PPC_SSE123_NONSECURE_CONFIG);
    }
}

void ppc_configure_to_secure_priv(enum ppc_bank_e bank, uint32_t pos)
{
    DRIVER_PPC_SSE123 *ppc_driver;

    if (bank >= PPC_BANK_COUNT) {
        return;
    }

    ppc_driver = ppc_bank_drivers[bank];
    if (ppc_driver) {
        ppc_driver->ConfigPrivilege(pos, PPC_SSE123_SECURE_CONFIG,
                                    PPC_SSE123_PRIV_CONFIG);
    }
}

void ppc_configure_to_secure_unpriv(enum ppc_bank_e bank, uint32_t pos)
{
    DRIVER_PPC_SSE123 *ppc_driver;

    if (bank >= PPC_BANK_COUNT) {
        return;
    }

    ppc_driver = ppc_bank_drivers[bank];
    if (ppc_driver) {
        ppc_driver->ConfigPrivilege(pos, PPC_SSE123_SECURE_CONFIG,
                                    PPC_SSE123_PRIV_AND_NONPRIV_CONFIG);
    }
}

void ppc_clear_irq(void)
{
    Driver_PPC_SSE123_AHB_EXP0.ClearInterrupt();
    Driver_PPC_SSE123_APB.ClearInterrupt();
    Driver_PPC_SSE123_APB_EXP2.ClearInterrupt();
    Driver_PPC_SSE123_APB_EXP3.ClearInterrupt();
}
