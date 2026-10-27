// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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

#include <stddef.h>
#include <stdint.h>

#include <app-common/zap-generated/cluster-enums.h>
#include <app/MessageDef/StatusIB.h>
#include <lib/core/CHIPError.h>
#include <lib/core/NodeId.h>
#include <lib/core/Optional.h>
#include <lib/core/ScopedNodeId.h>

namespace esp_matter::controller::icd_client {

enum method_t : uint8_t {
    METHOD_REGISTER,
    METHOD_UNREGISTER,
};

struct registration_t {
    // Check-in recipient on the controller's current fabric.
    chip::NodeId check_in_node_id;
    uint64_t monitored_subject;

    // Required: 16-byte symmetric key.
    const uint8_t *key;
    size_t key_size;

    chip::app::Clusters::IcdManagement::ClientTypeEnum client_type;

    // Optional: nullptr and size 0, otherwise 16 bytes.
    const uint8_t *verification_key;
    size_t verification_key_size;
};

struct unregistration_t {
    // Check-in node of the registration to remove.
    chip::NodeId check_in_node_id;

    // Optional: nullptr and size 0, otherwise 16 bytes.
    const uint8_t *verification_key;
    size_t verification_key_size;
};

struct request_t {
    // ICD server on the controller's current fabric.
    chip::NodeId node_id;
    method_t method;

    union {
        registration_t registration;
        unregistration_t unregistration;
    } params;
};

struct response_t {
    // Remote command status, including rejection.
    chip::app::StatusIB status;

    // Present only for a successful RegisterClient response.
    chip::Optional<uint32_t> icd_counter;
};

// Schedule interaction restart or destruction after callbacks return.
class delegate {
public:
    virtual ~delegate() = default;

    // Called when a valid remote response is available.
    // The reference is valid only during this callback.
    virtual void OnResponse(const response_t &response) {}

    // Includes connection, command, and local storage errors.
    // Success means the remote command and local storage update succeeded.
    virtual void OnDone(CHIP_ERROR error) {}
};

// Application-owned. Requires CHIP stack context for all operations,
// including destruction of an active interaction.
// OnDone() may restart, stop, or destroy the interaction; OnResponse() may not.
class interaction {
public:
    interaction();
    ~interaction();

    interaction(const interaction &) = delete;
    interaction &operator=(const interaction &) = delete;

    // Validates and copies borrowed request data before returning.
    // delegate must be non-null and outlive observation of the operation.
    //
    // Synchronous failure: returns an error; no callbacks.
    // Accepted submission: returns CHIP_NO_ERROR; callbacks are deferred.
    // Exactly one OnDone(), unless stop() detaches the delegate.
    //
    // Returns CHIP_ERROR_BUSY if this object is active or another explicit
    // ICD operation for the same scoped peer still has pending work.
    // Unregistration requires a matching local registration.
    CHIP_ERROR start(const request_t &request, delegate *delegate);

    // Detaches the delegate immediately; no subsequent delegate callbacks.
    // Stops work that has not yet sent a command.
    //
    // An already-sent command may complete remotely. Its internal context
    // survives to finish response handling and local storage bookkeeping.
    // Does not undo remote registration changes.
    // Safe when inactive. The destructor calls stop().
    void stop();

    // Tracks observation of the accepted operation.
    // False after completion or stop(); internal cleanup may remain pending.
    bool active() const;

private:
    struct context;
    context *m_context = nullptr;
};

// Stored registration metadata, copied from local ICD client storage.
struct entry_t {
    chip::ScopedNodeId peer_node;
    chip::ScopedNodeId check_in_node;
    uint64_t monitored_subject;
    uint32_t start_icd_counter;
    uint32_t offset;
    chip::app::Clusters::IcdManagement::ClientTypeEnum client_type;
};

// Return CHIP_NO_ERROR to continue; any other error stops iteration
// and is returned by for_each_registered_icd().
using entry_callback = CHIP_ERROR(*)(const entry_t &entry, void *context);

// Synchronously visits entries across all stored fabrics.
// Requires CHIP stack context and a non-null callback.
// Empty storage returns CHIP_NO_ERROR.
// Entry references are valid only during their callback.
// The callback must not modify ICD client storage.
CHIP_ERROR for_each_registered_icd(entry_callback callback, void *context = nullptr);

} // namespace esp_matter::controller::icd_client
