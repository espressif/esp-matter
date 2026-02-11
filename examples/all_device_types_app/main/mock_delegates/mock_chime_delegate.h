/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app-common/zap-generated/cluster-objects.h>

#include <app/clusters/chime-server/chime-server.h>

/*
 * Mock Chime Delegate Implementation
 * This file provides a mock implementation of the Chime::Delegate interface
 * that returns success for all methods.
*/

namespace chip {
namespace app {
namespace Clusters {
namespace Chime {

class MockChimeDelegate : public ChimeDelegate {
public:
    MockChimeDelegate() = default;
    ~MockChimeDelegate() override;

    CHIP_ERROR GetChimeSoundByIndex(uint8_t chimeIndex, uint8_t  &chimeID, MutableCharSpan  &name) override;
    CHIP_ERROR GetChimeIDByIndex(uint8_t chimeIndex, uint8_t  &chimeID) override;
    Protocols::InteractionModel::Status PlayChimeSound() override;

private:
    const char *LOG_TAG = "chime";
};

} // namespace Chime
} // namespace Clusters
} // namespace app
} // namespace chip
