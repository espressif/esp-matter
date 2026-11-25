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

#include <lib/core/CHIPError.h>
#include <lib/core/ScopedNodeId.h>

namespace esp_matter {
namespace client {

/** @brief Local provider or remote CASE dispatch. */
enum class execution_t : uint8_t {
    remote,
    local,
};

/**
 * @brief Validate a scoped node and select local or remote dispatch.
 *
 * Call this before start() if authorization depends on whether execution is local or remote.
 * Otherwise, start() selects the route itself.
 * Requires an initialized Interaction Model fabric table. Only the server's node on the supplied fabric is local;
 * client-only builds always select remote execution. This does not check whether local control is enabled.
 * Output is unusable on error.
 *
 * @param[in] node Operational node and fabric.
 * @param[out] execution Selected execution route.
 *
 * @return
 *      - CHIP_NO_ERROR on success
 *      - CHIP_ERROR_INVALID_ARGUMENT if the node is not operational
 *      - CHIP_ERROR_INCORRECT_STATE if the fabric table is unavailable
 *      - CHIP_ERROR_INVALID_FABRIC_INDEX if the fabric is unknown
 */
CHIP_ERROR resolve_target(const chip::ScopedNodeId &node, execution_t &execution);

enum class target_type_t : uint8_t { node, group };

/** @brief Callback destination and fabric, including the execution route for node targets. */
struct target_t {
    explicit target_t(const chip::ScopedNodeId &peer, execution_t execution = execution_t::remote)
        : type(target_type_t::node), execution(execution), fabric_index(peer.GetFabricIndex()), node_id(peer.GetNodeId()) {}
    target_t(chip::GroupId group_id, chip::FabricIndex fabric_index)
        : type(target_type_t::group), fabric_index(fabric_index), group_id(group_id) {}

    target_type_t type;
    execution_t execution = execution_t::remote;
    chip::FabricIndex fabric_index;
    union {
        chip::NodeId node_id;
        chip::GroupId group_id;
    };
};

} // namespace client
} // namespace esp_matter
