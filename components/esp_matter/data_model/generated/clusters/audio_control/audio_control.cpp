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
#include <audio_control.h>
#include <audio_control_ids.h>
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
namespace audio_control {

namespace feature {
namespace decibel {
uint32_t get_id()
{
    return Decibel::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* decibel */

namespace basic_equalizer {
uint32_t get_id()
{
    return BasicEqualizer::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_min_correction(cluster, config->min_correction);
    attribute::create_max_correction(cluster, config->max_correction);

    return ESP_OK;
}
} /* basic_equalizer */

} /* feature */

namespace attribute {
attribute_t *create_soft_muted(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, SoftMuted::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_physically_muted(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, PhysicallyMuted::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_volume(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, Volume::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_min_device_volume(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinDeviceVolume::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(MinDeviceVolume::Min)), esp_matter_attr_val(static_cast<uint16_t>(MinDeviceVolume::Max)));
    return attribute;
}

attribute_t *create_max_device_volume(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, MaxDeviceVolume::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_max_device_volume_db(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, MaxDeviceVolumeDB::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_max_user_volume(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, MaxUserVolume::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_default_step_size(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DefaultStepSize::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(DefaultStepSize::Min)), esp_matter_attr_val(static_cast<uint16_t>(DefaultStepSize::Max)));
    return attribute;
}

attribute_t *create_set_volume_unmute_policy(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SetVolumeUnmutePolicy::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(SetVolumeUnmutePolicy::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(SetVolumeUnmutePolicy::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_increase_volume_unmute_policy(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, IncreaseVolumeUnmutePolicy::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(IncreaseVolumeUnmutePolicy::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(IncreaseVolumeUnmutePolicy::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_increase_volume_unmute_volume(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, IncreaseVolumeUnmuteVolume::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(IncreaseVolumeUnmuteVolume::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(IncreaseVolumeUnmuteVolume::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_decrease_volume_unmute_policy(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DecreaseVolumeUnmutePolicy::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(DecreaseVolumeUnmutePolicy::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(DecreaseVolumeUnmutePolicy::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_start_up_muted(cluster_t *cluster, nullable<bool> value)
{
    return esp_matter::attribute::create(cluster, StartUpMuted::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_start_up_volume(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, StartUpVolume::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_bass(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Bass::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_mid(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Mid::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_treble(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Treble::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_min_correction(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(basic_equalizer), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinCorrection::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(MinCorrection::Min)), esp_matter_attr_val(static_cast<int16_t>(MinCorrection::Max)));
    return attribute;
}

attribute_t *create_max_correction(cluster_t *cluster, int16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(basic_equalizer), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxCorrection::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int16_t>(MaxCorrection::Min)), esp_matter_attr_val(static_cast<int16_t>(MaxCorrection::Max)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_mute(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Mute::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_unmute(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Unmute::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_toggle_muted(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ToggleMuted::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_volume(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SetVolume::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_increase_volume(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, IncreaseVolume::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_decrease_volume(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DecreaseVolume::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, audio_control::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, audio_control::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterAudioControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_soft_muted(cluster, config->soft_muted);
        attribute::create_volume(cluster, config->volume);
        attribute::create_min_device_volume(cluster, config->min_device_volume);
        attribute::create_max_device_volume(cluster, config->max_device_volume);
        attribute::create_default_step_size(cluster, config->default_step_size);
        attribute::create_set_volume_unmute_policy(cluster, config->set_volume_unmute_policy);
        attribute::create_increase_volume_unmute_policy(cluster, config->increase_volume_unmute_policy);
        attribute::create_increase_volume_unmute_volume(cluster, config->increase_volume_unmute_volume);
        attribute::create_decrease_volume_unmute_policy(cluster, config->decrease_volume_unmute_policy);
        command::create_mute(cluster);
        command::create_unmute(cluster);
        command::create_toggle_muted(cluster);
        command::create_set_volume(cluster);
        command::create_increase_volume(cluster);
        command::create_decrease_volume(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterAudioControlClusterServerInitCallback,
                                                 ESPMatterAudioControlClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* audio_control */
} /* cluster */
} /* esp_matter */
