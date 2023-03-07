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

#ifndef __BT_CALLBACK_MANAGER_H__
#define __BT_CALLBACK_MANAGER_H__

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServices_CM BT Callback Manager
 * @{
 * This section defines the Bluetooth callback manager API to manage all Bluetooth callback functions.
 * @{
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b SDP                        | Service Discovery Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Bluetooth#SDP">Wikipedia</a>. |
 * |\b AVRCP                      | Audio/Video Remote Control Profile. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles#Audio.2FVideo_Remote_Control_Profile_.28AVRCP.29">Wikipedia</a>. |
 * |\b SPP                        | Serial Port Profile. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles#Serial_Port_Profile_.28SPP.29">Wikipedia</a>. |
 * |\b HFP                        | Hands-Free Profile. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles#Hands-Free_Profile_.28HFP.29">Hands-free profile</a> in Wikipedia. |
 * |\b GATT                       | Generic Attribute Profile defines a service framework using the Attribute Protocol for discovering services, and for reading and writing characteristic value on a remote device. |
 * |\b GAP                        | Generic Access Profile defines the generic procedures related to the discovery of Bluetooth devices and link management aspects of connecting to Bluetooth devices. It also defines procedures related to use of different security levels. |
 * |\b MM                         | Memory Management. For more information, please refer to bt_system.h.
 * |\b A2DP                       | Advanced Audio Distribution Profile. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles#Advanced_Audio_Distribution_Profile_.28A2DP.29">Wikipedia</a>. |
 * |\b PBAPC                      | Phonebook Access Profile Client. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles#Phone_Book_Access_Profile_.28PBAP.2C_PBA.29">Wikipedia</a>. |
 *
 * @section bt_callback_manager_api_usage How to use this module
 *
 * - Add a customized record with the service name, language list, service class ID list and a protocol description list for a service.
 *   Add your own record according to the SDP interoperability requirements in the related profile specification. To add customized records with the SDP macros and API:
 *  - Example code:
 *     @code
 *
 *         static bt_gap_le_local_config_req_ind_t *my_gap_le_get_local_config(void)
 *         {
 *             // Implement a user-defined function as specified by #bt_gap_le_get_local_config().
 *         }
 *
 *         static bt_status_t my_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *         {
 *             // Implement a user-defined function as specified by #bt_app_event_callback().
 *         }
 *
 *         // Refer to the bt_sdp.h for macros used below.
 *         static const uint8_t my_service_class_id_list[] =
 *         {
 *             BT_SDP_ATTRIBUTE_HEADER_8BIT(6),                    // In this example, each item is 3 bytes long and the total length of the listed items is 6 bytes.
 *             BT_SDP_UUID_16BIT(BT_SDP_SERVICE_CLASS_HANDSFREE),
 *             BT_SDP_UUID_16BIT(BT_SDP_SERVICE_CLASS_GENERIC_AUDIO)
 *         };
 *
 *         // Refer to the bt_sdp.h for the macros used below.
 *         static const bt_sdps_attribute_t my_sdp_attributes[] =
 *         {
 *             BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST, my_service_class_id_list),
 *         };
 *
 *         static const bt_sdps_record_t my_sdp_record =
 *         {
 *             .attribute_list_length = sizeof(my_sdp_attributes),
 *             .attribute_list = my_sdp_attributes,
 *         };
 *
 *         static bt_status_t my_init(void)
 *         {
 *             bt_status_t result;
 *             bt_callback_manager_register_callback(bt_callback_type_gap_le_get_local_cofig, 0, (void *)my_gap_le_get_local_config);
 *             result = bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP | MODULE_MASK_SYSTEM, (void*)my_app_event_callback);
 *             if (result != BT_STATUS_SUCCESS) {
 *                 printf("Initialization has failed! \r\n");
 *             }
 *             bt_callback_manager_add_sdp_customized_record(&my_sdp_record);
 *         }
 *         static bt_status_t my_deinit(void)
 *         {
 *             bt_status_t result;
 *             bt_callback_manager_deregister_callback(bt_callback_type_gap_le_get_local_cofig, (void *)my_gap_le_get_local_config);
 *             result = bt_callback_manager_deregister_callback(bt_callback_type_app_event, (void*)my_app_event_callback);
 *             if (result != BT_STATUS_SUCCESS) {
 *                 printf("De-initialization has failed! \r\n");
 *             }
 *         }

 *     @endcode
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "bt_type.h"
#if !defined(__MTK_BLE_ONLY_ENABLE__)
#include "bt_sdp.h"
#endif /* #if !defined(__MTK_BLE_ONLY_ENABLE__) */

#ifdef __cplusplus
extern "C"
{
#endif /* #ifdef __cplusplus */

/**
 * @defgroup BluetoothServices_CM_define Define
 * @{
 * Define the callback manager structures.
 */

/**
 * @brief This structure defines the callback type to register.
 */
typedef enum {
    bt_callback_type_app_event = 0,                              /**< The prototype of the callback function for this callback type is defined as #bt_app_event_callback(). */
#if !defined(__MTK_BLE_ONLY_ENABLE__)
    bt_callback_type_gap_get_local_configuration,                /**< Must have one and only one callback registered for this callback type. The prototype of the callback function for this callback type is defined as #bt_gap_get_local_configuration(). */
    bt_callback_type_gap_get_pin_code,                           /**< Cannot have more than one callback registered for this callback type. The prototype of the callback function for this callback type is defined as #bt_gap_get_pin_code(). */
    bt_callback_type_gap_get_link_key,                           /**< Cannot have more than one callback registered for this callback type. The prototype of the callback function for this callback type is defined as #bt_gap_get_link_key(). */
#endif /* #if !defined(__MTK_BLE_ONLY_ENABLE__) */
    bt_callback_type_gap_le_is_accept_connection_update_request, /**< Cannot have more than one callback registered for this callback type. The prototype of the callback function for this callback type is defined as #bt_gap_le_update_connection_parameter(). */
    bt_callback_type_gap_le_get_local_key,                       /**< Cannot have more than one callback registered for this callback type. The prototype of the callback function for this callback type is defined as #bt_gap_le_get_local_key(). */
    bt_callback_type_gap_le_get_local_cofig,                     /**< Must have one and only one callback registered for this callback type. The prototype of the callback function for this callback type is defined as #bt_gap_le_get_local_config(). */
    bt_callback_type_gap_le_get_pairing_config,                  /**< Must have one and only one callback registered for this callback type. The prototype of the callback function for this callback type is defined as #bt_gap_le_get_pairing_config(). */
    bt_callback_type_gap_le_get_bonding_info,                    /**< Must have one and only one callback registered for this callback type. The prototype of the callback function for this callback type is defined as #bt_gap_le_get_bonding_info(). */
    bt_callback_type_gatts_get_authorization_check_result,       /**< Cannot have more than one callback registered for this callback type. The prototype of the callback function for this callback type is defined as #bt_gatts_get_authorization_check_result(). */
    bt_callback_type_gatts_get_execute_write_result,             /**< Cannot have more than one callback registered for this callback type. The prototype of the callback function for this callback type is defined as #bt_gatts_get_execute_write_result(). */
#if !defined(__MTK_BLE_ONLY_ENABLE__)
    bt_callback_type_hfp_get_init_params,                        /**< Must have one and only one callback registered for this callback type, if HFP is required. The prototype of the callback function for this callback type is defined as #bt_hfp_get_init_params(). */
    bt_callback_type_a2dp_get_init_params,                       /**< Must have one and only one callback registered for this callback type, if A2DP is required. The prototype of the callback function for this callback type is defined as #bt_a2dp_get_init_params(). */
#endif /* #if !defined(__MTK_BLE_ONLY_ENABLE__) */
    bt_callback_type_get_feature_mask_configuration,             /**< Must have one and only one callback registered for this callback type. The prototype of the callback function for this callback type is defined as #bt_get_feature_mask_configuration(). */
    bt_callback_type_max                                         /**< The maximum number of the callback types. */
} bt_callback_type_t;

/**
 * @}
 */


/**
 * @addtogroup BluetoothServices_CM_define Define
 * @{
 * This section defines the module mask for different module events. Please refer to the bt_type.h for more details about BT_MOUDLE_**.
 */
#define MODULE_MASK_OFFSET(value) (1<<((value) >> BT_MODULE_OFFSET)) /**< To get the module offset. */
#define MODULE_MASK_MM     MODULE_MASK_OFFSET(BT_MODULE_MM)          /**< To get the module offset of the #BT_MODULE_MM. */
#define MODULE_MASK_GAP    MODULE_MASK_OFFSET(BT_MODULE_GAP)         /**< To get the module offset of the #BT_MODULE_GAP. */
#define MODULE_MASK_SYSTEM MODULE_MASK_OFFSET(BT_MODULE_SYSTEM)      /**< To get the module offset of the #BT_MODULE_SYSTEM. */
#define MODULE_MASK_GATT   MODULE_MASK_OFFSET(BT_MODULE_GATT)        /**< To get the module offset of the #BT_MODULE_GATT. */
#if !defined(__MTK_BLE_ONLY_ENABLE__)
#define MODULE_MASK_HFP    MODULE_MASK_OFFSET(BT_MODULE_HFP)         /**< To get the module offset of the #BT_MODULE_HFP. */
#define MODULE_MASK_HSP    MODULE_MASK_OFFSET(BT_MODULE_HSP)         /**< To get the module offset of the #BT_MODULE_HSP. */
#define MODULE_MASK_SPP    MODULE_MASK_OFFSET(BT_MODULE_SPP)         /**< To get the module offset of the #BT_MODULE_SPP. */
#define MODULE_MASK_AVRCP  MODULE_MASK_OFFSET(BT_MODULE_AVRCP)       /**< To get the module offset of the #BT_MODULE_AVRCP. */
#define MODULE_MASK_A2DP   MODULE_MASK_OFFSET(BT_MODULE_A2DP)        /**< To get the module offset of the #BT_MODULE_A2DP. */
#define MODULE_MASK_PBAPC  MODULE_MASK_OFFSET(BT_MODULE_PBAPC)       /**< To get the module offset of the #BT_MODULE_PBAPC. */
#define MODULE_MASK_AWS    MODULE_MASK_OFFSET(BT_MODULE_AWS)         /**< To get the module offset of the #BT_MODULE_AWS. */
#define MODULE_MASK_AWS_MCE  MODULE_MASK_OFFSET(BT_MODULE_AWS_MCE)   /**< To get the module offset of the #BT_MODULE_AWS_MCE. */
#define MODULE_MASK_AVM    MODULE_MASK_OFFSET(BT_MODULE_AVM)         /**< To get the module offset of the #BT_MODULE_AWS. */
#define MODULE_MASK_SDP    MODULE_MASK_OFFSET(BT_MODULE_SDP)         /**< To get the module offset of the #BT_MODULE_SDP. */
#define MODULE_MASK_AIRUPDATE MODULE_MASK_OFFSET(BT_MODULE_AIRUPDATE)   /**< To get the module offset of the #BT_MODULE_AIRUPDATE. */
#endif /* #if !defined(__MTK_BLE_ONLY_ENABLE__) */
#define MODULE_MASK_LE_AUDIO  MODULE_MASK_OFFSET(BT_MODULE_LE_AUDIO)    /**< To get the module offset of the #BT_MODULE_GATT. */

typedef uint32_t bt_callback_module_mask_t; /**< Type definition of a module mask. */

/**
 * @}
 */

/**
 * @brief     This function registers a callback to the Bluetooth callback manager.
 * @param[in] type          is the callback type to register.
 * @param[in] module_mask   Specifies the modules that the callback is registered for, so that the callback manager can filter the unnecessary events.
 *                          The parameter is only applicable to #bt_callback_type_app_event and needs to be set when registering for #bt_callback_type_app_event type.
 *                          The value of this parameter is ignored when registering for any other callback types. Please refer to #bt_callback_module_mask_t.
 * @param[in] callback      is a pointer to the callback function to register. This callback function is invoked by the callback manager when the Bluetooth stack notifies the callback manager.
 * @return                  #BT_STATUS_SUCCESS, if the operation is successful.
 *                          #BT_STATUS_FAIL, if the callback type is incorrect or already registered or if the maximum number of callbacks,
 *                          as defined by #BT_CALLBACK_MANAGER_APP_EVENT_CALLBACK_MAX in the header file bt_callback_manager_config.h for the #bt_callback_type_app_event type, has been reached.
 */
bt_status_t bt_callback_manager_register_callback(bt_callback_type_t type, bt_callback_module_mask_t module_mask, void *callback);

/**
 * @brief     This function unregister the callback from the Bluetooth callback manager.
 * @param[in] type          is the callback type to unregister.
 * @param[in] callback      is a pointer to the callback function to unregister.
 * @return                  #BT_STATUS_SUCCESS, if the operation is successful.
 *                          #BT_STATUS_FAIL, if the callback type is incorrect or unregistered, or the callback function is unregistered.
 */
bt_status_t bt_callback_manager_deregister_callback(bt_callback_type_t type, void *callback);

#if !defined(__MTK_BLE_ONLY_ENABLE__)
/**
 * @brief     This function adds a user record to the SDP. For more information about how to add a record, please refer to the bt_sdp.h.
 * @param[in] record   is the pointer to the record.
 * @return             #BT_STATUS_SUCCESS, if the record was added successfully.
 *                     #BT_STATUS_FAIL, if the maximum number of records, as defined by #BT_CALLBACK_MANAGER_USER_RECORD_MAX in bt_callback_manager_config.h, has been reached.
 */
bt_status_t bt_callback_manager_add_sdp_customized_record(const bt_sdps_record_t *record);
#endif /* #if !defined(__MTK_BLE_ONLY_ENABLE__) */

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/**
 * @}
 * @}
 * @}
 */


#endif /* #ifndef __BT_CALLBACK_MANAGER_H__ */

