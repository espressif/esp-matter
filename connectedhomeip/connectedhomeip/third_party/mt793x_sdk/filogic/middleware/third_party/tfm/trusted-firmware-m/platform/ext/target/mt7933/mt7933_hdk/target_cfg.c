/*
 * Copyright (c) 2017-2019 Arm Limited
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

#include "cmsis.h"
#include "target_cfg.h"
#include "Driver_MPC.h"
#include "platform_retarget_dev.h"
#include "region_defs.h"
#include "memory_map_mt7933.h"
#include "hal_asic_mpu.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
/* Macros to pick linker symbols */
#define REGION(a, b, c) a##b##c
#define REGION_NAME(a, b, c) REGION(a, b, c)
#define REGION_DECLARE(a, b, c) extern uint32_t REGION_NAME(a, b, c)

#ifdef TARGET_PLATFORM_MT793X
  #define MT7933_HW_VER_E1 0x8A00
  #define MT7933_HW_VER_E2 0x8A10
#else
  #error "Not supported this chip."
#endif

/* The section names come from the scatter file */
REGION_DECLARE(Load$$LR$$, LR_NS_PARTITION, $$Base);
REGION_DECLARE(Load$$LR$$, LR_VENEER, $$Base);
REGION_DECLARE(Load$$LR$$, LR_VENEER, $$Limit);
#ifdef BL2
REGION_DECLARE(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base);
#endif /* BL2 */

const struct memory_region_limits memory_regions = {
    .non_secure_code_start =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base) ,

    .non_secure_partition_base =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base),

    .non_secure_partition_limit =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base) +
        NS_PARTITION_SIZE - 1,

    .veneer_base = (uint32_t)&REGION_NAME(Load$$LR$$, LR_VENEER, $$Base),
    .veneer_limit = (uint32_t)&REGION_NAME(Load$$LR$$, LR_VENEER, $$Limit),

#ifdef BL2
    .secondary_partition_base =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base),

    .secondary_partition_limit =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base) +
        SECONDARY_PARTITION_SIZE - 1,
#endif /* BL2 */
};

/* Allows software, via SAU, to define the code region as a NSC */
#define NSCCFG_CODENSC  1

/* Import MPC driver */
extern ARM_DRIVER_MPC Driver_ISRAM0_MPC;
extern ARM_DRIVER_MPC Driver_SRAM1_MPC;
extern ARM_DRIVER_MPC Driver_SRAM2_MPC;
/* Define Peripherals NS address range for the platform */
#define PERIPHERALS_BASE_NS_START (0x20000000)
//#define PERIPHERALS_BASE_NS_END   (0x7FFFFFFF)  //0x4100CFFF -> 0x7FFFFFFF, some module may access peripheral address over 0x4100CFFF such as WIFI, BT
#define PERIPHERALS_BASE_NS_END   (0x4100CFFF)

/* Enable system reset request for CPU 0 */
#define ENABLE_CPU0_SYSTEM_RESET_REQUEST (1U << 4U)

/* To write into AIRCR register, 0x5FA value must be write to the VECTKEY field,
 * otherwise the processor ignores the write.
 */
#define SCB_AIRCR_WRITE_MASK ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos))

/* Debug configuration flags */
#define SPNIDEN_SEL_STATUS (0x01u << 7)
#define SPNIDEN_STATUS     (0x01u << 6)
#define SPIDEN_SEL_STATUS  (0x01u << 5)
#define SPIDEN_STATUS      (0x01u << 4)
#define NIDEN_SEL_STATUS   (0x01u << 3)
#define NIDEN_STATUS       (0x01u << 2)
#define DBGEN_SEL_STATUS   (0x01u << 1)
#define DBGEN_STATUS       (0x01u << 0)

#define All_SEL_STATUS (SPNIDEN_SEL_STATUS | SPIDEN_SEL_STATUS | \
                        NIDEN_SEL_STATUS | DBGEN_SEL_STATUS)

struct tfm_spm_partition_platform_data_t tfm_peripheral_std_uart = {
        UART0_BASE_NS,
        UART0_BASE_NS + 0xFFF,
        PPC_SP_DO_NOT_CONFIGURE,
        -1
};

struct tfm_spm_partition_platform_data_t tfm_peripheral_uart1 = {
        UART1_BASE_S,
        UART1_BASE_S + 0xFFF,
        PPC_SP_APB_PPC_EXP1,
        CMSDK_UART1_APB_PPC_POS
};
#if 0
struct tfm_spm_partition_platform_data_t tfm_peripheral_fpga_io = {
        MPS2_IO_FPGAIO_BASE_S,
        MPS2_IO_FPGAIO_BASE_S + 0xFFF,
        PPC_SP_APB_PPC_EXP2,
        CMSDK_FPGA_IO_PPC_POS
};
#endif
struct tfm_spm_partition_platform_data_t tfm_peripheral_timer0 = {
        CMSDK_TIMER0_BASE_S,
        CMSDK_TIMER1_BASE_S - 1,
        PPC_SP_APB_PPC0,
        CMSDK_TIMER0_APB_PPC_POS
};

enum tfm_plat_err_t enable_fault_handlers(void)
{
    /* Explicitly set secure fault priority to the highest */
    NVIC_SetPriority(SecureFault_IRQn, 0);

    /* Enables BUS, MEM, USG and Secure faults */
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk
                  | SCB_SHCSR_BUSFAULTENA_Msk
                  | SCB_SHCSR_MEMFAULTENA_Msk
                  | SCB_SHCSR_SECUREFAULTENA_Msk;
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t system_reset_cfg(void)
{
    uint32_t reg_value = SCB->AIRCR;

    /* Clear SCB_AIRCR_VECTKEY value */
    reg_value &= ~(uint32_t)(SCB_AIRCR_VECTKEY_Msk);

    /* Enable system reset request only to the secure world */
    reg_value |= (uint32_t)(SCB_AIRCR_WRITE_MASK | SCB_AIRCR_SYSRESETREQS_Msk);

    SCB->AIRCR = reg_value;

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t init_debug(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

/*----------------- NVIC interrupt target state to NS configuration ----------*/
enum tfm_plat_err_t nvic_interrupt_target_state_cfg(void)
{
    /* Target every interrupt to NS; unimplemented interrupts will be WI */
    for (uint8_t i=0; i<sizeof(NVIC->ITNS)/sizeof(NVIC->ITNS[0]); i++) {
        NVIC->ITNS[i] = 0xFFFFFFFF;
    }

#if 0
    /* Make sure that MPC and PPC are targeted to S state */
    NVIC_ClearTargetState(MPC_IRQn);
    NVIC_ClearTargetState(PPC_IRQn);
#endif

#ifdef SECURE_UART1
    /* UART1 is a secure peripheral, so its IRQs have to target S state */
    NVIC_ClearTargetState(UARTRX1_IRQn);
    NVIC_ClearTargetState(UARTTX1_IRQn);
    NVIC_ClearTargetState(UART1_IRQn);
#endif

    return TFM_PLAT_ERR_SUCCESS;
}

/*----------------- NVIC interrupt enabling for S peripherals ----------------*/
enum tfm_plat_err_t nvic_interrupt_enable(void)
{
#if 0
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    int32_t ret = ARM_DRIVER_OK;

    /* MPC interrupt enabling */
    ret = Driver_SRAM1_MPC.EnableInterrupt();
    if (ret != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    ret = Driver_SRAM2_MPC.EnableInterrupt();
    if (ret != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    NVIC_EnableIRQ(MPC_IRQn);

    /* PPC interrupt enabling */
    /* Clear pending PPC interrupts */
    /* In the PPC configuration function, we have used the Non-Secure
     * Privilege Control Block to grant unprivilged NS access to some
     * peripherals used by NS. That triggers a PPC0 exception as that
     * register is meant for NS privileged access only. Clear it here
     */
    spctrl->secppcintclr = CMSDK_APB_PPC0_INT_POS_MASK;

    /* Enable PPC interrupts for APB PPC */
    spctrl->secppcinten |= CMSDK_APB_PPC0_INT_POS_MASK |
                           CMSDK_APB_PPC1_INT_POS_MASK |
                           CMSDK_APB_PPCEXP0_INT_POS_MASK |
                           CMSDK_APB_PPCEXP1_INT_POS_MASK |
                           CMSDK_APB_PPCEXP2_INT_POS_MASK |
                           CMSDK_APB_PPCEXP3_INT_POS_MASK;

    NVIC_EnableIRQ(PPC_IRQn);
#endif

    return TFM_PLAT_ERR_SUCCESS;
}

/*------------------- SAU/IDAU configuration functions -----------------------*/
struct sau_cfg_t {
    uint32_t RBAR;
    uint32_t RLAR;
    bool nsc;
};

void sau_and_idau_cfg(void)
{
//    struct spctrl_def* spctrl = CMSDK_SPCTRL;

    /* Enables SAU */
    TZ_SAU_Enable();

    SAU->RNR  = 0U;
    SAU->RBAR = 0x00000000 & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = (((TCM_TFM_BASE - 1) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk);

    SAU->RNR  = 1U;
    SAU->RBAR = (TCM_TFM_BASE + TCM_TFM_LENGTH) & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = (((VIR_SYSRAM_MASK(SYSRAM_TFM_BASE - 1)) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk);

    SAU->RNR  = 2U;
    SAU->RBAR = VIR_SYSRAM_MASK(SYSRAM_SYS_BASE) & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = ((XIP_TFM_INT_START - 1) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;

    SAU->RNR  = 3U;
    SAU->RBAR = (memory_regions.veneer_base  & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = (((memory_regions.veneer_limit & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk) | SAU_RLAR_NSC_Msk);

    SAU->RNR  = 4U;
    SAU->RBAR = (XIP_RTOS_START - 0x80) & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = ((SYSRAM_TFM_BASE - 1) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;

    SAU->RNR  = 5U;
    SAU->RBAR = SYSRAM_SYS_BASE & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = (PHY_FLASH_MASK(XIP_TFM_INT_START - 1) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;

    SAU->RNR  = 6U;
    SAU->RBAR = PHY_FLASH_MASK(XIP_RTOS_START - 0x80) & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = (0xAFFFFFFF & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;

    SAU->RNR  = 7U;
    SAU->RBAR = 0xE0000000 & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = (0xFFFFFFFF & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;

    /* Allows SAU to define the code region as a NSC */
//    spctrl->nsccfg |= NSCCFG_CODENSC;
}

/*------------------- Memory configuration functions -------------------------*/

int32_t mpc_init_cfg(void)
{
    int32_t ret = ARM_DRIVER_OK;

    ret = Driver_ISRAM0_MPC.Initialize();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }
    MPC_ISRAM0_DEV_S.data->cur_region = 0;
    ret = Driver_ISRAM0_MPC.ConfigRegion(
                                      MPC_ISRAM0_RANGE0_BASE_NS,
                                      MPC_ISRAM0_RANGE0_LIMIT_NS,
                                      SMPU_SEC_ATTR_SRW_NSRW);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    MPC_CODE_SRAM1_DEV_S.data->cur_region = 0;
    ret = Driver_SRAM1_MPC.Initialize();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = Driver_SRAM1_MPC.ConfigRegion(
                                      MPC_CODE_SRAM1_RANGE0_BASE_NS,
                                      MPC_CODE_SRAM1_RANGE0_LIMIT_NS,
                                      SMPU_SEC_ATTR_SRW_NSRW);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    MPC_CODE_SRAM1_DEV_S.data->cur_region = 1;
    ret = Driver_SRAM1_MPC.ConfigRegion(
                                      MPC_CODE_SRAM1_RANGE1_BASE_NS,
                                      MPC_CODE_SRAM1_RANGE1_LIMIT_NS,
                                      SMPU_SEC_ATTR_SRW_NSRW);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    MPC_CODE_SRAM2_DEV_S.data->cur_region = 0;
    ret = Driver_SRAM2_MPC.Initialize();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = Driver_SRAM2_MPC.ConfigRegion(
                                      MPC_CODE_SRAM1_RANGE0_BASE_NS,
                                      MPC_CODE_SRAM1_RANGE0_LIMIT_NS,
                                      SMPU_SEC_ATTR_SRW_NSRW);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }


    /* Lock down the MPC configuration */
    ret = Driver_SRAM1_MPC.LockDown();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Add barriers to assure the MPC configuration is done before continue
     * the execution.
     */
    __DSB();
    __ISB();

    return ARM_DRIVER_OK;
}

/*---------------------- PPC configuration functions -------------------------*/

void ppc_init_cfg(void)
{
#if 0
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    struct nspctrl_def* nspctrl = CMSDK_NSPCTRL;

    /* Grant non-secure access to peripherals in the PPC0
     * (timer0 and 1, dualtimer, watchdog, mhu 0 and 1)
     */
     spctrl->apbnsppc0 |= (1U << CMSDK_TIMER0_APB_PPC_POS) |
                          (1U << CMSDK_TIMER1_APB_PPC_POS) |
                          (1U << CMSDK_DTIMER_APB_PPC_POS) |
                          (1U << CMSDK_MHU0_APB_PPC_POS) |
                          (1U << CMSDK_MHU1_APB_PPC_POS);
    /* Grant non-secure access to S32K Timer in PPC1*/
    spctrl->apbnsppc1 |= (1U << CMSDK_S32K_TIMER_PPC_POS);
    /* Grant non-secure access for APB peripherals on EXP1 */
    spctrl->apbnsppcexp1 |= (1U << CMSDK_SPI0_APB_PPC_POS) |
                            (1U << CMSDK_SPI1_APB_PPC_POS) |
                            (1U << CMSDK_SPI2_APB_PPC_POS) |
                            (1U << CMSDK_SPI3_APB_PPC_POS) |
                            (1U << CMSDK_SPI4_APB_PPC_POS) |
                            (1U << CMSDK_UART0_APB_PPC_POS) |
#ifdef SECURE_UART1
    /* To statically configure a peripheral as secure, skip PPC NS peripheral
     * configuration for the given device.
     */
#else
                            (1U << CMSDK_UART1_APB_PPC_POS) |
#endif
                            (1U << CMSDK_UART2_APB_PPC_POS) |
                            (1U << CMSDK_UART3_APB_PPC_POS) |
                            (1U << CMSDK_UART4_APB_PPC_POS) |
                            (1U << CMSDK_I2C0_APB_PPC_POS) |
                            (1U << CMSDK_I2C1_APB_PPC_POS) |
                            (1U << CMSDK_I2C2_APB_PPC_POS) |
                            (1U << CMSDK_I2C3_APB_PPC_POS);
    /* Grant non-secure access for APB peripherals on EXP2 */
    spctrl->apbnsppcexp2 |= (1U << CMSDK_FPGA_SCC_PPC_POS) |
                            (1U << CMSDK_FPGA_AUDIO_PPC_POS) |
                            (1U << CMSDK_FPGA_IO_PPC_POS);

    /* Grant non-secure access to all peripherals on AHB EXP:
     * Make sure that all possible peripherals are enabled by default
     */
    spctrl->ahbnsppcexp0 |= (1U << CMSDK_VGA_PPC_POS) |
                            (1U << CMSDK_GPIO0_PPC_POS) |
                            (1U << CMSDK_GPIO1_PPC_POS) |
                            (1U << CMSDK_GPIO2_PPC_POS) |
                            (1U << CMSDK_GPIO3_PPC_POS) |
                            (1U << MPS2_ETHERNET_PPC_POS);

    spctrl->ahbnsppcexp1 |= (1U << CMSDK_DMA0_PPC_POS) |
                            (1U << CMSDK_DMA1_PPC_POS) |
                            (1U << CMSDK_DMA2_PPC_POS) |
                            (1U << CMSDK_DMA3_PPC_POS);

    /* in NS, grant un-privileged for UART0 */
    nspctrl->apbnspppcexp1 |= (1U << CMSDK_UART0_APB_PPC_POS);

    /* in NS, grant un-privileged access for LEDs */
    nspctrl->apbnspppcexp2 |= (1U << CMSDK_FPGA_SCC_PPC_POS) |
                              (1U << CMSDK_FPGA_IO_PPC_POS);

    /* Configure the response to a security violation as a
     * bus error instead of RAZ/WI
     */
    spctrl->secrespcfg |= 1U;
#endif
}

void ppc_configure_to_non_secure(enum ppc_bank_e bank, uint16_t pos)
{
    /* Setting NS flag for peripheral to enable NS access */
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    ((uint32_t*)&(spctrl->ahbnsppc0))[bank] |= (1U << pos);
}

void ppc_configure_to_secure(enum ppc_bank_e bank, uint16_t pos)
{
    /* Clear NS flag for peripheral to prevent NS access */
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    ((uint32_t*)&(spctrl->ahbnsppc0))[bank] &= ~(1U << pos);
}

void ppc_en_secure_unpriv(enum ppc_bank_e bank, uint16_t pos)
{
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    ((uint32_t*)&(spctrl->ahbspppc0))[bank] |= (1U << pos);
}

void ppc_clr_secure_unpriv(enum ppc_bank_e bank, uint16_t pos)
{
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    ((uint32_t*)&(spctrl->ahbspppc0))[bank] &= ~(1U << pos);
}

void ppc_clear_irq(void)
{
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    /* Clear APB PPC EXP2 IRQ */
    spctrl->secppcintclr = CMSDK_APB_PPCEXP2_INT_POS_MASK;
}

//disable all asic_mpu protection
#ifndef TFM_ENABLE_PLATFORM_ISOLATION
void asic_mpu_disable_all_protect(void)
{
    for (uint32_t region = 0; region < REGION_NUM; region++) {
        hal_asic_mpu_set_region_apc(HAL_ASIC_MPU_TYPE_FLASH, region, -1, 0);
        hal_asic_mpu_set_region_apc(HAL_ASIC_MPU_TYPE_SYSRAM, region, -1, 0);
        hal_asic_mpu_set_region_apc(HAL_ASIC_MPU_TYPE_PSRAM, region, -1, 0);
        hal_asic_mpu_set_region_apc(HAL_ASIC_MPU_TYPE_TCM, region, -1, 0);
    }
}
#endif /* #ifndef TFM_ENABLE_PLATFORM_ISOLATION */
