/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app-common/zap-generated/cluster-objects.h>

#include <app/clusters/audio-output-server/audio-output-delegate.h>

/*
 * Mock AudioOutput Delegate Implementation
 * This file provides a mock implementation of the AudioOutput::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/audio-output-server/audio-output-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/audio-output/AudioOutputManager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/audio-output/AudioOutputManager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace AudioOutput {

class MockAudioOutputDelegate : public Delegate {
public:
    MockAudioOutputDelegate() = default;

    uint8_t HandleGetCurrentOutput() override;
    CHIP_ERROR HandleGetOutputList(app::AttributeValueEncoder  &aEncoder) override;
    bool HandleRenameOutput(const uint8_t  &index, const chip::CharSpan  &name) override;
    bool HandleSelectOutput(const uint8_t  &index) override;

private:
    const char *LOG_TAG = "audio_output";
};

} // namespace AudioOutput
} // namespace Clusters
} // namespace app
} // namespace chip