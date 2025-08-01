/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/zone-management-server/zone-management-server.h>
#include <protocols/interaction_model/StatusCode.h>

/*
 * Mock ZoneManagement Delegate Implementation
 * This file provides a mock implementation of the ZoneManagement::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/zone-management-server/zone-management-server.h
 */

namespace chip {
namespace app {
namespace Clusters {
namespace ZoneManagement {

class MockZoneManagementDelegate : public Delegate {
public:
    MockZoneManagementDelegate() : Delegate() {}
    virtual ~MockZoneManagementDelegate() = default;

    // Zone management command handlers
    Protocols::InteractionModel::Status CreateTwoDCartesianZone(const TwoDCartesianZoneStorage  &zone,
                                                                uint16_t  &outZoneID) override;
    Protocols::InteractionModel::Status UpdateTwoDCartesianZone(uint16_t zoneID, const TwoDCartesianZoneStorage  &zone) override;
    Protocols::InteractionModel::Status RemoveZone(uint16_t zoneID) override;
    Protocols::InteractionModel::Status CreateTrigger(const ZoneTriggerControlStruct  &zoneTrigger) override;
    Protocols::InteractionModel::Status UpdateTrigger(const ZoneTriggerControlStruct  &zoneTrigger) override;
    Protocols::InteractionModel::Status RemoveTrigger(uint16_t zoneID) override;

    // Callback handlers
    void OnAttributeChanged(AttributeId attributeId) override;
    CHIP_ERROR PersistentAttributesLoadedCallback() override;

    // Load handlers
    CHIP_ERROR LoadZones(std::vector<ZoneInformationStorage>  &aZones) override;
    CHIP_ERROR LoadTriggers(std::vector<ZoneTriggerControlStruct>  &aTriggers) override;

private:
    static constexpr const char * LOG_TAG = "MockZoneManagementDelegate";
};

} // namespace ZoneManagement
} // namespace Clusters
} // namespace app
} // namespace chip
