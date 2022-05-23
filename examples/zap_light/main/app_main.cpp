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

using namespace esp_matter;
using namespace esp_matter::attribute;

static const char *TAG = "app_main";
uint16_t light_endpoint_id = 0;

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
        /* Other ecosystems update */
    }

    return err;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Initialize matter callback */
    attribute::set_callback(app_attribute_update_cb, NULL);
    light_endpoint_id = 1; /* This is from zap-generated/endpoint_config.h */

    /* Initialize driver */
    app_driver_init();

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Matter start failed: %d", err);
    }
    app_qrcode_print();

    app_driver_attribute_set_defaults();

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter_console_diagnostics_register_commands();
    esp_matter_console_init();
#endif
}
