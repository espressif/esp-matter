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
#include <switch_cluster.h>
#include <switch_cluster_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

#include <app/clusters/switch-server/switch-server.h>

using chip::EndpointId;
using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 2;

namespace esp_matter {
namespace cluster {
namespace switch_cluster {

namespace feature {
namespace latching_switch {
uint32_t get_id()
{
    return LatchingSwitch::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    event::create_switch_latched(cluster);

    return ESP_OK;
}
} /* latching_switch */

namespace momentary_switch {
uint32_t get_id()
{
    return MomentarySwitch::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    event::create_initial_press(cluster);

    return ESP_OK;
}
} /* momentary_switch */

namespace momentary_switch_release {
uint32_t get_id()
{
    return MomentarySwitchRelease::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(momentary_switch)) && (!(has_feature(action_switch)))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    event::create_short_release(cluster);

    return ESP_OK;
}
} /* momentary_switch_release */

namespace momentary_switch_long_press {
uint32_t get_id()
{
    return MomentarySwitchLongPress::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(momentary_switch)) && (((has_feature(momentary_switch_release)) || (has_feature(action_switch))))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    event::create_long_press(cluster);
    event::create_long_release(cluster);

    return ESP_OK;
}
} /* momentary_switch_long_press */

namespace momentary_switch_multi_press {
uint32_t get_id()
{
    return MomentarySwitchMultiPress::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError((has_feature(action_switch) || ((has_feature(momentary_switch)) && (has_feature(momentary_switch_release)))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_multi_press_max(cluster, config->multi_press_max);
    event::create_multi_press_ongoing(cluster);
    event::create_multi_press_complete(cluster);

    return ESP_OK;
}
} /* momentary_switch_multi_press */

namespace action_switch {
uint32_t get_id()
{
    return ActionSwitch::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(momentary_switch), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    event_t *multi_press_ongoing = esp_matter::event::get(cluster, event::MultiPressOngoing::Id);
    if (multi_press_ongoing) {
        esp_matter::event::destroy(cluster, multi_press_ongoing);
    }

    return ESP_OK;
}
} /* action_switch */

} /* feature */

namespace attribute {
attribute_t *create_number_of_positions(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, NumberOfPositions::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(NumberOfPositions::Min)), esp_matter_attr_val(static_cast<uint8_t>(NumberOfPositions::Max)));
    return attribute;
}

attribute_t *create_current_position(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CurrentPosition::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_multi_press_max(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(momentary_switch_multi_press), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MultiPressMax::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(MultiPressMax::Min)), esp_matter_attr_val(static_cast<uint8_t>(MultiPressMax::Max)));
    return attribute;
}

} /* attribute */

namespace event {
event_t *create_switch_latched(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(latching_switch), NULL);
    return esp_matter::event::create(cluster, SwitchLatched::Id);
}

event_t *create_initial_press(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(momentary_switch), NULL);
    return esp_matter::event::create(cluster, InitialPress::Id);
}

event_t *create_long_press(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(momentary_switch_long_press), NULL);
    return esp_matter::event::create(cluster, LongPress::Id);
}

event_t *create_short_release(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(momentary_switch_release), NULL);
    return esp_matter::event::create(cluster, ShortRelease::Id);
}

event_t *create_long_release(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(momentary_switch_long_press), NULL);
    return esp_matter::event::create(cluster, LongRelease::Id);
}

event_t *create_multi_press_ongoing(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(momentary_switch_multi_press)) && (!(has_feature(action_switch)))), NULL);
    return esp_matter::event::create(cluster, MultiPressOngoing::Id);
}

event_t *create_multi_press_complete(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(momentary_switch_multi_press), NULL);
    return esp_matter::event::create(cluster, MultiPressComplete::Id);
}

esp_err_t send_switch_latched(EndpointId endpoint, uint8_t new_position)
{
    chip::app::Clusters::SwitchCluster * cluster = chip::app::Clusters::Switch::FindClusterOnEndpoint(endpoint);
    VerifyOrReturnError(cluster != nullptr, ESP_ERR_INVALID_STATE);
    (void) cluster->OnSwitchLatch(new_position);
    return ESP_OK;
}

esp_err_t send_initial_press(EndpointId endpoint, uint8_t new_position)
{
    chip::app::Clusters::SwitchCluster * cluster = chip::app::Clusters::Switch::FindClusterOnEndpoint(endpoint);
    VerifyOrReturnError(cluster != nullptr, ESP_ERR_INVALID_STATE);
    (void) cluster->OnInitialPress(new_position);
    return ESP_OK;
}

esp_err_t send_long_press(EndpointId endpoint, uint8_t new_position)
{
    chip::app::Clusters::SwitchCluster * cluster = chip::app::Clusters::Switch::FindClusterOnEndpoint(endpoint);
    VerifyOrReturnError(cluster != nullptr, ESP_ERR_INVALID_STATE);
    (void) cluster->OnLongPress(new_position);
    return ESP_OK;
}

esp_err_t send_short_release(EndpointId endpoint, uint8_t previous_position)
{
    chip::app::Clusters::SwitchCluster * cluster = chip::app::Clusters::Switch::FindClusterOnEndpoint(endpoint);
    VerifyOrReturnError(cluster != nullptr, ESP_ERR_INVALID_STATE);
    (void) cluster->OnShortRelease(previous_position);
    return ESP_OK;
}

esp_err_t send_long_release(EndpointId endpoint, uint8_t previous_position)
{
    chip::app::Clusters::SwitchCluster * cluster = chip::app::Clusters::Switch::FindClusterOnEndpoint(endpoint);
    VerifyOrReturnError(cluster != nullptr, ESP_ERR_INVALID_STATE);
    (void) cluster->OnLongRelease(previous_position);
    return ESP_OK;
}

esp_err_t send_multi_press_ongoing(EndpointId endpoint, uint8_t new_position, uint8_t count)
{
    chip::app::Clusters::SwitchCluster * cluster = chip::app::Clusters::Switch::FindClusterOnEndpoint(endpoint);
    VerifyOrReturnError(cluster != nullptr, ESP_ERR_INVALID_STATE);
    (void) cluster->OnMultiPressOngoing(new_position, count);
    return ESP_OK;
}

esp_err_t send_multi_press_complete(EndpointId endpoint, uint8_t new_position, uint8_t count)
{
    chip::app::Clusters::SwitchCluster * cluster = chip::app::Clusters::Switch::FindClusterOnEndpoint(endpoint);
    VerifyOrReturnError(cluster != nullptr, ESP_ERR_INVALID_STATE);
    (void) cluster->OnMultiPressComplete(new_position, count);
    return ESP_OK;
}
} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, switch_cluster::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, switch_cluster::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_number_of_positions(cluster, config->number_of_positions);
        attribute::create_current_position(cluster, config->current_position);

        uint32_t feature_map = config->feature_flags;
        if (feature_map & feature::action_switch::get_id()) {
            VerifyOrReturnValue(feature::action_switch::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
            VerifyOrReturnValue(feature::momentary_switch_multi_press::add(cluster, &(config->features.momentary_switch_multi_press)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        } else {
            if (feature_map & feature::momentary_switch_multi_press::get_id()) {
                VerifyOrReturnValue(feature::momentary_switch_multi_press::add(cluster, &(config->features.momentary_switch_multi_press)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
            }
        }
        VALIDATE_FEATURES_EXACT_ONE("LatchingSwitch,MomentarySwitch",
                                    feature::latching_switch::get_id(), feature::momentary_switch::get_id());
        if (feature_map & feature::latching_switch::get_id()) {
            VerifyOrReturnValue(feature::latching_switch::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::momentary_switch::get_id()) {
            VerifyOrReturnValue(feature::momentary_switch::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::momentary_switch_release::get_id()) {
            VerifyOrReturnValue(feature::momentary_switch_release::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::momentary_switch_long_press::get_id()) {
            VerifyOrReturnValue(feature::momentary_switch_long_press::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        /* Events */
        event::create_multi_press_ongoing(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* switch_cluster */
} /* cluster */
} /* esp_matter */
