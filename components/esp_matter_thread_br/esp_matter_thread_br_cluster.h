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

namespace esp_matter {

namespace cluster {
namespace thread_br {

static constexpr chip::ClusterId Id = 0x131BFC02;

namespace attribute {

namespace dataset_tlvs {
static constexpr chip::AttributeId Id = 0x00000000;
} // namespace dataset_tlvs
attribute_t *create_dataset_tlvs(cluster_t *cluster, uint8_t *value, uint16_t length);

namespace role {
static constexpr chip::AttributeId Id = 0x00000001;
} // namespace role
attribute_t *create_role(cluster_t *cluster, uint8_t role);

namespace border_agent_id {
static constexpr chip::AttributeId Id = 0x00000002;
} // namespace border_agent_id
attribute_t *create_border_agent_id(cluster_t *cluster, uint8_t *value, uint16_t length);


} // namespace attribute

namespace command {

namespace configure_dataset_tlvs {
static constexpr chip::CommandId Id = 0x00000000;
} // namespace configure_dataset_tlvs
command_t *create_configure_dataset_tlvs(cluster_t *cluster);

namespace start_thread {
static constexpr chip::CommandId Id = 0x00000001;
} // namespace start_thread
command_t *create_start_thread(cluster_t *cluster);

namespace stop_thread {
static constexpr chip::CommandId Id = 0x00000002;
} // namespace stop_thread
command_t *create_stop_thread(cluster_t *cluster);

} // namespace command

cluster_t *create(endpoint_t *endpoint, uint8_t flags);

} // namespace thread_br
} // namespace cluster

} // namespace esp_matter
