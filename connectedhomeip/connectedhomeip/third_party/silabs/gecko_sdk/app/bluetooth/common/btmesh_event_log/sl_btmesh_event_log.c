/***************************************************************************//**
 * @file
 * @brief Bt Mesh Event Logging module
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "sl_btmesh_api.h"
#include "sl_bt_api.h"

#include "app_log.h"

#include "sl_btmesh_event_log_config.h"
#include "sl_btmesh_event_log.h"

/***************************************************************************//**
 * @addtogroup BtMeshEventLog
 * @{
 ******************************************************************************/

/*******************************************************************************
 * Handling of sensor client stack events. Both BLuetooth LE and Bluetooth mesh
 * events are handled here.
 * @param[in] evt_id  Incoming event ID.
 * @param[in] evt     Pointer to incoming event.
 ******************************************************************************/
void sl_btmesh_handle_btmesh_logging_events(sl_btmesh_msg_t *evt)
{
  if (NULL == evt) {
    return;
  }

  uint32_t evt_id = SL_BT_MSG_ID(evt->header);

  // Handle events
  switch (evt_id) {
    /* Node */
    case sl_bt_evt_connection_opened_id:
      app_log("evt:le_connection_opened_id\r\n");
      break;

    /* Connection */
    case sl_bt_evt_connection_parameters_id:
      app_log("evt:le_connection_parameters_id: "
              "interval %u, latency %u, timeout %u\r\n",
              ((sl_bt_msg_t *)(evt))->data.evt_connection_parameters.interval,
              ((sl_bt_msg_t *)(evt))->data.evt_connection_parameters.latency,
              ((sl_bt_msg_t *)(evt))->data.evt_connection_parameters.timeout);
      break;
    case sl_bt_evt_connection_closed_id:
      app_log("evt:conn closed, reason 0x%x\r\n",
              ((sl_bt_msg_t *)(evt))->data.evt_connection_closed.reason);
      break;
    case sl_btmesh_evt_node_reset_id:
      app_log("evt:mesh_node_reset\r\n");
      break;
    case sl_btmesh_evt_node_initialized_id:
      app_log("evt:mesh_node_initialized\r\n");
      break;

    /* Provisioning */
    case sl_btmesh_evt_node_provisioning_started_id:
      app_log("evt:mesh_node_provisioning_started\r\n");
      break;

    case sl_btmesh_evt_node_provisioned_id:
      app_log("evt:mesh_node_node_provisioned\r\n");
      break;

    case sl_btmesh_evt_node_provisioning_failed_id:
      app_log("evt:mesh_node_provisioning_failed, code %x\r\n",
              evt->data.evt_node_provisioning_failed.result);
      break;

    /* Sensor Client */
    case sl_btmesh_evt_sensor_client_descriptor_status_id:
      app_log("evt:mesh_sensor_client_descriptor_status\r\n");
      break;
    case sl_btmesh_evt_sensor_client_status_id:
      app_log("evt:mesh_sensor_client_status %s\r\n",
              (evt->data.evt_sensor_client_status.client_address & 0xC000)
              == 0xC000 ? "(group broadcast)"
              : (evt->data.evt_sensor_client_status.client_address & 0x1000)
              == 0 ? "(unicast)" : "(virtual)");
      break;

    /* Sensor Server */
    case sl_btmesh_evt_sensor_server_get_request_id:
      app_log("evt:mesh_sensor_server_get_request (from: 0x%04x)\r\n",
              evt->data.evt_sensor_server_get_request.client_address);
      break;

    case sl_btmesh_evt_sensor_server_get_column_request_id:
      app_log("evt:mesh_sensor_server_get_column_request\r\n");
      break;

    case sl_btmesh_evt_sensor_server_get_series_request_id:
      app_log("evt:mesh_sensor_server_get_series_request\r\n");
      break;

    case sl_btmesh_evt_sensor_server_publish_id:
      app_log("evt:mesh_sensor_server_publish\r\n");
      break;

    case sl_btmesh_evt_sensor_setup_server_get_cadence_request_id:
      app_log("evt:mesh_sensor_setup_server_get_cadence_request\r\n");
      break;

    case sl_btmesh_evt_sensor_setup_server_set_cadence_request_id:
      app_log("evt:mesh_sensor_setup_server_set_cadence_request\r\n");
      break;

    case sl_btmesh_evt_sensor_setup_server_get_settings_request_id:
      app_log("evt:mesh_sensor_setup_server_get_settings_request\r\n");
      break;

    case sl_btmesh_evt_sensor_setup_server_get_setting_request_id:
      app_log("evt:mesh_sensor_setup_server_get_setting_request\r\n");
      break;

    case sl_btmesh_evt_sensor_setup_server_set_setting_request_id:
      app_log("evt:mesh_sensor_setup_server_set_setting_request\r\n");
      break;

    /* Node Configuration */
    case sl_btmesh_evt_node_config_set_id:
      app_log("evt:mesh_node_config_set id=0x%04x\r\n",
              evt->data.evt_node_config_set.id);
      break;

    case sl_btmesh_evt_node_model_config_changed_id: {
      uint8_t conf_state =
        evt->data.evt_node_model_config_changed.node_config_state;
      app_log("evt:mesh_node_model_config_changed "
              "vid=%x, model=%x, conf_state=%s\r\n",
              evt->data.evt_node_model_config_changed.vendor_id, evt->data
              .evt_node_model_config_changed.model_id,
              conf_state == 0 ? "application key bindings"
              : conf_state == 1
              ? "publication parameters" : "subscription list");
      (void)conf_state;
    }
    break;

    case sl_btmesh_evt_node_key_added_id:
      app_log("evt:mesh_node_key_added\r\nGot new %s key with index 0x%x\r\n",
              evt->data.evt_node_key_added.type == 0
              ? "network" : "application",
              evt->data.evt_node_key_added.index);
      break;

    /* Friendship - Friend Node */
    case sl_btmesh_evt_friend_friendship_established_id:
      app_log("evt:mesh_friend_friendship_established "
              "netkey_index=%d, lpn_address=0x%04x\r\n",
              evt->data.evt_friend_friendship_established.netkey_index,
              evt->data.evt_friend_friendship_established.lpn_address);
      break;

    case sl_btmesh_evt_friend_friendship_terminated_id:
      app_log("evt:mesh_friend_friendship_terminated "
              "netkey_index=%d, lpn_address=0x%04x, reason=0x%04x\r\n",
              evt->data.evt_friend_friendship_terminated.netkey_index,
              evt->data.evt_friend_friendship_terminated.lpn_address,
              evt->data.evt_friend_friendship_terminated.reason);
      break;

    /* Friendship - Low Power Node*/
    case sl_btmesh_evt_lpn_friendship_failed_id:
      app_log("evt:mesh_lpn_friendship_failed "
              "netkey_index=%d, reason=0x%04x\r\n",
              evt->data.evt_lpn_friendship_failed.netkey_index,
              evt->data.evt_lpn_friendship_failed.reason);
      break;

    case sl_btmesh_evt_lpn_friendship_established_id:
      app_log("evt:mesh_lpn_friendship_established "
              "netkey_index=%d, addr=0x%04x\r\n",
              evt->data.evt_lpn_friendship_established.netkey_index,
              evt->data.evt_lpn_friendship_established.friend_address);
      break;

    case sl_btmesh_evt_lpn_friendship_terminated_id:
      app_log("evt:mesh_lpn_friendship_terminated "
              "netkey_index=%d, reason=0x%04x\r\n",
              evt->data.evt_lpn_friendship_terminated.netkey_index,
              evt->data.evt_lpn_friendship_terminated.reason);
      break;

    /* Proxy */
    case sl_btmesh_evt_proxy_connected_id:
      app_log("evt:mesh_proxy_connected_id\r\n");
      break;

    case sl_btmesh_evt_proxy_disconnected_id:
      app_log("evt:mesh_proxy_disconnected_id\r\n");
      break;

    /* Scene Server */
    case sl_btmesh_evt_scene_server_get_id:
      app_log("evt:sl_btmesh_evt_scene_server_get_id, "
              "client_address=%u, appkey_index=%u\r\n",
              evt->data.evt_scene_server_get.client_address,
              evt->data.evt_scene_server_get.appkey_index);
      break;

    case sl_btmesh_evt_scene_server_register_get_id:
      app_log("evt:sl_btmesh_evt_scene_server_register_get_id, "
              "client_address=%u, appkey_index=%u\r\n",
              evt->data.evt_scene_server_register_get.client_address,
              evt->data.evt_scene_server_register_get.appkey_index);
      break;

    case sl_btmesh_evt_scene_server_recall_id:
      app_log("evt:sl_btmesh_evt_scene_server_recall_id, client_address=%u, "
              "appkey_index=%u, selected_scene=%u, transition_time=%lu\r\n",
              evt->data.evt_scene_server_recall.client_address,
              evt->data.evt_scene_server_recall.appkey_index,
              evt->data.evt_scene_server_recall.selected_scene,
              evt->data.evt_scene_server_recall.transition_time_ms);
      break;

    case sl_btmesh_evt_scene_server_publish_id:
      app_log("evt:sl_btmesh_evt_scene_server_publish_id, period_ms=%lu\r\n",
              evt->data.evt_scene_server_publish.period_ms);
      break;

    case sl_btmesh_evt_scene_setup_server_store_id:
      app_log("evt:sl_btmesh_evt_scene_setup_server_store_id, "
              "client_address=%u, appkey_index=%u, scene_id=%u\r\n",
              evt->data.evt_scene_setup_server_store.client_address,
              evt->data.evt_scene_setup_server_store.appkey_index,
              evt->data.evt_scene_setup_server_store.scene_id);
      break;

    case sl_btmesh_evt_scene_setup_server_delete_id:
      app_log("evt:sl_btmesh_evt_scene_setup_server_delete_id, "
              "client_address=%u, appkey_index=%u, scene_id=%u\r\n",
              evt->data.evt_scene_setup_server_delete.client_address,
              evt->data.evt_scene_setup_server_delete.appkey_index,
              evt->data.evt_scene_setup_server_delete.scene_id);
      break;

    case sl_btmesh_evt_scene_setup_server_publish_id:
      app_log("evt:sl_btmesh_evt_scene_setup_server_publish_id, "
              "period_ms=%lu\r\n",
              evt->data.evt_scene_setup_server_publish.period_ms);
      break;
    /* Generic Server Events */
    case sl_btmesh_evt_generic_server_client_request_id:
      app_log("evt:sl_btmesh_evt_generic_server_client_request_id\r\n");
      break;

    case sl_btmesh_evt_generic_server_state_recall_id:
      app_log("evt:sl_btmesh_evt_generic_server_state_recall_id\r\n");
      break;

    /* LC Events */
    case sl_btmesh_evt_lc_server_mode_updated_id:
      app_log("evt:sl_btmesh_evt_lc_server_mode_updated_id, mode=%u\r\n",
              evt->data.evt_lc_server_mode_updated.mode_value);
      break;

    case sl_btmesh_evt_lc_server_om_updated_id:
      app_log("evt:sl_btmesh_evt_lc_server_om_updated_id, om=%u\r\n",
              evt->data.evt_lc_server_om_updated.om_value);
      break;

    case sl_btmesh_evt_lc_server_light_onoff_updated_id:
      app_log("evt:sl_btmesh_evt_lc_server_light_onoff_updated_id, "
              "lc_onoff=%u, transtime=%lu\r\n",
              evt->data.evt_lc_server_light_onoff_updated.onoff_state,
              evt->data.evt_lc_server_light_onoff_updated.onoff_trans_time_ms);
      break;

    case sl_btmesh_evt_lc_server_occupancy_updated_id:
      app_log("evt:sl_btmesh_evt_lc_server_occupancy_updated_id, "
              "occupancy=%u\r\n",
              evt->data.evt_lc_server_occupancy_updated.occupancy_value);
      break;

    case sl_btmesh_evt_lc_server_ambient_lux_level_updated_id:
      app_log("evt:sl_btmesh_evt_lc_server_ambient_lux_level_updated_id, "
              "lux_level=%lu\r\n",
              evt->data.evt_lc_server_ambient_lux_level_updated.ambient_lux_level_value);
      break;

    case sl_btmesh_evt_lc_server_linear_output_updated_id:
#if defined(SL_BTMESH_LC_LINEAR_OUTPUT_LOG_ENABLE_CFG_VAL) && SL_BTMESH_LC_LINEAR_OUTPUT_LOG_ENABLE_CFG_VAL
      app_log("evt:sl_btmesh_evt_lc_server_linear_output_updated_id, "
              "linear_output=%u\r\n",
              evt->data.evt_lc_server_linear_output_updated.linear_output_value);
#endif // SL_BTMESH_LC_LINEAR_OUTPUT_LOG_ENABLE_CFG_VAL
      break;

    case sl_btmesh_evt_lc_setup_server_set_property_id:
      app_log("evt:sl_btmesh_evt_lc_setup_server_property_set_id, "
              "property=0x%4.4x, value=0x",
              evt->data.evt_lc_setup_server_set_property.property_id);
      break;

    default:
#if defined(SL_BTMESH_UNKNOWN_EVENTS_LOG_ENABLE_CFG_VAL) && SL_BTMESH_UNKNOWN_EVENTS_LOG_ENABLE_CFG_VAL
      app_log("unknown evt: %8.8x class %2.2x method %2.2x\r\n",
              evt_id,
              (evt_id >> 16) & 0xFF,
              (evt_id >> 24) & 0xFF);
#endif // SL_BTMESH_UNKNOWN_EVENTS_LOG_ENABLE_CFG_VAL
      break;
  }
}

/** @} (end addtogroup BtMeshEventLog) */
