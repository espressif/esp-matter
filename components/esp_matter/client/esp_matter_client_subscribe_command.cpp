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

#include <esp_matter_client_subscribe_command.h>

#include "esp_matter_client_operation.h"

#include <app/BufferedReadCallback.h>
#include <app/InteractionModelEngine.h>
#include <app/ReadClient.h>
#include <app/ReadPrepareParams.h>
#include <lib/support/CodeUtils.h>
#include <platform/CHIPDeviceLayer.h>

namespace esp_matter {
namespace client {
namespace subscribe_command {
namespace {

using chip::SessionHandle;
using chip::app::BufferedReadCallback;
using chip::app::ConcreteDataAttributePath;
using chip::app::InteractionModelEngine;
using chip::app::ReadClient;
using chip::app::ReadPrepareParams;
using chip::app::StatusIB;
using chip::Messaging::ExchangeManager;

CHIP_ERROR ValidateRequest(chip::Span<const chip::app::AttributePathParams> attribute_paths,
                           chip::Span<const chip::app::EventPathParams> event_paths,
                           const options_t &options)
{
    VerifyOrReturnError(options.min_interval <= options.max_interval,
                        CHIP_ERROR_INVALID_ARGUMENT);

    return operation::ValidateReadPaths(attribute_paths, event_paths);
}

} // namespace

class subscription::Impl final : public operation::CaseConnection,
    public ReadClient::Callback {
public:
    Impl(const chip::ScopedNodeId &peer, const options_t &options)
        : mPeer(peer),
          mOptions(options), mBufferedCallback(*this) {}

    ~Impl()
    {
        Stop();
    }

    CHIP_ERROR CopyPaths(chip::Span<const chip::app::AttributePathParams> attribute_paths,
                         chip::Span<const chip::app::EventPathParams> event_paths)
    {
        return mPaths.Copy(attribute_paths, event_paths);
    }

    CHIP_ERROR Begin(delegate &delegate)
    {
        CHIP_ERROR err = mOptions.auto_resubscribe ? StartAuto() : Start(mPeer);
        if (err != CHIP_NO_ERROR) {
            return err;
        }

        if (mState == ConnectionState::terminated) {
            return mStateError == CHIP_NO_ERROR ? CHIP_ERROR_INTERNAL : mStateError;
        }

        mDelegate = &delegate;
        return CHIP_NO_ERROR;
    }

    void NotifyState()
    {
        if (mDelegate != nullptr) {
            operation::CallbackScope scope(mInCallback);
            mDelegate->OnStateChanged(target_t(mPeer), State(), mStateError);
        }
    }

    CHIP_ERROR RetryNow()
    {
        VerifyOrReturnError(mOptions.auto_resubscribe &&
                            mState == ConnectionState::backoff && mClient != nullptr,
                            CHIP_ERROR_INCORRECT_STATE);

        mState = ConnectionState::triggering_retry;

        if (!mClient->TriggerResubscribeIfScheduled("client RetryNow")) {
            mState = ConnectionState::backoff;
            return CHIP_ERROR_INCORRECT_STATE;
        }

        // TriggerResubscribeIfScheduled may synchronously retry, enter a new
        // backoff, or terminate the subscription.
        VerifyOrReturnError(mState != ConnectionState::terminated && mClient != nullptr,
                            mStateError == CHIP_NO_ERROR ? CHIP_ERROR_INCORRECT_STATE
                            : mStateError);

        if (mState == ConnectionState::triggering_retry) {
            BeginRetry();
        }

        return CHIP_NO_ERROR;
    }

    void Stop()
    {
        VerifyOrDie(!mInCallback);
        mState = ConnectionState::terminated;
        mStateError = CHIP_NO_ERROR;
        mDelegate = nullptr;
        Cancel();
        mClient.reset();
        mPaths.Free();
    }

    state_t State() const
    {
        switch (mState) {
        case ConnectionState::connecting:
            return state_t::connecting;
        case ConnectionState::active:
            return state_t::active;
        case ConnectionState::backoff:
        case ConnectionState::triggering_retry:
            return state_t::backoff;
        case ConnectionState::terminated:
            return state_t::terminated;
        }

        return state_t::terminated;
    }

    chip::ScopedNodeId Peer() const
    {
        return mPeer;
    }

    chip::Optional<chip::SubscriptionId> Id() const
    {
        return mId;
    }

    void OnReportBegin() override
    {
        BeginRetry();
        mReportState = ReportState::open;
        if (mDelegate != nullptr) {
            operation::CallbackScope scope(mInCallback);
            mDelegate->OnReportBegin(target_t(mPeer));
        }
    }

    void OnAttributeData(const ConcreteDataAttributePath &path,
                         chip::TLV::TLVReader *value,
                         const StatusIB &status) override
    {
        if (mDelegate != nullptr) {
            operation::CallbackScope scope(mInCallback);
            mDelegate->OnAttribute(target_t(mPeer), path, value, status);
        }
    }

    void OnEventData(const chip::app::EventHeader &header,
                     chip::TLV::TLVReader *value,
                     const StatusIB *status) override
    {
        if (mDelegate != nullptr) {
            operation::CallbackScope scope(mInCallback);
            mDelegate->OnEvent(target_t(mPeer), header, value, status);
        }
    }

    void OnReportEnd() override
    {
        if (mReportState != ReportState::open) {
            return;
        }

        mReportState = ReportState::complete;
        if (mDelegate != nullptr) {
            operation::CallbackScope scope(mInCallback);
            mDelegate->OnReportEnd(target_t(mPeer), CHIP_NO_ERROR);
        }

        MaybeActive();
    }

    void OnSubscriptionEstablished(chip::SubscriptionId id) override
    {
        BeginRetry();
        mId.SetValue(id);

        if (mReportState == ReportState::none) {
            // CHIP omits report boundaries for a successfully parsed empty priming
            // report.
            mReportState = ReportState::complete;
        }

        MaybeActive();
    }

    CHIP_ERROR OnResubscriptionNeeded(ReadClient *client,
                                      CHIP_ERROR error) override
    {
        BeginRetry();
        mId.ClearValue();
        mReportState = ReportState::none;
        VerifyOrReturnError(mOptions.auto_resubscribe, error);

        CHIP_ERROR err = client->DefaultResubscribePolicy(error);
        if (err == CHIP_ERROR_LIT_SUBSCRIBE_INACTIVE_TIMEOUT) {
            EnterBackoff(error);
            return err;
        }

        ReturnErrorOnFailure(err);
        EnterBackoff(error);

        return CHIP_NO_ERROR;
    }

    void OnCASESessionEstablished(const SessionHandle &,
                                  ReadPrepareParams &) override
    {
        BeginRetry();
    }

    void OnError(CHIP_ERROR err) override
    {
        if (mReportState == ReportState::open && mDelegate != nullptr) {
            operation::CallbackScope scope(mInCallback);
            mDelegate->OnReportEnd(target_t(mPeer), err);
        }

        mReportState = ReportState::none;
        mStateError = err;
    }

    void OnDeallocatePaths(ReadPrepareParams &&params) override
    {
        mPaths.Deallocate(params);
    }

    void OnDone(ReadClient *) override
    {
        if (mState != ConnectionState::terminated) {
            Finish(mStateError == CHIP_NO_ERROR ? CHIP_ERROR_CONNECTION_ABORTED
                   : mStateError);
        }
    }

private:
    enum class ConnectionState : uint8_t {
        connecting,
        active,
        backoff,
        // Distinguishes no synchronous callback from a callback that entered a new backoff.
        triggering_retry,
        terminated,
    };

    enum class ReportState : uint8_t {
        none,
        open,
        complete,
    };

    CHIP_ERROR CreateClient(ExchangeManager *exchange_manager)
    {
        if (mClient == nullptr) {
            mClient = chip::Platform::MakeUnique<ReadClient>(
                          InteractionModelEngine::GetInstance(), exchange_manager,
                          mBufferedCallback, ReadClient::InteractionType::Subscribe);
        }

        return mClient == nullptr ? CHIP_ERROR_NO_MEMORY : CHIP_NO_ERROR;
    }

    void FillParams(ReadPrepareParams &params)
    {
        mPaths.Fill(params);
        params.mEventNumber = mOptions.min_event_number;
        params.mMinIntervalFloorSeconds = mOptions.min_interval;
        params.mMaxIntervalCeilingSeconds = mOptions.max_interval;
        params.mKeepSubscriptions = mOptions.keep_subscriptions;
        params.mIsFabricFiltered = mOptions.fabric_filtered;
        params.mTimeout = mOptions.response_timeout;
    }

    CHIP_ERROR StartAuto()
    {
        ReturnErrorOnFailure(CreateClient(nullptr));

        ReadPrepareParams params;
        FillParams(params);

        CHIP_ERROR err =
            mClient->SendAutoResubscribeRequest(mPeer, std::move(params));

        return err;
    }

public:
    CHIP_ERROR Send(ExchangeManager &exchange_manager,
                    const SessionHandle &session) override
    {
        ReturnErrorOnFailure(CreateClient(&exchange_manager));

        ReadPrepareParams params(session);
        FillParams(params);

        return mClient->SendRequest(params);
    }

    void Finish(CHIP_ERROR err) override
    {
        if (DeferFinish(err)) {
            return;
        }

        mStateError = err;
        mId.ClearValue();
        Cancel();
        mState = ConnectionState::terminated;
        NotifyState();
        mDelegate = nullptr;
        mClient.reset();
    }

private:
    void EnterBackoff(CHIP_ERROR error)
    {
        mState = ConnectionState::backoff;
        mStateError = error;
        NotifyState();
    }

    void MaybeActive()
    {
        if (mId.HasValue() && mReportState == ReportState::complete &&
                mState != ConnectionState::active &&
                mState != ConnectionState::terminated) {
            mState = ConnectionState::active;
            mStateError = CHIP_NO_ERROR;
            NotifyState();
        }
    }

    void BeginRetry()
    {
        if (mState == ConnectionState::backoff ||
                mState == ConnectionState::triggering_retry) {
            mState = ConnectionState::connecting;
            NotifyState();
        }
    }

    chip::ScopedNodeId mPeer;
    options_t mOptions;
    delegate *mDelegate = nullptr;
    BufferedReadCallback mBufferedCallback;
    chip::Platform::UniquePtr<ReadClient> mClient;
    operation::OwnedReadPaths mPaths;
    chip::Optional<chip::SubscriptionId> mId;
    CHIP_ERROR mStateError = CHIP_NO_ERROR;
    ConnectionState mState = ConnectionState::connecting;
    ReportState mReportState = ReportState::none;
    bool mInCallback = false;
};

subscription::subscription() = default;

subscription::~subscription()
{
    stop();
}

CHIP_ERROR subscription::start(const chip::ScopedNodeId &scoped_node_id, const request_t &request,
                               delegate &delegate)
{
    if (mImpl != nullptr) {
        VerifyOrReturnError(mImpl->State() == state_t::terminated,
                            CHIP_ERROR_INCORRECT_STATE);
        stop();
    }
    ReturnErrorOnFailure(operation::ValidateRemoteTarget(scoped_node_id));
    ReturnErrorOnFailure(ValidateRequest(request.attribute_paths,
                                         request.event_paths, request.options));

    auto *implementation = chip::Platform::New<subscription::Impl>(scoped_node_id, request.options);
    VerifyOrReturnError(implementation != nullptr, CHIP_ERROR_NO_MEMORY);

    CHIP_ERROR err = implementation->CopyPaths(request.attribute_paths, request.event_paths);
    if (err == CHIP_NO_ERROR) {
        err = implementation->Begin(delegate);
    }

    if (err != CHIP_NO_ERROR) {
        chip::Platform::Delete(implementation);
        return err;
    }

    mImpl = implementation;
    // Publish the handle before its first callback. A timer per subscription here
    // consumes pool headroom needed by CASE/MRP when many starts share one task turn.
    implementation->NotifyState();

    return CHIP_NO_ERROR;
}

CHIP_ERROR subscription::retry_now()
{
    VerifyOrReturnError(mImpl != nullptr, CHIP_ERROR_INCORRECT_STATE);

    return mImpl->RetryNow();
}

void subscription::stop()
{
    chip::Platform::Delete(mImpl);
    mImpl = nullptr;
}

state_t subscription::state() const
{
    return mImpl == nullptr ? state_t::terminated : mImpl->State();
}

chip::ScopedNodeId subscription::peer() const
{
    return mImpl == nullptr ? chip::ScopedNodeId() : mImpl->Peer();
}

chip::Optional<chip::SubscriptionId> subscription::id() const
{
    return mImpl == nullptr ? chip::NullOptional : mImpl->Id();
}

} // namespace subscribe_command
} // namespace client
} // namespace esp_matter
