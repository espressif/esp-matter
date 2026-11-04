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

#include <app/ConcreteAttributePath.h>
#include <app/MessageDef/StatusIB.h>
#include <app/data-model/EncodableToTLV.h>
#include <lib/core/Optional.h>
#include <lib/support/Span.h>

namespace esp_matter {
namespace client {
namespace write_command {

/** @brief Write options. */
struct options_t {
    chip::Optional<uint16_t> timed_timeout_ms = chip::NullOptional;
};

/**
 * @brief Attribute path and value consumed by start().
 *
 * The encoder emits the complete attribute value with the supplied tag.
 */
struct attribute_write_t {
    chip::app::ConcreteDataAttributePath path = {
        chip::kInvalidEndpointId, chip::kInvalidClusterId, chip::kInvalidAttributeId
    };
    const chip::app::DataModel::EncodableToTLV *value = nullptr;
};

/** @brief Attribute values and Write options consumed by start(). */
struct request_t {
    chip::Span<const attribute_write_t> attributes;
    options_t options;
};

/** @brief Optional path-status and completion callbacks. */
class delegate {
public:
    virtual ~delegate() = default;
    virtual void OnResponse(const target_t &target, const chip::app::ConcreteDataAttributePath &path, const chip::app::StatusIB &status) {}
    virtual void OnDone(const target_t &target, CHIP_ERROR err) {}
};

/**
 * @brief Caller-owned, non-atomic attribute Write.
 *
 * Each unicast logical path receives OnResponse(). Path failures do not roll back successes.
 * Accepted requests finish with one OnDone(), unless stopped. It reports protocol/transport completion, not path success.
 * Returned errors produce no callbacks. Accepted node CASE and dispatch failures are asynchronous.
 * Callbacks occur after submission returns and end at OnDone()/stop().
 *
 * Call start()/stop() in CHIP context or with the stack lock.
 * Starting another request releases completed state.
 * OnDone() may stop, restart, or destroy this object. Other callbacks may not.
 * Destruction requires CHIP context/lock unless already stopped.
 */
class interaction {
public:
    interaction() = default;
    ~interaction();
    interaction(const interaction &) = delete;
    interaction &operator=(const interaction &) = delete;

    /**
     * @brief Write one or more complete attribute values.
     *
     * Copies paths and consumes encoders before return. List-item operations are unsupported; complete lists use ReplaceAll.
     * Non-timed lists support Matter chunking; timed requests requiring chunking fail synchronously.
     *
     * @param[in] scoped_node_id Remote node and fabric.
     * @param[in] request Attribute paths, value encoders, and options.
     * @param[in] delegate Borrowed until completion or stop(); nullptr logs failures and discards successful responses.
     *
     * @return CHIP_NO_ERROR if accepted; other CHIP_ERROR values from validation or preparation.
     */
    CHIP_ERROR start(const chip::ScopedNodeId &scoped_node_id, const request_t &request, delegate *delegate);

    /**
     * @brief Dispatch a group Write without member acknowledgments.
     *
     * Copies values before return; path endpoints are omitted. Timed writes, data versions, and chunking are unsupported.
     * OnResponse() is not called. OnDone(CHIP_NO_ERROR) reports dispatch, not member success, after start() returns.
     *
     * @param[in] group_id Destination group.
     * @param[in] fabric_index Fabric containing the group.
     * @param[in] request Attribute paths, value encoders, and options.
     * @param[in] delegate Borrowed until completion or stop(); nullptr logs failures.
     *
     * @return CHIP_NO_ERROR if dispatched; other CHIP_ERROR values from validation, encoding, or transport.
     */
    CHIP_ERROR start(chip::GroupId group_id, chip::FabricIndex fabric_index,
                     const request_t &request, delegate *delegate);

    /**
     * @brief Detach the delegate synchronously and release protocol state.
     *
     * Idempotent. Dispatched writes are not undone; no callbacks occur after return.
     */
    void stop();

    /** @brief Whether this interaction is awaiting completion. */
    bool active() const;

private:
    class Impl;
    Impl *mImpl = nullptr;
};

} // namespace write_command
} // namespace client
} // namespace esp_matter
