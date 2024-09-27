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

#include <esp_matter_attribute.h>
#include <esp_matter_attribute_utils.h>
#include <esp_matter_client.h>
#include <esp_matter_cluster.h>
#include <esp_matter_command.h>
#include <esp_matter_core.h>
#include <esp_matter_endpoint.h>
#include <esp_matter_event.h>
#include <esp_matter_feature.h>
#include <esp_matter_identify.h>

#include <app/util/att-storage.h>
#include <app/server/Dnssd.h>
#include <platform/CHIPDeviceEvent.h>
#include <platform/CHIPDeviceLayer.h>

namespace esp_matter {

/** Endpoint flags */
typedef enum endpoint_flags {
    /** No specific flags */
    ENDPOINT_FLAG_NONE = 0x00,
    /** The endpoint can be destroyed using `endpoint::destroy()` */
    ENDPOINT_FLAG_DESTROYABLE = 0x01,
    /** The endpoint is a bridged node */
    ENDPOINT_FLAG_BRIDGE = 0x02,
} endpoint_flags_t;

/** Cluster flags */
typedef enum cluster_flags {
    /** No specific flags */
    CLUSTER_FLAG_NONE = 0x00,
    /** The cluster has an init function (function_flag) */
    CLUSTER_FLAG_INIT_FUNCTION = CLUSTER_MASK_INIT_FUNCTION, /* 0x01 */
    /** The cluster has an attribute changed function (function_flag) */
    CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION = CLUSTER_MASK_ATTRIBUTE_CHANGED_FUNCTION, /* 0x02 */
    /** The cluster has a shutdown function (function_flag) */
    CLUSTER_FLAG_SHUTDOWN_FUNCTION = CLUSTER_MASK_SHUTDOWN_FUNCTION, /* 0x10 */
    /** The cluster has a pre attribute changed function (function_flag) */
    CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION = CLUSTER_MASK_PRE_ATTRIBUTE_CHANGED_FUNCTION, /* 0x20 */
    /** The cluster is a server cluster */
    CLUSTER_FLAG_SERVER = CLUSTER_MASK_SERVER, /* 0x40 */
    /** The cluster is a client cluster */
    CLUSTER_FLAG_CLIENT = CLUSTER_MASK_CLIENT, /* 0x80 */
} cluster_flags_t;

/** Attribute flags */
typedef enum attribute_flags {
    /** No specific flags */
    ATTRIBUTE_FLAG_NONE = 0x00,
    /** The attribute is writable and can be directly changed by clients */
    ATTRIBUTE_FLAG_WRITABLE = ATTRIBUTE_MASK_WRITABLE, /* 0x01 */
    /** The attribute is non volatile and its value will be stored in NVS */
    ATTRIBUTE_FLAG_NONVOLATILE = ATTRIBUTE_MASK_NONVOLATILE, /* 0x02 */
    /** The attribute has bounds */
    ATTRIBUTE_FLAG_MIN_MAX = ATTRIBUTE_MASK_MIN_MAX, /* 0x04 */
    ATTRIBUTE_FLAG_MUST_USE_TIMED_WRITE = ATTRIBUTE_MASK_MUST_USE_TIMED_WRITE, /* 0x08 */
    /** The attribute uses external storage for its value. If attributes
    have this flag enabled, as all of them are stored in the ESP Matter database. */
    ATTRIBUTE_FLAG_EXTERNAL_STORAGE = ATTRIBUTE_MASK_EXTERNAL_STORAGE, /* 0x10 */
    ATTRIBUTE_FLAG_SINGLETON = ATTRIBUTE_MASK_SINGLETON, /* 0x20 */
    ATTRIBUTE_FLAG_NULLABLE = ATTRIBUTE_MASK_NULLABLE, /* 0x40 */
    /** The attribute read and write are overridden. The attribute value will be fetched from and will be updated using
    the override callback. The value of this attribute is not maintained internally. */
    ATTRIBUTE_FLAG_OVERRIDE = ATTRIBUTE_FLAG_NULLABLE << 1, /* 0x80 */
    /** The attribute is non-volatile but its value will be changed frequently. If an attribute has this flag, its value
     will not be written to flash immediately. A timer will be started and the attribute value will be written after
     timeout. */
    ATTRIBUTE_FLAG_DEFERRED = ATTRIBUTE_FLAG_NULLABLE << 2, /* 0x100 */
    /** The attribute is managed internally and is not stored in the ESP Matter database.
    If not set, ATTRIBUTE_FLAG_EXTERNAL_STORAGE flag will be enabled. */
    ATTRIBUTE_FLAG_MANAGED_INTERNALLY = ATTRIBUTE_FLAG_NULLABLE << 3, /* 0x200 */
} attribute_flags_t;

/** Command flags */
typedef enum command_flags {
    /** No specific flags */
    COMMAND_FLAG_NONE = 0x00,
    /** The command is not a standard command */
    COMMAND_FLAG_CUSTOM = 0x01,
    /** The command is client generated */
    COMMAND_FLAG_ACCEPTED = 0x02,
    /** The command is server generated */
    COMMAND_FLAG_GENERATED = 0x04,
} command_flags_t;

} /* esp_matter */

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
