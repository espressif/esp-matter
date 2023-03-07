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

#ifndef __HAL_SLEEP_MANAGER_INTERNAL_H__
#define __HAL_SLEEP_MANAGER_INTERNAL_H__
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_platform.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#define SLEEP_MANAGEMENT_DEBUG_ENABLE
#undef SLP_SUPPORT_LP_DEBUG_PROBE

typedef enum {
    UNLOCK_SLEEP    = 0,
    LOCK_SLEEP      = 1
} sleep_management_lock_sleep_t;

typedef enum {
    SLP_SUSPEND  = 0,
    SLP_RESUME = 1
} sleep_management_suspend_resume_type_t;


typedef enum {
    SLEEP_MANAGEMENT_UNINITIALIZED  = 0,
    SLEEP_MANAGEMENT_INITIALIZED    = 1
} sleep_management_init_status_t;


typedef struct {
    void *func;
    void *data;
} sleep_management_callback_func_t;


struct slp_deep_sleep_return_t {
    volatile uint32_t sp;
    volatile uint32_t pc;
};

typedef enum {
    SLEEP_MANAGEMENT_LOG_ID_EMPTY = 0,
    SLEEP_MANAGEMENT_LOG_ID_ERROR = 1,
    SLEEP_MANAGEMENT_LOG_ID_TICKLESS_WFI = 2,
    SLEEP_MANAGEMENT_LOG_ID_TICKLESS_DS = 3,
    SLEEP_MANAGEMENT_LOG_ID_TICKLESS_LEGACY = 4,
    SLEEP_MANAGEMENT_LOG_ID_MAX
} sleep_management_debug_log_id_t;

void sleep_management_register_suspend_callback(sleep_management_backup_restore_module_t module, hal_sleep_manager_callback_t callback, void *data);
void sleep_management_register_resume_callback(sleep_management_backup_restore_module_t module, hal_sleep_manager_callback_t callback, void *data);
void sleep_management_module_suspend(void);
void sleep_management_module_resume(void);
uint8_t sleep_management_get_lock_handle(const char *handle_name);
hal_sleep_manager_status_t sleep_management_release_lock_handle(uint8_t handle_index);
hal_sleep_manager_status_t sleep_management_lock_sleep(sleep_management_lock_sleep_t lock, uint8_t handle_index);
bool sleep_management_check_handle_status(uint8_t handle_index);
char *sleep_management_sleep_handle_name(uint32_t handle_index);
uint32_t sleep_management_get_lock_sleep_request_info(void);
uint32_t sleep_management_get_lock_sleep_handle_list(void);
void sleep_management_low_power_init_setting(void);
int8_t hal_lp_connsys_get_own_enable_int(void);
int8_t hal_lp_connsys_give_n9_own(void);
void sleep_management_read_mtcmos_status(uint32_t *mtcmos_onoff_state, uint32_t *sram_onoff_state, uint32_t *sram_sleep_state);
void sleep_management_dump_mtcmos_status(uint32_t mtcmos_onoff_state, uint32_t sram_onoff_state, uint32_t sram_sleep_state);
void sleep_management_internal_init(void);
void sleep_management_platform_sys_init(void);
void sleep_management_platform_init(void);
bool sleep_management_check_sleep_mode_lock(hal_sleep_mode_t mode, bool full_check);
uint32_t sleep_management_enter_deep_sleep(hal_sleep_mode_t mode);
uint32_t sleep_management_enter_deep_sleep_sys(void);
void sleep_management_enter_wfi_sys(void);
uint32_t sleep_management_enter_wfi(void);
void sleep_management_dump_status(void);
void slp_wdt_mpu_backup(void *data);
void slp_wdt_mpu_restore(void *data);



#ifdef SLEEP_MANAGEMENT_DEBUG_ENABLE

void sleep_management_debug_lock_sleep_timelog(sleep_management_lock_sleep_t lock, uint8_t handle_index);
void sleep_management_debug_dump_lock_sleep_time(void);
void sleep_management_debug_reset_lock_sleep_time(void);
void sleep_management_debug_dump_suspend_resume_time(void);
void sleep_management_debug_dump_suspend_mtcmos_sram_status(void);
void sleep_management_debug_sleep_log(sleep_management_debug_log_id_t log_id, uint16_t v1, uint16_t v2, uint32_t v3);

#else /* #ifdef SLEEP_MANAGEMENT_DEBUG_ENABLE */

#define sleep_management_debug_lock_sleep_timelog(...)
#define sleep_management_debug_dump_lock_sleep_time(...)
#define sleep_management_debug_reset_lock_sleep_time(...)
#define sleep_management_debug_dump_suspend_resume_time(...)
#define sleep_management_debug_dump_suspend_mtcmos_sram_status(...)
#define sleep_management_debug_sleep_log(...)

#endif /* #ifdef SLEEP_MANAGEMENT_DEBUG_ENABLE */

#endif /* #ifdef HAL_SLEEP_MANAGER_ENABLED */
#endif /* #ifndef __HAL_SLEEP_MANAGER_INTERNAL_H__ */

