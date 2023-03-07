/* Copyright Statement:
 *
 * (C) 2020-2022  MediaTek Inc. All rights reserved.
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


#ifndef __HAL_BOOT_H__
#define __HAL_BOOT_H__

#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/**
 * @defgroup top Enum
 * @{
 */

/** @brief Currently possible booting sources are listed. */
typedef enum {
    HAL_BOOT_SOURCE_UART,   /**< booting from UART.        */
    HAL_BOOT_SOURCE_NORMAL, /**< booting from normal flow. */
    HAL_BOOT_SOURCE_SDIO,   /**< booting from SDIO.        */
    HAL_BOOT_SOURCE_USB     /**< booting from USB.         */
} hal_boot_source_t;

/**
 * @}
 */


/** @brief  Detemine how the chip is booting.
 *
 *  The chip may have previously booted up and to save power, it went into
 *  sleep mode that turns off the CPU and kept some hardware up during the
 *  sleep. Software can use this API to tell whether CPU is booting from a
 *  deep sleep or from a clean boot.
 *
 *  @retval true  the CPU is booting from a low power state.
 *  @retval false the CPU is booting from a power cycle or just booting up.
 */
bool hal_boot_is_resuming(void);


/** @brief  Read the booting source from hardware.
 *
 *  @return the source of booting to calling function.
 */
hal_boot_source_t hal_boot_get_boot_source(void);


/** @brief  Detemine whether this is a FPGA or ASIC
 *
 *  @retval true    if running on an FPGA.
 *  @retval false   if running on an ASIC.
 */
#define hal_boot_is_fpga() ((bool)((*(volatile int *)0x340305F0) != 0))


/** @brief  Read the chip revision
 *
 *  @return the revision ID of this chip.
 */
uint16_t hal_boot_get_hw_ver(void);


/** @brief  Read the BROM revision
 *
 *  @return the BROM revision of this chip.
 */
uint16_t hal_boot_get_fw_ver(void);


/** @brief  Store the duration of BOOTROM
 *
 *  @param duration of BOOTROM execution in unit of millisecond.
 */
void hal_boot_set_bootrom_duration(uint16_t duration);


/** @brief  Read the duration of BOOTROM
 *
 *  @return duration of BOOTROM execution in unit of millisecond.
 */
uint16_t hal_boot_get_bootrom_duration(void);


/** @brief  Read the duration of BOOTLOADER
 *
 *  @return duration of BOOTLOADER execution in unit of millisecond.
 */
void hal_boot_set_bootloader_duration(uint16_t duration);


/** @brief  Read the duration of BOOTLOADER
 *
 *  @return duration of BOOTLOADER execution in unit of millisecond.
 */
uint16_t hal_boot_get_bootloader_duration(void);


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


#endif /* #ifndef __HAL_BOOT_H__ */

