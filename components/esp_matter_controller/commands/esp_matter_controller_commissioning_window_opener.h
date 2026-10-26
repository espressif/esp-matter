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

#include <app/MessageDef/StatusIB.h>
#include <lib/core/Optional.h>
#include <lib/core/ScopedNodeId.h>
#include <lib/support/Span.h>
#include <setup_payload/SetupPayload.h>

namespace esp_matter {
namespace controller {

namespace commissioning_window {

enum class method_t : uint8_t {
    basic,
    passcode,
    verifier,
};

struct passcode_t {
    uint16_t discriminator = 0;
    uint32_t iterations = 0;
    chip::Optional<uint32_t> setup_pin; /**< Absent: generate; zero is invalid. */
    chip::Optional<chip::ByteSpan> salt; /**< Absent: generate; empty is invalid. */
};

struct verifier_t {
    uint16_t discriminator = 0;
    uint32_t iterations = 0;
    chip::ByteSpan verifier;
    chip::ByteSpan salt;
};

/** @brief Explicit remote node/fabric and window parameters; method selects params. */
struct request_t {
    explicit request_t(method_t type = method_t::basic) : method(type) {}

    chip::ScopedNodeId node;
    uint16_t window_timeout_seconds = 0;
    uint16_t timed_invoke_timeout_ms = 10000;
    method_t method;
    struct params_t {
        passcode_t passcode;
        verifier_t verifier;
    } params;
};

struct response_t {
    chip::app::StatusIB status;
    const chip::SetupPayload *setup_payload; /**< Callback-only; successful passcode requests only. */
};

class delegate {
public:
    virtual ~delegate() = default;
    virtual void OnResponse(const response_t &response) {}
    virtual void OnDone(CHIP_ERROR error) {}
};

/**
 * @brief Caller-owned commissioning-window request.
 *
 * Methods and active destruction require CHIP stack context or its lock.
 * OnDone() may start, stop, or destroy this object. OnResponse() may not.
 */
class interaction {
public:
    interaction() = default;
    ~interaction();
    interaction(const interaction &) = delete;
    interaction &operator=(const interaction &) = delete;

    /**
     * @brief Request a remote commissioning window.
     *
     * One active request per object; separate objects may overlap. Returned errors produce no callbacks.
     *
     * @param[in] request Target and window parameters; borrowed until return.
     * @param[in] delegate Borrowed until completion or stop(); nullptr logs errors.
     *
     * @return CHIP_NO_ERROR if accepted; other CHIP_ERROR values from validation or preparation.
     */
    CHIP_ERROR start(const request_t &request, delegate *delegate);
    /**
     * @brief Stop local work and detach callbacks.
     *
     * Does not close an already-open remote window.
     */
    void stop();
    /**
     * @brief Return whether request completion is pending, not the remote window state.
     */
    bool active() const;

private:
    class Impl;
    Impl *mImpl = nullptr;
};

} // namespace commissioning_window

} // namespace controller
} // namespace esp_matter
