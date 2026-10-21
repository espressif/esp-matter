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

#pragma once

#include <lib/core/ScopedNodeId.h>

namespace esp_matter {
namespace client {

enum class target_type_t : uint8_t { node, group };

/** @brief Resolved node/group and fabric, available from native delegate callbacks. */
struct target_t {
    explicit target_t(const chip::ScopedNodeId &peer)
        : type(target_type_t::node), fabric_index(peer.GetFabricIndex()), node_id(peer.GetNodeId()) {}
    target_t(chip::GroupId group_id, chip::FabricIndex fabric_index)
        : type(target_type_t::group), fabric_index(fabric_index), group_id(group_id) {}

    target_type_t type;
    chip::FabricIndex fabric_index;
    union {
        chip::NodeId node_id;
        chip::GroupId group_id;
    };
};

} // namespace client
} // namespace esp_matter
