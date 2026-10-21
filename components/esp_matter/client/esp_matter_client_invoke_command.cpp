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

#include <esp_matter_client_invoke_command.h>

#include "esp_matter_client_operation.h"

#include <app/CommandSender.h>
#include <app/InteractionModelEngine.h>
#include <esp_log.h>
#include <platform/CHIPDeviceLayer.h>
#include <transport/GroupSession.h>

namespace esp_matter {
namespace client {
namespace invoke_command {

static const char *TAG = "client_invoke";

using chip::app::CommandSender;

namespace {

CHIP_ERROR EncodeFields(const chip::app::DataModel::EncodableToTLV *value, operation::EncodedElement &fields)
{
    ReturnErrorOnFailure(fields.Encode(value));
    chip::TLV::TLVReader reader;
    ReturnErrorOnFailure(fields.GetReader(reader));
    return reader.GetType() == chip::TLV::kTLVType_Structure ? CHIP_NO_ERROR : CHIP_ERROR_INVALID_ARGUMENT;
}

} // namespace

class interaction::Impl final : public operation::CaseConnection,
    public CommandSender::ExtendableCallback {
public:
    Impl(const target_t &target, const request_t &request, delegate *delegate)
        : mTarget(target), mRequest(request), mDelegate(delegate)
    {
        mRequest.fields = nullptr;
    }

    ~Impl()
    {
        VerifyOrDie(!mInCallback);
        mDelegate = nullptr;
        chip::DeviceLayer::SystemLayer().CancelTimer(Begin, this);
        Cancel();
        mSender.reset();
    }

    bool active() const
    {
        return mActive;
    }

    CHIP_ERROR Prepare(const request_t &request)
    {
        ReturnErrorOnFailure(EncodeFields(request.fields, mFields));
        return chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::kZero, Begin, this);
    }

    CHIP_ERROR PrepareGroup(const request_t &request)
    {
        operation::EncodedElement fields;
        ReturnErrorOnFailure(EncodeFields(request.fields, fields));
        auto *exchange_manager = chip::app::InteractionModelEngine::GetInstance()->GetExchangeManager();
        VerifyOrReturnError(exchange_manager != nullptr, CHIP_ERROR_INCORRECT_STATE);
        chip::app::CommandPathParams path(mTarget.group_id, request.cluster_id, request.command_id,
                                          chip::app::CommandPathFlags::kGroupIdValid);
        chip::Transport::OutgoingGroupSession group_session(mTarget.group_id, mTarget.fabric_index);
        CommandSender sender(nullptr, exchange_manager);
        CommandSender::AddRequestDataParameters parameters;
        ReturnErrorOnFailure(sender.AddRequestData(path, fields, parameters));
        // Reserve queued completion before the network side effect.
        ReturnErrorOnFailure(chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::kZero, Begin, this));
        return sender.SendGroupCommandRequest(chip::SessionHandle(group_session));
    }

    void OnResponse(CommandSender *, const CommandSender::ResponseData &response) override
    {
        if (mDelegate != nullptr) {
            operation::CallbackScope scope(mInCallback);
            mDelegate->OnResponse(mTarget, response.path, response.statusIB, response.data);
        } else if (!response.statusIB.IsSuccess()) {
            ESP_LOGE(TAG, "Invoke response failed: %s", chip::ErrorStr(response.statusIB.ToChipError()));
        }
    }

    void OnError(const CommandSender *, const CommandSender::ErrorData &err) override
    {
        mTerminalError = err.error;
    }

    void OnDone(CommandSender *) override
    {
        Finish(mTerminalError);
    }

    CHIP_ERROR Send(chip::Messaging::ExchangeManager &exchange_manager, const chip::SessionHandle &session) override
    {
        mSender = chip::Platform::MakeUnique<CommandSender>(static_cast<CommandSender::ExtendableCallback *>(this),
                                                            &exchange_manager,
                                                            mRequest.options.timed_timeout_ms.HasValue());
        VerifyOrReturnError(mSender != nullptr, CHIP_ERROR_NO_MEMORY);
        chip::app::CommandPathParams path(mRequest.endpoint_id, mRequest.cluster_id, mRequest.command_id,
                                          chip::app::CommandPathFlags::kEndpointIdValid);
        CommandSender::AddRequestDataParameters parameters(mRequest.options.timed_timeout_ms);
        ReturnErrorOnFailure(mSender->AddRequestData(path, mFields, parameters));
        mFields.Free();
        return mSender->SendCommandRequest(session, mRequest.options.response_timeout);
    }

    void Finish(CHIP_ERROR err) override
    {
        if (DeferFinish(err)) {
            return;
        }
        mFields.Free();
        mActive = false;
        mSender.reset();
        delegate *delegate = mDelegate;
        mDelegate = nullptr;
        if (delegate != nullptr) {
            // Terminal callback: the delegate may destroy this interaction.
            const target_t target = mTarget;
            delegate->OnDone(target, err);
        } else if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Invoke failed: %s", chip::ErrorStr(err));
        }
    }

private:
    static void Begin(chip::System::Layer *, void *context)
    {
        auto *self = static_cast<Impl *>(context);
        if (self->mTarget.type == target_type_t::group) {
            self->Finish(CHIP_NO_ERROR);
        } else {
            CHIP_ERROR err = self->Start(chip::ScopedNodeId(self->mTarget.node_id, self->mTarget.fabric_index));
            if (err != CHIP_NO_ERROR) {
                self->Finish(err);
            }
        }
    }

    target_t mTarget;
    request_t mRequest;
    delegate *mDelegate;
    operation::EncodedElement mFields;
    chip::Platform::UniquePtr<CommandSender> mSender;
    CHIP_ERROR mTerminalError = CHIP_NO_ERROR;
    bool mActive = true;
    bool mInCallback = false;
};

interaction::~interaction()
{
    stop();
}

void interaction::stop()
{
    chip::Platform::Delete(mImpl);
    mImpl = nullptr;
}

bool interaction::active() const
{
    return mImpl != nullptr && mImpl->active();
}

CHIP_ERROR interaction::start(const chip::ScopedNodeId &scoped_node_id, const request_t &request, delegate *delegate)
{
    if (mImpl != nullptr) {
        VerifyOrReturnError(!mImpl->active(), CHIP_ERROR_INCORRECT_STATE);
        stop();
    }
    VerifyOrReturnError(request.endpoint_id != chip::kInvalidEndpointId &&
                        request.cluster_id != chip::kInvalidClusterId &&
                        request.command_id != chip::kInvalidCommandId, CHIP_ERROR_INVALID_ARGUMENT);
    ReturnErrorOnFailure(operation::ValidateRemoteTarget(scoped_node_id));
    mImpl = chip::Platform::New<Impl>(target_t(scoped_node_id), request, delegate);
    VerifyOrReturnError(mImpl != nullptr, CHIP_ERROR_NO_MEMORY);
    CHIP_ERROR err = mImpl->Prepare(request);
    if (err != CHIP_NO_ERROR) {
        stop();
    }
    return err;
}

CHIP_ERROR interaction::start(chip::GroupId group_id, chip::FabricIndex fabric_index,
                              const request_t &request, delegate *delegate)
{
    if (mImpl != nullptr) {
        VerifyOrReturnError(!mImpl->active(), CHIP_ERROR_INCORRECT_STATE);
        stop();
    }
    ReturnErrorOnFailure(operation::ValidateGroupTarget(group_id, fabric_index));
    VerifyOrReturnError(request.cluster_id != chip::kInvalidClusterId && request.command_id != chip::kInvalidCommandId,
                        CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(!request.options.timed_timeout_ms.HasValue() && !request.options.response_timeout.HasValue(),
                        CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE);
    mImpl = chip::Platform::New<Impl>(target_t(group_id, fabric_index), request, delegate);
    VerifyOrReturnError(mImpl != nullptr, CHIP_ERROR_NO_MEMORY);
    CHIP_ERROR err = mImpl->PrepareGroup(request);
    if (err != CHIP_NO_ERROR) {
        stop();
    }
    return err;
}

} // namespace invoke_command
} // namespace client
} // namespace esp_matter
