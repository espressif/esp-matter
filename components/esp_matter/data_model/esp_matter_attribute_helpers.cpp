// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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

#include <data_model/esp_matter_attribute_helpers.h>

#include <clusters/shared/GlobalIds.h>
#include <lib/support/CodeUtils.h>

namespace esp_matter {

uint32_t read_feature_map_u32(chip::EndpointId endpointId, chip::ClusterId clusterId)
{
    uint32_t feature_map;
    VerifyOrReturnValue(read_attribute_raw_value(endpointId, clusterId, chip::app::Clusters::Globals::Attributes::FeatureMap::Id, feature_map), 0);
    return feature_map;
}

uint32_t read_feature_map_u32(cluster_t *cluster)
{
    esp_matter_attr_val_t val;
    VerifyOrReturnValue(get_stored_attr_val(cluster, chip::app::Clusters::Globals::Attributes::FeatureMap::Id, val) == ESP_OK, 0);
    // FeatureMap is a bitmap32 global attribute; esp-matter stores it as BITMAP32.
    VerifyOrReturnValue(val.type == ESP_MATTER_VAL_TYPE_BITMAP32, 0);
    return val.val.u32;
}

bool is_attribute_enabled(cluster_t *cluster, uint32_t attribute_id)
{
    return attribute::get(cluster, attribute_id) != nullptr;
}

bool is_command_enabled(cluster_t *cluster, uint32_t command_id, uint16_t flags)
{
    return command::get(cluster, command_id, flags) != nullptr;
}

esp_err_t get_stored_attr_val(cluster_t *cluster, uint32_t attribute_id, esp_matter_attr_val_t &val)
{
    attribute_t *attr = attribute::get(cluster, attribute_id);
    VerifyOrReturnValue(attr != nullptr, ESP_FAIL);
    return attribute::get_val_internal(attr, &val);
}

} // namespace esp_matter
