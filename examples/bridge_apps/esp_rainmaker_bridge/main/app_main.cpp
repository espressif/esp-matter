/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <app_bridged_device.h>
#include <app_network.h>
#include <app_rainmaker_matter_mapping.h>
#include <app_rainmaker_bridged_device.h>
#include <common_macros.h>
#include <rainmaker_controller.h>

#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>
#include <esp_rmaker_core.h>
#include <nvs_flash.h>

#if CONFIG_OPENTHREAD_BORDER_ROUTER
#include <app_thread_config.h>
#include <esp_rmaker_thread_br.h>
#include <platform/ESP32/OpenthreadLauncher.h>
#ifdef CONFIG_AUTO_UPDATE_RCP
#include <esp_ot_rcp_update.h>
#include <esp_rcp_update.h>
#endif
#endif

static const char *TAG = "app_main";

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;

uint16_t aggregator_endpoint_id = chip::kInvalidEndpointId;

static void app_event_cb(const ChipDeviceEvent *event, [[maybe_unused]] intptr_t arg)
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
                                         uint32_t attribute_id, esp_matter_attr_val_t *val,
                                         [[maybe_unused]] void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == PRE_UPDATE) {
        err = app_map_matter_attribute_to_rainmaker(endpoint_id, cluster_id, attribute_id, val);
    }
    return err;
}

// This callback is invoked after the creation or resumption of a bridge endpoint.
// It can be used to add data model elements (e.g., attributes, commands, etc.) to the bridge endpoint.
esp_err_t create_bridge_devices(esp_matter::endpoint_t *ep, uint32_t device_type_id,
                                [[maybe_unused]] void *priv_data)
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
        dimmable_light_conf.level_control_lighting.start_up_current_level = nullptr;
        err = dimmable_light::add(ep, &dimmable_light_conf);
        break;
    }
    case ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID: {
        color_temperature_light::config_t color_temperature_light_conf;
        color_temperature_light_conf.level_control_lighting.start_up_current_level = nullptr;
        err = color_temperature_light::add(ep, &color_temperature_light_conf);
        break;
    }
    case ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID: {
        extended_color_light::config_t extended_color_light_conf;
        extended_color_light_conf.level_control_lighting.start_up_current_level = nullptr;
        err = extended_color_light::add(ep, &extended_color_light_conf);
        cluster_t *color_cluster = cluster::get(ep, chip::app::Clusters::ColorControl::Id);
        cluster::color_control::feature::hue_saturation::config_t hs_config;
        cluster::color_control::feature::hue_saturation::add(color_cluster, &hs_config);
        break;
    }
    case ESP_MATTER_ON_OFF_LIGHT_SWITCH_DEVICE_TYPE_ID: {
        on_off_light_switch::config_t switch_config;
        err = on_off_light_switch::add(ep, &switch_config);
        break;
    }
    default: {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_ARG, TAG, "Unsupported bridged matter device type");
    }
    }
    return err;
}

static app_bridged_device_t *create_rainmaker_bridged_device([[maybe_unused]] node_t *node,
                                                             [[maybe_unused]] uint16_t endpoint)
{
    return chip::Platform::New<app_rainmaker_bridged_device_t>();
}

static void free_rainmaker_bridged_device(app_bridged_device_t *device)
{
    chip::Platform::Delete((app_rainmaker_bridged_device_t *)device);
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    app_network_init();

    esp_rmaker_config_t rainmaker_cfg = {
        .enable_time_sync = false,
    };
    esp_rmaker_node_t *rainmaker_node = esp_rmaker_node_init(&rainmaker_cfg, "ESP RainMaker Device", "RainmakerController");
    ABORT_APP_ON_FAILURE(rainmaker_node != nullptr, ESP_LOGE(TAG, "Failed to create RainMaker node"));

    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, nullptr);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

    aggregator::config_t aggregator_config;
    endpoint_t *aggregator = endpoint::aggregator::create(node, &aggregator_config, ENDPOINT_FLAG_NONE, nullptr);
    ABORT_APP_ON_FAILURE(aggregator != nullptr, ESP_LOGE(TAG, "Failed to create aggregator endpoint"));

    aggregator_endpoint_id = endpoint::get_id(aggregator);

    rainmaker_controller_config_t rainmaker_controller_cfg = {
        .group_nodes_cb = app_map_rainmaker_nodes_to_matter,
        .node_params_cb = app_map_rainmaker_node_params_to_matter,
        .node_connectivity_cb = app_map_rainmaker_node_connectivity_to_matter,
    };
    err = rainmaker_controller_init(rainmaker_node, &rainmaker_controller_cfg);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to initialize RainMaker controller, err:%d", err));

#if CONFIG_OPENTHREAD_BORDER_ROUTER
    esp_openthread_platform_config_t thread_cfg = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG()
    };
#ifdef CONFIG_AUTO_UPDATE_RCP
    esp_rcp_update_config_t rcp_update_cfg = ESP_OPENTHREAD_RCP_UPDATE_CONFIG();
    esp_rcp_update_init(&rcp_update_cfg);
    esp_ot_register_rcp_handler();
#endif // CONFIG_AUTO_UPDATE_RCP
    esp_rmaker_thread_br_enable(&thread_cfg);
#endif // CONFIG_OPENTHREAD_BORDER_ROUTER

    esp_rmaker_start();

    err = app_network_start(POP_TYPE_RANDOM);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to start network, err:%d", err));

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to start Matter, err:%d", err));

    err = app_bridge_initialize(node, create_bridge_devices, create_rainmaker_bridged_device,
                                free_rainmaker_bridged_device);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to resume the bridged endpoints: %d", err));

    rainmaker_controller_start();

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::factoryreset_register_commands();
    esp_matter::console::init();
#endif
}
