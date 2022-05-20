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

#include <app_zigbee_bridge_device.h>
#include <app_qrcode.h>
#include <app_zboss.h>

#include "zigbee_bridge.h"
static const char *TAG = "app_main";

using namespace esp_matter;
using namespace esp_matter::attribute;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    if (event->Type == chip::DeviceLayer::DeviceEventType::PublicEventTypes::kInterfaceIpAddressChanged) {
        chip::app::DnssdServer::Instance().StartServer();
        esp_route_hook_init(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"));
    }
    ESP_LOGI(TAG, "Current free heap: %zu", heap_caps_get_free_size(MALLOC_CAP_8BIT));
}

static esp_err_t app_attribute_update_cb(callback_type_t type, int endpoint_id, int cluster_id, int attribute_id,
                                         esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == PRE_UPDATE) {
        err = zigbee_bridge_attribute_update(endpoint_id, cluster_id, attribute_id, val);
    }
    return err;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Create matter device */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, NULL);

    /* These node and endpoint handles can be used to create/add other endpoints and clusters. */
    if (!node) {
        ESP_LOGE(TAG, "Matter device creation failed");
    }

    /* Matter start */
    err = esp_matter::start(app_event_cb);
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
    launch_app_zboss();
}
