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
#include <esp_matter_providers.h>

#include <app_priv.h>

#include <common_macros.h>

#include <app/clusters/bindings/BindingManager.h>
#include <app/server/Server.h>
#include <platform/CHIPDeviceLayer.h>

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include <platform/ESP32/OpenthreadLauncher.h>
#endif

static const char *TAG = "app_main";
uint16_t switch_endpoint_id = 0;

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        break;
    default:
        break;
    }
}

static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                       uint8_t effect_variant, void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: type: %u, effect: %u, variant: %u", type, effect_id, effect_variant);
    return ESP_OK;
}

static esp_err_t app_attribute_update_cb(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    if (type == PRE_UPDATE) {
        app_driver_handle_t driver_handle = (app_driver_handle_t)priv_data;
        app_driver_attribute_update(driver_handle, endpoint_id, cluster_id, attribute_id, val);
    }
    return ESP_OK;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Initialize driver */
    app_driver_handle_t driver_handle = app_driver_init();
    // Factory reset via button disabled - use console command if needed

    /* Create a Matter node */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

    /* Create Dimmable Light Endpoint (Server Clusters: OnOff, LevelControl, Groups, Scenes, Descriptor, Identify) */
    dimmable_light::config_t dimmable_light_config;
    dimmable_light_config.on_off.on_off = true; // Default On
    dimmable_light_config.level_control.current_level = 254; // Default Max brightness
    dimmable_light_config.level_control.on_level = 254;

    endpoint_t *endpoint =
        dimmable_light::create(node, &dimmable_light_config, ENDPOINT_FLAG_NONE, driver_handle);
    ABORT_APP_ON_FAILURE(endpoint != nullptr, ESP_LOGE(TAG, "Failed to create dimmable light endpoint"));

    switch_endpoint_id = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "Dimmable Light created with endpoint_id %d", switch_endpoint_id);

    /* Add Dimmer Switch device type to the same endpoint (creates a composite device) */
    dimmer_switch::config_t dimmer_switch_config;
    err = dimmer_switch::add(endpoint, &dimmer_switch_config);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to add dimmer switch device type"));
    ESP_LOGI(TAG, "Added Dimmer Switch device type to endpoint %d", switch_endpoint_id);

    /* Add Client Clusters for sending commands to bound devices */
    cluster::on_off::config_t on_off_client_config;
    cluster::on_off::create(endpoint, &on_off_client_config, CLUSTER_FLAG_CLIENT);

    cluster::level_control::config_t level_control_client_config;
    cluster::level_control::create(endpoint, &level_control_client_config, CLUSTER_FLAG_CLIENT);

    cluster::identify::config_t identify_client_config;
    cluster::identify::create(endpoint, &identify_client_config, CLUSTER_FLAG_CLIENT);

    /* Add Binding Cluster (Server) - Required to store bindings for client clusters */
    cluster::binding::config_t binding_config;
    cluster::binding::create(endpoint, &binding_config, CLUSTER_FLAG_SERVER);

    /* Initialize Binding Manager (Required for sending commands to bound devices) */
    // Note: BindingManager initialization is typically handled internally or lazily,
    // but ensuring the structure is ready for the driver is key.

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };
    set_openthread_platform_config(&config);
#endif

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to start Matter, err:%d", err));

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::factoryreset_register_commands();
    esp_matter::console::init();
#endif
}
