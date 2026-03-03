/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/thermostat-server/thermostat-delegate.h>

/*
 * Mock Thermostat Delegate Implementation
 * This file provides a mock implementation of the Thermostat::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/thermostat-server/thermostat-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/thermostat/thermostat-common/include/thermostat-delegate-impl.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/thermostat/thermostat-common/src/thermostat-delegate-impl.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace Thermostat {

class MockThermostatDelegate : public Delegate {
public:
    MockThermostatDelegate() = default;

    std::optional<System::Clock::Milliseconds16> GetMaxAtomicWriteTimeout(chip::AttributeId attributeId) override;
    CHIP_ERROR GetPresetTypeAtIndex(size_t index, Structs::PresetTypeStruct::Type  &presetType) override;
    uint8_t GetNumberOfPresets() override;
    CHIP_ERROR GetPresetAtIndex(size_t index, PresetStructWithOwnedMembers  &preset) override;
    CHIP_ERROR GetActivePresetHandle(DataModel::Nullable<MutableByteSpan>  &activePresetHandle) override;
    CHIP_ERROR SetActivePresetHandle(const DataModel::Nullable<ByteSpan>  &newActivePresetHandle) override;
    void InitializePendingPresets() override;
    CHIP_ERROR AppendToPendingPresetList(const PresetStructWithOwnedMembers  &preset) override;
    CHIP_ERROR GetPendingPresetAtIndex(size_t index, PresetStructWithOwnedMembers  &preset) override;
    CHIP_ERROR CommitPendingPresets() override;
    void ClearPendingPresetList() override;
    uint8_t GetMaxThermostatSuggestions() override;
    uint8_t GetNumberOfThermostatSuggestions() override;
    CHIP_ERROR GetThermostatSuggestionAtIndex(size_t index,
                                              ThermostatSuggestionStructWithOwnedMembers  &thermostatSuggestion) override;
    void GetCurrentThermostatSuggestion(
        DataModel::Nullable<ThermostatSuggestionStructWithOwnedMembers>  &currentThermostatSuggestion) override;
    DataModel::Nullable<ThermostatSuggestionNotFollowingReasonBitmap> GetThermostatSuggestionNotFollowingReason() override;
    CHIP_ERROR
    AppendToThermostatSuggestionsList(const Structs::ThermostatSuggestionStruct::Type  &thermostatSuggestion) override;
    CHIP_ERROR RemoveFromThermostatSuggestionsList(size_t indexToRemove) override;
    CHIP_ERROR GetUniqueID(uint8_t  &uniqueID) override;
    CHIP_ERROR ReEvaluateCurrentSuggestion() override;
    CHIP_ERROR GetScheduleTypeAtIndex(size_t index, Structs::ScheduleTypeStruct::Type  &scheduleType) override;

private:
    const char *LOG_TAG = "thermostat";
};

} // namespace Thermostat
} // namespace Clusters
} // namespace app
} // namespace chip