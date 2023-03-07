/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_SPM_H__
#define __HAL_SPM_H__
#include "common.h"


#define WAKEUP_AO_REG                (CHIP_MISC_BASE + 0x00C)
#define WAKEUP_REBOOT_STATUS         (0x1 << 0)
#define WAKEUP_PSRAM_MODE_SHIFT      (2)
#define WAKEUP_PSRAM_MODE_MASK       (0x7 << WAKEUP_PSRAM_MODE_SHIFT)
#define WAKEUP_PSRAM_MODE_VALUE(x)   (((x)& WAKEUP_PSRAM_MODE_MASK) >> WAKEUP_PSRAM_MODE_SHIFT)
#define WAKEUP_XIP_BOOT              (0x1 << 11)

#define EFUSE_GRP2_BLK_0                 (GRP2_EEF_TOP + 0x80)
#define EFUSE_PSRAM_MODE_MASK        (BITS(8,10))
#define EFUSE_PSRAM_MODE_SHIFT       (8)
#define EFUSE_PSRAM_MODE_VALUE(x)    (((x)& EFUSE_PSRAM_MODE_MASK) >> EFUSE_PSRAM_MODE_SHIFT)
#define EFUSE_PKG_MODE_MASK          (BITS(16,17))
#define EFUSE_PKG_MODE_SHIFT         (16)
#define EFUSE_PKG_MODE_VALUE(x)      (((x)& EFUSE_PSRAM_MODE_MASK) >> EFUSE_PSRAM_MODE_SHIFT)

#define EFUSE_PKG_MODE_DRQFN 0
#define EFUSE_PKG_MODE_WBBGA 1
#define EFUSE_PKG_MODE_SBGA  2

#define CONN_BUS_REQ_INT_CTRL        (SPM_TOP_BASE + 0x060C)

#define SPM_IS_REBOOT()              ((HAL_REG_32(WAKEUP_AO_REG) & WAKEUP_REBOOT_STATUS) != 0)

#define MTCMOS_PWR_ON_TOP_OFF_L1
#define MTCMOS_PWR_DOWN_TOP_OFF_L1
#define MTCMOS_PWR_ON_TOP_OFF_L2
#define MTCMOS_PWR_DOWN_TOP_OFF_L2
#define MTCMOS_PWR_ON_USB2                  spm_mtcmos(SPM_MODULE_USB2, SPM_ON, 0)
#define MTCMOS_PWR_DOWN_USB2                spm_mtcmos(SPM_MODULE_USB2, SPM_OFF, 0)
#define MTCMOS_PWR_ON_PSRAM_AXI_BUS         spm_mtcmos(SPM_MODULE_PSRAM_BUS, SPM_ON, 0)
#define MTCMOS_PWR_DOWN_PSRAM_AXI_BUS       spm_mtcmos(SPM_MODULE_PSRAM_BUS, SPM_OFF, 0)
#define MTCMOS_PWR_ON_NONUHS_PSRAM          spm_mtcmos(SPM_MODULE_NONUHS_PSRAM, SPM_ON, 0)
#define MTCMOS_PWR_DOWN_NONUHS_PSRAM        spm_mtcmos(SPM_MODULE_NONUHS_PSRAM, SPM_OFF, 0)
#define MTCMOS_PWR_ON_SFLASH_IOMACRO        spm_mtcmos(SPM_MODULE_SFLASH, SPM_ON, 0)
#define MTCMOS_PWR_DOWN_SFLASH_IOMACRO      spm_mtcmos(SPM_MODULE_SFLASH, SPM_OFF, 0)
#define MTCMOS_PWR_ON_DSP                   spm_mtcmos(SPM_MODULE_DSP, SPM_ON, 0)
#define MTCMOS_PWR_DOWN_DSP                 spm_mtcmos(SPM_MODULE_DSP, SPM_OFF, 0)
#define MTCMOS_PWR_ON_AUDIO_AFE             spm_mtcmos(SPM_MODULE_AUDIO_AFE, SPM_ON, 0)
#define MTCMOS_PWR_DOWN_AUDIO_AFE           spm_mtcmos(SPM_MODULE_AUDIO_AFE, SPM_OFF, 0)
#define MTCMOS_PWR_ON_AUDIO_AO              spm_mtcmos(SPM_MODULE_AUDIO_AO, SPM_ON, 0)
#define MTCMOS_PWR_DOWN_AUDIO_AO            spm_mtcmos(SPM_MODULE_AUDIO_AO, SPM_OFF, 0)
#define MTCMOS_PWR_ON_CONNSYS_ON            spm_mtcmos(SPM_MODULE_CONNSYS_ON, SPM_ON, 0)
#define MTCMOS_PWR_DOWN_CONNSYS_ON          spm_mtcmos(SPM_MODULE_CONNSYS_ON, SPM_OFF, 0)
#define MTCMOS_PWR_ON_UHS_PSRAM_OFF         spm_mtcmos(SPM_MODULE_UHS_PSRAM_OFF, SPM_ON, 0)
#define MTCMOS_PWR_DOWN_UHS_PSRAM_OFF       spm_mtcmos(SPM_MODULE_UHS_PSRAM_OFF, SPM_OFF, 0)
#define MTCMOS_PWR_ON_UHS_PSRAM_AON         spm_mtcmos(SPM_MODULE_UHS_PSRAM_AO, SPM_ON, 0)
#define MTCMOS_PWR_DOWN_UHS_PSRAM_AON       spm_mtcmos(SPM_MODULE_UHS_PSRAM_AO, SPM_OFF, 0)

#define SRAM_PWR_ON_DSP(chain)              spm_mtcmos(SPM_MODULE_SRAM_DSP, SPM_ON, chain)
#define SRAM_PWR_DOWN_DSP(chain)            spm_mtcmos(SPM_MODULE_SRAM_DSP, SPM_OFF, chain)
#define SRAM_PWR_ON_DSP_POOL(chain)         spm_mtcmos(SPM_MODULE_SRAM_DSP_POOL, SPM_ON, chain)
#define SRAM_PWR_DOWN_DSP_POOL(chain)       spm_mtcmos(SPM_MODULE_SRAM_DSP_POOL, SPM_OFF, chain)
#define SRAM_PWR_ON_AUDIO(chain)            spm_mtcmos(SPM_MODULE_SRAM_AUDIO, SPM_ON, chain)
#define SRAM_PWR_DOWN_AUDIO(chain)          spm_mtcmos(SPM_MODULE_SRAM_AUDIO, SPM_OFF, chain)
#define SRAM_PWR_ON_TOP_SYSRAM(chain)       spm_mtcmos(SPM_MODULE_SRAM_SYSRAM, SPM_ON, chain)
#define SRAM_PWR_DOWN_TOP_SYSRAM(chain)     spm_mtcmos(SPM_MODULE_SRAM_SYSRAM, SPM_OFF, chain)
#define SRAM_PWR_ON_SF_DMA_SPIS             spm_mtcmos(SPM_MODULE_SRAM_SF_DMA_SPIS, SPM_ON, 0)
#define SRAM_PWR_DOWN_SF_DMA_SPIS           spm_mtcmos(SPM_MODULE_SRAM_SF_DMA_SPIS, SPM_OFF, 0)
#define SRAM_PWR_ON_MSDC_ECC                spm_mtcmos(SPM_MODULE_SRAM_MSDC_ECC, SPM_ON, 0)
#define SRAM_PWR_DOWN_MSDC_ECC              spm_mtcmos(SPM_MODULE_SRAM_MSDC_ECC, SPM_OFF, 0)
#define SRAM_PWR_ON_GCPU(chain)             spm_mtcmos(SPM_MODULE_SRAM_GCPU, SPM_ON, chain)
#define SRAM_PWR_DOWN_GCPU(chain)           spm_mtcmos(SPM_MODULE_SRAM_GCPU, SPM_OFF, chain)
#define SRAM_PWR_ON_SDCTL                   spm_mtcmos(SPM_MODULE_SRAM_SDCTL, SPM_ON, 0)
#define SRAM_PWR_DOWN_SDCTL                 spm_mtcmos(SPM_MODULE_SRAM_SDCTL, SPM_OFF, 0)
#define SRAM_PWR_ON_UHS_PSRAM               spm_mtcmos(SPM_MODULE_SRAM_UHS_PSRAM, SPM_ON, 0)
#define SRAM_PWR_DOWN_UHS_PSRAM             spm_mtcmos(SPM_MODULE_SRAM_UHS_PSRAM, SPM_OFF, 0)
#define SRAM_PWR_ON_USB2                    /* merged to SPM_MODULE_USB2 */
#define SRAM_PWR_DOWN_USB2                  /* merged to SPM_MODULE_USB2 */

#define SRAM_WAKE_DSP_POOL(chain)
#define SRAM_SLP_DSP_POOL(chain)
#define SRAM_WAKE_GCPU(chain)
#define SRAM_SLP_GCPU(chain)
#define SRAM_SLP_STATE_GCPU(chain)
#define SRAM_WAKE_TOP_SYSRAM(chain)
#define SRAM_SLP_TOP_SYSRAM(chain)
#define SRAM_SLP_STATE_TOP_SYSRAM(chain)
#define SRAM_WAKE_SF_DMA_SPIS
#define SRAM_SLP_SF_DMA_SPIS
#define SRAM_SLP_STATE_SF_DMA_SPIS

typedef enum {
    MTCMOS_STATE_AON            = 0,
    MTCMOS_STATE_TOP_CLK_OFF    = 1,
    MTCMOS_STATE_SDIO_IO        = 2,
    MTCMOS_STATE_CM33_OFF       = 3,
    MTCMOS_STATE_TOP_OFF_L1     = 4,
    MTCMOS_STATE_TOP_OFF_L2     = 5,
    MTCMOS_STATE_SF_IO          = 6,
    MTCMOS_STATE_USB2           = 7,
    MTCMOS_STATE_PSRAM_BUS      = 8,
    MTCMOS_STATE_NON_UHS_PSRAM  = 9,
    MTCMOS_STATE_UHS_PSRAM_AON  = 10,
    MTCMOS_STATE_UHS_PSRAM      = 11,
    MTCMOS_STATE_AUDIO_ON       = 12,
    MTCMOS_STATE_AUDIO_AFE      = 13,
    MTCMOS_STATE_DSP            = 14,
    MTCMOS_STATE_INFRA_ON       = 15,
    MTCMOS_STATE_INFRA_OFF      = 16,
    MTCMOS_STATE_WFSYS_ON       = 17,
    MTCMOS_STATE_WFSYS_OFF      = 18,
    MTCMOS_STATE_BTSYS_ON       = 19,
    MTCMOS_STATE_BTSYS_OFF      = 20,
    MTCMOS_STATE_SRAM_SYSRAM    = 21,
} spm_mtcmos_module_t;

typedef enum {
    SPM_SUSPEND_LOCK_CM33,
    SPM_SUSPEND_LOCK_SDIO,
    SPM_SUSPEND_LOCK_USB,
    SPM_SUSPEND_LOCK_DSP,
    SPM_SUSPEND_LOCK_XO,
} spm_suspend_lock_module_t;

typedef enum {
    SPM_ON,
    SPM_OFF
} spm_onoff_t;

typedef enum {
    SPM_MODULE_USB2,
    SPM_MODULE_PSRAM_BUS,
    SPM_MODULE_NONUHS_PSRAM,
    SPM_MODULE_SFLASH,
    SPM_MODULE_DSP,
    SPM_MODULE_AUDIO_AFE,
    SPM_MODULE_AUDIO_AO,
    SPM_MODULE_CONNSYS_ON,
    SPM_MODULE_UHS_PSRAM_OFF,
    SPM_MODULE_UHS_PSRAM_AO,
    SPM_MODULE_SRAM_DSP,
    SPM_MODULE_SRAM_DSP_POOL,
    SPM_MODULE_SRAM_AUDIO,
    SPM_MODULE_SRAM_SYSRAM,
    SPM_MODULE_SRAM_SF_DMA_SPIS,
    SPM_MODULE_SRAM_MSDC_ECC,
    SPM_MODULE_SRAM_GCPU,
    SPM_MODULE_SRAM_SDCTL,
    SPM_MODULE_SRAM_UHS_PSRAM,
} spm_module_t;

typedef enum {
    SPM_CTRL_HW_INIT,
    SPM_CTRL_MTCMOS_CONNSYS_ON_RESET,
    SPM_CTRL_SRCCLKENA_PRE,
    SPM_CTRL_SRCCLKENA_ACK,
    SPM_CTRL_SET_AP_BUS_REQ,
    SPM_CTRL_SET_AP_SRC_REQ
} spm_ctrl_t;

#define MTCMOS_IS_ON(module)                spm_mtcmos_is_on(module)

void spm_suspend_module_lock(spm_suspend_lock_module_t module, bool lock);
void spm_mtcmos(spm_module_t module, spm_onoff_t onoff, unsigned int chain_no);
bool spm_mtcmos_is_on(spm_mtcmos_module_t mtcmos);
void spm_ctrl(spm_ctrl_t ctrl);

void hal_spm_init(void);
void hal_spm_conninfra_pos(void);
void hal_spm_conninfra_on(void);
void hal_spm_conninfra_off(void);
void hal_spm_conninfra_wakeup(void);
void hal_spm_conninfra_sleep(void);
void hal_spm_conninfra_sw_reset(bool on);
void hal_spm_conninfra_slp_prot_disable(void);
void hal_spm_conninfra_a_die_cfg(void);
void hal_spm_conninfra_afe_wbg_init(void);
void hal_spm_conninfra_afe_wbg_cal(void);
void hal_spm_conninfra_pll_init(void);
void hal_spm_conninfra_osc_init(void);
void topspi_write(uint32_t addr, uint32_t value, uint32_t id);
uint32_t topspi_read(uint32_t addr, uint32_t id);
void hal_spm_bt_pwr_off(void);
void hal_spm_wf_pwr_off(void);
void hal_spm_conninfra_clk_osc(void);
void hal_spm_conninfra_dcm_on(void);
void hal_spm_conninfra_osc_floor(void);
void hal_spm_conninfra_osc2x_floor(void);
void hal_spm_conninfra_wpll_floor(void);
void hal_spm_conninfra_bpll_floor(void);
void hal_spm_conninfra_ds_floor(void);
void hal_spm_conninfra_plltree_floor(void);
void hal_spm_conninfra_speedup_wakeup(void);
bool hal_spm_conninfra_is_wakeup(void);
void hal_spm_bus_dbg_int_ctrl(uint32_t is_enable);

void rom_pwr_on_gcpu(void);         // deprecated, remove in SDK 2.0
void rom_pwr_down_gcpu(void);       // deprecated, remove in SDK 2.0


#endif /* #ifndef __HAL_SPM_H__ */
