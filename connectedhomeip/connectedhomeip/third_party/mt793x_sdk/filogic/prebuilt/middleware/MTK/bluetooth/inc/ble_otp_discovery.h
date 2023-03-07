/* Copyright Statement:
 *
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

#ifndef __BLE_OTP_DISCOVERY_H__
#define __BLE_OTP_DISCOVERY_H__
/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothOTP_DISCOVERY OTP_DISCOVERY
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
#include "ble_ots_def.h"

/**
 * @defgroup Bluetoothhbif_otp_discovery_define Define
 * @{
 */

/**
 * @brief The OTP set attribute callback.
 */
typedef void (*ble_otp_set_attribute_callback_t)(uint8_t service_idx);

/**
 *  @brief This structure defines the attribute handles of OTS.
 */
typedef struct {
    uint16_t uuid;                  /**< UUID of characteristic.*/
    uint16_t value_handle;          /**< The handle of characteristic value.*/
    uint16_t desc_handle;           /**< The handle of descriptor.*/
} ble_otp_characteristic_t;

/**
* @brief The parameter of #ble_mcp_set_attribute_handle.
*/
typedef struct {
    uint16_t start_handle;                       /**< The start attribute handle of the OTS instance. */
    uint16_t end_handle;                         /**< The end attribute handle of the OTS instance. */
    bool is_complete;                            /**< Indicate OTS discovery is complete or not. */
    uint8_t charc_num;                           /**< The characteristic count of the OTS instance. */
    ble_otp_characteristic_t *charc;             /**< The characteristic information of the MCS instance. */
    ble_otp_set_attribute_callback_t callback;   /**< The callback is invoked when finish setting MCS attribute. */
} ble_otp_set_service_attribute_parameter_t;

/**
 * @}
 */

/**
 * @brief                       This function set the service attribute of OTS.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] params            is the attribute information of OTS services.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t ble_otp_set_service_attribute(bt_handle_t handle, ble_otp_set_service_attribute_parameter_t *params);

/**
 * @brief                       This function reset the service attribute of OTS.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t ble_otp_reset_service_attribute(bt_handle_t handle);
/**
 * @}
 * @}
 * @}
 */
#endif  /* __BLE_OTP_DISCOVERY_H__ */

