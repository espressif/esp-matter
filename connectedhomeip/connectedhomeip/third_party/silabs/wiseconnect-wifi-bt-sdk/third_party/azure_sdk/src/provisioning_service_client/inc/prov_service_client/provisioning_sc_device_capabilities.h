// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROVISIONING_SC_DEVICE_CAPABILITIES_H
#define PROVISIONING_SC_DEVICE_CAPABILITIES_H

#ifdef __cplusplus
#include <cstdbool>
extern "C" {
#else
#include <stdbool.h>
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"
#include "parson.h"

typedef struct DEVICE_CAPABILITIES_TAG* DEVICE_CAPABILITIES_HANDLE;

/** @brief  Creates a Device Capabilities handle that can be used in consequent APIs, with all fields initialized to "false".
*
* @return   A non-NULL handle representing Device Capabilities for use with the Provisioning Service, and NULL on failure.
*/
MOCKABLE_FUNCTION(, DEVICE_CAPABILITIES_HANDLE, deviceCapabilities_create);

/** @brief  Destroys a Device Capabilities handle, freeing all associated memory. Please note that if the Device Capabilities are
*           attached to an Enrollment, this will remove it.
*
* @param    capabilities    The handle of the Device Capabilities to be destroyed
*/
MOCKABLE_FUNCTION(, void, deviceCapabilities_destroy, DEVICE_CAPABILITIES_HANDLE, capabilities);

/* Accessor Functions */
MOCKABLE_FUNCTION(, bool, deviceCapabilities_isIotEdgeCapable, DEVICE_CAPABILITIES_HANDLE, capabilities);
MOCKABLE_FUNCTION(, void, deviceCapabilities_setIotEdgeCapable, DEVICE_CAPABILITIES_HANDLE, capabilities, bool, iotEdgeCapable);


/*---INTERNAL USAGE ONLY---*/
MOCKABLE_FUNCTION(, JSON_Value*, deviceCapabilities_toJson, DEVICE_CAPABILITIES_HANDLE, capabilities);
MOCKABLE_FUNCTION(, DEVICE_CAPABILITIES_HANDLE, deviceCapabilities_fromJson, JSON_Object*, root_object);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROVISIONING_SC_DEVICE_CAPABILITIES_H */