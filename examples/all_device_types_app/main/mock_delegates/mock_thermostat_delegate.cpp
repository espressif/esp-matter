/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_thermostat_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace Thermostat {

std::optional<System::Clock::Milliseconds16> MockThermostatDelegate::GetMaxAtomicWriteTimeout(chip::AttributeId attributeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return std::nullopt;
}

CHIP_ERROR MockThermostatDelegate::GetPresetTypeAtIndex(size_t index, Structs::PresetTypeStruct::Type  &presetType)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

uint8_t MockThermostatDelegate::GetNumberOfPresets()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

CHIP_ERROR MockThermostatDelegate::GetPresetAtIndex(size_t index, PresetStructWithOwnedMembers  &preset)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

CHIP_ERROR MockThermostatDelegate::GetActivePresetHandle(DataModel::Nullable<MutableByteSpan>  &activePresetHandle)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    activePresetHandle.SetNull();
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockThermostatDelegate::SetActivePresetHandle(const DataModel::Nullable<ByteSpan>  &newActivePresetHandle)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

void MockThermostatDelegate::InitializePendingPresets()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

CHIP_ERROR MockThermostatDelegate::AppendToPendingPresetList(const PresetStructWithOwnedMembers  &preset)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockThermostatDelegate::GetPendingPresetAtIndex(size_t index, PresetStructWithOwnedMembers  &preset)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

CHIP_ERROR MockThermostatDelegate::CommitPendingPresets()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

void MockThermostatDelegate::ClearPendingPresetList()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

uint8_t MockThermostatDelegate::GetMaxThermostatSuggestions()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint8_t MockThermostatDelegate::GetNumberOfThermostatSuggestions()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

CHIP_ERROR MockThermostatDelegate::GetThermostatSuggestionAtIndex(
    size_t index, ThermostatSuggestionStructWithOwnedMembers  &thermostatSuggestion)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

void MockThermostatDelegate::GetCurrentThermostatSuggestion(
    DataModel::Nullable<ThermostatSuggestionStructWithOwnedMembers>  &currentThermostatSuggestion)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    currentThermostatSuggestion.SetNull();
    return;
}

DataModel::Nullable<ThermostatSuggestionNotFollowingReasonBitmap>
MockThermostatDelegate::GetThermostatSuggestionNotFollowingReason()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<ThermostatSuggestionNotFollowingReasonBitmap>();
}

CHIP_ERROR
MockThermostatDelegate::AppendToThermostatSuggestionsList(const Structs::ThermostatSuggestionStruct::Type  &thermostatSuggestion)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockThermostatDelegate::RemoveFromThermostatSuggestionsList(size_t indexToRemove)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockThermostatDelegate::GetUniqueID(uint8_t  &uniqueID)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    uniqueID = 0;
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockThermostatDelegate::ReEvaluateCurrentSuggestion()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockThermostatDelegate::GetScheduleTypeAtIndex(size_t index, Structs::ScheduleTypeStruct::Type  &scheduleType)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

} // namespace Thermostat
} // namespace Clusters
} // namespace app
} // namespace chip