/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_closure_dimension_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ClosureDimension {

Protocols::InteractionModel::Status MockClosureDimensionDelegate::HandleSetTarget(const Optional<Percent100ths>  &position,
                                                                                  const Optional<bool>  &latch,
                                                                                  const Optional<Globals::ThreeLevelAutoEnum>  &speed)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockClosureDimensionDelegate::HandleStep(const StepDirectionEnum  &direction,
                                                                             const uint16_t  &numberOfSteps,
                                                                             const Optional<Globals::ThreeLevelAutoEnum>  &speed)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

} // namespace ClosureDimension
} // namespace Clusters
} // namespace app
} // namespace chip
