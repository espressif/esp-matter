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

static const char *TAG = "esp_matter_cluster";

esp_err_t esp_matter_cluster_update_feature_map(esp_matter_cluster_t *cluster, uint32_t value)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    /* Get the attribute */
    esp_matter_attribute_t *attribute = esp_matter_attribute_get(cluster, ZCL_FEATURE_MAP_SERVER_ATTRIBUTE_ID);

    /* Create the attribute with the new value if it does not exist */
    if (!attribute) {
        attribute = esp_matter_attribute_create_feature_map(cluster, value);
        if (!attribute) {
            ESP_LOGE(TAG, "Could not create feature map attribute");
            return ESP_FAIL;
        }
        return ESP_OK;
    }

    /* Update the value if the attribute already exists */
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    esp_matter_attribute_get_val(attribute, &val);
    val.val.u32 |= value;
    /* Here we can't call esp_matter_attribute_update() since the chip stack would not have started yet, since we are
    still creating the data model. So, we are directly using esp_matter_attribute_set_val(). */
    return esp_matter_attribute_set_val(attribute, &val);
}

esp_err_t esp_matter_on_off_cluster_add_feature_lighting(esp_matter_cluster_t *cluster,
                                                         esp_matter_on_off_cluster_lighting_config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    esp_matter_cluster_update_feature_map(cluster, ESP_MATTER_ON_OFF_CLUSTER_LIGHTING_FEATURE_ID);

    /* Attributes not managed internally */
    esp_matter_attribute_create_global_scene_control(cluster, config->global_scene_control);
    esp_matter_attribute_create_on_time(cluster, config->on_time);
    esp_matter_attribute_create_off_wait_time(cluster, config->off_wait_time);
    esp_matter_attribute_create_start_up_on_off(cluster, config->start_up_on_off);

    /* Commands */
    esp_matter_command_create_off_with_effect(cluster);
    esp_matter_command_create_on_with_recall_global_scene(cluster);
    esp_matter_command_create_on_with_timed_off(cluster);

    return ESP_OK;
}

esp_err_t esp_matter_level_control_cluster_add_feature_on_off(esp_matter_cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    esp_matter_cluster_update_feature_map(cluster, ESP_MATTER_LEVEL_CONTROL_CLUSTER_ON_OFF_FEATURE_ID);

    return ESP_OK;
}

esp_err_t esp_matter_level_control_cluster_add_feature_lighting(esp_matter_cluster_t *cluster,
                                                            esp_matter_level_control_cluster_lighting_config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    esp_matter_cluster_update_feature_map(cluster, ESP_MATTER_LEVEL_CONTROL_CLUSTER_LIGHTING_FEATURE_ID);

    /* Attributes not managed internally */
    esp_matter_attribute_create_remaining_time(cluster, config->remaining_time);
    esp_matter_attribute_create_min_level(cluster, config->min_level);
    esp_matter_attribute_create_max_level(cluster, config->max_level);
    esp_matter_attribute_create_start_up_current_level(cluster, config->start_up_current_level);

    return ESP_OK;
}

esp_err_t esp_matter_color_control_cluster_add_feature_hue_saturation(esp_matter_cluster_t *cluster,
                                                    esp_matter_color_control_cluster_hue_saturation_config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    esp_matter_cluster_update_feature_map(cluster, ESP_MATTER_COLOR_CONTROL_CLUSTER_HUE_SATURATION_FEATURE_ID);

    /* Attributes not managed internally */
    esp_matter_attribute_create_current_hue(cluster, config->current_hue);
    esp_matter_attribute_create_current_saturation(cluster, config->current_saturation);

    /* Commands */
    esp_matter_command_create_move_to_hue(cluster);
    esp_matter_command_create_move_hue(cluster);
    esp_matter_command_create_step_hue(cluster);
    esp_matter_command_create_move_to_saturation(cluster);
    esp_matter_command_create_move_saturation(cluster);
    esp_matter_command_create_step_saturation(cluster);
    esp_matter_command_create_move_to_hue_and_saturation(cluster);
    esp_matter_command_create_stop_move_step(cluster);

    return ESP_OK;
}
