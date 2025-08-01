/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_closure_control_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ClosureControl {

Protocols::InteractionModel::Status MockClosureControlDelegate::HandleStopCommand()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockClosureControlDelegate::HandleMoveToCommand(const Optional<TargetPositionEnum>  &position,
                                                                                    const Optional<bool>  &latch,
                                                                                    const Optional<Globals::ThreeLevelAutoEnum>  &speed)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockClosureControlDelegate::HandleCalibrateCommand()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

bool MockClosureControlDelegate::IsReadyToMove()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

ElapsedS MockClosureControlDelegate::GetCalibrationCountdownTime()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return ElapsedS(0);
}

ElapsedS MockClosureControlDelegate::GetMovingCountdownTime()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return ElapsedS(0);
}

ElapsedS MockClosureControlDelegate::GetWaitingForMotionCountdownTime()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return ElapsedS(0);
}

} // namespace ClosureControl
} // namespace Clusters
} // namespace app
} // namespace chip