/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_service_area_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ServiceArea {

CHIP_ERROR MockServiceAreaDelegate::Init()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

bool MockServiceAreaDelegate::IsSetSelectedAreasAllowed(MutableCharSpan  &statusText)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

bool MockServiceAreaDelegate::IsValidSelectAreasSet(const Span<const uint32_t>  &selectedAreas, SelectAreasStatus  &locationStatus,
                                                    MutableCharSpan  &statusText)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    locationStatus = SelectAreasStatus::kSuccess;
    return true;
}

bool MockServiceAreaDelegate::HandleSkipArea(uint32_t skippedArea, MutableCharSpan  &skipStatusText)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

} // namespace ServiceArea
} // namespace Clusters
} // namespace app
} // namespace chip