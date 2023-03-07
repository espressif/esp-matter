// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#include "prov_service_client/provisioning_sc_twin.h"
#include "prov_service_client/provisioning_sc_json_const.h"
#include "prov_service_client/provisioning_sc_shared_helpers.h"
#include "parson.h"

static const char* EMPTY_JSON = "{}";

typedef struct TWIN_COLLECTION_TAG
{
    char* json;
} TWIN_COLLECTION;

typedef struct INITIAL_TWIN_PROPERTIES_TAG
{
    TWIN_COLLECTION* desired;
} INITIAL_TWIN_PROPERTIES;

typedef struct INITIAL_TWIN_TAG
{
    TWIN_COLLECTION* tags;
    INITIAL_TWIN_PROPERTIES* properties;
} INITIAL_TWIN;

static void twinCollection_free(TWIN_COLLECTION* twin_collection)
{
    if (twin_collection != NULL)
    {
        free(twin_collection->json);
        free(twin_collection);
    }
}

static JSON_Value* twinCollection_toJson(const TWIN_COLLECTION* twin_collection)
{
    JSON_Value* root_value = NULL;
    if ((root_value = json_parse_string(twin_collection->json)) == NULL)
    {
        LogError("Failed to parse Twin Collection json");
    }
    return root_value;
}

static TWIN_COLLECTION* twinCollection_fromJson(JSON_Object* root_object)
{
    TWIN_COLLECTION* new_twinCollection = NULL;

    if ((new_twinCollection = malloc(sizeof(TWIN_COLLECTION))) == NULL)
    {
        LogError("Allocation of Twin Collection failed");
    }
    else
    {
        memset(new_twinCollection, 0, sizeof(TWIN_COLLECTION));

        JSON_Value* root_value = NULL;
        char* json_rep = NULL;

        if ((root_value = json_object_get_wrapping_value(root_object)) == NULL)
        {
            LogError("Failed to get Twin Collection root");
            twinCollection_free(new_twinCollection);
            new_twinCollection = NULL;
        }
        else if ((json_rep = json_serialize_to_string(root_value)) == NULL)
        {
            LogError("Failed to serialize Twin Collection");
            twinCollection_free(new_twinCollection);
            new_twinCollection = NULL;
        }
        else if (mallocAndStrcpy_s(&(new_twinCollection->json), json_rep) != 0)
        {
            LogError("Failed to copy Twin Collection json");
            twinCollection_free(new_twinCollection);
            new_twinCollection = NULL;
        }
        json_free_serialized_string(json_rep);
    }

    return new_twinCollection;
}

static TWIN_COLLECTION* twinCollection_create(const char* json)
{
    TWIN_COLLECTION* new_twinCollection = NULL;

    if ((new_twinCollection = malloc(sizeof(TWIN_COLLECTION))) == NULL)
    {
        LogError("Allocation of Twin Collection failed");
    }
    else
    {
        memset(new_twinCollection, 0, sizeof(TWIN_COLLECTION));

        if (mallocAndStrcpy_s(&(new_twinCollection->json), json) != 0)
        {
            LogError("Failed to set json in Twin Collection");
            twinCollection_free(new_twinCollection);
            new_twinCollection = NULL;
        }
    }

    return new_twinCollection;
}

static void twinProperties_free(INITIAL_TWIN_PROPERTIES* twin_properties)
{
    if (twin_properties != NULL)
    {
        twinCollection_free(twin_properties->desired);
        free(twin_properties);
    }
}

static INITIAL_TWIN_PROPERTIES* twinProperties_create(const char* desired_properties)
{
    INITIAL_TWIN_PROPERTIES* new_twinProperties = NULL;

    if ((new_twinProperties = malloc(sizeof(INITIAL_TWIN_PROPERTIES))) == NULL)
    {
        LogError("Allocation of Twin Properties failed");
    }
    else
    {
        memset(new_twinProperties, 0, sizeof(INITIAL_TWIN_PROPERTIES));

        if ((new_twinProperties->desired = twinCollection_create(desired_properties)) == NULL)
        {
            LogError("Failed to set desired properties in Twin Properties");
            twinProperties_free(new_twinProperties);
            new_twinProperties = NULL;
        }
    }

    return new_twinProperties;
}

static JSON_Value* twinProperties_toJson(INITIAL_TWIN_PROPERTIES* twin_properties)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    if ((root_value = json_value_init_object()) == NULL)
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
    else if (json_serialize_and_set_struct(root_object, INITIAL_TWIN_PROPERTIES_JSON_KEY_DESIRED, twin_properties->desired, (TO_JSON_FUNCTION)twinCollection_toJson, false) != 0)
    {
        LogError("Failed to set '%s' in JSON string representation of Twin Properties", INITIAL_TWIN_PROPERTIES_JSON_KEY_DESIRED);
        json_value_free(root_value);
        root_value = NULL;
    }

    return root_value;
}

static INITIAL_TWIN_PROPERTIES* twinProperties_fromJson(JSON_Object* root_object)
{
    INITIAL_TWIN_PROPERTIES* new_twinProperties = NULL;

    if ((new_twinProperties = malloc(sizeof(INITIAL_TWIN_PROPERTIES))) == NULL)
    {
        LogError("Allocation of Twin Properties failed");
    }
    else
    {
        memset(new_twinProperties, 0, sizeof(INITIAL_TWIN_PROPERTIES));

        if (json_deserialize_and_get_struct((void**)&(new_twinProperties->desired), root_object, INITIAL_TWIN_PROPERTIES_JSON_KEY_DESIRED, (FROM_JSON_FUNCTION)twinCollection_fromJson, false) != 0)
        {
            LogError("Failed to set '%s' in Twin Properties", INITIAL_TWIN_PROPERTIES_JSON_KEY_DESIRED);
            twinProperties_free(new_twinProperties);
            new_twinProperties = NULL;
        }
    }

    return new_twinProperties;
}

void initialTwin_destroy(INITIAL_TWIN_HANDLE twin)
{
    if (twin != NULL)
    {
        twinCollection_free(twin->tags);
        twinProperties_free(twin->properties);
        free(twin);
    }
}

JSON_Value* initialTwin_toJson(const INITIAL_TWIN_HANDLE twin)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    if (twin == NULL)
    {
        LogError("twin is NULL");
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
    else if (json_serialize_and_set_struct(root_object, INITIAL_TWIN_JSON_KEY_TAGS, twin->tags, (TO_JSON_FUNCTION)twinCollection_toJson, false) != 0)
    {
        LogError("Failed to set '%s' in JSON string representation", INITIAL_TWIN_JSON_KEY_TAGS);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_serialize_and_set_struct(root_object, INITIAL_TWIN_JSON_KEY_PROPERTIES, twin->properties, (TO_JSON_FUNCTION)twinProperties_toJson, false) != 0)
    {
        LogError("Failed to set '%s' in JSON string representation", INITIAL_TWIN_JSON_KEY_PROPERTIES);
        json_value_free(root_value);
        root_value = NULL;
    }

    return root_value;
}

INITIAL_TWIN_HANDLE initialTwin_fromJson(JSON_Object* root_object)
{
    INITIAL_TWIN_HANDLE new_initialTwin = NULL;

    if (root_object == NULL)
    {
        LogError("No initialTwin in JSON");
    }
    else if ((new_initialTwin = malloc(sizeof(INITIAL_TWIN))) == NULL)
    {
        LogError("Allocation of Twin State failed");
    }
    else
    {
        memset(new_initialTwin, 0, sizeof(INITIAL_TWIN));

        if (json_deserialize_and_get_struct((void**)&(new_initialTwin->tags), root_object, INITIAL_TWIN_JSON_KEY_TAGS, (FROM_JSON_FUNCTION)twinCollection_fromJson, false) != 0)
        {
            LogError("Failed to set '%s' in Twin State", INITIAL_TWIN_JSON_KEY_TAGS);
            initialTwin_destroy(new_initialTwin);
            new_initialTwin = NULL;
        }
        else if (json_deserialize_and_get_struct((void**)&(new_initialTwin->properties), root_object, INITIAL_TWIN_JSON_KEY_PROPERTIES, (FROM_JSON_FUNCTION)twinProperties_fromJson, false) != 0)
        {
            LogError("Failed to set '%s' in Twin State", INITIAL_TWIN_JSON_KEY_PROPERTIES);
            initialTwin_destroy(new_initialTwin);
            new_initialTwin = NULL;
        }
    }

    return new_initialTwin;
}

INITIAL_TWIN_HANDLE initialTwin_create(const char* tags, const char* desired_properties)
{
    INITIAL_TWIN_HANDLE new_twin = NULL;

    if ((tags != NULL) && (strcmp(tags, EMPTY_JSON) == 0))
    {
        tags = NULL;
    }
    if ((desired_properties != NULL) && (strcmp(desired_properties, EMPTY_JSON) == 0))
    {
        desired_properties = NULL;
    }
    if (tags == NULL && desired_properties == NULL)
    {
        LogError("no inputs");
    }
    else if ((new_twin = malloc(sizeof(INITIAL_TWIN))) == NULL)
    {
        LogError("Allocation of Twin State failed");
    }
    else
    {
        memset(new_twin, 0, sizeof(INITIAL_TWIN));

        if (tags != NULL && ((new_twin->tags = twinCollection_create(tags)) == NULL))
        {
            LogError("Failed to create tags");
            initialTwin_destroy(new_twin);
            new_twin = NULL;
        }
        else if (desired_properties != NULL && ((new_twin->properties = twinProperties_create(desired_properties)) == NULL))
        {
            LogError("Failed to create desired properties");
            initialTwin_destroy(new_twin);
            new_twin = NULL;
        }
    }

    return new_twin;
}

const char* initialTwin_getTags(INITIAL_TWIN_HANDLE twin)
{
    char* result = NULL;

    if (twin == NULL)
    {
        LogError("Twin State is NULL");
    }
    else if (twin->tags == NULL)
    {
        LogError("No tags in Twin State");
    }
    else
    {
        result = twin->tags->json;
    }

    return result;
}

int initialTwin_setTags(INITIAL_TWIN_HANDLE twin, const char* tags)
{
    int result = 0;

    if ((tags != NULL) && (strcmp(tags, EMPTY_JSON) == 0))
    {
        tags = NULL;
    }
    if (twin == NULL)
    {
        LogError("TwinState is NULL");
        result = MU_FAILURE;
    }
    else if ((tags != NULL) && (twin->tags == NULL))
    {
        if ((twin->tags = twinCollection_create(tags)) == NULL)
        {
            LogError("Failure creating Twin Collection for tags");
            result = MU_FAILURE;
        }
    }
    else if (tags == NULL)
    {
        twinCollection_free(twin->tags);
        twin->tags = NULL;
    }
    else
    {
        if (mallocAndStrcpy_overwrite(&(twin->tags->json), tags) != 0)
        {
            LogError("Failure setting tags");
            result = MU_FAILURE;
        }
    }

    return result;
}

const char* initialTwin_getDesiredProperties(INITIAL_TWIN_HANDLE twin)
{
    char* result = NULL;

    if (twin == NULL)
    {
        LogError("Twin State is NULL");
    }
    else if ((twin->properties == NULL) || (twin->properties->desired == NULL))
    {
        LogError("No Desired Properties in Twin State");
    }
    else
    {
        result = twin->properties->desired->json;
    }

    return result;
}

int initialTwin_setDesiredProperties(INITIAL_TWIN_HANDLE twin, const char* desired_properties)
{
    int result = 0;

    if ((desired_properties != NULL) && (strcmp(desired_properties, EMPTY_JSON) == 0))
    {
        desired_properties = NULL;
    }
    if (twin == NULL)
    {
        LogError("TwinState is NULL");
        result = MU_FAILURE;
    }
    else if ((desired_properties != NULL) && (twin->properties == NULL))
    {
        if ((twin->properties = twinProperties_create(desired_properties)) == NULL)
        {
            LogError("Failure creating Twin Properties");
            result = MU_FAILURE;
        }
    }
    else if ((desired_properties != NULL) && (twin->properties->desired == NULL))
    {
        if ((twin->properties->desired = twinCollection_create(desired_properties)) == NULL)
        {
            LogError("Failure creating Twin Collection for desired properties");
            result = MU_FAILURE;
        }
    }
    else if (desired_properties == NULL)
    {
        //This logic will have to change if/when additional fields are added to properties
        twinProperties_free(twin->properties);
        twin->properties = NULL;
    }
    else
    {
        if (mallocAndStrcpy_overwrite(&(twin->properties->desired->json), desired_properties) != 0)
        {
            LogError("Failure setting desired properties");
            result = MU_FAILURE;
        }
    }

    return result;
}
