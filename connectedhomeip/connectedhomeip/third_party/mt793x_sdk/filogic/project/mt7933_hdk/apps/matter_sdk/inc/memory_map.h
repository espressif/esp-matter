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

#ifndef __MEMORY_MAP_H__
#define __MEMORY_MAP_H__


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
 * FORWARD DECLARATIONS - SECTION SYMBOLS @ FLASH PHYSICAL ADDRESS
 *
 ****************************************************************************/


// linker script generated symbols
extern int _rom_bl_start[];
extern int _rom_bl_length[];
extern int _rom_rbl_start[];
extern int _rom_rbl_length[];
extern int _rom_tfm_start[];
extern int _rom_tfm_length[];
extern int _rom_rtos_start[];
extern int _rom_rtos_length[];
extern int _rom_fota_start[];
extern int _rom_fota_length[];
extern int _rom_wifi_patch_start[];
extern int _rom_wifi_patch_length[];
extern int _rom_wifi_start[];
extern int _rom_wifi_length[];
extern int _rom_wifi_ext_start[];
extern int _rom_wifi_ext_length[];
extern int _rom_wifi_pwrtbl_start[];
extern int _rom_wifi_pwrtbl_length[];
extern int _rom_buffer_bin_start[];
extern int _rom_buffer_bin_length[];
extern int _rom_bt_start[];
extern int _rom_bt_length[];
extern int _rom_dsp_start[];
extern int _rom_dsp_length[];
extern int _rom_nvdm_start[];
extern int _rom_nvdm_length[];
extern int _rom_log_start[];
extern int _rom_log_length[];


/****************************************************************************
 *
 * CONSTANTS AND MACROS - SECTION SYMBOLS @ FLASH PHYSICAL ADDRESS
 *
 ****************************************************************************/


#define FLASH_BASE              ((const uint32_t)0)

#define LOADER_BASE             ((const uint32_t)_rom_bl_start)
#define LOADER_LENGTH           ((const uint32_t)_rom_bl_length)
#define PRELOADER_BASE          ((const uint32_t)_rom_rbl_start)
#define PRELOADER_LENGTH        ((const uint32_t)_rom_rbl_length)
#define TFM_BASE                ((const uint32_t)_rom_tfm_start)
#define TFM_LENGTH              ((const uint32_t)_rom_tfm_length)
#define RTOS_BASE               ((const uint32_t)_rom_rtos_start)
#define RTOS_LENGTH             ((const uint32_t)_rom_rtos_length)
#define FOTA_BASE               ((const uint32_t)_rom_fota_start)
#define FOTA_LENGTH             ((const uint32_t)_rom_fota_length)
#define WIFI_PATCH_BASE         ((const uint32_t)_rom_wifi_patch_start)
#define WIFI_PATCH_LENGTH       ((const uint32_t)_rom_wifi_patch_length)
#define WIFI_BASE               ((const uint32_t)_rom_wifi_start)
#define WIFI_LENGTH             ((const uint32_t)_rom_wifi_length)
#define WIFI_EXT_BASE           ((const uint32_t)_rom_wifi_ext_start)
#define WIFI_EXT_LENGTH         ((const uint32_t)_rom_wifi_ext_length)
#define WIFI_PWRTBL_BASE        ((const uint32_t)_rom_wifi_pwrtbl_start)
#define WIFI_PWRTBL_LENGTH      ((const uint32_t)_rom_wifi_pwrtbl_length)
#define CONN_BUF_BIN_BASE       ((const uint32_t)_rom_buffer_bin_start)
#define CONN_BUF_BIN_LENGTH     ((const uint32_t)_rom_buffer_bin_length)
#define BT_BASE                 ((const uint32_t)_rom_bt_start)
#define BT_LENGTH               ((const uint32_t)_rom_bt_length)
#define DSP_BASE                ((const uint32_t)_rom_dsp_start)
#define DSP_LENGTH              ((const uint32_t)_rom_dsp_length)
#define NVDM_BASE               ((const uint32_t)_rom_nvdm_start)
#define NVDM_LENGTH             ((const uint32_t)_rom_nvdm_length)
#define LOG_BASE                ((const uint32_t)_rom_log_start)
#define LOG_FLASH_LENGTH        ((const uint32_t)_rom_log_length)


/****************************************************************************
 *
 * FORWARD DECLARATIONS - SECTION SYMBOLS @ BUS ADDRESS
 *
 ****************************************************************************/


extern int _xip_bl_addr[];
extern int _xip_rbl_addr[];
extern int _xip_tfm_addr[];
extern int _xip_rtos_addr[];
extern int _xip_nvdm_start[];
extern int _xip_log_start[];
extern int _xip_dsp_start[];
extern int _xip_bt_start[];

extern int _sysram_start[];
extern int _sysram_length[];
extern int _ram_start[];
extern int _ram_length[];
extern int _ram_wifi_ext_start[];
extern int _ram_wifi_ext_length[];
extern int _vram_start[];
extern int _vram_length[];
extern int _vsysram_start[];
extern int _vsysram_length[];
extern int _tcm_start[];
extern int _tcm_length[];
extern int _text_start[];
extern int _text_end[];


/****************************************************************************
 *
 * CONSTANTS AND MACROS - SECTION SYMBOLS @ BUS ADDRESS
 *
 ****************************************************************************/


#define XIP_BL_START            ((const uint32_t)_xip_bl_addr)
#define XIP_RBL_START           ((const uint32_t)_xip_rbl_addr)
#define XIP_TFM_START           ((const uint32_t)_xip_tfm_addr)
#define XIP_RTOS_START          ((const uint32_t)_xip_rtos_addr)
#define XIP_NVDM_START          ((const uint32_t)_xip_nvdm_start)
#define XIP_LOG_START           ((const uint32_t)_xip_log_start)
#define XIP_DSP_START           ((const uint32_t)_xip_dsp_start)
#define XIP_BT_START            ((const uint32_t)_xip_bt_start)
#define XIP_BT_LEGNTH           ((const uint32_t)_rom_bt_length)

#define SYSRAM_BASE             ((const uint32_t)_sysram_start)
#define SYSRAM_LENGTH           ((const uint32_t)_sysram_length)
#define RAM_BASE                ((const uint32_t)_ram_start)
#define RAM_LENGTH              ((const uint32_t)_ram_length)
#define RAM_WIFI_EXT_BASE       ((const uint32_t)_ram_wifi_ext_start)
#define RAM_WIFI_EXT_LENGTH     ((const uint32_t)_ram_wifi_ext_length)
#define VRAM_BASE               ((const uint32_t)_vram_start)
#define VRAM_LENGTH             ((const uint32_t)_vram_length)
#define VSYSRAM_BASE            ((const uint32_t)_vsysram_start)
#define VSYSRAM_LENGTH          ((const uint32_t)_vsysram_length)
#define TCM_BASE                ((const uint32_t)_tcm_start)
#define TCM_LENGTH              ((const uint32_t)_tcm_length)
#define TEXT_BASE               ((const uint32_t)_text_start)
#define TEXT_END                ((const uint32_t)_text_end)


/****************************************************************************
 *
 * CONSTANTS AND MACROS - MEMORY TYPE CHECK AND CONVERSION
 *
 ****************************************************************************/


#define MEM_MASK( _addr_ )      ( ( _addr_ ) & 0xFF000000 )
#define ADR_MASK( _addr_ )      ( ( _addr_ ) & 0x00FFFFFF )
#define PHY_SYSRAM_MASK         ( MEM_MASK( SYSRAM_BASE  ) )
#define VIRTUAL_SYSRAM_MASK     ( MEM_MASK( VSYSRAM_BASE ) )
#define PHY_RAM_MASK            ( MEM_MASK( RAM_BASE     ) )
#define VIRTUAL_RAM_MASK        ( MEM_MASK( VRAM_BASE    ) )


#define _IF_SYSRAM_(  _addr_ )  ( MEM_MASK(_addr_) == PHY_SYSRAM_MASK     )
#define _IF_VSYSRAM_( _addr_ )  ( MEM_MASK(_addr_) == VIRTUAL_SYSRAM_MASK )
#define _IF_RAM_(     _addr_ )  ( MEM_MASK(_addr_) == PHY_RAM_MASK        )
#define _IF_VRAM_(    _addr_ )  ( MEM_MASK(_addr_) == VIRTUAL_RAM_MASK    )
#define _ELSE_ :


#define _TO_SYSRAM_(  _addr_ )  ( ADR_MASK(_addr_) | PHY_SYSRAM_MASK     )
#define _TO_RAM_(     _addr_ )  ( ADR_MASK(_addr_) | PHY_RAM_MASK        )
#define _TO_VSYSRAM_( _addr_ )  ( ADR_MASK(_addr_) | VIRTUAL_SYSRAM_MASK )
#define _TO_VRAM_(    _addr_ )  ( ADR_MASK(_addr_) | VIRTUAL_RAM_MASK    )


/**
 * @brief  This macro converts the virtual address to physical address.
 */
#define HAL_CACHE_VIRTUAL_TO_PHYSICAL(address) \
    (                                           \
        _IF_VSYSRAM_( address ) ?               \
            _TO_SYSRAM_( address )              \
        _ELSE_                                  \
        _IF_VRAM_( address ) ?                  \
            _TO_RAM_( address )                 \
        _ELSE_                                  \
            address                             \
    )


/**
 * @brief  This macro converts the physical address to virtual address.
 */
#define HAL_CACHE_PHYSICAL_TO_VIRTUAL(address) \
    (                                           \
        _IF_SYSRAM_( address ) ?                \
            _TO_VSYSRAM_( address )             \
        _ELSE_                                  \
        _IF_RAM_( address ) ?                   \
            _TO_VRAM_( address )                \
        _ELSE_                                  \
            address                             \
    )


/**
 * @brief  This macro checks whether address is in SYSRAM (regardless of
 *         virtual or physical address.
 */
#define IS_ADDR_IN_SYSRAM(address) \
    (                                           \
        _IF_VSYSRAM_( address ) ||              \
        _IF_SYSRAM_ ( address )                 \
    )


/**
 * @brief  This macro checks whether address is in PSRAM (regardless of
 *         virtual or physical address.
 */
#define IS_ADDR_IN_PSRAM(address) \
    (                                           \
        _IF_VRAM_( address ) ||                 \
        _IF_RAM_ ( address )                    \
    )


/****************************************************************************
 *
 * FORWARD DECLARATIONS - IN SECTION SYMBOLS
 *
 ****************************************************************************/


extern int _sysram_code_start[];
extern int _sysram_code_end[];
extern int _sysram_data_start[];
extern int _sysram_data_end[];
extern int _sysram_bss_start[];
extern int _sysram_bss_end[];
extern int _noncached_sysram_code_start[];
extern int _noncached_sysram_code_end[];
extern int _noncached_sysram_data_start[];
extern int _noncached_sysram_data_end[];
extern int _ram_code_start[];
extern int _ram_code_end[];
extern int _ram_data_start[];
extern int _ram_data_end[];
extern int _ram_bss_start[];
extern int _ram_bss_end[];
extern int _noncached_ram_code_start[];
extern int _noncached_ram_code_end[];
extern int _noncached_ram_data_start[];
extern int _noncached_ram_data_end[];
extern int _noncached_ram_bss_start[];
extern int _noncached_ram_bss_end[];
extern int _tcm_code_start[];
extern int _tcm_code_end[];
extern int _tcm_data_start[];
extern int _tcm_data_end[];
extern int _tcm_bss_start[];
extern int _tcm_bss_end[];
extern int _stack_start[];
extern int _stack_end[];


/****************************************************************************
 *
 * CONSTANTS AND MACROS - IN SECTION SYMBOLS
 *
 ****************************************************************************/


#define SYSRAM_CODE_START           ((const uint32_t)_sysram_code_start)
#define SYSRAM_CODE_END             ((const uint32_t)_sysram_code_end)
#define SYSRAM_DATA_START           ((const uint32_t)_sysram_data_start)
#define SYSRAM_DATA_END             ((const uint32_t)_sysram_data_end)
#define SYSRAM_BSS_START            ((const uint32_t)_sysram_bss_start)
#define SYSRAM_BSS_END              ((const uint32_t)_sysram_bss_end)
#define NONCACHED_SYSRAM_CODE_START ((const uint32_t)_noncached_sysram_code_start)
#define NONCACHED_SYSRAM_CODE_END   ((const uint32_t)_noncached_sysram_code_end)
#define NONCACHED_SYSRAM_DATA_START ((const uint32_t)_noncached_sysram_data_start)
#define NONCACHED_SYSRAM_DATA_END   ((const uint32_t)_noncached_sysram_data_end)
#define RAM_CODE_START              ((const uint32_t)_ram_code_start)
#define RAM_CODE_END                ((const uint32_t)_ram_code_end)
#define RAM_DATA_START              ((const uint32_t)_ram_data_start)
#define RAM_DATA_END                ((const uint32_t)_ram_data_end)
#define RAM_BSS_START               ((const uint32_t)_ram_bss_start)
#define RAM_BSS_END                 ((const uint32_t)_ram_bss_end)
#define NONCACHED_RAM_CODE_START    ((const uint32_t)_noncached_ram_code_start)
#define NONCACHED_RAM_CODE_END      ((const uint32_t)_noncached_ram_code_end)
#define NONCACHED_RAM_DATA_START    ((const uint32_t)_noncached_ram_data_start)
#define NONCACHED_RAM_DATA_END      ((const uint32_t)_noncached_ram_data_end)
#define NONCACHED_RAM_BSS_START     ((const uint32_t)_noncached_ram_bss_start)
#define NONCACHED_RAM_BSS_END       ((const uint32_t)_noncached_ram_bss_end)
#define TCM_CODE_START              ((const uint32_t)_tcm_code_start)
#define TCM_CODE_END                ((const uint32_t)_tcm_code_end)
#define TCM_DATA_START              ((const uint32_t)_tcm_data_start)
#define TCM_DATA_END                ((const uint32_t)_tcm_data_end)
#define TCM_BSS_START               ((const uint32_t)_tcm_bss_start)
#define TCM_BSS_END                 ((const uint32_t)_tcm_bss_end)
#define STACK_START                 ((const uint32_t)_stack_start)
#define STACK_END                   ((const uint32_t)_stack_end)


/****************************************************************************
 *
 * CONSTANTS AND MACROS - ROM PARTITION IDENTIFIER
 *
 ****************************************************************************/


typedef enum {
    ROM_REGION_BL                  =  1, /* 1st bootloader */
    ROM_REGION_MCUBOOT_A           =  2, /* 2nd bootloader */
    ROM_REGION_MCUBOOT_B           =  3, /* 2nd bootloader */
    ROM_REGION_TFM_A               =  4, /* TF-M image 1 */
    ROM_REGION_TFM_B               =  5, /* TF-M image 2 */
    ROM_REGION_WIFI_FW             =  6, /* Wi-Fi F/W */
    ROM_REGION_WIFI_PATCH          =  7, /* Wi-Fi F/W */
    ROM_REGION_BT_FW               =  8, /* Bluetooth F/W */
    ROM_REGION_DSP_FW              =  9, /* Bluetooth F/W */
    ROM_REGION_RTOS                = 10, /* RTOS app image */
    ROM_REGION_FOTA                = 11, /* Over-the-Air upgrade image */
    ROM_REGION_MAX,
} rom_region_id_t;


#endif /* #ifndef __MEMORY_MAP_H__ */



