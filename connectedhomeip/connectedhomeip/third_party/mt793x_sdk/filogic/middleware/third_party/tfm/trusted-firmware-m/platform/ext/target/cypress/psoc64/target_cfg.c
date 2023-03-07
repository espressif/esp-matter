/*
 * Copyright (c) 2018 Arm Limited
 * Copyright (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
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

#include <assert.h>
#include <stdio.h> /* for debugging printfs */
#include <inttypes.h>

#include "cy_prot.h"
#include "cycfg.h"
#include "device_definition.h"
#include "driver_ppu.h"
#include "driver_smpu.h"
#include "pc_config.h"
#include "platform_description.h"
#include "region_defs.h"
#include "RTE_Device.h"
#include "target_cfg.h"
#include "tfm_plat_defs.h"
#include "region.h"


/* The section names come from the scatter file */
REGION_DECLARE(Load$$LR$$, LR_NS_PARTITION, $$Base);
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
};


#ifdef BL2
REGION_DECLARE(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base);
#endif /* BL2 */

/* To write into AIRCR register, 0x5FA value must be write to the VECTKEY field,
 * otherwise the processor ignores the write.
 */
#define SCB_AIRCR_WRITE_MASK ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos))

struct tfm_spm_partition_platform_data_t tfm_peripheral_std_uart = {
        SCB5_BASE,
        SCB5_BASE + 0xFFF,
        -1,
        -1
};

struct tfm_spm_partition_platform_data_t tfm_peripheral_timer0 = {
        TCPWM0_BASE,
        TCPWM0_BASE + (sizeof(TCPWM_Type) - 1),
        -1,
        -1
};

void enable_fault_handlers(void)
{
    /* Fault handles enable registers are not present in Cortex-M0+ */
}

void system_reset_cfg(void)
{
    uint32_t reg_value = SCB->AIRCR;

    /* Clear SCB_AIRCR_VECTKEY value */
    reg_value &= ~(uint32_t)(SCB_AIRCR_VECTKEY_Msk);

    /* Set Access Key (0x05FA must be written to this field) */
    reg_value |= (uint32_t)(SCB_AIRCR_WRITE_MASK);

    SCB->AIRCR = reg_value;
}

extern void Cy_Platform_Init(void);
void platform_init(void)
{
#ifdef TFM_ENABLE_IRQ_TEST
    cy_en_sysint_status_t rc;
#endif

    Cy_PDL_Init(CY_DEVICE_CFG);

    init_cycfg_all();
    Cy_Platform_Init();

#ifdef TFM_ENABLE_IRQ_TEST
    rc = Cy_SysInt_Init(&CY_TCPWM_NVIC_CFG_S, TFM_TIMER0_IRQ_Handler);
    if (rc != CY_SYSINT_SUCCESS) {
        printf("WARNING: Fail to initialize timer interrupt (IRQ TEST might fail)!\n");
    }
#endif /* TFM_ENABLE_IRQ_TEST */

    /* make sure CM4 is disabled */
    if (CY_SYS_CM4_STATUS_ENABLED == Cy_SysGetCM4Status()) {
        Cy_SysDisableCM4();
    }
}

enum tfm_plat_err_t nvic_interrupt_target_state_cfg(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t nvic_interrupt_enable(void)
{
    /* PPU and SMPU don't generate interrupts.
     * USART and Flash drivers don't export an EnableInterrupt function.
     * So there's nothing to do here.
     */
    return TFM_PLAT_ERR_SUCCESS;
}

static cy_en_prot_status_t set_bus_master_attr(void)
{
    cy_en_prot_status_t ret;

    /* Cortex-M4 - PC=6 */
    ret = Cy_Prot_SetActivePC(CPUSS_MS_ID_CM4, CY_PROT_HOST_DEFAULT);
    if (ret != CY_PROT_SUCCESS) {
        return ret;
    }

    /* Test Controller - PC=7 */
    ret = Cy_Prot_SetActivePC(CPUSS_MS_ID_TC, CY_PROT_TC);
    if (ret != CY_PROT_SUCCESS) {
        return ret;
    }

    /* Cortex-M0+ - PC=1 */
    ret = Cy_Prot_SetActivePC(CPUSS_MS_ID_CM0, CY_PROT_SPM_DEFAULT);
    if (ret != CY_PROT_SUCCESS) {
        return ret;
    }

    return CY_PROT_SUCCESS;
}

void bus_masters_cfg(void)
{
    cy_en_prot_status_t ret;
    ret = set_bus_master_attr();
    assert(ret == CY_PROT_SUCCESS);
    (void)ret;
}

const SMPU_Resources *smpu_init_table[] = {
#if RTE_SMPU0
    &SMPU0_Resources,
#endif

#if RTE_SMPU1
    &SMPU1_Resources,
#endif

#if RTE_SMPU2
    &SMPU2_Resources,
#endif

#if RTE_SMPU3
    &SMPU3_Resources,
#endif

#if RTE_SMPU4
    &SMPU4_Resources,
#endif

#if RTE_SMPU5
    &SMPU5_Resources,
#endif

#if RTE_SMPU6
    &SMPU6_Resources,
#endif

#if RTE_SMPU7
    &SMPU7_Resources,
#endif

#if RTE_SMPU8
    &SMPU8_Resources,
#endif

#if RTE_SMPU9
    &SMPU9_Resources,
#endif

#if RTE_SMPU10
    &SMPU10_Resources,
#endif

#if RTE_SMPU11
    &SMPU11_Resources,
#endif

#if RTE_SMPU12
    &SMPU12_Resources,
#endif

#if RTE_SMPU13
    &SMPU13_Resources,
#endif
};

void smpu_init_cfg(void)
{
    cy_en_prot_status_t ret;

    size_t n = sizeof(smpu_init_table)/sizeof(smpu_init_table[0]);

    for (int i = (n - 1); i >= 0; i--)
    {
        ret = SMPU_Configure(smpu_init_table[i]);
        assert(ret == CY_PROT_SUCCESS);
    }

    /* Now protect all unconfigured SMPUs */
    ret = protect_unconfigured_smpus();
    assert(ret == CY_PROT_SUCCESS);
    (void)ret;

    __DSB();
    __ISB();
}

void smpu_print_config(void)
{
    printf("\nSMPU config:\n");
    printf("memory_regions.non_secure_code_start = %#"PRIx32"\n",
           memory_regions.non_secure_code_start);
    printf("memory_regions.non_secure_partition_base = %#"PRIx32"\n",
           memory_regions.non_secure_partition_base);
    printf("memory_regions.non_secure_partition_limit = %#"PRIx32"\n",
           memory_regions.non_secure_partition_limit);

    size_t n = sizeof(smpu_init_table)/sizeof(smpu_init_table[0]);

    for (int i = 0; i < n; i++)
    {
        SMPU_Print_Config(smpu_init_table[i]);
    }
}

const PPU_Resources *ppu_init_table[] = {
#if RTE_MS_PPU_PR7
&PR7_PPU_Resources,
#endif
#if RTE_MS_PPU_PERI_MAIN
    &PERI_MAIN_PPU_Resources,
#endif

#if RTE_MS_PPU_PERI_GR0_GROUP
    &PERI_GR0_GROUP_PPU_Resources,
#endif

#if RTE_MS_PPU_PERI_GR1_GROUP
    &PERI_GR1_GROUP_PPU_Resources,
#endif

#if RTE_MS_PPU_PERI_GR2_GROUP
    &PERI_GR2_GROUP_PPU_Resources,
#endif

#if RTE_MS_PPU_PERI_GR3_GROUP
    &PERI_GR3_GROUP_PPU_Resources,
#endif

#if RTE_MS_PPU_PERI_GR4_GROUP
    &PERI_GR4_GROUP_PPU_Resources,
#endif

#if RTE_MS_PPU_PERI_GR6_GROUP
    &PERI_GR6_GROUP_PPU_Resources,
#endif

#if RTE_MS_PPU_PERI_GR9_GROUP
    &PERI_GR9_GROUP_PPU_Resources,
#endif

#if RTE_MS_PPU_PERI_GR10_GROUP
    &PERI_GR10_GROUP_PPU_Resources,
#endif

#if RTE_MS_PPU_PERI_TR
    &PERI_TR_PPU_Resources,
#endif

#if RTE_MS_PPU_CRYPTO_MAIN
    &CRYPTO_MAIN_PPU_Resources,
#endif

#if RTE_MS_PPU_CRYPTO_CRYPTO
    &CRYPTO_CRYPTO_PPU_Resources,
#endif

#if RTE_MS_PPU_CRYPTO_BOOT
    &CRYPTO_BOOT_PPU_Resources,
#endif

#if RTE_MS_PPU_CRYPTO_KEY0
    &CRYPTO_KEY0_PPU_Resources,
#endif

#if RTE_MS_PPU_CRYPTO_KEY1
    &CRYPTO_KEY1_PPU_Resources,
#endif

#if RTE_MS_PPU_CRYPTO_BUF
    &CRYPTO_BUF_PPU_Resources,
#endif

#if RTE_MS_PPU_CPUSS_CM4
    &CPUSS_CM4_PPU_Resources,
#endif

#if RTE_MS_PPU_CPUSS_CM0
    &CPUSS_CM0_PPU_Resources,
#endif

#if RTE_MS_PPU_CPUSS_BOOT
    &CPUSS_BOOT_PPU_Resources,
#endif

#if RTE_MS_PPU_CPUSS_CM0_INT
    &CPUSS_CM0_INT_PPU_Resources,
#endif

#if RTE_MS_PPU_CPUSS_CM4_INT
    &CPUSS_CM4_INT_PPU_Resources,
#endif

#if RTE_MS_PPU_FAULT_STRUCT0_MAIN
    &FAULT_STRUCT0_MAIN_PPU_Resources,
#endif

#if RTE_MS_PPU_FAULT_STRUCT1_MAIN
    &FAULT_STRUCT1_MAIN_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT0_IPC
    &IPC_STRUCT0_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT1_IPC
    &IPC_STRUCT1_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT2_IPC
    &IPC_STRUCT2_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT3_IPC
    &IPC_STRUCT3_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT4_IPC
    &IPC_STRUCT4_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT5_IPC
    &IPC_STRUCT5_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT6_IPC
    &IPC_STRUCT6_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT7_IPC
    &IPC_STRUCT7_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT8_IPC
    &IPC_STRUCT8_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT9_IPC
    &IPC_STRUCT9_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT10_IPC
    &IPC_STRUCT10_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT11_IPC
    &IPC_STRUCT11_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT12_IPC
    &IPC_STRUCT12_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT13_IPC
    &IPC_STRUCT13_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT14_IPC
    &IPC_STRUCT14_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_STRUCT15_IPC
    &IPC_STRUCT15_IPC_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT0_INTR
    &IPC_INTR_STRUCT0_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT1_INTR
    &IPC_INTR_STRUCT1_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT2_INTR
    &IPC_INTR_STRUCT2_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT3_INTR
    &IPC_INTR_STRUCT3_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT4_INTR
    &IPC_INTR_STRUCT4_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT5_INTR
    &IPC_INTR_STRUCT5_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT6_INTR
    &IPC_INTR_STRUCT6_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT7_INTR
    &IPC_INTR_STRUCT7_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT8_INTR
    &IPC_INTR_STRUCT8_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT9_INTR
    &IPC_INTR_STRUCT9_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT10_INTR
    &IPC_INTR_STRUCT10_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT11_INTR
    &IPC_INTR_STRUCT11_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT12_INTR
    &IPC_INTR_STRUCT12_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT13_INTR
    &IPC_INTR_STRUCT13_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT14_INTR
    &IPC_INTR_STRUCT14_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT15_INTR
    &IPC_INTR_STRUCT15_INTR_PPU_Resources,
#endif

#if RTE_MS_PPU_PROT_SMPU_MAIN
    &PROT_SMPU_MAIN_PPU_Resources,
#endif

#if RTE_MS_PPU_PROT_MPU0_MAIN
    &PROT_MPU0_MAIN_PPU_Resources,
#endif

#if RTE_MS_PPU_PROT_MPU5_MAIN
    &PROT_MPU5_MAIN_PPU_Resources,
#endif

#if RTE_MS_PPU_PROT_MPU6_MAIN
    &PROT_MPU6_MAIN_PPU_Resources,
#endif

#if RTE_MS_PPU_PROT_MPU14_MAIN
    &PROT_MPU14_MAIN_PPU_Resources,
#endif

#if RTE_MS_PPU_PROT_MPU15_MAIN
    &PROT_MPU15_MAIN_PPU_Resources,
#endif

#if RTE_MS_PPU_FLASHC_MAIN
    &FLASHC_MAIN_PPU_Resources,
#endif

#if RTE_MS_PPU_FLASHC_CMD
    &FLASHC_CMD_PPU_Resources,
#endif

#if RTE_MS_PPU_FLASHC_DFT
    &FLASHC_DFT_PPU_Resources,
#endif

#if RTE_MS_PPU_FLASHC_CM0
    &FLASHC_CM0_PPU_Resources,
#endif

#if RTE_MS_PPU_FLASHC_CM4
    &FLASHC_CM4_PPU_Resources,
#endif

#if RTE_MS_PPU_FLASHC_CRYPTO
    &FLASHC_CRYPTO_PPU_Resources,
#endif

#if RTE_MS_PPU_FLASHC_DW0
    &FLASHC_DW0_PPU_Resources,
#endif

#if RTE_MS_PPU_FLASHC_DW1
    &FLASHC_DW1_PPU_Resources,
#endif

#if RTE_MS_PPU_FLASHC_DMAC
    &FLASHC_DMAC_PPU_Resources,
#endif

#if RTE_MS_PPU_FLASHC_EXT_MS0
    &FLASHC_EXT_MS0_PPU_Resources,
#endif

#if RTE_MS_PPU_FLASHC_EXT_MS1
    &FLASHC_EXT_MS1_PPU_Resources,
#endif

#if RTE_MS_PPU_FLASHC_FM
    &FLASHC_FM_PPU_Resources,
#endif

#if RTE_MS_PPU_SRSS_MAIN1
    &SRSS_MAIN1_PPU_Resources,
#endif

#if RTE_MS_PPU_SRSS_MAIN2
    &SRSS_MAIN2_PPU_Resources,
#endif

#if RTE_MS_PPU_WDT
    &WDT_PPU_Resources,
#endif

#if RTE_MS_PPU_MAIN
    &MAIN_PPU_Resources,
#endif

#if RTE_MS_PPU_SRSS_MAIN3
    &SRSS_MAIN3_PPU_Resources,
#endif

#if RTE_MS_PPU_SRSS_MAIN4
    &SRSS_MAIN4_PPU_Resources,
#endif

#if RTE_MS_PPU_SRSS_MAIN5
    &SRSS_MAIN5_PPU_Resources,
#endif

#if RTE_MS_PPU_SRSS_MAIN6
    &SRSS_MAIN6_PPU_Resources,
#endif

#if RTE_MS_PPU_SRSS_MAIN7
    &SRSS_MAIN7_PPU_Resources,
#endif

#if RTE_MS_PPU_BACKUP_BACKUP
    &BACKUP_BACKUP_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_DW
    &DW0_DW_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_DW
    &DW1_DW_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_DW_CRC
    &DW0_DW_CRC_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_DW_CRC
    &DW1_DW_CRC_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT0_CH
    &DW0_CH_STRUCT0_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT1_CH
    &DW0_CH_STRUCT1_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT2_CH
    &DW0_CH_STRUCT2_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT3_CH
    &DW0_CH_STRUCT3_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT4_CH
    &DW0_CH_STRUCT4_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT5_CH
    &DW0_CH_STRUCT5_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT6_CH
    &DW0_CH_STRUCT6_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT7_CH
    &DW0_CH_STRUCT7_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT8_CH
    &DW0_CH_STRUCT8_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT9_CH
    &DW0_CH_STRUCT9_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT10_CH
    &DW0_CH_STRUCT10_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT11_CH
    &DW0_CH_STRUCT11_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT12_CH
    &DW0_CH_STRUCT12_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT13_CH
    &DW0_CH_STRUCT13_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT14_CH
    &DW0_CH_STRUCT14_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT15_CH
    &DW0_CH_STRUCT15_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT16_CH
    &DW0_CH_STRUCT16_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT17_CH
    &DW0_CH_STRUCT17_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT18_CH
    &DW0_CH_STRUCT18_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT19_CH
    &DW0_CH_STRUCT19_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT20_CH
    &DW0_CH_STRUCT20_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT21_CH
    &DW0_CH_STRUCT21_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT22_CH
    &DW0_CH_STRUCT22_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT23_CH
    &DW0_CH_STRUCT23_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT24_CH
    &DW0_CH_STRUCT24_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT25_CH
    &DW0_CH_STRUCT25_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT26_CH
    &DW0_CH_STRUCT26_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT27_CH
    &DW0_CH_STRUCT27_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT28_CH
    &DW0_CH_STRUCT28_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT0_CH
    &DW1_CH_STRUCT0_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT1_CH
    &DW1_CH_STRUCT1_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT2_CH
    &DW1_CH_STRUCT2_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT3_CH
    &DW1_CH_STRUCT3_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT4_CH
    &DW1_CH_STRUCT4_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT5_CH
    &DW1_CH_STRUCT5_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT6_CH
    &DW1_CH_STRUCT6_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT7_CH
    &DW1_CH_STRUCT7_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT8_CH
    &DW1_CH_STRUCT8_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT9_CH
    &DW1_CH_STRUCT9_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT10_CH
    &DW1_CH_STRUCT10_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT11_CH
    &DW1_CH_STRUCT11_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT12_CH
    &DW1_CH_STRUCT12_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT13_CH
    &DW1_CH_STRUCT13_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT14_CH
    &DW1_CH_STRUCT14_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT15_CH
    &DW1_CH_STRUCT15_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT16_CH
    &DW1_CH_STRUCT16_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT17_CH
    &DW1_CH_STRUCT17_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT18_CH
    &DW1_CH_STRUCT18_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT19_CH
    &DW1_CH_STRUCT19_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT20_CH
    &DW1_CH_STRUCT20_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT21_CH
    &DW1_CH_STRUCT21_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT22_CH
    &DW1_CH_STRUCT22_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT23_CH
    &DW1_CH_STRUCT23_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT24_CH
    &DW1_CH_STRUCT24_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT25_CH
    &DW1_CH_STRUCT25_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT26_CH
    &DW1_CH_STRUCT26_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT27_CH
    &DW1_CH_STRUCT27_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT28_CH
    &DW1_CH_STRUCT28_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DMAC_TOP
    &DMAC_TOP_PPU_Resources,
#endif

#if RTE_MS_PPU_DMAC_CH0_CH
    &DMAC_CH0_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DMAC_CH1_CH
    &DMAC_CH1_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DMAC_CH2_CH
    &DMAC_CH2_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_DMAC_CH3_CH
    &DMAC_CH3_CH_PPU_Resources,
#endif

#if RTE_MS_PPU_EFUSE_CTL
    &EFUSE_CTL_PPU_Resources,
#endif

#if RTE_MS_PPU_EFUSE_DATA
    &EFUSE_DATA_PPU_Resources,
#endif

#if RTE_MS_PPU_PROFILE
    &PROFILE_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT0_PRT
    &HSIOM_PRT0_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT1_PRT
    &HSIOM_PRT1_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT2_PRT
    &HSIOM_PRT2_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT3_PRT
    &HSIOM_PRT3_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT4_PRT
    &HSIOM_PRT4_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT5_PRT
    &HSIOM_PRT5_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT6_PRT
    &HSIOM_PRT6_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT7_PRT
    &HSIOM_PRT7_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT8_PRT
    &HSIOM_PRT8_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT9_PRT
    &HSIOM_PRT9_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT10_PRT
    &HSIOM_PRT10_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT11_PRT
    &HSIOM_PRT11_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT12_PRT
    &HSIOM_PRT12_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT13_PRT
    &HSIOM_PRT13_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_PRT14_PRT
    &HSIOM_PRT14_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_AMUX
    &HSIOM_AMUX_PPU_Resources,
#endif

#if RTE_MS_PPU_HSIOM_MON
    &HSIOM_MON_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT0_PRT
    &GPIO_PRT0_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT1_PRT
    &GPIO_PRT1_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT2_PRT
    &GPIO_PRT2_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT3_PRT
    &GPIO_PRT3_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT4_PRT
    &GPIO_PRT4_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT5_PRT
    &GPIO_PRT5_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT6_PRT
    &GPIO_PRT6_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT7_PRT
    &GPIO_PRT7_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT8_PRT
    &GPIO_PRT8_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT9_PRT
    &GPIO_PRT9_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT10_PRT
    &GPIO_PRT10_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT11_PRT
    &GPIO_PRT11_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT12_PRT
    &GPIO_PRT12_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT13_PRT
    &GPIO_PRT13_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT14_PRT
    &GPIO_PRT14_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT0_CFG
    &GPIO_PRT0_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT1_CFG
    &GPIO_PRT1_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT2_CFG
    &GPIO_PRT2_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT3_CFG
    &GPIO_PRT3_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT4_CFG
    &GPIO_PRT4_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT5_CFG
    &GPIO_PRT5_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT6_CFG
    &GPIO_PRT6_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT7_CFG
    &GPIO_PRT7_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT8_CFG
    &GPIO_PRT8_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT9_CFG
    &GPIO_PRT9_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT10_CFG
    &GPIO_PRT10_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT11_CFG
    &GPIO_PRT11_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT12_CFG
    &GPIO_PRT12_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT13_CFG
    &GPIO_PRT13_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_PRT14_CFG
    &GPIO_PRT14_CFG_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_GPIO
    &GPIO_GPIO_PPU_Resources,
#endif

#if RTE_MS_PPU_GPIO_TEST
    &GPIO_TEST_PPU_Resources,
#endif

#if RTE_MS_PPU_SMARTIO_PRT8_PRT
    &SMARTIO_PRT8_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_SMARTIO_PRT9_PRT
    &SMARTIO_PRT9_PRT_PPU_Resources,
#endif

#if RTE_MS_PPU_LPCOMP
    &LPCOMP_PPU_Resources,
#endif

#if RTE_MS_PPU_CSD0
    &CSD0_PPU_Resources,
#endif

#if RTE_MS_PPU_TCPWM0
    &TCPWM0_PPU_Resources,
#endif

#if RTE_MS_PPU_TCPWM1
    &TCPWM1_PPU_Resources,
#endif

#if RTE_MS_PPU_LCD0
    &LCD0_PPU_Resources,
#endif

#if RTE_MS_PPU_USBFS0
    &USBFS0_PPU_Resources,
#endif

#if RTE_MS_PPU_SMIF0
    &SMIF0_PPU_Resources,
#endif

#if RTE_MS_PPU_SDHC0
    &SDHC0_PPU_Resources,
#endif

#if RTE_MS_PPU_SDHC1
    &SDHC1_PPU_Resources,
#endif

#if RTE_MS_PPU_SCB0
    &SCB0_PPU_Resources,
#endif

#if RTE_MS_PPU_SCB1
    &SCB1_PPU_Resources,
#endif

#if RTE_MS_PPU_SCB2
    &SCB2_PPU_Resources,
#endif

#if RTE_MS_PPU_SCB3
    &SCB3_PPU_Resources,
#endif

#if RTE_MS_PPU_SCB4
    &SCB4_PPU_Resources,
#endif

#if RTE_MS_PPU_SCB5
    &SCB5_PPU_Resources,
#endif

#if RTE_MS_PPU_SCB6
    &SCB6_PPU_Resources,
#endif

#if RTE_MS_PPU_SCB7
    &SCB7_PPU_Resources,
#endif

#if RTE_MS_PPU_SCB8
    &SCB8_PPU_Resources,
#endif

#if RTE_MS_PPU_SCB9
    &SCB9_PPU_Resources,
#endif

#if RTE_MS_PPU_SCB10
    &SCB10_PPU_Resources,
#endif

#if RTE_MS_PPU_SCB11
    &SCB11_PPU_Resources,
#endif

#if RTE_MS_PPU_SCB12
    &SCB12_PPU_Resources,
#endif

#if RTE_MS_PPU_PDM0
    &PDM0_PPU_Resources,
#endif

#if RTE_MS_PPU_I2S0
    &I2S0_PPU_Resources,
#endif

#if RTE_MS_PPU_I2S1
    &I2S1_PPU_Resources,
#endif
};

void ppu_init_cfg(void)
{
    size_t n = sizeof(ppu_init_table)/sizeof(ppu_init_table[0]);

    for (int i = 0; i < n; i++)
    {
        cy_en_prot_status_t ret = PPU_Configure(ppu_init_table[i]);
        assert(ret == CY_PROT_SUCCESS);
        (void)ret;
    }

    __DSB();
    __ISB();
}
