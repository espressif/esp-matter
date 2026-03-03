/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_power_topology_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace PowerTopology {

CHIP_ERROR MockPowerTopologyDelegate::GetAvailableEndpointAtIndex(size_t index, EndpointId  &endpointId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

CHIP_ERROR MockPowerTopologyDelegate::GetActiveEndpointAtIndex(size_t index, EndpointId  &endpointId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

} // namespace PowerTopology
} // namespace Clusters
} // namespace app
} // namespace chip