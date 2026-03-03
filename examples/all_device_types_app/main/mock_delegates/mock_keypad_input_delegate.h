/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app-common/zap-generated/cluster-objects.h>

#include <app/clusters/keypad-input-server/keypad-input-delegate.h>

/*
 * Mock KeypadInput Delegate Implementation
 * This file provides a mock implementation of the KeypadInput::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/keypad-input-server/keypad-input-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/keypad-input/KeypadInputManager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/keypad-input/KeypadInputManager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace KeypadInput {

class MockKeypadInputDelegate : public Delegate {
public:
    MockKeypadInputDelegate() = default;

    void HandleSendKey(CommandResponseHelper<Commands::SendKeyResponse::Type>  &helper, const CECKeyCodeEnum  &keyCode) override;
    uint32_t GetFeatureMap(chip::EndpointId endpoint) override;

private:
    const char *LOG_TAG = "keypad_input";
};

} // namespace KeypadInput
} // namespace Clusters
} // namespace app
} // namespace chip