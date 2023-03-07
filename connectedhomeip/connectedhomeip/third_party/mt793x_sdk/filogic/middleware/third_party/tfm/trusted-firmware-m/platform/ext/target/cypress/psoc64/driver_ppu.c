/*
 * Copyright (c) 2019, Cypress Semiconductor Corporation. All rights reserved.
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

#include <stdio.h>

#include "driver_ppu.h"
#include "pc_config.h"
#include "ppu_config.h"
#include "RTE_Device.h"

#include "cy_device_headers.h"
#include "cy_prot.h"

struct ms_ppu_config {
    uint16_t pcMask;
    cy_en_prot_perm_t user;
    cy_en_prot_perm_t priv;
    bool secure;
};

struct ppu_resources {
    enum ppu_type ppu_type;
    union ppu {
        PERI_MS_PPU_PR_Type *ms_ppu_pr;
        PERI_MS_PPU_FX_Type *ms_ppu_fx;
        PERI_PPU_PR_Type *ppu_pr;
        PERI_PPU_GR_Type *ppu_gr;
        PERI_GR_PPU_SL_Type *gr_ppu_sl;
        PERI_GR_PPU_RG_Type *gr_ppu_rg;
    } ppu;
    union master_config {
        struct ms_ppu_config ms_ppu;
        cy_stc_ppu_prog_cfg_t ppu_pr;
        cy_stc_ppu_gr_cfg_t ppu_gr;
        cy_stc_ppu_sl_cfg_t gr_ppu_sl;
        cy_stc_ppu_rg_cfg_t gr_ppu_rg;
    } master_cfg;
    union slave_config {
        struct ms_ppu_config ms_ppu;
        cy_stc_ppu_prog_cfg_t ppu_pr;
        cy_stc_ppu_gr_cfg_t ppu_gr;
        cy_stc_ppu_sl_cfg_t gr_ppu_sl;
        cy_stc_ppu_rg_cfg_t gr_ppu_rg;
    } slave_cfg;
    /* These are only applicable when ppu_type is MS_PPU_PR */
    uint32_t slave_address;
    cy_en_prot_size_t slave_region_size;
};

/* Affect all 8 subregions */
#define ALL_ENABLED 0

/* The PDL doesn't work if we pass in pcMask values that are not supported by the hardware */
/* Note that this macro depends on the values of CY_PROT_PCMASKx macros */
#define FILTER_PCS(x) ((x) & ((1<<(PERI_PC_NR-1))-1))

/* Shared Driver wrapper functions */
cy_en_prot_status_t PPU_Configure(const PPU_Resources *ppu_dev)
{
    cy_en_prot_status_t ret;

    switch(ppu_dev->ppu_type) {
/* This block is only needed if there are MS_PPU_PR PPUs on the board */
#if defined(PERI_MS_PPU_PR0)
    case MS_PPU_PR:
        ret = Cy_Prot_ConfigPpuProgSlaveAddr(ppu_dev->ppu.ms_ppu_pr,
                                             ppu_dev->slave_address,
                                             ppu_dev->slave_region_size);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        /* Disable access from any other Protection Contexts */
        ret = Cy_Prot_ConfigPpuProgSlaveAtt(ppu_dev->ppu.ms_ppu_pr,
                                            FILTER_PCS(~ppu_dev->slave_cfg.ms_ppu.pcMask),
                                            CY_PROT_PERM_DISABLED,
                                            CY_PROT_PERM_DISABLED,
                                            true);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_ConfigPpuProgSlaveAtt(ppu_dev->ppu.ms_ppu_pr,
                                            FILTER_PCS(ppu_dev->slave_cfg.ms_ppu.pcMask),
                                            ppu_dev->slave_cfg.ms_ppu.user,
                                            ppu_dev->slave_cfg.ms_ppu.priv,
                                            ppu_dev->slave_cfg.ms_ppu.secure);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_EnablePpuProgSlaveRegion(ppu_dev->ppu.ms_ppu_pr);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        /* Read-only access from any other Protection Contexts */
        ret = Cy_Prot_ConfigPpuProgMasterAtt(ppu_dev->ppu.ms_ppu_pr,
                                            FILTER_PCS(~ppu_dev->master_cfg.ms_ppu.pcMask),
                                            CY_PROT_PERM_R,
                                            CY_PROT_PERM_R,
                                            true);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_ConfigPpuProgMasterAtt(ppu_dev->ppu.ms_ppu_pr,
                                             FILTER_PCS(ppu_dev->master_cfg.ms_ppu.pcMask),
                                             ppu_dev->master_cfg.ms_ppu.user,
                                             ppu_dev->master_cfg.ms_ppu.priv,
                                             ppu_dev->master_cfg.ms_ppu.secure);
        break;
#endif

/* This block is only needed if there are MS_PPU_FX PPUs on the board */
#if defined(PERI_MS_PPU_FX_PERI_MAIN)
    case MS_PPU_FX:
        /* Disable access from any other Protection Contexts */
        ret = Cy_Prot_ConfigPpuFixedSlaveAtt(ppu_dev->ppu.ms_ppu_fx,
                                             FILTER_PCS(~ppu_dev->slave_cfg.ms_ppu.pcMask),
                                             CY_PROT_PERM_DISABLED,
                                             CY_PROT_PERM_DISABLED,
                                             true);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_ConfigPpuFixedSlaveAtt(ppu_dev->ppu.ms_ppu_fx,
                                             FILTER_PCS(ppu_dev->slave_cfg.ms_ppu.pcMask),
                                             ppu_dev->slave_cfg.ms_ppu.user,
                                             ppu_dev->slave_cfg.ms_ppu.priv,
                                             ppu_dev->slave_cfg.ms_ppu.secure);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        /* Read-only access from any other Protection Contexts */
        ret = Cy_Prot_ConfigPpuFixedMasterAtt(ppu_dev->ppu.ms_ppu_fx,
                                              FILTER_PCS(~ppu_dev->master_cfg.ms_ppu.pcMask),
                                              CY_PROT_PERM_R,
                                              CY_PROT_PERM_R,
                                              true);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_ConfigPpuFixedMasterAtt(ppu_dev->ppu.ms_ppu_fx,
                                              FILTER_PCS(ppu_dev->master_cfg.ms_ppu.pcMask),
                                              ppu_dev->master_cfg.ms_ppu.user,
                                              ppu_dev->master_cfg.ms_ppu.priv,
                                              ppu_dev->master_cfg.ms_ppu.secure);
        break;
#endif

/* This block is only needed if there are PPU_PR PPUs on the board */
#if defined(PERI_PPU_PR0)
    case PPU_PR:
        ret = Cy_Prot_ConfigPpuProgSlaveStruct(ppu_dev->ppu.ppu_pr,
                                               &ppu_dev->slave_cfg.ppu_pr);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_EnablePpuProgSlaveStruct(ppu_dev->ppu.ppu_pr);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_ConfigPpuProgMasterStruct(ppu_dev->ppu.ppu_pr,
                                                &ppu_dev->master_cfg.ppu_pr);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_EnablePpuProgMasterStruct(ppu_dev->ppu.ppu_pr);
        break;
#endif

/* This block is only needed if there are PPU_GR PPUs on the board */
#if defined(PERI_PPU_GR0)
    case PPU_GR:
        ret = Cy_Prot_ConfigPpuFixedGrSlaveStruct(ppu_dev->ppu.ppu_gr,
                                                  &ppu_dev->slave_cfg.ppu_gr);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_EnablePpuFixedGrSlaveStruct(ppu_dev->ppu.ppu_gr);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_ConfigPpuFixedGrMasterStruct(ppu_dev->ppu.ppu_gr,
                                                   &ppu_dev->master_cfg.ppu_gr);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_EnablePpuFixedGrMasterStruct(ppu_dev->ppu.ppu_gr);
        break;
#endif

/* This block is only needed if there are GR_PPU_SL PPUs on the board */
#if defined(PERI_GR_PPU_SL_CRYPTO)
    case GR_PPU_SL:
        ret = Cy_Prot_ConfigPpuFixedSlSlaveStruct(ppu_dev->ppu.gr_ppu_sl,
                                                  &ppu_dev->slave_cfg.gr_ppu_sl);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_EnablePpuFixedSlSlaveStruct(ppu_dev->ppu.gr_ppu_sl);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_ConfigPpuFixedSlMasterStruct(ppu_dev->ppu.gr_ppu_sl,
                                                   &ppu_dev->master_cfg.gr_ppu_sl);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_EnablePpuFixedSlMasterStruct(ppu_dev->ppu.gr_ppu_sl);
        break;
#endif

/* This block is only needed if there are GR_PPU_RG PPUs on the board */
#if defined(PERI_GR_PPU_RG_IPC_STRUCT0)
    case GR_PPU_RG:
        ret = Cy_Prot_ConfigPpuFixedRgSlaveStruct(ppu_dev->ppu.gr_ppu_rg,
                                                  &ppu_dev->slave_cfg.gr_ppu_rg);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_EnablePpuFixedRgSlaveStruct(ppu_dev->ppu.gr_ppu_rg);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_ConfigPpuFixedRgMasterStruct(ppu_dev->ppu.gr_ppu_rg,
                                                   &ppu_dev->master_cfg.gr_ppu_rg);
        if (ret != CY_PROT_SUCCESS)
            return ret;
        ret = Cy_Prot_EnablePpuFixedRgMasterStruct(ppu_dev->ppu.gr_ppu_rg);
        break;
#endif

    default:
        printf("Unexpected peripheral type %d\n", ppu_dev->ppu_type);
        return CY_PROT_BAD_PARAM;
    }

    return ret;
}

#define DEFINE_MS_PPU_PR(N) const PPU_Resources N##_PPU_Resources = { \
    .ppu_type = MS_PPU_PR, \
    .ppu = {.ms_ppu_pr = PERI_MS_PPU_##N}, \
    .master_cfg.ms_ppu = PPU_##N##_MASTER_CONFIG, \
    .slave_cfg.ms_ppu = PPU_##N##_SLAVE_CONFIG, \
    .slave_address = PPU_##N##_SLAVE_ADDRESS, \
    .slave_region_size = PPU_##N##_SLAVE_REGION_SIZE, \
};

#define DEFINE_MS_PPU_FX(N) const PPU_Resources N##_PPU_Resources = { \
    .ppu_type = MS_PPU_FX, \
    .ppu = {.ms_ppu_fx = PERI_MS_PPU_FX_##N}, \
    .master_cfg.ms_ppu = PPU_##N##_MASTER_CONFIG, \
    .slave_cfg.ms_ppu = PPU_##N##_SLAVE_CONFIG, \
};

#define DEFINE_PPU_PR(N) const PPU_Resources N##_PPU_Resources = { \
    .ppu_type = PPU_PR, \
    .ppu = {.ppu_pr = PERI_PPU_##N}, \
    .master_cfg.ppu_pr = PPU_##N##_MASTER_CONFIG, \
    .slave_cfg.ppu_pr = PPU_##N##_SLAVE_CONFIG, \
};

#define DEFINE_PPU_GR(N) const PPU_Resources N##_PPU_Resources = { \
    .ppu_type = PPU_GR, \
    .ppu = {.ppu_gr = PERI_PPU_##N}, \
    .master_cfg.ppu_gr = PPU_##N##_MASTER_CONFIG, \
    .slave_cfg.ppu_gr = PPU_##N##_SLAVE_CONFIG, \
};

#define DEFINE_GR_PPU_SL(N) const PPU_Resources N##_PPU_Resources = { \
    .ppu_type = GR_PPU_SL, \
    .ppu = {.gr_ppu_sl = PERI_GR_PPU_##N}, \
    .master_cfg.gr_ppu_sl = PPU_##N##_MASTER_CONFIG, \
    .slave_cfg.gr_ppu_sl = PPU_##N##_SLAVE_CONFIG, \
};

#define DEFINE_GR_PPU_RG(N) const PPU_Resources N##_PPU_Resources = { \
    .ppu_type = GR_PPU_RG, \
    .ppu = {.gr_ppu_rg = PERI_GR_PPU_##N}, \
    .master_cfg.gr_ppu_rg = PPU_##N##_MASTER_CONFIG, \
    .slave_cfg.gr_ppu_rg = PPU_##N##_SLAVE_CONFIG, \
};

#if (RTE_MS_PPU_PR7)
DEFINE_MS_PPU_PR(PR7)
#endif

#if (RTE_MS_PPU_PERI_MAIN)
DEFINE_MS_PPU_FX(PERI_MAIN)
#endif

#if (RTE_MS_PPU_PERI_GR0_GROUP)
DEFINE_MS_PPU_FX(PERI_GR0_GROUP)
#endif

#if (RTE_MS_PPU_PERI_GR1_GROUP)
DEFINE_MS_PPU_FX(PERI_GR1_GROUP)
#endif

#if (RTE_MS_PPU_PERI_GR2_GROUP)
DEFINE_MS_PPU_FX(PERI_GR2_GROUP)
#endif

#if (RTE_MS_PPU_PERI_GR3_GROUP)
DEFINE_MS_PPU_FX(PERI_GR3_GROUP)
#endif

#if (RTE_MS_PPU_PERI_GR4_GROUP)
DEFINE_MS_PPU_FX(PERI_GR4_GROUP)
#endif

#if (RTE_MS_PPU_PERI_GR6_GROUP)
DEFINE_MS_PPU_FX(PERI_GR6_GROUP)
#endif

#if (RTE_MS_PPU_PERI_GR9_GROUP)
DEFINE_MS_PPU_FX(PERI_GR9_GROUP)
#endif

#if (RTE_MS_PPU_PERI_GR10_GROUP)
DEFINE_MS_PPU_FX(PERI_GR10_GROUP)
#endif

#if (RTE_MS_PPU_PERI_TR)
DEFINE_MS_PPU_FX(PERI_TR)
#endif

#if (RTE_MS_PPU_CRYPTO_MAIN)
DEFINE_MS_PPU_FX(CRYPTO_MAIN)
#endif

#if (RTE_MS_PPU_CRYPTO_CRYPTO)
DEFINE_MS_PPU_FX(CRYPTO_CRYPTO)
#endif

#if (RTE_MS_PPU_CRYPTO_BOOT)
DEFINE_MS_PPU_FX(CRYPTO_BOOT)
#endif

#if (RTE_MS_PPU_CRYPTO_KEY0)
DEFINE_MS_PPU_FX(CRYPTO_KEY0)
#endif

#if (RTE_MS_PPU_CRYPTO_KEY1)
DEFINE_MS_PPU_FX(CRYPTO_KEY1)
#endif

#if (RTE_MS_PPU_CRYPTO_BUF)
DEFINE_MS_PPU_FX(CRYPTO_BUF)
#endif

#if (RTE_MS_PPU_CPUSS_CM4)
DEFINE_MS_PPU_FX(CPUSS_CM4)
#endif

#if (RTE_MS_PPU_CPUSS_CM0)
DEFINE_MS_PPU_FX(CPUSS_CM0)
#endif

#if (RTE_MS_PPU_CPUSS_CM0_INT)
DEFINE_MS_PPU_FX(CPUSS_CM0_INT)
#endif

#if (RTE_MS_PPU_CPUSS_CM4_INT)
DEFINE_MS_PPU_FX(CPUSS_CM4_INT)
#endif

#if (RTE_MS_PPU_FAULT_STRUCT0_MAIN)
DEFINE_MS_PPU_FX(FAULT_STRUCT0_MAIN)
#endif

#if (RTE_MS_PPU_FAULT_STRUCT1_MAIN)
DEFINE_MS_PPU_FX(FAULT_STRUCT1_MAIN)
#endif

#if (RTE_MS_PPU_IPC_STRUCT0_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT0_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT1_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT1_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT2_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT2_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT3_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT3_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT4_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT4_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT5_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT5_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT6_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT6_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT7_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT7_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT8_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT8_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT9_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT9_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT10_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT10_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT11_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT11_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT12_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT12_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT13_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT13_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT14_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT14_IPC)
#endif

#if (RTE_MS_PPU_IPC_STRUCT15_IPC)
DEFINE_MS_PPU_FX(IPC_STRUCT15_IPC)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT1_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT1_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT2_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT2_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT3_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT3_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT4_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT4_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT5_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT5_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT6_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT6_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT7_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT7_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT8_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT8_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT9_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT9_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT10_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT10_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT11_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT11_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT12_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT12_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT13_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT13_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT14_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT14_INTR)
#endif

#if (RTE_MS_PPU_IPC_INTR_STRUCT15_INTR)
DEFINE_MS_PPU_FX(IPC_INTR_STRUCT15_INTR)
#endif

#if (RTE_MS_PPU_PROT_SMPU_MAIN)
DEFINE_MS_PPU_FX(PROT_SMPU_MAIN)
#endif

#if (RTE_MS_PPU_PROT_MPU0_MAIN)
DEFINE_MS_PPU_FX(PROT_MPU0_MAIN)
#endif

#if (RTE_MS_PPU_PROT_MPU5_MAIN)
DEFINE_MS_PPU_FX(PROT_MPU5_MAIN)
#endif

#if (RTE_MS_PPU_PROT_MPU6_MAIN)
DEFINE_MS_PPU_FX(PROT_MPU6_MAIN)
#endif

#if (RTE_MS_PPU_PROT_MPU14_MAIN)
DEFINE_MS_PPU_FX(PROT_MPU14_MAIN)
#endif

#if (RTE_MS_PPU_FLASHC_MAIN)
DEFINE_MS_PPU_FX(FLASHC_MAIN)
#endif

#if (RTE_MS_PPU_FLASHC_CMD)
DEFINE_MS_PPU_FX(FLASHC_CMD)
#endif

#if (RTE_MS_PPU_SRSS_MAIN1)
DEFINE_MS_PPU_FX(SRSS_MAIN1)
#endif

#if (RTE_MS_PPU_SRSS_MAIN2)
DEFINE_MS_PPU_FX(SRSS_MAIN2)
#endif

#if (RTE_MS_PPU_WDT)
DEFINE_MS_PPU_FX(WDT)
#endif

#if (RTE_MS_PPU_MAIN)
DEFINE_MS_PPU_FX(MAIN)
#endif

#if (RTE_MS_PPU_SRSS_MAIN3)
DEFINE_MS_PPU_FX(SRSS_MAIN3)
#endif

#if (RTE_MS_PPU_SRSS_MAIN4)
DEFINE_MS_PPU_FX(SRSS_MAIN4)
#endif

#if (RTE_MS_PPU_SRSS_MAIN5)
DEFINE_MS_PPU_FX(SRSS_MAIN5)
#endif

#if (RTE_MS_PPU_SRSS_MAIN6)
DEFINE_MS_PPU_FX(SRSS_MAIN6)
#endif

#if (RTE_MS_PPU_SRSS_MAIN7)
DEFINE_MS_PPU_FX(SRSS_MAIN7)
#endif

#if (RTE_MS_PPU_BACKUP_BACKUP)
DEFINE_MS_PPU_FX(BACKUP_BACKUP)
#endif

#if (RTE_MS_PPU_DW0_DW)
DEFINE_MS_PPU_FX(DW0_DW)
#endif

#if (RTE_MS_PPU_DW1_DW)
DEFINE_MS_PPU_FX(DW1_DW)
#endif

#if (RTE_MS_PPU_DW0_DW_CRC)
DEFINE_MS_PPU_FX(DW0_DW_CRC)
#endif

#if (RTE_MS_PPU_DW1_DW_CRC)
DEFINE_MS_PPU_FX(DW1_DW_CRC)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT0_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT0_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT1_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT1_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT2_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT2_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT3_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT3_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT4_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT4_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT5_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT5_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT6_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT6_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT7_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT7_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT8_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT8_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT9_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT9_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT10_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT10_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT11_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT11_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT12_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT12_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT13_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT13_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT14_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT14_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT15_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT15_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT16_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT16_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT17_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT17_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT18_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT18_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT19_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT19_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT20_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT20_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT21_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT21_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT22_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT22_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT23_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT23_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT24_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT24_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT25_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT25_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT26_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT26_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT27_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT27_CH)
#endif

#if (RTE_MS_PPU_DW0_CH_STRUCT28_CH)
DEFINE_MS_PPU_FX(DW0_CH_STRUCT28_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT0_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT0_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT1_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT1_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT2_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT2_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT3_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT3_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT4_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT4_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT5_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT5_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT6_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT6_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT7_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT7_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT8_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT8_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT9_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT9_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT10_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT10_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT11_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT11_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT12_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT12_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT13_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT13_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT14_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT14_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT15_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT15_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT16_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT16_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT17_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT17_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT18_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT18_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT19_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT19_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT20_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT20_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT21_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT21_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT22_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT22_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT23_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT23_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT24_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT24_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT25_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT25_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT26_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT26_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT27_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT27_CH)
#endif

#if (RTE_MS_PPU_DW1_CH_STRUCT28_CH)
DEFINE_MS_PPU_FX(DW1_CH_STRUCT28_CH)
#endif

#if (RTE_MS_PPU_DMAC_TOP)
DEFINE_MS_PPU_FX(DMAC_TOP)
#endif

#if (RTE_MS_PPU_DMAC_CH0_CH)
DEFINE_MS_PPU_FX(DMAC_CH0_CH)
#endif

#if (RTE_MS_PPU_DMAC_CH1_CH)
DEFINE_MS_PPU_FX(DMAC_CH1_CH)
#endif

#if (RTE_MS_PPU_DMAC_CH2_CH)
DEFINE_MS_PPU_FX(DMAC_CH2_CH)
#endif

#if (RTE_MS_PPU_DMAC_CH3_CH)
DEFINE_MS_PPU_FX(DMAC_CH3_CH)
#endif

#if (RTE_MS_PPU_EFUSE_DATA)
DEFINE_MS_PPU_FX(EFUSE_DATA)
#endif

#if (RTE_MS_PPU_PROFILE)
DEFINE_MS_PPU_FX(PROFILE)
#endif

#if (RTE_MS_PPU_HSIOM_PRT0_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT0_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT1_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT1_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT2_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT2_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT3_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT3_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT4_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT4_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT5_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT5_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT6_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT6_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT7_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT7_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT8_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT8_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT9_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT9_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT10_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT10_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT11_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT11_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT12_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT12_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT13_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT13_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_PRT14_PRT)
DEFINE_MS_PPU_FX(HSIOM_PRT14_PRT)
#endif

#if (RTE_MS_PPU_HSIOM_AMUX)
DEFINE_MS_PPU_FX(HSIOM_AMUX)
#endif

#if (RTE_MS_PPU_HSIOM_MON)
DEFINE_MS_PPU_FX(HSIOM_MON)
#endif

#if (RTE_MS_PPU_GPIO_PRT0_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT0_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT1_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT1_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT2_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT2_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT3_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT3_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT4_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT4_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT5_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT5_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT6_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT6_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT7_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT7_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT8_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT8_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT9_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT9_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT10_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT10_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT11_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT11_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT12_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT12_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT13_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT13_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT14_PRT)
DEFINE_MS_PPU_FX(GPIO_PRT14_PRT)
#endif

#if (RTE_MS_PPU_GPIO_PRT0_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT0_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT1_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT1_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT2_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT2_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT3_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT3_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT4_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT4_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT5_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT5_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT6_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT6_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT7_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT7_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT8_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT8_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT9_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT9_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT10_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT10_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT11_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT11_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT12_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT12_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT13_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT13_CFG)
#endif

#if (RTE_MS_PPU_GPIO_PRT14_CFG)
DEFINE_MS_PPU_FX(GPIO_PRT14_CFG)
#endif

#if (RTE_MS_PPU_GPIO_GPIO)
DEFINE_MS_PPU_FX(GPIO_GPIO)
#endif

#if (RTE_MS_PPU_SMARTIO_PRT8_PRT)
DEFINE_MS_PPU_FX(SMARTIO_PRT8_PRT)
#endif

#if (RTE_MS_PPU_SMARTIO_PRT9_PRT)
DEFINE_MS_PPU_FX(SMARTIO_PRT9_PRT)
#endif

#if (RTE_MS_PPU_LPCOMP)
DEFINE_MS_PPU_FX(LPCOMP)
#endif

#if (RTE_MS_PPU_CSD0)
DEFINE_MS_PPU_FX(CSD0)
#endif

#if (RTE_MS_PPU_TCPWM0)
DEFINE_MS_PPU_FX(TCPWM0)
#endif

#if (RTE_MS_PPU_TCPWM1)
DEFINE_MS_PPU_FX(TCPWM1)
#endif

#if (RTE_MS_PPU_LCD0)
DEFINE_MS_PPU_FX(LCD0)
#endif

#if (RTE_MS_PPU_USBFS0)
DEFINE_MS_PPU_FX(USBFS0)
#endif

#if (RTE_MS_PPU_SMIF0)
DEFINE_MS_PPU_FX(SMIF0)
#endif

#if (RTE_MS_PPU_SDHC0)
DEFINE_MS_PPU_FX(SDHC0)
#endif

#if (RTE_MS_PPU_SDHC1)
DEFINE_MS_PPU_FX(SDHC1)
#endif

#if (RTE_MS_PPU_SCB0)
DEFINE_MS_PPU_FX(SCB0)
#endif

#if (RTE_MS_PPU_SCB1)
DEFINE_MS_PPU_FX(SCB1)
#endif

#if (RTE_MS_PPU_SCB2)
DEFINE_MS_PPU_FX(SCB2)
#endif

#if (RTE_MS_PPU_SCB3)
DEFINE_MS_PPU_FX(SCB3)
#endif

#if (RTE_MS_PPU_SCB4)
DEFINE_MS_PPU_FX(SCB4)
#endif

#if (RTE_MS_PPU_SCB5)
DEFINE_MS_PPU_FX(SCB5)
#endif

#if (RTE_MS_PPU_SCB6)
DEFINE_MS_PPU_FX(SCB6)
#endif

#if (RTE_MS_PPU_SCB7)
DEFINE_MS_PPU_FX(SCB7)
#endif

#if (RTE_MS_PPU_SCB8)
DEFINE_MS_PPU_FX(SCB8)
#endif

#if (RTE_MS_PPU_SCB9)
DEFINE_MS_PPU_FX(SCB9)
#endif

#if (RTE_MS_PPU_SCB10)
DEFINE_MS_PPU_FX(SCB10)
#endif

#if (RTE_MS_PPU_SCB11)
DEFINE_MS_PPU_FX(SCB11)
#endif

#if (RTE_MS_PPU_SCB12)
DEFINE_MS_PPU_FX(SCB12)
#endif

#if (RTE_MS_PPU_PDM0)
DEFINE_MS_PPU_FX(PDM0)
#endif

#if (RTE_MS_PPU_I2S0)
DEFINE_MS_PPU_FX(I2S0)
#endif

#if (RTE_MS_PPU_I2S1)
DEFINE_MS_PPU_FX(I2S1)
#endif
