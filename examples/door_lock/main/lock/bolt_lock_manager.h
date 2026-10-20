/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/door-lock-server/door-lock-server.h>
#include <lib/core/CHIPError.h>
#include <system/SystemLayer.h>

#include <stdint.h>

class BoltLockManager final {
public:
    enum class State : uint8_t {
        kLockingInitiated = 0,
        kLockingCompleted,
        kUnlockingInitiated,
        kUnlockingCompleted,
    };

    using OperationSource = chip::app::Clusters::DoorLock::OperationSourceEnum;
    using StateChangeCallback = void (*)(State state, OperationSource source, bool success);

    static constexpr uint32_t kActuatorMovementTimeMs = 2000;

    static BoltLockManager  &Instance()
    {
        static BoltLockManager manager;
        return manager;
    }

    CHIP_ERROR Init(State initialState, StateChangeCallback callback);
    State GetState() const
    {
        return mState;
    }
    bool IsLocked() const
    {
        return mState == State::kLockingCompleted;
    }

    /** Copied request identity retained for state-change events; user=0 means no credential attribution. */
    struct Attribution {
        uint16_t user = 0;
        CredentialStruct credential = {};
        chip::app::DataModel::Nullable<chip::FabricIndex> fabric;
        chip::app::DataModel::Nullable<chip::NodeId> node;
    };

    /** Borrow state-change attribution; backing may change on the next Lock/Unlock request. */
    const Attribution &GetAttribution() const
    {
        return mAttribution;
    }

    /** Request locking without credential attribution; return synchronous acceptance, not completion. */
    bool Lock(OperationSource source);
    /** Request locking with copied attribution; timer failures are reported through the callback. */
    bool Lock(OperationSource source, const Attribution &attribution);
    /** Request unlocking without credential attribution; callers own authorization. */
    bool Unlock(OperationSource source);
    /** Request unlocking with copied attribution; callers own authorization. */
    bool Unlock(OperationSource source, const Attribution &attribution);

private:
    BoltLockManager() = default;

    bool StartMovement(State movementState, OperationSource source, const Attribution &attribution);
    void SetState(State state, OperationSource source, bool success = true);
    static void ActuatorTimerHandler(chip::System::Layer * systemLayer, void * context);

    Attribution mAttribution;
    State mState = State::kLockingCompleted;
    State mStableState = State::kLockingCompleted;
    StateChangeCallback mStateChangeCallback = nullptr;
    OperationSource mOperationSource = OperationSource::kUnspecified;
};
