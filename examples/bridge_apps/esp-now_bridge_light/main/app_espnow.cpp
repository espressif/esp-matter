/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>
#include <led_driver.h>
#include <esp_matter.h>
#include "esp_mac.h"
#include "app_bridged_device.h"
#include "espnow_bridge.h"
#include "espnow.h"
#include "espnow_ctrl.h"
#include "app_espnow.h"

#ifdef CONFIG_ESPNOW_BRIDGE_APP_PS_ENABLE
#include <esp_wifi.h>
#endif

using namespace chip::app::Clusters;
using namespace esp_matter;

static const char *TAG = "app_espnow";
static bool light_status = false;
extern uint16_t light_endpoint_id;
extern uint16_t aggregator_endpoint_id;

static void espnow_ctrl_onoff(espnow_addr_t src_addr, bool status)
{
    ESP_LOGI(TAG, "ESP-NOW button pressed");

    // Update bound light
    client::request_handle_t req_handle;
    req_handle.type = esp_matter::client::INVOKE_CMD;
    req_handle.command_path.mClusterId = OnOff::Id;
    if (status) {
        req_handle.command_path.mCommandId = OnOff::Commands::On::Id;
    } else {
        req_handle.command_path.mCommandId = OnOff::Commands::Off::Id;
    }
    uint16_t bridged_switch_endpoint_id = app_bridge_get_matter_endpointid_by_espnow_macaddr(src_addr);
    ESP_LOGI(TAG, "Using bridge endpoint: %d", bridged_switch_endpoint_id);

    if (bridged_switch_endpoint_id != chip::kInvalidEndpointId) {
        lock::chip_stack_lock(portMAX_DELAY);
        client::cluster_update(bridged_switch_endpoint_id, &req_handle);
        lock::chip_stack_unlock();
    } else {
        ESP_LOGE(TAG, "Can't find endpoint for bridged device: " MACSTR, MAC2STR(src_addr));
    }

    // Update local light
    uint16_t endpoint_id = light_endpoint_id;
    uint32_t cluster_id = OnOff::Id;
    uint32_t attribute_id = OnOff::Attributes::OnOff::Id;

    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);

    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute, &val);
    if (val.type == ESP_MATTER_VAL_TYPE_BOOLEAN) {
        val.val.b = status;
        attribute::update(endpoint_id, cluster_id, attribute_id, &val);
    }
}

static void espnow_ctrl_responder_raw_data_cb(espnow_addr_t src_addr, espnow_ctrl_data_t *data, wifi_pkt_rx_ctrl_t *rx_ctrl)
{
    ESP_LOGI(TAG, "espnow_ctrl_responder_raw_data_cb, from initiator: " MACSTR
        ", initiator_attribute: %d, responder_attribute: %d, value: %d",
        MAC2STR(src_addr),
        data->initiator_attribute,
        data->responder_attribute,
        data->responder_value_i);

    light_status = !light_status;
    ESP_LOGI(TAG, "Toggle Status to %d", light_status);
    espnow_ctrl_onoff(src_addr, light_status);
}

static void espnow_ctrl_responder_data_cb(espnow_attribute_t initiator_attribute,
                                     espnow_attribute_t responder_attribute,
                                     uint32_t status)
{
    ESP_LOGI(TAG, "espnow_ctrl_responder_recv, initiator_attribute: %d, responder_attribute: %d, value: %d",
                initiator_attribute, responder_attribute, status);
}

static void espnow_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    if (base != ESP_EVENT_ESPNOW) {
        return;
    }
    // Update local light
    uint16_t endpoint_id = light_endpoint_id;
    uint32_t cluster_id = OnOff::Id;
    uint32_t attribute_id = OnOff::Attributes::OnOff::Id;

    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);

    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    switch (id) {
        case ESP_EVENT_ESPNOW_CTRL_BIND: {
            espnow_ctrl_bind_info_t *info = (espnow_ctrl_bind_info_t *)event_data;
            ESP_LOGI(TAG, "bind, uuid: " MACSTR ", initiator_type: %d", MAC2STR(info->mac), info->initiator_attribute);
            ESP_LOGI(TAG, "Create bridged switch type: 0x%04x", ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID);
            espnow_bridge_match_bridged_switch(info->mac, info->initiator_attribute, ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID);
            attribute::get_val(attribute, &val);
            val.val.b = !val.val.b;
            attribute::update(endpoint_id, cluster_id, attribute_id, &val);
            break;
        }

        case ESP_EVENT_ESPNOW_CTRL_UNBIND: {
            espnow_ctrl_bind_info_t *info = (espnow_ctrl_bind_info_t *)event_data;
            ESP_LOGI(TAG, "unbind, uuid: " MACSTR ", initiator_type: %d", MAC2STR(info->mac), info->initiator_attribute);
            espnow_bridge_remove_bridged_switch(info->mac);
            attribute::get_val(attribute, &val);
            val.val.b = !val.val.b;
            attribute::update(endpoint_id, cluster_id, attribute_id, &val);
            break;
        }

        default:
        break;
    }
    light_status = val.val.b;
}

void app_espnow_init()
{
    // ESPNOW_INIT_CONFIG_DEFAULT()
    espnow_config_t espnow_config = {
        "ESP_NOW",
        1,
        0,
        0,
        0,
        32,
        10,
        pdMS_TO_TICKS(3000),
        {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    esp_err_t ret = espnow_init(&espnow_config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "espnow init success.");
    } else {
        ESP_LOGI(TAG, "espnow init fail.");
    }

#ifdef CONFIG_ESPNOW_BRIDGE_APP_PS_ENABLE
    esp_wifi_connectionless_module_set_wake_interval(CONFIG_ESPNOW_BRIDGE_APP_WAKE_INTERVAL);
    esp_now_set_wake_window(CONFIG_ESPNOW_BRIDGE_APP_WAKE_WINDOW);
#endif

    esp_event_handler_register(ESP_EVENT_ESPNOW, ESP_EVENT_ANY_ID, espnow_event_handler, NULL);

    ESP_ERROR_CHECK(espnow_ctrl_responder_bind(30 * 60 * 1000, -55, NULL));
    // Without registering this callback, it crashes when calling the callback espnow_ctrl_responder_raw_data_cb
    espnow_ctrl_responder_data(espnow_ctrl_responder_data_cb);
    espnow_ctrl_recv(espnow_ctrl_responder_raw_data_cb);
    ESP_LOGI(TAG, "espnow init completed.");
}
