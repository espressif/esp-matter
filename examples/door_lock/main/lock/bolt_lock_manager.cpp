/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "bolt_lock_manager.h"

#include <esp_log.h>
#include <platform/CHIPDeviceLayer.h>
#include <system/SystemClock.h>

using chip::System::Clock::Milliseconds32;

namespace {
const char * TAG = "bolt_lock_manager";
}

CHIP_ERROR BoltLockManager::Init(State initialState, StateChangeCallback callback)
{
    VerifyOrReturnError(initialState == State::kLockingCompleted || initialState == State::kUnlockingCompleted,
                        CHIP_ERROR_INVALID_ARGUMENT);

    chip::DeviceLayer::SystemLayer().CancelTimer(ActuatorTimerHandler, this);
    mState = initialState;
    mStableState = initialState;
    mStateChangeCallback = callback;
    mOperationSource = OperationSource::kUnspecified;
    return CHIP_NO_ERROR;
}

bool BoltLockManager::Lock(OperationSource source)
{
    if (mState == State::kLockingInitiated || mState == State::kLockingCompleted) {
        return true;
    }
    return StartMovement(State::kLockingInitiated, source);
}

bool BoltLockManager::Unlock(OperationSource source)
{
    if (mState == State::kUnlockingInitiated || mState == State::kUnlockingCompleted) {
        return true;
    }
    return StartMovement(State::kUnlockingInitiated, source);
}

bool BoltLockManager::StartMovement(State movementState, OperationSource source)
{
    chip::DeviceLayer::SystemLayer().CancelTimer(ActuatorTimerHandler, this);
    CHIP_ERROR err = chip::DeviceLayer::SystemLayer().StartTimer(Milliseconds32(kActuatorMovementTimeMs),
                                                                 ActuatorTimerHandler, this);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to start actuator timer: %" CHIP_ERROR_FORMAT, err.Format());
        SetState(mStableState, source);
        return false;
    }

    mOperationSource = source;
    SetState(movementState, source);
    return true;
}

void BoltLockManager::ActuatorTimerHandler(chip::System::Layer * systemLayer, void * context)
{
    auto * lock = static_cast<BoltLockManager *>(context);
    VerifyOrReturn(lock != nullptr);

    switch (lock->mState) {
    case State::kLockingInitiated:
        lock->mStableState = State::kLockingCompleted;
        lock->SetState(State::kLockingCompleted, lock->mOperationSource);
        break;
    case State::kUnlockingInitiated:
        lock->mStableState = State::kUnlockingCompleted;
        lock->SetState(State::kUnlockingCompleted, lock->mOperationSource);
        break;
    case State::kLockingCompleted:
    case State::kUnlockingCompleted:
        break;
    }
}

void BoltLockManager::SetState(State state, OperationSource source)
{
    mState = state;
    if (mStateChangeCallback != nullptr) {
        mStateChangeCallback(state, source);
    }
}
