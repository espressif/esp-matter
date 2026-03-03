/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_application_basic_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ApplicationBasic {

CHIP_ERROR MockApplicationBasicDelegate::HandleGetVendorName(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeNull();
}

uint16_t MockApplicationBasicDelegate::HandleGetVendorId()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

CHIP_ERROR MockApplicationBasicDelegate::HandleGetApplicationName(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeNull();
}

uint16_t MockApplicationBasicDelegate::HandleGetProductId()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

CHIP_ERROR MockApplicationBasicDelegate::HandleGetApplicationVersion(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeNull();
}

CHIP_ERROR MockApplicationBasicDelegate::HandleGetAllowedVendorList(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeEmptyList();
}

std::list<uint16_t> MockApplicationBasicDelegate::GetAllowedVendorList()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return std::list<uint16_t>();
}

} // namespace ApplicationBasic
} // namespace Clusters
} // namespace app
} // namespace chip