/*
* (C) 2022  Airoha Technology Corp. All rights reserved.
*
* This software/firmware and related documentation ("Airoha Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
* Without the prior written permission of Airoha and/or its licensors,
* any reproduction, modification, use or disclosure of Airoha Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
* You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
* if you have agreed to and been bound by the applicable license agreement with
* Airoha ("License Agreement") and been granted explicit permission to do so within
* the License Agreement ("Permitted User").  If you are not a Permitted User,
* please cease any access or use of Airoha Software immediately.
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
* ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
* WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
* AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
*/

#ifndef __BLE_OTP_H__
#define __BLE_OTP_H__
/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothOTP OTP
 * @{
 * This section introduces the Object Transfer Profile (OTP) APIs including terms and acronyms, supported features, details on how to use this driver, OTP function groups, enums, structures and functions.
 * For more information about OTP, please refer to the <a href="https://www.bluetooth.com/specifications/specs/?status=active&show_latest_version=0&show_latest_version=1&keyword=OTP&filter=">Object Transfer Profile 1.0</a>.
 * @addtogroup OTP_LE Bluetooth Low Energy
 * @{
 * This section defines the Low Energy (LE) OTP confirmation and indication macros, structures and API prototypes.
 * It defines the generic LE procedures related to device discovery and LE link connectivity.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b OTP                        | Object Transfer Profile. This Bluetooth profile defines fundamental requirements to enable an Object Client to create and delete objects and to execute a variety of actions using the currently selected object such as reading object data from or writing object data to an Object Server that exposes the Object Transfer Service. |
 * |\b OTS                        | Object Transfer Service. This service provides management and control features supporting bulk data transfers which occur via a separate L2CAP connection oriented channel.|
 *
 * @section bt_otp_le_api_usage How to use this module
 *
 * - The application calls functions #ble_otp_init(), #ble_otp_set_service_attribute().
 * - Then it receives a OTP confirmation or indication. The OTP confirmation or indication is used to notify the application that the process is complete or the indication is received.
 *   BLE_OTP_XXX_CNF confirms that the process completed successfully.
 *   BLE_OTP_XXX_IND confirms that a corresponding indicator is received.
 * - The OTP confirmation and indication structures provide related information to the application.
 *   The application receives related data through the event parameters.
 *   In addition, a user-defined event callback is implemented to apply actions after receiving the confirmations or indications.
 *  - Sample code:
 *      @code
 *          // Initial OTP and register discovery OTS from the application.
 *          void user_application_init()
 *          {
 *              if (BT_STATUS_SUCCESS != ble_otp_init(APP_MAX_LINK_NUM)) {
 *                  return BT_STATUS_FAIL;
 *              }
 *
 *              memset(&g_otp_charc, 0, sizeof(app_otp_discovery_charc_t));
 *
 *              i = BLE_OTS_MAX_CHARC_NUMBER;
 *              while (i > 0) {
 *                  i--;
 *                  g_otp_charc.charc[i].descriptor_count = 1;
 *                  g_otp_charc.charc[i].descriptor = &g_otp_charc.descrp[i];
 *              }
 *
 *              // register for included service
 *              memset(&g_otp_included_service, 0, sizeof(bt_gattc_discovery_service_t));
 *              g_otp_included_service.characteristic_count = BLE_OTS_MAX_CHARC_NUMBER;
 *              g_otp_included_service.charateristics = g_otp_charc.charc;
 *              service_uuid.uuid.uuid16 = BLE_UUID16_OBJECT_TRANSFER_SERVICE;
 *              g_otp_included_service.is_included_definition = TRUE;
 *              bt_gattc_discovery_service_register(&service_uuid, &g_otp_included_service, app_otp_discovery_ots_included_service_callback);
 *          }
 *
 *          //A user-defined static callback for the application to listen to the service discovery event.
 *          static void app_otp_discovery_ots_included_service_callback(bt_gattc_discovery_event_t *event)
 *          {
 *              APPS_LOG_MSGID_I(LOG_TAG"[OTP] app_otp_discovery_ots_included_service_callback last:%d", 1, event->last_instance);
 *              is_last = event->last_instance;
 *              if (!app_otp_discovery_callback_included_service(event)) {
 *                  bt_gattc_discovery_event_response();
 *              }
 *          }
 *          //A user-defined static callback for the application to listen to the service discovery event.
 *          static bool app_otp_discovery_callback_included_service(bt_gattc_discovery_event_t *event)
 *          {
 *              ble_otp_characteristic_t charc[BLE_OTS_MAX_CHARC_NUMBER];
 *              ble_otp_set_service_attribute_parameter_t param;
 *              bt_status_t status;
 *              uint32_t i = 0;
 *              bt_gattc_discovery_service_t *p_service;
 *
 *              if (NULL == event) {
 *                  APPS_LOG_MSGID_I(LOG_TAG"[OTP] app_otp_discovery_callback_included_service null event", 0);
 *                  return FALSE;
 *              }
 *
 *              if (BT_GATTC_DISCOVERY_EVENT_COMPLETE != event->event_type) {
 *                  if (BT_GATTC_DISCOVERY_ERROR_SERVICE_NOT_FOUND == event->params.error_code) {
 *                      app_otp_set_attribute_callback_included_service(0xFF);
 *                      return TRUE;
 *                  }
 *                  return FALSE;
 *              }
 *
 *              p_service = &g_otp_included_service;
 *
 *              APPS_LOG_MSGID_I(LOG_TAG"[OTP] app_otp_discovery_callback_included_service, charc_num:%d", 1, p_service->char_count_found);
 *
 *              memset(&param, 0, sizeof(ble_otp_set_service_attribute_parameter_t));
 *
 *              if (0 != p_service->char_count_found) {
 *
 *                  // Fill OTP charc table
 *                  i = p_service->char_count_found;
 *                  while (i > 0) {
 *                      i--;
 *                      (charc + i)->uuid = p_service->charateristics[i].char_uuid.uuid.uuid16;
 *                      (charc + i)->value_handle = p_service->charateristics[i].value_handle;
 *                      (charc + i)->desc_handle = p_service->charateristics[i].descriptor[0].handle;
 *                  }
 *                  param.charc = charc;
 *              }
 *              param.start_handle = p_service->start_handle;
 *              param.end_handle = p_service->end_handle;
 *              param.charc_num = p_service->char_count_found;
 *
 *              param.is_complete = event->last_instance;
 *              param.callback = app_otp_set_attribute_callback_included_service;
 *
 *              status = ble_otp_set_service_attribute(event->conn_handle, &param);
 *              APPS_LOG_MSGID_I(LOG_TAG"[OTP] app_otp_discovery_callback_included_service, status:%x last_instance:%d", 2, status, event->last_instance);
 *
 *              if (BT_STATUS_SUCCESS != status) {
 *                  return FALSE;
 *              }
 *
 *              return TRUE;
 *          }
 *
 *          // A user-defined static callback for the application to listen to the event.
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *          {
 *              if (NULL == buff) {
 *                  return BT_STATUS_FAIL;
 *              }
 *
 *             switch (msg) {
 *                     case BLE_OTP_DISCOVER_SERVICE_COMPLETE_IND: {
 *                         ble_otp_discover_service_complete_t *evt = (ble_otp_discover_service_complete_t *)buff;
 *                         LE_AUDIO_LOG_I(BT_LE_AUDIO_MCP_100, 2, status, evt->instance_count);
 *                         break;
 *                     }
 *
 *                     case BLE_OTP_OBJECT_DATA_IND: {
 *                         ble_mcp_info_t *p_info;
 *                         ble_otp_object_data_ind_t *notify = (ble_otp_object_data_ind_t *)buff;
 *                         if (NULL == (p_info = ble_mcp_get_info(notify->handle))) {
 *                             return BT_STATUS_CONNECTION_NOT_FOUND;
 *                         }
 *                         notify->service_index = p_info->current_srv_idx;
 *                         le_audio_app_event_callback(BLE_MCP_OBJECT_DATA_NOTIFY, status, notify);
 *                         break;
 *                     }
 *                     case BLE_OTP_READ_OBJECT_BY_OBJECT_ID_CNF: {
 *                         ble_mcp_info_t *p_info;
 *                         ble_otp_read_object_by_object_id_cnf_t *cnf = (ble_otp_read_object_by_object_id_cnf_t *)buff;
 *                         if (NULL == (p_info = ble_mcp_get_info(cnf->handle))) {
 *                             return BT_STATUS_CONNECTION_NOT_FOUND;
 *                         }
 *                         return ble_mcp_read_object_state_machine(cnf->handle, p_info->current_srv_idx, BLE_OTP_READ_OBJECT_BY_OBJECT_ID_CNF, NULL, status);
 *                     }
 *             }
 *
 *             return BT_STATUS_SUCCESS;
 *         }
 *
 *      @endcode
 *
 */
#include "bt_type.h"
#include "ble_ots_def.h"
/**
 * @defgroup Bluetoothhbif_otp_define Define
 * @{
 */

#define BLE_OTP_INVALID_SERVICE_INDEX            (0xFF)     /**< Invalid OTS idx. */
#define BLE_OTP_ATTRIBUTE_HANDLE_INVALID         (0)        /**< Invalid attribute handle. */
#define BLE_OTP_OBJECT_TYPE_MAX_LENGTH           (16)       /**< Unit: Bytes. 16-bit or 128-bit UUID. */

/**
 * @brief Event reported to the user
 */
#define BLE_OTP_READ_OTS_FEATURE_CNF                                     (BT_MODULE_OTP | 0x01) /**< The result of reading OTS feature #ble_otp_read_ots_feature_cnf_t. */
#define BLE_OTP_READ_OBJECT_NAME_CNF                                     (BT_MODULE_OTP | 0x02) /**< The result of reading object name #ble_otp_read_object_name_cnf_t. */
#define BLE_OTP_READ_OBJECT_TYPE_CNF                                     (BT_MODULE_OTP | 0x03) /**< The result of reading object type #ble_otp_read_object_type_cnf_t. */
#define BLE_OTP_READ_OBJECT_SIZE_CNF                                     (BT_MODULE_OTP | 0x04) /**< The result of reading object size #ble_otp_read_object_size_cnf_t. */
#define BLE_OTP_READ_OBJECT_ID_CNF                                       (BT_MODULE_OTP | 0x05) /**< The result of reading object id #ble_otp_read_object_id_cnf_t. */
#define BLE_OTP_READ_OBJECT_PROPERTIES_CNF                               (BT_MODULE_OTP | 0x06) /**< The result of reading object properties #ble_otp_read_object_properties_cnf_t. */
#define BLE_OTP_READ_OBJECT_LIST_FILTER_CNF                              (BT_MODULE_OTP | 0x07) /**< The result of reading object list filter #ble_otp_read_object_list_filter_cnf_t. */
#define BLE_OTP_WRITE_OBJECT_LIST_FILTER_CNF                             (BT_MODULE_OTP | 0x08) /**< The result of writing object list filter #ble_otp_write_object_list_filter_cnf_t. */
#define BLE_OTP_CREATE_OBJECT_CNF                                        (BT_MODULE_OTP | 0x09) /**< The result of creating object #ble_otp_create_object_cnf_t. */
#define BLE_OTP_DELETE_OBJECT_CNF                                        (BT_MODULE_OTP | 0x0A) /**< The result of deleting object #ble_otp_delete_object_cnf_t. */
#define BLE_OTP_CALCULATE_CHECKSUM_CNF                                   (BT_MODULE_OTP | 0x0B) /**< The result of calculating checksum #ble_otp_calculate_checksum_cnf_t. */
#define BLE_OTP_EXECUTE_OBJECT_CNF                                       (BT_MODULE_OTP | 0x0C) /**< The result of executing object #ble_otp_execute_cnf_t. */
#define BLE_OTP_READ_OBJECT_CNF                                          (BT_MODULE_OTP | 0x0D) /**< The result of reading object name #ble_otp_read_object_cnf_t. */
#define BLE_OTP_ABORT_OBJECT_CNF                                         (BT_MODULE_OTP | 0x0E) /**< The result of aborting object #ble_otp_abort_object_cnf_t. */
#define BLE_OTP_LIST_FIRST_CNF                                           (BT_MODULE_OTP | 0x0F) /**< The result of listing first #ble_otp_list_first_cnf_t. */
#define BLE_OTP_LIST_LAST_CNF                                            (BT_MODULE_OTP | 0x10) /**< The result of listing last #ble_otp_list_last_cnf_t. */
#define BLE_OTP_LIST_PREVIOUS_CNF                                        (BT_MODULE_OTP | 0x11) /**< The result of listing previous #ble_otp_list_previous_cnf_t. */
#define BLE_OTP_LIST_NEXT_CNF                                            (BT_MODULE_OTP | 0x12) /**< The result of listing next #ble_otp_list_next_cnf_t. */
#define BLE_OTP_LIST_GO_TO_CNF                                           (BT_MODULE_OTP | 0x13) /**< The result of listing go to #ble_otp_list_go_to_cnf_t. */
#define BLE_OTP_LIST_ORDER_CNF                                           (BT_MODULE_OTP | 0x14) /**< The result of listing order #ble_otp_list_order_cnf_t. */
#define BLE_OTP_LIST_REQUEST_NUMBER_OF_OBJECTS_CNF                       (BT_MODULE_OTP | 0x15) /**< The result of listing request number of objects #ble_otp_list_request_number_of_objects_cnf_t. */
#define BLE_OTP_LIST_CLEAR_MARKING_CNF                                   (BT_MODULE_OTP | 0x16) /**< The result of listing clear marking #ble_otp_list_clear_marking_cnf_t. */
#define BLE_OTP_READ_OBJECT_BY_OBJECT_ID_CNF                             (BT_MODULE_OTP | 0x17) /**< The result of reading object by object id #ble_otp_write_cnf_t. */

#define BLE_OTP_SET_OBJECT_ACTION_CONTROL_POINT_INDICATION_CNF           (BT_MODULE_OTP | 0x40) /**< The result of setting notification of the Object Action Control Point characteristic with payload #ble_otp_set_object_action_control_point_indication_cnf_t. */
#define BLE_OTP_SET_OBJECT_LIST_CONTROL_POINT_INDICATION_CNF             (BT_MODULE_OTP | 0x41) /**< The result of setting notification of the Object List Control Point characteristic with payload #ble_otp_set_object_list_control_point_indication_cnf_t. */
#define BLE_OTP_SET_OBJECT_CHANGED_INDICATION_CNF                        (BT_MODULE_OTP | 0x42) /**< The result of setting notification of the Object Changed characteristic with payload #ble_otp_set_object_changed_indication_cnf_t. */

#define BLE_OTP_OBJECT_ACTION_CONTROL_POINT_INDICATION_IND               (BT_MODULE_OTP | 0x60) /**< This event indicates the Object Action Control Point responses with payload #ble_otp_object_action_control_point_ind_t. */
#define BLE_OTP_OBJECT_LIST_CONTROL_POINT_INDICATION_IND                 (BT_MODULE_OTP | 0x61) /**< This event indicates the Object List Control Point responses with payload #ble_otp_object_list_control_point_ind_t. */
#define BLE_OTP_OBJECT_CHANGED_INDICATION_IND                            (BT_MODULE_OTP | 0x62) /**< This event indicates the Object changes with payload #ble_otp_object_cheanged_ind_t. */

#define BLE_OTP_OBJECT_DATA_IND                                          (BT_MODULE_OTP | 0x80) /**< This event indicates the Object data with payload #ble_otp_object_data_ind_t. */
#define BLE_OTP_ACTION_FAILED_IND                                        (BT_MODULE_OTP | 0x81) /**< This event indicates the Object changes with payload #ble_otp_object_cheanged_ind_t. */

#define BLE_OTP_DISCOVER_SERVICE_COMPLETE_IND                            (BT_MODULE_OTP | 0x00) /**< This event indicates the MCP service discovery complete with payload #ble_otp_discover_service_complete_t. */
typedef uint32_t ble_otp_event_t;                                    /**< The type of OTP events.*/

/**
 * @}
 */

/**
 * @defgroup Bluetoothble_otp_struct Struct
 * @{
 */

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_DISCOVER_SERVICE_COMPLETE_IND.
 */
typedef struct {
    bt_handle_t handle;                 /**< Connection handle. */
    uint8_t instance_count;             /**< The number of service instances.*/
} ble_otp_discover_service_complete_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_READ_OTS_FEATURE_CNF.
 */
typedef struct {
    bt_handle_t handle;                         /**< Connection handle. */
    uint8_t service_index;                      /**< Service index. */
    ble_ots_oacp_features_t oacp_features;      /**< OACP features. */
    ble_ots_olcp_features_t olcp_features;      /**< OLCP features. */
} ble_otp_read_ots_feature_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_READ_OBJECT_NAME_CNF.
 */
typedef struct {
    bt_handle_t handle;                         /**< Connection handle. */
    uint8_t service_index;                      /**< Service index. */
    uint8_t object_name_length;                 /**< Object name length. */
    uint8_t *p_object_name;                     /**< Object name. */
} ble_otp_read_object_name_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_READ_OBJECT_TYPE_CNF.
 */
typedef struct {
    bt_handle_t handle;                         /**< Connection handle. */
    uint8_t service_index;                      /**< Service index. */
    uint8_t object_type_length;                 /**< Object type length. */
    uint8_t *p_object_type;                     /**< Object type. */
} ble_otp_read_object_type_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_READ_OBJECT_SIZE_CNF.
 */
typedef struct {
    bt_handle_t handle;                         /**< Connection handle. */
    uint8_t service_index;                      /**< Service index. */
    uint32_t object_current_size;               /**< Object current size. */
    uint32_t object_allocated_size;             /**< Object allocated size. */
} ble_otp_read_object_size_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_READ_OBJECT_ID_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
    uint8_t object_id[BLE_OTS_OBJECT_ID_LENGTH];    /**< Object id. */
} ble_otp_read_object_id_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_READ_OBJECT_BY_OBJECT_ID_CNF.
 */
typedef struct {
    bt_handle_t handle;                         /**< Connection handle. */
    uint8_t service_index;                      /**< Service index. */
} ble_otp_read_object_by_object_id_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_READ_OBJECT_PROPERTIES_CNF.
 */
typedef struct {
    bt_handle_t handle;                                 /**< Connection handle. */
    uint8_t service_index;                              /**< Service index. */
    ble_ots_object_properties_t object_properties;      /**< Object Properties. */
} ble_otp_read_object_properties_cnf_t;

/**
 *  @brief This structure defines the string for filter.
 */
typedef struct {
    uint8_t *p_string;      /**< String. */
} ble_otp_string_parameter;

/**
 *  @brief This structure defines the timestamp range for filter.
 */
typedef struct {
    uint32_t timestamp1;    /**< Timestamp1. */
    uint32_t timestamp2;    /**< Timestamp2. */
} ble_otp_timestamp_parameter;

/**
 *  @brief This structure defines the size range for filter.
 */
typedef struct {
    uint32_t size1;         /**< Size1. */
    uint32_t size2;         /**< Size2. */
} ble_otp_size_parameter;

/**
 *  @brief This structure defines the parameter of filter.
 */
typedef struct {
    union {
        ble_otp_string_parameter string;            /**< String for name starts, name ends with, name contains or name is exactly. */
        ble_otp_timestamp_parameter timestamp;      /**< Timestamp for created between or modified between. */
        ble_otp_size_parameter size;                /**< Size for current size between or allocated size between. */
    } value;
} ble_otp_filter_parameter;

/**
 *  @brief This structure defines the parameter of list filter characteristic.
 */
typedef struct {
    ble_ots_filter_value_t filter_value;            /**< Filter value. */
    uint8_t filter_length;                          /**< Filter length. */
    ble_otp_filter_parameter filter_parameter;      /**< Filter prarameter. */
} ble_otp_object_list_filter_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_READ_OBJECT_LIST_FILTER_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
    uint8_t characteristic_index;                   /**< Filter characteristic index. */
    ble_ots_filter_value_t filter_value;            /**< Filter value. */
    uint8_t filter_length;                          /**< Filter length. */
    ble_otp_filter_parameter filter_parameter;      /**< Filter parameter. */
} ble_otp_read_object_list_filter_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_WRITE_OBJECT_LIST_FILTER_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_write_object_list_filter_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_CREATE_OBJECT_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_create_object_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_DELETE_OBJECT_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_delete_object_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_CALCULATE_CHECKSUM_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_calculate_checksum_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_EXECUTE_OBJECT_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_execute_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_READ_OBJECT_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_read_object_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_ABORT_OBJECT_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_abort_object_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_LIST_FIRST_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_list_first_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_LIST_LAST_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_list_last_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_LIST_PREVIOUS_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_list_previous_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_LIST_NEXT_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_list_next_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_LIST_GO_TO_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_list_go_to_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_LIST_ORDER_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_list_order_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_LIST_REQUEST_NUMBER_OF_OBJECTS_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_list_request_number_of_objects_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_LIST_CLEAR_MARKING_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_list_clear_marking_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_SET_OBJECT_ACTION_CONTROL_POINT_INDICATION_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_set_object_action_control_point_indication_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_SET_OBJECT_LIST_CONTROL_POINT_INDICATION_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_set_object_list_control_point_indication_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_SET_OBJECT_CHANGED_INDICATION_CNF.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_set_object_changed_indication_cnf_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_DISCOVER_SERVICE_COMPLETE_IND.
 */
typedef struct {
    uint32_t checksum;
} ble_otp_object_action_control_point_parameter_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_OBJECT_ACTION_CONTROL_POINT_INDICATION_IND.
 */
typedef struct {
    bt_handle_t handle;                                             /**< Connection handle. */
    uint8_t service_index;                                          /**< Service index. */
    ble_ots_oacp_opcode_t response_code;                            /**< Response code. */
    ble_ots_oacp_opcode_t requested_opcode;                         /**< Requested opcode. */
    ble_ots_oacp_result_code_t result_code;                         /**< Result code. */
    ble_otp_object_action_control_point_parameter_t parameter;      /**< Parameter of the requested opcode. */
} ble_otp_object_action_control_point_ind_t;

typedef struct {
    uint32_t total_number_of_objects;
} ble_otp_object_list_control_point_parameter_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_OBJECT_LIST_CONTROL_POINT_INDICATION_IND.
 */
typedef struct {
    bt_handle_t handle;                                         /**< Connection handle. */
    uint8_t service_index;                                      /**< Service index. */
    ble_ots_olcp_opcode_t response_code;                        /**< Response code. */
    ble_ots_olcp_opcode_t requested_opcode;                     /**< Requested opcode. */
    ble_ots_olcp_result_code_t result_code;                     /**< Result code. */
    ble_otp_object_list_control_point_parameter_t parameter;    /**< Parameter of the requested opcode. */
} ble_otp_object_list_control_point_ind_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_OBJECT_CHANGED_INDICATION_IND.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
    ble_ots_object_changed_flags_field_t flags;     /**< Flags. */
    uint8_t object_id[BLE_OTS_OBJECT_ID_LENGTH];    /**< Object id. */
} ble_otp_object_cheanged_ind_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_DISCOVER_SERVICE_COMPLETE_IND.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
    uint32_t offset;                                /**< data offset. */
    uint32_t data_size;                             /**< Data size. */
    uint8_t *p_data;                                /**< Data. */
} ble_otp_object_data_ind_t;

/**
 *  @brief This structure defines the parameter for event #BLE_OTP_DISCOVER_SERVICE_COMPLETE_IND.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle. */
    uint8_t service_index;                          /**< Service index. */
} ble_otp_action_failed_ind_t;

/**
 * @}
 */

/**
 * @brief                       This function initializes Object Transfer Profile.
 * @param[in] max_link          is the supported maximum number of link.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 *                              #BT_STATUS_OUT_OF_MEMORY, the operation is out of memory.
 */
bt_status_t ble_otp_init(uint32_t max_link);

/**
 * @brief                       This function de-initializes Object Transfer Profile.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_deinit(void);

/**
 * @brief                       This function reads OTS feature.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_read_ots_feature_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function reads object name.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_read_object_name_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function reads object type.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_read_object_type_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function reads object size.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_read_object_size_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function reads object id.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_read_object_id_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function reads object properties.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_read_object_properties_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function reads object list filters.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_read_object_list_filter_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function writes object list filter.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @param[in] filters           is the three filters.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_write_object_list_filter_req(bt_handle_t handle, uint8_t service_index, ble_otp_object_list_filter_t filters[BLE_OTS_LIST_FILTER_CHARC_NUMBER]);

/**
 * @brief                       This function creates a object.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @param[in] size              is the object size.
 * @param[in] type              is the object type.
 * @param[in] object_type_size  is the object type size.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_create_object_req(bt_handle_t handle, uint8_t service_index, uint32_t size, uint8_t type[BLE_OTP_OBJECT_TYPE_MAX_LENGTH], uint8_t object_type_size);

/**
 * @brief                       This function deletes the current object.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_delete_object_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function calculates the checksum of the current object content.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @param[in] offset            is the offset of current object content.
 * @param[in] length            is the length of current object.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_calculate_checksum_req(bt_handle_t handle, uint8_t service_index, uint32_t offset, uint32_t length);

/**
 * @brief                       This function executes the current object.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_execute_object_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function reads the current object.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @param[in] offset            is the offset of current object content.
 * @param[in] length            is the length of current object.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_read_object_req(bt_handle_t handle, uint8_t service_index, uint32_t offset, uint32_t length);

/**
 * @brief                       This function aborts the current operation.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_abort_object_req(bt_handle_t handle, uint8_t service_index);

/* Object List Control Point */
/**
 * @brief                       This function sets the first object to current object.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_list_first_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function sets the last object to current object.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_list_last_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function sets the previous object to current object.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_list_previous_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function sets the next object to current object.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_list_next_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function sets current object by object id.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @param[in] service_idx       is the object id.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_list_go_to_req(bt_handle_t handle, uint8_t service_index, uint8_t object_id[BLE_OTS_OBJECT_ID_LENGTH]);

/**
 * @brief                       This function lists object by order.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @param[in] order             is the order.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_list_order_req(bt_handle_t handle, uint8_t service_index, ble_ots_list_sort_order_t order);

/**
 * @brief                       This function requests number of objects.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_list_request_number_of_objects_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function clears marking objects.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_list_clear_marking_req(bt_handle_t handle, uint8_t service_index);

/**
 * @brief                       This function sets object action control point indication.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @param[in] enable            is the enable indication.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_set_object_action_control_point_indication_req(bt_handle_t handle, uint8_t service_index, bool enable);

/**
 * @brief                       This function sets object list control point indication.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @param[in] enable            is the enable indication.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_set_object_list_control_point_indication_req(bt_handle_t handle, uint8_t service_index, bool enable);

/**
 * @brief                       This function sets object changed indication.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @param[in] enable            is the enable indication.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_set_object_changed_indication_req(bt_handle_t handle, uint8_t service_index, bool enable);

/**
 * @brief                       This function writes object list filter.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] service_idx       is the service idx.
 * @param[in] object id         is the object id.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_read_object_by_object_id_req(bt_handle_t handle, uint8_t service_index, uint8_t object_id[BLE_OTS_OBJECT_ID_LENGTH]);

/**
 * @brief                       This function opens connection oriented channel.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_otp_open_channel(bt_handle_t handle);
/**
 * @}
 * @}
 * @}
 */
#endif

