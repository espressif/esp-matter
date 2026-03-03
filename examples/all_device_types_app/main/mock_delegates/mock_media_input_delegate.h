/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/media-input-server/media-input-delegate.h>

/*
 * Mock MediaInput Delegate Implementation
 * This file provides a mock implementation of the MediaInput::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/media-input-server/media-input-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/media-input/MediaInputManager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/media-input/MediaInputManager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace MediaInput {

class MockMediaInputDelegate : public Delegate {
public:
    MockMediaInputDelegate() = default;

    CHIP_ERROR HandleGetInputList(app::AttributeValueEncoder  &aEncoder) override;
    uint8_t HandleGetCurrentInput() override;
    bool HandleSelectInput(const uint8_t index) override;
    bool HandleShowInputStatus() override;
    bool HandleHideInputStatus() override;
    bool HandleRenameInput(const uint8_t index, const chip::CharSpan  &name) override;

private:
    const char *LOG_TAG = "media_input";
};

} // namespace MediaInput
} // namespace Clusters
} // namespace app
} // namespace chip