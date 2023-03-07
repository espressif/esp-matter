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

#ifndef __BT_PLATFORM_H__
#define __BT_PLATFORM_H__

/**
 * This header file describes the task apis or packed define used by user or SDK header file.
 */

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
#include <string.h>
BT_EXTERN_C_BEGIN

/**
 * @brief The type of bt firmware when supported multiple type in the firmware.
 */
#define BT_FIRMWARE_TYPE_HAEDSET 0x00    /**< The type of heaset firmware. */
#define BT_FIRMWARE_TYPE_SPEAKER 0x01    /**< The type of speaker firmware. */
#define BT_FIRMWARE_TYPE_EARBUDS 0x02    /**< The type of earbuds firmware. */
#define BT_FIRMWARE_TYPE_RELAY   0x03    /**< The type of relay firmware. */
#define BT_FIRMWARE_TYPE_MAX     0x04    /**< The count of the type. */
#define BT_FIRMWARE_TYPE_INVALID 0xFF    /**< Invalid type. */
typedef uint8_t bt_firmware_type_t;


/**
 * @brief BT_PACKED is used in structure define to make the structure more compact.The define is different as per compiler.
 */
#if _MSC_VER >= 1500
#define BT_PACKED(...) __pragma(pack(push, 1)) __VA_ARGS__ __pragma(pack(pop))
#elif defined(__GNUC__)
#define BT_PACKED(...) __VA_ARGS__ __attribute__((__packed__))
#define BT_ALIGNMENT4(...) __VA_ARGS__ __attribute__((aligned(4)))
#elif defined(__ARMCC_VERSION)
#pragma anon_unions
#define BT_PACKED(...) __VA_ARGS__ __attribute__((__packed__))
#define BT_ALIGNMENT4(...) __VA_ARGS__ __attribute__((aligned(4)))
#elif defined(__ICCARM__)
#define BT_PACKED(...) __packed __VA_ARGS__
#define BT_ALIGNMENT4(...) _Pragma("data_alignment=4") __VA_ARGS__
#else
#error "Unsupported Platform"
#endif

/*
 * @brief                  It is a user define function being invoked if having new bt interrupt.
 * @param[in] is_from_isr  is the current contex isr or not.
 * @return                 None.
 * @par                    Example
 * @code
 *       static uint32_t bt_task_semaphore = 0;
 *       void bt_trigger_interrupt(uint32_t is_from_isr)
 *       {
 *           if(bt_task_semaphore == 0) {
 *               return;
 *           }
 *           if(is_from_isr != 0) {
 *               bt_os_layer_give_semaphore_from_isr(bt_task_semaphore);
 *           }
 *           else {
 *               bt_os_layer_give_semaphore(bt_task_semaphore);
 *           }
 *       }
 *
 *      void bt_task(void * arg)
 *      {
 *          // Initialyze samaphore.
 *          bt_task_semaphore = bt_os_layer_create_semaphore();
 *          BT_ASSERT(bt_task_semaphore);
 *
 *          // Power on bt.
 *          bt_power_on(NULL, NULL);
 *
 *          // Main loop.
 *          do {
 *              bt_os_layer_take_semaphore(bt_task_semaphore);
 *
 *              // Handle interrupt.
 *              if (BT_STATUS_SUCCESS != bt_handle_interrupt()) {
 *                  break;
 *              }
 *          } while (1);
 *      }
 * @endcode
 */
void bt_trigger_interrupt(uint32_t is_from_isr);

/**
 * @brief                This function handle the bt interrupt.
 * @return               #BT_STATUS_SUCCESS means the operation was successful, otherwise failed.
 */
int32_t bt_handle_interrupt(void);

/**
 * @brief This function set the type of bt firmware before called #bt_power_on().
 * @param[in] type           is the type to set.
 * @return                          0, the type set successfully.
 *                                      1, the type set failed.
 */
uint8_t bt_firmware_type_set(bt_firmware_type_t type);

/**
 * @brief This function get the type of bt firmware after called #bt_power_on().
 * @return    the type of bt firmware.
 */
bt_firmware_type_t bt_firmware_type_get(void);


BT_EXTERN_C_END

#endif
