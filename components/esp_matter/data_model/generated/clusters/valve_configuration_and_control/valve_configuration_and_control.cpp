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
#include <esp_matter_delegate_callbacks.h>
#include <valve_configuration_and_control.h>
#include <valve_configuration_and_control_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace valve_configuration_and_control {

namespace feature {
namespace time_sync {
uint32_t get_id()
{
    return TimeSync::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_auto_close_time(cluster, config->auto_close_time);

    return ESP_OK;
}
} /* time_sync */

namespace level {
uint32_t get_id()
{
    return Level::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_current_level(cluster, config->current_level);
    attribute::create_target_level(cluster, config->target_level);

    return ESP_OK;
}
} /* level */

} /* feature */

namespace attribute {
attribute_t *create_open_duration(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, OpenDuration::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(1)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_default_open_duration(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DefaultOpenDuration::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(1)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_auto_close_time(cluster_t *cluster, nullable<uint64_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_sync), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, AutoCloseTime::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint64_t>(0)), esp_matter_attr_val(nullable<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_remaining_duration(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RemainingDuration::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_current_state(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentState::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(nullable<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_target_state(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TargetState::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(nullable<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_current_level(cluster_t *cluster, nullable<uint8_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(level), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentLevel::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_target_level(cluster_t *cluster, nullable<uint8_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(level), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, TargetLevel::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_default_open_level(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DefaultOpenLevel::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(1)), esp_matter_attr_val(static_cast<uint8_t>(100)));
    return attribute;
}

attribute_t *create_valve_fault(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ValveFault::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint16_t>(65535), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_level_step(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LevelStep::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(1)), esp_matter_attr_val(static_cast<uint8_t>(50)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_open(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Open::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_close(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Close::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_valve_state_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ValveStateChanged::Id);
}

event_t *create_valve_fault(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ValveFault::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, valve_configuration_and_control::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, valve_configuration_and_control::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = ValveConfigurationAndControlDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterValveConfigurationAndControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_open_duration(cluster, config->open_duration);
        attribute::create_default_open_duration(cluster, config->default_open_duration);
        attribute::create_remaining_duration(cluster, config->remaining_duration);
        attribute::create_current_state(cluster, config->current_state);
        attribute::create_target_state(cluster, config->target_state);
        command::create_open(cluster);
        command::create_close(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterValveConfigurationAndControlClusterServerInitCallback,
                                                 ESPMatterValveConfigurationAndControlClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* valve_configuration_and_control */
} /* cluster */
} /* esp_matter */
