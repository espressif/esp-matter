/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include "door_lock_storage.h"

namespace DoorLockAuthorization {

/** Check actuator, source and current mode, including PIN-less remote commands. */
bool ModeAllows(chip::EndpointId endpoint, OperationSourceEnum source, OperationErrorEnum &error,
                bool programmingUser = false);

/** Resolve current user/credential relationships and policy; authentication alone is insufficient. */
bool Authorize(const DoorLockStorage::Match &match, OperationSourceEnum source, OperationErrorEnum &error);

/** Emit an unlock-error event with known attribution, without changing actuator state. */
void ReportUnlockFailure(const DoorLockStorage::Match &match, OperationSourceEnum source, OperationErrorEnum error);

} // namespace DoorLockAuthorization
