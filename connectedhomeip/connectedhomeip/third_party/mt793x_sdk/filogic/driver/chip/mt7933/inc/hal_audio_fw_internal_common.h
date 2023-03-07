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

#ifndef __HAL_AUDIO_FW_INTERNAL_COMMON_H__
#define __HAL_AUDIO_FW_INTERNAL_COMMON_H__

/*******************************************************************************\
| Include Files                                                                 |
\*******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal_platform.h"
#include "hal_audio_post_processing_internal.h"

/*******************************************************************************\
| Defines                                                                       |
\*******************************************************************************/

//#define __DSP_COMPRESS_7Z__ /* Disable LZMA for shortening response time */

#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
#define DDL_PCM_SD_TAB_LEN  1
#define DDL_AUD_IIR_TAB_LEN 1
#else /* #if defined(MTK_AUDIO_IIR_FILTER_ENABLE) */
#define DDL_PCM_SD_TAB_LEN  0
#define DDL_AUD_IIR_TAB_LEN 0
#endif /* #if defined(MTK_AUDIO_IIR_FILTER_ENABLE) */
#define DDL_PCM_TAB_LEN     2 + DDL_AUD_IIR_TAB_LEN + DDL_PCM_SD_TAB_LEN
#define DDL_I2S_TAB_LEN     3 + DDL_AUD_IIR_TAB_LEN
#define DDL_AAC_TAB_LEN     5 + DDL_AUD_IIR_TAB_LEN
#define DDL_SBC_DEC_TAB_LEN 4 + DDL_AUD_IIR_TAB_LEN
#define DDL_SPH_ENH_TAB_LEN 4

#define DDL_PCM_FLAG        1
#if defined(HAL_I2S_MODULE_ENABLED)
#define DDL_I2S_FLAG        1
#else /* #if defined(HAL_I2S_MODULE_ENABLED) */
#define DDL_I2S_FLAG        0
#endif /* #if defined(HAL_I2S_MODULE_ENABLED) */
#if defined(MTK_BT_A2DP_AAC_ENABLE)
#define DDL_AAC_FLAG        1
#else /* #if defined(MTK_BT_A2DP_AAC_ENABLE) */
#define DDL_AAC_FLAG        0
#endif /* #if defined(MTK_BT_A2DP_AAC_ENABLE) */
#define DDL_SBC_DEC_FLAG    1
#define DDL_SPH_ENH_FLAG    1
#define DDL_ID_ADDR_TAB_LEN (DDL_PCM_FLAG + DDL_I2S_FLAG + DDL_AAC_FLAG + DDL_SBC_DEC_FLAG + DDL_SPH_ENH_FLAG)

#define DDL_CM_PAGE         4
#define DDL_CM2_PAGE        8
#define DDL_PM_PAGE         3
#define DDL_DM_PAGE         6

#define DDL_CM_TYPE         1
#define DDL_PM_TYPE         2
#define DDL_DM_TYPE         3

/*******************************************************************************\
| Structures                                                                    |
\*******************************************************************************/

typedef struct {
    const uint16_t *enable;
    const uint16_t *page;
    const uint16_t *address;
    const uint32_t *value;
    const uint32_t *content;
} dsp_audio_fw_patch_information_t;

typedef struct {
    int32_t address;
    const uint8_t *image;
    int32_t length;
} dsp_audio_fw_ddl_address_t;

typedef struct {
    uint32_t dsp_start_address;    /**< The dsp address that the codec will be downloaded to.*/
    const uint8_t *compressed_data_array_buffer;    /**< The data buffer of the compressed codec*/
    uint32_t compressed_data_array_length;    /**< The data buffer length of the compressed codec*/
    uint32_t uncompressed_data_array_length;    /**< The data length of the uncompressed codec*/
} dsp_dynamic_download_compressed_data_t;

typedef struct {
    uint16_t type;
    uint16_t page;
#ifndef __DSP_COMPRESS_7Z__
    const dsp_audio_fw_ddl_address_t *info;
#else /* #ifndef __DSP_COMPRESS_7Z__ */
    const dsp_dynamic_download_compressed_data_t *info;
#endif /* #ifndef __DSP_COMPRESS_7Z__ */
} dsp_audio_fw_type_address_entry_t;


typedef struct {
    uint16_t fw_id;
    uint16_t count;
    const dsp_audio_fw_type_address_entry_t *table;
} dsp_audio_fw_id_address_entry_t;

/*******************************************************************************\
| Tables                                                                        |
\*******************************************************************************/

#ifndef __DSP_COMPRESS_7Z__
extern const dsp_audio_fw_ddl_address_t cmdll_AAC;
extern const dsp_audio_fw_ddl_address_t cmdll_SBC_DEC;
extern const dsp_audio_fw_ddl_address_t cmdll_Header_Kernel;
extern const dsp_audio_fw_ddl_address_t cmdll_Audio_Header;
extern const dsp_audio_fw_ddl_address_t cmdll_TCH;
extern const dsp_audio_fw_ddl_address_t cmdll_TCH2;
extern const dsp_audio_fw_ddl_address_t cmdll_I2S;
extern const dsp_audio_fw_ddl_address_t cmdll_IIR;
extern const dsp_audio_fw_ddl_address_t cmdll_PCM_SD_PLAYBACK;
extern const dsp_audio_fw_ddl_address_t pmdll_AAC;
extern const dsp_audio_fw_ddl_address_t pmdll_SBC_DEC;
extern const dsp_audio_fw_ddl_address_t dmdll_AAC;
extern const dsp_audio_fw_ddl_address_t pmdll_AECS;

#else /* #ifndef __DSP_COMPRESS_7Z__ */
extern const dsp_dynamic_download_compressed_data_t cmdll_AAC;
extern const dsp_dynamic_download_compressed_data_t cmdll_SBC_DEC;
extern const dsp_dynamic_download_compressed_data_t cmdll_Header_Kernel;
extern const dsp_dynamic_download_compressed_data_t cmdll_Audio_Header;
extern const dsp_dynamic_download_compressed_data_t cmdll_TCH;
extern const dsp_dynamic_download_compressed_data_t cmdll_TCH2;
extern const dsp_dynamic_download_compressed_data_t cmdll_I2S;
extern const dsp_dynamic_download_compressed_data_t cmdll_IIR;
extern const dsp_dynamic_download_compressed_data_t cmdll_PCM_SD_PLAYBACK;
extern const dsp_dynamic_download_compressed_data_t pmdll_AAC;
extern const dsp_dynamic_download_compressed_data_t pmdll_SBC_DEC;
extern const dsp_dynamic_download_compressed_data_t dmdll_AAC;
extern const dsp_dynamic_download_compressed_data_t pmdll_AECS;
#endif /* #ifndef __DSP_COMPRESS_7Z__ */

extern const dsp_audio_fw_patch_information_t dsp_patch_normal;
extern const dsp_audio_fw_patch_information_t dsp_patch_slt;

#endif /* #ifndef __HAL_AUDIO_FW_INTERNAL_COMMON_H__ */
