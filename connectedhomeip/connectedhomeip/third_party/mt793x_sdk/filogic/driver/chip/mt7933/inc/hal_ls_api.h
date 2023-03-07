/* Copyright Statement:
 *
 * (C) 2020-2020  MediaTek Inc. All rights reserved.
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


#include <stdint.h>
#include <stdbool.h>


#ifndef __HAL_LS_API_H__
#define __HAL_LS_API_H__


struct hal_ls_register {
    uint32_t    reg;
    uint32_t    val;
    uint32_t    bak;
};


struct hal_ls_module {
    int init_count;
    int conf_count;
    int lock_count;

    struct hal_ls_register *init_register;
    struct hal_ls_register *conf_register;
    struct hal_ls_register *lock_register;
};
#define hal_ls_module_size sizeof(struct hal_ls_module)


/**
 * @note Due to register resource limitation. Table alignment is forced to 64
 *       bytes on MT7933. May need to generalize on future platforms.
 */
struct hal_ls_table {
    int count;
    struct hal_ls_module *modules;
} __attribute__((aligned(64)));
#define hal_ls_table_size sizeof(struct hal_ls_table)

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */


/**
 * Declare backup/restore API function.
 *
 * Planned usage:
 *
 * 1. call the following API during init phase:
 *
 *    hal_ls_init()
 *    hal_ls_lock()
 *
 * 2. call the following API during backup phase:
 *
 *    hal_ls_backup()
 *
 * 3. call the following API during restore phase:
 *
 *    hal_ls_restore()
 *    hal_ls_lock()
 */


void hal_ls_init(struct hal_ls_table *table);


void hal_ls_lock(struct hal_ls_table *table);


void hal_ls_backup(struct hal_ls_table *table);


void hal_ls_restore(struct hal_ls_table *table);


/**
 * API to check the given memory address is within the range of allowed
 * memory region and alignment.
 *
 * The purpose of this API is checked the validness of a memory address
 * that can be used to backup/restore register (and potentially some other)
 * settings.
 *
 * @param addr the address flash initialization paramteres.
 *
 * @retval true if addr falls within SYSRAM.
 * @retval false if addr falls out of SYSRAM.
 *
 * @note MT7933 SYSRAM can be 0x8000-0000 or 0x0800-0000 based. 64-bytes
 *       alignment is required, too.
 */
bool hal_ls_is_valid_memory(uint32_t addr);


/**
 * Store the address of flash initialization parameters for later use.
 *
 * This API is the counter part of hal_ls_flash_init_param_addr_get.
 * The addr must be in the range of 0x8000-0000 ~ 0x8010-0000 and must
 *        be 64-bytes aligned.
 *
 * @param addr the address flash initialization paramteres.
 *
 * @return true if the address is valid, false otherwise.
 *
 */
bool hal_ls_flash_init_param_addr_set(void *addr);


/**
 * Get the address of flash initialization parameters from storage.
 *
 * This API is the counter part of hal_ls_flash_init_param_addr_set.
 *
 * @return The address of flash initialization parameters.
 */
void *hal_ls_flash_init_param_addr_get(void);


/**
 * Store the address of secure settings for later use.
 *
 * This API is the counter part of hal_ls_secure_settings_addr_get.
 * The addr must be in the range of 0x8000-0000 ~ 0x8010-0000 and must
 *        be 64-bytes aligned.
 *
 * @param addr the address secure settings.
 *
 * @return true if the address is valid, false otherwise.
 *
 */
bool hal_ls_secure_settings_addr_set(void *addr);


/**
 * Get the address of secure settings from storage.
 *
 * This API is the counter part of hal_ls_secure_settings_addr_set.
 *
 * @return The address of secure settings.
 */
void *hal_ls_secure_settings_addr_get(void);


/**
 * Store the address of hardware CR patch for later use.
 *
 * This API is the counter part of hal_ls_hw_cr_patch__addr_get.
 * The addr must be in the range of 0x8000-0000 ~ 0x8010-0000 and must
 *        be 64-bytes aligned.
 *
 * @param addr the address of hardware CR patch.
 *
 * @return true if the address is valid, false otherwise.
 *
 */
bool hal_ls_hw_cr_patch_addr_set(void *addr);


/**
 * Get the address of hardware CR patch from storage.
 *
 * This API is the counter part of hal_ls_hw_cr_patch_addr_set.
 *
 * @return The address of hardware CR patch.
 */
void *hal_ls_hw_cr_patch_addr_get(void);


/**
 * Store the address of PSRAM PARAMETER for later use.
 *
 * This API is the counter part of hal_ls_psram_param_addr_get.
 * The addr must be in the range of 0x8000-0000 ~ 0x8010-0000 and must
 *        be 64-bytes aligned.
 *
 * @param addr the address of PSRAM PARAMETER.
 *
 * @return true if the address is valid, false otherwise.
 *
 */
bool hal_ls_psram_param_addr_set(void *addr);


/**
 * Get the address of PSRAM PARAMETER from storage.
 *
 * This API is the counter part of hal_ls_psram_param_addr_set.
 *
 * @return The address of PSRAM PARAMETER.
 */
void *hal_ls_psram_param_addr_get(void);


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


#endif /* #ifndef __HAL_LS_API_H__ */
