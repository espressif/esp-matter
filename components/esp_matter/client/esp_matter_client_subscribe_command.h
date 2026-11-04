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

namespace esp_matter {
namespace client {
namespace subscribe_command {

/** @brief Subscription connection state. */
enum class state_t : uint8_t {
    connecting,
    active,
    backoff,
    terminated,
};

/** @brief Subscription and retry options. */
struct options_t {
    uint16_t min_interval = 0;
    uint16_t max_interval = 600;
    bool fabric_filtered = true;
    bool keep_subscriptions = true;
    bool auto_resubscribe = true;
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

/** @brief Report callbacks; OnStateChanged() is required. Target references are callback-local. */
class delegate {
public:
    virtual ~delegate() = default;
    virtual void OnReportBegin(const target_t &target) {}
    virtual void OnAttribute(const target_t &target, const chip::app::ConcreteDataAttributePath &path, chip::TLV::TLVReader *value,
                             const chip::app::StatusIB &status) {}
    virtual void OnEvent(const target_t &target, const chip::app::EventHeader &header, chip::TLV::TLVReader *value,
                         const chip::app::StatusIB *status) {}
    virtual void OnReportEnd(const target_t &target, CHIP_ERROR err) {}
    virtual void OnStateChanged(const target_t &target, state_t state, CHIP_ERROR error) = 0;
};

/**
 * @brief Caller-owned remote subscription.
 *
 * The delegate is borrowed until stop() or terminal failure. Report readers are valid only during their callback.
 * The active state requires establishment and a successful priming report.
 *
 * Call methods in CHIP context or with the stack lock.
 * Restart only after stop() or termination. The next start() releases terminated state.
 * Do not restart, stop, or destroy this object from its callbacks, including terminated-state notifications.
 * Destruction requires CHIP context/lock unless already stopped.
 */
class subscription {
public:
    subscription();
    ~subscription();

    /**
     * @brief Retry an automatic subscription in backoff immediately.
     *
     * @return CHIP_NO_ERROR if triggered; CHIP_ERROR_INCORRECT_STATE if unavailable, or the retry failure.
     */
    CHIP_ERROR retry_now();

    /**
     * @brief Detach the delegate synchronously and prevent further callbacks.
     *
     * Idempotent. Defer stop() and destruction until delegate callbacks return.
     */
    void stop();

    /**
     * @brief Subscribe to remote attributes or events.
     *
     * Copies paths before return. The initial state callback runs inside start(), after handle initialization;
     * callback-visible state must be ready before this call.
     *
     * @param[in] scoped_node_id Remote peer and fabric.
     * @param[in] request Attribute/event paths and subscription options.
     * @param[in] delegate Borrowed until stop() or terminal failure.
     *
     * @return CHIP_NO_ERROR if accepted; other CHIP_ERROR values from validation or preparation.
     */
    CHIP_ERROR start(const chip::ScopedNodeId &scoped_node_id, const request_t &request, delegate &delegate);

    /** @brief Return the current subscription state. */
    state_t state() const;

    /** @brief Return the peer, or an empty scoped node before start() or after stop(). */
    chip::ScopedNodeId peer() const;

    /** @brief Return the established subscription ID, or NullOptional. */
    chip::Optional<chip::SubscriptionId> id() const;

    subscription(const subscription &) = delete;
    subscription &operator=(const subscription &) = delete;

private:
    class Impl;
    Impl *mImpl = nullptr;
};

} // namespace subscribe_command
} // namespace client
} // namespace esp_matter
