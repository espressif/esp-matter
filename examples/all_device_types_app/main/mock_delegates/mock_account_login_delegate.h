/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app-common/zap-generated/cluster-objects.h>

#include <app/clusters/account-login-server/account-login-delegate.h>

/*
 * Mock AccountLogin Delegate Implementation
 * This file provides a mock implementation of the AccountLogin::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/account-login-server/account-login-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/master/examples/tv-app/android/include/account-login/AccountLoginManager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/master/examples/tv-app/android/include/account-login/AccountLoginManager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace AccountLogin {

class MockAccountLoginDelegate : public Delegate {
public:
    MockAccountLoginDelegate() = default;

    void SetSetupPin(char * setupPin) override;
    bool HandleLogin(const chip::CharSpan  &tempAccountIdentifierString, const chip::CharSpan  &setupPinString,
                     const chip::Optional<NodeId>  &nodeId) override;
    bool HandleLogout(const chip::Optional<NodeId>  &nodeId) override;
    void HandleGetSetupPin(CommandResponseHelper<Commands::GetSetupPINResponse::Type>  &helper,
                           const chip::CharSpan  &tempAccountIdentifierString) override;
    void GetSetupPin(char * setupPin, size_t setupPinSize, const chip::CharSpan  &tempAccountIdentifierString) override;
    uint16_t GetClusterRevision(chip::EndpointId endpoint) override;

private:
    const char *LOG_TAG = "account_login";
};

} // namespace AccountLogin
} // namespace Clusters
} // namespace app
} // namespace chip
