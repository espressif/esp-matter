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
#include <camera_av_stream_management.h>
#include <camera_av_stream_management_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 2;

namespace esp_matter {
namespace cluster {
namespace camera_av_stream_management {

namespace feature {
namespace audio {
uint32_t get_id()
{
    return Audio::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_microphone_muted(cluster, config->microphone_muted);
    attribute::create_microphone_volume_level(cluster, config->microphone_volume_level);
    attribute::create_microphone_max_level(cluster, config->microphone_max_level);
    attribute::create_microphone_min_level(cluster, config->microphone_min_level);
    attribute::create_microphone_capabilities(cluster, NULL, 0, 0);
    attribute::create_allocated_audio_streams(cluster, NULL, 0, 0);
    command::create_audio_stream_allocate(cluster);
    command::create_audio_stream_allocate_response(cluster);
    command::create_audio_stream_deallocate(cluster);

    return ESP_OK;
}
} /* audio */

namespace video {
uint32_t get_id()
{
    return Video::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_max_concurrent_encoders(cluster, config->max_concurrent_encoders);
    attribute::create_max_encoded_pixel_rate(cluster, config->max_encoded_pixel_rate);
    attribute::create_current_frame_rate(cluster, config->current_frame_rate);
    attribute::create_local_video_recording_enabled(cluster, config->local_video_recording_enabled);
    attribute::create_video_sensor_params(cluster, NULL, 0, 0);
    attribute::create_min_viewport_resolution(cluster, NULL, 0, 0);
    attribute::create_rate_distortion_trade_off_points(cluster, NULL, 0, 0);
    attribute::create_allocated_video_streams(cluster, NULL, 0, 0);
    attribute::create_viewport(cluster, NULL, 0, 0);
    command::create_video_stream_allocate(cluster);
    command::create_video_stream_allocate_response(cluster);
    command::create_video_stream_modify(cluster);
    command::create_video_stream_deallocate(cluster);

    return ESP_OK;
}
} /* video */

namespace snapshot {
uint32_t get_id()
{
    return Snapshot::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_max_concurrent_encoders(cluster, config->max_concurrent_encoders);
    attribute::create_max_encoded_pixel_rate(cluster, config->max_encoded_pixel_rate);
    attribute::create_local_snapshot_recording_enabled(cluster, config->local_snapshot_recording_enabled);
    attribute::create_snapshot_capabilities(cluster, NULL, 0, 0);
    attribute::create_allocated_snapshot_streams(cluster, NULL, 0, 0);
    command::create_snapshot_stream_allocate(cluster);
    command::create_snapshot_stream_allocate_response(cluster);
    command::create_snapshot_stream_modify(cluster);
    command::create_snapshot_stream_deallocate(cluster);
    command::create_capture_snapshot(cluster);
    command::create_capture_snapshot_response(cluster);

    return ESP_OK;
}
} /* snapshot */

namespace privacy {
uint32_t get_id()
{
    return Privacy::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_soft_recording_privacy_mode_enabled(cluster, config->soft_recording_privacy_mode_enabled);
    attribute::create_soft_livestream_privacy_mode_enabled(cluster, config->soft_livestream_privacy_mode_enabled);

    return ESP_OK;
}
} /* privacy */

namespace speaker {
uint32_t get_id()
{
    return Speaker::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(audio), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_two_way_talk_support(cluster, config->two_way_talk_support);
    attribute::create_speaker_muted(cluster, config->speaker_muted);
    attribute::create_speaker_volume_level(cluster, config->speaker_volume_level);
    attribute::create_speaker_max_level(cluster, config->speaker_max_level);
    attribute::create_speaker_min_level(cluster, config->speaker_min_level);
    attribute::create_speaker_capabilities(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* speaker */

namespace image_control {
uint32_t get_id()
{
    return ImageControl::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(video)) || (has_feature(snapshot))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* image_control */

namespace watermark {
uint32_t get_id()
{
    return Watermark::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(video)) || (has_feature(snapshot))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_video_stream_modify(cluster);
    command::create_snapshot_stream_modify(cluster);

    return ESP_OK;
}
} /* watermark */

namespace on_screen_display {
uint32_t get_id()
{
    return OnScreenDisplay::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(video)) || (has_feature(snapshot))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_video_stream_modify(cluster);
    command::create_snapshot_stream_modify(cluster);

    return ESP_OK;
}
} /* on_screen_display */

namespace local_storage {
uint32_t get_id()
{
    return LocalStorage::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_local_video_recording_enabled(cluster, config->local_video_recording_enabled);
    attribute::create_local_snapshot_recording_enabled(cluster, config->local_snapshot_recording_enabled);

    return ESP_OK;
}
} /* local_storage */

namespace high_dynamic_range {
uint32_t get_id()
{
    return HighDynamicRange::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(video)) || (has_feature(snapshot))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_hdr_mode_enabled(cluster, config->hdr_mode_enabled);

    return ESP_OK;
}
} /* high_dynamic_range */

namespace night_vision {
uint32_t get_id()
{
    return NightVision::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((has_feature(video)) || (has_feature(snapshot))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_night_vision_uses_infrared(cluster, config->night_vision_uses_infrared);
    attribute::create_night_vision(cluster, config->night_vision);

    return ESP_OK;
}
} /* night_vision */

} /* feature */

namespace attribute {
attribute_t *create_max_concurrent_encoders(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(video)) || (has_feature(snapshot))), NULL);
    return esp_matter::attribute::create(cluster, MaxConcurrentEncoders::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_max_encoded_pixel_rate(cluster_t *cluster, uint32_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(video)) || (has_feature(snapshot))), NULL);
    return esp_matter::attribute::create(cluster, MaxEncodedPixelRate::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_video_sensor_params(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(video), NULL);
    return esp_matter::attribute::create(cluster, VideoSensorParams::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_night_vision_uses_infrared(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(night_vision), NULL);
    return esp_matter::attribute::create(cluster, NightVisionUsesInfrared::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_min_viewport_resolution(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(video), NULL);
    return esp_matter::attribute::create(cluster, MinViewportResolution::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_rate_distortion_trade_off_points(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(video), NULL);
    return esp_matter::attribute::create(cluster, RateDistortionTradeOffPoints::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_max_content_buffer_size(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, MaxContentBufferSize::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_microphone_capabilities(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(audio), NULL);
    return esp_matter::attribute::create(cluster, MicrophoneCapabilities::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_speaker_capabilities(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(speaker), NULL);
    return esp_matter::attribute::create(cluster, SpeakerCapabilities::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_two_way_talk_support(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(speaker), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, TwoWayTalkSupport::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(TwoWayTalkSupport::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(TwoWayTalkSupport::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_snapshot_capabilities(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(snapshot), NULL);
    return esp_matter::attribute::create(cluster, SnapshotCapabilities::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_max_network_bandwidth(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, MaxNetworkBandwidth::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_current_frame_rate(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(video), NULL);
    return esp_matter::attribute::create(cluster, CurrentFrameRate::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_hdr_mode_enabled(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(high_dynamic_range), NULL);
    return esp_matter::attribute::create(cluster, HDRModeEnabled::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_supported_stream_usages(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, SupportedStreamUsages::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_allocated_video_streams(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(video), NULL);
    return esp_matter::attribute::create(cluster, AllocatedVideoStreams::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_allocated_audio_streams(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(audio), NULL);
    return esp_matter::attribute::create(cluster, AllocatedAudioStreams::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_allocated_snapshot_streams(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(snapshot), NULL);
    return esp_matter::attribute::create(cluster, AllocatedSnapshotStreams::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_stream_usage_priorities(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, StreamUsagePriorities::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_soft_recording_privacy_mode_enabled(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(privacy), NULL);
    return esp_matter::attribute::create(cluster, SoftRecordingPrivacyModeEnabled::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_soft_livestream_privacy_mode_enabled(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(privacy), NULL);
    return esp_matter::attribute::create(cluster, SoftLivestreamPrivacyModeEnabled::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_hard_privacy_mode_on(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, HardPrivacyModeOn::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_night_vision(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(night_vision), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, NightVision::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(NightVision::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(NightVision::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_night_vision_illum(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, NightVisionIllum::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(NightVisionIllum::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(NightVisionIllum::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_viewport(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(video), NULL);
    return esp_matter::attribute::create(cluster, Viewport::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_speaker_muted(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(speaker), NULL);
    return esp_matter::attribute::create(cluster, SpeakerMuted::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_speaker_volume_level(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(speaker), NULL);
    return esp_matter::attribute::create(cluster, SpeakerVolumeLevel::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_speaker_max_level(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(speaker), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, SpeakerMaxLevel::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(SpeakerMaxLevel::Min)), esp_matter_attr_val(static_cast<uint8_t>(SpeakerMaxLevel::Max)));
    return attribute;
}

attribute_t *create_speaker_min_level(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(speaker), NULL);
    return esp_matter::attribute::create(cluster, SpeakerMinLevel::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_microphone_muted(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(audio), NULL);
    return esp_matter::attribute::create(cluster, MicrophoneMuted::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_microphone_volume_level(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(audio), NULL);
    return esp_matter::attribute::create(cluster, MicrophoneVolumeLevel::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_microphone_max_level(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(audio), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MicrophoneMaxLevel::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(MicrophoneMaxLevel::Min)), esp_matter_attr_val(static_cast<uint8_t>(MicrophoneMaxLevel::Max)));
    return attribute;
}

attribute_t *create_microphone_min_level(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(audio), NULL);
    return esp_matter::attribute::create(cluster, MicrophoneMinLevel::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_microphone_agc_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, MicrophoneAGCEnabled::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_image_rotation(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ImageRotation::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(ImageRotation::Min)), esp_matter_attr_val(static_cast<uint16_t>(ImageRotation::Max)));
    return attribute;
}

attribute_t *create_image_flip_horizontal(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, ImageFlipHorizontal::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_image_flip_vertical(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, ImageFlipVertical::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_local_video_recording_enabled(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(video)) && (has_feature(local_storage))), NULL);
    return esp_matter::attribute::create(cluster, LocalVideoRecordingEnabled::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_local_snapshot_recording_enabled(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(snapshot)) && (has_feature(local_storage))), NULL);
    return esp_matter::attribute::create(cluster, LocalSnapshotRecordingEnabled::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_status_light_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, StatusLightEnabled::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_status_light_brightness(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, StatusLightBrightness::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
}

} /* attribute */
namespace command {
command_t *create_audio_stream_allocate(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(audio), NULL);
    return esp_matter::command::create(cluster, AudioStreamAllocate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_audio_stream_allocate_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(audio), NULL);
    return esp_matter::command::create(cluster, AudioStreamAllocateResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_audio_stream_deallocate(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(audio), NULL);
    return esp_matter::command::create(cluster, AudioStreamDeallocate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_video_stream_allocate(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(video), NULL);
    return esp_matter::command::create(cluster, VideoStreamAllocate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_video_stream_allocate_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(video), NULL);
    return esp_matter::command::create(cluster, VideoStreamAllocateResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_video_stream_modify(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(video)) && (((has_feature(watermark)) || (has_feature(on_screen_display))))), NULL);
    return esp_matter::command::create(cluster, VideoStreamModify::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_video_stream_deallocate(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(video), NULL);
    return esp_matter::command::create(cluster, VideoStreamDeallocate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_snapshot_stream_allocate(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(snapshot), NULL);
    return esp_matter::command::create(cluster, SnapshotStreamAllocate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_snapshot_stream_allocate_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(snapshot), NULL);
    return esp_matter::command::create(cluster, SnapshotStreamAllocateResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_snapshot_stream_modify(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(snapshot)) && (((has_feature(watermark)) || (has_feature(on_screen_display))))), NULL);
    return esp_matter::command::create(cluster, SnapshotStreamModify::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_snapshot_stream_deallocate(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(snapshot), NULL);
    return esp_matter::command::create(cluster, SnapshotStreamDeallocate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_stream_priorities(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SetStreamPriorities::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_capture_snapshot(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(snapshot), NULL);
    return esp_matter::command::create(cluster, CaptureSnapshot::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_capture_snapshot_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(snapshot), NULL);
    return esp_matter::command::create(cluster, CaptureSnapshotResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, camera_av_stream_management::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, camera_av_stream_management::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterCameraAvStreamManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_max_content_buffer_size(cluster, config->max_content_buffer_size);
        attribute::create_max_network_bandwidth(cluster, config->max_network_bandwidth);
        attribute::create_supported_stream_usages(cluster, NULL, 0, 0);
        attribute::create_stream_usage_priorities(cluster, NULL, 0, 0);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("Audio,Video,Snapshot",
                                       feature::audio::get_id(), feature::video::get_id(), feature::snapshot::get_id());
        if (feature_map & feature::audio::get_id()) {
            VerifyOrReturnValue(feature::audio::add(cluster, &(config->features.audio)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::video::get_id()) {
            VerifyOrReturnValue(feature::video::add(cluster, &(config->features.video)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::snapshot::get_id()) {
            VerifyOrReturnValue(feature::snapshot::add(cluster, &(config->features.snapshot)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::privacy::get_id()) {
            VerifyOrReturnValue(feature::privacy::add(cluster, &(config->features.privacy)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::speaker::get_id()) {
            VerifyOrReturnValue(feature::speaker::add(cluster, &(config->features.speaker)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::image_control::get_id()) {
            VerifyOrReturnValue(feature::image_control::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::watermark::get_id()) {
            VerifyOrReturnValue(feature::watermark::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::on_screen_display::get_id()) {
            VerifyOrReturnValue(feature::on_screen_display::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::local_storage::get_id()) {
            VerifyOrReturnValue(feature::local_storage::add(cluster, &(config->features.local_storage)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::high_dynamic_range::get_id()) {
            VerifyOrReturnValue(feature::high_dynamic_range::add(cluster, &(config->features.high_dynamic_range)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::night_vision::get_id()) {
            VerifyOrReturnValue(feature::night_vision::add(cluster, &(config->features.night_vision)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        command::create_set_stream_priorities(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterCameraAvStreamManagementClusterServerInitCallback,
                                                 ESPMatterCameraAvStreamManagementClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* camera_av_stream_management */
} /* cluster */
} /* esp_matter */
