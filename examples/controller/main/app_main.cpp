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
#include <esp_matter_commissioner.h>
#include <esp_matter_console.h>
#include <esp_matter_controller_console.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_ota.h>
#if CONFIG_ESP_MATTER_CONTROLLER_CUSTOM_CLUSTER_ENABLE
#include <matter_controller_cluster.h>
#include <matter_controller_device_mgr.h>
#endif // CONFIG_ESP_MATTER_CONTROLLER_CUSTOM_CLUSTER_ENABLE
#if CONFIG_OPENTHREAD_BORDER_ROUTER
#include <esp_matter_thread_br_console.h>
#include <esp_matter_thread_br_launcher.h>
#include <esp_ot_config.h>
#endif // CONFIG_OPENTHREAD_BORDER_ROUTER
#include <common_macros.h>
#include <app_reset.h>

#include <app/server/Server.h>
#include <credentials/FabricTable.h>

static const char *TAG = "app_main";
uint16_t switch_endpoint_id = 0;

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::PublicEventTypes::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address changed");
        break;
    case chip::DeviceLayer::DeviceEventType::kESPSystemEvent:
        if (event->Platform.ESPSystemEvent.Base == IP_EVENT &&
            event->Platform.ESPSystemEvent.Id == IP_EVENT_STA_GOT_IP) {
#if CONFIG_OPENTHREAD_BORDER_ROUTER
            esp_openthread_platform_config_t config = {
                .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
                .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
                .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
            };
            esp_matter::thread_br_init(&config);
#endif
#if !CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
            // Check whether fabric exists at index 1, if yes, set the controller fabric index to 1.
            // If you controller works on other fabrics, please set the fabric index to another value.
            if (chip::Server::GetInstance().GetFabricTable().FindFabricWithIndex(1)) {
                esp_matter::controller::set_fabric_index(1);
            }
#endif
        }
        break;
    default:
        break;
    }
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();
#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::init();
#if CONFIG_ESP_MATTER_CONTROLLER_ENABLE
    esp_matter::console::controller_register_commands();
#endif // CONFIG_ESP_MATTER_CONTROLLER_ENABLE
#if CONFIG_OPENTHREAD_BORDER_ROUTER && CONFIG_OPENTHREAD_CLI
    esp_matter::console::thread_br_cli_register_command();
#endif // CONFIG_OPENTHREAD_BORDER_ROUTER && CONFIG_OPENTHREAD_CLI
#endif // CONFIG_ENABLE_CHIP_SHELL
#if !CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    // If there is no commissioner in the controller, we need a default node so that the controller can be commissioned
    // to a specific fabric.
    node::config_t node_config;
    node_t *node = node::create(&node_config, NULL, NULL);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

#endif // !CONFIG_ESP_MATTER_COMMISSIONER_ENABLE

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to start Matter, err:%d", err));

#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    esp_matter::lock::chip_stack_lock(portMAX_DELAY);
    esp_matter::commissioner::init(5580);
    esp_matter::lock::chip_stack_unlock();
#endif // CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
}
