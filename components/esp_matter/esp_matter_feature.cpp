// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_feature.h>

#include <app-common/zap-generated/cluster-enums.h>

static const char *TAG = "esp_matter_feature";

using namespace chip::app::Clusters;

namespace esp_matter {
namespace cluster {

static esp_err_t update_feature_map(cluster_t *cluster, uint32_t value)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    /* Get the attribute */
    attribute_t *attribute = attribute::get(cluster, Globals::Attributes::FeatureMap::Id);

    /* Create the attribute with the new value if it does not exist */
    if (!attribute) {
        attribute = global::attribute::create_feature_map(cluster, value);
        if (!attribute) {
            ESP_LOGE(TAG, "Could not create feature map attribute");
            return ESP_FAIL;
        }
        return ESP_OK;
    }

    /* Update the value if the attribute already exists */
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute, &val);
    val.val.u32 |= value;
    /* Here we can't call attribute::update() since the chip stack would not have started yet, since we are
    still creating the data model. So, we are directly using attribute::set_val(). */
    return attribute::set_val(attribute, &val);
}

namespace on_off {
namespace feature {
namespace lighting {

uint32_t get_id()
{
    return (uint32_t)OnOff::OnOffFeature::kLighting;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_global_scene_control(cluster, config->global_scene_control);
    attribute::create_on_time(cluster, config->on_time);
    attribute::create_off_wait_time(cluster, config->off_wait_time);
    attribute::create_start_up_on_off(cluster, config->start_up_on_off);

    /* Commands */
    command::create_off_with_effect(cluster);
    command::create_on_with_recall_global_scene(cluster);
    command::create_on_with_timed_off(cluster);

    return ESP_OK;
}

} /* lighting */
} /* feature */
} /* on_off */

namespace level_control {
namespace feature {
namespace on_off {

uint32_t get_id()
{
    return (uint32_t)LevelControl::LevelControlFeature::kOnOff;
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* on_off */

namespace lighting {

uint32_t get_id()
{
    return (uint32_t)LevelControl::LevelControlFeature::kLighting;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_remaining_time(cluster, config->remaining_time);
    attribute::create_min_level(cluster, config->min_level);
    attribute::create_max_level(cluster, config->max_level);
    attribute::create_start_up_current_level(cluster, config->start_up_current_level);

    return ESP_OK;
}

} /* lighting */

namespace frequency {

uint32_t get_id()
{
    return (uint32_t)LevelControl::LevelControlFeature::kFrequency;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_current_frequency(cluster, config->current_frequency);
    attribute::create_min_frequency(cluster, config->min_frequency);
    attribute::create_max_frequency(cluster, config->max_frequency);

    /* Commands */
    command::create_move_to_closest_frequency(cluster);

    return ESP_OK;
}
} /* frequency */
} /* feature */
} /* level_control */

namespace color_control {
namespace feature {
namespace hue_saturation {

uint32_t get_id()
{
    return (uint32_t)ColorControl::ColorCapabilities::kHueSaturationSupported;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_current_hue(cluster, config->current_hue);
    attribute::create_current_saturation(cluster, config->current_saturation);

    /* Commands */
    command::create_move_to_hue(cluster);
    command::create_move_hue(cluster);
    command::create_step_hue(cluster);
    command::create_move_to_saturation(cluster);
    command::create_move_saturation(cluster);
    command::create_step_saturation(cluster);
    command::create_move_to_hue_and_saturation(cluster);
    command::create_stop_move_step(cluster);

    return ESP_OK;
}

} /* hue_saturation */

namespace color_temperature {

uint32_t get_id()
{
    return (uint32_t)ColorControl::ColorCapabilities::kColorTemperatureSupported;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_color_temperature_mireds(cluster, config->color_temperature_mireds);
    attribute::create_color_temp_physical_min_mireds(cluster, config->color_temp_physical_min_mireds);
    attribute::create_color_temp_physical_max_mireds(cluster, config->color_temp_physical_max_mireds);
    attribute::create_couple_color_temp_to_level_min_mireds(cluster, config->couple_color_temp_to_level_min_mireds);
    attribute::create_startup_color_temperature_mireds(cluster, config->startup_color_temperature_mireds);

    /* Commands */
    command::create_move_to_color_temperature(cluster);
    command::create_stop_move_step(cluster);
    command::create_move_color_temperature(cluster);
    command::create_step_color_temperature(cluster);

    return ESP_OK;
}

} /* color_temperature */

namespace xy {

uint32_t get_id()
{
    return (uint32_t)ColorControl::ColorCapabilities::kXYAttributesSupported;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_current_x(cluster, config->current_x);
    attribute::create_current_y(cluster, config->current_y);

    /* Commands */
    command::create_move_to_color(cluster);
    command::create_stop_move_step(cluster);
    command::create_move_color(cluster);
    command::create_step_color(cluster);

    return ESP_OK;
}

} /* xy */

namespace enhanced_hue {

uint32_t get_id()
{
    return (uint32_t)chip::app::Clusters::ColorControl::ColorCapabilities::kEnhancedHueSupported;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_enhanced_current_hue(cluster, config->enhanced_current_hue);

    /* Commands */
    command::create_enhanced_move_to_hue(cluster);
    command::create_enhanced_move_hue(cluster);
    command::create_enhanced_step_hue(cluster);
    command::create_enhanced_move_to_hue_and_saturation(cluster);

    return ESP_OK;
}
} /* enhanced_hue */

namespace color_loop {
uint32_t get_id()
{
    return (uint32_t)chip::app::Clusters::ColorControl::ColorCapabilities::kColorLoopSupported;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_color_loop_active(cluster, config->color_loop_active);
    attribute::create_color_loop_direction(cluster, config->color_loop_direction);
    attribute::create_color_loop_time(cluster, config->color_loop_time);
    attribute::create_color_loop_start_enhanced_hue(cluster, config->color_loop_start_enhanced_hue);
    attribute::create_color_loop_stored_enhanced_hue(cluster, config->color_loop_stored_enhanced_hue);

    /* Commands */
    command::create_color_loop_set(cluster);

    return ESP_OK;
}
} /* color_loop */
} /* feature */
} /* color_control */

namespace diagnostics_network_wifi {
namespace feature {

namespace packets_counts {

uint32_t get_id()
{
    // The WiFiNetworkDiagnosticsFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return 0x01;
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_beacon_rx_count(cluster, 0);
    attribute::create_packet_multicast_rx_count(cluster, 0);
    attribute::create_packet_multicast_tx_count(cluster, 0);
    attribute::create_packet_unicast_rx_count(cluster, 0);
    attribute::create_packet_unicast_tx_count(cluster, 0);

    return ESP_OK;
}

} /* packets_counts */

namespace error_counts {

uint32_t get_id()
{
    // The WiFiNetworkDiagnosticsFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return 0x02;
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_beacon_lost_count(cluster, 0);
    attribute::create_overrun_count(cluster, 0);

    /* Commands */
    command::create_reset_counts(cluster);

    return ESP_OK;
}

} /* error_counts */

} /* feature */
} /* diagnostics_network_wifi */

} /* cluster */
} /* esp_matter */
