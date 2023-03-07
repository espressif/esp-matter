// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROVISIONING_SC_TWIN_H
#define PROVISIONING_SC_TWIN_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"
#include "parson.h"

typedef struct INITIAL_TWIN_TAG* INITIAL_TWIN_HANDLE;

 /* Initial Twin Operation Functions */

/** @brief  Creates an Initial Twin handle for use in consequent APIs.
*
* @param    tags                    The json string for the tags of the initial Twin State
* @param    desired_properties      The json string for the desired properties of the initial Twin State
*
* @return   A non-NULL handle representing an Initial Twin for use with Provisioning Service, and NULL on failure.
*/
MOCKABLE_FUNCTION(, INITIAL_TWIN_HANDLE, initialTwin_create, const char*, tags, const char*, desired_properties);

/** @brief  Destroys an Initial Twin handle, freeing all associated memory. Please note that this will also cause any Enrollment
*           that the Initial Twin has been attached to to have unexpected behvaiours. Do not use this function
*           unless the Initial Twin is unattached.
*
* @param    twin      The handle of the Initial Twin to be destroyed
*/
MOCKABLE_FUNCTION(, void, initialTwin_destroy, INITIAL_TWIN_HANDLE, twin);

/* Initial Twin Accessor Functions */
MOCKABLE_FUNCTION(, const char*, initialTwin_getTags, INITIAL_TWIN_HANDLE, twin);
MOCKABLE_FUNCTION(, int, initialTwin_setTags, INITIAL_TWIN_HANDLE, twin, const char*, tags);
MOCKABLE_FUNCTION(, const char*, initialTwin_getDesiredProperties, INITIAL_TWIN_HANDLE, twin);
MOCKABLE_FUNCTION(, int, initialTwin_setDesiredProperties, INITIAL_TWIN_HANDLE, twin, const char*, desiredProperties);



/*---INTERNAL USAGE ONLY---*/
MOCKABLE_FUNCTION(, INITIAL_TWIN_HANDLE, initialTwin_fromJson, JSON_Object*, root_object);
MOCKABLE_FUNCTION(, JSON_Value*, initialTwin_toJson, const INITIAL_TWIN_HANDLE, twin);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROVISIONING_SC_TWIN_H */
