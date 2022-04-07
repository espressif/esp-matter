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
#include <esp_route_hook.h>

#include <app_priv.h>
#include <app_qrcode.h>

static const char *TAG = "app_main";
int light_endpoint_id = 0;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    if (event->Type == chip::DeviceLayer::DeviceEventType::PublicEventTypes::kInterfaceIpAddressChanged) {
#if !CHIP_DEVICE_CONFIG_ENABLE_THREAD
        chip::app::DnssdServer::Instance().StartServer();
        esp_route_hook_init(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"));
#endif
    }
    ESP_LOGI(TAG, "Current free heap: %zu", heap_caps_get_free_size(MALLOC_CAP_8BIT));
}

static esp_err_t app_attribute_update_cb(esp_matter_callback_type_t type, int endpoint_id, int cluster_id,
                                         int attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == ESP_MATTER_CALLBACK_TYPE_PRE_ATTRIBUTE) {
        /* Driver update */
        err = app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, val);
    } else if (type == ESP_MATTER_CALLBACK_TYPE_POST_ATTRIBUTE) {
        /* Other ecosystems update */
    }

    return err;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Create matter device */
    esp_matter_node_config_t node_config = NODE_CONFIG_DEFAULT();
    esp_matter_node_t *node = esp_matter_node_create(&node_config, app_attribute_update_cb, NULL);

    esp_matter_endpoint_color_dimmable_light_config_t light_config = ENDPOINT_CONFIG_COLOR_DIMMABLE_LIGHT_DEFAULT();
    light_config.on_off.on_off = DEFAULT_POWER;
    light_config.level_control.current_level = DEFAULT_BRIGHTNESS;
    light_config.color_control.hue_saturation.current_hue = DEFAULT_HUE;
    light_config.color_control.hue_saturation.current_saturation = DEFAULT_SATURATION;
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_create_color_dimmable_light(node, &light_config,
                                                                                      ESP_MATTER_ENDPOINT_FLAG_NONE);
    light_endpoint_id = esp_matter_endpoint_get_id(endpoint);

    /* These node and endpoint handles can be used to create/add other endpoints and clusters. */
    if (!node || !endpoint) {
        ESP_LOGE(TAG, "Matter device creation failed");
    }
    ESP_LOGI(TAG, "Light created with endpoint_id %d", light_endpoint_id);

    /* Initialize driver */
    app_driver_init();

    /* Matter start */
    err = esp_matter_start(app_event_cb);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Matter start failed: %d", err);
    }
    app_qrcode_print();

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter_console_diagnostics_register_commands();
    esp_matter_console_init();
#endif

#if CONFIG_ENABLE_OTA_REQUESTOR
    esp_matter_ota_requestor_init();
#endif
}
