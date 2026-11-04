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

#include <esp_matter_client_binding.h>

#include "esp_matter_client_binding_dispatch.h"

#include <app/clusters/bindings/BindingManager.h>
#include <app/server/Server.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <platform/CHIPDeviceLayer.h>

namespace esp_matter {
namespace client {

namespace binding {

DispatchRequest *DispatchRequest::sRequests = nullptr;
uintptr_t DispatchRequest::sNextId = 1;

namespace {

const char *TAG = "client_binding";

bool initialized = false;
using chip::app::Clusters::Binding::TableEntry;

bool IsBound(const TableEntry &binding)
{
    for (const auto &entry : chip::app::Clusters::Binding::Table::GetInstance()) {
        if (entry == binding) {
            return true;
        }
    }
    return false;
}

CHIP_ERROR ValidateRequestCluster(read_command::attribute_paths_t attributes, read_command::event_paths_t events,
                                  chip::ClusterId &cluster)
{
    ReturnErrorOnFailure(operation::ValidateReadPaths(attributes, events));
    cluster = attributes.empty() ? events[0].mClusterId : attributes[0].mClusterId;
    VerifyOrReturnError(chip::IsValidClusterId(cluster), CHIP_ERROR_INVALID_ARGUMENT);
    for (const auto &path : attributes) {
        VerifyOrReturnError(path.mClusterId == cluster, CHIP_ERROR_INVALID_ARGUMENT);
    }
    for (const auto &path : events) {
        VerifyOrReturnError(path.mClusterId == cluster, CHIP_ERROR_INVALID_ARGUMENT);
    }
    return CHIP_NO_ERROR;
}

#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
void InitializeManagerOnChipTask(intptr_t)
{
    auto &server = chip::Server::GetInstance();
    chip::app::Clusters::Binding::ManagerInitParams params = {
        .mFabricTable = &server.GetFabricTable(),
                               .mCASESessionManager = server.GetCASESessionManager(),
                               .mStorage = &server.GetPersistentStorage(),
    };
    CHIP_ERROR err = chip::app::Clusters::Binding::Manager::GetInstance().Init(params);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to initialize Binding Manager: %s", chip::ErrorStr(err));
    }
}
#endif // CONFIG_ESP_MATTER_ENABLE_DATA_MODEL

} // namespace

DispatchRequest::Write::~Write()
{
    for (auto *value : values.Span()) {
        chip::Platform::Delete(value);
    }
}

void DispatchRequest::ReleaseRequest()
{
    if (!mPending) {
        return;
    }
    switch (mType) {
    case Type::invoke: chip::Platform::Delete(mRequest.invoke); break;
    case Type::read: chip::Platform::Delete(mRequest.read); break;
    case Type::write: chip::Platform::Delete(mRequest.write); break;
    case Type::subscribe: chip::Platform::Delete(mRequest.subscribe); break;
    }
    mPending = false;
}

void DispatchRequest::DestroyOperation(Operation *operation)
{
    switch (mType) {
    case Type::invoke: chip::Platform::Delete(operation->invoke); break;
    case Type::read: chip::Platform::Delete(operation->read); break;
    case Type::write: chip::Platform::Delete(operation->write); break;
    case Type::subscribe: chip::Platform::Delete(operation->subscribe); break;
    }
    chip::Platform::Delete(operation);
}

DispatchRequest::~DispatchRequest()
{
    VerifyOrDie(operation::callback_depth == 0);
    if (mId != 0) {
        assertChipStackLockedByCurrentThread();
        auto **link = &sRequests;
        while (*link != this) {
            link = &(*link)->mNextRegisteredRequest;
        }
        *link = mNextRegisteredRequest;
    }
    ReleaseRequest();
    while (mOperations != nullptr) {
        auto *operation = mOperations;
        mOperations = operation->next_operation;
        DestroyOperation(operation);
    }
}

DispatchRequest *DispatchRequest::Find(uintptr_t id)
{
    for (auto *dispatch_request = sRequests; dispatch_request != nullptr; dispatch_request = dispatch_request->mNextRegisteredRequest) {
        if (dispatch_request->mId == id) {
            return dispatch_request;
        }
    }
    return nullptr;
}

void DispatchRequest::OnBoundDeviceContextRelease(void *context)
{
    if (auto *dispatch_request = Find(reinterpret_cast<uintptr_t>(context))) {
        dispatch_request->ReleaseRequest();
    }
}

void DispatchRequest::OnBoundDeviceChanged(const TableEntry &binding, chip::OperationalDeviceProxy *, void *context)
{
    auto *dispatch_request = Find(reinterpret_cast<uintptr_t>(context));
    if (dispatch_request == nullptr || !dispatch_request->mPending || !IsBound(binding) ||
            operation::ValidateFabricIndex(binding.fabricIndex) != CHIP_NO_ERROR) {
        return;
    }
    bool group = binding.type == chip::app::Clusters::Binding::MATTER_MULTICAST_BINDING;
    if (group && (dispatch_request->mType == Type::read || dispatch_request->mType == Type::subscribe)) {
        return;
    }
    auto *operation = chip::Platform::New<Operation>();
    if (operation == nullptr) {
        ESP_LOGE(TAG, "No memory for bound operation");
        return;
    }
    operation->binding = binding;
    chip::ScopedNodeId peer(group ? chip::kUndefinedNodeId : binding.nodeId, binding.fabricIndex);
    CHIP_ERROR err = CHIP_ERROR_NO_MEMORY;
    switch (dispatch_request->mType) {
    case Type::invoke: {
        auto &data = *dispatch_request->mRequest.invoke;
        auto request = data.request;
        request.endpoint_id = binding.remote;
        auto *interaction = chip::Platform::New<invoke_command::interaction>();
        operation->invoke = interaction;
        if (interaction != nullptr) {
            err = group ? interaction->start(binding.groupId, binding.fabricIndex, request, data.delegate) : interaction->start(peer, request, data.delegate);
        }
        break;
    }
    case Type::read: {
        auto &data = *dispatch_request->mRequest.read;
        data.paths.SetEndpoint(binding.remote);
        read_command::request_t request(data.paths.AttributePaths(), data.paths.EventPaths(), data.options);
        auto *interaction = chip::Platform::New<read_command::interaction>();
        operation->read = interaction;
        if (interaction != nullptr) {
            err = interaction->start(peer, request, *data.delegate);
        }
        break;
    }
    case Type::write: {
        auto &data = *dispatch_request->mRequest.write;
        for (auto &attribute : data.attributes.Span()) {
            attribute.path.mEndpointId = binding.remote;
        }
        write_command::request_t request = {data.attributes.Span(), data.options};
        auto *interaction = chip::Platform::New<write_command::interaction>();
        operation->write = interaction;
        if (interaction != nullptr) {
            err = group ? interaction->start(binding.groupId, binding.fabricIndex, request, data.delegate) : interaction->start(peer, request, data.delegate);
        }
        break;
    }
    case Type::subscribe: {
        auto &data = *dispatch_request->mRequest.subscribe;
        data.paths.SetEndpoint(binding.remote);
        subscribe_command::request_t request(data.paths.AttributePaths(), data.paths.EventPaths(), data.options);
        auto *subscription = chip::Platform::New<subscribe_command::subscription>();
        operation->subscribe = subscription;
        if (subscription != nullptr) {
            err = subscription->start(peer, request, *data.delegate);
        }
        break;
    }
    }
    if (err != CHIP_NO_ERROR) {
        dispatch_request->DestroyOperation(operation);
        ESP_LOGE(TAG, "Bound dispatch failed: %s", chip::ErrorStr(err));
        return;
    }
    operation->next_operation = dispatch_request->mOperations;
    dispatch_request->mOperations = operation;
}

CHIP_ERROR DispatchRequest::Submit(DispatchRequest * &requests, chip::EndpointId local_endpoint)
{
    assertChipStackLockedByCurrentThread();
    VerifyOrReturnError(operation::callback_depth == 0, CHIP_ERROR_INCORRECT_STATE);
    VerifyOrReturnError(initialized, CHIP_ERROR_INCORRECT_STATE);
    VerifyOrReturnError(local_endpoint != chip::kInvalidEndpointId && chip::IsValidClusterId(mCluster),
                        CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(sNextId != 0, CHIP_ERROR_NO_MEMORY);
    ReturnErrorOnFailure(RegisterHandlers());

    // Never reuse a token the SDK might retain after stop() or a returned error.
    mId = sNextId++;
    mNextRegisteredRequest = sRequests;
    sRequests = this;
    next_request = requests;
    requests = this;
    auto *context = reinterpret_cast<void *>(mId);
    CHIP_ERROR err = chip::app::Clusters::Binding::Manager::GetInstance().NotifyBoundClusterChanged(
                         local_endpoint, mCluster, context);
    if (err != CHIP_NO_ERROR) {
        ReleaseRequest();
    }
    return err;
}

CHIP_ERROR DispatchRequest::Start(DispatchRequest * &requests, chip::EndpointId local_endpoint,
                                  const invoke_command::request_t &request, invoke_command::delegate *delegate)
{
    VerifyOrReturnError(request.command_id != chip::kInvalidCommandId, CHIP_ERROR_INVALID_ARGUMENT);
    auto dispatch_request = chip::Platform::MakeUnique<DispatchRequest>(Type::invoke);
    auto data = chip::Platform::MakeUnique<Invoke>();
    VerifyOrReturnError(dispatch_request != nullptr && data != nullptr, CHIP_ERROR_NO_MEMORY);
    ReturnErrorOnFailure(data->fields.Encode(request.fields));
    data->request = request;
    data->request.fields = &data->fields;
    data->delegate = delegate;
    dispatch_request->mCluster = request.cluster_id;
    dispatch_request->mRequest.invoke = data.release();
    dispatch_request->mPending = true;
    CHIP_ERROR err = dispatch_request->Submit(requests, local_endpoint);
    if (requests == dispatch_request.get()) {
        dispatch_request.release();
    }
    return err;
}

CHIP_ERROR DispatchRequest::Start(DispatchRequest * &requests, chip::EndpointId local_endpoint,
                                  const read_command::request_t &request, read_command::delegate &delegate)
{
    auto dispatch_request = chip::Platform::MakeUnique<DispatchRequest>(Type::read);
    auto data = chip::Platform::MakeUnique<Read>();
    VerifyOrReturnError(dispatch_request != nullptr && data != nullptr, CHIP_ERROR_NO_MEMORY);
    ReturnErrorOnFailure(ValidateRequestCluster(request.attribute_paths, request.event_paths, dispatch_request->mCluster));
    ReturnErrorOnFailure(data->paths.Copy(request.attribute_paths, request.event_paths));
    data->options = request.options;
    data->delegate = &delegate;
    dispatch_request->mRequest.read = data.release();
    dispatch_request->mPending = true;
    CHIP_ERROR err = dispatch_request->Submit(requests, local_endpoint);
    if (requests == dispatch_request.get()) {
        dispatch_request.release();
    }
    return err;
}

CHIP_ERROR DispatchRequest::Start(DispatchRequest * &requests, chip::EndpointId local_endpoint,
                                  const write_command::request_t &request, write_command::delegate *delegate)
{
    VerifyOrReturnError(!request.attributes.empty() && request.attributes.data() != nullptr &&
                        request.attributes.size() <= SIZE_MAX / sizeof(write_command::attribute_write_t),
                        CHIP_ERROR_INVALID_ARGUMENT);
    auto dispatch_request = chip::Platform::MakeUnique<DispatchRequest>(Type::write);
    auto data = chip::Platform::MakeUnique<Write>();
    VerifyOrReturnError(dispatch_request != nullptr && data != nullptr, CHIP_ERROR_NO_MEMORY);
    VerifyOrReturnError(data->attributes.Alloc(request.attributes.size()) && data->values.Calloc(request.attributes.size()),
                        CHIP_ERROR_NO_MEMORY);
    dispatch_request->mCluster = request.attributes[0].path.mClusterId;
    for (size_t index = 0; index < request.attributes.size(); ++index) {
        const auto &attribute = request.attributes[index];
        VerifyOrReturnError(attribute.path.mClusterId == dispatch_request->mCluster && attribute.value != nullptr,
                            CHIP_ERROR_INVALID_ARGUMENT);
        data->values[index] = chip::Platform::New<operation::EncodedElement>();
        VerifyOrReturnError(data->values[index] != nullptr, CHIP_ERROR_NO_MEMORY);
        ReturnErrorOnFailure(data->values[index]->Encode(attribute.value));
        data->attributes[index] = attribute;
        data->attributes[index].value = data->values[index];
    }
    data->options = request.options;
    data->delegate = delegate;
    dispatch_request->mRequest.write = data.release();
    dispatch_request->mPending = true;
    CHIP_ERROR err = dispatch_request->Submit(requests, local_endpoint);
    if (requests == dispatch_request.get()) {
        dispatch_request.release();
    }
    return err;
}

CHIP_ERROR DispatchRequest::Start(DispatchRequest * &requests, chip::EndpointId local_endpoint,
                                  const subscribe_command::request_t &request, subscribe_command::delegate &delegate)
{
    VerifyOrReturnError(request.options.min_interval <= request.options.max_interval, CHIP_ERROR_INVALID_ARGUMENT);
    auto dispatch_request = chip::Platform::MakeUnique<DispatchRequest>(Type::subscribe);
    auto data = chip::Platform::MakeUnique<Subscribe>();
    VerifyOrReturnError(dispatch_request != nullptr && data != nullptr, CHIP_ERROR_NO_MEMORY);
    ReturnErrorOnFailure(ValidateRequestCluster(request.attribute_paths, request.event_paths, dispatch_request->mCluster));
    ReturnErrorOnFailure(data->paths.Copy(request.attribute_paths, request.event_paths));
    data->options = request.options;
    data->delegate = &delegate;
    dispatch_request->mRequest.subscribe = data.release();
    dispatch_request->mPending = true;
    CHIP_ERROR err = dispatch_request->Submit(requests, local_endpoint);
    if (requests == dispatch_request.get()) {
        dispatch_request.release();
    }
    return err;
}

bool DispatchRequest::Active() const
{
    if (mPending) {
        return true;
    }
    for (auto *operation = mOperations; operation != nullptr; operation = operation->next_operation) {
        bool active = false;
        switch (mType) {
        case Type::invoke: active = operation->invoke->active(); break;
        case Type::read: active = operation->read->active(); break;
        case Type::write: active = operation->write->active(); break;
        case Type::subscribe:
            active = operation->subscribe->state() != subscribe_command::state_t::terminated;
            break;
        }
        if (active) {
            return true;
        }
    }
    return false;
}

void DispatchRequest::OnBindingEvent(const chip::DeviceLayer::ChipDeviceEvent *event, intptr_t)
{
    if (event->Type != chip::DeviceLayer::DeviceEventType::kBindingsChangedViaCluster &&
            event->Type != chip::DeviceLayer::DeviceEventType::kFabricRemoved) {
        return;
    }
    for (auto *dispatch_request = sRequests; dispatch_request != nullptr; dispatch_request = dispatch_request->mNextRegisteredRequest) {
        auto **link = &dispatch_request->mOperations;
        while (*link != nullptr) {
            auto *operation = *link;
            if (operation::ValidateFabricIndex(operation->binding.fabricIndex) != CHIP_NO_ERROR ||
                    (dispatch_request->mType == Type::subscribe && !IsBound(operation->binding))) {
                *link = operation->next_operation;
                dispatch_request->DestroyOperation(operation);
            } else {
                link = &operation->next_operation;
            }
        }
    }
}

CHIP_ERROR DispatchRequest::RegisterHandlers()
{
    static bool registered = false;
    if (registered) {
        return CHIP_NO_ERROR;
    }
    ReturnErrorOnFailure(chip::DeviceLayer::PlatformMgr().AddEventHandler(OnBindingEvent, 0));
    auto &manager = chip::app::Clusters::Binding::Manager::GetInstance();
    manager.RegisterBoundDeviceChangedHandler(OnBoundDeviceChanged);
    manager.RegisterBoundDeviceContextReleaseHandler(OnBoundDeviceContextRelease);
    registered = true;
    return CHIP_NO_ERROR;
}

CHIP_ERROR dispatcher::Prune()
{
    assertChipStackLockedByCurrentThread();
    VerifyOrReturnError(operation::callback_depth == 0, CHIP_ERROR_INCORRECT_STATE);
    auto **link = &mRequests;
    while (*link != nullptr) {
        auto *request = *link;
        if (request->Active()) {
            link = &request->next_request;
        } else {
            *link = request->next_request;
            chip::Platform::Delete(request);
        }
    }
    return CHIP_NO_ERROR;
}

void dispatcher::stop()
{
    if (mRequests == nullptr) {
        return;
    }
    assertChipStackLockedByCurrentThread();
    VerifyOrDie(operation::callback_depth == 0);
    while (mRequests != nullptr) {
        auto *request = mRequests;
        mRequests = request->next_request;
        chip::Platform::Delete(request);
    }
}

dispatcher::~dispatcher()
{
    stop();
}

CHIP_ERROR dispatcher::dispatch(chip::EndpointId local_endpoint, const invoke_command::request_t &request,
                                invoke_command::delegate *delegate)
{
    ReturnErrorOnFailure(Prune());
    return DispatchRequest::Start(mRequests, local_endpoint, request, delegate);
}

CHIP_ERROR dispatcher::dispatch(chip::EndpointId local_endpoint, const read_command::request_t &request,
                                read_command::delegate &delegate)
{
    ReturnErrorOnFailure(Prune());
    return DispatchRequest::Start(mRequests, local_endpoint, request, delegate);
}

CHIP_ERROR dispatcher::dispatch(chip::EndpointId local_endpoint, const write_command::request_t &request,
                                write_command::delegate *delegate)
{
    ReturnErrorOnFailure(Prune());
    return DispatchRequest::Start(mRequests, local_endpoint, request, delegate);
}

CHIP_ERROR dispatcher::dispatch(chip::EndpointId local_endpoint, const subscribe_command::request_t &request,
                                subscribe_command::delegate &delegate)
{
    ReturnErrorOnFailure(Prune());
    return DispatchRequest::Start(mRequests, local_endpoint, request, delegate);
}

void initialize()
{
    // Keep startup independent of dispatcher callbacks so unused client code can be linked out.
    if (initialized) {
        return;
    }
    initialized = true;
#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
    if (endpoint::get_cluster_count(chip::kInvalidEndpointId, chip::app::Clusters::Binding::Id,
                                    CLUSTER_FLAG_SERVER) > 0 &&
            chip::DeviceLayer::PlatformMgr().ScheduleWork(InitializeManagerOnChipTask) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to schedule Binding Manager initialization");
    }
#endif // CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
}

} // namespace binding
} // namespace client
} // namespace esp_matter
