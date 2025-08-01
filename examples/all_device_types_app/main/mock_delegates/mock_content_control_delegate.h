/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once
#include <app/clusters/content-control-server/content-control-delegate.h>

/*
 * Mock ContentControl Delegate Implementation
 * This file provides a mock implementation of the ContentControl::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/content-control-server/content-control-delegate.h
 */

namespace chip {
namespace app {
namespace Clusters {
namespace ContentControl {

class MockContentControlDelegate : public Delegate {
public:
    MockContentControlDelegate() = default;

    bool HandleGetEnabled() override;
    CHIP_ERROR HandleGetOnDemandRatings(app::AttributeValueEncoder  &aEncoder) override;
    chip::CharSpan HandleGetOnDemandRatingThreshold() override;
    CHIP_ERROR HandleGetScheduledContentRatings(app::AttributeValueEncoder  &aEncoder) override;
    chip::CharSpan HandleGetScheduledContentRatingThreshold() override;
    uint32_t HandleGetScreenDailyTime() override;
    uint32_t HandleGetRemainingScreenTime() override;
    bool HandleGetBlockUnrated() override;

    void HandleUpdatePIN(chip::CharSpan oldPIN, chip::CharSpan newPIN) override;
    void HandleResetPIN(CommandResponseHelper<Commands::ResetPINResponse::Type>  &helper) override;
    void HandleEnable() override;
    void HandleDisable() override;
    void HandleAddBonusTime(Optional<chip::CharSpan> PINCode, uint32_t bonusTime) override;
    void HandleSetScreenDailyTime(uint32_t screenDailyTime) override;
    void HandleBlockUnratedContent() override;
    void HandleUnblockUnratedContent() override;
    void HandleSetOnDemandRatingThreshold(chip::CharSpan rating) override;
    void HandleSetScheduledContentRatingThreshold(chip::CharSpan rating) override;

    uint32_t GetFeatureMap(chip::EndpointId endpoint) override;

private:
    const char *LOG_TAG = "content_control";
};

} // namespace ContentControl
} // namespace Clusters
} // namespace app
} // namespace chip