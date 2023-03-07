/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

#ifndef __HAL_CLOCK_INTERNAL_H__
#define __HAL_CLOCK_INTERNAL_H__

#include "hal_platform.h"
#ifdef HAL_CLOCK_MODULE_ENABLED
#include "hal_clock_platform_ab155x.h"
#include "memory_attribute.h"
#include "hal_clock.h"
#include "hal_dvfs.h"

#define NUM_SRC                          3
#define mux_bit(mux_id)                  (0x1 << mux_id)
#define SPI_MUX_SET_LENGTH               2
#define AUD_INTERFACE_MUX_SET_LENGTH     1
#define AUD_HIRES_MUX_SET_LENGTH         1

#define CKSYS_CLK_CFG_REG                ((cksys_clk_cfg*)CKSYS_CLK_CFG_0__F_CLK_SYS_SEL) ->field
#define CKSYS_CLK_CHG_REG                ((cksys_clk_chg*)CKSYS_CLK_UPDATE_0__F_CHG_SYS) ->field
#define CKSYS_CLK_OK_REG                 ((cksys_clk_update_status*)CKSYS_CLK_UPDATE_STATUS_0__F_CHG_SYS_OK) ->field
#define CKSYS_CLK_FORCE_ON_REG           ((cksys_clk_force_on*)CKSYS_CLK_FORCE_ON_0__F_CLK_SYS_FORCE_ON) ->field
#define CKSYS_CLK_DIV_REG                ((cksys_clk_div*)CKSYS_CLK_DIV_0__F_CLK_PLL1_D2_EN) ->field


#define UPLL_DOMAIN_USR_EXIST            (clock_table.pll_domain.field.upll.src_cnt || clock_table.pll_domain.field.free.field.upll ? exist : nonexist)
#define MPLL_DOMAIN_USR_EXIST            (clock_table.pll_domain.field.mpll.src_cnt || clock_table.pll_domain.field.free.field.mpll ? exist : nonexist)
#define APLL1_DOMAIN_USR_EXIST           (clock_table.pll_domain.field.free.field.apll1 ? exist : nonexist)
#define APLL2_DOMAIN_USR_EXIST           (clock_table.pll_domain.field.free.field.apll2 ? exist : nonexist)

#define OSC_DIV_DOMAIN_USR_EXIST         (clock_table.pll_domain.field.osc.src_cnt  ? exist : nonexist)
#define UPLL_DIV_DOMAIN_USR_EXIST        (clock_table.pll_domain.field.upll.src_cnt ? exist : nonexist)
#define MPLL_DIV_DOMAIN_USR_EXIST        (clock_table.pll_domain.field.mpll.src_cnt ? exist : nonexist)

typedef void (*post_handle)(void);

typedef enum {
    CLK_DISABLE = 0xff,
    CLK_ENABLE = 0xfe,
} clock_linkage_type;

typedef enum {
    CLK_DIV_104M_D2 = 1,
    CLK_DIV_D2 = 2,
    CLK_DIV_D3 = 3,
    CLK_DIV_D5 = 5,
    CLK_DIV_D7 = 7,
    CLK_DIV_NONE = 0xff
} AB155X_DIV_TYPE;

typedef enum {
    UPLL_DIV_IDX,
    MPLL_DIV_IDX,
    OSC_DIV_IDX,
    NONE_DIV,
} clock_div_ctrl_idx;

typedef struct {
    clock_pll_id src;
    uint8_t div;
} pin_mux_t ;

typedef enum {
    decr,
    incr
} mod_type;

typedef enum {
    CLK_OSC_FREE,
    CLK_OSC_DIV
} osc_domain_indx;

typedef enum {
    _1_1v_level = 0,
    _1_3v_level,
    volt_max_level,
    _0_9v_level
} volt_level;

typedef enum {
    nonexist,
    exist,
} clk_usr_status;
typedef clk_usr_status(*usr_status)(void);

typedef struct {
    usr_status  usr_sta;
    post_handle post_hdr;
} clk_src_handler ;

typedef union {
    uint32_t src_cnt;
    struct {
        uint8_t div_low;
        uint8_t div_middle;
        uint8_t div_high;
        uint8_t div_max;
    } __attribute__((packed)) field;
} clk_div_info;

typedef union {
    uint32_t src_cnt;
    struct {
        uint8_t upll;
        uint8_t mpll;
        uint8_t apll1;
        uint8_t apll2;
    } __attribute__((packed)) field;
} clk_pll_non_div_info;

typedef union {
    uint32_t src_cnt[4];
    struct {
        clk_div_info upll;
        clk_div_info mpll;
        clk_div_info osc;
        clk_pll_non_div_info free;
    } __attribute__((packed)) field;
} clk_pll_domain;

typedef struct {
    clk_div_info div_tbl[NUM_SRC];
} clk_top_div_info;

typedef struct {
    volatile pin_mux_t *cur_sel_2;
    volatile pin_mux_t *cur_sel_1;
    volatile pin_mux_t *cur_sel_0;
    pin_mux_t mux_sels[NR_MUXS];
} top_mux_info;

typedef union {
    uint32_t value[3];
    struct {
        __IO uint8_t CHG_SYS_SEL;
        __IO uint8_t CHG_SFC_SEL;
        __IO uint8_t CHG_SPIMST0_SEL;
        __IO uint8_t CHG_SPIMST1_SEL;
        __IO uint8_t CHG_SPIMST2_SEL;
        __IO uint8_t CHG_SDIOMST_SEL;
        __IO uint8_t CHG_SPISLV_SEL;
        __IO uint8_t CHG_USB_SEL;
        __IO uint8_t CHG_AUD_BUS_SEL;
        __IO uint8_t CHG_AUD_GPSRC_SEL;
        __IO uint8_t CHG_AUD_INTERFACE0_SEL;
        __IO uint8_t CHG_AUD_INTERFACE1_SEL;
        __IO uint8_t CHG_SPM_SEL;
        __IO uint8_t CHG_26M_SEL;
        uint16_t RESERVED;
    } __attribute__((packed)) field;
} cksys_clk_cfg;

typedef union {
    uint32_t value[3];
    struct {
        __I uint8_t CHG_SYS_OK;
        __I uint8_t CHG_SFC_OK;
        __I uint8_t CHG_SPIMST0_OK;
        __I uint8_t CHG_SPIMST1_OK;
        __I uint8_t CHG_SPIMST2_OK;
        __I uint8_t CHG_SDIOMST_OK;
        __I uint8_t CHG_SPISLV_OK;
        __I uint8_t CHG_USB_OK;
        __I uint8_t CHG_AUD_GPSRC_OK;
        __I uint8_t CHG_EMI_OK;
        uint16_t RESERVED;
    } __attribute__((packed)) field;
} cksys_clk_update_status;

typedef union {
    uint32_t value[3];
    struct {
        __IO uint8_t CHG_SYS_FORCE_ON;
        __IO uint8_t CHG_SFC_FORCE_ON;
        __IO uint8_t CHG_SPIMST0_FORCE_ON;
        __IO uint8_t CHG_SPIMST1_FORCE_ON;
        __IO uint8_t CHG_SPIMST2_FORCE_ON;
        __IO uint8_t CHG_SDIOMST_FORCE_ON;
        __IO uint8_t CHG_SPISLV_FORCE_ON;
        __IO uint8_t CHG_USB_FORCE_ON;
        __IO uint8_t CHG_AUD_GPSRC_FORCE_ON;
        __IO uint8_t CHG_EMI_FORCE_ON;
        uint16_t RESERVED;
    } __attribute__((packed)) field;
} cksys_clk_force_on;

typedef union {
    uint32_t value[3];
    struct {
        __IO uint8_t CHG_SYS;
        __IO uint8_t CHG_SFC;
        __IO uint8_t CHG_SPIMST0;
        __IO uint8_t CHG_SPIMST1;
        __IO uint8_t CHG_SPIMST2;
        __IO uint8_t CHG_SDIOMST;
        __IO uint8_t CHG_SPISLV;
        __IO uint8_t CHG_USB;
        __IO uint8_t CHG_AUD_GPSRC;
        __IO uint8_t CHG_EMI;
        uint16_t RESERVED;
    } __attribute__((packed)) field;
} cksys_clk_chg;

typedef union {
    uint32_t value[4];
    struct {
        __IO uint8_t CLK_UPLL_D2_EN;
        __IO uint8_t CLK_UPLL_D3_EN;
        __IO uint8_t CLK_UPLL_D5_EN;
        __IO uint8_t CLK_UPLL_D7_EN;
        __IO uint8_t CLK_MPLL_D2_EN;
        __IO uint8_t CLK_MPLL_D3_EN;
        __IO uint8_t CLK_MPLL_D5_EN;
        __IO uint8_t CLK_MPLL_D7_EN;
        __IO uint8_t CLK_OSC_104M_D2_EN;
        __IO uint8_t CLK_OSC_D2_EN;
        __IO uint8_t CLK_OSC_D5_EN;
        __IO uint8_t CLK_OSC_D7_EN;
        __IO uint8_t CLK_UPLL_DIV_EN;
        __IO uint8_t CLK_MPLL_DIV_EN;
        __IO uint8_t CLK_OSC_DIV_EN;
        uint16_t RESERVED ;
    } __attribute__((packed)) field;
} cksys_clk_div;

typedef union {
    uint64_t src_cnt;
    struct {
        uint32_t free;
        clk_div_info osc;
    } __attribute__((packed)) field;
} clk_lposc_domain;


typedef struct {
    top_mux_info               top_mux_info_list[NR_MUXS];
    clk_top_div_info           div_info;
    volatile clk_pll_domain    pll_domain;
    volatile clk_lposc_domain  osc_domain;
} clock_domain_t;

/*
typedef union {
    uint16_t value;
    struct {
        uint8_t upll_en : 1;
        uint8_t mpll_en : 1;
        uint8_t osc_en : 1;
        uint8_t apll1_en : 1;
        uint8_t apll2_en : 1;
        uint8_t pll1_div_en : 3;
        uint8_t pll2_div_en : 3;
        uint8_t osc_div_en : 3;
        uint8_t reserved : 2;
    }__attribute__((packed)) field;
}pll_flags;
*/
/*************************************************************************
 * Define customized function prototype
 *************************************************************************/
uint32_t hal_clock_get_freq_meter(uint8_t tcksel, uint8_t fcksel);
uint32_t hal_clock_get_freq_meter_cycle(uint8_t tcksel, uint8_t fcksel, uint16_t winset);
uint8_t clock_set_pll_off(clock_pll_id pll_id);
void hal_clock_set_pll_dcm_init(void);
void clock_dump_info(void);

hal_clock_status_t clock_mux_sel(clock_mux_sel_id mux_id, uint32_t mux_sel);
/* Suspend/Resume */
volt_level hal_clock_query_volt_domain(clock_mux_sel_id, uint32_t);
ATTR_TEXT_IN_TCM hal_clock_status_t clock_suspend(void);
ATTR_TEXT_IN_TCM hal_clock_status_t clock_resume(void);
ATTR_TEXT_IN_TCM hal_clock_status_t clock_dsp_dcm_enable(clock_dsp_num dsp_num);
ATTR_TEXT_IN_TCM hal_clock_status_t clock_dsp_dcm_disable(clock_dsp_num dsp_num);
ATTR_TEXT_IN_TCM void hal_clock_set_running_flags(uint32_t, bool);
ATTR_TEXT_IN_SYSRAM void hal_clock_cm4_clk_26m(void);
ATTR_TEXT_IN_SYSRAM void hal_clock_cm4_clk_39m(void);
ATTR_TEXT_IN_SYSRAM void hal_clock_cm4_clk_78m(void);
ATTR_TEXT_IN_SYSRAM void hal_clock_cm4_clk_156m(void);

#endif /* #ifdef HAL_CLOCK_MODULE_ENABLED */
#endif /* #ifndef __HAL_CLOCK_INTERNAL_H__ */
