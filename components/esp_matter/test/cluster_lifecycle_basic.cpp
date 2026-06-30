/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unity.h>
#include <esp_heap_caps.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <esp_matter_data_model.h>
#include <unity_test_utils_memory.h>

#include "cluster_lifecycle_common.h"

#include <app-common/zap-generated/ids/Clusters.h>
#include <clusters/ElectricalEnergyMeasurement/Enums.h>
#include <clusters/OccupancySensing/Enums.h>
#include <clusters/shared/Enums.h>
#include <clusters/soil_measurement/integration.h>

using namespace esp_matter;

static constexpr uint32_t k_reenable_iterations = 3;
static constexpr uint32_t k_unique_endpoint_iterations = 3;

template <typename Config>
static void create_disable_enable_destroy_cluster_endpoint(node_t *node, uint16_t endpoint_id,
                                                           chip::ClusterId cluster_id,
                                                           cluster_t *(*create_cluster)(endpoint_t *, Config *, uint8_t),
                                                           void (*after_enable)(uint16_t) = nullptr)
{
    endpoint_t *endpoint = endpoint::resume(node, ENDPOINT_FLAG_DESTROYABLE, endpoint_id, nullptr);
    TEST_ASSERT_NOT_NULL(endpoint);
    TEST_ASSERT_EQUAL(endpoint_id, endpoint::get_id(endpoint));

    Config config;
    cluster_t *cluster = create_cluster(endpoint, &config, CLUSTER_FLAG_SERVER);
    TEST_ASSERT_NOT_NULL(cluster);

    esp_err_t err = endpoint::enable(endpoint);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    if (after_enable) {
        after_enable(endpoint_id);
    }

    for (uint32_t reenable_index = 0; reenable_index < k_reenable_iterations; ++reenable_index) {
        err = endpoint::disable(endpoint);
        TEST_ASSERT_EQUAL(ESP_OK, err);
        TEST_ASSERT_EQUAL_PTR(cluster, cluster::get(endpoint, cluster_id));

        err = endpoint::enable(endpoint);
        TEST_ASSERT_EQUAL(ESP_OK, err);
        TEST_ASSERT_EQUAL_PTR(cluster, cluster::get(endpoint, cluster_id));
    }

    err = endpoint::destroy(node, endpoint);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NULL(endpoint::get(node, endpoint_id));
}

template <typename Config>
static void verify_cluster_lifecycle_no_heap_leak(chip::ClusterId cluster_id,
                                                  cluster_t *(*create_cluster)(endpoint_t *, Config *, uint8_t),
                                                  void (*after_enable)(uint16_t) = nullptr)
{
    node_t *node = test::get_or_create_node();
    test::start_matter_if_needed();

    uint16_t warm_endpoint_ids[k_unique_endpoint_iterations];
    uint16_t measured_endpoint_ids[k_unique_endpoint_iterations];
    for (uint32_t endpoint_index = 0; endpoint_index < k_unique_endpoint_iterations; ++endpoint_index) {
        warm_endpoint_ids[endpoint_index] = test::reserve_destroyable_endpoint_id(node);
        measured_endpoint_ids[endpoint_index] = test::reserve_destroyable_endpoint_id(node);
    }

    // Warm per-endpoint allocator paths using throwaway endpoint ids. The measured pass uses different ids, so stale
    // provider map entries left by kPermanentRemove accumulate and are visible as heap loss.
    for (uint32_t endpoint_index = 0; endpoint_index < k_unique_endpoint_iterations; ++endpoint_index) {
        create_disable_enable_destroy_cluster_endpoint(node, warm_endpoint_ids[endpoint_index], cluster_id, create_cluster,
                                                       after_enable);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

    unity_utils_record_free_mem();

    for (uint32_t endpoint_index = 0; endpoint_index < k_unique_endpoint_iterations; ++endpoint_index) {
        create_disable_enable_destroy_cluster_endpoint(node, measured_endpoint_ids[endpoint_index], cluster_id, create_cluster,
                                                       after_enable);
    }

    unity_utils_evaluate_leaks_direct(0);
}

// -----------------------------------------------------------------------------
// SCOPE
// -----------------------------------------------------------------------------

// Covers endpoint enable/disable/destroy lifecycle for cluster init/shutdown callbacks implemented under
// components/esp_matter/data_model_provider/clusters. The test keeps cluster metadata alive across disable/enable and
// verifies permanent destroy releases provider-owned state without heap leaks.

// -----------------------------------------------------------------------------
// IGNORED
// -----------------------------------------------------------------------------

// Empty generated/provider init and shutdown callbacks
// keep-sorted start sticky_comments=no
// - Chime
// - DeviceEnergyManagement
// - ElectricalPowerMeasurement
// - EnergyEvse
// - PowerTopology
// - WebRTCTransportProvider
// - WebRTCTransportRequestor
// keep-sorted end

// Root endpoint singleton providers. These clusters are initialized as node-wide/root endpoint services, so a
// destroyable dynamic endpoint does not exercise their real ownership or teardown path.
// keep-sorted start sticky_comments=no
// - AccessControl
// - AdministratorCommissioning
// - BasicInformation
// - DiagnosticLogs
// - GeneralCommissioning
// - GeneralDiagnostics
// - GroupKeyManagement
// - Groupcast
// - IcdManagement
// - LocalizationConfiguration
// - OperationalCredentials
// - SoftwareDiagnostics
// - TimeFormatLocalization
// - TimeSynchronization
// - UnitLocalization
// keep-sorted end

// Requires an application delegate.
// keep-sorted start sticky_comments=no
// - CameraAvStreamManagement
// - ClosureControl
// - TlsCertificateManagement
// - TlsClientManagement
// - WebRTCTransportProvider
// keep-sorted end

// Uses unordered_map that retains entries after endpoint destroy — leaks by design.
// - ClosureDimension
// - CommissionerControl
// - ZoneManagement

// Requires an upstream server constructor that is not linked into this unit-test app build.
// keep-sorted start sticky_comments=no
// - ThreadNetworkDiagnostics
// keep-sorted end

// Shared provider implementation exercised by the concrete Resource Monitoring clusters below.
// keep-sorted start sticky_comments=no
// - ResourceMonitor: ActivatedCarbonFilterMonitoring, HepaFilterMonitoring
// keep-sorted end

// -----------------------------------------------------------------------------
// TESTS
// -----------------------------------------------------------------------------

#define TEST_CLUSTER_LIFECYCLE(CLUSTER_NAME, CLUSTER_ID, CONFIG_TYPE, CREATE_FN) \
    TEST_CASE("endpoint lifecycle preserves and frees " CLUSTER_NAME " cluster", "[cluster_lifecycle][basic]") \
    { \
        verify_cluster_lifecycle_no_heap_leak<CONFIG_TYPE>(CLUSTER_ID, CREATE_FN); \
    }

static cluster_t *create_electrical_energy_measurement_cluster(endpoint_t *endpoint,
                                                               cluster::electrical_energy_measurement::config_t *config,
                                                               uint8_t flags)
{
    config->feature_flags = static_cast<uint32_t>
                            (chip::app::Clusters::ElectricalEnergyMeasurement::Feature::kImportedEnergy) |
                            static_cast<uint32_t>(chip::app::Clusters::ElectricalEnergyMeasurement::Feature::kCumulativeEnergy);
    return cluster::electrical_energy_measurement::create(endpoint, config, flags);
}

static cluster_t *create_occupancy_sensing_cluster(endpoint_t *endpoint, cluster::occupancy_sensing::config_t *config,
                                                   uint8_t flags)
{
    config->occupancy_sensor_type = 0;
    config->occupancy_sensor_type_bitmap =
        static_cast<uint8_t>(chip::app::Clusters::OccupancySensing::OccupancySensorTypeBitmap::kPir);
    config->feature_flags = static_cast<uint32_t>(chip::app::Clusters::OccupancySensing::Feature::kPassiveInfrared);
    return cluster::occupancy_sensing::create(endpoint, config, flags);
}

static cluster_t *create_power_source_cluster(endpoint_t *endpoint, cluster::power_source::config_t *config,
                                              uint8_t flags)
{
    config->feature_flags = cluster::power_source::feature::wired::get_id();
    return cluster::power_source::create(endpoint, config, flags);
}

static cluster_t *create_soil_measurement_cluster(endpoint_t *endpoint, cluster::soil_measurement::config_t *config,
                                                  uint8_t flags)
{
    chip::app::Clusters::SoilMeasurement::Attributes::SoilMoistureMeasurementLimits::TypeInfo::Type limits;
    limits.measurementType = chip::app::Clusters::Globals::MeasurementTypeEnum::kSoilMoisture;
    limits.measured = true;
    limits.minMeasuredValue = 0;
    limits.maxMeasuredValue = 100;
    chip::app::Clusters::SoilMeasurement::SetSoilMoistureLimits(endpoint::get_id(endpoint), limits);

    return cluster::soil_measurement::create(endpoint, config, flags);
}

static cluster_t *create_closure_dimension_cluster(endpoint_t *endpoint, cluster::closure_dimension::config_t *config,
                                                   uint8_t flags)
{
    config->feature_flags = cluster::closure_dimension::feature::positioning::get_id() |
                            cluster::closure_dimension::feature::translation::get_id();
    return cluster::closure_dimension::create(endpoint, config, flags);
}

static cluster_t *create_microwave_oven_control_cluster(endpoint_t *endpoint,
                                                        cluster::microwave_oven_control::config_t *config, uint8_t flags)
{
    config->feature_flags = cluster::microwave_oven_control::feature::power_as_number::get_id();
    return cluster::microwave_oven_control::create(endpoint, config, flags);
}

static cluster_t *create_switch_cluster(endpoint_t *endpoint, cluster::switch_cluster::config_t *config, uint8_t flags)
{
    config->number_of_positions = 2;
    config->feature_flags = cluster::switch_cluster::feature::latching_switch::get_id();
    return cluster::switch_cluster::create(endpoint, config, flags);
}

static cluster_t *create_temperature_control_cluster(endpoint_t *endpoint,
                                                     cluster::temperature_control::config_t *config,
                                                     uint8_t flags)
{
    config->features.temperature_number.min_temperature = 0;
    config->features.temperature_number.temp_setpoint = 50;
    config->features.temperature_number.max_temperature = 100;
    config->feature_flags = cluster::temperature_control::feature::temperature_number::get_id();
    return cluster::temperature_control::create(endpoint, config, flags);
}

TEST_CLUSTER_LIFECYCLE("ActivatedCarbonFilterMonitoring", chip::app::Clusters::ActivatedCarbonFilterMonitoring::Id,
                       cluster::activated_carbon_filter_monitoring::config_t,
                       cluster::activated_carbon_filter_monitoring::create)
TEST_CLUSTER_LIFECYCLE("AirQuality", chip::app::Clusters::AirQuality::Id, cluster::air_quality::config_t,
                       cluster::air_quality::create)
TEST_CLUSTER_LIFECYCLE("Binding", chip::app::Clusters::Binding::Id, cluster::binding::config_t,
                       cluster::binding::create)
TEST_CLUSTER_LIFECYCLE("BooleanState", chip::app::Clusters::BooleanState::Id,
                       cluster::boolean_state::config_t, cluster::boolean_state::create)
TEST_CLUSTER_LIFECYCLE("BooleanStateConfiguration", chip::app::Clusters::BooleanStateConfiguration::Id,
                       cluster::boolean_state_configuration::config_t, cluster::boolean_state_configuration::create)
TEST_CLUSTER_LIFECYCLE("BridgedDeviceBasicInformation", chip::app::Clusters::BridgedDeviceBasicInformation::Id,
                       cluster::bridged_device_basic_information::config_t, cluster::bridged_device_basic_information::create)
// ClosureDimension: see IGNORED section (unordered_map leak)
// CommissionerControl: see IGNORED section (unordered_map leak)
TEST_CLUSTER_LIFECYCLE("Descriptor", chip::app::Clusters::Descriptor::Id, cluster::descriptor::config_t,
                       cluster::descriptor::create)
TEST_CLUSTER_LIFECYCLE("ElectricalEnergyMeasurement", chip::app::Clusters::ElectricalEnergyMeasurement::Id,
                       cluster::electrical_energy_measurement::config_t, create_electrical_energy_measurement_cluster)
TEST_CLUSTER_LIFECYCLE("EthernetNetworkDiagnostics", chip::app::Clusters::EthernetNetworkDiagnostics::Id,
                       cluster::ethernet_network_diagnostics::config_t, cluster::ethernet_network_diagnostics::create)
TEST_CLUSTER_LIFECYCLE("FixedLabel", chip::app::Clusters::FixedLabel::Id, cluster::fixed_label::config_t,
                       cluster::fixed_label::create)
TEST_CLUSTER_LIFECYCLE("FlowMeasurement", chip::app::Clusters::FlowMeasurement::Id,
                       cluster::flow_measurement::config_t, cluster::flow_measurement::create)
TEST_CLUSTER_LIFECYCLE("Groups", chip::app::Clusters::Groups::Id, cluster::groups::config_t, cluster::groups::create)
TEST_CLUSTER_LIFECYCLE("HepaFilterMonitoring", chip::app::Clusters::HepaFilterMonitoring::Id,
                       cluster::hepa_filter_monitoring::config_t, cluster::hepa_filter_monitoring::create)
TEST_CLUSTER_LIFECYCLE("IlluminanceMeasurement", chip::app::Clusters::IlluminanceMeasurement::Id,
                       cluster::illuminance_measurement::config_t, cluster::illuminance_measurement::create)
TEST_CLUSTER_LIFECYCLE("MicrowaveOvenControl", chip::app::Clusters::MicrowaveOvenControl::Id,
                       cluster::microwave_oven_control::config_t, create_microwave_oven_control_cluster)
TEST_CLUSTER_LIFECYCLE("NetworkCommissioning", chip::app::Clusters::NetworkCommissioning::Id,
                       cluster::network_commissioning::config_t, cluster::network_commissioning::create)
TEST_CLUSTER_LIFECYCLE("Identify", chip::app::Clusters::Identify::Id, cluster::identify::config_t,
                       cluster::identify::create)
TEST_CLUSTER_LIFECYCLE("OccupancySensing", chip::app::Clusters::OccupancySensing::Id,
                       cluster::occupancy_sensing::config_t, create_occupancy_sensing_cluster)
TEST_CLUSTER_LIFECYCLE("OtaSoftwareUpdateProvider", chip::app::Clusters::OtaSoftwareUpdateProvider::Id,
                       cluster::ota_software_update_provider::config_t, cluster::ota_software_update_provider::create)
TEST_CLUSTER_LIFECYCLE("OtaSoftwareUpdateRequestor", chip::app::Clusters::OtaSoftwareUpdateRequestor::Id,
                       cluster::ota_software_update_requestor::config_t, cluster::ota_software_update_requestor::create)
TEST_CLUSTER_LIFECYCLE("PowerSource", chip::app::Clusters::PowerSource::Id, cluster::power_source::config_t,
                       create_power_source_cluster)
TEST_CLUSTER_LIFECYCLE("PressureMeasurement", chip::app::Clusters::PressureMeasurement::Id,
                       cluster::pressure_measurement::config_t, cluster::pressure_measurement::create)
TEST_CLUSTER_LIFECYCLE("PushAvStreamTransport", chip::app::Clusters::PushAvStreamTransport::Id,
                       cluster::push_av_stream_transport::config_t, cluster::push_av_stream_transport::create)
TEST_CLUSTER_LIFECYCLE("RelativeHumidityMeasurement", chip::app::Clusters::RelativeHumidityMeasurement::Id,
                       cluster::relative_humidity_measurement::config_t, cluster::relative_humidity_measurement::create)
TEST_CLUSTER_LIFECYCLE("ScenesManagement", chip::app::Clusters::ScenesManagement::Id,
                       cluster::scenes_management::config_t, cluster::scenes_management::create)
TEST_CLUSTER_LIFECYCLE("SoilMeasurement", chip::app::Clusters::SoilMeasurement::Id, cluster::soil_measurement::config_t,
                       create_soil_measurement_cluster)
TEST_CLUSTER_LIFECYCLE("Switch", chip::app::Clusters::Switch::Id, cluster::switch_cluster::config_t,
                       create_switch_cluster)
TEST_CLUSTER_LIFECYCLE("TemperatureControl", chip::app::Clusters::TemperatureControl::Id,
                       cluster::temperature_control::config_t, create_temperature_control_cluster)
TEST_CLUSTER_LIFECYCLE("TemperatureMeasurement", chip::app::Clusters::TemperatureMeasurement::Id,
                       cluster::temperature_measurement::config_t, cluster::temperature_measurement::create)
TEST_CLUSTER_LIFECYCLE("ThreadBorderRouterManagement", chip::app::Clusters::ThreadBorderRouterManagement::Id,
                       cluster::thread_border_router_management::config_t, cluster::thread_border_router_management::create)
TEST_CLUSTER_LIFECYCLE("ThreadNetworkDirectory", chip::app::Clusters::ThreadNetworkDirectory::Id,
                       cluster::thread_network_directory::config_t, cluster::thread_network_directory::create)
TEST_CLUSTER_LIFECYCLE("UserLabel", chip::app::Clusters::UserLabel::Id, cluster::user_label::config_t,
                       cluster::user_label::create)
TEST_CLUSTER_LIFECYCLE("ValveConfigurationAndControl", chip::app::Clusters::ValveConfigurationAndControl::Id,
                       cluster::valve_configuration_and_control::config_t, cluster::valve_configuration_and_control::create)
TEST_CLUSTER_LIFECYCLE("WiFiNetworkDiagnostics", chip::app::Clusters::WiFiNetworkDiagnostics::Id,
                       cluster::wifi_network_diagnostics::config_t, cluster::wifi_network_diagnostics::create)
// ZoneManagement: see IGNORED section (unordered_map leak)
