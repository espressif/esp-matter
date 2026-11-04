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

#include <esp_matter_client_read_command.h>

#include "esp_matter_client_operation.h"

#include <app/BufferedReadCallback.h>
#include <app/InteractionModelEngine.h>
#include <app/ReadClient.h>
#include <platform/CHIPDeviceLayer.h>

namespace esp_matter {
namespace client {
namespace read_command {

using chip::app::ConcreteDataAttributePath;
using chip::app::ReadClient;
using chip::app::ReadPrepareParams;
using chip::app::StatusIB;

class interaction::Impl final : public operation::CaseConnection, public ReadClient::Callback {
public:
    Impl(const chip::ScopedNodeId &target, const options_t &options, delegate &delegate)
        : mTarget(target), mOptions(options),
          mDelegate(&delegate), mBufferedCallback(*this)
    {
    }

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
        ReturnErrorOnFailure(mPaths.Copy(request.attribute_paths, request.event_paths));
        return chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::kZero, Begin, this);
    }

    CHIP_ERROR Send(chip::Messaging::ExchangeManager &exchange_manager, const chip::SessionHandle &session) override
    {
        mClient = chip::Platform::MakeUnique<ReadClient>(chip::app::InteractionModelEngine::GetInstance(),
                                                         &exchange_manager, mBufferedCallback,
                                                         ReadClient::InteractionType::Read);
        VerifyOrReturnError(mClient != nullptr, CHIP_ERROR_NO_MEMORY);
        ReadPrepareParams params(session);
        mPaths.Fill(params);
        params.mEventNumber = mOptions.min_event_number;
        params.mIsFabricFiltered = mOptions.fabric_filtered;
        params.mTimeout = mOptions.response_timeout;
        return mClient->SendRequest(params);
    }

    void Finish(CHIP_ERROR err) override
    {
        if (DeferFinish(err)) {
            return;
        }
        mClient.reset();
        mPaths.Free();
        mActive = false;
        delegate *delegate = mDelegate;
        mDelegate = nullptr;
        if (delegate != nullptr) {
            // Terminal callback: the delegate may destroy this interaction.
            const target_t target(mTarget);
            delegate->OnDone(target, err);
        }
    }

    void OnReportBegin() override
    {
        mReportOpen = true;
        if (mDelegate != nullptr) {
            operation::CallbackScope scope(mInCallback);
            mDelegate->OnReportBegin(target_t(mTarget));
        }
    }

    void OnAttributeData(const ConcreteDataAttributePath &path, chip::TLV::TLVReader *value,
                         const StatusIB &status) override
    {
        if (mDelegate != nullptr) {
            operation::CallbackScope scope(mInCallback);
            mDelegate->OnAttribute(target_t(mTarget), path, value, status);
        }
    }

    void OnEventData(const chip::app::EventHeader &header, chip::TLV::TLVReader *value,
                     const StatusIB *status) override
    {
        if (mDelegate != nullptr) {
            operation::CallbackScope scope(mInCallback);
            mDelegate->OnEvent(target_t(mTarget), header, value, status);
        }
    }

    void OnReportEnd() override
    {
        mReportOpen = false;
        if (mDelegate != nullptr) {
            operation::CallbackScope scope(mInCallback);
            mDelegate->OnReportEnd(target_t(mTarget), CHIP_NO_ERROR);
        }
    }

    void OnError(CHIP_ERROR err) override
    {
        if (mReportOpen) {
            mReportOpen = false;
            if (mDelegate != nullptr) {
                operation::CallbackScope scope(mInCallback);
                mDelegate->OnReportEnd(target_t(mTarget), err);
            }
        }
        mTerminalError = err;
    }

    void OnDone(ReadClient *) override
    {
        Finish(mTerminalError);
    }

private:
    static void Begin(chip::System::Layer *, void *context)
    {
        auto *self = static_cast<Impl *>(context);
        CHIP_ERROR err = self->Start(self->mTarget);
        if (err != CHIP_NO_ERROR) {
            self->Finish(err);
        }
    }

    chip::ScopedNodeId mTarget;
    options_t mOptions;
    delegate *mDelegate;
    operation::OwnedReadPaths mPaths;
    chip::app::BufferedReadCallback mBufferedCallback;
    chip::Platform::UniquePtr<ReadClient> mClient;
    CHIP_ERROR mTerminalError = CHIP_NO_ERROR;
    bool mReportOpen = false;
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

CHIP_ERROR interaction::start(const chip::ScopedNodeId &scoped_node_id, const request_t &request, delegate &delegate)
{
    VerifyOrReturnError(!active(), CHIP_ERROR_BUSY);
    stop();
    ReturnErrorOnFailure(operation::ValidateReadPaths(request.attribute_paths, request.event_paths));
    ReturnErrorOnFailure(operation::ValidateRemoteTarget(scoped_node_id));
    mImpl = chip::Platform::New<Impl>(scoped_node_id, request.options, delegate);
    VerifyOrReturnError(mImpl != nullptr, CHIP_ERROR_NO_MEMORY);
    CHIP_ERROR err = mImpl->Prepare(request);
    if (err != CHIP_NO_ERROR) {
        stop();
    }
    return err;
}

} // namespace read_command
} // namespace client
} // namespace esp_matter
