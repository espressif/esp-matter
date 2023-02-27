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

#include <app_priv.h>
#include <app_reset.h>
#include <app_dqsmart.h>

static const char *TAG = "app_main";
uint16_t light_endpoint_id = 0;


using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;



static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address changed");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        DQSLED::stop_timer_led_status();
        break;

    case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:
        ESP_LOGI(TAG, "Commissioning failed, fail safe timer expired");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
        ESP_LOGI(TAG, "Commissioning session started");
        DQSLED::stop_timer_led_status();
        DQSLED::start_timer_led_status(50000);
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped:
        ESP_LOGI(TAG, "Commissioning session stopped");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:
        ESP_LOGI(TAG, "Commissioning window opened");
        DQSLED::start_timer_led_status(500000);
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
        ESP_LOGI(TAG, "Commissioning window closed");
    case chip::DeviceLayer::DeviceEventType::kESPSystemEvent:
    {
        if (event->Platform.ESPSystemEvent.Base == WIFI_EVENT)
        {
            // switch (event->Platform.ESPSystemEvent.Id)
            // {
            //     case WIFI_EVENT_STA_DISCONNECTED:
            //         ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
            //         DQSLED::stop_timer_led_status();
            //         DQSLED::start_timer_led_status(100000);
            //         break;
            //     case WIFI_EVENT_STA_CONNECTED:
            //         ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
            //         DQSLED::stop_timer_led_status();
            //         break;
            // }
        }
    
        break;
    }    
    default:
        break;
    }
}

static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                       void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: type: %d, effect: %d", type, effect_id);
    return ESP_OK;
}

static esp_err_t app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == PRE_UPDATE) {
        /* Driver update */
        app_driver_handle_t driver_handle = (app_driver_handle_t)priv_data;
        err = app_driver_attribute_update(driver_handle, endpoint_id, cluster_id, attribute_id, val);
    }

    return err;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Initialize driver */
    app_driver_handle_t light_handle = app_driver_light_init();
    app_driver_handle_t button_handle = app_driver_button_init();
    app_reset_button_register(button_handle);

    /*Call control led status*/
    DQSLED::create_timer_led_status(light_handle);
    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    DQSLED::set_led_status_off();
    on_off_light::config_t  light_config;
    light_config.on_off.on_off = DEFAULT_POWER;
#if THREE_BUTTON           
    endpoint_t *endpoint = on_off_light::create(node, &light_config, ENDPOINT_FLAG_NONE, light_handle);
    endpoint_t *endpoint1 = on_off_light::create(node, &light_config, ENDPOINT_FLAG_NONE, light_handle);
    endpoint_t *endpoint2 = on_off_light::create(node, &light_config, ENDPOINT_FLAG_NONE, light_handle);
#else
    endpoint_t *endpoint = on_off_light::create(node, &light_config, ENDPOINT_FLAG_NONE, light_handle);
    endpoint_t *endpoint1 = on_off_light::create(node, &light_config, ENDPOINT_FLAG_NONE, light_handle);
#endif
    /* These node and endpoint handles can be used to create/add other endpoints and clusters. */
    if (!node || !endpoint) {
        ESP_LOGE(TAG, "Matter node creation failed");
    }

    light_endpoint_id = endpoint::get_id(endpoint);

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Matter start failed: %d", err);
    }

    /* Starting driver with default values */
    app_driver_light_set_defaults(light_endpoint_id);
    app_driver_light_set_defaults(light_endpoint_id + 1);

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::init();
#endif
}
