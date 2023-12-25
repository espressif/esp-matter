// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include <esp_err.h>
#include <esp_matter.h>
#include <esp_matter_core.h>

#define ESP_MATTER_RAINMAKER_MAX_ACCESS_TOKEN_LEN 2048
#define ESP_MATTER_RAINMAKER_MAX_REFRESH_TOKEN_LEN 2048
#define ESP_MATTER_RAINMAKER_MAX_ENDPOINT_URL_LEN 64
#define ESP_MATTER_RAINMAKER_MAX_GROUP_ID_LEN 24
#define HTTP_API_VERSION "v1"

namespace esp_matter {

namespace controller {
esp_err_t controller_authorize(uint16_t endpoint_id);
const char *get_current_access_token();
}

namespace cluster {
namespace matter_controller {

static constexpr chip::ClusterId Id = 0x131BFC01;

namespace attribute {

namespace refresh_token {
static constexpr chip::AttributeId Id = 0x00000000;
esp_err_t get(uint16_t endpoint_id, char *refresh_token);
attribute_t *create(cluster_t *cluster, char *value, uint16_t length);
} // namespace refresh_token

namespace refresh_token_verified {
static constexpr chip::AttributeId Id = 0x00000001;
esp_err_t get(uint16_t endpoint_id, bool &refresh_token_verified);
attribute_t *create(cluster_t *cluster, bool refresh_token_verified);
} // namespace refresh_token_verified

namespace authorized {
static constexpr chip::AttributeId Id = 0x00000002;
esp_err_t update(uint16_t endpoint_id, bool authorized);
esp_err_t get(uint16_t endpoint_id, bool &authorized);
attribute_t *create(cluster_t *cluster, bool value);
} // namespace authorized

namespace user_noc_installed {
static constexpr chip::AttributeId Id = 0x00000003;
esp_err_t get(uint16_t endpoint_id, bool &user_noc_installed);
attribute_t *create(cluster_t *cluster, bool value);
} // namespace user_noc_installed

namespace endpoint_url {
static constexpr chip::AttributeId Id = 0x00000004;
esp_err_t get(uint16_t endpoint_id, char *endpoint_url);
attribute_t *create(cluster_t *cluster, char *value, uint16_t length);
} // namespace endpoint_url

namespace rainmaker_group_id {
static constexpr chip::AttributeId Id = 0x00000005;
esp_err_t get(uint16_t endpoint_id, char *group_id);
attribute_t *create(cluster_t *cluster, char *value, uint16_t length);
} // namespace rainmaker_group_id

namespace user_noc_fabric_index {
static constexpr chip::AttributeId Id = 0x00000006;
esp_err_t get(uint16_t endpoint_id, uint8_t &user_noc_fabric_index);
attribute_t *create(cluster_t *cluster, uint8_t user_noc_fabric_index);
} // namespace user_noc_fabric_index

} // namespace attribute

namespace command {

namespace append_refresh_token {
static constexpr chip::CommandId Id = 0x00000000;
command_t *create(cluster_t *cluster);
} // namespace append_refresh_token

namespace reset_refresh_token {
static constexpr chip::CommandId Id = 0x00000001;
command_t *create(cluster_t *cluster);
} // namespace reset_refresh_token

namespace authorize {
static constexpr chip::CommandId Id = 0x00000002;
command_t *create(cluster_t *cluster);
} // namespace authorize

namespace update_user_noc {
static constexpr chip::CommandId Id = 0x00000003;
command_t *create(cluster_t *cluster);
} // namespace update_user_noc

namespace update_device_list {
static constexpr chip::CommandId Id = 0x00000004;
command_t *create(cluster_t *cluster);
} // namespace update_device_list

} // namespace command
cluster_t *create(endpoint_t *endpoint, uint8_t flags);
} // namespace matter_controller
} // namespace cluster
} // namespace esp_matter
