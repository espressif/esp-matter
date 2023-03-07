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

#ifndef __BT_UUID_H__
#define __BT_UUID_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothCommon Common
 * @{
 * @addtogroup BluetoothCommonUUID UUID
 * @{
 * This section introduces the UUID APIs including terms and acronyms, supported features, details on how to
 * use this module, UUID function groups, structures and functions.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                                   |
 * |------------------------------|------------------------------------------------------------------------------------------|
 * |\b UUID                       | Universally Unique Identifier. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Universally_unique_identifier">Wikipedia</a>. |
 *
 * @section bt_uuid_api_usage How to use this module
 *
 * - Call #bt_uuid_load() to convert from integer to #bt_uuid_t structure.
 *  - Sample code:
 *     @code
 *       bt_gattc_discover_charc_req_t req;
 *
 *       req.opcode = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST;
 *       req.starting_handle = (uint16_t)strtoul(starting_handle, NULL, 16);
 *       req.ending_handle = (uint16_t)strtoul(ending_handle, NULL, 16);
 *       uint16_t uuid_16 = BT_GATT_UUID16_CHARC;
 *       bt_uuid_load(&(req.type), (void *)&uuid_16, 2);
 *     @endcode
 *
 *
 */

#include "bt_platform.h"
#include "bt_type.h"

#if defined(__ARMCC_VERSION)
#pragma anon_unions
#endif

BT_EXTERN_C_BEGIN

/**
 * @defgroup Bluetoothbt_uuid_define Define
 * @{
 * This section defines the Bluetooth SIG base UUID.
 */

#define BT_UUID_INIT_WITH_UUID16(x)                                     \
   {{0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,                     \
    0x00, 0x10, 0x00, 0x00,                                             \
                           (uint8_t)x,                                  \
                                  (uint8_t)(x >> 8),                    \
                                        0x00, 0x00}}         /**< Bluetooth SIG base UUID. */

/**
 * @}
 */

/**
 * @defgroup Bluetoothbt_uuid_struct Struct
 * @{
 * This section defines #bt_uuid data structure.
 */

typedef union bt_uuid bt_uuid_t; /**< Please refer to #bt_uuid. */

/**
 * @brief bt_uuid union.
 */
BT_PACKED(union bt_uuid{
    uint8_t uuid[16];           /**< An array to store 128-bit UUID. */
    BT_PACKED(struct {
        uint32_t reserved1[3];  /**< Placeholder. */
        uint32_t uuid32;        /**< 32-bit UUID. */
    });
    BT_PACKED(struct {
        uint16_t reserved2[6]; /**< Placeholder. */
        uint16_t uuid16;       /**< 16-bit UUID. */
        uint16_t __zero16;     /**< Placeholder. */
    });                        /**< Union of bt_uuid. */
});

/**
 * @}
 */

/**
 * @brief                   This function copies the UUID from another UUID.
 * @param[in] dst           is the destination UUID.
 * @param[in] src           is the source UUID.
 * @return                  None.
 */
void bt_uuid_copy(bt_uuid_t *dst, const bt_uuid_t *src);

/**
 * @brief                   This function checks if two UUIDs are identical.
 * @param[in] uuid1         is the first UUID.
 * @param[in] uuid2         is the second UUID.
 * @return                  true, if two UUIDs are identical.
 */
bool bt_uuid_equal(const bt_uuid_t *uuid1, const bt_uuid_t *uuid2);

/**
 * @brief                   This function checks if it is a 32-bit UUID.
 * @param[in] uuid          is the UUID to check.
 * @return                  true, if it is a 32-bit UUID.
 */
bool bt_uuid_is_uuid32(const bt_uuid_t *uuid);

/**
 * @brief                   This function checks if it is a 16-bit UUID.
 * @param[in] uuid          is the UUID to check.
 * @return                  true, if it is a 16-bit UUID.
 */
bool bt_uuid_is_uuid16(const bt_uuid_t *uuid);

/**
 * @brief                   This function converts the UUID stored as a number in string format to #bt_uuid_t structure.
 * @param[in] dst           is the pointer to the converted UUID.
 * @param[in] src           is the pointer to the input integer in string format.
 * @param[in] src_size      is the size of the input integer in string format.
 * @return                  None
 */
void bt_uuid_load(bt_uuid_t *dst, const void *src, const uint8_t src_size);

/**
 * @brief                   This function converts the UUID in 16-bit integer format to #bt_uuid_t structure.
 * @param[in] dst           is the pointer to the converted UUID.
 * @param[in] uuid16        is the UUID in 16-bit integer format.
 * @return                  None
 */
void bt_uuid_from_uuid16(bt_uuid_t *dst, const uint16_t uuid16);

/**
 * @brief                   This function converts the UUID in 32-bit integer format to #bt_uuid_t structure.
 * @param[in] dst           is the pointer to the converted UUID.
 * @param[in] uuid32        is the UUID in 32-bit integer format.
 * @return                  None.
 */
void bt_uuid_from_uuid32(bt_uuid_t *dst, const uint32_t uuid32);

BT_EXTERN_C_END

/**
 * @}
 * @}
 * @}
 */

#endif /*__BT_UUID_H__*/
