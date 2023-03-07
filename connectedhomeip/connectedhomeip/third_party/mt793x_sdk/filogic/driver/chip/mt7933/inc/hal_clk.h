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

#ifndef __HAL_CLK_H__
#define __HAL_CLK_H__

#ifdef HAL_CLOCK_MODULE_ENABLED
#include "type_def.h"
#include "reg_base.h"
#include "hal_clock.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */
/**
* @defgroup top Enum
* @{
*/


#ifdef HAL_CLK_CTP_SUPPORT

typedef enum  {
    FM_TOP_PLL_DIV2_CK,         // [0] 600MHz
    FM_DSPPLL_CK,               // [1] 600MHz
    FM_USBPLL_CK,               // [2] 192MHz
    FM_XPLL_CK,                 // [3] 196.608MHz
    FM_DSP_CK,                  // [4] 600MHz
    FM_PSRAM_CK,                // [5] 400MHz
    FM_SPIS_CK,                 // [6] 400MHz
    FM_CM33_HCLK_PRE_CK,        // [7] 300MHz
    FM_TRACE_CK,                // [8] 300MHz
    FM_GCPU_CK,                 // [9] 300MHz
    FM_ECC_CK,                  // [10] 300MHz
    FM_AUX_ADC_CK,              // [11] 2MHz
    FM_AUDSYS_BUS_CK,           // [12] 266.67MHz
    FM_FAUD_INTBUS_CK,          // [13] 266.67MHz
    FM_PSRAM_AXI_CK,            // [14] 266.67MHz
    FM_SPIM0_CK,                // [15] 200MHz
    FM_SPIM1_CK,                // [16] 200MHz
    FM_HAPLL_CK,                // [17] 196.608MHz
    FM_FASYS_CK,                // [18] 196.608MHz
    FM_FAUD_CK,                 // [19] 196.608MHz
    FM_AXI_BUS_FREE_CK,         // [20] 133.34MHz
    FM_USB_XHCI_CK,             // [21] 133.34MHz
    FM_DBG_CK,                  // [22] 133.34MHz
    FM_I2C_CK,                  // [23] 120MHz
    FM_FASM_CK,                 // [24] 120MHz
    FM_USB_SYS_CK,              // [25] 120MHz
    FM_FLASH_CK,                // [26] 60MHz
    FM_SDIOM_CK,                // [27] 50MHz
    FM_FAUDIO_CK,               // [28] 49.152MHz
    FM_SSUSB_FRMCNT_CK,         // [29] 60MHz
    FM_XTAL_CK,                 // [30] 26MHz
    FM_AUD_DAC_26M_MON_CK,      // [31] 26MHz
    FM_UHS_PSRAM_CK,            // [32] 259.983MHz
    FM_NON_UHS_PSRAM_CK,        // [33] 400MHz
    FM_NON_UHS_PSRAM_DIV2_CK,   // [34] 200MHz
    FM_CKGEN_CLK_END
} FM_CKGEN_CKID;

/* subsys cg support */
enum subsys {
    PLL_CG = 0,
    XTAL_CG = 1,
};

#endif /* #ifdef HAL_CLK_CTP_SUPPORT */

#define fqmtr_err(fmt, args...)       \
    dbg_print(fmt, ##args)
#define fqmtr_warn(fmt, args...)      \
    dbg_print(fmt, ##args)
#define fqmtr_info(fmt, args...)      \
    dbg_print(fmt, ##args)
#define fqmtr_dbg(fmt, args...)       \
    dbg_print(fmt, ##args)
#define fqmtr_ver(fmt, args...)       \
    dbg_print(fmt, ##args)

#define TRACE(fmt, args...) \
    printf("[clk-mtk] %s():%d: " fmt, __func__, __LINE__, ##args)

#define PR(fmt, args...) \
    printf(fmt, ##args)

#define udelay(us)          \
    do {                    \
        volatile int count = us * 1000;\
        while (count--);\
    } while(0)

#define ALL_CLK_ON      1
#define DEBUG_FQMTR     0

/**
 * @}
 */

/**
 * @brief  Updates the SystemCoreClock variable by current CPU frequency variable
 * @param  none
 * @return void
 */
extern void SystemCoreClockUpdate(void);
extern uint32_t SystemCoreClock;      /**< System Clock Frequency (Core Clock) */

// hal_clk_ds_backup_and_set: Backup clock setting and set to low power mode
// code is in SYSRAM
void clk_ds_backup_and_set(void *data);

// hal_clk_ds_restore_phase1: Clock restore after deep sleep phase 1
//   these code shall be running at the early beginning to speedup restore, code is in SYSRAM
void clk_ds_restore_phase1(void *data);

// hal_clk_ds_restore_phase2: Clock restore after deep sleep phase 2
//   these code shall be run after the critical path, code is in default code memory region
void clk_ds_restore_phase2(void *data);

/* Common used */

/** @brief  CM4 hclk protection enable
 *  @brief  When the targeted PLL clock source is not available, the protection event will be asserted.
 *  @brief  Clock is forced to XTAL.
 *  @param  none
 *  @return void
 */
void hal_clk_hclk_prot_en(void);

void hal_clk_conninfra_pll_init(void);
void hal_clk_conninfra_osc_init(void);

/** @brief  Get F32K clock divider, according to f32k clock mux selection
 *  @param  none
 *  @return f32k divider
 */
uint32_t hal_clk_get_f32k_divider(void);

#ifdef HAL_CLK_CTP_SUPPORT
/** @brief Enable pll tuner
*   @param en : enable(1), disable(0)
*   @return None
**/
void mt_pll_tuner_enable(unsigned int en);

/** @brief Configure pll tuner by SW
*          In general case, HW default value is no need to change
*          Provided here only for special purpose
*   @param pcw : upper and lower bound for pll tuner
*   @return None
**/
void mt_pll_tuner_config(unsigned int pcw_hi, unsigned int pcw_lo);
#endif /* #ifdef HAL_CLK_CTP_SUPPORT */
#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/**
* @}
* @}
*/

// Code wrapper for legacy codes
#define hal_clk_psramaxi_clksel(mode)   hal_clock_mux_select(HAL_CLOCK_SEL_PSRAM_AXI, mode)
#define hal_clk_cm33_clksel(mode)       hal_clock_mux_select(HAL_CLOCK_SEL_CM33_HCLK, mode)
#define hal_clk_axi_clksel(mode)       hal_clock_mux_select(HAL_CLOCK_SEL_INFRA_BUS, mode)
#define hal_clk_sflash_clksel(mode)       hal_clock_mux_select(HAL_CLOCK_SEL_FLASH, mode)
#define hal_clk_f32k_clksel(mode)       hal_clock_mux_select(HAL_CLOCK_SEL_F32K, mode)
#define PSRAMAXI_SRC_XTAL CLK_PSRAM_AXI_CLKSEL_XTAL
#define PSRAMAXI_SRC_PSRAM CLK_PSRAM_AXI_CLKSEL_PSRAM
#define CM33CLK_SRC_XTAL CLK_CM33_HCLK_CLKSEL_XTAL
#define AXICLK_SRC_XTAL CLK_INFRA_BUS_CLKSEL_XTAL
#define SFLASHCLK_SRC_XTAL CLK_FLASH_CLKSEL_XTAL
#define F32KCLK_SRC_RTC CLK_F32K_CLKSEL_RTC

/*************************************************************************
 * Define customized function prototype
 *************************************************************************/
void hal_clock_set_pll_dcm_init(void);
void hal_clock_set_dcm(bool enable);

#endif /* #ifdef HAL_CLOCK_MODULE_ENABLED */
#endif /* #ifndef __HAL_CLK_H__ */
