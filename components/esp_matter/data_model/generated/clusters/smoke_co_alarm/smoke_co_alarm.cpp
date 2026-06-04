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
#include <app-common/zap-generated/callback.h>
#include <app/InteractionModelEngine.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <smoke_co_alarm.h>
#include <smoke_co_alarm_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "smoke_co_alarm_cluster";
constexpr uint16_t cluster_revision = 2;

static esp_err_t esp_matter_command_callback_self_test_request(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                               void *opaque_ptr)
{
    chip::app::Clusters::SmokeCoAlarm::Commands::SelfTestRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfSmokeCoAlarmClusterSelfTestRequestCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace smoke_co_alarm {

namespace feature {
namespace smoke_alarm {
uint32_t get_id()
{
    return SmokeAlarm::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_smoke_state(cluster, config->smoke_state);
    event::create_smoke_alarm(cluster);

    return ESP_OK;
}
} /* smoke_alarm */

namespace co_alarm {
uint32_t get_id()
{
    return COAlarm::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_co_state(cluster, config->co_state);
    event::create_co_alarm(cluster);

    return ESP_OK;
}
} /* co_alarm */

} /* feature */

namespace attribute {
attribute_t *create_expressed_state(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ExpressedState::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(9), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_smoke_state(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(smoke_alarm), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, SmokeState::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_co_state(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(co_alarm), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, COState::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_battery_alert(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatteryAlert::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_device_muted(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DeviceMuted::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(1), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_test_in_progress(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, TestInProgress::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_hardware_fault_alert(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, HardwareFaultAlert::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_end_of_service_alert(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, EndOfServiceAlert::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(1), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_interconnect_smoke_alarm(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, InterconnectSmokeAlarm::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_interconnect_co_alarm(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, InterconnectCOAlarm::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_contamination_state(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ContaminationState::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_smoke_sensitivity_level(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SmokeSensitivityLevel::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_expiry_date(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ExpiryDate::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_unmounted(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, Unmounted::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

} /* attribute */
namespace command {
command_t *create_self_test_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SelfTestRequest::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_self_test_request);
}

} /* command */

namespace event {
event_t *create_smoke_alarm(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(smoke_alarm), NULL);
    return esp_matter::event::create(cluster, SmokeAlarm::Id);
}

event_t *create_co_alarm(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(co_alarm), NULL);
    return esp_matter::event::create(cluster, COAlarm::Id);
}

event_t *create_low_battery(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, LowBattery::Id);
}

event_t *create_hardware_fault(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, HardwareFault::Id);
}

event_t *create_end_of_service(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, EndOfService::Id);
}

event_t *create_self_test_complete(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SelfTestComplete::Id);
}

event_t *create_alarm_muted(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, AlarmMuted::Id);
}

event_t *create_mute_ended(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, MuteEnded::Id);
}

event_t *create_interconnect_smoke_alarm(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, InterconnectSmokeAlarm::Id);
}

event_t *create_interconnect_co_alarm(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, InterconnectCOAlarm::Id);
}

event_t *create_all_clear(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, AllClear::Id);
}

} /* event */

static void create_default_binding_cluster(endpoint_t *endpoint)
{
    binding::config_t config;
    binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, smoke_co_alarm::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, smoke_co_alarm::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterSmokeCoAlarmPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_expressed_state(cluster, config->expressed_state);
        attribute::create_battery_alert(cluster, config->battery_alert);
        attribute::create_test_in_progress(cluster, config->test_in_progress);
        attribute::create_hardware_fault_alert(cluster, config->hardware_fault_alert);
        attribute::create_end_of_service_alert(cluster, config->end_of_service_alert);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("SmokeAlarm,COAlarm",
                                       feature::smoke_alarm::get_id(), feature::co_alarm::get_id());
        if (feature_map & feature::smoke_alarm::get_id()) {
            VerifyOrReturnValue(feature::smoke_alarm::add(cluster, &(config->features.smoke_alarm)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::co_alarm::get_id()) {
            VerifyOrReturnValue(feature::co_alarm::add(cluster, &(config->features.co_alarm)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        /* Events */
        event::create_low_battery(cluster);
        event::create_hardware_fault(cluster);
        event::create_end_of_service(cluster);
        event::create_self_test_complete(cluster);
        event::create_all_clear(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* smoke_co_alarm */
} /* cluster */
} /* esp_matter */
