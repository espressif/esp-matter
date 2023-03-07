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

#ifndef __HAL_PMU_WRAP_INTERFACE_H__
#define __HAL_PMU_WRAP_INTERFACE_H__

#include "hal_platform.h"
#ifdef HAL_PMU_MODULE_ENABLED
#include "hal_pmu.h"

#define MTK_PMU_BASE                    (PMU_CTRL_BASE)

#define PMU_RG_MLDO3                    (MTK_PMU_BASE+0x0008)
#define PMU_RG_MLDO4                    (MTK_PMU_BASE+0x000C)
#define PMU_RG_MLDO5                    (MTK_PMU_BASE+0x0010)
#define PMU_VS1_3                   (MTK_PMU_BASE+0x0038)
#define PMU_VS1_5                   (MTK_PMU_BASE+0x0040)
#define PMU_RG_SPM_1                    (MTK_PMU_BASE+0x00c0)
#define PMU_RG_SPM_3                    (MTK_PMU_BASE+0x00c8)
#define PMU_RG_SPM_5                    (MTK_PMU_BASE+0x00d0)

#define delay_us(delay)         hal_gpt_delay_us(delay)

/*****************************************************************************
 * Structures
 *****************************************************************************/

/** @brief This structure defines the init config structure. */
typedef struct {
    hal_pmu_status_t (*init)(void);
    hal_pmu_status_t (*init_power_mode)(void);
    hal_pmu_status_t (*set_vcore_voltage)(hal_pmu_vcore_vosel_t vol);
    hal_pmu_status_t (*enter_sleep_mode)(void);
    hal_pmu_status_t (*enter_retention_mode)(hal_pmu_retention_status_t status);
    hal_pmu_status_t (*resume_to_normal)(hal_pmu_stage_flow mode);
    hal_pmu_status_t (*set_cldo_voltage)(int index);
    hal_pmu_status_t (*set_mldo_voltage)(hal_pmu_mldo_vosel_t vol);
    hal_pmu_status_t (*control_retention_lod)(hal_pmu_control_t status);
    hal_pmu_status_t (*sram_power_switch_control)(hal_pmu_control_t status);
    hal_pmu_status_t (*set_retention_ldo)(int adjust_value);
} hal_pmu_wrap_struct;

hal_pmu_status_t pmu_init_mt7933(void);
hal_pmu_status_t pmu_set_vcore_voltage_mt7933(hal_pmu_vcore_vosel_t vol);
hal_pmu_status_t pmu_set_mldo_voltage_mt7933(hal_pmu_mldo_vosel_t vol);

hal_pmu_status_t pmu_set_mldo_slp_vol_mt7933(hal_pmu_mldo_vosel_t vol);
hal_pmu_status_t pmu_set_vcore_slp_vol_mt7933(hal_pmu_vcore_vosel_t vol);

hal_pmu_status_t pmu_mldo_psw_en(void);

#endif /* #ifdef HAL_PMU_MODULE_ENABLED */
#endif /* #ifndef __HAL_PMU_WRAP_INTERFACE_H__ */
