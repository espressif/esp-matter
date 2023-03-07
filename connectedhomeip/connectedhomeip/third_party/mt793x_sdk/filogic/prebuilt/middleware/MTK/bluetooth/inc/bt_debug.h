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

#ifndef __BT_DEBUG_H__
#define __BT_DEBUG_H__

/**
 * This header file describes the debug apis used by Bluetooth stack.
 */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "bt_log.h"
#include "bt_type.h"
#include "bt_os_layer_api.h"

BT_EXTERN_C_BEGIN
#if (!defined(BT_NO_DEBUG) && !defined(BT_DEBUG))
#define BT_DEBUG
#endif

#define COLOR_NONE          "\033[m"
#define COLOR_RED           "\033[0;31m"
#define COLOR_GREEN         "\033[0;32m"
#define COLOR_BLUE          "\033[0;34m"
#define COLOR_CYAN          "\033[0;36m"
#define COLOR_PURPLE        "\033[0;35m"
#define COLOR_BROWN         "\033[0;33m"
#define COLOR_LIGHT_GRAY    "\033[0;37m"
#define COLOR_LIGHT_RED     "\033[1;31m"
#define COLOR_LIGHT_GREEN   "\033[1;32m"
#define COLOR_LIGHT_BLUE    "\033[1;34m"
#define COLOR_DARY_GRAY     "\033[1;30m"
#define COLOR_LIGHT_CYAN    "\033[1;36m"
#define COLOR_LIGHT_PURPLE  "\033[1;35m"
#define COLOR_YELLOW        "\033[1;33m"
#define COLOR_WHITE         "\033[1;37m"

/**
 * Assert function.
 */
extern uint8_t bt_asserted;
#ifdef BT_DEBUG
#define BT_ASSERT(expr) {if(!(expr)){bt_debug_log("[E]", __func__, __LINE__, "BT SW ASSERT!");bt_asserted=1;while(1){bt_os_layer_sleep_task(1);};}}
#define BT_FW_ASSERT(expr) {if(!(expr)){bt_debug_log("[E]", __func__, __LINE__, "BT FW ASSERT!");bt_asserted=1;}}
#else
#define BT_ASSERT(expr)
#define BT_FW_ASSERT(expr)
#endif
/**
 * Log function. Example usage: BT_LOGD("HCI", "test%d", 123);
 */
#ifdef BT_DEBUG
void bt_debug_log(const char* module, const char *function_name, const uint16_t line_num, const char *format, ...);

#define BT_LOGI(module_name, format, ...)   \
    bt_debug_log("[I][" module_name"]", __func__, __LINE__, format, ## __VA_ARGS__)

#define BT_LOGD(module_name, format, ...)   \
    bt_debug_log("[D][" module_name"]", __func__, __LINE__, format, ## __VA_ARGS__)

#define BT_LOGW(module_name, format, ...)   \
    bt_debug_log("[W][" module_name"]", __func__, __LINE__, format, ## __VA_ARGS__)

#define BT_LOGE(module_name, format, ...)   \
    bt_debug_log("[E][" module_name"]", __func__, __LINE__, format, ## __VA_ARGS__)

#define BT_LOGD_16B(module, str, p) \
    {BT_LOGD(module,str"%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", \
               (p)[0], (p)[1], (p)[2], (p)[3], (p)[4], (p)[5], (p)[6], (p)[7], \
               (p)[8], (p)[9], (p)[10], (p)[11], (p)[12], (p)[13], (p)[14], (p)[15]);}
#define BT_LOGD_8B(module, str, p) \
    {BT_LOGD(module,str"%02X %02X %02X %02X %02X %02X %02X %02X", \
               (p)[0], (p)[1], (p)[2], (p)[3], (p)[4], (p)[5], (p)[6], (p)[7]);}

/**
 * Define BT log with msg id.
 */
#define BT_LOG_MSGID_I(module_name, msg, arg_cnt, ...)   \
    bt_debug_log("[I][" module_name"]", __func__, __LINE__, msg, ## __VA_ARGS__)

#define BT_LOG_MSGID_D(module_name, msg, arg_cnt, ...)   \
    bt_debug_log("[D][" module_name"]", __func__, __LINE__, msg, ## __VA_ARGS__)

#define BT_LOG_MSGID_W(module_name, msg, arg_cnt, ...)   \
    bt_debug_log("[W][" module_name"]", __func__, __LINE__, msg, ## __VA_ARGS__)

#define BT_LOG_MSGID_E(module_name, msg, arg_cnt, ...)   \
    bt_debug_log("[E][" module_name"]", __func__, __LINE__, msg, ## __VA_ARGS__)

#else
#define BT_LOGI(...)
#define BT_LOGD(...)
#define BT_LOGW(...)
#define BT_LOGE(...)
#define BT_LOGD_16B(...)
#define BT_LOGD_8B(...)
#define BT_LOG_MSGID_I(...)
#define BT_LOG_MSGID_D(...)
#define BT_LOG_MSGID_W(...)
#define BT_LOG_MSGID_E(...)
#endif


/**
 * Data print function.
 */
#ifdef BT_DEBUG
#define BT_PRINT_16_BYTE_BIG_ENDIAN(p) {BT_LOGI("HCI", "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x", \
        (p)[0], (p)[1], (p)[2], (p)[3], (p)[4], (p)[5], (p)[6], (p)[7], (p)[8], (p)[9], (p)[10], (p)[11], (p)[12], (p)[13], (p)[14], (p)[15]);}
#define BT_PRINT_8_BYTE_BIG_ENDIAN(p) {BT_LOGI("HCI", "%02x %02x %02x %02x %02x %02x %02x %02x", \
        (p)[0], (p)[1], (p)[2], (p)[3], (p)[4], (p)[5], (p)[6], (p)[7]);}
#else
#define BT_PRINT_16_BYTE_BIG_ENDIAN(p)
#define BT_PRINT_8_BYTE_BIG_ENDIAN(p)
#endif
/**
 * For address debug printing.
 */
#ifdef BT_DEBUG
/**
 * @brief     Convert address to string.
 * @param[in] addr is address using bt_bd_addr_t.
 * @return    An addrss string with format: 11-22-33-44-55-66.
 */
const char *bt_debug_bd_addr2str(const bt_bd_addr_t addr);

/**
 * @brief     Convert address to string.
 * @param[in] addr is address using bt_bd_addr_t.
 * @return    An addrss string with format: LAP: 11-22-33, UAP: 44, NAP: 55-66.
 */
const char *bt_debug_bd_addr2str2(const bt_bd_addr_t addr);

/**
 * @brief     Convert address to string.
 * @param[in] addr is address using bt_bd_addr_t.
 * @return    An addrss string with format: 11:22:33:44:55:66.
 */
const char *bt_debug_bd_addr2str3(const bt_bd_addr_t addr);

/**
 * @brief     Convert address to string.
 * @param[in] addr is address using bt_bd_addr_t.
 * @return    An addrss string with format: [PUBLIC]11-22-33-44-55-66.
 */
const char *bt_debug_addr2str(const bt_addr_t *p);

/**
 * @brief     Convert address to string.
 * @param[in] addr is address using bt_bd_addr_t.
 * @return    An addrss string with format: [PUBLIC]LAP: 11-22-33, UAP: 44, NAP: 55-66.
 */
const char *bt_debug_addr2str2(const bt_addr_t *p);

/**
 * @brief     Convert string to address
 * @param[in] string which contains address with format : 11:22:33:44:55:66
 * @param[out] uint8_t[6] array.
 * @return    void
 */
void bt_debug_str2addr(const uint8_t *addr_from, uint8_t *addr_to);

/**
 * @brief     Reason code to string
 * @param[in] reason code
 * @return    reason string
 */
const char * bt_debug_reason2str(const uint8_t reason);
#endif

BT_EXTERN_C_END

#endif /* __BT_DEBUG_H__ */
