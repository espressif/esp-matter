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

#include <esp_matter_controller_client.h>
#include <esp_matter_controller_icd_client.h>

#include <app-common/zap-generated/cluster-objects.h>
#include <app/CommandSender.h>
#include <app/data-model/Decode.h>
#include <crypto/CHIPCryptoPAL.h>
#include <lib/support/AutoRelease.h>
#include <lib/support/CHIPMem.h>
#include <lib/support/CodeUtils.h>
#include <platform/CHIPDeviceLayer.h>

#include <cstring>

using namespace ::chip;

namespace esp_matter::controller::icd_client {

namespace {

namespace IcdManagement = app::Clusters::IcdManagement;

CHIP_ERROR find_entry(const ScopedNodeId &peer_node, app::ICDClientInfo &info, bool &found)
{
    AutoRelease iter(matter_controller_client::get_instance().get_icd_client_storage().IterateICDClientInfo());
    VerifyOrReturnError(!iter.IsNull(), CHIP_ERROR_NO_MEMORY);
    found = false;
    while (iter->Next(info)) {
        if (info.peer_node == peer_node) {
            found = true;
            break;
        }
    }
    return CHIP_NO_ERROR;
}

bool valid_verification_key(const uint8_t *key, size_t size)
{
    return key == nullptr ? size == 0 : size == Crypto::kAES_CCM128_Key_Length;
}

} // namespace

struct interaction::context final : app::CommandSender::ExtendableCallback {
    context(interaction *owner, delegate *delegate, const request_t &request, FabricIndex fabric_index)
        : m_owner(owner)
        , m_delegate(delegate)
        , m_peer_node(request.node_id, fabric_index)
        , m_check_in_node(request.method == METHOD_REGISTER ? request.params.registration.check_in_node_id :
                          request.params.unregistration.check_in_node_id, fabric_index)
        , m_method(request.method)
        , m_on_connected(on_connected, this)
        , m_on_connection_failure(on_connection_failure, this)
    {
    }

    ~context() override
    {
        m_on_connected.Cancel();
        m_on_connection_failure.Cancel();
    }

    CHIP_ERROR copy_request(const request_t &request)
    {
        const uint8_t *verification_key;
        size_t verification_key_size;
        if (m_method == METHOD_REGISTER) {
            const auto &params = request.params.registration;
            m_monitored_subject = params.monitored_subject;
            m_client_type = params.client_type;
            ReturnErrorOnFailure(m_key.SetLength(params.key_size));
            memcpy(m_key.Bytes(), params.key, params.key_size);
            verification_key = params.verification_key;
            verification_key_size = params.verification_key_size;
        } else {
            verification_key = request.params.unregistration.verification_key;
            verification_key_size = request.params.unregistration.verification_key_size;
        }
        if (verification_key != nullptr) {
            ReturnErrorOnFailure(m_verification_key.SetLength(verification_key_size));
            memcpy(m_verification_key.Bytes(), verification_key, verification_key_size);
        }
        return CHIP_NO_ERROR;
    }

    static void begin(intptr_t arg)
    {
        auto *self = reinterpret_cast<context *>(arg);
        if (self->m_owner == nullptr) {
            self->finish(CHIP_ERROR_CANCELLED);
            return;
        }

        auto &controller = matter_controller_client::get_instance();
        // Do not resolve the captured node IDs against a newly selected fabric.
        if (controller.get_fabric_index() != self->m_peer_node.GetFabricIndex()) {
            self->finish(CHIP_ERROR_INVALID_FABRIC_INDEX);
            return;
        }
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
        auto *device_controller = controller.get_commissioner();
#else
        auto *device_controller = controller.get_controller();
#endif
        self->m_connecting = true;
        // These callbacks may finish and delete self inline. An error return
        // guarantees that neither callback has run or will run.
        CHIP_ERROR err = device_controller->GetConnectedDevice(self->m_peer_node.GetNodeId(), &self->m_on_connected,
                                                               &self->m_on_connection_failure);
        if (err != CHIP_NO_ERROR) {
            self->finish(err);
        }
    }

    template <typename Request>
    CHIP_ERROR send(const Request &request, Messaging::ExchangeManager &exchange_manager,
                    const SessionHandle &session_handle)
    {
        m_sender.reset(Platform::New<app::CommandSender>(this, &exchange_manager));
        VerifyOrReturnError(m_sender != nullptr, CHIP_ERROR_NO_MEMORY);
        app::CommandPathParams path = {kRootEndpointId, 0, Request::GetClusterId(), Request::GetCommandId(),
                                       app::CommandPathFlags::kEndpointIdValid
                                      };
        ReturnErrorOnFailure(m_sender->AddRequestData(path, request));
        return m_sender->SendCommandRequest(session_handle);
    }

    static void on_connected(void *arg, Messaging::ExchangeManager &exchange_manager, const SessionHandle &session_handle)
    {
        auto *self = static_cast<context *>(arg);
        self->m_connecting = false;
        CHIP_ERROR err;
        if (self->m_method == METHOD_REGISTER) {
            IcdManagement::Commands::RegisterClient::Type request;
            request.checkInNodeID = self->m_check_in_node.GetNodeId();
            request.monitoredSubject = self->m_monitored_subject;
            request.key = self->m_key.Span();
            request.clientType = self->m_client_type;
            if (self->m_verification_key.Length() != 0) {
                request.verificationKey.SetValue(self->m_verification_key.Span());
            }
            err = self->send(request, exchange_manager, session_handle);
        } else {
            IcdManagement::Commands::UnregisterClient::Type request;
            request.checkInNodeID = self->m_check_in_node.GetNodeId();
            if (self->m_verification_key.Length() != 0) {
                request.verificationKey.SetValue(self->m_verification_key.Span());
            }
            err = self->send(request, exchange_manager, session_handle);
        }
        if (err != CHIP_NO_ERROR) {
            self->finish(err);
        }
    }

    static void on_connection_failure(void *arg, const ScopedNodeId &, CHIP_ERROR err)
    {
        static_cast<context *>(arg)->finish(err);
    }

    CHIP_ERROR store_registration(uint32_t icd_counter)
    {
        // Read the previous handles now: a background key refresh may have
        // replaced them while this command was establishing its CASE session.
        app::ICDClientInfo previous;
        bool found;
        ReturnErrorOnFailure(find_entry(m_peer_node, previous, found));

        app::ICDClientInfo info;
        info.peer_node = m_peer_node;
        info.check_in_node = m_check_in_node;
        info.monitored_subject = m_monitored_subject;
        info.start_icd_counter = icd_counter;
        info.offset = 0;
        info.client_type = m_client_type;

        auto &storage = matter_controller_client::get_instance().get_icd_client_storage();
        ReturnErrorOnFailure(storage.SetKey(info, m_key.Span()));
        CHIP_ERROR err = storage.StoreEntry(info);
        if (err != CHIP_NO_ERROR) {
            storage.RemoveKey(info);
            return err;
        }
        if (found) {
            storage.RemoveKey(previous);
        }
        return CHIP_NO_ERROR;
    }

    void OnResponse(app::CommandSender *, const app::CommandSender::ResponseData &data) override
    {
        VerifyOrReturn(!m_result_received);
        m_result_received = true;
        m_error = CHIP_ERROR_SCHEMA_MISMATCH;
        VerifyOrReturn(data.path.mEndpointId == kRootEndpointId && data.path.mClusterId == IcdManagement::Id);

        response_t response;
        response.status = data.statusIB;
        if (m_method == METHOD_REGISTER && data.statusIB.IsSuccess()) {
            using Response = IcdManagement::Commands::RegisterClientResponse::DecodableType;
            VerifyOrReturn(data.data != nullptr && data.path.mCommandId == Response::GetCommandId());
            Response decoded;
            m_error = app::DataModel::Decode(*data.data, decoded);
            VerifyOrReturn(m_error == CHIP_NO_ERROR);
            response.icd_counter.SetValue(decoded.ICDCounter);
        } else {
            const CommandId expected = m_method == METHOD_REGISTER ? IcdManagement::Commands::RegisterClient::Id :
                                       IcdManagement::Commands::UnregisterClient::Id;
            VerifyOrReturn(data.data == nullptr && data.path.mCommandId == expected);
        }

        m_error = response.status.ToChipError();
        if (m_delegate != nullptr) {
            m_delegate->OnResponse(response);
        }
        if (m_error == CHIP_NO_ERROR) {
            auto &storage = matter_controller_client::get_instance().get_icd_client_storage();
            m_error = m_method == METHOD_REGISTER ? store_registration(response.icd_counter.Value()) :
                      storage.DeleteEntry(m_peer_node);
        }
    }

    void OnError(const app::CommandSender *, const app::CommandSender::ErrorData &data) override
    {
        VerifyOrReturn(!m_result_received);
        m_result_received = true;
        m_error = data.error;
        // Non-path-specific remote status responses arrive through OnError.
        if (m_delegate != nullptr && data.error.IsIMStatus()) {
            response_t response;
            response.status = app::StatusIB(data.error);
            m_delegate->OnResponse(response);
        }
    }

    void OnDone(app::CommandSender *) override
    {
        m_sender.reset();
        finish(m_error);
    }

    void finish(CHIP_ERROR error)
    {
        auto **link = &s_pending;
        while (*link != this) {
            link = &(*link)->m_next;
        }
        *link = m_next;
        if (m_owner != nullptr) {
            m_owner->m_context = nullptr;
        }
        auto *delegate = m_delegate;
        Platform::Delete(this);
        if (delegate != nullptr) {
            delegate->OnDone(error);
        }
    }

    static context *s_pending;
    context *m_next = nullptr;
    interaction *m_owner;
    delegate *m_delegate;
    ScopedNodeId m_peer_node;
    ScopedNodeId m_check_in_node;
    method_t m_method;
    uint64_t m_monitored_subject = 0;
    IcdManagement::ClientTypeEnum m_client_type = IcdManagement::ClientTypeEnum::kPermanent;
    Crypto::SensitiveDataBuffer<Crypto::kAES_CCM128_Key_Length> m_key;
    Crypto::SensitiveDataBuffer<Crypto::kAES_CCM128_Key_Length> m_verification_key;
    bool m_connecting = false;
    bool m_result_received = false;
    CHIP_ERROR m_error = CHIP_END_OF_TLV;
    Callback::Callback<OnDeviceConnected> m_on_connected;
    Callback::Callback<OnDeviceConnectionFailure> m_on_connection_failure;
    Platform::UniquePtr<app::CommandSender> m_sender;
};

interaction::context *interaction::context::s_pending = nullptr;

interaction::interaction() = default;

interaction::~interaction()
{
    stop();
}

CHIP_ERROR interaction::start(const request_t &request, delegate *delegate)
{
    VerifyOrReturnError(!active(), CHIP_ERROR_BUSY);
    VerifyOrReturnError(delegate != nullptr && IsOperationalNodeId(request.node_id), CHIP_ERROR_INVALID_ARGUMENT);
    if (request.method == METHOD_REGISTER) {
        const auto &params = request.params.registration;
        VerifyOrReturnError(IsOperationalNodeId(params.check_in_node_id) &&
                            params.key != nullptr && params.key_size == Crypto::kAES_CCM128_Key_Length &&
                            (params.client_type == IcdManagement::ClientTypeEnum::kPermanent ||
                             params.client_type == IcdManagement::ClientTypeEnum::kEphemeral) &&
                            valid_verification_key(params.verification_key, params.verification_key_size),
                            CHIP_ERROR_INVALID_ARGUMENT);
    } else {
        VerifyOrReturnError(request.method == METHOD_UNREGISTER, CHIP_ERROR_INVALID_ARGUMENT);
        const auto &params = request.params.unregistration;
        VerifyOrReturnError(IsOperationalNodeId(params.check_in_node_id) &&
                            valid_verification_key(params.verification_key, params.verification_key_size),
                            CHIP_ERROR_INVALID_ARGUMENT);
    }

    const FabricIndex fabric_index = matter_controller_client::get_instance().get_fabric_index();
    VerifyOrReturnError(fabric_index != kUndefinedFabricIndex, CHIP_ERROR_INVALID_FABRIC_INDEX);
    const ScopedNodeId peer_node(request.node_id, fabric_index);
    for (auto *pending = context::s_pending; pending != nullptr; pending = pending->m_next) {
        VerifyOrReturnError(pending->m_peer_node != peer_node, CHIP_ERROR_BUSY);
    }
    if (request.method == METHOD_UNREGISTER) {
        app::ICDClientInfo info;
        bool found;
        ReturnErrorOnFailure(find_entry(peer_node, info, found));
        VerifyOrReturnError(found, CHIP_ERROR_NOT_FOUND);
        VerifyOrReturnError(info.check_in_node.GetNodeId() == request.params.unregistration.check_in_node_id,
                            CHIP_ERROR_INVALID_ARGUMENT);
    }

    auto *operation = Platform::New<context>(this, delegate, request, fabric_index);
    VerifyOrReturnError(operation != nullptr, CHIP_ERROR_NO_MEMORY);
    CHIP_ERROR err = operation->copy_request(request);
    if (err != CHIP_NO_ERROR) {
        Platform::Delete(operation);
        return err;
    }
    operation->m_next = context::s_pending;
    context::s_pending = operation;
    m_context = operation;
    err = DeviceLayer::PlatformMgr().ScheduleWork(context::begin, reinterpret_cast<intptr_t>(operation));
    if (err != CHIP_NO_ERROR) {
        operation->m_delegate = nullptr;
        operation->finish(err);
    }
    return err;
}

void interaction::stop()
{
    VerifyOrReturn(m_context != nullptr);
    auto *operation = m_context;
    m_context = nullptr;
    operation->m_owner = nullptr;
    operation->m_delegate = nullptr;
    if (operation->m_connecting) {
        // Unlink our CASE callbacks without cancelling other session users.
        operation->finish(CHIP_ERROR_CANCELLED);
    }
    // Queued work owns its context until begin(). An in-flight sender owns it
    // until OnDone(), so registration storage stays consistent after detaching.
}

bool interaction::active() const
{
    return m_context != nullptr;
}

CHIP_ERROR for_each_registered_icd(entry_callback callback, void *context)
{
    VerifyOrReturnError(callback != nullptr, CHIP_ERROR_INVALID_ARGUMENT);
    AutoRelease iter(matter_controller_client::get_instance().get_icd_client_storage().IterateICDClientInfo());
    VerifyOrReturnError(!iter.IsNull(), CHIP_ERROR_NO_MEMORY);
    app::ICDClientInfo info;
    while (iter->Next(info)) {
        const entry_t entry = {info.peer_node, info.check_in_node, info.monitored_subject, info.start_icd_counter,
                               info.offset, info.client_type
                              };
        ReturnErrorOnFailure(callback(entry, context));
    }
    return CHIP_NO_ERROR;
}

} // namespace esp_matter::controller::icd_client
