/* Copyright Statement:
 *
 * (C) 2020-2020  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc.
 * ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part,
 * shall be strictly prohibited.
 * You may only use, reproduce, modify,
 * or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit
 * permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY.
 * MEDIATEK EXPRESSLY DISCLAIMS
 * ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY
 * ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM
 * ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE
 * FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO
 * CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND
 * EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT
 * TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */


#ifndef __HAL_EFUSE_GET_H__
#define __HAL_EFUSE_GET_H__


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */


typedef enum hal_efuse_pkg_mode {
    HAL_EFUSE_PKG_MODE_DRQFN,
    HAL_EFUSE_PKG_MODE_WBBGA,
    HAL_EFUSE_PKG_MODE_SBGA,
    HAL_EFUSE_PKG_MODE_RESERVED
} hal_efuse_pkg_mode_t;


typedef enum hal_efuse_segments {
    HAL_EFUSE_SEGMENTS_MT79XX,
    HAL_EFUSE_SEGMENTS_MT85XX,
    HAL_EFUSE_SEGMENTS_MT83XX,
    HAL_EFUSE_SEGMENTS_MT7696,
    HAL_EFUSE_SEGMENTS_RESERVED
} hal_efuse_segments_t;


typedef enum hal_efuse_psram_mode {
    HAL_EFUSE_PSRAM_MODE_4MB_APM_NONUHS,
    HAL_EFUSE_PSRAM_MODE_8MB_APM_NONUHS,
    HAL_EFUSE_PSRAM_MODE_16MB_UHS, /* note: in SDK: it's 8MB */
    HAL_EFUSE_PSRAM_MODE_8MB_UHS,  /* note: in SDK: it's 16MB */
    HAL_EFUSE_PSRAM_MODE_4MB_WB_NONUHS,
    HAL_EFUSE_PSRAM_MODE_8MB_WB_NONUHS,
    HAL_EFUSE_PSRAM_MODE_RESERVED,
    HAL_EFUSE_PSRAM_MODE_NO_PSRAM
} hal_efuse_psram_mode_t;


typedef enum hal_efuse_dsp_off {
    HAL_EFUSE_DSP_OFF_DSP_ON,
    HAL_EFUSE_DSP_OFF_DSP_OFF
} hal_efuse_dsp_off_t;


typedef enum hal_efuse_wf_ax_ac_n_mode {
    HAL_EFUSE_WF_AX_AC_N_MODE_AX_AC_N_MODE,
    HAL_EFUSE_WF_AX_AC_N_MODE_AC_N_MODE,
    HAL_EFUSE_WF_AX_AC_N_MODE_N_MODE,
    HAL_EFUSE_WF_AX_AC_N_MODE_OFF
} hal_efuse_wf_ax_ac_n_mode_t;


typedef enum hal_efuse_bt_mode {
    HAL_EFUSE_BT_MODE_DUAL_MODE,
    HAL_EFUSE_BT_MODE_BLE_ONLY_MODE,
    HAL_EFUSE_BT_MODE_BT_BLE_OFF,
    HAL_EFUSE_BT_MODE_RESERVED
} hal_efuse_bt_mode_t;

typedef enum hal_efuse_f32k_mode {
    HAL_EFUSE_F32K_MODE_RTC_XOSC,
    HAL_EFUSE_F32K_MODE_RTC_EOSC,
    HAL_EFUSE_F32K_MODE_INTERNAL,
    HAL_EFUSE_F32K_MODE_RTC_XOSC_2,
} hal_efuse_f32k_mode_t;

/**
 * @brief     This enumeration define the image type to r/w anti-rollback version
 *            mt7686 only supports one kind of sw image type now
 *
 */
typedef enum {
    ARB_EFUSE_RTOS_IMG,
} arb_efuse_hdr_type_t;

/**
 * @brief     This enumeration returns the status of anti-rollback read/write operation
 *
 */
typedef enum {
    ARB_STATUS_OK,
    ARB_STATUS_INVALID_PARAMETER_OUT_OF_RANGE,
    ARB_STATUS_INVALID_PARAMETER_NULL,
    ARB_STATUS_INVALID_SMALLER_THAN_CURRENT_VER,
    ARB_STATUS_INVALID_PARAMETER_IMAGE_TYPE,
    ARB_STATUS_HW_READ_FAIL,
    ARB_STATUS_HW_WRITE_FAIL,
} arb_status_t;

/** @brief  Read the chip package type
 *
 *  @return the package of this chip.
 */
hal_efuse_pkg_mode_t hal_efuse_get_pkg_mode(void);


/** @brief  Read the chip product segment
 *
 *  @return the product segment of this chip.
 */
hal_efuse_segments_t hal_efuse_get_segments(void);


/** @brief  Read the chip PSRAM mode
 *
 *  @return the PSRAM mode of this chip.
 */
hal_efuse_psram_mode_t hal_efuse_get_psram_mode(void);


/** @brief  Read the chip DSP mode
 *
 *  @return the DSP mode of this chip.
 */
hal_efuse_dsp_off_t hal_efuse_get_dsp_off(void);


/** @brief  Read the chip Wi-Fi mode
 *
 *  @return the Wi-Fi mode of this chip.
 */
hal_efuse_wf_ax_ac_n_mode_t hal_efuse_get_wf_ax_ac_n_mode(void);


/** @brief  Read the chip BT mode
 *
 *  @return the BT mode of this chip.
 */
hal_efuse_bt_mode_t hal_efuse_get_bt_mode(void);

/** @brief  Read the chip F32K clock mode
 *
 *  @return the F32K clock mode of this chip.
 */
hal_efuse_f32k_mode_t hal_efuse_get_f32k_mode(void);

/** @brief  Read the chip security boot enable or not
 *
 *  @return the security boot enable or not of this chip.
 */
bool hal_efuse_get_sbc_enable(void);

/**
 * @brief     This function writes a specific version to the EFUSE address of bootloader anti-rollback,
 *            MockingBird supports 65 BL anti-rollback versions
 * @param[in]   The ver is used to write a specific version to the EFUSE
 * @return  #ARB_STATUS_OK, anti-rollback related operation successfully
 *          #ARB_STATUS_INVALID_PARAMETER_OUT_OF_RANGE, the write version is larger than the supported version
 *          #ARB_STATUS_INVALID_SMALLER_THAN_CURRENT_VER, the write version is smaller than the current version
 *          #ARB_STATUS_HW_READ_FAIL, read anti-rollback from efuse fail
 *          #ARB_STATUS_HW_WRITE_FAIL, write anti-rollback to efuse fail
 *
 */
arb_status_t sboot_efuse_bl_ver_write(uint8_t ver);

/**
 * @brief     This function reads the version of bootloader anti-rollback from EFUSE,
 *            MockingBird supports 65 BL anti-rollback versions
 * @param[out]  The ver is used to store the version read from EFUSE
 * @return  #ARB_STATUS_OK, anti-rollback related operation successfully
 *          #ARB_STATUS_INVALID_PARAMETER_NULL, the input pointer used to store version number is null
 *          #ARB_STATUS_HW_READ_FAIL, read anti-rollback from efuse fail
 *
 */
arb_status_t sboot_efuse_bl_ver_read(uint8_t *ver);

/**
 * @brief     This function writes a specific version to the EFUSE address of SW image anti-rollback,
 *            MockingBird supports 257 RTOS anti-rollback versions
 * @param[in]   The img_type is used to identify the SW image type
 * @param[in]   The ver is used to write a specific version to the EFUSE
 * @return  #ARB_STATUS_OK, anti-rollback related operation successfully
 *          #ARB_STATUS_INVALID_PARAMETER_OUT_OF_RANGE, the write version is larger than the supported version
 *          #ARB_STATUS_INVALID_SMALLER_THAN_CURRENT_VER, the write version is smaller than the current version
 *          #ARB_STATUS_INVALID_PARAMETER_IMAGE_TYPE, not supported image type
 *          #ARB_STATUS_HW_READ_FAIL, read anti-rollback from efuse fail
 *          #ARB_STATUS_HW_WRITE_FAIL, write anti-rollback to efuse fail
 *
 */
arb_status_t sboot_efuse_sw_ver_write(arb_efuse_hdr_type_t img_type, uint32_t ver);

/**
 * @brief     This function reads the version of SW image anti-rollback from EFUSE,
 *            MockingBird supports 257 RTOS anti-rollback versions
 * @param[in]   The img_type is used to identify the SW image type
 * @param[out]  The ver is used to store the version read from EFUSE
 * @return  #ARB_STATUS_OK, anti-rollback related operation successfully
 *          #ARB_STATUS_INVALID_PARAMETER_NULL, the input pointer used to store version number is null
 *          #ARB_STATUS_INVALID_PARAMETER_IMAGE_TYPE, not supported image type
 *          #ARB_STATUS_HW_READ_FAIL, read anti-rollback from efuse fail
 *
 */
arb_status_t sboot_efuse_sw_ver_read(arb_efuse_hdr_type_t img_type, uint32_t *ver);

/**
 * @brief     This function dumps all version of SW image anti-rollback written to EFUSE,
 *            MockingBird supports 257 RTOS anti-rollback versions
 * @return  #ARB_STATUS_OK,  anti-rollback related operation successfully
 *          #ARB_STATUS_READ_FAIL, read anti-rollback from efuse fail
 *
 */
arb_status_t sboot_efuse_sw_ver_dump(void);
#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


#endif /* #ifndef __HAL_EFUSE_GET_H__ */

