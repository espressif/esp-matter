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
/* MediaTek restricted information */

#ifndef __BT_PLATFORM_INTERNAL_H__
#define __BT_PLATFORM_INTERNAL_H__

#ifdef __cplusplus
#define BT_EXTERN_C_BEGIN extern "C" {
#else
#define BT_EXTERN_C_BEGIN
#endif

#ifdef __cplusplus
#define BT_EXTERN_C_END }
#else
#define BT_EXTERN_C_END
#endif

#include <stdint.h>
#include "bt_platform.h"

#include "bt_os_layer_api.h"

BT_EXTERN_C_BEGIN

/* If successful, these should return 0 */
#define BT_MUTEX_T                  uint32_t
#define BT_RETURN_ADDRESS ((int32_t)__builtin_return_address(0))
#define BT_THREAD_SAFE_ENABLED
#ifdef BT_THREAD_SAFE_ENABLED
#define BT_MUTEX_INIT()
#define BT_MUTEX_LOCK() bt_os_take_stack_mutex()
#define BT_MUTEX_UNLOCK() bt_os_give_stack_mutex()
#define BT_MUTEX_UNINIT()
#else
#define BT_MUTEX_INIT()
#define BT_MUTEX_LOCK()
#define BT_MUTEX_UNLOCK()
#define BT_MUTEX_UNINIT()
#endif

#define BT_UNUSED(x)    (void)(x)

/* For util/timer use, get current system tick */
uint32_t os_bt_get_current_tick(void);

uint32_t bt_os_get_stack_mutex(void);
void bt_os_take_stack_mutex(void);
void bt_os_give_stack_mutex(void);
void bt_init(void);
void bt_hummingbird_deinit(void);
void bt_hci_tx_notify(void);
void bt_hci_notify(void);
void bt_fw_assert_notify(void);
int32_t bt_timer_start_timer(uint32_t ms);
int32_t bt_timer_stop_timer(void);
uint32_t bt_timer_get_current_tick(void);
void *bt_memcpy(void *dest, const void *src, uint32_t size);
int bt_memcmp(const void *buf1, const void *buf2, uint32_t size);
void *bt_memset(void *buf, uint8_t ch, uint32_t size);
void *bt_memmove(void *dest, const void *src, uint32_t size);
bool bt_is_bt_task_running(void);


/* color */
#define BT_COLOR_SET(color)
#define BT_COLOR_RED
#define BT_COLOR_BLUE
#define BT_COLOR_GREEN
#define BT_COLOR_WHITE

typedef struct {
    uint32_t total[2];
    uint32_t state[4];
    unsigned char buffer[64];
} bt_md5_context;

void bt_memory_info_notify(void *p);
/*For obex*/
void bt_md5_init(bt_md5_context *ctx);
void bt_md5_free(bt_md5_context *ctx);
void bt_md5_clone(bt_md5_context *dst, const bt_md5_context *src);
void bt_md5_starts(bt_md5_context *ctx);
void bt_md5_process(bt_md5_context *ctx, const unsigned char data[64]);
void bt_md5_update(bt_md5_context *ctx, const unsigned char *input, unsigned int ilen);
void bt_md5_finish(bt_md5_context *ctx, unsigned char output[16]);
BT_EXTERN_C_END

#endif
