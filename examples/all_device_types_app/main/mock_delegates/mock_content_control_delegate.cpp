/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_content_control_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ContentControl {

bool MockContentControlDelegate::HandleGetEnabled()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return false;
}

CHIP_ERROR MockContentControlDelegate::HandleGetOnDemandRatings(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeEmptyList();
}

chip::CharSpan MockContentControlDelegate::HandleGetOnDemandRatingThreshold()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return chip::CharSpan();
}

CHIP_ERROR MockContentControlDelegate::HandleGetScheduledContentRatings(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeEmptyList();
}

chip::CharSpan MockContentControlDelegate::HandleGetScheduledContentRatingThreshold()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return chip::CharSpan();
}

uint32_t MockContentControlDelegate::HandleGetScreenDailyTime()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint32_t MockContentControlDelegate::HandleGetRemainingScreenTime()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

bool MockContentControlDelegate::HandleGetBlockUnrated()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return false;
}

void MockContentControlDelegate::HandleUpdatePIN(chip::CharSpan oldPIN, chip::CharSpan newPIN)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

void MockContentControlDelegate::HandleResetPIN(CommandResponseHelper<Commands::ResetPINResponse::Type>  &helper)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::ResetPINResponse::Type response;
    helper.Success(response);
}

void MockContentControlDelegate::HandleEnable()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

void MockContentControlDelegate::HandleDisable()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

void MockContentControlDelegate::HandleAddBonusTime(Optional<chip::CharSpan> PINCode, uint32_t bonusTime)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

void MockContentControlDelegate::HandleSetScreenDailyTime(uint32_t screenDailyTime)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

void MockContentControlDelegate::HandleBlockUnratedContent()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

void MockContentControlDelegate::HandleUnblockUnratedContent()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

void MockContentControlDelegate::HandleSetOnDemandRatingThreshold(chip::CharSpan rating)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

void MockContentControlDelegate::HandleSetScheduledContentRatingThreshold(chip::CharSpan rating)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

uint32_t MockContentControlDelegate::GetFeatureMap(chip::EndpointId endpoint)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

} // namespace ContentControl
} // namespace Clusters
} // namespace app
} // namespace chip