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

#include <esp_matter_client_binding.h>
#include <app/clusters/bindings/binding-table.h>

#include "esp_matter_client_operation.h"

namespace esp_matter {
namespace client {
namespace binding {

class DispatchRequest {
public:
    enum class Type : uint8_t { invoke, read, write, subscribe };
    explicit DispatchRequest(Type type) : mType(type) {}
    ~DispatchRequest();
    DispatchRequest(const DispatchRequest &) = delete;
    DispatchRequest &operator=(const DispatchRequest &) = delete;

    static CHIP_ERROR Start(DispatchRequest * &requests, chip::EndpointId local_endpoint,
                            const invoke_command::request_t &request, invoke_command::delegate *delegate);
    static CHIP_ERROR Start(DispatchRequest * &requests, chip::EndpointId local_endpoint,
                            const read_command::request_t &request, read_command::delegate &delegate);
    static CHIP_ERROR Start(DispatchRequest * &requests, chip::EndpointId local_endpoint,
                            const write_command::request_t &request, write_command::delegate *delegate);
    static CHIP_ERROR Start(DispatchRequest * &requests, chip::EndpointId local_endpoint,
                            const subscribe_command::request_t &request, subscribe_command::delegate &delegate);
    DispatchRequest *next_request = nullptr;
    bool Active() const;

private:
    struct Invoke;
    struct Read;
    struct Write;
    struct Subscribe;
    struct Operation;
    static CHIP_ERROR RegisterHandlers();
    CHIP_ERROR Submit(DispatchRequest * &requests, chip::EndpointId local_endpoint);
    void ReleaseRequest();
    void DestroyOperation(Operation *operation);
    static DispatchRequest *Find(uintptr_t id);
    static void OnBoundDeviceContextRelease(void *context);
    static void OnBoundDeviceChanged(const chip::app::Clusters::Binding::TableEntry &binding,
                                     chip::OperationalDeviceProxy *, void *context);
    static void OnBindingEvent(const chip::DeviceLayer::ChipDeviceEvent *event, intptr_t);

    // mType selects both union members. Request storage is read only while pending.
    Type mType;
    bool mPending = false;
    union {
        Invoke *invoke;
        Read *read;
        Write *write;
        Subscribe *subscribe;
    } mRequest;
    chip::ClusterId mCluster = chip::kInvalidClusterId;
    uintptr_t mId = 0;
    Operation *mOperations = nullptr;
    DispatchRequest *mNextRegisteredRequest = nullptr;
    static DispatchRequest *sRequests;
    static uintptr_t sNextId;
};

struct DispatchRequest::Invoke {
    invoke_command::request_t request;
    operation::EncodedElement fields;
    invoke_command::delegate *delegate = nullptr;
};

struct DispatchRequest::Read {
    operation::OwnedReadPaths paths;
    read_command::options_t options;
    read_command::delegate *delegate = nullptr;
};

struct DispatchRequest::Write {
    ~Write();

    chip::Platform::ScopedMemoryBufferWithSize<write_command::attribute_write_t> attributes;
    chip::Platform::ScopedMemoryBufferWithSize<operation::EncodedElement *> values;
    write_command::options_t options;
    write_command::delegate *delegate = nullptr;
};

struct DispatchRequest::Subscribe {
    operation::OwnedReadPaths paths;
    subscribe_command::options_t options;
    subscribe_command::delegate *delegate = nullptr;
};

struct DispatchRequest::Operation {
    chip::app::Clusters::Binding::TableEntry binding;
    // The owning request's mType selects this operation.
    union {
        invoke_command::interaction *invoke;
        read_command::interaction *read;
        write_command::interaction *write;
        subscribe_command::subscription *subscribe;
    };
    Operation *next_operation = nullptr;
};

} // namespace binding
} // namespace client
} // namespace esp_matter
