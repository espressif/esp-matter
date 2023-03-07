/* Copyright Statement:
 *
 * (C) 2005-2020  MediaTek Inc. All rights reserved.
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

#ifndef __BT_PRIMARY_TYPE_H__
#define __BT_PRIMARY_TYPE_H__

#ifdef BT_PLATFORM_BLE_ONLY
/**
 * @addtogroup Bluetooth
 * @{
 * MediaTek LinkIt development platform for RTOS provides Bluetooth Low Energy (LE) connectivity support for IoT and Wearable's applications. Devices that support Bluetooth Low Energy are referred to as single-mode devices where the low power consumption is the primary concern for application development, such as those that run on coin cell batteries.
 */

#else /* BT_PLATFORM_BLE_ONLY */
/**
* @addtogroup Bluetooth
* @{
*  MediaTek LinkIt development platform for RTOS provides Bluetooth and Bluetooth Low Energy (LE) connectivity support for IoT and Wearable's applications. Bluetooth standard offers basic rate (BR) or enhanced data rate (EDR) and also Bluetooth Low Energy support. Devices that can support BR/EDR and Bluetooth Low Energy are referred to as dual-mode devices. Typically in a Bluetooth system, a mobile phone or laptop computer acts as a dual-mode device.
*/

#endif /* BT_PLATFORM_BLE_ONLY */
/**
 * @addtogroup BluetoothCommon Common
 * @{
 * This section introduces the following modules - the memory management, the power management, the host controller interface, the universally unique identifier. It also provides the terms and acronyms, definitions and structures.
 * @defgroup Bluetoothtype Define and structure
 * @{
 * This section defines the basic types.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b ATT                        | Attribute Protocol, to discover, read and write attributes on a peer device.|
 * |\b GAP                        | Generic Access Profile defines the generic procedures related to the discovery of Bluetooth devices and link management aspects of connecting to Bluetooth devices. It also defines procedures related to use of different security levels. |
 * |\b GATT                       | Generic Attribute Profile defines a service framework using the attribute Protocol for discovering services, and for reading and writing characteristic value on a remote device. |
 * |\b HCI                        | Host Controller Interface. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#HCI">Wikipedia</a>.|
 * |\b L2CAP                      | Logical Link Control And Adaptation Protocol Specification. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Logical_link_control_and_adaption_protocol_.28L2CAP.29">Wikipedia</a>. |
 * |\b SM                         | Security Manager. It defines the protocol and behavior to manage pairing, authentication and encryption between LE-only or BR/EDR/LE devices. |
 *
 */
#include <stdbool.h>
#include "bt_platform.h"

/**
 * @defgroup Bluetoothbt_primary_type_define Define
 * @{
 */

/**
 *  @brief      Bluetooth connection handle.
 */
#define BT_HANDLE_INVALID       0xFFFF  /**< Invalid connection handle. */
typedef uint16_t bt_handle_t; /**< Define the connection handle type. */

/**
 *  @brief      Bluetooth SM key size, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 3, part H] Section 2.3.4</a>.
 */
#define BT_KEY_SIZE  16
typedef uint8_t bt_key_t[BT_KEY_SIZE]; /**< Define the SM key type. */

/**
 *  @brief      Bluetooth GAP role, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 6, part B] Section 4.5</a>.
 */
#define BT_ROLE_MASTER                  0x00 /**< Master or Central. */
#define BT_ROLE_SLAVE                   0x01 /**< Slave or Peripheral. */
typedef uint8_t bt_role_t;  /**< Define the role type. */

/*
 * +---------+-----------------------------+
 * |Module ID  | Data                      |
 * +---------+-----------------------------+
 */
#define BT_MODULE_OFFSET                26          /**< Module range: 0x00 ~ 0x3F. The maximum number of modules: 64. */
#define BT_MODULE_MASK                  0x03FFFFFFU /**< Mask for Bluetooth module. */
#define BT_MODULE_CUSTOM_OFFSET         20          /**< Module range: 0x00 ~ 0x3F. The maximum number of modules: 64. */
#define BT_MODULE_CUSTOM_MASK           0x000FFFFFU /**< Mask for Bluetooth custom module. */

#define BT_MODULE_GENERAL               (0x00U<<BT_MODULE_OFFSET) /**< Prefix of the general module. 0x00000000*/
#define BT_MODULE_TIMER                 (0x01U<<BT_MODULE_OFFSET) /**< Prefix of the timer module. 0x04000000*/
#define BT_MODULE_MM                    (0x02U<<BT_MODULE_OFFSET) /**< Prefix of the Memory Management module. 0x08000000*/
#define BT_MODULE_HCI                   (0x03U<<BT_MODULE_OFFSET) /**< Prefix of the HCI module. 0x0C000000*/
#define BT_MODULE_GAP                   (0x04U<<BT_MODULE_OFFSET) /**< Prefix of the GAP module. 0x10000000*/
#define BT_MODULE_L2CAP                 (0x05U<<BT_MODULE_OFFSET) /**< Prefix of the L2CAP module. 0x14000000*/
#define BT_MODULE_SM                    (0x06U<<BT_MODULE_OFFSET) /**< Prefix of the SM module. 0x18000000*/
#define BT_MODULE_ATT                   (0x07U<<BT_MODULE_OFFSET) /**< Prefix of the ATT module. 0x1C000000*/
#define BT_MODULE_GATT                  (0x08U<<BT_MODULE_OFFSET) /**< Prefix of the GATT module. 0x20000000*/
#define BT_MODULE_SYSTEM                (0x09U<<BT_MODULE_OFFSET) /**< Prefix of the System module. 0x24000000*/
#define BT_MODULE_SDP                   (0x0AU<<BT_MODULE_OFFSET) /**< Prefix of the SDP module. 0x28000000*/
#define BT_MODULE_RFCOMM                (0x0BU<<BT_MODULE_OFFSET) /**< Prefix of the RFCOMM module. 0x2C000000*/
#define BT_MODULE_HFP                   (0x0CU<<BT_MODULE_OFFSET) /**< Prefix of the HFP module. 0x30000000*/
#define BT_MODULE_SPP                   (0x0DU<<BT_MODULE_OFFSET) /**< Prefix of the SPP module. 0x34000000*/
#define BT_MODULE_AVRCP                 (0x0EU<<BT_MODULE_OFFSET) /**< Prefix of the SPP module. 0x38000000*/
#define BT_MODULE_A2DP                  (0x0FU<<BT_MODULE_OFFSET) /**< Prefix of the A2DP module. 0x3C000000*/
#define BT_MODULE_GOEP                  (0x10U<<BT_MODULE_OFFSET) /**< Prefix of the GOEP module. 0x40000000*/
#define BT_MODULE_PBAPC                 (0x11U<<BT_MODULE_OFFSET) /**< Prefix of the PBAPC module. 0x44000000*/
#define BT_MODULE_AWS                   (0x12U<<BT_MODULE_OFFSET) /**< Prefix of the AWS module. 0x48000000*/
#define BT_MODULE_HID                   (0x13U<<BT_MODULE_OFFSET) /**< Prefix of the HID module. 0x4C000000*/
#define BT_MODULE_AWS_MCE               (0x14U<<BT_MODULE_OFFSET) /**< Prefix of the AWS-MCE module. 0x50000000*/
#define BT_MODULE_HSP                   (0x15U<<BT_MODULE_OFFSET) /**< Prefix of the HID module. 0x54000000*/
#define BT_MODULE_AVM                   (0x16U<<BT_MODULE_OFFSET) /**< Prefix of the AVM module. 0x58000000*/
#define BT_MODULE_L2CAP_ERTM            (0x17U<<BT_MODULE_OFFSET) /**< Prefix of the L2CAP ERTM module. 0x5C000000*/
#define BT_MODULE_AIRUPDATE             (0x18U<<BT_MODULE_OFFSET) /**< Prefix of the AIRUPDATE module. 0x60000000*/
#define BT_MODULE_DRIVER                (0x19U<<BT_MODULE_OFFSET) /**< Prefix of the BT Driver module. */
#define BT_MODULE_CTP                   (0x1AU<<BT_MODULE_OFFSET) /**< Prefix of the CTP module. */
#define BT_MODULE_MAP                   (0x1BU<<BT_MODULE_OFFSET) /**< Prefix of the MAP module. 0x6C000000*/
#define BT_MODULE_LE_AUDIO              (0x1CU<<BT_MODULE_OFFSET) /**< Prefix of the LE Audio module. 0x70000000 */
#define BT_MODULE_OTP                   (0x1DU<<BT_MODULE_OFFSET) /**< Prefix of the OTP module. 0x80000000 */
#define BT_MODULE_ULL                   (0x1EU<<BT_MODULE_OFFSET) /**< Prefix of the ULL module. 0x78000000 */
#define BT_MODULE_CUSTOM                (0x3EU<<BT_MODULE_OFFSET) /**< Prefix of the custom module. 0xF8000000*/
#define BT_MODULE_GENERAL_ERROR         (0x3FU<<BT_MODULE_OFFSET) /**< Prefix of the general error module. 0xFC000000*/

#define BT_MODULE_NUM                   BT_MODULE_GENERAL_ERROR   /**< The maximum prefix value of the module. */
typedef uint32_t bt_msg_type_t;  /**< Define the message type. */

/**
 *  @brief      Bluetooth status definitions. For the ATT or HCI error codes, please refer to \ref BluetoothGATT \ref BluetoothCommonHCI .
 */
#define BT_STATUS_SUCCESS               (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x00))        /**< General Success. */
#define BT_STATUS_PENDING               (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF9))        /**< General pending. */
#define BT_STATUS_BUSY                  (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFFa))        /**< General busy, please re-try the operation later. */

#define BT_STATUS_TIMER_FULL            (BT_MODULE_TIMER|(BT_MODULE_MASK&0x1))           /**< The timer is out-of-memory. */
#define BT_STATUS_TIMER_NOT_FOUND       (BT_MODULE_TIMER|(BT_MODULE_MASK&0x2))           /**< Timer not found in the timer manager. */

#define BT_STATUS_FAIL                  (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF1))  /**< General failed. */
#define BT_STATUS_OUT_OF_MEMORY         (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF2))  /**< General Out-of-memory. */
#define BT_STATUS_TIMEOUT               (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF3))  /**< General Timeout. */
#define BT_STATUS_BUFFER_USED           (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF4))  /**< General Buffer-used. */
#define BT_STATUS_UNSUPPORTED           (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF5))  /**< General Unsupported. */
#define BT_STATUS_CONNECTION_IN_USE     (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF6))  /**< General Connection-in-use. */
#define BT_STATUS_LINK_IS_DISCONNECTING (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF7))  /**< General Link-is-disconnecting. */
#define BT_STATUS_LINK_SECURITY_BLCOK   (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF8))  /**< General Link-security-block. */

#define BT_STATUS_HCI_FLOW_CONTROLLED   (BT_MODULE_HCI|0x01)            /**< The packet is flow-controlled. */
#define BT_STATUS_RX_PENDING            (BT_MODULE_HCI|0x02)            /**< Internal use of the HCI. */

#define BT_STATUS_DIRECTED_ADVERTISING_TIMEOUT    (BT_MODULE_GAP|0x03)  /**< Directed Advertising Timeout defined in the specification. */
#define BT_STATUS_CONNECTION_NOT_FOUND            (BT_MODULE_GAP|0x04)  /**< Unable to find the specified connection. */
#define BT_STATUS_CONNECTION_IN_SNIFF             (BT_MODULE_GAP|0x05)  /**< Action is not allowed when connection in sniff/subrating mode. */
#define BT_STATUS_CONNECTION_IS_MASTER            (BT_MODULE_GAP|0x06)  /**< Action is not allowed when local acts as a master in the connection. */
#define BT_STATUS_LE_CONNECTION_EXISTS            (BT_MODULE_GAP|0x07)  /**< Action is not allowed when LE connection exists. */
#define BT_STATUS_LE_ADVERTISING_ONGOING          (BT_MODULE_GAP|0x08)  /**< Action is not allowed when LE advertising is handling. */


#define BT_STATUS_L2CAP_REQUEST_TIMEOUT                (BT_MODULE_L2CAP|0x01)       /**< A L2CAP request time out. */
#define BT_STATUS_L2CAP_LOCAL_SECURITY_BLOCKED         (BT_MODULE_L2CAP|0x02)       /**< The local security manager refuses the connection attempt. */
#define BT_STATUS_L2CAP_LOCAL_PSM_UNSUPPORTED          (BT_MODULE_L2CAP|0x03)       /**< The local device does not support the protocol service (PSM). */
#define BT_STATUS_L2CAP_USER_REQUEST_DISCONNECT        (BT_MODULE_L2CAP|0x04)       /**< The local or remote device requests the disconnection. */
#define BT_STATUS_L2CAP_UNEXPECTED_SIGNAL_RECEIVED     (BT_MODULE_L2CAP|0x05)       /**< Received an unexpected signal from the remote device. */
#define BT_STATUS_L2CAP_LOCAL_REJECTED_NO_RESOURCES      (BT_MODULE_L2CAP|0x06)     /**< The local device is rejected due to the lack of resources. */
#define BT_STATUS_L2CAP_REMOTE_PSM_UNSUPPORTED         (BT_MODULE_L2CAP|0x42)       /**< The remote device does not support the requested protocol service (PSM). */
#define BT_STATUS_L2CAP_REMOTE_SECURITY_BLOCKED        (BT_MODULE_L2CAP|0x43)       /**< The remote device's security architecture denies the connection. */
#define BT_STATUS_L2CAP_REMOTE_NO_RESOURCES            (BT_MODULE_L2CAP|0x44)       /**< The remote device is out of resources. */
#define BT_STATUS_L2CAP_CONFIG_UNACCEPTED              (BT_MODULE_L2CAP|0x81)       /**< Configuration failed due to unacceptable parameters. */
#define BT_STATUS_L2CAP_CONFIG_REJECTED                (BT_MODULE_L2CAP|0x82)       /**< Configuration is rejected (unknown reason). */
#define BT_STATUS_L2CAP_CONFIG_OPTIONS_UNKNOWN         (BT_MODULE_L2CAP|0x83)       /**< Configuration failed due to an unrecognized configuration option. */
#define BT_STATUS_L2CAP_CHANNEL_EXISTED                (BT_MODULE_L2CAP|0xA0)       /**< User should return it to L2CAP if the L2CAP channel already exists when L2CAP requests the user to allocate channel.*/
#define BT_STATUS_L2CAP_CHANNEL_NOT_FOUND              (BT_MODULE_L2CAP|0xA1)       /**< The L2CAP channel is unknown*/
#define BT_STATUS_L2CAP_CHANNEL_OUT_OF_CREDIT          (BT_MODULE_L2CAP|0xA2)       /**< The L2CAP channel is short of credit*/
#define BT_STATUS_L2CAP_CHANNEL_MTU_EXCEEDED           (BT_MODULE_L2CAP|0xA3)       /**< The L2CAP channel MTU is exceeded by the PDU length*/
#define BT_STATUS_L2CAP_CHANNEL_K_FRAME_INVALID        (BT_MODULE_L2CAP|0xA4)       /**< The L2CAP channel received invalid k-frame*/
#define BT_STATUS_L2CAP_FIX_CID_IN_USE                 (BT_MODULE_L2CAP|0xB0)       /**< l2cap register fix channel id is use. */
#define BT_STATUS_L2CAP_FIX_CID_INVAILD                (BT_MODULE_L2CAP|0xB1)       /**< l2cap register fix channel id is invaild. */


typedef int32_t bt_status_t;    /**< Define the status type, returned from the APIs, and defined as BT_STATUS_XXX. */

#define BT_BD_ADDR_LEN      6   /**< Bluetooth Device Address Length defined in the specification. */

/**
 *  @brief      Address type, defined in Bluetooth core specification version 4.2.
 */
typedef uint8_t bt_bd_addr_t[BT_BD_ADDR_LEN];   /**< Bluetooth Device Address defined in the specification. */
typedef uint8_t const *bt_bd_addr_ptr_t;        /**< A pointer to the #bt_bd_addr_t. */

/**
 *  @brief      Address type, defined in Bluetooth core specification version 4.2.
 */
#define BT_ADDR_PUBLIC          0x00 /**< Public address type. */
#define BT_ADDR_RANDOM          0x01 /**< Random address type. */
#define BT_ADDR_PUBLIC_IDENTITY 0x02 /**< Public identity address type. */
#define BT_ADDR_RANDOM_IDENTITY 0x03 /**< Random identity address type. */
typedef uint8_t bt_addr_type_t;      /**< Define the address type. */

#define BT_NULL     0   /**< NULL */

/**
 *  @brief      Variable length data type.
 */
typedef uint8_t bt_data_t[1];

/**
 * @}
 */

/**
 * @defgroup Bluetoothbt_primary_type_struct Struct
 * @{
 */

/**
 *  @brief      This structure defines the Bluetooth address.
 */
BT_PACKED(
typedef struct {
    bt_addr_type_t  type;   /**< Address type. */
    bt_bd_addr_t    addr;   /**< Bluetooth device address. */
}) bt_addr_t;

/**
 * @}
 */

/**
* @}
* @}
* @}
*/

#endif /*__BT_PRIMARY_TYPE_H__*/

