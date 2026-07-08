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

#include <descriptor.h>
#include <on_off.h>
#include <media_playback.h>
#include <keypad_input.h>

#include <esp_matter_core.h>

#define ESP_MATTER_BASIC_VIDEO_PLAYER_DEVICE_TYPE_ID 0x0028
#define ESP_MATTER_BASIC_VIDEO_PLAYER_DEVICE_TYPE_VERSION 2

namespace esp_matter {
namespace endpoint {
namespace basic_video_player {

typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::on_off::config_t on_off;
    cluster::media_playback::config_t media_playback;
    cluster::keypad_input::config_t keypad_input;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* basic_video_player */
} /* endpoint */
} /* esp_matter */
