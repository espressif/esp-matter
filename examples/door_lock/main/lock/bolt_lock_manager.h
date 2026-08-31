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
    using StateChangeCallback = void (*)(State state, OperationSource source);

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
    bool Lock(OperationSource source);
    bool Unlock(OperationSource source);

private:
    BoltLockManager() = default;

    bool StartMovement(State movementState, OperationSource source);
    void SetState(State state, OperationSource source);
    static void ActuatorTimerHandler(chip::System::Layer * systemLayer, void * context);

    State mState = State::kLockingCompleted;
    State mStableState = State::kLockingCompleted;
    StateChangeCallback mStateChangeCallback = nullptr;
    OperationSource mOperationSource = OperationSource::kUnspecified;
};
