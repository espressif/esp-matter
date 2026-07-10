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
#include <camera_av_settings_user_level_management.h>
#include <camera_av_settings_user_level_management_ids.h>
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
namespace camera_av_settings_user_level_management {

namespace feature {
namespace digital_ptz {
uint32_t get_id()
{
    return DigitalPTZ::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_dptz_streams(cluster, NULL, 0, 0);
    command::create_dptz_set_viewport(cluster);

    return ESP_OK;
}
} /* digital_ptz */

namespace mechanical_pan {
uint32_t get_id()
{
    return MechanicalPan::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_pan_min(cluster, config->pan_min);
    attribute::create_pan_max(cluster, config->pan_max);
    attribute::create_movement_state(cluster, config->movement_state);
    attribute::create_mptz_position(cluster, NULL, 0, 0);
    command::create_mptz_set_position(cluster);
    command::create_mptz_relative_move(cluster);

    return ESP_OK;
}
} /* mechanical_pan */

namespace mechanical_tilt {
uint32_t get_id()
{
    return MechanicalTilt::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_tilt_min(cluster, config->tilt_min);
    attribute::create_tilt_max(cluster, config->tilt_max);
    attribute::create_movement_state(cluster, config->movement_state);
    attribute::create_mptz_position(cluster, NULL, 0, 0);
    command::create_mptz_set_position(cluster);
    command::create_mptz_relative_move(cluster);

    return ESP_OK;
}
} /* mechanical_tilt */

namespace mechanical_zoom {
uint32_t get_id()
{
    return MechanicalZoom::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_zoom_max(cluster, config->zoom_max);
    attribute::create_movement_state(cluster, config->movement_state);
    attribute::create_mptz_position(cluster, NULL, 0, 0);
    command::create_mptz_set_position(cluster);
    command::create_mptz_relative_move(cluster);

    return ESP_OK;
}
} /* mechanical_zoom */

namespace mechanical_presets {
uint32_t get_id()
{
    return MechanicalPresets::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(mechanical_pan)) || (has_feature(mechanical_tilt)) || (has_feature(mechanical_zoom))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_max_presets(cluster, config->max_presets);
    attribute::create_mptz_presets(cluster, NULL, 0, 0);
    command::create_mptz_move_to_preset(cluster);
    command::create_mptz_save_preset(cluster);
    command::create_mptz_remove_preset(cluster);

    return ESP_OK;
}
} /* mechanical_presets */

} /* feature */

namespace attribute {
attribute_t *create_mptz_position(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(mechanical_pan)) || (has_feature(mechanical_tilt)) || (has_feature(mechanical_zoom))), NULL);
    return esp_matter::attribute::create(cluster, MPTZPosition::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_max_presets(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(mechanical_presets), NULL);
    return esp_matter::attribute::create(cluster, MaxPresets::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_mptz_presets(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(mechanical_presets), NULL);
    return esp_matter::attribute::create(cluster, MPTZPresets::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_dptz_streams(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(digital_ptz), NULL);
    return esp_matter::attribute::create(cluster, DPTZStreams::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_zoom_max(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(mechanical_zoom), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ZoomMax::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(ZoomMax::Min)), esp_matter_attr_val(static_cast<uint8_t>(ZoomMax::Max)));
    return attribute;
}

attribute_t *create_tilt_min(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(mechanical_tilt), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, TiltMin::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(TiltMin::Min)), esp_matter_attr_val(static_cast<int16_t>(TiltMin::Max)));
    return attribute;
}

attribute_t *create_tilt_max(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(mechanical_tilt), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, TiltMax::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(TiltMax::Min)), esp_matter_attr_val(static_cast<int16_t>(TiltMax::Max)));
    return attribute;
}

attribute_t *create_pan_min(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(mechanical_pan), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, PanMin::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(PanMin::Min)), esp_matter_attr_val(static_cast<int16_t>(PanMin::Max)));
    return attribute;
}

attribute_t *create_pan_max(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(mechanical_pan), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, PanMax::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(PanMax::Min)), esp_matter_attr_val(static_cast<int16_t>(PanMax::Max)));
    return attribute;
}

attribute_t *create_movement_state(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(mechanical_pan)) || (has_feature(mechanical_tilt)) || (has_feature(mechanical_zoom))), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MovementState::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(MovementState::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(MovementState::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_mptz_set_position(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(mechanical_pan)) || (has_feature(mechanical_tilt)) || (has_feature(mechanical_zoom))), NULL);
    return esp_matter::command::create(cluster, MPTZSetPosition::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_mptz_relative_move(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(mechanical_pan)) || (has_feature(mechanical_tilt)) || (has_feature(mechanical_zoom))), NULL);
    return esp_matter::command::create(cluster, MPTZRelativeMove::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_mptz_move_to_preset(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(mechanical_presets), NULL);
    return esp_matter::command::create(cluster, MPTZMoveToPreset::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_mptz_save_preset(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(mechanical_presets), NULL);
    return esp_matter::command::create(cluster, MPTZSavePreset::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_mptz_remove_preset(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(mechanical_presets), NULL);
    return esp_matter::command::create(cluster, MPTZRemovePreset::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_dptz_set_viewport(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(digital_ptz), NULL);
    return esp_matter::command::create(cluster, DPTZSetViewport::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_dptz_relative_move(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DPTZRelativeMove::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, camera_av_settings_user_level_management::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, camera_av_settings_user_level_management::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterCameraAvSettingsUserLevelManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("DigitalPTZ,MechanicalPan,MechanicalTilt,MechanicalZoom",
                                       feature::digital_ptz::get_id(), feature::mechanical_pan::get_id(), feature::mechanical_tilt::get_id(), feature::mechanical_zoom::get_id());
        if (feature_map & feature::digital_ptz::get_id()) {
            VerifyOrReturnValue(feature::digital_ptz::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::mechanical_pan::get_id()) {
            VerifyOrReturnValue(feature::mechanical_pan::add(cluster, &(config->features.mechanical_pan)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::mechanical_tilt::get_id()) {
            VerifyOrReturnValue(feature::mechanical_tilt::add(cluster, &(config->features.mechanical_tilt)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::mechanical_zoom::get_id()) {
            VerifyOrReturnValue(feature::mechanical_zoom::add(cluster, &(config->features.mechanical_zoom)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::mechanical_presets::get_id()) {
            VerifyOrReturnValue(feature::mechanical_presets::add(cluster, &(config->features.mechanical_presets)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterCameraAvSettingsUserLevelManagementClusterServerInitCallback,
                                                 ESPMatterCameraAvSettingsUserLevelManagementClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* camera_av_settings_user_level_management */
} /* cluster */
} /* esp_matter */
