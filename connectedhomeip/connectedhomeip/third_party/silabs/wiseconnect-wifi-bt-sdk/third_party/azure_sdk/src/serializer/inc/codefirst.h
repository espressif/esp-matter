// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CODEFIRST_H
#define CODEFIRST_H

#include "methodreturn.h"
#include "agenttypesystem.h"
#include "schema.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/strings.h"
#include "iotdevice.h"

#ifdef __cplusplus
#include <cstddef>
#include <cstdarg>
extern "C" {
#else
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#endif

typedef char* ascii_char_ptr;
typedef char* ascii_char_ptr_no_quotes;

typedef enum REFLECTION_TYPE_TAG
{
    REFLECTION_METHOD_TYPE,
    REFLECTION_DESIRED_PROPERTY_TYPE,
    REFLECTION_REPORTED_PROPERTY_TYPE,
    REFLECTION_STRUCT_TYPE,
    REFLECTION_FIELD_TYPE,
    REFLECTION_PROPERTY_TYPE,
    REFLECTION_ACTION_TYPE,
    REFLECTION_MODEL_TYPE,
    REFLECTION_NOTHING
}REFLECTION_TYPE;

typedef EXECUTE_COMMAND_RESULT (*actionWrapper)(void* device, size_t ParameterCount, const AGENT_DATA_TYPE* values);

typedef METHODRETURN_HANDLE (*methodWrapper)(void* device, size_t ParameterCount, const AGENT_DATA_TYPE* values);

typedef struct REFLECTION_STRUCT_TAG
{
    const char* name;
}REFLECTION_STRUCT;

typedef struct WRAPPER_ARGUMENT_TAG
{
    const char* type;
    const char* name;
}WRAPPER_ARGUMENT;

typedef struct REFLECTION_ACTION_TAG
{
    const char* name;
    size_t nArguments;
    const WRAPPER_ARGUMENT* arguments;
    actionWrapper wrapper;
    const char* modelName;
}REFLECTION_ACTION;

typedef struct REFLECTION_METHOD_TAG
{
    const char* name;
    size_t nArguments;
    const WRAPPER_ARGUMENT* arguments;
    methodWrapper wrapper;
    const char* modelName;
}REFLECTION_METHOD;

typedef struct REFLECTION_FIELD_TAG
{
    const char* fieldName;
    const char* fieldType;
    const char* structName;
}REFLECTION_FIELD;

typedef struct REFLECTION_PROPERTY_TAG
{
    const char* name;
    const char* type;
    int(*Create_AGENT_DATA_TYPE_from_Ptr)(void* param, AGENT_DATA_TYPE* dest);
    size_t offset;
    size_t size;
    const char* modelName;
} REFLECTION_PROPERTY;


typedef struct REFLECTION_REPORTED_PROPERTY_TAG
{
    const char* name;
    const char* type;
    int(*Create_AGENT_DATA_TYPE_from_Ptr)(void* param, AGENT_DATA_TYPE* dest);
    size_t offset;
    size_t size;
    const char* modelName;
} REFLECTION_REPORTED_PROPERTY;

typedef struct REFLECTION_DESIRED_PROPERTY_TAG
{
    pfOnDesiredProperty onDesiredProperty;
    void(*desiredPropertInitialize)(void* destination);
    void(*desiredPropertDeinitialize)(void* destination);
    const char* name;
    const char* type;
    int(*FromAGENT_DATA_TYPE)(const AGENT_DATA_TYPE* source, void* dest); /*destination is "something" everytime. When the DESIRED_PROPERTY is a MODEL, the function is empty*/
    size_t offset;
    size_t size;
    const char* modelName;
} REFLECTION_DESIRED_PROPERTY;

typedef struct REFLECTION_MODEL_TAG
{
    const char* name;
} REFLECTION_MODEL;

typedef struct REFLECTED_SOMETHING_TAG
{
    REFLECTION_TYPE type;
    const struct REFLECTED_SOMETHING_TAG* next;
    struct what
    {
        REFLECTION_METHOD method;
        REFLECTION_DESIRED_PROPERTY desiredProperty;
        REFLECTION_REPORTED_PROPERTY reportedProperty;
        REFLECTION_STRUCT structure;
        REFLECTION_FIELD field;
        REFLECTION_PROPERTY property;
        REFLECTION_ACTION action;
        REFLECTION_MODEL model;
    } what;
} REFLECTED_SOMETHING;

typedef struct REFLECTED_DATA_FROM_DATAPROVIDER_TAG
{
    const REFLECTED_SOMETHING* reflectedData;
}REFLECTED_DATA_FROM_DATAPROVIDER;

#define ALL_SOMETHING_REFLECTED(schemaNamespace) MU_C2(schemaNamespace, _allSomethingReflected)
#define ALL_REFLECTED(schemaNamespace) MU_C2(schemaNamespace, _allReflected)
#define ADDRESS_OF_ALL_REFLECTED(schemaNamespace) & MU_C2(schemaNamespace, _allReflected),
#define DECLARE_EXTERN_CONST_DATAPROVIDER_DATA(x) extern const REFLECTED_DATA_FROM_DATAPROVIDER ALL_REFLECTED(x);

#define CODEFIRST_RESULT_VALUES                \
CODEFIRST_OK,                                  \
CODEFIRST_INVALID_ARG,                         \
CODEFIRST_ALREADY_INIT,                        \
CODEFIRST_NOT_INIT,                            \
CODEFIRST_ERROR,                               \
CODEFIRST_NOT_ENOUGH_MEMORY,                   \
CODEFIRST_ACTION_NOT_FOUND,                    \
CODEFIRST_ACTION_EXECUTION_ERROR,              \
CODEFIRST_SCHEMA_ERROR,                        \
CODEFIRST_AGENT_DATA_TYPE_ERROR,               \
CODEFIRST_VALUES_FROM_DIFFERENT_DEVICES_ERROR, \
CODEFIRST_DEVICE_FAILED,                       \
CODEFIRST_DEVICE_PUBLISH_FAILED,               \
CODEFIRST_NOT_A_PROPERTY

MU_DEFINE_ENUM_WITHOUT_INVALID(CODEFIRST_RESULT, CODEFIRST_RESULT_VALUES)

#include "umock_c/umock_c_prod.h"
MOCKABLE_FUNCTION(, CODEFIRST_RESULT, CodeFirst_Init, const char*, overrideSchemaNamespace);
MOCKABLE_FUNCTION(, void, CodeFirst_Deinit);
MOCKABLE_FUNCTION(, SCHEMA_HANDLE, CodeFirst_RegisterSchema, const char*, schemaNamespace, const REFLECTED_DATA_FROM_DATAPROVIDER*, metadata);

MOCKABLE_FUNCTION(, EXECUTE_COMMAND_RESULT, CodeFirst_InvokeAction, DEVICE_HANDLE, deviceHandle, void*, callbackUserContext, const char*, relativeActionPath, const char*, actionName, size_t, parameterCount, const AGENT_DATA_TYPE*, parameterValues);

MOCKABLE_FUNCTION(, METHODRETURN_HANDLE, CodeFirst_InvokeMethod, DEVICE_HANDLE, deviceHandle, void*, callbackUserContext, const char*, relativeMethodPath, const char*, methodName, size_t, parameterCount, const AGENT_DATA_TYPE*, parameterValues);

MOCKABLE_FUNCTION(, EXECUTE_COMMAND_RESULT, CodeFirst_ExecuteCommand, void*, device, const char*, command);

MOCKABLE_FUNCTION(, METHODRETURN_HANDLE, CodeFirst_ExecuteMethod, void*, device, const char*, methodName, const char*, methodPayload);

MOCKABLE_FUNCTION(, void*, CodeFirst_CreateDevice, SCHEMA_MODEL_TYPE_HANDLE, model, const REFLECTED_DATA_FROM_DATAPROVIDER*, metadata, size_t, dataSize, bool, includePropertyPath);
MOCKABLE_FUNCTION(, void, CodeFirst_DestroyDevice, void*, device);

extern CODEFIRST_RESULT CodeFirst_SendAsync(unsigned char** destination, size_t* destinationSize, size_t numProperties, ...);
extern CODEFIRST_RESULT CodeFirst_SendAsyncReported(unsigned char** destination, size_t* destinationSize, size_t numReportedProperties, ...);

MOCKABLE_FUNCTION(, CODEFIRST_RESULT, CodeFirst_IngestDesiredProperties, void*, device, const char*, jsonPayload, bool, parseDesiredNode);

MOCKABLE_FUNCTION(, AGENT_DATA_TYPE_TYPE, CodeFirst_GetPrimitiveType, const char*, typeName);

#ifdef __cplusplus
}
#endif

#endif /* CODEFIRST_H */
