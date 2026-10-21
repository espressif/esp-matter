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

#include <esp_matter_client_write_command.h>

#include "esp_matter_client_operation.h"

#include <app/ChunkedWriteCallback.h>
#include <app/InteractionModelEngine.h>
#include <app/WriteClient.h>
#include <transport/GroupSession.h>
#include <esp_log.h>
#include <platform/CHIPDeviceLayer.h>

namespace esp_matter {
namespace client {
namespace write_command {

static const char *TAG = "client_write";

using chip::app::ConcreteDataAttributePath;
using chip::app::StatusIB;
using chip::app::WriteClient;

namespace {

/* Encode paths and values in request order before dispatch. */
CHIP_ERROR PrepareRemoteWrite(WriteClient &client, chip::Span<const attribute_write_t> attributes, bool group)
{
    operation::EncodedElement staging;
    for (size_t index = 0; index < attributes.size(); ++index) {
        ReturnErrorOnFailure(staging.Encode(attributes[index].value));
        chip::TLV::TLVReader reader;
        ReturnErrorOnFailure(staging.GetReader(reader));
        auto path = attributes[index].path;
        if (group) {
            path.mEndpointId = chip::kInvalidEndpointId;
        }
        ReturnErrorOnFailure(client.PutPreencodedAttribute(path, reader));
        staging.Free();
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR ValidateRequest(const request_t &request, bool group = false)
{
    chip::Span<const attribute_write_t> attributes = request.attributes;
    VerifyOrReturnError(!attributes.empty(), CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(attributes.size() <= SIZE_MAX / sizeof(attribute_write_t), CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(attributes.data() != nullptr, CHIP_ERROR_INVALID_ARGUMENT);

    for (size_t index = 0; index < attributes.size(); ++index) {
        const attribute_write_t &attribute = attributes[index];
        VerifyOrReturnError(attribute.value != nullptr, CHIP_ERROR_INVALID_ARGUMENT);
        auto path = attribute.path;
        if (group) {
            path.mEndpointId = 0;
            VerifyOrReturnError(!path.mDataVersion.HasValue(), CHIP_ERROR_INVALID_ARGUMENT);
        }
        VerifyOrReturnError(path.IsValid() && !path.IsListItemOperation(), CHIP_ERROR_INVALID_ARGUMENT);
    }
    VerifyOrReturnError(!group || !request.options.timed_timeout_ms.HasValue(), CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE);
    return CHIP_NO_ERROR;
}

} // namespace

class interaction::Impl final : public operation::CaseConnection,
    public WriteClient::Callback {
public:
    Impl(const target_t &target, const request_t &request, delegate *delegate)
        : mTarget(target), mOptions(request.options), mDelegate(delegate), mChunkedCallback(this) {}

    ~Impl()
    {
        VerifyOrDie(!mInCallback);
        mDelegate = nullptr;
        chip::DeviceLayer::SystemLayer().CancelTimer(Begin, this);
        Cancel();
        mClient.reset();
    }

    bool active() const
    {
        return mActive;
    }

    CHIP_ERROR Prepare(const request_t &request)
    {
        auto *engine = chip::app::InteractionModelEngine::GetInstance();
        auto *exchange_manager = engine == nullptr ? nullptr : engine->GetExchangeManager();
        VerifyOrReturnError(exchange_manager != nullptr, CHIP_ERROR_INCORRECT_STATE);
        mClient = chip::Platform::MakeUnique<WriteClient>(exchange_manager, &mChunkedCallback,
                                                          mOptions.timed_timeout_ms, mTarget.type == target_type_t::group);
        VerifyOrReturnError(mClient != nullptr, CHIP_ERROR_NO_MEMORY);
        ReturnErrorOnFailure(PrepareRemoteWrite(*mClient, request.attributes, mTarget.type == target_type_t::group));
        VerifyOrReturnError(!mOptions.timed_timeout_ms.HasValue() || !mClient->IsWriteRequestChunked(),
                            CHIP_ERROR_NO_MEMORY);

        // A cancellable timer provides acceptance-before-callback ordering and
        // lets stop release queued work without leaving a raw caller pointer.
        return chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::kZero, Begin, this);
    }

    CHIP_ERROR PrepareGroup(const request_t &request)
    {
        ReturnErrorOnFailure(Prepare(request));
        chip::Transport::OutgoingGroupSession session(mTarget.group_id, mTarget.fabric_index);
        CHIP_ERROR err = mClient->SendWriteRequest(chip::SessionHandle(session));
        mClient.reset();
        return err;
    }

    void OnResponse(const WriteClient *, const ConcreteDataAttributePath &path, StatusIB status) override
    {
        if (mDelegate != nullptr) {
            operation::CallbackScope scope(mInCallback);
            mDelegate->OnResponse(mTarget, path, status);
        } else if (!status.IsSuccess()) {
            ESP_LOGE(TAG, "Write response failed: %s", chip::ErrorStr(status.ToChipError()));
        }
    }

    void OnError(const WriteClient *, CHIP_ERROR err) override
    {
        mTerminalError = err;
    }

    void OnDone(WriteClient *) override
    {
        // Group SendWriteRequest completes synchronously; Begin delivers completion after start returns.
        if (mTarget.type == target_type_t::node) {
            Finish(mTerminalError);
        }
    }

    CHIP_ERROR Send(chip::Messaging::ExchangeManager &, const chip::SessionHandle &session) override
    {
        VerifyOrReturnError(mClient != nullptr, CHIP_ERROR_INCORRECT_STATE);
        return mClient->SendWriteRequest(session);
    }

    void Finish(CHIP_ERROR err) override
    {
        if (DeferFinish(err)) {
            return;
        }
        mClient.reset();
        mActive = false;
        delegate *delegate = mDelegate;
        mDelegate = nullptr;
        if (delegate != nullptr) {
            // Terminal callback: the delegate may destroy this interaction.
            const target_t target = mTarget;
            delegate->OnDone(target, err);
        } else if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Write failed: %s", chip::ErrorStr(err));
        }
    }

private:
    static void Begin(chip::System::Layer *, void *context)
    {
        auto *self = static_cast<Impl *>(context);
        if (self->mTarget.type == target_type_t::group) {
            self->Finish(self->mTerminalError);
            return;
        }
        CHIP_ERROR err = self->Start(chip::ScopedNodeId(self->mTarget.node_id, self->mTarget.fabric_index));
        if (err != CHIP_NO_ERROR) {
            self->Finish(err);
        }
    }

    target_t mTarget;
    options_t mOptions;
    delegate *mDelegate;
    chip::app::ChunkedWriteCallback mChunkedCallback;
    chip::Platform::UniquePtr<WriteClient> mClient;
    CHIP_ERROR mTerminalError = CHIP_NO_ERROR;
    bool mInCallback = false;
    bool mActive = true;
};

interaction::~interaction()
{
    stop();
}

bool interaction::active() const
{
    return mImpl != nullptr && mImpl->active();
}

void interaction::stop()
{
    chip::Platform::Delete(mImpl);
    mImpl = nullptr;
}

CHIP_ERROR interaction::start(const chip::ScopedNodeId &scoped_node_id, const request_t &request, delegate *delegate)
{
    VerifyOrReturnError(!active(), CHIP_ERROR_BUSY);
    stop();
    ReturnErrorOnFailure(ValidateRequest(request));
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
    VerifyOrReturnError(!active(), CHIP_ERROR_BUSY);
    stop();
    ReturnErrorOnFailure(ValidateRequest(request, true));
    ReturnErrorOnFailure(operation::ValidateGroupTarget(group_id, fabric_index));

    mImpl = chip::Platform::New<Impl>(target_t(group_id, fabric_index), request, delegate);
    VerifyOrReturnError(mImpl != nullptr, CHIP_ERROR_NO_MEMORY);
    CHIP_ERROR err = mImpl->PrepareGroup(request);
    if (err != CHIP_NO_ERROR) {
        stop();
    }
    return err;
}

} // namespace write_command
} // namespace client
} // namespace esp_matter
