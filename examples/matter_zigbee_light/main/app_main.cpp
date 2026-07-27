/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_core.h>

#include <common_macros.h>
#include <app_priv.h>

#include <app/server/CommissioningWindowManager.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/ESP32/OpenthreadLauncher.h>

#include <esp_vfs_eventfd.h>

static const char *TAG = "app_main";
uint16_t light_endpoint_id = 0;

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;
using namespace chip::DeviceLayer;

static constexpr auto k_timeout_seconds = 300;

static esp_err_t init_thread_stack_and_start_thread_task()
{
#if !CONFIG_ESP_MATTER_ENABLE_OPENTHREAD
    VerifyOrReturnError(ThreadStackMgr().InitThreadStack() == CHIP_NO_ERROR, ESP_FAIL,
                        ESP_LOGE(TAG, "Failed to initialize Thread stack"));
    VerifyOrReturnError(ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_Router) ==
                        CHIP_NO_ERROR,
                        ESP_FAIL, ESP_LOGE(TAG, "Failed to set the Thread device type"));
    VerifyOrReturnError(ThreadStackMgr().StartThreadTask() == CHIP_NO_ERROR, ESP_FAIL,
                        ESP_LOGE(TAG, "Failed to launch Thread task"));
#endif // CONFIG_ESP_MATTER_ENABLE_OPENTHREAD
    return ESP_OK;
}

static void matter_commissioning_stop_work([[maybe_unused]] intptr_t arg)
{
    chip::CommissioningWindowManager &commission_mgr =
        chip::Server::GetInstance().GetCommissioningWindowManager();
    if (commission_mgr.IsCommissioningWindowOpen()) {
        commission_mgr.CloseCommissioningWindow();
        ESP_LOGI(TAG, "Matter commissioning window closed");
    }
}

static void zigbee_network_joined_cb()
{
    LogErrorOnFailure(chip::DeviceLayer::PlatformMgr().ScheduleWork(matter_commissioning_stop_work, 0));
}

static void start_thread_stack()
{
    ESP_ERROR_CHECK(init_thread_stack_and_start_thread_task());
    ESP_LOGI(TAG, "Thread stack started, re-initializing DNS-SD");
    chip::app::DnssdServer::Instance().StartServer();
}

static void app_event_cb(const ChipDeviceEvent *event, [[maybe_unused]] intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete, running in Matter mode");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
        ESP_LOGI(TAG, "Commissioning session started, stopping Zigbee stack");
        app_zigbee_stack_stop();
        start_thread_stack();
        break;

    case chip::DeviceLayer::DeviceEventType::kFabricRemoved:
        ESP_LOGI(TAG, "Fabric removed successfully");
        if (chip::Server::GetInstance().GetFabricTable().FabricCount() == 0) {
            chip::CommissioningWindowManager &commission_mgr =
                chip::Server::GetInstance().GetCommissioningWindowManager();
            constexpr auto k_timeout = chip::System::Clock::Seconds16(k_timeout_seconds);
            if (!commission_mgr.IsCommissioningWindowOpen()) {
                CHIP_ERROR err = commission_mgr.OpenBasicCommissioningWindow(
                                     k_timeout, chip::CommissioningWindowAdvertisement::kDnssdOnly);
                if (err != CHIP_NO_ERROR) {
                    ESP_LOGE(TAG, "Failed to open commissioning window, err:%" CHIP_ERROR_FORMAT, err.Format());
                }
            }
        }
        break;

    default:
        break;
    }
}

static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                       uint8_t effect_variant, [[maybe_unused]] void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: ep: %u, type: %u, effect: %u, variant: %u", endpoint_id, type, effect_id, effect_variant);
    return ESP_OK;
}

static esp_err_t app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    if (type != PRE_UPDATE) {
        return ESP_OK;
    }

    return app_driver_attribute_update((app_driver_handle_t)priv_data, endpoint_id, cluster_id, attribute_id, val);
}

static void start_matter_mode()
{
    ESP_LOGI(TAG, "Running in Matter mode");
    start_thread_stack();
}

static void init_platform()
{
    esp_vfs_eventfd_config_t eventfd_config = {
        .max_fds = 7,
    };
    ESP_ERROR_CHECK(esp_vfs_eventfd_register(&eventfd_config));
    ESP_ERROR_CHECK(nvs_flash_init());
}

static void init_drivers(app_driver_handle_t *light_handle, app_driver_handle_t *button_handle)
{
    *light_handle = app_driver_light_init();
    *button_handle = app_driver_button_init();
    app_reset_button_register(*button_handle);
}

static void init_matter_data_model(app_driver_handle_t light_handle)
{
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

    extended_color_light::config_t light_config;
    light_config.on_off.on_off = DEFAULT_POWER;
    light_config.on_off_lighting.start_up_on_off = nullptr;
    light_config.level_control.current_level = DEFAULT_BRIGHTNESS;
    light_config.level_control.on_level = DEFAULT_BRIGHTNESS;
    light_config.level_control_lighting.start_up_current_level = DEFAULT_BRIGHTNESS;
    light_config.color_control.color_mode = (uint8_t)ColorControl::ColorMode::kColorTemperature;
    light_config.color_control.enhanced_color_mode = (uint8_t)ColorControl::ColorMode::kColorTemperature;
    light_config.color_control_color_temperature.start_up_color_temperature_mireds = nullptr;

    endpoint_t *endpoint =
        extended_color_light::create(node, &light_config, ENDPOINT_FLAG_NONE, light_handle);
    ABORT_APP_ON_FAILURE(endpoint != nullptr, ESP_LOGE(TAG, "Failed to create extended color light endpoint"));

    light_endpoint_id = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "Light created with endpoint_id %d", light_endpoint_id);

    attribute::set_deferred_persistence(
        attribute::get(light_endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id));
    attribute::set_deferred_persistence(
        attribute::get(light_endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentX::Id));
    attribute::set_deferred_persistence(
        attribute::get(light_endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentY::Id));
    attribute::set_deferred_persistence(
        attribute::get(light_endpoint_id, ColorControl::Id, ColorControl::Attributes::ColorTemperatureMireds::Id));
}

static void init_openthread_platform()
{
    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };
    set_openthread_platform_config(&config);
}

static void init_matter_stack()
{
    ESP_ERROR_CHECK(esp_matter::start(app_event_cb));
}

extern "C" void app_main()
{
    init_platform();

    app_driver_handle_t light_handle;
    app_driver_handle_t button_handle;
    init_drivers(&light_handle, &button_handle);

    app_zigbee_register_network_joined_cb(zigbee_network_joined_cb);

    init_matter_data_model(light_handle);
    init_openthread_platform();
    init_matter_stack();

    app_driver_light_set_defaults(light_endpoint_id);

    if (chip::Server::GetInstance().GetFabricTable().FabricCount() > 0) { // Matter is already commissioned
        start_matter_mode();
    } else {
        app_zigbee_stack_start();
    }
}
