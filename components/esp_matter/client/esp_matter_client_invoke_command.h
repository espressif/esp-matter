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

#include <esp_matter_client_target.h>

#include <app/ConcreteCommandPath.h>
#include <app/MessageDef/StatusIB.h>
#include <app/data-model/EncodableToTLV.h>
#include <lib/core/Optional.h>
#include <lib/core/TLVReader.h>
#include <system/SystemClock.h>

namespace esp_matter {
namespace client {
namespace invoke_command {

/** @brief Invoke options. */
struct options_t {
    chip::Optional<uint16_t> timed_timeout_ms = chip::NullOptional;
    chip::Optional<chip::System::Clock::Timeout> response_timeout = chip::NullOptional;
};

/** @brief Command path, fields, and options consumed by start(). */
struct request_t {
    chip::EndpointId endpoint_id = chip::kInvalidEndpointId;
    chip::ClusterId cluster_id = chip::kInvalidClusterId;
    chip::CommandId command_id = chip::kInvalidCommandId;
    const chip::app::DataModel::EncodableToTLV *fields = nullptr;
    options_t options;
};

/** @brief Optional response and completion callbacks. Target references are callback-local. */
class delegate {
public:
    virtual ~delegate() = default;
    virtual void OnResponse(const target_t &target, const chip::app::ConcreteCommandPath &path, const chip::app::StatusIB &status,
                            chip::TLV::TLVReader *response) {}
    virtual void OnDone(const target_t &target, CHIP_ERROR err) {}
};

/**
 * @brief Caller-owned unicast or group Invoke.
 *
 * Accepted requests finish with one OnDone(), unless stopped.
 * Returned errors produce no callbacks. Accepted failures are asynchronous.
 * Callbacks occur after submission returns and end at OnDone()/stop(). Response readers are valid only during their callback.
 *
 * Call methods in CHIP context or with the stack lock.
 * Starting another request releases completed state.
 * OnDone() may stop, restart, or destroy this object. Response callbacks may not.
 * Destruction requires CHIP context/lock unless already stopped.
 */
class interaction {
public:
    interaction() = default;
    ~interaction();
    interaction(const interaction &) = delete;
    interaction &operator=(const interaction &) = delete;

    /**
     * @brief Invoke a command on a remote node.
     *
     * Consumes fields before return; nullptr fields encode an empty structure.
     *
     * @param[in] scoped_node_id Remote node and fabric.
     * @param[in] request Command path, fields, and options.
     * @param[in] delegate Borrowed until completion or stop(); nullptr logs failures and discards responses.
     *
     * @return CHIP_NO_ERROR if accepted; other CHIP_ERROR values from validation or preparation.
     */
    CHIP_ERROR start(const chip::ScopedNodeId &scoped_node_id, const request_t &request, delegate *delegate);

    /**
     * @brief Send a groupcast.
     *
     * No OnResponse() callbacks. OnDone(CHIP_NO_ERROR) confirms dispatch, not member acknowledgment.
     * A synchronous failure cancels queued completion.
     *
     * @param[in] group_id Destination group.
     * @param[in] fabric_index Fabric containing the group.
     * @param[in] request Command fields; timed and response-timeout options are unsupported.
     * @param[in] delegate Borrowed until completion or stop(); nullptr logs failures.
     *
     * @return CHIP_NO_ERROR if accepted; other CHIP_ERROR values from validation, encoding, or dispatch.
     */
    CHIP_ERROR start(chip::GroupId group_id, chip::FabricIndex fabric_index,
                     const request_t &request, delegate *delegate);

    /**
     * @brief Return whether completion is pending.
     */
    bool active() const;

    /**
     * @brief Detach the delegate synchronously and prevent further callbacks.
     *
     * Idempotent. Dispatched commands are not undone.
     */
    void stop();

private:
    class Impl;
    Impl *mImpl = nullptr;
};

} // namespace invoke_command
} // namespace client
} // namespace esp_matter
