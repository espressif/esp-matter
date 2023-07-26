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
}

namespace cluster {
namespace matter_controller {

static constexpr chip::ClusterId Id = 0x131BFC01;

namespace attribute {

namespace refresh_token {
static constexpr chip::AttributeId Id = 0x00000000;
} // namespace refresh_token

namespace access_token {
static constexpr chip::AttributeId Id = 0x00000001;
} // namespace access_token

namespace authorized {
static constexpr chip::AttributeId Id = 0x00000002;
} // namespace authorized

namespace user_noc_installed {
static constexpr chip::AttributeId Id = 0x00000003;
} // namespace user_noc_installed

namespace endpoint_url {
static constexpr chip::AttributeId Id = 0x00000004;
} // namespace endpoint_url

namespace rainmaker_group_id {
static constexpr chip::AttributeId Id = 0x00000005;
} // namespace rainmaker_group_id

namespace user_noc_fabric_index {
static constexpr chip::AttributeId Id = 0x00000006;
} // namespace user_noc_fabric_index

esp_err_t refresh_token_attribute_get(uint16_t endpoint_id, char *refresh_token);
attribute_t *create_refresh_token(cluster_t *cluster, char *value, uint16_t length);
esp_err_t access_token_attribute_get(uint16_t endpoint_id, char *access_token);
attribute_t *create_access_token(cluster_t *cluster, char *value, uint16_t length);
esp_err_t authorized_attribute_update(uint16_t endpoint_id, bool authorized);
esp_err_t authorized_attribute_get(uint16_t endpoint_id, bool &authorized);
attribute_t *create_authorized(cluster_t *cluster, bool value);
esp_err_t user_noc_installed_attribute_get(uint16_t endpoint_id, bool &user_noc_installed);
attribute_t *create_user_noc_installed(cluster_t *cluster, bool value);
esp_err_t endpoint_url_attribute_get(uint16_t endpoint_id, char *endpoint_url);
attribute_t *create_endpoint_url(cluster_t *cluster, char *value, uint16_t length);
esp_err_t rainmaker_group_id_attribute_get(uint16_t endpoint_id, char *group_id);
attribute_t *create_rainmaker_group_id(cluster_t *cluster, char *value, uint16_t length);
esp_err_t user_noc_fabric_index_attribute_get(uint16_t endpoint_id, uint8_t &user_noc_fabric_index);
attribute_t *create_user_noc_fabric_index(cluster_t *cluster, uint8_t user_noc_fabric_index);

} // namespace attribute

namespace command {

namespace append_refresh_token {
static constexpr chip::CommandId Id = 0x00000000;
} // namespace append_refresh_token
command_t *create_append_refresh_token(cluster_t *cluster);

namespace reset_refresh_token {
static constexpr chip::CommandId Id = 0x00000001;
} // namespace reset_refresh_token
command_t *create_reset_refresh_token(cluster_t *cluster);

namespace authorize {
static constexpr chip::CommandId Id = 0x00000002;
} // namespace authorize
command_t *create_authorize(cluster_t *cluster);

namespace update_user_noc {
static constexpr chip::CommandId Id = 0x00000003;
} // namespace update_user_noc
command_t *create_update_user_noc(cluster_t *cluster);

namespace update_device_list {
static constexpr chip::CommandId Id = 0x00000004;
} // namespace update_device_list
command_t *create_update_device_list(cluster_t *cluster);

} // namespace command
cluster_t *create(endpoint_t *endpoint, uint8_t flags);
} // namespace matter_controller
} // namespace cluster
} // namespace esp_matter
