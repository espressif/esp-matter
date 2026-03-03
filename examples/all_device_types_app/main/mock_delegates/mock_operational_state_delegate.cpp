/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_operational_state_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace OperationalState {

app::DataModel::Nullable<uint32_t> MockOperationalStateDelegate::GetCountdownTime()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return app::DataModel::Nullable<uint32_t>();
}

CHIP_ERROR MockOperationalStateDelegate::GetOperationalStateAtIndex(size_t index, GenericOperationalState  &operationalState)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_FOUND;
}

CHIP_ERROR MockOperationalStateDelegate::GetOperationalPhaseAtIndex(size_t index, MutableCharSpan  &operationalPhase)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_FOUND;
}

void MockOperationalStateDelegate::HandlePauseStateCallback(GenericOperationalError  &err)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    err.Set(to_underlying(ErrorStateEnum::kNoError));
    return;
}

void MockOperationalStateDelegate::HandleResumeStateCallback(GenericOperationalError  &err)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    err.Set(to_underlying(ErrorStateEnum::kNoError));
    return;
}

void MockOperationalStateDelegate::HandleStartStateCallback(GenericOperationalError  &err)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    err.Set(to_underlying(ErrorStateEnum::kNoError));
    return;
}

void MockOperationalStateDelegate::HandleStopStateCallback(GenericOperationalError  &err)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    err.Set(to_underlying(ErrorStateEnum::kNoError));
    return;
}

} // namespace OperationalState
} // namespace Clusters
} // namespace app
} // namespace chip