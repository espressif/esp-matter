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
#include <closure_control.h>
#include <closure_control_ids.h>
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
namespace closure_control {

namespace feature {
namespace positioning {
uint32_t get_id()
{
    return Positioning::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* positioning */

namespace motion_latching {
uint32_t get_id()
{
    return MotionLatching::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_latch_control_modes(cluster, config->latch_control_modes);

    return ESP_OK;
}
} /* motion_latching */

namespace instantaneous {
uint32_t get_id()
{
    return Instantaneous::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command_t *stop = esp_matter::command::get(cluster, command::Stop::Id, COMMAND_FLAG_ACCEPTED);
    if (stop) {
        esp_matter::command::destroy(cluster, stop);
    }
    event_t *movement_completed = esp_matter::event::get(cluster, event::MovementCompleted::Id);
    if (movement_completed) {
        esp_matter::event::destroy(cluster, movement_completed);
    }

    return ESP_OK;
}
} /* instantaneous */

namespace speed {
uint32_t get_id()
{
    return Speed::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(positioning)) && (!(has_feature(instantaneous)))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* speed */

namespace ventilation {
uint32_t get_id()
{
    return Ventilation::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(positioning), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* ventilation */

namespace pedestrian {
uint32_t get_id()
{
    return Pedestrian::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(positioning), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* pedestrian */

namespace calibration {
uint32_t get_id()
{
    return Calibration::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(positioning), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_calibrate(cluster);

    return ESP_OK;
}
} /* calibration */

namespace protection {
uint32_t get_id()
{
    return Protection::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* protection */

namespace manually_operable {
uint32_t get_id()
{
    return ManuallyOperable::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    event::create_engage_state_changed(cluster);

    return ESP_OK;
}
} /* manually_operable */

} /* feature */

namespace attribute {
attribute_t *create_countdown_time(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, CountdownTime::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(259200)));
    return attribute;
}

attribute_t *create_main_state(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MainState::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(7), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_current_error_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, CurrentErrorList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_overall_current_state(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, OverallCurrentState::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_overall_target_state(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, OverallTargetState::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_latch_control_modes(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(motion_latching), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, LatchControlModes::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_stop(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(!(has_feature(instantaneous)), NULL);
    return esp_matter::command::create(cluster, Stop::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_move_to(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, MoveTo::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_calibrate(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(calibration), NULL);
    return esp_matter::command::create(cluster, Calibrate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_operational_error(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, OperationalError::Id);
}

event_t *create_movement_completed(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(!(has_feature(instantaneous)), NULL);
    return esp_matter::event::create(cluster, MovementCompleted::Id);
}

event_t *create_engage_state_changed(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(manually_operable), NULL);
    return esp_matter::event::create(cluster, EngageStateChanged::Id);
}

event_t *create_secure_state_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SecureStateChanged::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, closure_control::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, closure_control::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = ClosureControlDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterClosureControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_main_state(cluster, config->main_state);
        attribute::create_current_error_list(cluster, NULL, 0, 0);
        attribute::create_overall_current_state(cluster, NULL, 0, 0);
        attribute::create_overall_target_state(cluster, NULL, 0, 0);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("Positioning,MotionLatching",
                                       feature::positioning::get_id(), feature::motion_latching::get_id());
        if (feature_map & feature::positioning::get_id()) {
            VerifyOrReturnValue(feature::positioning::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::motion_latching::get_id()) {
            VerifyOrReturnValue(feature::motion_latching::add(cluster, &(config->features.motion_latching)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::instantaneous::get_id()) {
            VerifyOrReturnValue(feature::instantaneous::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::speed::get_id()) {
            VerifyOrReturnValue(feature::speed::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::ventilation::get_id()) {
            VerifyOrReturnValue(feature::ventilation::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::pedestrian::get_id()) {
            VerifyOrReturnValue(feature::pedestrian::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::calibration::get_id()) {
            VerifyOrReturnValue(feature::calibration::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::protection::get_id()) {
            VerifyOrReturnValue(feature::protection::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::manually_operable::get_id()) {
            VerifyOrReturnValue(feature::manually_operable::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        command::create_stop(cluster);
        command::create_move_to(cluster);
        /* Events */
        event::create_operational_error(cluster);
        event::create_movement_completed(cluster);
        event::create_secure_state_changed(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterClosureControlClusterServerInitCallback,
                                                 ESPMatterClosureControlClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* closure_control */
} /* cluster */
} /* esp_matter */
