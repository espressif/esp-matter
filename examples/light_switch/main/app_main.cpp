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

#include <common_macros.h>
#include <app_priv.h>
#include <app_reset.h>
#if CONFIG_SUBSCRIBE_TO_ON_OFF_SERVER_AFTER_BINDING
#include <app/AttributePathParams.h>
#include <app/ConcreteAttributePath.h>
#include <app/clusters/bindings/binding-table.h>
#include <esp_matter_client_subscribe_command.h>
#include <lib/core/TLVReader.h>
#endif
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include <platform/ESP32/OpenthreadLauncher.h>
#endif
#if CONFIG_DYNAMIC_PASSCODE_COMMISSIONABLE_DATA_PROVIDER
#include <custom_provider/dynamic_commissionable_data_provider.h>
#endif
#if CONFIG_ENABLE_SNTP_TIME_SYNC
#include <app/clusters/time-synchronization-server/DefaultTimeSyncDelegate.h>
#endif

static const char *TAG = "app_main";
uint16_t switch_endpoint_id = 0;

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;

#if CONFIG_DYNAMIC_PASSCODE_COMMISSIONABLE_DATA_PROVIDER
dynamic_commissionable_data_provider g_dynamic_passcode_provider;
#endif

#if CONFIG_SUBSCRIBE_TO_ON_OFF_SERVER_AFTER_BINDING
static bool do_subscribe = true;

class OnOffSubscriptionDelegate final : public client::subscribe_command::delegate {
public:
    void OnAttribute(const esp_matter::client::target_t &, const chip::app::ConcreteDataAttributePath &path,
                     chip::TLV::TLVReader *, const chip::app::StatusIB &) override
    {
        if (path.mClusterId == chip::app::Clusters::OnOff::Id &&
                path.mAttributeId == chip::app::Clusters::OnOff::Attributes::OnOff::Id) {
            ESP_LOGI(TAG, "Received OnOff attribute");
        }
    }

    void OnReportEnd(const esp_matter::client::target_t &, CHIP_ERROR err) override
    {
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "OnOff subscription report failed: %s", ErrorStr(err));
        }
    }

    void OnStateChanged(const esp_matter::client::target_t &, client::subscribe_command::state_t state,
                        CHIP_ERROR error) override
    {
        ESP_LOGI(TAG, "OnOff subscription state=%u: %s",
                 static_cast<unsigned>(state), ErrorStr(error));
    }
};

static OnOffSubscriptionDelegate on_off_subscription_delegate;
static client::subscribe_command::subscription on_off_subscription;
#endif

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
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

    case chip::DeviceLayer::DeviceEventType::kBindingsChangedViaCluster: {
        ESP_LOGI(TAG, "Binding entry changed");
#if CONFIG_SUBSCRIBE_TO_ON_OFF_SERVER_AFTER_BINDING
        if (do_subscribe) {
            for (const auto  &binding : chip::app::Clusters::Binding::Table::GetInstance()) {
                ESP_LOGI(
                    TAG,
                    "Read cached binding type=%d fabrixIndex=%d nodeId=0x" ChipLogFormatX64
                    " groupId=%d local endpoint=%d remote endpoint=%d cluster=" ChipLogFormatMEI,
                    binding.type, binding.fabricIndex, ChipLogValueX64(binding.nodeId), binding.groupId, binding.local,
                    binding.remote, ChipLogValueMEI(binding.clusterId.value_or(0)));
                if (binding.type == chip::app::Clusters::Binding::MATTER_UNICAST_BINDING &&
                        event->BindingsChanged.fabricIndex == binding.fabricIndex &&
                        binding.clusterId.value_or(chip::app::Clusters::OnOff::Id) ==
                        chip::app::Clusters::OnOff::Id) {
                    ESP_LOGI(
                        TAG,
                        "Matched accessingFabricIndex with nodeId=0x" ChipLogFormatX64,
                        ChipLogValueX64(binding.nodeId));

                    chip::app::AttributePathParams path(
                        binding.remote, chip::app::Clusters::OnOff::Id,
                        chip::app::Clusters::OnOff::Attributes::OnOff::Id);
                    client::subscribe_command::options_t options;
                    options.min_interval = 5;
                    options.max_interval = 10;
                    client::subscribe_command::request_t request(
                        chip::Span<const chip::app::AttributePathParams>(&path, 1), options);
                    CHIP_ERROR err = on_off_subscription.start(
                                         chip::ScopedNodeId(binding.nodeId, binding.fabricIndex),
                                         request, on_off_subscription_delegate);
                    if (err == CHIP_NO_ERROR) {
                        do_subscribe = false;
                    } else {
                        ESP_LOGE(TAG, "OnOff subscription submission failed: %s", ErrorStr(err));
                    }
                    break;
                }
            }
        }
#endif
    }
    break;

    default:
        break;
    }
}

// This callback is invoked when clients interact with the Identify Cluster.
// In the callback implementation, an endpoint can identify itself. (e.g., by flashing an LED or light).
static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                       uint8_t effect_variant, void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: type: %u, effect: %u, variant: %u", type, effect_id, effect_variant);
    return ESP_OK;
}

// This callback is called for every attribute update. The callback implementation shall
// handle the desired attributes and return an appropriate error code. If the attribute
// is not of your interest, please do not return an error code and strictly return ESP_OK.
static esp_err_t app_attribute_update_cb(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    if (type == PRE_UPDATE) {
        /* Handle the attribute updates here. */
    }

    return ESP_OK;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Initialize driver */
    app_driver_handle_t switch_handle = app_driver_switch_init();
    app_reset_button_register(switch_handle);

    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));
#ifdef CONFIG_ENABLE_SNTP_TIME_SYNC
    endpoint_t *root_node_ep = endpoint::get_first(node);
    ABORT_APP_ON_FAILURE(root_node_ep != nullptr, ESP_LOGE(TAG, "Failed to find root node endpoint"));

    cluster::time_synchronization::config_t time_sync_cfg;
    static chip::app::Clusters::TimeSynchronization::DefaultTimeSyncDelegate time_sync_delegate;
    time_sync_cfg.delegate = &time_sync_delegate;
    cluster_t *time_sync_cluster = cluster::time_synchronization::create(root_node_ep, &time_sync_cfg, CLUSTER_FLAG_SERVER);
    ABORT_APP_ON_FAILURE(time_sync_cluster != nullptr, ESP_LOGE(TAG, "Failed to create time_sync_cluster"));

    cluster::time_synchronization::feature::time_zone::config_t tz_cfg;
    cluster::time_synchronization::feature::time_zone::add(time_sync_cluster, &tz_cfg);
#endif

    on_off_light_switch::config_t switch_config;
    endpoint_t *endpoint = on_off_light_switch::create(node, &switch_config, ENDPOINT_FLAG_NONE, switch_handle);
    ABORT_APP_ON_FAILURE(endpoint != nullptr, ESP_LOGE(TAG, "Failed to create on off switch endpoint"));

    /* Add group cluster to the switch endpoint */
    cluster::groups::config_t groups_config;
    cluster::groups::create(endpoint, &groups_config, CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);

    switch_endpoint_id = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "Switch created with endpoint_id %d", switch_endpoint_id);

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    /* Set OpenThread platform config */
    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };
    set_openthread_platform_config(&config);
#endif

#if CONFIG_DYNAMIC_PASSCODE_COMMISSIONABLE_DATA_PROVIDER
    /* This should be called before esp_matter::start() */
    esp_matter::set_custom_commissionable_data_provider(&g_dynamic_passcode_provider);
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
