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

#include <esp_matter_client_target.h>

#include <app/InteractionModelEngine.h>
#include <app/OperationalSessionSetup.h>
#include <app/ReadPrepareParams.h>
#include <app/data-model/EncodableToTLV.h>
#include <credentials/FabricTable.h>
#include <lib/support/CHIPMem.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/Scoped.h>
#include <lib/support/Span.h>
#include <messaging/ExchangeMgr.h>
#include <stdint.h>
#include <transport/SessionManager.h>

namespace esp_matter {
namespace client {
namespace operation {

inline chip::FabricTable *GetFabricTable()
{
    auto *engine = chip::app::InteractionModelEngine::GetInstance();
    auto *exchange_manager = engine == nullptr ? nullptr : engine->GetExchangeManager();
    auto *session_manager = exchange_manager == nullptr ? nullptr : exchange_manager->GetSessionManager();
    return session_manager == nullptr ? nullptr : session_manager->GetFabricTable();
}

inline CHIP_ERROR ValidateFabricIndex(chip::FabricIndex fabric_index)
{
    chip::FabricTable *fabric_table = GetFabricTable();
    VerifyOrReturnError(fabric_table != nullptr, CHIP_ERROR_INCORRECT_STATE);
    VerifyOrReturnError(fabric_table->FindFabricWithIndex(fabric_index) != nullptr,
                        CHIP_ERROR_INVALID_FABRIC_INDEX);
    return CHIP_NO_ERROR;
}

inline CHIP_ERROR ValidateRemoteTarget(const chip::ScopedNodeId &target)
{
    VerifyOrReturnError(target.IsOperational(), CHIP_ERROR_INVALID_ARGUMENT);
    chip::FabricTable *fabric_table = GetFabricTable();
    VerifyOrReturnError(fabric_table != nullptr, CHIP_ERROR_INCORRECT_STATE);
    const chip::FabricInfo *fabric = fabric_table->FindFabricWithIndex(target.GetFabricIndex());
    VerifyOrReturnError(fabric != nullptr, CHIP_ERROR_INVALID_FABRIC_INDEX);
    VerifyOrReturnError(fabric->GetNodeId() != target.GetNodeId(), CHIP_ERROR_INVALID_ARGUMENT);
    return CHIP_NO_ERROR;
}

inline CHIP_ERROR ValidateGroupTarget(chip::GroupId group_id, chip::FabricIndex fabric_index)
{
    VerifyOrReturnError(fabric_index != chip::kUndefinedFabricIndex &&
                        group_id != chip::kUndefinedGroupId,
                        CHIP_ERROR_INVALID_ARGUMENT);
    return ValidateFabricIndex(fabric_index);
}

class EncodedElement final : public chip::app::DataModel::EncodableToTLV {
public:
    CHIP_ERROR Encode(const chip::app::DataModel::EncodableToTLV *value)
    {
        VerifyOrReturnError(mBuffer.Alloc(chip::kMaxAppMessageLen), CHIP_ERROR_NO_MEMORY);
        chip::TLV::TLVWriter writer;
        writer.Init(mBuffer.Get(), mBuffer.AllocatedSize());
        if (value != nullptr) {
            ReturnErrorOnFailure(value->EncodeTo(writer, chip::TLV::AnonymousTag()));
        } else {
            chip::TLV::TLVType type;
            ReturnErrorOnFailure(writer.StartContainer(chip::TLV::AnonymousTag(), chip::TLV::kTLVType_Structure, type));
            ReturnErrorOnFailure(writer.EndContainer(type));
        }
        ReturnErrorOnFailure(writer.Finalize());
        mSize = writer.GetLengthWritten();
        return CHIP_NO_ERROR;
    }

    void Free()
    {
        mBuffer.Free();
        mSize = 0;
    }

    CHIP_ERROR EncodeTo(chip::TLV::TLVWriter &writer, chip::TLV::Tag tag) const override
    {
        chip::TLV::TLVReader reader;
        ReturnErrorOnFailure(GetReader(reader));
        return writer.CopyElement(tag, reader);
    }

    CHIP_ERROR GetReader(chip::TLV::TLVReader &reader) const
    {
        reader.Init(mBuffer.Get(), mSize);
        return reader.Next();
    }

    chip::ByteSpan Bytes() const
    {
        return chip::ByteSpan(mBuffer.Get(), mSize);
    }

private:
    chip::Platform::ScopedMemoryBufferWithSize<uint8_t> mBuffer;
    size_t mSize = 0;
};

template <class Path>
CHIP_ERROR CopyPathList(chip::Span<const Path> paths, chip::Platform::ScopedMemoryBufferWithSize<Path> &copy)
{
    VerifyOrReturnError(paths.size() <= SIZE_MAX / sizeof(Path), CHIP_ERROR_INVALID_ARGUMENT);
    if (paths.empty()) {
        return CHIP_NO_ERROR;
    }
    VerifyOrReturnError(paths.data() != nullptr, CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(copy.Alloc(paths.size()), CHIP_ERROR_NO_MEMORY);
    for (size_t index = 0; index < paths.size(); ++index) {
        copy[index] = paths[index];
    }
    return CHIP_NO_ERROR;
}

inline CHIP_ERROR ValidateReadPaths(chip::Span<const chip::app::AttributePathParams> attribute_paths,
                                    chip::Span<const chip::app::EventPathParams> event_paths)
{
    VerifyOrReturnError(attribute_paths.size() <= SIZE_MAX / sizeof(chip::app::AttributePathParams),
                        CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(event_paths.size() <= SIZE_MAX / sizeof(chip::app::EventPathParams),
                        CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(!attribute_paths.empty() || !event_paths.empty(), CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(attribute_paths.empty() || attribute_paths.data() != nullptr, CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(event_paths.empty() || event_paths.data() != nullptr, CHIP_ERROR_INVALID_ARGUMENT);
    for (size_t index = 0; index < attribute_paths.size(); ++index) {
        VerifyOrReturnError(attribute_paths[index].IsValidAttributePath(), CHIP_ERROR_INVALID_ARGUMENT);
    }
    for (size_t index = 0; index < event_paths.size(); ++index) {
        VerifyOrReturnError(event_paths[index].IsValidEventPath(), CHIP_ERROR_INVALID_ARGUMENT);
    }
    return CHIP_NO_ERROR;
}

class OwnedReadPaths {
public:
    CHIP_ERROR Copy(chip::Span<const chip::app::AttributePathParams> attribute_paths,
                    chip::Span<const chip::app::EventPathParams> event_paths)
    {
        ReturnErrorOnFailure(CopyPathList(attribute_paths, mAttributePaths));
        return CopyPathList(event_paths, mEventPaths);
    }

    void Fill(chip::app::ReadPrepareParams &params)
    {
        params.mpAttributePathParamsList = mAttributePaths.Get();
        params.mAttributePathParamsListSize = mAttributePaths.AllocatedSize();
        params.mpEventPathParamsList = mEventPaths.Get();
        params.mEventPathParamsListSize = mEventPaths.AllocatedSize();
    }

    void Deallocate(const chip::app::ReadPrepareParams &params)
    {
        if (params.mpAttributePathParamsList == mAttributePaths.Get()) {
            mAttributePaths.Free();
        }
        if (params.mpEventPathParamsList == mEventPaths.Get()) {
            mEventPaths.Free();
        }
    }

    chip::Span<const chip::app::AttributePathParams> AttributePaths() const
    {
        return {mAttributePaths.Get(), mAttributePaths.AllocatedSize()};
    }

    chip::Span<const chip::app::EventPathParams> EventPaths() const
    {
        return {mEventPaths.Get(), mEventPaths.AllocatedSize()};
    }

    void SetEndpoint(chip::EndpointId endpoint)
    {
        for (auto &path : mAttributePaths.Span()) {
            path.mEndpointId = endpoint;
        }
        for (auto &path : mEventPaths.Span()) {
            path.mEndpointId = endpoint;
        }
    }

    void Free()
    {
        mAttributePaths.Free();
        mEventPaths.Free();
    }

private:
    chip::Platform::ScopedMemoryBufferWithSize<chip::app::AttributePathParams> mAttributePaths;
    chip::Platform::ScopedMemoryBufferWithSize<chip::app::EventPathParams> mEventPaths;
};

// Stack-lock protected; terminal OnDone() callbacks are excluded to permit destruction.
inline uint32_t callback_depth = 0;

class CallbackScope {
public:
    explicit CallbackScope(bool &active) : mActive(active), mDepth(callback_depth)
    {
        VerifyOrDie(!active);
        mActive = true;
        mDepth = callback_depth + 1;
    }
    CallbackScope(const CallbackScope &) = delete;
    CallbackScope &operator=(const CallbackScope &) = delete;

private:
    chip::ScopedChange<bool> mActive;
    chip::ScopedChange<uint32_t> mDepth;
};

class CaseConnection {
    enum class State : uint8_t {
        idle,
        starting,
        active,
        start_failed,
        finish_pending,
        cancelled,
    };

public:
    CaseConnection()
        : mConnected(OnConnected, this), mFailed(OnFailed, this)
    {
    }

    CHIP_ERROR Start(const chip::ScopedNodeId &peer)
    {
        auto *engine = chip::app::InteractionModelEngine::GetInstance();
        auto *case_session_manager = engine == nullptr ? nullptr : engine->GetCASESessionManager();
        if (case_session_manager == nullptr) {
            return CHIP_ERROR_INCORRECT_STATE;
        }
        mPendingError = CHIP_NO_ERROR;
        mState = State::starting;
        case_session_manager->FindOrEstablishSession(peer, &mConnected, &mFailed);
        if (mState == State::start_failed) {
            mState = State::cancelled;
            return mPendingError;
        }
        if (mState == State::finish_pending) {
            mState = State::active;
            Finish(mPendingError);
            return CHIP_NO_ERROR;
        }
        mState = State::active;
        return CHIP_NO_ERROR;
    }

    void Cancel()
    {
        mState = State::cancelled;
        mConnected.Cancel();
        mFailed.Cancel();
    }

protected:
    virtual ~CaseConnection() = default;
    virtual CHIP_ERROR Send(chip::Messaging::ExchangeManager &exchange_manager,
                            const chip::SessionHandle &session) = 0;
    virtual void Finish(CHIP_ERROR err) = 0;

    bool DeferFinish(CHIP_ERROR err)
    {
        if (!IsStarting()) {
            return false;
        }
        /* A synchronous CASE/send failure takes precedence over completion. */
        if (mState != State::start_failed) {
            mState = State::finish_pending;
            mPendingError = err;
        }
        return true;
    }

private:
    static void OnConnected(void *context, chip::Messaging::ExchangeManager &exchange_manager,
                            const chip::SessionHandle &session)
    {
        auto *connection = static_cast<CaseConnection *>(context);
        if (!connection->CallbacksEnabled()) {
            return;
        }
        CHIP_ERROR err = connection->Send(exchange_manager, session);
        if (err != CHIP_NO_ERROR) {
            connection->ConnectionFailed(err);
        }
    }

    static void OnFailed(void *context, const chip::ScopedNodeId &, CHIP_ERROR err)
    {
        auto *connection = static_cast<CaseConnection *>(context);
        if (connection->CallbacksEnabled()) {
            connection->ConnectionFailed(err);
        }
    }

    void ConnectionFailed(CHIP_ERROR err)
    {
        if (IsStarting()) {
            mState = State::start_failed;
            mPendingError = err;
            return;
        }
        Finish(err);
    }

    bool IsStarting() const
    {
        return mState == State::starting || mState == State::start_failed || mState == State::finish_pending;
    }

    bool CallbacksEnabled() const
    {
        return IsStarting() || mState == State::active;
    }

    chip::Callback::Callback<chip::OnDeviceConnected> mConnected;
    chip::Callback::Callback<chip::OnDeviceConnectionFailure> mFailed;
    CHIP_ERROR mPendingError = CHIP_NO_ERROR;
    State mState = State::idle;
};

} // namespace operation
} // namespace client
} // namespace esp_matter
