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

#include <controller/CommissioningDelegate.h>
#include <lib/core/NodeId.h>
#include <lib/core/Optional.h>
#include <lib/support/Span.h>
#include <transport/raw/PeerAddress.h>

namespace esp_matter {
namespace controller {

namespace pairing {

enum class phase_t : uint8_t {
    discovery,
    pase,
    commissioning,
    cancelling,
};

struct wifi_credentials_t {
    chip::ByteSpan ssid;
    chip::ByteSpan password; /**< Empty for an open network. */
};

struct credentials_t {
    chip::Optional<wifi_credentials_t> wifi;
    chip::Optional<chip::ByteSpan> thread_dataset;
};

/** @brief Discovery/PASE transport; operational connections use IP. */
enum class transport_t : uint8_t {
    automatic,  /**< SDK discovery with on-network fallback. */
    ble,        /**< BLE only; one payload, transport flags ignored. */
    on_network, /**< IP discovery only; transport flags ignored. */
};

enum class method_t : uint8_t { setup_code, address };

struct setup_code_t {
    const char *payload; /**< QR payload text or manual setup code. */
    transport_t transport;
};

struct address_t {
    chip::Transport::PeerAddress peer;
    uint32_t pin;
};

/** @brief Pairing inputs copied by start(); method selects params. */
struct request_t {
    chip::NodeId node_id = chip::kUndefinedNodeId;
    method_t method = method_t::setup_code;
    credentials_t credentials;
    struct parameters_t {
        setup_code_t setup_code = {nullptr, transport_t::automatic};
        address_t address = {};
    } params;

    void set_address(address_t value)
    {
        params.address = value;
        method = method_t::address;
    }
};

struct options_t {
    uint32_t timeout_ms = 300000; /**< Overall deadline; nonzero. */
    bool register_icd = true;
};

struct completion_t {
    chip::NodeId node_id;
    CHIP_ERROR error;
    phase_t phase; /**< Last phase before completion or cancellation. */
    const chip::Controller::CompletionStatus *details; /**< Callback-only; nullable. */
};

class delegate {
public:
    virtual ~delegate() = default;
    /* Callbacks may run inside start() or cancel(). */
    virtual void OnProgress(phase_t phase) {}
    virtual void OnCommissioningStage(chip::Controller::CommissioningStage stage) {}
    virtual void OnDone(const completion_t &result) {}
};

/**
 * @brief Caller-owned commissioning observer and cancellation handle.
 *
 * Methods and active destruction require CHIP stack context or its lock.
 * start(), cancel(), stop(), and destruction are prohibited in pairing callbacks.
 */
class interaction {
public:
    interaction();
    ~interaction();
    interaction(const interaction &) = delete;
    interaction &operator=(const interaction &) = delete;

    /**
     * @brief Start the commissioner's single pairing operation.
     *
     * Copies retained data. Returned errors produce no callbacks; accepted SDK errors use OnDone(), possibly inline.
     * Cancellation cleanup retains the commissioner. Failure statuses use CONNECTION_ABORTED and may supersede PASE errors.
     *
     * @param[in] request Node, setup code or address, and credentials.
     * @param[in] options Overall deadline and ICD registration.
     * @param[in] delegate Borrowed until completion or stop(); nullptr logs errors.
     *
     * @return CHIP_NO_ERROR if accepted; other CHIP_ERROR values from validation or preparation.
     */
    CHIP_ERROR start(const request_t &request, const options_t &options, delegate *delegate);
    /**
     * @brief Cancel pairing without detaching the delegate.
     *
     * OnDone(CHIP_ERROR_CANCELLED) follows SDK cleanup and may run inline. Remote changes are not undone.
     *
     * @return CHIP_NO_ERROR if requested; CHIP_ERROR_INCORRECT_STATE if inactive.
     */
    CHIP_ERROR cancel();
    /**
     * @brief Detach the delegate immediately and cancel unfinished pairing.
     *
     * Cleanup may retain the commissioner. Remote changes are not undone.
     */
    void stop();
    bool active() const;

private:
    class Session;
};

} // namespace pairing

} // namespace controller
} // namespace esp_matter
