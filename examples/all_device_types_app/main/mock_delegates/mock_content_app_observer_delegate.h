/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/content-app-observer/content-app-observer-delegate.h>

/*
 * Mock ContentAppObserver Delegate Implementation
 * This file provides a mock implementation of the ContentAppObserver::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/content-app-observer/content-app-observer-delegate.h
 */

namespace chip {
namespace app {
namespace Clusters {
namespace ContentAppObserver {

class MockContentAppObserverDelegate : public Delegate {
public:
    MockContentAppObserverDelegate() = default;
    virtual ~MockContentAppObserverDelegate() = default;

    // Content app observer command handlers
    void HandleContentAppMessage(CommandResponseHelper<Commands::ContentAppMessageResponse::Type>  &helper,
                                 const chip::Optional<chip::CharSpan>  &data, const chip::CharSpan  &encodingHint) override;

private:
    static constexpr const char * LOG_TAG = "MockContentAppObserverDelegate";
};

} // namespace ContentAppObserver
} // namespace Clusters
} // namespace app
} // namespace chip