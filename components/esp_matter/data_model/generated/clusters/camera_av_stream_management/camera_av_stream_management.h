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

#pragma once
#include <esp_matter_data_model.h>

namespace esp_matter {
namespace cluster {
namespace camera_av_stream_management {

namespace feature {
namespace audio {
typedef struct config {
    bool microphone_muted;
    uint8_t microphone_volume_level;
    uint8_t microphone_max_level;
    uint8_t microphone_min_level;
    config() : microphone_muted(false), microphone_volume_level(0), microphone_max_level(0), microphone_min_level(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* audio */

namespace video {
typedef struct config {
    uint8_t max_concurrent_encoders;
    uint32_t max_encoded_pixel_rate;
    uint16_t current_frame_rate;
    bool local_video_recording_enabled;
    config() : max_concurrent_encoders(0), max_encoded_pixel_rate(0), current_frame_rate(0), local_video_recording_enabled(false) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* video */

namespace snapshot {
typedef struct config {
    uint8_t max_concurrent_encoders;
    uint32_t max_encoded_pixel_rate;
    bool local_snapshot_recording_enabled;
    config() : max_concurrent_encoders(0), max_encoded_pixel_rate(0), local_snapshot_recording_enabled(false) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* snapshot */

namespace privacy {
typedef struct config {
    bool soft_recording_privacy_mode_enabled;
    bool soft_livestream_privacy_mode_enabled;
    config() : soft_recording_privacy_mode_enabled(false), soft_livestream_privacy_mode_enabled(false) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* privacy */

namespace speaker {
typedef struct config {
    uint8_t two_way_talk_support;
    bool speaker_muted;
    uint8_t speaker_volume_level;
    uint8_t speaker_max_level;
    uint8_t speaker_min_level;
    config() : two_way_talk_support(0), speaker_muted(false), speaker_volume_level(0), speaker_max_level(0), speaker_min_level(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* speaker */

namespace image_control {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* image_control */

namespace watermark {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* watermark */

namespace on_screen_display {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* on_screen_display */

namespace local_storage {
typedef struct config {
    bool local_video_recording_enabled;
    bool local_snapshot_recording_enabled;
    config() : local_video_recording_enabled(false), local_snapshot_recording_enabled(false) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* local_storage */

namespace high_dynamic_range {
typedef struct config {
    bool hdr_mode_enabled;
    config() : hdr_mode_enabled(false) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* high_dynamic_range */

namespace night_vision {
typedef struct config {
    bool night_vision_uses_infrared;
    uint8_t night_vision;
    config() : night_vision_uses_infrared(false), night_vision(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* night_vision */

} /* feature */

namespace attribute {
attribute_t *create_max_concurrent_encoders(cluster_t *cluster, uint8_t value);
attribute_t *create_max_encoded_pixel_rate(cluster_t *cluster, uint32_t value);
attribute_t *create_video_sensor_params(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_night_vision_uses_infrared(cluster_t *cluster, bool value);
attribute_t *create_min_viewport_resolution(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_rate_distortion_trade_off_points(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_max_content_buffer_size(cluster_t *cluster, uint32_t value);
attribute_t *create_microphone_capabilities(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_speaker_capabilities(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_two_way_talk_support(cluster_t *cluster, uint8_t value);
attribute_t *create_snapshot_capabilities(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_max_network_bandwidth(cluster_t *cluster, uint32_t value);
attribute_t *create_current_frame_rate(cluster_t *cluster, uint16_t value);
attribute_t *create_hdr_mode_enabled(cluster_t *cluster, bool value);
attribute_t *create_supported_stream_usages(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_allocated_video_streams(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_allocated_audio_streams(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_allocated_snapshot_streams(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_stream_usage_priorities(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_soft_recording_privacy_mode_enabled(cluster_t *cluster, bool value);
attribute_t *create_soft_livestream_privacy_mode_enabled(cluster_t *cluster, bool value);
attribute_t *create_hard_privacy_mode_on(cluster_t *cluster, bool value);
attribute_t *create_night_vision(cluster_t *cluster, uint8_t value);
attribute_t *create_night_vision_illum(cluster_t *cluster, uint8_t value);
attribute_t *create_viewport(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_speaker_muted(cluster_t *cluster, bool value);
attribute_t *create_speaker_volume_level(cluster_t *cluster, uint8_t value);
attribute_t *create_speaker_max_level(cluster_t *cluster, uint8_t value);
attribute_t *create_speaker_min_level(cluster_t *cluster, uint8_t value);
attribute_t *create_microphone_muted(cluster_t *cluster, bool value);
attribute_t *create_microphone_volume_level(cluster_t *cluster, uint8_t value);
attribute_t *create_microphone_max_level(cluster_t *cluster, uint8_t value);
attribute_t *create_microphone_min_level(cluster_t *cluster, uint8_t value);
attribute_t *create_microphone_agc_enabled(cluster_t *cluster, bool value);
attribute_t *create_image_rotation(cluster_t *cluster, uint16_t value);
attribute_t *create_image_flip_horizontal(cluster_t *cluster, bool value);
attribute_t *create_image_flip_vertical(cluster_t *cluster, bool value);
attribute_t *create_local_video_recording_enabled(cluster_t *cluster, bool value);
attribute_t *create_local_snapshot_recording_enabled(cluster_t *cluster, bool value);
attribute_t *create_status_light_enabled(cluster_t *cluster, bool value);
attribute_t *create_status_light_brightness(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace command {
command_t *create_audio_stream_allocate(cluster_t *cluster);
command_t *create_audio_stream_allocate_response(cluster_t *cluster);
command_t *create_audio_stream_deallocate(cluster_t *cluster);
command_t *create_video_stream_allocate(cluster_t *cluster);
command_t *create_video_stream_allocate_response(cluster_t *cluster);
command_t *create_video_stream_modify(cluster_t *cluster);
command_t *create_video_stream_deallocate(cluster_t *cluster);
command_t *create_snapshot_stream_allocate(cluster_t *cluster);
command_t *create_snapshot_stream_allocate_response(cluster_t *cluster);
command_t *create_snapshot_stream_modify(cluster_t *cluster);
command_t *create_snapshot_stream_deallocate(cluster_t *cluster);
command_t *create_set_stream_priorities(cluster_t *cluster);
command_t *create_capture_snapshot(cluster_t *cluster);
command_t *create_capture_snapshot_response(cluster_t *cluster);
} /* command */

typedef struct config {
    uint32_t max_content_buffer_size;
    uint32_t max_network_bandwidth;
    void *delegate;
    struct {
        feature::audio::config_t audio;
        feature::video::config_t video;
        feature::snapshot::config_t snapshot;
        feature::privacy::config_t privacy;
        feature::speaker::config_t speaker;
        feature::local_storage::config_t local_storage;
        feature::high_dynamic_range::config_t high_dynamic_range;
        feature::night_vision::config_t night_vision;
    } features;
    uint32_t feature_flags;
    config() : max_content_buffer_size(0), max_network_bandwidth(0), delegate(nullptr), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* camera_av_stream_management */
} /* cluster */
} /* esp_matter */
