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

// Identifies the credential (if any) that authorized a lock/unlock
// operation, so it can be reported on the emitted LockOperation event.
// Default-constructed (present == false) means "no credential", e.g. a
// button press or an unconditional remote/auto operation.
//
// Defined at file scope (rather than nested in BoltLockManager) because a
// nested class's default member initializers cannot be used -- e.g. via an
// implicit default constructor in a default argument -- until the enclosing
// class itself is complete, which member functions declared earlier in the
// same class body are not.
struct BoltLockCredentialMatch {
    bool present = false;
    uint16_t userIndex = 0;
    uint16_t credentialIndex = 0;
    chip::app::Clusters::DoorLock::CredentialTypeEnum credentialType =
        chip::app::Clusters::DoorLock::CredentialTypeEnum::kProgrammingPIN;
    // Fabric/node that requested a remote operation. DoorLockServer reports
    // these on the emitted LockOperation event and otherwise logs a warning
    // for OperationSourceEnum::kRemote if they're left null. Not applicable
    // to local sources (button, auto-relock, RFID, keypad PIN) -- default
    // Nullable state (null) is correct for those.
    chip::app::DataModel::Nullable<chip::FabricIndex> fabricIndex;
    chip::app::DataModel::Nullable<chip::NodeId> nodeId;
};

class BoltLockManager final {
public:
    enum class State : uint8_t {
        kLockingInitiated = 0,
        kLockingCompleted,
        kUnlockingInitiated,
        kUnlockingCompleted,
    };

    using OperationSource = chip::app::Clusters::DoorLock::OperationSourceEnum;
    using CredentialType = chip::app::Clusters::DoorLock::CredentialTypeEnum;
    using CredentialMatch = BoltLockCredentialMatch;

    using StateChangeCallback = void (*)(State state, OperationSource source, const CredentialMatch  &credential);

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
    bool Lock(OperationSource source, const CredentialMatch  &credential = CredentialMatch());
    bool Unlock(OperationSource source, const CredentialMatch  &credential = CredentialMatch());

private:
    BoltLockManager() = default;

    bool StartMovement(State movementState, OperationSource source, const CredentialMatch  &credential);
    void SetState(State state, OperationSource source, const CredentialMatch  &credential);
    static void ActuatorTimerHandler(chip::System::Layer * systemLayer, void * context);

    State mState = State::kLockingCompleted;
    State mStableState = State::kLockingCompleted;
    StateChangeCallback mStateChangeCallback = nullptr;
    OperationSource mOperationSource = OperationSource::kUnspecified;
    CredentialMatch mCredential;
};
