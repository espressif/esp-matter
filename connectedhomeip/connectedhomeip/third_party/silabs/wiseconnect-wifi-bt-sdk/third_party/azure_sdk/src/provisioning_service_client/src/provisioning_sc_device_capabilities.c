// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdbool.h>

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"

#include "prov_service_client/provisioning_sc_device_capabilities.h"
#include "prov_service_client/provisioning_sc_json_const.h"
#include "parson.h"

typedef struct DEVICE_CAPABILITIES_TAG
{
    bool iotEdge;
} DEVICE_CAPABILITIES;

DEVICE_CAPABILITIES_HANDLE deviceCapabilities_create(void)
{
    DEVICE_CAPABILITIES_HANDLE new_capabilities = NULL;
    if ((new_capabilities = malloc(sizeof(DEVICE_CAPABILITIES))) == NULL)
    {
        LogError("Failed to allocate device capabilities");
    }
    else
    {
        new_capabilities->iotEdge = false;
    }
    return new_capabilities;
}

void deviceCapabilities_destroy(DEVICE_CAPABILITIES_HANDLE capabilities)
{
    if (capabilities != NULL)
    {
        free(capabilities);
    }
}

DEVICE_CAPABILITIES_HANDLE deviceCapabilities_fromJson(JSON_Object* root_object)
{
    DEVICE_CAPABILITIES_HANDLE new_capabilities = NULL;

    if (root_object == NULL)
    {
        LogError("No device capabilites in JSON");
    }
    else if ((new_capabilities = malloc(sizeof(DEVICE_CAPABILITIES))) == NULL)
    {
        LogError("Allocation of Device Capabilities failed");
    }
    else
    {
        int iotEdge;
        memset(new_capabilities, 0, sizeof(DEVICE_CAPABILITIES));

        if ((iotEdge = json_object_get_boolean(root_object, DEVICE_CAPABILITIES_JSON_KEY_IOT_EDGE)) == -1)
        {
            LogError("Failure to retrieve key %s", DEVICE_CAPABILITIES_JSON_KEY_IOT_EDGE);
            deviceCapabilities_destroy(new_capabilities);
            new_capabilities = NULL;
        }
        else
        {
            new_capabilities->iotEdge = (bool)iotEdge;
        }
    }

    return new_capabilities;
}

JSON_Value* deviceCapabilities_toJson(DEVICE_CAPABILITIES_HANDLE capabilities)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    //Setup
    if (capabilities == NULL)
    {
        LogError("capabilities is NULL");
    }
    else if ((root_value = json_value_init_object()) == NULL)
    {
        LogError("json_value_init_object failed");
    }
    else if ((root_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("json_value_get_object failed");
        json_value_free(root_value);
        root_value = NULL;
    }
    //Set data
    else if (json_object_set_boolean(root_object, DEVICE_CAPABILITIES_JSON_KEY_IOT_EDGE, capabilities->iotEdge) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string", DEVICE_CAPABILITIES_JSON_KEY_IOT_EDGE);
        json_value_free(root_value);
        root_value = NULL;
    }
    return root_value;
}

/* Acessor Functions */
bool deviceCapabilities_isIotEdgeCapable(DEVICE_CAPABILITIES_HANDLE capabilities)
{
    bool result;

    if (capabilities == NULL)
    {
        LogError("device capabilities is NULL");
        result = false;

    }
    else
    {
        result = capabilities->iotEdge;
    }

    return result;
}

void deviceCapabilities_setIotEdgeCapable(DEVICE_CAPABILITIES_HANDLE capabilities, bool iotEdgeCapable)
{
    if (capabilities != NULL)
    {
        capabilities->iotEdge = iotEdgeCapable;
    }
}


