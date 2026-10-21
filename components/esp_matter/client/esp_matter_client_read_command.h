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

#include <app/AttributePathParams.h>
#include <app/EventHeader.h>
#include <app/EventPathParams.h>
#include <app/MessageDef/StatusIB.h>
#include <lib/core/Optional.h>
#include <lib/core/TLVReader.h>
#include <lib/support/Span.h>
#include <system/SystemClock.h>

#include <cstddef>

namespace esp_matter {
namespace client {
namespace read_command {

/** @brief Read options. */
struct options_t {
    bool fabric_filtered = true;
    chip::System::Clock::Timeout response_timeout = chip::System::Clock::kZero;
    chip::Optional<chip::EventNumber> min_event_number = chip::NullOptional;
};

using attribute_paths_t = chip::Span<const chip::app::AttributePathParams>;
using event_paths_t = chip::Span<const chip::app::EventPathParams>;

/**
 * @brief Borrowed attribute/event paths, copied by start().
 *
 * Invalid IDs or SetWildcard*() select wildcards. Wildcard event clusters require wildcard event IDs.
 * Reports contain concrete expanded paths.
 */
struct request_t {
    explicit request_t(attribute_paths_t paths, options_t request_options = {})
        : attribute_paths(paths), options(request_options) {}
    explicit request_t(event_paths_t paths, options_t request_options = {})
        : event_paths(paths), options(request_options) {}
    request_t(attribute_paths_t attributes, event_paths_t events, options_t request_options = {})
        : attribute_paths(attributes), event_paths(events), options(request_options) {}

    attribute_paths_t attribute_paths;
    event_paths_t event_paths;
    options_t options;
};

/** @brief Read report callbacks; OnDone() is required. */
class delegate {
public:
    virtual ~delegate() = default;
    virtual void OnReportBegin(const target_t &target) {}
    virtual void OnAttribute(const target_t &target, const chip::app::ConcreteDataAttributePath &path, chip::TLV::TLVReader *value,
                             const chip::app::StatusIB &status) {}
    virtual void OnEvent(const target_t &target, const chip::app::EventHeader &header, chip::TLV::TLVReader *value,
                         const chip::app::StatusIB *status) {}
    virtual void OnReportEnd(const target_t &target, CHIP_ERROR err) {}
    virtual void OnDone(const target_t &target, CHIP_ERROR err) = 0;
};

/**
 * @brief Caller-owned remote Read.
 *
 * Accepted requests finish with one OnDone(), unless stopped.
 * Returned errors produce no callbacks. Accepted CASE and dispatch failures are asynchronous.
 * Callbacks occur after start() returns. Value readers are valid only during their callback.
 * Report begin/end callbacks are paired, including errors, unless stopped.
 *
 * Call start()/stop() in CHIP context or with the stack lock.
 * The next start() releases completed state.
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
     * @brief Read attributes or events from a node.
     *
     * @param[in] scoped_node_id Remote node and fabric.
     * @param[in] request Paths copied before return, and request options.
     * @param[in] delegate Borrowed until completion or stop().
     *
     * @return CHIP_NO_ERROR if accepted; other CHIP_ERROR values from validation or preparation.
     */
    CHIP_ERROR start(const chip::ScopedNodeId &scoped_node_id, const request_t &request, delegate &delegate);

    /**
     * @brief Detach the delegate synchronously and release protocol state.
     *
     * Idempotent. No callbacks occur after return.
     */
    void stop();

    /** @brief Whether this interaction is awaiting completion. */
    bool active() const;

private:
    class Impl;
    Impl *mImpl = nullptr;
};

} // namespace read_command
} // namespace client
} // namespace esp_matter
