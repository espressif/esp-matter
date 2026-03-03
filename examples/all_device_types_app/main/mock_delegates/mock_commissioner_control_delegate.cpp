/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_commissioner_control_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace CommissionerControl {

CHIP_ERROR MockCommissionerControlDelegate::HandleCommissioningApprovalRequest(const CommissioningApprovalRequest  &request)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockCommissionerControlDelegate::ValidateCommissionNodeCommand(NodeId clientNodeId, uint64_t requestId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockCommissionerControlDelegate::GetCommissioningWindowParams(CommissioningWindowParams  &outParams)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockCommissionerControlDelegate::HandleCommissionNode(const CommissioningWindowParams  &params)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

} // namespace CommissionerControl
} // namespace Clusters
} // namespace app
} // namespace chip