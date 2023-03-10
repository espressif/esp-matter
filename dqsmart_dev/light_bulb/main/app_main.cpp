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
#include <app_qrcode.h>
#include <app_dqsmart.h>

static const char *TAG = "app_main";
uint16_t light_endpoint_id = 0;


using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

DQSmartBulb dqsmart_bulb;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address changed");
        break;
    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        DQSTIMER::stop_timer_led();
        break;

    case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:
        ESP_LOGI(TAG, "Commissioning failed, fail safe timer expired");
        DQSTIMER::stop_timer_led();
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
        ESP_LOGI(TAG, "Commissioning session started");
        DQSTIMER::stop_timer_led();
        DQSTIMER::start_timer_led(200000);
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped:
        ESP_LOGI(TAG, "Commissioning session stopped");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:
        ESP_LOGI(TAG, "Commissioning window opened");
        DQSTIMER::start_timer_led(500000);
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
        ESP_LOGI(TAG, "Commissioning window closed");
        break;

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
        err = dqsmart_bulb.app_driver_attribute_update(driver_handle, endpoint_id, cluster_id, attribute_id, val);
    }

    return err;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();
    /*Initialize class  non pointer*/
    dqsmart_bulb = DQSmartBulb();
    /* Initialize driver */
    app_driver_handle_t light_handle  = dqsmart_bulb.Init();
    DQSTIMER::create_timer_reset_factory();
    DQSTIMER::create_timer_led();
    /*Check reset factory when on off < 4s with five count*/
    ESP_LOGI(TAG,"Opening Non-Volatile Storage (NVS) handle... ");
    err = nvs_open("storage", NVS_READWRITE, &dqsmart_bulb.my_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else {
        ESP_LOGI(TAG,"Done\n");

        // Read
        ESP_LOGI(TAG,"Reading restart counter from NVS ... ");
        dqsmart_bulb.counter_reset_factory = 0;
        err = nvs_get_i32(dqsmart_bulb.my_handle, "restart_counter", &dqsmart_bulb.counter_reset_factory);
        switch (err) {
            case ESP_OK:
                //write new value counter
                ESP_LOGI(TAG,"Updating restart counter in NVS ... ");
                dqsmart_bulb.counter_reset_factory++;
                err = nvs_set_i32(dqsmart_bulb.my_handle, "restart_counter", dqsmart_bulb.counter_reset_factory);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGI(TAG,"The value is not initialized yet!\n");
                break;
            default :
                ESP_LOGI(TAG,"Error (%s) reading!\n", esp_err_to_name(err));
        }

        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        ESP_LOGI(TAG,"Restart counter = %d\n", dqsmart_bulb.counter_reset_factory);
        DQSTIMER::start_timer_reset_factory(4000000);
        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        ESP_LOGI(TAG,"Committing updates in NVS ... ");
        err = nvs_commit(dqsmart_bulb.my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    }
    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    
    node::config_t node_config;
    node_t *node = node::create(&node_config,app_attribute_update_cb, app_identification_cb);

    color_temperature_light::config_t light_config;
    light_config.on_off.on_off = DEFAULT_POWER;
    light_config.on_off.lighting.start_up_on_off = nullptr;
    light_config.level_control.current_level = DEFAULT_BRIGHTNESS;
    light_config.level_control.lighting.start_up_current_level = DEFAULT_BRIGHTNESS;
    light_config.color_control.color_mode = EMBER_ZCL_COLOR_MODE_COLOR_TEMPERATURE;
    light_config.color_control.enhanced_color_mode = EMBER_ZCL_COLOR_MODE_COLOR_TEMPERATURE;
    light_config.color_control.color_temperature.startup_color_temperature_mireds = nullptr;
    endpoint_t *endpoint = color_temperature_light::create(node, &light_config, ENDPOINT_FLAG_NONE, light_handle);

    /* These node and endpoint handles can be used to create/add other endpoints and clusters. */
    if (!node || !endpoint) {
        ESP_LOGE(TAG, "Matter node creation failed");
    }

    light_endpoint_id = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "Light created with endpoint_id %d", light_endpoint_id);

    /* Add additional features to the node */
    cluster_t *cluster = cluster::get(endpoint, ColorControl::Id);
    cluster::color_control::feature::hue_saturation::config_t hue_saturation_config;
    hue_saturation_config.current_hue = DEFAULT_HUE;
    hue_saturation_config.current_saturation = DEFAULT_SATURATION;
    cluster::color_control::feature::hue_saturation::add(cluster, &hue_saturation_config);

    /* Matter start */

    err = esp_matter::start(app_event_cb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Matter start failed: %d", err);
    }

    /*check reset factory*/

    do 
    {
        ESP_LOGI(TAG,"Do while reset counter ... ");
        if(dqsmart_bulb.counter_reset_factory >= 3)
        {
            // Restart module
            esp_err_t err = ESP_OK;
            dqsmart_bulb.counter_reset_factory = 0;
            err = nvs_set_i32(dqsmart_bulb.my_handle, "restart_counter", dqsmart_bulb.counter_reset_factory);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
            nvs_close(dqsmart_bulb.my_handle);
            dqsmart_bulb.mSetStateResetFactory();
            esp_matter::factory_reset();
            break;
        }
        vTaskDelay(100); //1s
    }
    while (dqsmart_bulb.counter_reset_factory != 0);

    /* Starting driver with default values */
    app_qrcode_print();
    app_driver_light_set_defaults(light_endpoint_id);

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::init();
#endif
}
