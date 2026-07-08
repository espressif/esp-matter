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
#include <zone_management.h>
#include <zone_management_ids.h>
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
namespace zone_management {

namespace feature {
namespace two_dimensional_cartesian_zone {
uint32_t get_id()
{
    return TwoDimensionalCartesianZone::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_two_d_cartesian_max(cluster, NULL, 0, 0);
    command::create_create_two_d_cartesian_zone(cluster);
    command::create_create_two_d_cartesian_zone_response(cluster);
    command::create_update_two_d_cartesian_zone(cluster);

    return ESP_OK;
}
} /* two_dimensional_cartesian_zone */

namespace per_zone_sensitivity {
uint32_t get_id()
{
    return PerZoneSensitivity::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(two_dimensional_cartesian_zone), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute_t *sensitivity =
        esp_matter::attribute::get(cluster, attribute::Sensitivity::Id);
    if (sensitivity) {
        esp_matter::attribute::destroy(cluster, sensitivity);
    }

    return ESP_OK;
}
} /* per_zone_sensitivity */

namespace user_defined {
uint32_t get_id()
{
    return UserDefined::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(two_dimensional_cartesian_zone), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_max_user_defined_zones(cluster, config->max_user_defined_zones);
    command::create_create_two_d_cartesian_zone(cluster);
    command::create_create_two_d_cartesian_zone_response(cluster);
    command::create_update_two_d_cartesian_zone(cluster);
    command::create_remove_zone(cluster);

    return ESP_OK;
}
} /* user_defined */

namespace focus_zones {
uint32_t get_id()
{
    return FocusZones::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(user_defined), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* focus_zones */

} /* feature */

namespace attribute {
attribute_t *create_max_user_defined_zones(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(user_defined), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxUserDefinedZones::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(5)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_max_zones(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxZones::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(1)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_zones(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Zones::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_triggers(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Triggers::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_sensitivity_max(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SensitivityMax::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(2)), esp_matter_attr_val(static_cast<uint8_t>(10)));
    return attribute;
}

attribute_t *create_sensitivity(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(!(has_feature(per_zone_sensitivity)), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, Sensitivity::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(1)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_two_d_cartesian_max(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(two_dimensional_cartesian_zone), NULL);
    return esp_matter::attribute::create(cluster, TwoDCartesianMax::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_create_two_d_cartesian_zone(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(two_dimensional_cartesian_zone)) && (has_feature(user_defined))), NULL);
    return esp_matter::command::create(cluster, CreateTwoDCartesianZone::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_create_two_d_cartesian_zone_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(two_dimensional_cartesian_zone)) && (has_feature(user_defined))), NULL);
    return esp_matter::command::create(cluster, CreateTwoDCartesianZoneResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_update_two_d_cartesian_zone(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(two_dimensional_cartesian_zone)) && (has_feature(user_defined))), NULL);
    return esp_matter::command::create(cluster, UpdateTwoDCartesianZone::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_remove_zone(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(user_defined), NULL);
    return esp_matter::command::create(cluster, RemoveZone::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_create_or_update_trigger(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CreateOrUpdateTrigger::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_remove_trigger(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveTrigger::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_zone_triggered(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ZoneTriggered::Id);
}

event_t *create_zone_stopped(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ZoneStopped::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, zone_management::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, zone_management::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterZoneManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_max_zones(cluster, config->max_zones);
        attribute::create_sensitivity_max(cluster, config->sensitivity_max);
        attribute::create_sensitivity(cluster, config->sensitivity);
        attribute::create_zones(cluster, NULL, 0, 0);
        attribute::create_triggers(cluster, NULL, 0, 0);
        command::create_create_or_update_trigger(cluster);
        command::create_remove_trigger(cluster);
        /* Events */
        event::create_zone_triggered(cluster);
        event::create_zone_stopped(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterZoneManagementClusterServerInitCallback,
                                                 ESPMatterZoneManagementClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* zone_management */
} /* cluster */
} /* esp_matter */
