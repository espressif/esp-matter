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
#include <closure_dimension.h>
#include <closure_dimension_ids.h>
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
namespace closure_dimension {

namespace feature {
namespace positioning {
uint32_t get_id()
{
    return Positioning::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_resolution(cluster, config->resolution);
    attribute::create_step_value(cluster, config->step_value);
    command::create_step(cluster);

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

namespace unit {
uint32_t get_id()
{
    return Unit::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(positioning), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_unit(cluster, config->unit);
    attribute::create_unit_range(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* unit */

namespace limitation {
uint32_t get_id()
{
    return Limitation::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(positioning), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_limit_range(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* limitation */

namespace speed {
uint32_t get_id()
{
    return Speed::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(positioning), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* speed */

namespace translation {
uint32_t get_id()
{
    return Translation::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(positioning), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_translation_direction(cluster, config->translation_direction);

    return ESP_OK;
}
} /* translation */

namespace rotation {
uint32_t get_id()
{
    return Rotation::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(positioning), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_rotation_axis(cluster, config->rotation_axis);
    attribute::create_overflow(cluster, config->overflow);

    return ESP_OK;
}
} /* rotation */

namespace modulation {
uint32_t get_id()
{
    return Modulation::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(positioning), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_modulation_type(cluster, config->modulation_type);

    return ESP_OK;
}
} /* modulation */

} /* feature */

namespace attribute {
attribute_t *create_current_state(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, CurrentState::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_target_state(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, TargetState::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_resolution(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(positioning), NULL);
    return esp_matter::attribute::create(cluster, Resolution::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_step_value(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(positioning), NULL);
    return esp_matter::attribute::create(cluster, StepValue::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_unit(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(unit), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, Unit::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(Unit::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(Unit::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_unit_range(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(unit), NULL);
    return esp_matter::attribute::create(cluster, UnitRange::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_limit_range(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(limitation), NULL);
    return esp_matter::attribute::create(cluster, LimitRange::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_translation_direction(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(translation), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, TranslationDirection::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(TranslationDirection::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(TranslationDirection::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_rotation_axis(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(rotation), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, RotationAxis::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(RotationAxis::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(RotationAxis::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_overflow(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(rotation), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, Overflow::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(Overflow::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(Overflow::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_modulation_type(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(modulation), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ModulationType::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(ModulationType::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(ModulationType::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_latch_control_modes(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(motion_latching), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, LatchControlModes::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(LatchControlModes::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint8_t>(LatchControlModes::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_set_target(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SetTarget::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_step(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(positioning), NULL);
    return esp_matter::command::create(cluster, Step::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, closure_dimension::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, closure_dimension::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = ClosureDimensionDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterClosureDimensionPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_current_state(cluster, NULL, 0, 0);
        attribute::create_target_state(cluster, NULL, 0, 0);

        uint32_t feature_map = config->feature_flags;
        if (feature_map & feature::positioning::get_id()) {
            VALIDATE_FEATURES_EXACT_ONE("Translation,Rotation,Modulation",
                                        feature::translation::get_id(), feature::rotation::get_id(), feature::modulation::get_id());
            if (feature_map & feature::translation::get_id()) {
                VerifyOrReturnValue(feature::translation::add(cluster, &(config->features.translation)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
            }
            if (feature_map & feature::rotation::get_id()) {
                VerifyOrReturnValue(feature::rotation::add(cluster, &(config->features.rotation)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
            }
            if (feature_map & feature::modulation::get_id()) {
                VerifyOrReturnValue(feature::modulation::add(cluster, &(config->features.modulation)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
            }
        }
        VALIDATE_FEATURES_AT_LEAST_ONE("Positioning,MotionLatching",
                                       feature::positioning::get_id(), feature::motion_latching::get_id());
        if (feature_map & feature::positioning::get_id()) {
            VerifyOrReturnValue(feature::positioning::add(cluster, &(config->features.positioning)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::motion_latching::get_id()) {
            VerifyOrReturnValue(feature::motion_latching::add(cluster, &(config->features.motion_latching)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::unit::get_id()) {
            VerifyOrReturnValue(feature::unit::add(cluster, &(config->features.unit)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::limitation::get_id()) {
            VerifyOrReturnValue(feature::limitation::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::speed::get_id()) {
            VerifyOrReturnValue(feature::speed::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        command::create_set_target(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterClosureDimensionClusterServerInitCallback,
                                                 ESPMatterClosureDimensionClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* closure_dimension */
} /* cluster */
} /* esp_matter */
