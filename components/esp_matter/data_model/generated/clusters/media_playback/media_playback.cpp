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
#include <app-common/zap-generated/callback.h>
#include <app/InteractionModelEngine.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <esp_matter_delegate_callbacks.h>
#include <media_playback.h>
#include <media_playback_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "media_playback_cluster";
constexpr uint16_t cluster_revision = 2;

static esp_err_t esp_matter_command_callback_play(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                  void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::Play::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterPlayCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_pause(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                   void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::Pause::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterPauseCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_stop(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                  void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::Stop::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterStopCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_start_over(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                        void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::StartOver::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterStartOverCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_previous(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                      void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::Previous::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterPreviousCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_next(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                  void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::Next::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterNextCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_rewind(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                    void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::Rewind::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterRewindCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_fast_forward(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                          void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::FastForward::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterFastForwardCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_skip_forward(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                          void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::SkipForward::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterSkipForwardCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_skip_backward(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                           void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::SkipBackward::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterSkipBackwardCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_seek(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                  void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::Seek::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterSeekCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_activate_audio_track(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                  void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::ActivateAudioTrack::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterActivateAudioTrackCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_activate_text_track(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                 void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::ActivateTextTrack::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterActivateTextTrackCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_deactivate_text_track(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                   void *opaque_ptr)
{
    chip::app::Clusters::MediaPlayback::Commands::DeactivateTextTrack::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfMediaPlaybackClusterDeactivateTextTrackCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace media_playback {

namespace feature {
namespace advanced_seek {
uint32_t get_id()
{
    return AdvancedSeek::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_start_time(cluster, 0);
    attribute::create_duration(cluster, 0);
    attribute::create_sampled_position(cluster, NULL, 0, 0);
    attribute::create_playback_speed(cluster, 0);
    attribute::create_seek_range_end(cluster, 0);
    attribute::create_seek_range_start(cluster, 0);
    command::create_seek(cluster);

    return ESP_OK;
}
} /* advanced_seek */

namespace variable_speed {
uint32_t get_id()
{
    return VariableSpeed::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_rewind(cluster);
    command::create_fast_forward(cluster);

    return ESP_OK;
}
} /* variable_speed */

namespace text_tracks {
uint32_t get_id()
{
    return TextTracks::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_active_text_track(cluster, NULL, 0, 0);
    attribute::create_available_text_tracks(cluster, NULL, 0, 0);
    command::create_activate_text_track(cluster);
    command::create_deactivate_text_track(cluster);

    return ESP_OK;
}
} /* text_tracks */

namespace audio_tracks {
uint32_t get_id()
{
    return AudioTracks::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_active_audio_track(cluster, NULL, 0, 0);
    attribute::create_available_audio_tracks(cluster, NULL, 0, 0);
    command::create_activate_audio_track(cluster);

    return ESP_OK;
}
} /* audio_tracks */

namespace audio_advance {
uint32_t get_id()
{
    return AudioAdvance::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* audio_advance */

} /* feature */

namespace attribute {
attribute_t *create_current_state(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CurrentState::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
}

attribute_t *create_start_time(cluster_t *cluster, nullable<uint64_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(advanced_seek), NULL);
    return esp_matter::attribute::create(cluster, StartTime::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_duration(cluster_t *cluster, nullable<uint64_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(advanced_seek), NULL);
    return esp_matter::attribute::create(cluster, Duration::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_sampled_position(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(advanced_seek), NULL);
    return esp_matter::attribute::create(cluster, SampledPosition::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_playback_speed(cluster_t *cluster, float value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(advanced_seek), NULL);
    return esp_matter::attribute::create(cluster, PlaybackSpeed::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value));
}

attribute_t *create_seek_range_end(cluster_t *cluster, nullable<uint64_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(advanced_seek), NULL);
    return esp_matter::attribute::create(cluster, SeekRangeEnd::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_seek_range_start(cluster_t *cluster, nullable<uint64_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(advanced_seek), NULL);
    return esp_matter::attribute::create(cluster, SeekRangeStart::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_active_audio_track(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(audio_tracks), NULL);
    return esp_matter::attribute::create(cluster, ActiveAudioTrack::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_available_audio_tracks(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(audio_tracks), NULL);
    return esp_matter::attribute::create(cluster, AvailableAudioTracks::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_active_text_track(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(text_tracks), NULL);
    return esp_matter::attribute::create(cluster, ActiveTextTrack::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_available_text_tracks(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(text_tracks), NULL);
    return esp_matter::attribute::create(cluster, AvailableTextTracks::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_play(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Play::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_play);
}

command_t *create_pause(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Pause::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_pause);
}

command_t *create_stop(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Stop::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_stop);
}

command_t *create_start_over(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StartOver::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_start_over);
}

command_t *create_previous(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Previous::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_previous);
}

command_t *create_next(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Next::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_next);
}

command_t *create_rewind(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(variable_speed), NULL);
    return esp_matter::command::create(cluster, Rewind::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_rewind);
}

command_t *create_fast_forward(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(variable_speed), NULL);
    return esp_matter::command::create(cluster, FastForward::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_fast_forward);
}

command_t *create_skip_forward(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SkipForward::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_skip_forward);
}

command_t *create_skip_backward(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SkipBackward::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_skip_backward);
}

command_t *create_playback_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, PlaybackResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_seek(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(advanced_seek), NULL);
    return esp_matter::command::create(cluster, Seek::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_seek);
}

command_t *create_activate_audio_track(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(audio_tracks), NULL);
    return esp_matter::command::create(cluster, ActivateAudioTrack::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_activate_audio_track);
}

command_t *create_activate_text_track(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(text_tracks), NULL);
    return esp_matter::command::create(cluster, ActivateTextTrack::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_activate_text_track);
}

command_t *create_deactivate_text_track(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(text_tracks), NULL);
    return esp_matter::command::create(cluster, DeactivateTextTrack::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_deactivate_text_track);
}

} /* command */

namespace event {
event_t *create_state_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, StateChanged::Id);
}

} /* event */

static void create_default_binding_cluster(endpoint_t *endpoint)
{
    binding::config_t config;
    binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, media_playback::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, media_playback::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = MediaPlaybackDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterMediaPlaybackPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_current_state(cluster, 0);
        command::create_play(cluster);
        command::create_pause(cluster);
        command::create_stop(cluster);
        command::create_playback_response(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* media_playback */
} /* cluster */
} /* esp_matter */
