/* Copyright Statement:
 *
 * (C) 2020-2021  MediaTek Inc. All rights reserved.
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

#ifndef __MEMORY_MAP_MT7933_H__
#define __MEMORY_MAP_MT7933_H__


/**
 * @file this file contains symbols created by linker.
 *
 * These linker script symbols has only address but no memory associated.
 *
 * @note do not do math in macros using these symbols. If needed, do the math
 *       in linker script.
 */


/****************************************************************************
 *
 * HEADER FILES
 *
 ****************************************************************************/


#include <stdint.h>


/****************************************************************************
 *
 * FORWARD DECLARATIONS - SECTION SYMBOLS @ BUS ADDRESS
 *
 ****************************************************************************/

/* FLASH */
extern int _xip_bl_addr[];
extern int _xip_bl_length[];
extern int _xip_rbl_addr[];
extern int _xip_rbl_length[];
extern int _xip_tfm_int_addr[];
extern int _xip_tfm_int_length[];
extern int _xip_tfm_addr[];
extern int _xip_tfm_length[];
extern int _xip_rtos_addr[];
extern int _xip_rtos_length[];
extern int _xip_fota_addr[];
extern int _xip_fota_length[];
extern int _xip_nvdm_start[];
extern int _xip_nvdm_length[];

#ifdef MTK_AUDIO_SUPPORT
extern int _xip_dsp_start[];
extern int _xip_dsp_length[];
#endif /* ifdef MTK_AUDIO_SUPPORT */

#ifdef TFM_NORMAL_BUILD
extern int _xip_log_start[];
extern int _xip_log_length[];
#endif /* ifdef TFM_NORMAL_BUILD */
extern int _xip_bt_start[];
extern int _xip_bt_length[];
extern int _xip_wifi_pwrtbl_start[];
extern int _xip_wifi_pwrtbl_length[];
extern int _xip_wifi_ext_addr[];
extern int _xip_wifi_ext_length[];
extern int _xip_buffer_bin_addr[];
extern int _xip_buffer_bin_length[];

/* SYSRAM */
extern int _sysram_start[];
extern int _sysram_length[];
extern int _sysram_bl_start[];
extern int _sysram_bl_length[];
extern int _sysram_tfm_start[];
extern int _sysram_tfm_length[];
extern int _sysram_sys_start[];
extern int _sysram_sys_length[];
extern int _vsysram_start[];
extern int _vsysram_length[];

/* PSRAM */
#ifdef MTK_AUDIO_SUPPORT
extern int _ram_bt_fw_start[];
extern int _ram_bt_fw_length[];
extern int _ram_dsp_start[];
extern int _ram_dsp_length[];
extern int _ram_audio_start[];
extern int _ram_audio_length[];
#endif

extern int _ram_start[];
extern int _ram_length[];
extern int _ram_wifi_fw_start[];
extern int _ram_wifi_fw_length[];
extern int _vram_start[];
extern int _vram_length[];

/* TCM */
extern int _tcm_cache_start[];
extern int _tcm_cache_length[];
extern int _tcm_start[];
extern int _tcm_length[];
extern int _tcm_sec_shm_start[];
extern int _tcm_sec_shm_length[];
extern int _tcm_tfm_start[];
extern int _tcm_tfm_length[];
extern int _tcm_bl_start[];
extern int _tcm_bl_length[];


/****************************************************************************
 *
 * CONSTANTS AND MACROS - SECTION SYMBOLS @ BUS ADDRESS
 *
 ****************************************************************************/

/* FLASH */
#define XIP_BL_START            ((const uint32_t)_xip_bl_addr)
#define XIP_BL_LENGTH           ((const uint32_t)_xip_bl_length)
#define XIP_RBL_START           ((const uint32_t)_xip_rbl_addr)
#define XIP_RBL_LENGTH          ((const uint32_t)_xip_rbl_length)
#define XIP_TFM_INT_START       ((const uint32_t)_xip_tfm_int_addr)
#define XIP_TFM_INT_LENGTH      ((const uint32_t)_xip_tfm_int_length)
#define XIP_TFM_START           ((const uint32_t)_xip_tfm_addr)
#define XIP_TFM_LENGTH          ((const uint32_t)_xip_tfm_length)
#define XIP_RTOS_START          ((const uint32_t)_xip_rtos_addr)
#define XIP_RTOS_LENGTH         ((const uint32_t)_xip_rtos_length)
#define XIP_FOTA_START          ((const uint32_t)_xip_fota_addr)
#define XIP_FOTA_LENGTH         ((const uint32_t)_xip_fota_length)
#define XIP_NVDM_START          ((const uint32_t)_xip_nvdm_start)
#define XIP_NVDM_LENGTH         ((const uint32_t)_xip_nvdm_length)
#ifdef TFM_NORMAL_BUILD
#define XIP_LOG_START           ((const uint32_t)_xip_log_start)
#define XIP_LOG_LENGTH          ((const uint32_t)_xip_log_length
#endif /* ifdef TFM_NORMAL_BUILD */

#ifdef MTK_AUDIO_SUPPORT
#define XIP_DSP_START           ((const uint32_t)_xip_dsp_start)
#define XIP_DSP_LENGTH          ((const uint32_t)_xip_dsp_length)
#endif

#define XIP_BT_START            ((const uint32_t)_xip_bt_start)
#define XIP_BT_LENGTH           ((const uint32_t)_xip_bt_length)
#define XIP_WIFI_PWRTBL_START   ((const uint32_t)_xip_wifi_pwrtbl_start)
#define XIP_WIFI_PWRTBL_LENGTH  ((const uint32_t)_xip_wifi_pwrtbl_length)
#define XIP_WIFI_EXT_START      ((const uint32_t)_xip_wifi_ext_addr)
#define XIP_WIFI_EXT_LENGTH     ((const uint32_t)_xip_wifi_ext_length)
#define XIP_BUFFER_BIN_START    ((const uint32_t)_xip_buffer_bin_addr)
#define XIP_BUFFER_BIN_LENGTH   ((const uint32_t)_xip_buffer_bin_length)

/* SYSRAM */
#define SYSRAM_BASE             ((const uint32_t)_sysram_start)
#define SYSRAM_LENGTH           ((const uint32_t)_sysram_length)
#define SYSRAM_BL_BASE          ((const uint32_t)_sysram_bl_start)
#define SYSRAM_BL_LENGTH        ((const uint32_t)_sysram_bl_length)
#define SYSRAM_TFM_BASE         ((const uint32_t)_sysram_tfm_start)
#define SYSRAM_TFM_LENGTH       ((const uint32_t)_sysram_tfm_length)
#define SYSRAM_SYS_BASE         ((const uint32_t)_sysram_sys_start)
#define SYSRAM_SYS_LENGTH       ((const uint32_t)_sysram_sys_length)
#define VSYSRAM_BASE            ((const uint32_t)_vsysram_start)
#define VSYSRAM_LENGTH          ((const uint32_t)_vsysram_length)

/* PSRAM */
#ifdef MTK_AUDIO_SUPPORT
#define RAM_BT_FW_BASE          ((const uint32_t)_ram_bt_fw_start)
#define RAM_BT_FW_LENGTH        ((const uint32_t)_ram_bt_fw_length)
#define RAM_DSP_BASE            ((const uint32_t)_ram_dsp_start)
#define RAM_DSP_LENGTH          ((const uint32_t)_ram_dsp_length)
#define RAM_AUDIO_BASE          ((const uint32_t)_ram_audio_start)
#define RAM_AUDIO_LENGTH        ((const uint32_t)_ram_audio_length)
#endif

#define RAM_BASE                ((const uint32_t)_ram_start)
#define RAM_LENGTH              ((const uint32_t)_ram_length)
#define RAM_WIFI_FW_BASE        ((const uint32_t)_ram_wifi_fw_start)
#define RAM_WIFI_FW_LENGTH      ((const uint32_t)_ram_wifi_fw_length)
#define VRAM_BASE               ((const uint32_t)_vram_start)
#define VRAM_LENGTH             ((const uint32_t)_vram_length)

/* TCM */
#define TCM_CACHE_BASE          ((const uint32_t)_tcm_cache_start)
#define TCM_CACHE_LENGTH        ((const uint32_t)_tcm_cache_length)
#define TCM_SYS_BASE            ((const uint32_t)_tcm_start)
#define TCM_SYS_LENGTH          ((const uint32_t)_tcm_length)
#define TCM_SEC_SHM_BASE        ((const uint32_t)_tcm_sec_shm_start)
#define TCM_SEC_SHM_LENGTH      ((const uint32_t)_tcm_sec_shm_length)
#define TCM_TFM_BASE            ((const uint32_t)_tcm_tfm_start)
#define TCM_TFM_LENGTH          ((const uint32_t)_tcm_tfm_length)
#define TCM_BL_BASE             ((const uint32_t)_tcm_bl_start)
#define TCM_BL_LENGTH           ((const uint32_t)_tcm_bl_length)


/****************************************************************************
 *
 * CONSTANTS AND MACROS - MEMORY TYPE CHECK AND CONVERSION
 *
 ****************************************************************************/

#define MEM_MASK( _addr_ )              ( ( _addr_ ) & 0xFF000000 )
#define ADR_MASK( _addr_ )              ( ( _addr_ ) & 0x00FFFFFF )
#define PHY_FLASH_MASK( _addr_ )        ( ADR_MASK( _addr_  ) | (0x90000000))
#define VIR_SYSRAM_MASK( _addr_ )       ( ADR_MASK( _addr_  ) | (0x08000000))

#endif /* __MEMORY_MAP_MT7933_H__ */

