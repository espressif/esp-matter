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
#include <common_macros.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>
#include <esp_matter_optional_attribute.h>

#include <app/server/CommissioningWindowManager.h>
#include <app/server/Server.h>

static const char *TAG = "app_main";
uint16_t test_endpoint_id = 0;

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

constexpr auto k_timeout_seconds = 300;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address changed");
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

    case chip::DeviceLayer::DeviceEventType::kFabricRemoved: {
        ESP_LOGI(TAG, "Fabric removed successfully");
        if (chip::Server::GetInstance().GetFabricTable().FabricCount() == 0) {
            chip::CommissioningWindowManager  &commissionMgr = chip::Server::GetInstance().GetCommissioningWindowManager();
            constexpr auto kTimeoutSeconds = chip::System::Clock::Seconds16(k_timeout_seconds);
            if (!commissionMgr.IsCommissioningWindowOpen()) {
                /* After removing last fabric, this example does not remove the Wi-Fi credentials
                 * and still has IP connectivity so, only advertising on DNS-SD.
                 */
                CHIP_ERROR err = commissionMgr.OpenBasicCommissioningWindow(kTimeoutSeconds,
                                                                            chip::CommissioningWindowAdvertisement::kDnssdOnly);
                if (err != CHIP_NO_ERROR) {
                    ESP_LOGE(TAG, "Failed to open commissioning window, err:%" CHIP_ERROR_FORMAT, err.Format());
                }
            }
        }
        break;
    }

    case chip::DeviceLayer::DeviceEventType::kFabricWillBeRemoved:
        ESP_LOGI(TAG, "Fabric will be removed");
        break;

    case chip::DeviceLayer::DeviceEventType::kFabricUpdated:
        ESP_LOGI(TAG, "Fabric is updated");
        break;

    case chip::DeviceLayer::DeviceEventType::kFabricCommitted:
        ESP_LOGI(TAG, "Fabric is committed");
        break;

    case chip::DeviceLayer::DeviceEventType::kBLEDeinitialized:
        ESP_LOGI(TAG, "BLE deinitialized and memory reclaimed");
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
static esp_err_t app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == POST_UPDATE) {
        ESP_LOGI(TAG, "Attribute update for Endpoint 0x%04" PRIX16 "'s Cluster 0x%08" PRIX32 "'s Attribute 0x%08" PRIX32, endpoint_id, cluster_id, attribute_id);
    }

    return err;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node::config_t node_config;

    // node handle can be used to add/modify other endpoints.
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

    // Add optional attributes for clusters on Root Node (Endpoint 0)
    endpoint_t *root_endpoint = endpoint::get(node, 0);

    // Existing clusters on Root Node - just add optional attributes
    cluster::basic_information::create_optional_attributes(cluster::get(root_endpoint, BasicInformation::Id));
    cluster::general_diagnostics::create_optional_attributes(cluster::get(root_endpoint, GeneralDiagnostics::Id));

    // Create new clusters and their optional attributes
    // 1. Boolean State Configuration
    cluster::boolean_state_configuration::config_t bool_config;
    cluster::boolean_state_configuration::create(root_endpoint, &bool_config, CLUSTER_FLAG_SERVER);
    cluster::boolean_state_configuration::create_optional_attributes(cluster::get(root_endpoint, BooleanStateConfiguration::Id));

    // 2. Electrical Energy Measurement
    cluster::electrical_energy_measurement::config_t energy_config;
    energy_config.feature_flags = cluster::electrical_energy_measurement::feature::imported_energy::get_id() | cluster::electrical_energy_measurement::feature::cumulative_energy::get_id();
    cluster::electrical_energy_measurement::create(root_endpoint, &energy_config, CLUSTER_FLAG_SERVER);
    cluster::electrical_energy_measurement::create_optional_attributes(cluster::get(root_endpoint, ElectricalEnergyMeasurement::Id));

    // 3. Electrical Power Measurement
    cluster::electrical_power_measurement::config_t power_config;
    power_config.feature_flags = cluster::electrical_power_measurement::feature::direct_current::get_id();
    cluster::electrical_power_measurement::create(root_endpoint, &power_config, CLUSTER_FLAG_SERVER);
    cluster::electrical_power_measurement::create_optional_attributes(cluster::get(root_endpoint, ElectricalPowerMeasurement::Id));

    // 4. Ethernet Network Diagnostics
    // Note: Usually only one of Ethernet or WiFi diagnostics should be present, but for test purpose we try adding.
    cluster::ethernet_network_diagnostics::create(root_endpoint, nullptr, CLUSTER_FLAG_SERVER);
    cluster::ethernet_network_diagnostics::create_optional_attributes(cluster::get(root_endpoint, EthernetNetworkDiagnostics::Id));

    // 5. Occupancy Sensing
    cluster::occupancy_sensing::config_t occupancy_config;
    occupancy_config.feature_flags = cluster::occupancy_sensing::feature::other::get_id();
    cluster::occupancy_sensing::create(root_endpoint, &occupancy_config, CLUSTER_FLAG_SERVER);
    cluster::occupancy_sensing::create_optional_attributes(cluster::get(root_endpoint, OccupancySensing::Id));

    // 6. Resource Monitoring
    cluster::resource_monitoring::config_t resource_config; // Assuming generic config or specific like HepaFilter
    // Resource Monitoring is usually a base for specific clusters like HEPA Filter Monitoring.
    // We'll create HepaFilterMonitoring as an example of ResourceMonitoring
    cluster::hepa_filter_monitoring::create(root_endpoint, &resource_config, CLUSTER_FLAG_SERVER);
    cluster::resource_monitoring::create_optional_attributes(cluster::get(root_endpoint, HepaFilterMonitoring::Id));

    // 7. Software Diagnostics
    cluster::software_diagnostics::config_t sw_diag_config;
    cluster::software_diagnostics::create(root_endpoint, &sw_diag_config, CLUSTER_FLAG_SERVER);
    cluster::software_diagnostics::create_optional_attributes(cluster::get(root_endpoint, SoftwareDiagnostics::Id));

    // 8. Time Synchronization
    cluster::time_synchronization::config_t time_sync_config;
    cluster::time_synchronization::create(root_endpoint, &time_sync_config, CLUSTER_FLAG_SERVER);
    cluster::time_synchronization::create_optional_attributes(cluster::get(root_endpoint, TimeSynchronization::Id));

    // 9. Wifi Network Diagnostics
    // Usually created by root_node::create if config enabled, but we ensure it's here and add optionals
    if (!cluster::get(root_endpoint, WiFiNetworkDiagnostics::Id)) {
        cluster::wifi_network_diagnostics::create(root_endpoint, nullptr, CLUSTER_FLAG_SERVER);
    }
    cluster::wifi_network_diagnostics::create_optional_attributes(cluster::get(root_endpoint, WiFiNetworkDiagnostics::Id));

    // 10. Temperature Measurement
    cluster::temperature_measurement::config_t temp_config;
    cluster::temperature_measurement::create(root_endpoint, &temp_config, CLUSTER_FLAG_SERVER);
    cluster::temperature_measurement::create_optional_attributes(cluster::get(root_endpoint, TemperatureMeasurement::Id));

    // 11. Flow Measurement
    cluster::flow_measurement::config_t flow_config;
    cluster::flow_measurement::create(root_endpoint, &flow_config, CLUSTER_FLAG_SERVER);
    cluster::flow_measurement::create_optional_attributes(cluster::get(root_endpoint, FlowMeasurement::Id));

    // 12. Pressure Measurement (with Extended feature for ScaledTolerance)
    cluster::pressure_measurement::config_t pressure_config;
    cluster::pressure_measurement::create(root_endpoint, &pressure_config, CLUSTER_FLAG_SERVER);
    cluster::pressure_measurement::feature::extended::config_t extended_config;
    cluster::pressure_measurement::feature::extended::add(cluster::get(root_endpoint, PressureMeasurement::Id), &extended_config);
    cluster::pressure_measurement::create_optional_attributes(cluster::get(root_endpoint, PressureMeasurement::Id));

    // 13. Relative Humidity Measurement
    cluster::relative_humidity_measurement::config_t rh_config;
    cluster::relative_humidity_measurement::create(root_endpoint, &rh_config, CLUSTER_FLAG_SERVER);
    cluster::relative_humidity_measurement::create_optional_attributes(cluster::get(root_endpoint, RelativeHumidityMeasurement::Id));

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to start Matter, err:%d", err));

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::factoryreset_register_commands();
    esp_matter::console::attribute_register_commands();
#if CONFIG_OPENTHREAD_CLI
    esp_matter::console::otcli_register_commands();
#endif
    esp_matter::console::init();
#endif

}
