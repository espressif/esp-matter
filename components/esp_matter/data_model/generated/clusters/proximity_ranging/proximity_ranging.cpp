// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#include <esp_log.h>
#include <esp_matter_core.h>
#include <esp_matter.h>

#include <app-common/zap-generated/cluster-enums.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <proximity_ranging.h>
#include <proximity_ranging_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace proximity_ranging {

namespace feature {
namespace wi_fi_usd_proximity_detection {
uint32_t get_id()
{
    return WiFiUsdProximityDetection::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_wi_fi_dev_ik(cluster, config->wi_fi_dev_ik, sizeof(config->wi_fi_dev_ik));

    return ESP_OK;
}
} /* wi_fi_usd_proximity_detection */

namespace bluetooth_channel_sounding {
uint32_t get_id()
{
    return BluetoothChannelSounding::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_blt_dev_ik(cluster, config->blt_dev_ik, sizeof(config->blt_dev_ik));
    attribute::create_bltcs_security_level(cluster, config->bltcs_security_level);
    attribute::create_bltcs_mode_capability(cluster, config->bltcs_mode_capability);

    return ESP_OK;
}
} /* bluetooth_channel_sounding */

namespace ble_beacon_rssi {
uint32_t get_id()
{
    return BleBeaconRssi::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_ble_device_id(cluster, config->ble_device_id);

    return ESP_OK;
}
} /* ble_beacon_rssi */

} /* feature */

namespace attribute {
attribute_t *create_ranging_capabilities(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, RangingCapabilities::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_wi_fi_dev_ik(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(wi_fi_usd_proximity_detection), NULL);
    return esp_matter::attribute::create(cluster, WiFiDevIK::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length));
}

attribute_t *create_ble_device_id(cluster_t *cluster, uint64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(ble_beacon_rssi), NULL);
    return esp_matter::attribute::create(cluster, BLEDeviceID::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_blt_dev_ik(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(bluetooth_channel_sounding), NULL);
    return esp_matter::attribute::create(cluster, BLTDevIK::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length));
}

attribute_t *create_bltcs_security_level(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(bluetooth_channel_sounding), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, BLTCSSecurityLevel::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(BLTCSSecurityLevel::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(BLTCSSecurityLevel::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_bltcs_mode_capability(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(bluetooth_channel_sounding), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, BLTCSModeCapability::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(BLTCSModeCapability::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(BLTCSModeCapability::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_session_id_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, SessionIDList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_ranging_constraints(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, RangingConstraints::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_start_ranging_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StartRangingRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_start_ranging_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StartRangingResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_stop_ranging_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StopRangingRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_ranging_result(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, RangingResult::Id);
}

event_t *create_ranging_session_status(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, RangingSessionStatus::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, proximity_ranging::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, proximity_ranging::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterProximityRangingPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_ranging_capabilities(cluster, NULL, 0, 0);
        attribute::create_session_id_list(cluster, NULL, 0, 0);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("Wi-Fi_USD_Proximity_Detection,Bluetooth_Channel_Sounding,BLE_Beacon_RSSI",
                                       feature::wi_fi_usd_proximity_detection::get_id(), feature::bluetooth_channel_sounding::get_id(), feature::ble_beacon_rssi::get_id());
        if (feature_map & feature::wi_fi_usd_proximity_detection::get_id()) {
            VerifyOrReturnValue(feature::wi_fi_usd_proximity_detection::add(cluster, &(config->features.wi_fi_usd_proximity_detection)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::bluetooth_channel_sounding::get_id()) {
            VerifyOrReturnValue(feature::bluetooth_channel_sounding::add(cluster, &(config->features.bluetooth_channel_sounding)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::ble_beacon_rssi::get_id()) {
            VerifyOrReturnValue(feature::ble_beacon_rssi::add(cluster, &(config->features.ble_beacon_rssi)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        command::create_start_ranging_request(cluster);
        command::create_start_ranging_response(cluster);
        command::create_stop_ranging_request(cluster);
        /* Events */
        event::create_ranging_result(cluster);
        event::create_ranging_session_status(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterProximityRangingClusterServerInitCallback,
                                                 ESPMatterProximityRangingClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* proximity_ranging */
} /* cluster */
} /* esp_matter */
