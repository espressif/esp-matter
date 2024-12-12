/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>

#include <common_macros.h>
#include <app_bridged_device.h>

#include "blemesh_bridge.h"
#include "app_blemesh.h"

static const char *TAG = "app_main";

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;

uint16_t aggregator_endpoint_id = chip::kInvalidEndpointId;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address Changed");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        break;

    case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:
        ESP_LOGI(TAG, "Commissioning failed, fail safe timer expired");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
        ESP_LOGI(TAG, "Commissioning session started");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped:
        ESP_LOGI(TAG, "Commissioning session stopped");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:
        ESP_LOGI(TAG, "Commissioning window opened");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
        ESP_LOGI(TAG, "Commissioning window closed");
        break;

    default:
        break;
    }
}

// This callback is called for every attribute update. The callback implementation shall
// handle the desired attributes and return an appropriate error code. If the attribute
// is not of your interest, please do not return an error code and strictly return ESP_OK.
static esp_err_t app_attribute_update_cb(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;
    if (type == PRE_UPDATE) {
        err = blemesh_bridge_attribute_update(endpoint_id, cluster_id, attribute_id, val, (app_bridged_device_t *)priv_data);
    }
    return err;
}

// This callback is invoked after the creation or resumption of a bridge endpoint.
//  It can be used to add data model elements (e.g., attributes, commands, etc.) to the bridge endpoint.
esp_err_t create_bridge_devices(esp_matter::endpoint_t *ep, uint32_t device_type_id, void *priv_data)
{
    esp_err_t err = ESP_OK;

    switch (device_type_id) {
    case ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID: {
        on_off_light::config_t on_off_light_conf;
        err = on_off_light::add(ep, &on_off_light_conf);
        break;
    }
    case ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID: {
        dimmable_light::config_t dimmable_light_conf;
        err = dimmable_light::add(ep, &dimmable_light_conf);
        break;
    }
    case ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID: {
        color_temperature_light::config_t color_temperature_light_conf;
        err = color_temperature_light::add(ep, &color_temperature_light_conf);
        break;
    }
    case ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID: {
        extended_color_light::config_t extended_color_light_conf;
        err = extended_color_light::add(ep, &extended_color_light_conf);
        break;
    }
    case ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID: {
        on_off_switch::config_t switch_config;
        err = on_off_switch::add(ep, &switch_config);
        break;
    }
    default: {
        ESP_LOGE(TAG, "Unsupported bridged matter device type");
        return ESP_ERR_INVALID_ARG;
    }
    }
    return err;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node::config_t node_config;
    // node handle can be used to add/modify other endpoints.
    node_t *node = node::create(&node_config, app_attribute_update_cb, NULL);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

    aggregator::config_t aggregator_config;
    endpoint_t *aggregator = endpoint::aggregator::create(node, &aggregator_config, ENDPOINT_FLAG_NONE, NULL);
    ABORT_APP_ON_FAILURE(aggregator != nullptr, ESP_LOGE(TAG, "Failed to create aggregator endpoint"));

    aggregator_endpoint_id = endpoint::get_id(aggregator);

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to start Matter, err:%d", err));

    err = app_bridge_initialize(node, create_bridge_devices);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to resume the bridged endpoints: %d", err));

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::factoryreset_register_commands();
    esp_matter::console::init();
#endif
}
