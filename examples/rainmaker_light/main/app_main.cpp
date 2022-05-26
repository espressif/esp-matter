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
#include <esp_route_hook.h>

#include <app_ble.h>
#include <app_priv.h>
#include <app_qrcode.h>
#include <app_rainmaker.h>

static const char *TAG = "app_main";
uint16_t light_endpoint_id = 0;

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::PublicEventTypes::kInterfaceIpAddressChanged:
#if !CHIP_DEVICE_CONFIG_ENABLE_THREAD
        chip::app::DnssdServer::Instance().StartServer();
        esp_route_hook_init(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"));
#endif
        break;

    case chip::DeviceLayer::DeviceEventType::PublicEventTypes::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        app_ble_disable();
        break;

    default:
        break;
    }
}

static esp_err_t app_attribute_update_cb(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == PRE_UPDATE) {
        /* Driver update */
        err = app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, val);
    } else if (type == POST_UPDATE) {
        /* Rainmaker update */
        err = app_rainmaker_attribute_update(endpoint_id, cluster_id, attribute_id, val);
    }

    return err;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Create a Matter node */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, NULL);

    color_temperature_light::config_t light_config;
    light_config.on_off.on_off = DEFAULT_POWER;
    light_config.level_control.current_level = DEFAULT_BRIGHTNESS;
    endpoint_t *endpoint = color_temperature_light::create(node, &light_config, ENDPOINT_FLAG_NONE);

    /* These node and endpoint handles can be used to create/add other endpoints and clusters. */
    if (!node || !endpoint) {
        ESP_LOGE(TAG, "Matter node creation failed");
    }

    light_endpoint_id = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "Light created with endpoint_id %d", light_endpoint_id);

    /* Add additional features to the node */
    cluster_t *cluster = cluster::get(endpoint, ColorControl::Id);
    color_control::feature::hue_saturation::config_t hue_saturation_config;
    hue_saturation_config.current_hue = DEFAULT_HUE;
    hue_saturation_config.current_saturation = DEFAULT_SATURATION;
    color_control::feature::hue_saturation::add(cluster, &hue_saturation_config);

    /* Initialize driver */
    app_driver_init();

    /* Initialize rainmaker */
    app_rainmaker_init();

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Matter start failed: %d", err);
    }
    app_qrcode_print();

    /* Start rainmaker */
    app_rainmaker_start();

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter_console_diagnostics_register_commands();
    esp_matter_console_init();
#endif
}
