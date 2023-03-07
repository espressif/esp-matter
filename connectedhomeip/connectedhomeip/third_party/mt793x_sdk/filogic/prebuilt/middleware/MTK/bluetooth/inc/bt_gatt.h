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

#ifndef __BT_GATT_SPEC_H__
#define __BT_GATT_SPEC_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothGATT GATT
 * @{
 * This section introduces the GATT Client and Server and service framework using the attribute protocol to discover services and
 * read and write characteristic values from a peer device.
 * @addtogroup BluetoothBLE_ATT Define and structure
 * @{
 *
 */


#include "bt_uuid.h"
BT_EXTERN_C_BEGIN


/**
 * @addtogroup BluetoothBLE_ATT_GATT_define Define
 * @{
 */

/* UUID */
#define BT_GATT_UUID16_PRIMARY_SERVICE                              0x2800      /**< Primary service. */
#define BT_GATT_UUID16_SECONDARY_SERVICE                            0x2801      /**< Secondary service. */
#define BT_GATT_UUID16_INCLUDE                                      0x2802      /**< Include service. */
#define BT_GATT_UUID16_CHARC                               0x2803      /**< Characteristic. */
#define BT_GATT_UUID16_CHARC_EXTENDED_PROPERTIES           0x2900      /**< Characteristic extended properties. */
#define BT_GATT_UUID16_CHARC_USER_DESCRIPTION              0x2901      /**< Characteristic user description. */
#define BT_GATT_UUID16_CLIENT_CHARC_CONFIG                 0x2902      /**< Client characteristic configuration. */
#define BT_GATT_UUID16_SERVER_CHARC_CONFIG                 0x2903      /**< Server characteristic configuration. */
#define BT_GATT_UUID16_CHARC_FORMAT                        0x2904      /**< Characteristic format. */
#define BT_GATT_UUID16_CHARC_AGGREGATE_FORMAT              0x2905      /**< Characteristic aggregate format. */

#define BT_GATT_UUID16_GAP_SERVICE                                  0x1800      /**< GAP service. */
#define BT_GATT_UUID16_GATT_SERVICE                                 0x1801      /**< GATT service. */
#define BT_GATT_UUID16_HEART_RATE_SERVICE                           0x180D      /**< Heart rate service. */
#define BT_GATT_UUID16_BATTERY_SERVICE                              0x180F      /**< Battery service. */
#define BT_GATT_UUID16_ENVIRONMENTAL_SENSING                        0x181A      /**< Environmental sensing. */

#define BT_SIG_UUID16_DEVICE_NAME                                   0x2A00      /**< Device name. */
#define BT_SIG_UUID16_APPEARANCE                                    0x2A01      /**< Appearance. */
#define BT_SIG_UUID16_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS    0x2A04      /**< Peripheral preferred connection parameters. */
#define BT_SIG_UUID16_SERVICE_CHANGED                               0x2A05      /**< Service changed. */
#define BT_SIG_UUID16_BATTERY_LEVEL                                 0x2A19      /**< Battery level. */
#define BT_SIG_UUID16_SERIAL_NUMBER                                 0x2A25      /**< Serial number. */
#define BT_SIG_UUID16_MANUFACTURER_NAME                             0x2A29      /**< Manufacturer name. */
#define BT_SIG_UUID16_TEMPERATURE                                   0x2A6E      /**< Temperature. */
#define BT_SIG_UUID16_CENTRAL_ADDRESS_RESOLUTION                    0x2AA6      /**< Central address resolution. */

#define BT_SIG_UUID16_CLIENT_SUPPORTED_FEATURES                     0x2B29      /**< Client Supported Features. */
#define BT_SIG_UUID16_DATABASE_HASH                                 0x2B2A      /**< Database Hash. */
#define BT_SIG_UUID16_SERVER_SUPPORTED_FEATURES                     0x2B3A      /**< Server Supported Features. */

#define BT_GATT_CHARC_PROP_BROADCAST                            0x01       /**< Broadcast. */
#define BT_GATT_CHARC_PROP_READ                                 0x02       /**< Read. */
#define BT_GATT_CHARC_PROP_WRITE_WITHOUT_RSP                    0x04       /**< Write without response. */
#define BT_GATT_CHARC_PROP_WRITE                                0x08       /**< Write. */
#define BT_GATT_CHARC_PROP_NOTIFY                               0x10       /**< Notify. */
#define BT_GATT_CHARC_PROP_INDICATE                             0x20       /**< Indicate. */
#define BT_GATT_CHARC_PROP_AUTHENTICATED_SIGNED_WRITE           0x40       /**< Authenticated signed write. */
#define BT_GATT_CHARC_PROP_EXTENDED_PROPERTIES                  0x80       /**< Extended properties. */

#define BT_GATT_CHARC_EXTENDED_PROP_RELIABLE_WRITE              0x0001     /**< Reliable write. */
#define BT_GATT_CHARC_EXTENDED_PROP_WRITABLE_AUXILIARIES        0x0002     /**< Writable auxiliaries. */

/**
 * @}
 */

BT_EXTERN_C_END

/**
 * @}
 * @}
 * @}
 */

#endif /* __BT_GATT_SPEC_H__ */
