/* Copyright Statement:
 *
 * (C) 2005-2016 MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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
#ifndef __BT_HID_H__
#define __BT_HID_H__

#include "bt_type.h"
#include "bt_platform.h"
/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothHID HID
 * @{
 * The Human Interface Device Profile (HID) can be used to emulate a HID connection using Logical Link Control and Adaptation Protocol (L2CAP) between two peer devices.
 * There are two roles (Device and Host) in the HID specification. Currently, it only supports Device role. The HID device is implemented for a device side connection (Acceptor). It can wait
 * for another device to take initiative to connect. The HID host is implemented for a host side connection (Initiator) to initiate a connection with another device.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b HID                       |Human Interface Device Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Human_device_protocol_.28SDP.29">Wikipedia</a>. |
 *
 * @section bt_hid_api_usage How to use this module
 * - HID connection in device mode, as shown in the figure titled as "HID connection establishment message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *  - Step 1. Mandatory, implement #bt_sdps_get_customized_record() to add SDP records of HID Device. The HID's SDP records contain attributes, including the service class ID list,
 *    the protocol description list, the language list, the profile_descriptor_list, the attribute list, the additional protocol descriptor list, the service name, the version number list, the parser version,
 *    the device subclass, the device country code, the virtual cable, the reconnection, the boot device, the descriptor, the language ID base list and the profile. In general, user needs to provide
 *    the service name and the descriptor, while the other attributes are constant. In the description list, user needs to fill the HID report descriptor, which represents a specific feature in HID device.
 *    Host will do the matched operation when it receives data as declared in SDP. In the service name, user can assign a name to the HID device.
 *    For more information about the SDP record, please refer to \ref SDP.
 * - Sample code:
 *    @code
 *      static const uint8_t bt_hid_service_class_id_pattern[] = {
 *          BT_HID_SDP_SERVICE_CLASS_ID
 *      };
 *      static const uint8_t bt_hid_protocol_descriptor_list_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(BT_HID_CONTROL_CHANNEL)
 *      };
 *      static const uint8_t bt_hid_language_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_LANGUAGE
 *      };
 *      static const uint8_t bt_hid_addition_protocol_descriptor_list_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_ADDTIONAL_PROTOCOL_DESCRIPTOR(BT_HID_INTERRUPT_CHANNEL)
 *      };
 *      static const uint8_t bt_hid_service_name_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME(4),
 *          'K', 'y', 'b', '\0'
 *      };
 *      static const uint8_t bt_hid_bt_profile_descriptor_list_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_BT_PROFILE_DESCRIPTOR
 *      };
 *      static const uint8_t bt_hid_parser_version_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_PARSERVERSION
 *      };
 *      static const uint8_t bt_hid_device_subclass_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_DEVICE_SUBCLASS
 *      };
 *      static const uint8_t bt_hid_device_country_code_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_COUNTRYCODE
 *      };
 *      static const uint8_t bt_hid_virtural_cable_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_VITRUALCABLE
 *      };
 *      static const uint8_t bt_hid_reconnection_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_RECONNECTION
 *      };
 *      static const uint8_t bt_hid_version_number_list_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_VERSION_NUMBER_LIST
 *      };
 *  #define HID_REPORT_DESCRIPTOR \
 *          0x05, 0x01, 0x09, 0x06,0xA1, \
 *          0x01, 0x15, 0x00, 0x25,0x01, \
*           0x0b, 0x66, 0x00, 0x07, 0x00, \
 *          0x0b, 0xe2, 0x00, 0x0c, 0x00, \
 *          0x0b, 0x50, 0x00, 0x07, 0x00, \
 *          0x0b, 0xb4, 0x00, 0x0c, 0x00, \
 *          0x0b, 0xe9, 0x00, 0x0c, 0x00, \
 *          0x0b, 0xf1, 0x00, 0x07, 0x00, \
 *          0x0b, 0x58, 0x00, 0x07, 0x00, \
 *          0x0b, 0xcd, 0x00, 0x0c, 0x00, \
 *          0x0b, 0x89, 0x00, 0x0c, 0x00, \
 *          0x0b, 0x52, 0x00, 0x07, 0x00, \
 *          0x0b, 0x4f, 0x00, 0x07, 0x00, \
 *          0x0b, 0xb3, 0x00, 0x0c, 0x00, \
 *          0x0b, 0xea, 0x00, 0x0c, 0x00, \
 *          0x0b, 0x23, 0x02, 0x0c, 0x00, \
 *          0x0b, 0x51, 0x00, 0x07, 0x00, \
 *          0x0b, 0x40, 0x00, 0x0c, 0x00, \
 *          0x0b, 0x21, 0x02, 0x0c, 0x00, \
 *          0x95, 0x11, 0x75, 0x01, 0x81, \
 *          0x02, 0x95, 0x07, 0x75, 0x01, \
 *          0x81, 0x03, 0x15, 0x00, 0x25, \
 *          0x64, 0x0b, 0x20, 0x00, 0x06, \
 *          0x00, 0x95, 0x01, 0x75, 0x08, \
 *          0x81, 0x02, 0xc0
 *
 *      static const uint8_t bt_hid_descriptor_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_HID_DESCRIPTOR_LIST(123),
 *          HID_REPORT_DESCRIPTOR
 *      };
 *
 *      static const uint8_t bt_hid_langid_base_list_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_LANGID_BASELIST
 *      };
 *
 *      static const uint8_t bt_hid_boot_device_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_BOOTDEVICE
 *      };
 *
 *      static const uint8_t bt_hid_attibute_list[] = {
 *          BT_HID_ATTRIBUTE_ID_LIST
 *      };
 *
 *      static const uint8_t bt_hid_profile_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_HID_VERSION
 *      };
 *
 *    static const bt_sdps_attribute_t bt_hid_sdp_attributes_pattern[] = {
 *          BT_HID_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(bt_hid_service_class_id_pattern),
 *          BT_HID_SDP_ATTRIBUTE_PROTOCOL_DESC_LIST(bt_hid_protocol_descriptor_list_pattern),
 *          BT_HID_SDP_ATTRIBUTE_LANGUAGE_BASE_LIST(bt_hid_language_pattern),
 *          BT_HID_SDP_ATTRIBUTE_BT_PROFILE_DESC_LIST(bt_hid_bt_profile_descriptor_list_pattern),
 *          BT_HID_SDP_ATTRIBUTE_ID_LIST(bt_hid_attibute_list),
 *          BT_HID_SDP_ATTRIBUTE_ADDITIONAL_PROTOCOL_DESC_LIST(bt_hid_addition_protocol_descriptor_list_pattern),
 *          BT_HID_SDP_ATTRIBUTE_SERVICE_NAME(bt_hid_service_name_pattern),
 *          BT_HID_SDP_ATTRIBUTE_VERSION_NUMBER(bt_hid_version_number_list_pattern),
 *          BT_HID_SDP_ATTRIBUTE_PARSER_VERSION(bt_hid_parser_version_pattern),
 *          BT_HID_SDP_ATTRIBUTE_DEVICE_SUB_CLASS(bt_hid_device_subclass_pattern),
 *          BT_HID_SDP_ATTRIBUTE_COUNTRY_CODE(bt_hid_device_country_code_pattern),
 *          BT_HID_SDP_ATTRIBUTE_VITURAL_CABLE(bt_hid_virtural_cable_pattern),
 *          BT_HID_SDP_ATTRIBUTE_RECONNECTION_INITIATE(bt_hid_reconnection_pattern),
 *          BT_HID_SDP_ATTRIBUTE_BOOT_DEVICE(bt_hid_boot_device_pattern),
 *          BT_HID_SDP_ATTRIBUTE_HID_DESCRIPTOR(bt_hid_descriptor_pattern),
 *          BT_HID_SDP_ATTRIBUTE_LANGID_BASE_LIST(bt_hid_langid_base_list_pattern),
 *          BT_HID_SDP_ATTRIBUTE_HID_PROFILE(bt_hid_profile_pattern),
 *      };
 *      static const bt_sdps_attribute_t bt_spp_sdp_attributes_pattern[] = {
 *          BT_SPP_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(bt_spp_service_class_id_pattern),
 *          BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESC_LIST(bt_spp_protocol_descriptor_list_pattern),
 *          BT_SPP_SDP_ATTRIBUTE_BROWSE_GROUP_LIST(bt_spp_browse_group_pattern),
 *          BT_SPP_SDP_ATTRIBUTE_LANGUAGE_BASE_LIST(bt_spp_language_pattern),
 *          BT_SPP_SDP_ATTRIBUTE_SERVICE_NAME(bt_spp_service_name_pattern)
 *      };
 *      const bt_sdps_record_t bt_hid_sdp_record_1 = {
 *         .attribute_list_length = sizeof(bt_hid_sdp_attributes_pattern),
 *         .attribute_list = bt_hid_sdp_attributes_pattern,
 *      };
 *      static const bt_sdps_record_t *sdps_spp_record[]= {
 *          &bt_spp_sdp_record,
 *          &bt_di_sdp_record,
 *          &fota_spp_sdp_record,
 *          &bt_hid_sdp_record_1
 *      };
 *     @endcode
 *  - Step 2. If a remote HID host initiates a connection to the HID Device, the #BT_HID_CONNECT_IND event notifies the function #bt_app_event_callback().
 *  - Step 3. Call the function #bt_hid_connect_response() to accept or reject this connection request from a remote HID host.
 *  - Step 4. The #BT_HID_CONNECT_CNF event notifies the function #bt_app_event_callback() if the device is connected successfully or not.
 *   - Sample code:
 *     @code
 *       bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *       {
 *           switch (msg) {
 *               case BT_HID_CONNECT_IND:
 *                    bt_hid_connect_ind_t  *ind = (bt_hid_connect_ind_t *)buff;
 *                    bt_hid_connect_response(ind->handle, true);
 *                    break;
 *               case BT_HID_CONNECT_CNF:
 *                    bt_hid_connect_cnf_t *cnf = (bt_hid_connect_cnf_t *)buff;
 *                    break;
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 * - Initiate HID connection in device mode, as shown in the figure titled as "HID initiate connection message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *  - Step 1. Call the function #bt_hid_connect() to initiate a connection to a remote host.
 *   - Sample code:
 *    @code
 *      bt_hid_connect(&(hid_handle), &remote_addr);
 *    @endcode
 *  - Step 2. The #BT_HID_CONNECT_CNF event notifies the function #bt_app_event_callback() if the device is connected successfully or not.
 * -  Send data, as shown in the figure titled as "HID data transfer message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *  - Step 1. Call the function #bt_hid_send_data() to send application data to the remote device.
 *           If returned value is #BT_STATUS_PENDING, the application must wait for the #BT_SPP_READY_TO_SEND_IND event before the next send operation.
 *   - Sample code:
 *    @code
 *        bt_hid_data_t value;
 *        bt_status_t result = bt_hid_send_data(hid_handle, &value);
 *    @endcode
 *    @code
 * -  Disconnect the HID connection, as shown in the figure titled as "HID connection release message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *  - Step 1. Call the function #bt_spp_disconnect() to disconnect the SPP connection from a remote device.
 *   - Sample code:
 *    @code
 *       bt_status_t result = bt_hid_disconnect(hid_handle);
 *    @endcode
 *  - Step 2. The #BT_HID_DISCONNECT_IND event will notify the function #bt_app_event_callback() to indicate the HID connection is released.
 *   - Sample code:
 *    @code
 *       bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *       {
 *           switch (msg) {
 *               case BT_HID_DISCONNECT_IND: {
 *                  bt_hid_disconnect_ind_t* disc_ind_p = (bt_hid_disconnect_ind_t*)buff;
 *      }
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 */

/**
 * @defgroup Bluetoothbt_hid_define Define
 * @{
 * This section defines the macros for the HID.
 */

#ifdef __MTK_COMMON__
#define BT_SDP_ATTRIBUTE_ID_HID_DEVICE_SUB_CLASS                0x0202
#define BT_SDP_ATTRIBUTE_ID_HID_COUNTRY_CODE                    0x0203
#define BT_SDP_ATTRIBUTE_ID_HID_VIRTUAL_CABLE                   0x0204
#define BT_SDP_ATTRIBUTE_ID_HID_RECONNECTION_INIITALIZATION     0x0205
#define BT_SDP_ATTRIBUTE_ID_HID_DESCRIPTOR                      0x0206
#define BT_SDP_ATTRIBUTE_ID_HID_LANGID_BASE_LIST                0x0207
#define BT_SDP_ATTRIBUTE_ID_HID_SDP_DISABLE                     0x0208
#define BT_SDP_ATTRIBUTE_ID_HID_BATTERY_POWER                   0x0209
#define BT_SDP_ATTRIBUTE_ID_HID_REMOTE_WAKEUP                   0x020A
#define BT_SDP_ATTRIBUTE_ID_HID_PROFILE                         0x020B
#define BT_SDP_ATTRIBUTE_ID_HID_SUPERVISION_TIMEOUT             0x020C
#define BT_SDP_ATTRIBUTE_ID_HID_NORMAL_CONNECTABLE              0x020D
#define BT_SDP_ATTRIBUTE_ID_HID_BOOT_DEVICE                     0x020E
#endif

/*
*
 * @brief  UUID of the HID profile.
 */
#define BT_HID_DEVICE_UUID    0x1124

/**
 * @brief  Channel ID of the HID control.
 */
#define BT_HID_CONTROL_CHANNEL 0x0011

/**
 * @brief  Channel ID of the HID interrupt.
 */
#define BT_HID_INTERRUPT_CHANNEL 0x0013

/**
 * @brief  The type of Descriptor is Report.
 */
#define BT_HID_DESCRIPTOR_TYPE 0x22

/**
 * @brief  The maximum length of report data.
 */
#define BT_HID_MAX_REPORT_DATA_LENGTH 46

/**
 * @brief Attribute value of the HID service class ID.
 */
#define BT_HID_SDP_SERVICE_CLASS_ID  \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(3), \
    BT_SDP_UUID_16BIT(BT_HID_DEVICE_UUID) \

/**
 * @brief HID protocol.
 */
#define BT_SDP_PROTOCOL_HID 0x0011

/**
 * @brief Attribute value of the HID protocol descriptor list.
 * @param[in] psm_id  is a uint8 L2CAP channel number, the value is 0x11.
 */
#define BT_HID_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(psm_id)   \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(13),     \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(6),    \
    BT_SDP_UUID_16BIT(BT_SDP_PROTOCOL_L2CAP),     \
    BT_SDP_UINT_16BIT(psm_id),  \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(3),           \
    BT_SDP_UUID_16BIT(BT_SDP_PROTOCOL_HID),     \

/**
 * @brief Attribute value of the HID additional protocol descriptor list.
 * @param[in] psm_id  is a uint8 L2CAP channel number, the value is 0x13.
 */
#define BT_HID_SDP_ATTRIBUTE_ADDTIONAL_PROTOCOL_DESCRIPTOR(psm_id) \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(15), \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(13),     \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(6),    \
    BT_SDP_UUID_16BIT(BT_SDP_PROTOCOL_L2CAP),     \
    BT_SDP_UINT_16BIT(psm_id),  \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(3),           \
    BT_SDP_UUID_16BIT(BT_SDP_PROTOCOL_HID),     \

/**
 * @brief The HID language base.
 */
#define BT_HID_SDP_ATTRIBUTE_LANGUAGE     \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(9),     \
    BT_SDP_UINT_16BIT(0x656E),          \
    BT_SDP_UINT_16BIT(0x006A),          \
    BT_SDP_UINT_16BIT(0x0100)

/**
 * @brief The HID Bluetooth profile descriptor.
 */
#define BT_HID_SDP_ATTRIBUTE_BT_PROFILE_DESCRIPTOR     \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(8), \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(6),     \
    BT_SDP_UUID_16BIT(BT_HID_DEVICE_UUID),    \
    BT_SDP_UINT_16BIT(0x0100)

/**
 * @brief The HID language base ID list attribute.
 */
#define BT_HID_ATTRIBUTE_ID_LIST \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(3),\
    BT_SDP_UINT_16BIT(0x656E)

/**
 * @brief Attribute value of the HID version number.
 */
#define BT_HID_SDP_ATTRIBUTE_HID_VERSION   \
    BT_SDP_UINT_16BIT(0x0100)

/**
 * @brief Attribute value of the HID parser version number.
 */
#define BT_HID_SDP_ATTRIBUTE_PARSERVERSION    \
    BT_SDP_UINT_16BIT(0x0111)

/**
 * @brief Attribute value of the HID device subclass.
 */
#define BT_HID_SDP_ATTRIBUTE_DEVICE_SUBCLASS \
    BT_SDP_UINT_8BIT(0x80)

/**
 * @brief Attribute value of the HID country code.
 */
#define BT_HID_SDP_ATTRIBUTE_COUNTRYCODE \
    BT_SDP_UINT_8BIT(0x00)

/**
 * @brief Attribute value of the HID virtual cable.
 */
#define BT_HID_SDP_ATTRIBUTE_VITRUALCABLE \
    BT_SDP_BOOL(true)

/**
 * @brief Attribute value of the HID normal connectable.
 */
#define BT_HID_SDP_ATTRIBUTE_NORMALCONNECTABLE \
    BT_SDP_BOOL(false)

/**
 * @brief Attribute value of the HID boot device.
 */
#define BT_HID_SDP_ATTRIBUTE_BOOTDEVICE \
    BT_SDP_BOOL(false)

/**
 * @brief Attribute value of the HID reconnection.
 */
#define BT_HID_SDP_ATTRIBUTE_RECONNECTION \
    BT_SDP_BOOL(true)

/**
 * @brief Attribute value of the HID version list.
 */
#define BT_HID_SDP_ATTRIBUTE_VERSION_NUMBER_LIST \
    BT_SDP_UINT_16BIT(0x0100)

/**
 * @brief Attribute value of the HID language ID base list.
 */
#define BT_HID_SDP_ATTRIBUTE_LANGID_BASE_ID_LIST \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(8), \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(6), \
    BT_SDP_UINT_16BIT(0x0409), \
    BT_SDP_UINT_16BIT(0x0100)

/**
 * @brief Attribute value of the HID descriptor list.
 * @param[in] size  is the size of the HID descriptor list.
 */
#define BT_HID_SDP_ATTRIBUTE_HID_DESCRIPTOR_LIST(size) \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(size + 6), \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(size + 4), \
    BT_SDP_UINT_8BIT(BT_HID_DESCRIPTOR_TYPE), \
    BT_SDP_TEXT_8BIT(size)

#ifdef __MTK_COMMON__
/**
 * @brief Attribute value of the Supervision Timeout
 */
#define BT_HID_SDP_ATTRIBUTE_SUPERVISION_TIMEOUT   \
    BT_SDP_UINT_16BIT(0x0c80)

/**
 * @brief Attribute value of the HID descriptor list for descriptor size > 255
 * @param[in] size  is the size of the HID descriptor list.
 */
#define BT_HID_SDP_ATTRIBUTE_HID_LONG_DESCRIPTOR_LIST(size) \
    BT_SDP_ATTRIBUTE_HEADER_16BIT(size + 8), \
    BT_SDP_ATTRIBUTE_HEADER_16BIT(size + 5), \
    BT_SDP_UINT_8BIT(BT_HID_DESCRIPTOR_TYPE), \
    BT_SDP_TEXT_16BIT(size)

/**
 * @brief The HID service record state list attribute.
 * @param[in] uuid_array  is an array to hold the #BT_HID_SDP_ATTRIBUTE_RECORD_STATE.
 */
#define BT_HID_SDP_ATTRIBUTE_SERVICE_RECORD_STATE_LIST(record_state)  \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_SERVICE_RECORD_STATE, record_state)
#endif /* #ifdef __MTK_COMMON__ */

/**
 * @brief The HID service name's size.
 * @param[in] size  is the length of the HID service name.
 */
#define BT_HID_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME(size)     \
    BT_SDP_TEXT_8BIT(size)

/**
 * @brief The HID service class ID list attribute.
 * @param[in] uuid_array  is an array to hold the #BT_HID_SDP_SERVICE_CLASS_ID.
 */
#define BT_HID_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(uuid_array)  \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST, uuid_array)

/**
 * @brief The HID protocol descriptor list attribute.
 * @param[in] protocol_descriptor_array  is an array to hold the #BT_HID_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR.
 */
#define BT_HID_SDP_ATTRIBUTE_PROTOCOL_DESC_LIST(protocol_descriptor_array)  \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_PROTOCOL_DESC_LIST, protocol_descriptor_array)

/**
 * @brief The HID language base ID list attribute.
 * @param[in] language_array  is an array to hold the #BT_HID_SDP_ATTRIBUTE_LANGUAGE.
*/
#define BT_HID_SDP_ATTRIBUTE_LANGUAGE_BASE_LIST(language_array)  \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_LANG_BASE_ID_LIST, language_array)

/**
 * @brief The HID attribute ID list attribute.
 * @param[in] id_list  is an array to hold the #BT_HID_ATTRIBUTE_ID_LIST.
*/
#define BT_HID_SDP_ATTRIBUTE_ID_LIST(id_list) \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_LANG_BASE_ID_LIST, id_list)

/**
 * @brief The HID protocol descriptor list attribute.
 * @param[in] additional_protocol_descriptor_array is an array to hold the #BT_HID_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR.
 */
#define BT_HID_SDP_ATTRIBUTE_ADDITIONAL_PROTOCOL_DESC_LIST(additional_protocol_descriptor_array)  \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_ADDITIONAL_PROT_DESC_LISTS, additional_protocol_descriptor_array)

/**
 * @brief The HID Bluetooth profile descriptor list.
 * @param[in] profile_descriptor_array  is an array to hold the #BT_HID_SDP_ATTRIBUTE_BT_PROFILE_DESCRIPTOR.
 */
#define BT_HID_SDP_ATTRIBUTE_BT_PROFILE_DESC_LIST(profile_descriptor_array)  \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_BT_PROFILE_DESC_LIST, profile_descriptor_array)

/**
 * @brief The HID service name attribute.
 * @param[in] name_array  is an array including the size of name (#BT_HID_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME)
 *      and its text or string name.
 */
#define BT_HID_SDP_ATTRIBUTE_SERVICE_NAME(name_array)  \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_SERVICE_NAME + 0x0100, name_array)

/**
 * @brief The HID parser version attribute.
 * @param[in] parser_version  is an array to hold the #BT_HID_SDP_ATTRIBUTE_PARSERVERSION.
 */
#define BT_HID_SDP_ATTRIBUTE_PARSER_VERSION(parser_version) \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_SERVICE_DATABASE_STATE, parser_version)

/**
 * @brief The HID version attribute.
 * @param[in] version_number  is an array to hold the #BT_HID_SDP_ATTRIBUTE_VERSION_NUMBER_LIST.
 */
#define BT_HID_SDP_ATTRIBUTE_VERSION_NUMBER(version_number) \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_VERSION_NUMBER_LIST, version_number)

/**
 * @brief The HID subclass attribute.
 * @param[in] subclass  is an array to hold the #BT_HID_SDP_ATTRIBUTE_DEVICE_SUBCLASS.
 */
#define BT_HID_SDP_ATTRIBUTE_DEVICE_SUB_CLASS(subclass) \
    BT_SDP_ATTRIBUTE(0x0202, subclass)

/**
 * @brief The HID country code attribute.
 * @param[in] country_code  is an array to hold the #BT_HID_SDP_ATTRIBUTE_COUNTRYCODE.
 */
#define BT_HID_SDP_ATTRIBUTE_COUNTRY_CODE(country_code) \
    BT_SDP_ATTRIBUTE(0x0203, country_code)

/**
 * @brief The HID virtual cable attribute.
 * @param[in] virtual_cable  is an array to hold the #BT_HID_SDP_ATTRIBUTE_VITRUALCABLE.
 */
#define BT_HID_SDP_ATTRIBUTE_VIRTUAL_CABLE(virtual_cable) \
    BT_SDP_ATTRIBUTE(0x0204, virtual_cable)

/**
 * @brief The HID reconnection attribute.
 * @param[in] reconnection  is an array to hold the #BT_HID_SDP_ATTRIBUTE_RECONNECTION.
 */
#define BT_HID_SDP_ATTRIBUTE_RECONNECTION_INIITALIZATION(reconnection) \
    BT_SDP_ATTRIBUTE(0x0205, reconnection)

/**
 * @brief The HID description.
 * @param[in] descriptor  is an array to hold the #BT_HID_SDP_ATTRIBUTE_DESCRIPTOR_LIST.
 */
#define BT_HID_SDP_ATTRIBUTE_HID_DESCRIPTOR(descriptor) \
    BT_SDP_ATTRIBUTE(0x0206, descriptor)

/**
 * @brief The HID language base list attribute.
 * @param[in] base is an array to hold the #BT_HID_SDP_ATTRIBUTE_LANGID_BASE_LIST.
 */
#define BT_HID_SDP_ATTRIBUTE_LANGID_BASE_LIST(base) \
    BT_SDP_ATTRIBUTE(0x0207, base)

/**
 * @brief The HID boot device attribute.
 * @param[in] boot_device is an array to hold the #BT_HID_SDP_ATTRIBUTE_BOOTDEVICE.
 */
#define BT_HID_SDP_ATTRIBUTE_BOOT_DEVICE(boot_device) \
    BT_SDP_ATTRIBUTE(0x020E, boot_device)
/**
 * @brief The HID profile version attribute.
 * @param[in] profile_version is an array to hold the #BT_HID_SDP_ATTRIBUTE_VERSION_NUMBER_LIST.
 */
#define BT_HID_SDP_ATTRIBUTE_HID_PROFILE(profile_version) \
    BT_SDP_ATTRIBUTE(0x020B, profile_version)

#ifdef __MTK_COMMON__
/**
 * @brief The HID supervision timeout attribute.
 * @param[in] supervision_timeout is an array to hold the #BT_HID_SDP_ATTRIBUTE_SUPERVISION_TIMEOUT_LIST.
 */
#define BT_HID_SDP_ATTRIBUTE_HID_SUPERVISION_TIMEOUT(supervision_timeout) \
    BT_SDP_ATTRIBUTE(0x020C, supervision_timeout)

/**
 * @brief The HID profile normal connectable attribute.
 * @param[in] normal_connectable is an array to hold the #BT_HID_SDP_ATTRIBUTE_VERSION_NUMBER_LIST.
 */
#define BT_HID_SDP_ATTRIBUTE_HID_NORMAL_CONNECTABLE(normal_connectable) \
    BT_SDP_ATTRIBUTE(0x020D, normal_connectable)
#endif

/**
 * @brief The HID profile normal connectable attribute.
 * @param[in] normal_connectable is an array to hold the #BT_HID_SDP_ATTRIBUTE_VERSION_NUMBER_LIST.
 */
#define BT_HID_SDP_ATTRIBUTE_HID_NORMAL_CONENCTABLE(normal_connectable) \
    BT_SDP_ATTRIBUTE(0x020D, normal_connectable)

/**
 * @brief The HID message type.
 */
typedef uint8_t  bt_hid_message_type_t;
#define BT_HID_HANDSHAKE    0x00   /**< It can be sent only by the Device as an acknowledgement to the Host.*/
#define BT_HID_CONTROL      0x01   /**< It can be sent by the Device and Host as a request for a major state change in the Device.*/
#define BT_HID_GET_REPORT   0x04   /**< It can be sent by the Host to get an HID report from the Device.*/
#define BT_HID_SET_REPORT   0x05   /**< It can be sent by the Host to initiate the report.*/
#define BT_HID_GET_PROTOCOL 0x06   /**< It can be sent by the Host to get the current mode of the Device.*/
#define BT_HID_SET_PROTOCOL 0x07   /**< It can be sent by the Host to set the Device's mode.*/
#define BT_HID_DATA         0x0a   /**< It can be sent by the Host to transfer data.*/
#define BT_HID_SET_IDLE     0x09   /**< It can be sent by the Device and Host. Deprecated in HID version 1.1 .*/
#define BT_HID_GET_IDLE     0x08   /**< It can be sent by the Device and Host. Deprecated in HID version 1.1 .*/

/**
 * @brief The HID Report message.
 */
typedef uint8_t bt_hid_report_t;
#define BT_HID_INPUT 1  /**< Input sent from the Device to the Host. */
#define BT_HID_OUTPUT 2 /**< Output sent from the Host to the Device. */
#define BT_HID_FEATURE 3 /**< Bi-directional transfer. */

/**
 * @brief The HID response error codes.
 */
typedef uint8_t bt_hid_rsp_error_code_t;
#define BT_HID_SUCCESS 0x00                 /**< The operation completed successfully. */
#define BT_HID_NOT_READY 0x01               /**< The Device is busy. */
#define BT_HID_ERR_INVALID_REPORT_ID 0x02   /**< The report ID is invalid in the request. */
#define BT_HID_ERR_UNSUPPORTED_REQUEST 0x03 /**< The Device does not support the request. */
#define BT_HID_ERR_INVALID_PARAMETER 0x04    /**< Invalid parameter in the request. */
#define BT_HID_ERR_UNKNOWN  0x0E            /**< Unknown error occurred. */
#define BT_HID_ERR_FATAL 0x0F                /**< Fatal error. */

/**
 * @brief The HID control type.
 */
typedef uint8_t  bt_hid_control_t;
#define BT_HID_CONTROL_SUSPEND               3 /**< Reduced power mode. */
#define BT_HID_CONTROL_EXIT_SUSPEND          4 /**< Exit the reduced power mode. */
#define BT_HID_CONTROL_VIRTUAL_CABLE_UNPLUG  5 /**< Wipe off the bonding information. */

/**
 * @brief The HID protocol mode.
 */
typedef uint8_t bt_hid_protocol_mode_t;
#define HID_BOOT_PROTOCOL_MODE 1    /**< It is only used for Keyboard and Mouse. */
#define HID_REPORT_PROTOCOL_MODE 2  /**< The default mode after the HID is connected. */


/**
* @brief The HID event definitions.
*/
#define BT_HID_CONNECT_IND              (BT_MODULE_HID|0x02)  /**< A connect attempt is initiated from a remote device. Call the function #bt_hid_connect_response() to accept or reject the attempt. */
#define BT_HID_CONNECT_CNF              (BT_MODULE_HID|0x03)  /**< The result of a connect attempt initiated by a local or remote device is available. */
#define BT_HID_DISCONNECT_IND         (BT_MODULE_HID|0x04)  /**< A disconnect attempt is initiated by a local or remote device or a link loss occurred. */
#define BT_HID_GET_REPORT_IND  (BT_MODULE_HID|0x05)  /**< A #BT_HID_GET_REPORT_IND request is recevied from the Host. */
#define BT_HID_SET_REPORT_IND  (BT_MODULE_HID|0x06)  /**< A SET_REPORT request is received from HID.  */
#define BT_HID_CONTROL_IND    (BT_MODULE_HID|0x07) /**< A HID_CONTROL request is received from HID.  */
/**
 * @}
 */


/**
 * @defgroup Bluetoothbt_hid_struct Struct
 * @{
 * This section defines the structures for the HID.
 */

/**
 *  @brief This structure defines the #BT_HID_CONNECT_IND result.
 */
typedef struct {
    uint32_t handle;    /**< The HID handle of the current connection.*/
} bt_hid_connect_ind_t;

/**
 *  @brief This structure defines the #BT_HID_CONNECT_CNF result.
 */
typedef struct {
    uint32_t handle;    /**< The HID handle of the current connection.*/
} bt_hid_connect_cnf_t;

/**
 *  @brief This structure defines the #BT_HID_DISCONNECT_IND result.
 */
typedef struct {
    uint32_t handle;    /**< The HID handle of the current connection.*/
} bt_hid_disconnect_ind_t;

/**
*  @brief This structure defines the packaged data sent to the remote device.
*/
typedef struct {
    bt_hid_report_t type; /**< The data type.*/
    uint8_t  *packet;          /**< The data to send to a remote device.*/
    uint16_t packet_len;       /**< The data length.*/
} bt_hid_data_t;

/**
*  @brief This structure defines the data received when the Device gets the #BT_HID_GET_REPORT_IND request from the Host.
*/
typedef  struct {
    uint32_t handle;           /**< The HID handle of the current connection.*/
    bt_hid_report_t type; /**< The report type.*/
    uint8_t flag;              /**< The flag indicates if there is a report ID in the #BT_HID_GET_REPORT_IND request.*/
    uint8_t report_id;         /**< The report ID, it must exist in Boot mode or Report mode as a global item in the report descriptor.*/
    uint16_t buffer_size;      /**< The Host has a limited data buffer size.*/
} bt_hid_get_report_ind_t ;

/**
*  @brief This structure defines the response sent to the Host, when the Device receives the #BT_HID_GET_REPORT_IND request.
*/
typedef struct {
    bt_hid_report_t type; /**< The report type.*/
    uint8_t flag;              /**< The flag indicates if there is a report ID in the #BT_HID_GET_REPORT_IND request.*/
    uint8_t report_id;         /**< The report ID, it must exist in Boot mode or Report mode as a global item in the report descriptor.*/
    uint16_t buffer_size;      /**< The Host has a limited data buffer size.*/
    uint8_t report_len;        /**< The length of report.*/
    uint8_t report_data[BT_HID_MAX_REPORT_DATA_LENGTH];   /**< When in Boot mode, it cannot exceed 46 octets.*/
} bt_hid_get_report_response_t;

/**
*  @brief This struct defines the response struct send to Host, when Device recevied Get_Report request.
*/
typedef struct {
    uint32_t handle;           /**< The HID handle of the current connection.*/
    bt_hid_report_t type; /**< The report type.*/
    uint8_t report_id;
    uint8_t report_len;
    uint8_t *report_data;
} bt_hid_set_report_ind_t;

/**
*  @brief This struct defines the request struct recevied from Host, when Device recevied Hid_Control request.
*/
typedef struct {
    uint32_t handle;           /**< The HID handle of the current connection.*/
    uint8_t flag;             /**< The flag means suspend or exit suspend or unplug */
} bt_hid_control_ind_t;
/**
 * @}
 */

BT_EXTERN_C_BEGIN
/**
 * @brief                       This function is for the HID Device to connect to a remote server, the #BT_HID_CONNECT_CNF event is reported to
 *                              indicate the result of the connection. Note that this API can only be used by an HID Device.
 * @param[out] handle           is the HID handle of the current connection.
 * @param[in] address           is the Bluetooth address of a remote device.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 *                              #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_hid_connect(uint32_t *handle, const bt_bd_addr_t *address);
/**
 * @brief                 This function is for the HID device to accept or reject connection from the Host, the #BT_HID_CONNECT_CNF event
 *                        is reported to indicate the result of the response.
 * @param[in] handle      is the HID handle of the current connection.
 * @param[in] accept      defines whether the connection is accepted.
 * @return                #BT_STATUS_SUCCESS, the operation completed successfully.
 *                        #BT_STATUS_FAIL, the operation has failed.
 *                        #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */

bt_status_t bt_hid_connect_response(uint32_t handle, bool accept);/*name keep same*/

/**
 * @brief                 This function is for the HID Device to send data to the HID Host.
 * @param[in] handle      is the HID handle of the current connection.
 * @param[in] data        defines the data packet of send the connection is accepted.
 * @return                #BT_STATUS_SUCCESS, the operation completed successfully.
 *                        #BT_STATUS_FAIL, the operation has failed.
 *                        #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_hid_send_data(uint32_t handle, const bt_hid_data_t *data);

/**
* @brief                 This function is for the HID Device to respond to the #BT_HID_GET_REPORT_IND request received from the Host. Note that this API can only be used by the HID Device.
* @param[in] handle      is the HID handle of the current connection.
* @param[in] report      is the responded data.
* @return                #BT_STATUS_SUCCESS, the operation completed successfully.
*                        #BT_STATUS_FAIL, the operation has failed.
*                        #BT_STATUS_OUT_OF_MEMORY, out of memory.
*/
bt_status_t bt_hid_send_get_report_response(uint32_t handle, const bt_hid_get_report_response_t *report);

/**
* @brief                 This function is for the HID Device to send a HID control request to the Host.
* @param[in] handle      is the HID handle of the current connection.
* @param[in] type        is the operation type of HID control.
* @return                #BT_STATUS_SUCCESS, the operation completed successfully.
*                        #BT_STATUS_FAIL, the operation has failed.
*                        #BT_STATUS_OUT_OF_MEMORY, out of memory.
*/
bt_status_t bt_hid_send_hid_control(uint32_t handle, bt_hid_control_t type);

/**
 * @brief               This function disconnects an existing connection. The #BT_HID_DISCONNECT_IND event is reported
 *                      to indicate the result of the disconnection.
 * @param[in] handle    is the HID handle of the current connection.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_hid_disconnect(uint32_t handle);

/**
* @brief                This function gets the remote device's Bluetooth address of the current connection.
* @param[in] handle     is the handle of the current connection.
* @return               The Bluetooth address.
*/
bt_bd_addr_t *bt_hid_get_bd_addr_by_handle(uint32_t handle);

/**
* @brief                This function gets the handle of the current connection.
* @param[in] addr       is the remote device's Bluetooth address of the current connection.
* @return               The hid handle.
*/
uint32_t bt_hid_get_handle_by_addr(bt_bd_addr_t *addr);

BT_EXTERN_C_END

/**
* @}
* @}
*
*/
#endif

