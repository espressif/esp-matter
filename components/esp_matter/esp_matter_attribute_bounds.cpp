// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
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
#include <esp_matter_attribute_bounds.h>

static const char *TAG = "esp_matter_cluster";

using namespace chip::app::Clusters;

namespace esp_matter {
namespace cluster {

esp_err_t get_attribute_value(cluster_t *cluster, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    attribute_t *attribute = esp_matter::attribute::get(cluster, attribute_id);
    esp_err_t err = ESP_ERR_INVALID_ARG;
    if(attribute) {
        err = esp_matter::attribute::get_val(attribute, val);
    }
    return err;
}

namespace identify {

void add_bounds_cb(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster is NULL. Add bounds Failed!!");
        return;
    }
    attribute_t *current_attribute = esp_matter::attribute::get_first(cluster);
    if (!current_attribute) {
        ESP_LOGE(TAG, "Attribute is NULL.");
        return;
    }
    while(current_attribute) {
        switch(esp_matter::attribute::get_id(current_attribute)) {

            case Identify::Attributes::IdentifyType::Id: {
                uint8_t min = 0, max = 6;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_enum8(min), esp_matter_enum8(max));
                break;
            }
            default:
                ESP_LOGI(TAG, "No need to set boudnds for attribute!!!");
        }
        current_attribute = esp_matter::attribute::get_next(current_attribute);
    }
}

} /* identify */

namespace scenes_management {

void add_bounds_cb(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster is NULL. Add bounds Failed!!");
        return;
    }
    attribute_t *current_attribute = esp_matter::attribute::get_first(cluster);
    if (!current_attribute) {
        ESP_LOGE(TAG, "Attribute is NULL.");
        return;
    }
    while(current_attribute) {
        switch(esp_matter::attribute::get_id(current_attribute)) {

            case ScenesManagement::Attributes::SceneTableSize::Id: {
                uint16_t min = 16, max = UINT16_MAX;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_uint16(min), esp_matter_uint16(max));
                break;
            }
            default:
                ESP_LOGI(TAG, "No need to set boudnds for attribute!!!");
        }
        current_attribute = esp_matter::attribute::get_next(current_attribute);
    }
}
} /* scenes_management */

namespace on_off {

void add_bounds_cb(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster is NULL. Add bounds Failed!!");
        return;
    }
    attribute_t *current_attribute = esp_matter::attribute::get_first(cluster);
    if (!current_attribute) {
        ESP_LOGE(TAG, "Attribute is NULL.");
        return;
    }
    while(current_attribute) {
        switch(esp_matter::attribute::get_id(current_attribute)) {

            case OnOff::Attributes::StartUpOnOff::Id: {
                uint8_t min = 0, max = 2;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_nullable_enum8(min), esp_matter_nullable_enum8(max));
                break;
            }
            default:
                ESP_LOGI(TAG, "No need to set boudnds for attribute!!!");
        }
        current_attribute = esp_matter::attribute::get_next(current_attribute);
    }
}

} /* on_off */

namespace level_control {

void add_bounds_cb(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster is NULL. Add bounds Failed!!");
        return;
    }
    attribute_t *current_attribute = esp_matter::attribute::get_first(cluster);
    if (!current_attribute) {
        ESP_LOGE(TAG, "Attribute is NULL.");
        return;
    }
    while(current_attribute) {
        switch(esp_matter::attribute::get_id(current_attribute)) {

            case LevelControl::Attributes::CurrentLevel::Id: {
                esp_matter_attr_val_t min_val = esp_matter_invalid(NULL);
                esp_matter_attr_val_t max_val = esp_matter_invalid(NULL);
                uint16_t min = 0, max = UINT16_MAX;
                if(cluster::get_attribute_value(cluster, LevelControl::Attributes::MinLevel::Id,
                                        &min_val) == ESP_OK)
                {
                    min = min_val.val.u16;
                }
                if(cluster::get_attribute_value(cluster, LevelControl::Attributes::MaxLevel::Id,
                                        &max_val) == ESP_OK)
                {
                    max = max_val.val.u16;
                }
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_nullable_uint8(min), esp_matter_nullable_uint8(max));
                break;
            }
            case LevelControl::Attributes::MinLevel::Id: {
                esp_matter_attr_val_t max_val = esp_matter_invalid(NULL);
                uint8_t min = 1, max = UINT8_MAX;
                if(cluster::get_attribute_value(cluster, LevelControl::Attributes::MaxLevel::Id,
                                        &max_val) == ESP_OK)
                {
                    max = max_val.val.u8;
                }
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_uint8(min), esp_matter_uint8(max));
                break;
            }
            case LevelControl::Attributes::MaxLevel::Id: {
                esp_matter_attr_val_t min_val = esp_matter_invalid(NULL);
                uint8_t min = 1, max = UINT8_MAX;
                if(cluster::get_attribute_value(cluster, LevelControl::Attributes::MinLevel::Id,
                                        &min_val) == ESP_OK)
                {
                    min = min_val.val.u8;
                }
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_uint8(min), esp_matter_uint8(max));
                break;
            }
            case LevelControl::Attributes::CurrentFrequency::Id: {
                esp_matter_attr_val_t min_val = esp_matter_invalid(NULL);
                esp_matter_attr_val_t max_val = esp_matter_invalid(NULL);
                uint16_t min = 0, max = UINT16_MAX;
                if(cluster::get_attribute_value(cluster, LevelControl::Attributes::MinFrequency::Id,
                                        &min_val) == ESP_OK)
                {
                    min = min_val.val.u16;
                }
                if(cluster::get_attribute_value(cluster, LevelControl::Attributes::MaxFrequency::Id,
                                        &max_val) == ESP_OK)
                {
                    max = max_val.val.u16;
                }
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_uint16(min), esp_matter_uint16(max));
                break;
            }
            case LevelControl::Attributes::MinFrequency::Id: {
                esp_matter_attr_val_t max_val = esp_matter_invalid(NULL);
                uint16_t min = 0, max = UINT16_MAX;
                if(cluster::get_attribute_value(cluster, LevelControl::Attributes::MaxFrequency::Id,
                                        &max_val) == ESP_OK)
                {
                    max = max_val.val.u16;
                }
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_uint16(min), esp_matter_uint16(max));
                break;
            }
            case LevelControl::Attributes::MaxFrequency::Id: {
                esp_matter_attr_val_t min_val = esp_matter_invalid(NULL);
                uint16_t min = 0, max = UINT16_MAX;
                if(cluster::get_attribute_value(cluster, LevelControl::Attributes::MinFrequency::Id,
                                        &min_val) == ESP_OK)
                {
                    min = min_val.val.u16;
                }
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_uint16(min), esp_matter_uint16(max));
                break;
            }
            case LevelControl::Attributes::OnLevel::Id: {
                esp_matter_attr_val_t min_val = esp_matter_invalid(NULL);
                esp_matter_attr_val_t max_val = esp_matter_invalid(NULL);
                uint8_t min = 0, max = UINT8_MAX;
                if(cluster::get_attribute_value(cluster, LevelControl::Attributes::MinLevel::Id,
                                        &min_val) == ESP_OK)
                {
                    min = min_val.val.u8;
                }
                if(cluster::get_attribute_value(cluster, LevelControl::Attributes::MaxLevel::Id,
                                        &max_val) == ESP_OK)
                {
                    max = max_val.val.u8;
                }
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_nullable_uint8(min), esp_matter_nullable_uint8(max));
                break;
            }
            case LevelControl::Attributes::Options::Id: {
                uint8_t min = 0, max = 3;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_bitmap8(min), esp_matter_bitmap8(max));
                break;
            }
            default:
                ESP_LOGI(TAG, "No need to set boudnds for attribute!!!");
        }
        current_attribute = esp_matter::attribute::get_next(current_attribute);
    }
}

} /* level_control */

namespace color_control {

void add_bounds_cb(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster is NULL. Add bounds Failed!!");
        return;
    }
    attribute_t *current_attribute = esp_matter::attribute::get_first(cluster);
    if (!current_attribute) {
        ESP_LOGE(TAG, "Attribute is NULL.");
        return;
    }
    while(current_attribute) {
        switch(esp_matter::attribute::get_id(current_attribute)) {

            case ColorControl::Attributes::CurrentHue::Id:
            case ColorControl::Attributes::CurrentSaturation::Id: {
                uint8_t min = 0, max = 254;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_uint8(min), esp_matter_uint8(max));
                break;
            }
            case ColorControl::Attributes::RemainingTime::Id: {
                uint16_t min = 0, max = 65534;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_uint16(min), esp_matter_uint16(max));
                break;
            }
            case ColorControl::Attributes::CurrentX::Id:
            case ColorControl::Attributes::CurrentY::Id:
            case ColorControl::Attributes::ColorTemperatureMireds::Id:
            case ColorControl::Attributes::Primary1X::Id:
            case ColorControl::Attributes::Primary1Y::Id:
            case ColorControl::Attributes::Primary2X::Id:
            case ColorControl::Attributes::Primary2Y::Id:
            case ColorControl::Attributes::Primary3X::Id:
            case ColorControl::Attributes::Primary3Y::Id:
            case ColorControl::Attributes::Primary4X::Id:
            case ColorControl::Attributes::Primary4Y::Id:
            case ColorControl::Attributes::Primary5X::Id:
            case ColorControl::Attributes::Primary5Y::Id:
            case ColorControl::Attributes::Primary6X::Id:
            case ColorControl::Attributes::Primary6Y::Id:
            case ColorControl::Attributes::WhitePointX::Id:
            case ColorControl::Attributes::WhitePointY::Id:
            case ColorControl::Attributes::ColorPointRX::Id:
            case ColorControl::Attributes::ColorPointRY::Id:
            case ColorControl::Attributes::ColorPointGX::Id:
            case ColorControl::Attributes::ColorPointGY::Id:
            case ColorControl::Attributes::ColorPointBX::Id:
            case ColorControl::Attributes::ColorPointBY::Id:
            case ColorControl::Attributes::ColorTempPhysicalMinMireds::Id:
            case ColorControl::Attributes::ColorTempPhysicalMaxMireds::Id: {
                uint16_t min = 0, max = 65279;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_uint16(min), esp_matter_uint16(max));
                break;
            }
            case ColorControl::Attributes::DriftCompensation::Id: {
                uint8_t min = 0, max = 4;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_enum8(min), esp_matter_enum8(max));
                break;
            }
            case ColorControl::Attributes::ColorMode::Id: {
                uint8_t min = 0, max = 2;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_enum8(min), esp_matter_enum8(max));
                break;
            }
            case ColorControl::Attributes::Options::Id: {
                uint8_t min = 0, max = 1;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_bitmap8(min), esp_matter_bitmap8(max));
                break;
            }
            case ColorControl::Attributes::NumberOfPrimaries::Id: {
                uint8_t min = 0, max = 6;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_nullable_uint8(min), esp_matter_nullable_uint8(max));
                break;
            }
            case ColorControl::Attributes::EnhancedColorMode::Id: {
                uint8_t min = 0, max = 3;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_enum8(min), esp_matter_enum8(max));
                break;
            }
            case ColorControl::Attributes::ColorLoopActive::Id:
            case ColorControl::Attributes::ColorLoopDirection::Id: {
                uint8_t min = 0, max = 1;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_uint8(min), esp_matter_uint8(max));
                break;
            }
            case ColorControl::Attributes::ColorCapabilities::Id: {
                uint8_t min = 0, max = 31;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_bitmap16(min), esp_matter_bitmap16(max));
                break;
            }
            case ColorControl::Attributes::StartUpColorTemperatureMireds::Id: {
                uint16_t min = 0, max = 65279;
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_nullable_uint16(min), esp_matter_nullable_uint16(max));
                break;
            }
            case ColorControl::Attributes::CoupleColorTempToLevelMinMireds::Id: {
                esp_matter_attr_val_t min_val = esp_matter_invalid(NULL);
                esp_matter_attr_val_t max_val = esp_matter_invalid(NULL);
                uint16_t min = 0, max = UINT16_MAX;
                if(cluster::get_attribute_value(cluster, ColorControl::Attributes::ColorTempPhysicalMinMireds::Id,
                                        &min_val) == ESP_OK)
                {
                    min = min_val.val.u16;
                }
                if(cluster::get_attribute_value(cluster, ColorControl::Attributes::ColorTemperatureMireds::Id,
                                        &max_val) == ESP_OK)
                {
                    max = max_val.val.u16;
                }
                esp_matter::attribute::add_bounds(current_attribute, esp_matter_uint16(min), esp_matter_uint16(max));
                break;
            }
            default:
                ESP_LOGI(TAG, "No need to set boudnds for attribute!!!");
        }
        current_attribute = esp_matter::attribute::get_next(current_attribute);
    }
}

} /* color_control */

} /* cluster */
} /* esp_matter */
