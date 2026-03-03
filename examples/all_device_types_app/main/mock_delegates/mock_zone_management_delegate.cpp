/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_zone_management_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ZoneManagement {

Protocols::InteractionModel::Status MockZoneManagementDelegate::CreateTwoDCartesianZone(const TwoDCartesianZoneStorage  &zone,
                                                                                        uint16_t  &outZoneID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    outZoneID = 0;
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockZoneManagementDelegate::UpdateTwoDCartesianZone(uint16_t zoneID, const TwoDCartesianZoneStorage  &zone)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockZoneManagementDelegate::RemoveZone(uint16_t zoneID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockZoneManagementDelegate::CreateTrigger(const ZoneTriggerControlStruct  &zoneTrigger)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockZoneManagementDelegate::UpdateTrigger(const ZoneTriggerControlStruct  &zoneTrigger)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockZoneManagementDelegate::RemoveTrigger(uint16_t zoneID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

void MockZoneManagementDelegate::OnAttributeChanged(AttributeId attributeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

CHIP_ERROR MockZoneManagementDelegate::PersistentAttributesLoadedCallback()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockZoneManagementDelegate::LoadZones(std::vector<ZoneInformationStorage>  &aZones)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    aZones.clear();
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockZoneManagementDelegate::LoadTriggers(std::vector<ZoneTriggerControlStruct>  &aTriggers)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    aTriggers.clear();
    return CHIP_NO_ERROR;
}

} // namespace ZoneManagement
} // namespace Clusters
} // namespace app
} // namespace chip
