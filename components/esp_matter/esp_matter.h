// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

/*
This is a common include file which includes all the other esp_matter component files which would be required by the
application.
*/
#include <sdkconfig.h>
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
#include <esp_matter_attribute.h>
#include <esp_matter_cluster.h>
#include <esp_matter_command.h>
#include <esp_matter_endpoint.h>
#include <esp_matter_event.h>
#include <esp_matter_feature.h>
#include <esp_matter_data_model.h>
#endif // CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
#include <app/server/Dnssd.h>
#include <esp_matter_attribute_utils.h>
#include <esp_matter_identify.h>
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
#include <esp_matter_client.h>
#include <esp_matter_core.h>
#include <platform/CHIPDeviceEvent.h>
#include <platform/CHIPDeviceLayer.h>

namespace chip {
namespace DeviceLayer {
namespace DeviceEventType {

/**
 * Enumerates platform-specific event types that are visible to the application.
 */
enum
{
    /** Signals that Commissioning session has started */
    kCommissioningSessionStarted  = kRange_PublicPlatformSpecific + 0x1000,
    /** Signals that Commissioning session has stopped */
    kCommissioningSessionStopped,
    /** Signals that Commissioning window is now opend */
    kCommissioningWindowOpened,
    /** Signals that Commissioning window is now closed */
    kCommissioningWindowClosed,
    /** Signals that a fabric is about to be deleted. This allows actions to be taken that need the
    fabric to still be around before we delete it */
    kFabricWillBeRemoved,
    /** Signals that a fabric is deleted */
    kFabricRemoved,
    /** Signals that a fabric in Fabric Table is persisted to storage, by CommitPendingFabricData */
    kFabricCommitted,
    /** Signals that operational credentials are changed, which may not be persistent.
    Can be used to affect what is needed for UpdateNOC prior to commit */
    kFabricUpdated,
};

} // DeviceEventType
} // DeviceLayer
} // chip
