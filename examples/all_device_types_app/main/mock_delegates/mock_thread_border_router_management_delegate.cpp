/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_thread_border_router_management_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ThreadBorderRouterManagement {

CHIP_ERROR MockThreadBorderRouterManagementDelegate::Init(AttributeChangeCallback * attributeChangeCallback)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

bool MockThreadBorderRouterManagementDelegate::GetPanChangeSupported()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return false;
}

void MockThreadBorderRouterManagementDelegate::GetBorderRouterName(MutableCharSpan  &borderRouterName)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    borderRouterName.reduce_size(0);
    return;
}

CHIP_ERROR MockThreadBorderRouterManagementDelegate::GetBorderAgentId(MutableByteSpan  &borderAgentId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

uint16_t MockThreadBorderRouterManagementDelegate::GetThreadVersion()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

bool MockThreadBorderRouterManagementDelegate::GetInterfaceEnabled()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return false;
}

CHIP_ERROR MockThreadBorderRouterManagementDelegate::GetDataset(Thread::OperationalDataset  &dataset, DatasetType type)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_FOUND;
}

void MockThreadBorderRouterManagementDelegate::SetActiveDataset(const Thread::OperationalDataset  &activeDataset, uint32_t sequenceNum,
                                                                ActivateDatasetCallback * callback)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

CHIP_ERROR MockThreadBorderRouterManagementDelegate::CommitActiveDataset()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockThreadBorderRouterManagementDelegate::RevertActiveDataset()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockThreadBorderRouterManagementDelegate::SetPendingDataset(const Thread::OperationalDataset  &pendingDataset)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

} // namespace ThreadBorderRouterManagement
} // namespace Clusters
} // namespace app
} // namespace chip
