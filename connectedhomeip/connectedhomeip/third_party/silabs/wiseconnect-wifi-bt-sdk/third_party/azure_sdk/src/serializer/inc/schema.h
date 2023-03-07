// Copyright (c) Microsoft. All rights reserved.MOCKABLE_FUNCTION(,
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SCHEMA_H
#define SCHEMA_H

#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "agenttypesystem.h"

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

/* Codes_SRS_SCHEMA_99_095: [Schema shall expose the following API:] */

typedef struct SCHEMA_HANDLE_DATA_TAG* SCHEMA_HANDLE;
typedef struct SCHEMA_MODEL_TYPE_HANDLE_DATA_TAG* SCHEMA_MODEL_TYPE_HANDLE;
typedef struct SCHEMA_STRUCT_TYPE_HANDLE_DATA_TAG* SCHEMA_STRUCT_TYPE_HANDLE;
typedef struct SCHEMA_PROPERTY_HANDLE_DATA_TAG* SCHEMA_PROPERTY_HANDLE;
typedef struct SCHEMA_REPORTED_PROPERTY_HANDLE_DATA_TAG* SCHEMA_REPORTED_PROPERTY_HANDLE;
typedef struct SCHEMA_DESIRED_PROPERTY_HANDLE_DATA_TAG* SCHEMA_DESIRED_PROPERTY_HANDLE;
typedef struct SCHEMA_ACTION_HANDLE_DATA_TAG* SCHEMA_ACTION_HANDLE;
typedef struct SCHEMA_ACTION_ARGUMENT_HANDLE_DATA_TAG* SCHEMA_ACTION_ARGUMENT_HANDLE;
typedef struct SCHEMA_METHOD_ARGUMENT_HANDLE_DATA_TAG* SCHEMA_METHOD_ARGUMENT_HANDLE;
typedef struct SCHEMA_METHOD_HANDLE_DATA_TAG* SCHEMA_METHOD_HANDLE;


typedef void(*pfOnDesiredProperty)(void* model);
typedef int(*pfDesiredPropertyFromAGENT_DATA_TYPE)(const AGENT_DATA_TYPE* source, void* dest);
typedef void(*pfDesiredPropertyInitialize)(void* destination);
typedef void(*pfDesiredPropertyDeinitialize)(void* destination);


#define SCHEMA_RESULT_VALUES    \
SCHEMA_OK,                      \
SCHEMA_INVALID_ARG,             \
SCHEMA_DUPLICATE_ELEMENT,       \
SCHEMA_ELEMENT_NOT_FOUND,       \
SCHEMA_MODEL_IN_USE,            \
SCHEMA_DEVICE_COUNT_ZERO,       \
SCHEMA_ERROR

MU_DEFINE_ENUM_WITHOUT_INVALID(SCHEMA_RESULT, SCHEMA_RESULT_VALUES)

#define SCHEMA_ELEMENT_TYPE_VALUES \
SCHEMA_NOT_FOUND, \
SCHEMA_SEARCH_INVALID_ARG, \
SCHEMA_PROPERTY, \
SCHEMA_REPORTED_PROPERTY, \
SCHEMA_DESIRED_PROPERTY, \
SCHEMA_MODEL_ACTION, \
SCHEMA_MODEL_IN_MODEL

MU_DEFINE_ENUM_WITHOUT_INVALID(SCHEMA_ELEMENT_TYPE, SCHEMA_ELEMENT_TYPE_VALUES);

typedef struct SCHEMA_MODEL_ELEMENT_TAG
{
    SCHEMA_ELEMENT_TYPE elementType;
    union ELEMENT_HANDLE_UNION_TAG
    {
        SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle;
        SCHEMA_PROPERTY_HANDLE propertyHandle;
        SCHEMA_REPORTED_PROPERTY_HANDLE reportedPropertyHandle;
        SCHEMA_ACTION_HANDLE actionHandle;
        SCHEMA_MODEL_TYPE_HANDLE modelHandle;
    } elementHandle;
}SCHEMA_MODEL_ELEMENT;

MOCKABLE_FUNCTION(, SCHEMA_HANDLE, Schema_Create, const char*, schemaNamespace, void*, metadata);
MOCKABLE_FUNCTION(, void*, Schema_GetMetadata, SCHEMA_HANDLE, schemaHandle);
MOCKABLE_FUNCTION(, size_t, Schema_GetSchemaCount);
MOCKABLE_FUNCTION(, SCHEMA_HANDLE, Schema_GetSchemaByNamespace, const char*, schemaNamespace);
MOCKABLE_FUNCTION(, SCHEMA_HANDLE, Schema_GetSchemaForModel, const char*, modelName);
MOCKABLE_FUNCTION(, const char*, Schema_GetSchemaNamespace, SCHEMA_HANDLE, schemaHandle);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddDeviceRef, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_ReleaseDeviceRef, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);

MOCKABLE_FUNCTION(, SCHEMA_MODEL_TYPE_HANDLE, Schema_CreateModelType, SCHEMA_HANDLE, schemaHandle, const char*, modelName);
MOCKABLE_FUNCTION(, SCHEMA_HANDLE, Schema_GetSchemaForModelType, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
MOCKABLE_FUNCTION(, const char*, Schema_GetModelName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);

MOCKABLE_FUNCTION(, SCHEMA_STRUCT_TYPE_HANDLE, Schema_CreateStructType, SCHEMA_HANDLE, schemaHandle, const char*, structTypeName);

MOCKABLE_FUNCTION(, const char*, Schema_GetStructTypeName, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddStructTypeProperty, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, const char*, propertyName, const char*, propertyType);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddModelProperty, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName, const char*, propertyType);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddModelReportedProperty, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, reportedPropertyName, const char*, reportedPropertyType);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddModelDesiredProperty, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, desiredPropertyName, const char*, desiredPropertyType, pfDesiredPropertyFromAGENT_DATA_TYPE, desiredPropertyFromAGENT_DATA_TYPE, pfDesiredPropertyInitialize, desiredPropertyInitialize, pfDesiredPropertyDeinitialize, desiredPropertyDeinitialize, size_t, offset, pfOnDesiredProperty, onDesiredProperty);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddModelModel, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName, SCHEMA_MODEL_TYPE_HANDLE, modelType, size_t, offset, pfOnDesiredProperty, onDesiredProperty);
MOCKABLE_FUNCTION(, SCHEMA_ACTION_HANDLE, Schema_CreateModelAction, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, actionName);
MOCKABLE_FUNCTION(, SCHEMA_METHOD_HANDLE, Schema_CreateModelMethod, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, methodName);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddModelActionArgument, SCHEMA_ACTION_HANDLE, actionHandle, const char*, argumentName, const char*, argumentType);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddModelMethodArgument, SCHEMA_METHOD_HANDLE, methodHandle, const char*, argumentName, const char*, argumentType);
MOCKABLE_FUNCTION(, pfDesiredPropertyFromAGENT_DATA_TYPE, Schema_GetModelDesiredProperty_pfDesiredPropertyFromAGENT_DATA_TYPE, SCHEMA_DESIRED_PROPERTY_HANDLE, desiredPropertyHandle);
MOCKABLE_FUNCTION(, pfOnDesiredProperty, Schema_GetModelDesiredProperty_pfOnDesiredProperty, SCHEMA_DESIRED_PROPERTY_HANDLE, desiredPropertyHandle);


MOCKABLE_FUNCTION(, size_t, Schema_GetModelModelByName_Offset, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName);
MOCKABLE_FUNCTION(, pfOnDesiredProperty, Schema_GetModelModelByName_OnDesiredProperty, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName);

MOCKABLE_FUNCTION(, size_t, Schema_GetModelModelByIndex_Offset, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index);

MOCKABLE_FUNCTION(, size_t, Schema_GetModelDesiredProperty_offset, SCHEMA_DESIRED_PROPERTY_HANDLE, desiredPropertyHandle);
MOCKABLE_FUNCTION(, const char*, Schema_GetModelDesiredPropertyType, SCHEMA_DESIRED_PROPERTY_HANDLE, desiredPropertyHandle);
MOCKABLE_FUNCTION(, pfDesiredPropertyDeinitialize, Schema_GetModelDesiredProperty_pfDesiredPropertyDeinitialize, SCHEMA_DESIRED_PROPERTY_HANDLE, desiredPropertyHandle);
MOCKABLE_FUNCTION(, pfDesiredPropertyInitialize, Schema_GetModelDesiredProperty_pfDesiredPropertyInitialize, SCHEMA_DESIRED_PROPERTY_HANDLE, desiredPropertyHandle);

MOCKABLE_FUNCTION(, SCHEMA_MODEL_ELEMENT, Schema_GetModelElementByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, elementName);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelCount, SCHEMA_HANDLE, schemaHandle, size_t*, modelCount);
MOCKABLE_FUNCTION(, SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelByName, SCHEMA_HANDLE, schemaHandle, const char*, modelName);
MOCKABLE_FUNCTION(, SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelByIndex, SCHEMA_HANDLE, schemaHandle, size_t, index);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelPropertyCount, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t*, propertyCount);
MOCKABLE_FUNCTION(, SCHEMA_PROPERTY_HANDLE, Schema_GetModelPropertyByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName);
MOCKABLE_FUNCTION(, SCHEMA_PROPERTY_HANDLE, Schema_GetModelPropertyByIndex, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelReportedPropertyCount, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t*, reportedPropertyCount);
MOCKABLE_FUNCTION(, SCHEMA_REPORTED_PROPERTY_HANDLE, Schema_GetModelReportedPropertyByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, reportedPropertyName);
MOCKABLE_FUNCTION(, SCHEMA_REPORTED_PROPERTY_HANDLE, Schema_GetModelReportedPropertyByIndex, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelDesiredPropertyCount, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t*, desiredPropertyCount);
MOCKABLE_FUNCTION(, SCHEMA_DESIRED_PROPERTY_HANDLE, Schema_GetModelDesiredPropertyByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, desiredPropertyName);
MOCKABLE_FUNCTION(, SCHEMA_DESIRED_PROPERTY_HANDLE, Schema_GetModelDesiredPropertyByIndex, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelModelCount, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t*, modelCount);
MOCKABLE_FUNCTION(, SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelModelByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName);
MOCKABLE_FUNCTION(, SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelModelyByIndex, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index);
MOCKABLE_FUNCTION(, const char*, Schema_GetModelModelPropertyNameByIndex, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index);

MOCKABLE_FUNCTION(, bool, Schema_ModelPropertyByPathExists, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyPath);

MOCKABLE_FUNCTION(, bool, Schema_ModelReportedPropertyByPathExists, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, reportedPropertyPath);
MOCKABLE_FUNCTION(, bool, Schema_ModelDesiredPropertyByPathExists, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, desiredPropertyPath);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelActionCount, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t*, actionCount);
MOCKABLE_FUNCTION(, SCHEMA_ACTION_HANDLE, Schema_GetModelActionByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, actionName);
MOCKABLE_FUNCTION(, SCHEMA_METHOD_HANDLE, Schema_GetModelMethodByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, methodName);
MOCKABLE_FUNCTION(, SCHEMA_ACTION_HANDLE, Schema_GetModelActionByIndex, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelActionArgumentCount, SCHEMA_ACTION_HANDLE, actionHandle, size_t*, argumentCount);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelMethodArgumentCount, SCHEMA_METHOD_HANDLE, methodHandle, size_t*, argumentCount);
MOCKABLE_FUNCTION(, const char*, Schema_GetModelActionName, SCHEMA_ACTION_HANDLE, actionHandle);

MOCKABLE_FUNCTION(, SCHEMA_ACTION_ARGUMENT_HANDLE, Schema_GetModelActionArgumentByName, SCHEMA_ACTION_HANDLE, actionHandle, const char*, actionArgumentName);
MOCKABLE_FUNCTION(, SCHEMA_ACTION_ARGUMENT_HANDLE, Schema_GetModelActionArgumentByIndex, SCHEMA_ACTION_HANDLE, actionHandle, size_t, argumentIndex);
MOCKABLE_FUNCTION(, SCHEMA_METHOD_ARGUMENT_HANDLE, Schema_GetModelMethodArgumentByIndex, SCHEMA_METHOD_HANDLE, actionHandle, size_t, argumentIndex);
MOCKABLE_FUNCTION(, const char*, Schema_GetActionArgumentName, SCHEMA_ACTION_ARGUMENT_HANDLE, actionArgumentHandle);
MOCKABLE_FUNCTION(, const char*, Schema_GetMethodArgumentName, SCHEMA_METHOD_ARGUMENT_HANDLE, methodArgumentHandle);
MOCKABLE_FUNCTION(, const char*, Schema_GetActionArgumentType, SCHEMA_ACTION_ARGUMENT_HANDLE, actionArgumentHandle);
MOCKABLE_FUNCTION(, const char*, Schema_GetMethodArgumentType, SCHEMA_METHOD_ARGUMENT_HANDLE, methodArgumentHandle);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetStructTypeCount, SCHEMA_HANDLE, schemaHandle, size_t*, structTypeCount);
MOCKABLE_FUNCTION(, SCHEMA_STRUCT_TYPE_HANDLE, Schema_GetStructTypeByName, SCHEMA_HANDLE, schemaHandle, const char*, structTypeName);
MOCKABLE_FUNCTION(, SCHEMA_STRUCT_TYPE_HANDLE, Schema_GetStructTypeByIndex, SCHEMA_HANDLE, schemaHandle, size_t, index);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetStructTypePropertyCount, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, size_t*, propertyCount);
MOCKABLE_FUNCTION(, SCHEMA_PROPERTY_HANDLE, Schema_GetStructTypePropertyByName, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, const char*, propertyName);
MOCKABLE_FUNCTION(, SCHEMA_PROPERTY_HANDLE, Schema_GetStructTypePropertyByIndex, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, size_t, index);
MOCKABLE_FUNCTION(, const char*, Schema_GetPropertyName, SCHEMA_PROPERTY_HANDLE, propertyHandle);
MOCKABLE_FUNCTION(, const char*, Schema_GetPropertyType, SCHEMA_PROPERTY_HANDLE, propertyHandle);

MOCKABLE_FUNCTION(, void, Schema_Destroy, SCHEMA_HANDLE, schemaHandle);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_DestroyIfUnused,SCHEMA_MODEL_TYPE_HANDLE, modelHandle);

#ifdef __cplusplus
}
#endif

#endif /* SCHEMA_H */
