// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"

#include "schema.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/vector.h"


MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(SCHEMA_RESULT, SCHEMA_RESULT_VALUES);

typedef struct SCHEMA_PROPERTY_HANDLE_DATA_TAG
{
    const char* PropertyName;
    const char* PropertyType;
} SCHEMA_PROPERTY_HANDLE_DATA;

typedef struct SCHEMA_REPORTED_PROPERTY_HANDLE_DATA_TAG
{
    const char* reportedPropertyName;
    const char* reportedPropertyType;
} SCHEMA_REPORTED_PROPERTY_HANDLE_DATA;

typedef struct SCHEMA_DESIRED_PROPERTY_HANDLE_DATA_TAG
{
    pfOnDesiredProperty onDesiredProperty;
    pfDesiredPropertyInitialize desiredPropertInitialize;
    pfDesiredPropertyDeinitialize desiredPropertDeinitialize;
    const char* desiredPropertyName;
    const char* desiredPropertyType;
    pfDesiredPropertyFromAGENT_DATA_TYPE desiredPropertyFromAGENT_DATA_TYPE;
    size_t offset;
} SCHEMA_DESIRED_PROPERTY_HANDLE_DATA;

typedef struct SCHEMA_ACTION_ARGUMENT_HANDLE_DATA_TAG
{
    const char* Name;
    const char* Type;
} SCHEMA_ACTION_ARGUMENT_HANDLE_DATA;

typedef struct SCHEMA_METHOD_ARGUMENT_HANDLE_DATA_TAG
{
    char* Name;
    char* Type;
} SCHEMA_METHOD_ARGUMENT_HANDLE_DATA;

typedef struct SCHEMA_ACTION_HANDLE_DATA_TAG
{
    const char* ActionName;
    size_t ArgumentCount;
    SCHEMA_ACTION_ARGUMENT_HANDLE* ArgumentHandles;
} SCHEMA_ACTION_HANDLE_DATA;

typedef struct SCHEMA_METHOD_HANDLE_DATA_TAG
{
    char* methodName;
    VECTOR_HANDLE methodArguments; /*holds SCHEMA_METHOD_ARGUMENT_HANDLE*/
} SCHEMA_METHOD_HANDLE_DATA;

typedef struct MODEL_IN_MODEL_TAG
{
    pfOnDesiredProperty onDesiredProperty; /*is NULL if not specified or if the model in model is not WITH_DESIRED_PROPERTY*/
    size_t offset; /*offset of the model in model (offsetof)*/
    const char* propertyName;
    SCHEMA_MODEL_TYPE_HANDLE modelHandle;
} MODEL_IN_MODEL;

typedef struct SCHEMA_MODEL_TYPE_HANDLE_DATA_TAG
{
    VECTOR_HANDLE methods; /*holds SCHEMA_METHOD_HANDLE*/
    VECTOR_HANDLE desiredProperties; /*holds SCHEMA_DESIRED_PROPERTY_HANDLE_DATA*/
    const char* Name;
    SCHEMA_HANDLE SchemaHandle;
    SCHEMA_PROPERTY_HANDLE* Properties;
    size_t PropertyCount;
    VECTOR_HANDLE reportedProperties; /*holds SCHEMA_REPORTED_PROPERTY_HANDLE*/
    SCHEMA_ACTION_HANDLE* Actions;
    size_t ActionCount;
    VECTOR_HANDLE models;
    size_t DeviceCount;
} SCHEMA_MODEL_TYPE_HANDLE_DATA;

typedef struct SCHEMA_STRUCT_TYPE_HANDLE_DATA_TAG
{
    const char* Name;
    SCHEMA_PROPERTY_HANDLE* Properties;
    size_t PropertyCount;
} SCHEMA_STRUCT_TYPE_HANDLE_DATA;

typedef struct SCHEMA_HANDLE_DATA_TAG
{
    void* metadata;
    const char* Namespace;
    SCHEMA_MODEL_TYPE_HANDLE* ModelTypes;
    size_t ModelTypeCount;
    SCHEMA_STRUCT_TYPE_HANDLE* StructTypes;
    size_t StructTypeCount;
} SCHEMA_HANDLE_DATA;

static VECTOR_HANDLE g_schemas = NULL;

static void DestroyProperty(SCHEMA_PROPERTY_HANDLE propertyHandle)
{
    SCHEMA_PROPERTY_HANDLE_DATA* propertyType = (SCHEMA_PROPERTY_HANDLE_DATA*)propertyHandle;
    free((void*)propertyType->PropertyName);
    free((void*)propertyType->PropertyType);
    free(propertyType);
}

static void DestroyActionArgument(SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle)
{
    SCHEMA_ACTION_ARGUMENT_HANDLE_DATA* actionArgument = (SCHEMA_ACTION_ARGUMENT_HANDLE_DATA*)actionArgumentHandle;
    if (actionArgument != NULL)
    {
        free((void*)actionArgument->Name);
        free((void*)actionArgument->Type);
        free(actionArgument);
    }
}

static void DestroyMethodArgument(SCHEMA_METHOD_ARGUMENT_HANDLE methodArgumentHandle)
{
    free(methodArgumentHandle->Name);
    free(methodArgumentHandle->Type);
    free(methodArgumentHandle);
}

static void DestroyAction(SCHEMA_ACTION_HANDLE actionHandle)
{
    SCHEMA_ACTION_HANDLE_DATA* action = (SCHEMA_ACTION_HANDLE_DATA*)actionHandle;
    if (action != NULL)
    {
        size_t j;

        for (j = 0; j < action->ArgumentCount; j++)
        {
            DestroyActionArgument(action->ArgumentHandles[j]);
        }
        free(action->ArgumentHandles);

        free((void*)action->ActionName);
        free(action);
    }
}

static void DestroyMethod(SCHEMA_METHOD_HANDLE methodHandle)
{
    size_t nArguments = VECTOR_size(methodHandle->methodArguments);

    for (size_t j = 0; j < nArguments; j++)
    {
        SCHEMA_METHOD_ARGUMENT_HANDLE* methodArgumentHandle = VECTOR_element(methodHandle->methodArguments, j);
        if (methodArgumentHandle != NULL)
        {
            DestroyMethodArgument(*methodArgumentHandle);
        }
    }
    free(methodHandle->methodName);
    VECTOR_destroy(methodHandle->methodArguments);
    free(methodHandle);
}

static void DestroyMethods(SCHEMA_MODEL_TYPE_HANDLE modelHandle)
{
    size_t nMethods = VECTOR_size(modelHandle->methods);

    for (size_t j = 0; j < nMethods; j++)
    {
        SCHEMA_METHOD_HANDLE* methodHandle = VECTOR_element(modelHandle->methods, j);
        if (methodHandle != NULL)
        {
            DestroyMethod(*methodHandle);
        }
    }
    VECTOR_destroy(modelHandle->methods);
}


static void DestroyStruct(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle)
{
    size_t i;
    SCHEMA_STRUCT_TYPE_HANDLE_DATA* structType = (SCHEMA_STRUCT_TYPE_HANDLE_DATA*)structTypeHandle;
    if (structType != NULL)
    {
        for (i = 0; i < structType->PropertyCount; i++)
        {
            DestroyProperty(structType->Properties[i]);
        }
        free(structType->Properties);

        free((void*)structType->Name);

        free(structType);
    }
}

static void DestroyModel(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle)
{
    SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
    size_t i, nReportedProperties;

    free((void*)modelType->Name);
    modelType->Name = NULL;

    for (i = 0; i < modelType->PropertyCount; i++)
    {
        DestroyProperty(modelType->Properties[i]);
    }

    free(modelType->Properties);

    for (i = 0; i < modelType->ActionCount; i++)
    {
        DestroyAction(modelType->Actions[i]);
    }

    DestroyMethods(modelType);

    /*destroy the vector holding the added models. This does not destroy the said models, however, their names shall be*/
    for (i = 0; i < VECTOR_size(modelType->models); i++)
    {
        MODEL_IN_MODEL* temp = (MODEL_IN_MODEL*)VECTOR_element(modelType->models, i);
        if (temp != NULL)
        {
            free((void*)temp->propertyName);
        }
    }

    nReportedProperties = VECTOR_size(modelType->reportedProperties);
    for (i = 0;i < nReportedProperties;i++)
    {
        SCHEMA_REPORTED_PROPERTY_HANDLE_DATA* reportedProperty = *(SCHEMA_REPORTED_PROPERTY_HANDLE_DATA **)VECTOR_element(modelType->reportedProperties, i);
        free((void*)reportedProperty->reportedPropertyName);
        free((void*)reportedProperty->reportedPropertyType);
        free(reportedProperty);
    }
    VECTOR_destroy(modelType->reportedProperties);

    size_t nDesiredProperties = VECTOR_size(modelType->desiredProperties);
    for (i = 0;i < nDesiredProperties;i++)
    {
        SCHEMA_DESIRED_PROPERTY_HANDLE_DATA* desiredProperty = *(SCHEMA_DESIRED_PROPERTY_HANDLE_DATA **)VECTOR_element(modelType->desiredProperties, i);
        free((void*)desiredProperty->desiredPropertyName);
        free((void*)desiredProperty->desiredPropertyType);
        free(desiredProperty);
    }
    VECTOR_destroy(modelType->desiredProperties);

    VECTOR_clear(modelType->models);
    VECTOR_destroy(modelType->models);

    free(modelType->Actions);
    free(modelType);
}

static SCHEMA_RESULT AddModelProperty(SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType, const char* name, const char* type)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_013:[If any of the arguments is NULL, Schema_AddModelProperty shall return SCHEMA_INVALID_ARG.] */
    if ((modelType == NULL) ||
        (name == NULL) ||
        (type == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        size_t i;

        /* Codes_SRS_SCHEMA_99_015:[The property name shall be unique per model, if the same property name is added twice to a model, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
        for (i = 0; i < modelType->PropertyCount; i++)
        {
            SCHEMA_PROPERTY_HANDLE_DATA* property = (SCHEMA_PROPERTY_HANDLE_DATA*)modelType->Properties[i];
            if (strcmp(property->PropertyName, name) == 0)
            {
                break;
            }
        }

        if (i < modelType->PropertyCount)
        {
            result = SCHEMA_DUPLICATE_ELEMENT;
            LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
        }
        else
        {
            SCHEMA_PROPERTY_HANDLE* newProperties = (SCHEMA_PROPERTY_HANDLE*)realloc(modelType->Properties, sizeof(SCHEMA_PROPERTY_HANDLE) * (modelType->PropertyCount + 1));
            if (newProperties == NULL)
            {
                /* Codes_SRS_SCHEMA_99_014:[On any other error, Schema_AddModelProperty shall return SCHEMA_ERROR.] */
                result = SCHEMA_ERROR;
                LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
            }
            else
            {
                SCHEMA_PROPERTY_HANDLE_DATA* newProperty;

                modelType->Properties = newProperties;
                if ((newProperty = (SCHEMA_PROPERTY_HANDLE_DATA*)malloc(sizeof(SCHEMA_PROPERTY_HANDLE_DATA))) == NULL)
                {
                    /* Codes_SRS_SCHEMA_99_014:[On any other error, Schema_AddModelProperty shall return SCHEMA_ERROR.] */
                    result = SCHEMA_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
                }
                else
                {
                    if (mallocAndStrcpy_s((char**)&newProperty->PropertyName, name) != 0)
                    {
                        /* Codes_SRS_SCHEMA_99_014:[On any other error, Schema_AddModelProperty shall return SCHEMA_ERROR.] */
                        free(newProperty);
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else if (mallocAndStrcpy_s((char**)&newProperty->PropertyType, type) != 0)
                    {
                        /* Codes_SRS_SCHEMA_99_014:[On any other error, Schema_AddModelProperty shall return SCHEMA_ERROR.] */
                        free((void*)newProperty->PropertyName);
                        free(newProperty);
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else
                    {
                        modelType->Properties[modelType->PropertyCount] = (SCHEMA_PROPERTY_HANDLE)newProperty;
                        modelType->PropertyCount++;

                        /* Codes_SRS_SCHEMA_99_012:[On success, Schema_AddModelProperty shall return SCHEMA_OK.] */
                        result = SCHEMA_OK;
                    }
                }

                /* If possible, reduce the memory of over allocation */
                if (result != SCHEMA_OK)
                {
                    if (modelType->PropertyCount > 0)
                    {
                        SCHEMA_PROPERTY_HANDLE *oldProperties = (SCHEMA_PROPERTY_HANDLE *)realloc(modelType->Properties, sizeof(SCHEMA_PROPERTY_HANDLE) * modelType->PropertyCount);
                        if (oldProperties == NULL)
                        {
                            result = SCHEMA_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
                        }
                        else
                        {
                            modelType->Properties = oldProperties;
                        }
                    }
                    else
                    {
                        modelType->Properties = NULL;
                    }
                }
            }
        }
    }

    return result;
}

static bool SchemaHandlesMatch(const SCHEMA_HANDLE* handle, const SCHEMA_HANDLE* otherHandle)
{
    return (*handle == *otherHandle);
}

static bool SchemaNamespacesMatch(const SCHEMA_HANDLE* handle, const char* schemaNamespace)
{
    const SCHEMA_HANDLE_DATA* schema = (SCHEMA_HANDLE_DATA*)*handle;
    return (strcmp(schema->Namespace, schemaNamespace) == 0);
}

/* Codes_SRS_SCHEMA_99_001:[Schema_Create shall initialize a schema with a given namespace.] */
SCHEMA_HANDLE Schema_Create(const char* schemaNamespace, void* metadata)
{
    SCHEMA_HANDLE_DATA* result;
    /*Codes_SRS_SCHEMA_02_090: [ If metadata is NULL then Schema_Create shall fail and return NULL. ]*/
    /* Codes_SRS_SCHEMA_99_004:[If schemaNamespace is NULL, Schema_Create shall fail.] */
    if (
        (schemaNamespace == NULL)||
        (metadata == NULL)
        )
    {
        /* Codes_SRS_SCHEMA_99_003:[On failure, NULL shall be returned.] */
        LogError("invalid arg const char* schemaNamespace=%p, void* metadata=%p",schemaNamespace, metadata);
        result = NULL;
    }
    else
    {
        if (g_schemas == NULL && (g_schemas = VECTOR_create(sizeof(SCHEMA_HANDLE_DATA*) ) ) == NULL)
        {
            /* Codes_SRS_SCHEMA_99_003:[On failure, NULL shall be returned.] */
            result = NULL;
            LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
        }
        else if ((result = (SCHEMA_HANDLE_DATA*)calloc(1, sizeof(SCHEMA_HANDLE_DATA))) == NULL)
        {
            /* Codes_SRS_SCHEMA_99_003:[On failure, NULL shall be returned.] */
            LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
        }
        else if (mallocAndStrcpy_s((char**)&result->Namespace, schemaNamespace) != 0)
        {
            /* Codes_SRS_SCHEMA_99_003:[On failure, NULL shall be returned.] */
            free(result);
            result = NULL;
            LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
        }
        else if (VECTOR_push_back(g_schemas, &result, 1) != 0)
        {
            free((void*)result->Namespace);
            free(result);
            result = NULL;
            LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
        }
        else
        {
            /* Codes_SRS_SCHEMA_99_002:[On success a non-NULL handle to the newly created schema shall be returned.] */
            result->ModelTypes = NULL;
            result->ModelTypeCount = 0;
            result->StructTypes = NULL;
            result->StructTypeCount = 0;
            result->metadata = metadata;
        }
    }

    return (SCHEMA_HANDLE)result;
}

size_t Schema_GetSchemaCount(void)
{
    /* Codes_SRS_SCHEMA_99_153: [Schema_GetSchemaCount shall return the number of "active" schemas (all schemas created with Schema_Create
    in the current process, for which Schema_Destroy has not been called).] */
    return VECTOR_size(g_schemas);
}

SCHEMA_HANDLE Schema_GetSchemaByNamespace(const char* schemaNamespace)
{
    /* Codes_SRS_SCHEMA_99_151: [If no active schema matches the schemaNamespace argument, Schema_GetSchemaByNamespace shall return NULL.] */
    SCHEMA_HANDLE result = NULL;

    /* Codes_SRS_SCHEMA_99_150: [If the schemaNamespace argument is NULL, Schema_GetSchemaByNamespace shall return NULL.] */
    if (schemaNamespace != NULL)
    {
        SCHEMA_HANDLE* handle = (g_schemas==NULL)?NULL:(SCHEMA_HANDLE*)VECTOR_find_if(g_schemas, (PREDICATE_FUNCTION)SchemaNamespacesMatch, schemaNamespace);
        if (handle != NULL)
        {
            /* Codes_SRS_SCHEMA_99_148: [Schema_GetSchemaByNamespace shall search all active schemas and return the schema with the
               namespace given by the schemaNamespace argument.] */
            result = *handle;
        }
    }

    return result;
}

const char* Schema_GetSchemaNamespace(SCHEMA_HANDLE schemaHandle)
{
    const char* result;

    /* Codes_SRS_SCHEMA_99_130: [If the schemaHandle argument is NULL, Schema_GetSchemaNamespace shall return NULL.] */
    if (schemaHandle == NULL)
    {
        result = NULL;
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_129: [Schema_GetSchemaNamespace shall return the namespace for the schema identified by schemaHandle.] */
        result = ((SCHEMA_HANDLE_DATA*)schemaHandle)->Namespace;
    }

    return result;
}

void Schema_Destroy(SCHEMA_HANDLE schemaHandle)
{
    /* Codes_SRS_SCHEMA_99_006:[If the schemaHandle is NULL, Schema_Destroy shall do nothing.] */
    if (schemaHandle != NULL)
    {
        SCHEMA_HANDLE_DATA* schema = (SCHEMA_HANDLE_DATA*)schemaHandle;
        size_t i;

        /* Codes_SRS_SCHEMA_99_005:[Schema_Destroy shall free all resources associated with a schema.] */
        for (i = 0; i < schema->ModelTypeCount; i++)
        {
            DestroyModel(schema->ModelTypes[i]);
        }

        free(schema->ModelTypes);

        /* Codes_SRS_SCHEMA_99_005:[Schema_Destroy shall free all resources associated with a schema.] */
        for (i = 0; i < schema->StructTypeCount; i++)
        {
            DestroyStruct(schema->StructTypes[i]);
        }

        free(schema->StructTypes);
        free((void*)schema->Namespace);
        free(schema);

        schema = (SCHEMA_HANDLE_DATA*)VECTOR_find_if(g_schemas, (PREDICATE_FUNCTION)SchemaHandlesMatch, &schemaHandle);
        if (schema != NULL)
        {
            VECTOR_erase(g_schemas, schema, 1);
        }
        // If the g_schema is empty then destroy it
        if (VECTOR_size(g_schemas) == 0)
        {
            VECTOR_destroy(g_schemas);
            g_schemas = NULL;
        }
    }
}

SCHEMA_RESULT Schema_DestroyIfUnused(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_07_189: [If modelHandle variable is NULL Schema_DestroyIfUnused shall do nothing.] */
    if (modelTypeHandle != NULL)
    {
        SCHEMA_HANDLE schemaHandle = Schema_GetSchemaForModelType(modelTypeHandle);
        if (schemaHandle == NULL)
        {
            result = SCHEMA_ERROR;
        }
        else
        {
            SCHEMA_HANDLE_DATA* schema = (SCHEMA_HANDLE_DATA*)schemaHandle;
            size_t nIndex;

            /* Codes_SRS_SCHEMA_07_190: [Schema_DestroyIfUnused shall iterate through the ModuleTypes objects and if all the DeviceCount variables 0 then it will delete the schemaHandle.] */
            for (nIndex = 0; nIndex < schema->ModelTypeCount; nIndex++)
            {
                SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)schema->ModelTypes[nIndex];
                if (modelType->DeviceCount > 0)
                    break;
            }
            /* Codes_SRS_SCHEMA_07_191: [If 1 or more DeviceCount variables are > 0 then Schema_DestroyIfUnused shall do nothing.] */
            if (nIndex == schema->ModelTypeCount)
            {
                Schema_Destroy(schemaHandle);
                result = SCHEMA_OK;
            }
            else
            {
                result = SCHEMA_MODEL_IN_USE;
            }
        }
    }
    else
    {
        result = SCHEMA_INVALID_ARG;
    }
    return result;
}

SCHEMA_RESULT Schema_AddDeviceRef(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle)
{
    SCHEMA_RESULT result;
    if (modelTypeHandle == NULL)
    {
        /* Codes_SRS_SCHEMA_07_187: [Schema_AddDeviceRef shall return SCHEMA_INVALID_ARG if modelTypeHandle is NULL.] */
        result = SCHEMA_INVALID_ARG;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* model = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        /* Codes_SRS_SCHEMA_07_188: [If the modelTypeHandle is nonNULL, Schema_AddDeviceRef shall increment the SCHEMA_MODEL_TYPE_HANDLE_DATA DeviceCount variable.] */
        model->DeviceCount++;
        result = SCHEMA_OK;
    }
    return result;
}

SCHEMA_RESULT Schema_ReleaseDeviceRef(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle)
{
    SCHEMA_RESULT result;
    /* Codes_SRS_SCHEMA_07_184: [Schema_DeviceRelease shall do nothing if the supplied modelHandle is NULL.] */
    if (modelTypeHandle == NULL)
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* model = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        if (model->DeviceCount > 0)
        {
            /* Codes_SRS_SCHEMA_07_186: [On a nonNULL SCHEMA_MODEL_TYPE_HANDLE if the DeviceCount variable is > 0 then the variable will be decremented.] */
            model->DeviceCount--;
            result = SCHEMA_OK;
        }
        else
        {
result = SCHEMA_DEVICE_COUNT_ZERO;
LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
        }
    }
    return result;
}

/* Codes_SRS_SCHEMA_99_007:[Schema_CreateModelType shall create a new model type and return a handle to it.] */
SCHEMA_MODEL_TYPE_HANDLE Schema_CreateModelType(SCHEMA_HANDLE schemaHandle, const char* modelName)
{
    SCHEMA_MODEL_TYPE_HANDLE result;

    /* Codes_SRS_SCHEMA_99_010:[If any of the arguments is NULL, Schema_CreateModelType shall fail.] */
    if ((schemaHandle == NULL) ||
        (modelName == NULL))
    {
        /* Codes_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        SCHEMA_HANDLE_DATA* schema = (SCHEMA_HANDLE_DATA*)schemaHandle;

        /* Codes_SRS_SCHEMA_99_100: [Schema_CreateModelType shall return SCHEMA_DUPLICATE_ELEMENT if modelName already exists.] */
        size_t i;
        for (i = 0; i < schema->ModelTypeCount; i++)
        {
            SCHEMA_MODEL_TYPE_HANDLE_DATA* model = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)(schema->ModelTypes[i]);
            if (strcmp(model->Name, modelName) == 0)
            {
                break;
            }
        }

        if (i < schema->ModelTypeCount)
        {
            /* Codes_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
            result = NULL;
            LogError("%s Model Name already exists", modelName);
        }

        else
        {
            SCHEMA_MODEL_TYPE_HANDLE* newModelTypes = (SCHEMA_MODEL_TYPE_HANDLE*)realloc(schema->ModelTypes, sizeof(SCHEMA_MODEL_TYPE_HANDLE) * (schema->ModelTypeCount + 1));
            if (newModelTypes == NULL)
            {
                /* Codes_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
                result = NULL;
                LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
            }
            else
            {
                SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType;
                schema->ModelTypes = newModelTypes;

                if ((modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)calloc(1, sizeof(SCHEMA_MODEL_TYPE_HANDLE_DATA))) == NULL)
                {

                    /* Codes_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
                    result = NULL;
                    LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                }
                else if (mallocAndStrcpy_s((char**)&modelType->Name, modelName) != 0)
                {
                    /* Codes_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
                    result = NULL;
                    free(modelType);
                    LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                }
                else
                {
                    modelType->models = VECTOR_create(sizeof(MODEL_IN_MODEL));
                    if (modelType->models == NULL)
                    {
                        /* Codes_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
                        LogError("unable to VECTOR_create");
                        free((void*)modelType->Name);
                        free((void*)modelType);
                        result = NULL;
                    }
                    else
                    {
                        if ((modelType->reportedProperties = VECTOR_create(sizeof(SCHEMA_REPORTED_PROPERTY_HANDLE))) == NULL)
                        {
                            /* Codes_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
                            LogError("failed to VECTOR_create (reported properties)");
                            VECTOR_destroy(modelType->models);
                            free((void*)modelType->Name);
                            free((void*)modelType);
                            result = NULL;

                        }
                        else
                        {
                            /* Codes_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
                            if ((modelType->desiredProperties = VECTOR_create(sizeof(SCHEMA_DESIRED_PROPERTY_HANDLE))) == NULL)
                            {
                                LogError("failure in VECTOR_create (desired properties)");
                                VECTOR_destroy(modelType->reportedProperties);
                                VECTOR_destroy(modelType->models);
                                free((void*)modelType->Name);
                                free((void*)modelType);
                                result = NULL;
                            }
                            else
                            {
                                if ((modelType->methods = VECTOR_create(sizeof(SCHEMA_METHOD_HANDLE))) == NULL)
                                {
                                    LogError("failure in VECTOR_create (desired properties)");
                                    VECTOR_destroy(modelType->desiredProperties);
                                    VECTOR_destroy(modelType->reportedProperties);
                                    VECTOR_destroy(modelType->models);
                                    free((void*)modelType->Name);
                                    free((void*)modelType);
                                    result = NULL;
                                }
                                else
                                {
                                    modelType->PropertyCount = 0;
                                    modelType->Properties = NULL;
                                    modelType->ActionCount = 0;
                                    modelType->Actions = NULL;
                                    modelType->SchemaHandle = schemaHandle;
                                    modelType->DeviceCount = 0;

                                    schema->ModelTypes[schema->ModelTypeCount] = modelType;
                                    schema->ModelTypeCount++;
                                    /* Codes_SRS_SCHEMA_99_008:[On success, a non-NULL handle shall be returned.] */
                                    result = (SCHEMA_MODEL_TYPE_HANDLE)modelType;
                                }
                            }
                        }
                    }
                }

                /* If possible, reduce the memory of over allocation */
                if ((result == NULL) &&(schema->ModelTypeCount>0))
                {
                    SCHEMA_MODEL_TYPE_HANDLE* oldModelTypes = (SCHEMA_MODEL_TYPE_HANDLE*)realloc(schema->ModelTypes, sizeof(SCHEMA_MODEL_TYPE_HANDLE) * schema->ModelTypeCount);
                    if (oldModelTypes == NULL)
                    {
                        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                    }
                    else
                    {
                        schema->ModelTypes = oldModelTypes;
                    }
                }
            }
        }
    }

    return result;
}

SCHEMA_HANDLE Schema_GetSchemaForModelType(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle)
{
    SCHEMA_HANDLE result;

    if (modelTypeHandle == NULL)
    {
        /* Codes_SRS_SCHEMA_99_132: [If the modelTypeHandle argument is NULL, Schema_GetSchemaForModelType shall return NULL.] */
        result = NULL;
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_131: [Schema_GetSchemaForModelType returns the schema handle for a given model type.] */
        result = ((SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle)->SchemaHandle;
    }

    return result;
}

/* Codes_SRS_SCHEMA_99_011:[Schema_AddModelProperty shall add one property to the model type identified by modelTypeHandle.] */
SCHEMA_RESULT Schema_AddModelProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName, const char* propertyType)
{
    return AddModelProperty((SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle, propertyName, propertyType);
}

static bool reportedPropertyExists(const void* element, const void* value)
{
    SCHEMA_REPORTED_PROPERTY_HANDLE_DATA* reportedProperty = *(SCHEMA_REPORTED_PROPERTY_HANDLE_DATA**)element;
    return (strcmp(reportedProperty->reportedPropertyName, value) == 0);
}

SCHEMA_RESULT Schema_AddModelReportedProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* reportedPropertyName, const char* reportedPropertyType)
{
    SCHEMA_RESULT result;
    /*Codes_SRS_SCHEMA_02_001: [ If modelTypeHandle is NULL then Schema_AddModelReportedProperty shall fail and return SCHEMA_INVALID_ARG. ]*/
    /*Codes_SRS_SCHEMA_02_002: [ If reportedPropertyName is NULL then Schema_AddModelReportedProperty shall fail and return SCHEMA_INVALID_ARG. ]*/
    /*Codes_SRS_SCHEMA_02_003: [ If reportedPropertyType is NULL then Schema_AddModelReportedProperty shall fail and return SCHEMA_INVALID_ARG. ]*/
    if (
        (modelTypeHandle == NULL) ||
        (reportedPropertyName == NULL) ||
        (reportedPropertyType == NULL)
        )
    {
        LogError("invalid argument SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, const char* reportedPropertyName=%p, const char* reportedPropertyType=%p", modelTypeHandle, reportedPropertyName, reportedPropertyType);
        result = SCHEMA_INVALID_ARG;
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        /*Codes_SRS_SCHEMA_02_004: [ If reportedPropertyName has already been added then Schema_AddModelReportedProperty shall fail and return SCHEMA_PROPERTY_ELEMENT_EXISTS. ]*/
        if (VECTOR_find_if(modelType->reportedProperties, reportedPropertyExists, reportedPropertyName) != NULL)
        {
            LogError("unable to add reportedProperty %s because it already exists", reportedPropertyName);
            result = SCHEMA_DUPLICATE_ELEMENT;
        }
        else
        {
            /*Codes_SRS_SCHEMA_02_005: [ Schema_AddModelReportedProperty shall record reportedPropertyName and reportedPropertyType. ]*/
            SCHEMA_REPORTED_PROPERTY_HANDLE_DATA* reportedProperty = (SCHEMA_REPORTED_PROPERTY_HANDLE_DATA*)malloc(sizeof(SCHEMA_REPORTED_PROPERTY_HANDLE_DATA));
            if (reportedProperty == NULL)
            {
                /*Codes_SRS_SCHEMA_02_006: [ If any error occurs then Schema_AddModelReportedProperty shall fail and return SCHEMA_ERROR. ]*/
                LogError("unable to malloc");
                result = SCHEMA_ERROR;
            }
            else
            {
                if (mallocAndStrcpy_s((char**)&reportedProperty->reportedPropertyName, reportedPropertyName) != 0)
                {
                    /*Codes_SRS_SCHEMA_02_006: [ If any error occurs then Schema_AddModelReportedProperty shall fail and return SCHEMA_ERROR. ]*/
                    LogError("unable to mallocAndStrcpy_s");
                    free(reportedProperty);
                    result = SCHEMA_ERROR;
                }
                else
                {
                    if (mallocAndStrcpy_s((char**)&reportedProperty->reportedPropertyType, reportedPropertyType) != 0)
                    {
                        /*Codes_SRS_SCHEMA_02_006: [ If any error occurs then Schema_AddModelReportedProperty shall fail and return SCHEMA_ERROR. ]*/
                        LogError("unable to mallocAndStrcpy_s");
                        free((void*)reportedProperty->reportedPropertyName);
                        free(reportedProperty);
                        result = SCHEMA_ERROR;
                    }
                    else
                    {
                        if (VECTOR_push_back(modelType->reportedProperties, &reportedProperty, 1) != 0)
                        {
                            /*Codes_SRS_SCHEMA_02_006: [ If any error occurs then Schema_AddModelReportedProperty shall fail and return SCHEMA_ERROR. ]*/
                            LogError("unable to VECTOR_push_back");
                            free((void*)reportedProperty->reportedPropertyType);
                            free((void*)reportedProperty->reportedPropertyName);
                            free(reportedProperty);
                            result = SCHEMA_ERROR;
                        }
                        else
                        {
                            /*Codes_SRS_SCHEMA_02_007: [ Otherwise Schema_AddModelReportedProperty shall succeed and return SCHEMA_OK. ]*/
                            result = SCHEMA_OK;
                        }
                    }
                }
            }
        }
    }
    return result;
}

SCHEMA_ACTION_HANDLE Schema_CreateModelAction(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* actionName)
{
    SCHEMA_ACTION_HANDLE result;

    /* Codes_SRS_SCHEMA_99_104: [If any of the modelTypeHandle or actionName arguments is NULL, Schema_CreateModelAction shall return NULL.] */
    if ((modelTypeHandle == NULL) ||
        (actionName == NULL))
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        size_t i;

        /* Codes_SRS_SCHEMA_99_105: [The action name shall be unique per model, if the same action name is added twice to a model, Schema_CreateModelAction shall return NULL.] */
        for (i = 0; i < modelType->ActionCount; i++)
        {
            SCHEMA_ACTION_HANDLE_DATA* action = (SCHEMA_ACTION_HANDLE_DATA*)modelType->Actions[i];
            if (strcmp(action->ActionName, actionName) == 0)
            {
                break;
            }
        }

        if (i < modelType->ActionCount)
        {
            result = NULL;
            LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_DUPLICATE_ELEMENT));
        }
        else
        {
            /* Codes_SRS_SCHEMA_99_102: [Schema_CreateModelAction shall add one action to the model type identified by modelTypeHandle.] */
            SCHEMA_ACTION_HANDLE* newActions = (SCHEMA_ACTION_HANDLE*)realloc(modelType->Actions, sizeof(SCHEMA_ACTION_HANDLE) * (modelType->ActionCount + 1));
            if (newActions == NULL)
            {
                /* Codes_SRS_SCHEMA_99_106: [On any other error, Schema_CreateModelAction shall return NULL.]*/
                result = NULL;
                LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
            }
            else
            {
                SCHEMA_ACTION_HANDLE_DATA* newAction;
                modelType->Actions = newActions;

                /* Codes_SRS_SCHEMA_99_103: [On success, Schema_CreateModelAction shall return a none-NULL SCHEMA_ACTION_HANDLE to the newly created action.] */
                if ((newAction = (SCHEMA_ACTION_HANDLE_DATA*)calloc(1, sizeof(SCHEMA_ACTION_HANDLE_DATA))) == NULL)
                {
                    /* Codes_SRS_SCHEMA_99_106: [On any other error, Schema_CreateModelAction shall return NULL.]*/
                    result = NULL;
                    LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                }
                else
                {
                    if (mallocAndStrcpy_s((char**)&newAction->ActionName, actionName) != 0)
                    {
                        /* Codes_SRS_SCHEMA_99_106: [On any other error, Schema_CreateModelAction shall return NULL.]*/
                        free(newAction);
                        result = NULL;
                        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                    }
                    else
                    {
                        newAction->ArgumentCount = 0;
                        newAction->ArgumentHandles = NULL;

                        modelType->Actions[modelType->ActionCount] = newAction;
                        modelType->ActionCount++;
                        result = (SCHEMA_ACTION_HANDLE)(newAction);
                    }

                    /* If possible, reduce the memory of over allocation */
                    if (result == NULL)
                    {
                        if (modelType->ActionCount > 0)
                        {
                            SCHEMA_ACTION_HANDLE *oldActions = (SCHEMA_ACTION_HANDLE *)realloc(modelType->Actions, sizeof(SCHEMA_ACTION_HANDLE) * modelType->ActionCount);
                            if (oldActions == NULL)
                            {
                                LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                            }
                            else
                            {
                                modelType->Actions = oldActions;
                            }
                        }
                        else
                        {
                            modelType->Actions = NULL;
                        }
                    }
                }
            }
        }
    }
    return result;
}


static bool methodExists(const void* element, const void* value)
{
    const SCHEMA_METHOD_HANDLE* method = (const SCHEMA_METHOD_HANDLE*)element;
    return (strcmp((*method)->methodName, value) == 0);
}


SCHEMA_METHOD_HANDLE Schema_CreateModelMethod(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* methodName)
{
    SCHEMA_METHOD_HANDLE result;

    /*Codes_SRS_SCHEMA_02_096: [ If modelTypeHandle is NULL then Schema_CreateModelMethod shall fail and return NULL. ]*/
    /*Codes_SRS_SCHEMA_02_097: [ If methodName is NULL then Schema_CreateModelMethod shall fail and return NULL. ]*/
    if ((modelTypeHandle == NULL) ||
        (methodName == NULL))
    {
        result = NULL;
        LogError("invalid argument: SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, const char* methodName=%p", modelTypeHandle, methodName);
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_103: [ If methodName already exists, then Schema_CreateModelMethod shall fail and return NULL. ]*/
        if (VECTOR_find_if(modelTypeHandle->methods, methodExists, methodName) != NULL)
        {
            LogError("method %s already exists", methodName);
            result = NULL;
        }
        else
        {
            /*Codes_SRS_SCHEMA_02_098: [ Schema_CreateModelMethod shall allocate the space for the method. ]*/
            result = malloc(sizeof(SCHEMA_METHOD_HANDLE_DATA));
            if (result == NULL)
            {
                LogError("failed to malloc");
                /*Codes_SRS_SCHEMA_02_102: [ If any of the above fails, then Schema_CreateModelMethod shall fail and return NULL. ]*/
                /*return as is*/
            }
            else
            {
                /*Codes_SRS_SCHEMA_02_099: [ Schema_CreateModelMethod shall create a VECTOR that will hold the method's arguments. ]*/
                result->methodArguments = VECTOR_create(sizeof(SCHEMA_METHOD_ARGUMENT_HANDLE));
                if (result->methodArguments == NULL)
                {
                    /*Codes_SRS_SCHEMA_02_102: [ If any of the above fails, then Schema_CreateModelMethod shall fail and return NULL. ]*/
                    LogError("failure in VECTOR_create");
                    free(result);
                    result = NULL;
                }
                else
                {
                    /*Codes_SRS_SCHEMA_02_100: [ Schema_CreateModelMethod shall clone methodName ]*/
                    if (mallocAndStrcpy_s(&result->methodName, methodName) != 0)
                    {
                        /*Codes_SRS_SCHEMA_02_102: [ If any of the above fails, then Schema_CreateModelMethod shall fail and return NULL. ]*/
                        LogError("failure in mallocAndStrcpy_s");
                        VECTOR_destroy(result->methodArguments);
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        /*Codes_SRS_SCHEMA_02_101: [ Schema_CreateModelMethod shall add the new created method to the model's list of methods. ]*/
                        if (VECTOR_push_back(modelTypeHandle->methods, &result, 1) != 0)
                        {
                            /*Codes_SRS_SCHEMA_02_102: [ If any of the above fails, then Schema_CreateModelMethod shall fail and return NULL. ]*/
                            LogError("failure in VECTOR_push_back");
                            free(result->methodName);
                            VECTOR_destroy(result->methodArguments);
                            free(result);
                            result = NULL;
                        }
                        else
                        {
                            /*Codes_SRS_SCHEMA_02_104: [ Otherwise, Schema_CreateModelMethod shall succeed and return a non-NULL SCHEMA_METHOD_HANDLE. ]*/
                            /*return as is*/
                        }
                    }
                }
            }
        }
    }
    return result;
}


SCHEMA_RESULT Schema_AddModelActionArgument(SCHEMA_ACTION_HANDLE actionHandle, const char* argumentName, const char* argumentType)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_109: [If any of the arguments is NULL, Schema_AddModelActionArgument shall return SCHEMA_INVALID_ARG.] */
    if ((actionHandle == NULL) ||
        (argumentName == NULL) ||
        (argumentType == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        SCHEMA_ACTION_HANDLE_DATA* action = (SCHEMA_ACTION_HANDLE_DATA*)actionHandle;

        /* Codes_SRS_SCHEMA_99_110: [The argument name shall be unique per action, if the same name is added twice to an action, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
        /* Codes_SRS_SCHEMA_99_111: [Schema_AddModelActionArgument shall accept arguments with different names of the same type.]  */
        size_t i;
        for (i = 0; i < action->ArgumentCount; i++)
        {
            SCHEMA_ACTION_ARGUMENT_HANDLE_DATA* actionArgument = (SCHEMA_ACTION_ARGUMENT_HANDLE_DATA*)action->ArgumentHandles[i];
            if (strcmp((actionArgument->Name), argumentName) == 0)
            {
                break;
            }
        }

        if (i < action->ArgumentCount)
        {
            result = SCHEMA_DUPLICATE_ELEMENT;
            LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
        }
        else
        {
            /* Codes_SRS_SCHEMA_99_107: [Schema_AddModelActionArgument shall add one argument name & type to an action identified by actionHandle.]  */
            SCHEMA_ACTION_ARGUMENT_HANDLE* newArguments = (SCHEMA_ACTION_ARGUMENT_HANDLE*)realloc(action->ArgumentHandles, sizeof(SCHEMA_ACTION_ARGUMENT_HANDLE) * (action->ArgumentCount + 1));
            if (newArguments == NULL)
            {
                /* Codes_SRS_SCHEMA_99_112: [On any other error, Schema_ AddModelActionArgumet shall return SCHEMA_ERROR.] */
                result = SCHEMA_ERROR;
                LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
            }
            else
            {
                action->ArgumentHandles = newArguments;

                SCHEMA_ACTION_ARGUMENT_HANDLE_DATA* newActionArgument;

                action->ArgumentHandles = newArguments;
                if ((newActionArgument = (SCHEMA_ACTION_ARGUMENT_HANDLE_DATA*)malloc(sizeof(SCHEMA_ACTION_ARGUMENT_HANDLE_DATA))) == NULL)
                {
                    /* Codes_SRS_SCHEMA_99_112: [On any other error, Schema_ AddModelActionArgumet shall return SCHEMA_ERROR.] */
                    result = SCHEMA_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
                }
                else
                {
                    if (mallocAndStrcpy_s((char**)&newActionArgument->Name, argumentName) != 0)
                    {
                        /* Codes_SRS_SCHEMA_99_112: [On any other error, Schema_ AddModelActionArgumet shall return SCHEMA_ERROR.] */
                        free(newActionArgument);
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else if (mallocAndStrcpy_s((char**)&newActionArgument->Type, argumentType) != 0)
                    {
                        /* Codes_SRS_SCHEMA_99_112: [On any other error, Schema_ AddModelActionArgumet shall return SCHEMA_ERROR.] */
                        free((void*)newActionArgument->Name);
                        free(newActionArgument);
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else
                    {
                        /* Codes_SRS_SCHEMA_99_119: [Schema_AddModelActionArgument shall preserve the order of the action arguments according to the order in which they were added, starting with index 0 for the first added argument.] */
                        action->ArgumentHandles[action->ArgumentCount] = newActionArgument;
                        action->ArgumentCount++;

                        /* Codes_SRS_SCHEMA_99_108: [On success, Schema_AddModelActionArgunent shall return SCHEMA_OK.] */
                        result = SCHEMA_OK;
                    }
                }

                /* If possible, reduce the memory of over allocation */
                if (result == SCHEMA_ERROR)
                {
                    if (action->ArgumentCount > 0)
                    {
                        SCHEMA_ACTION_ARGUMENT_HANDLE *oldArguments = (SCHEMA_ACTION_ARGUMENT_HANDLE *)realloc(action->ArgumentHandles, sizeof(SCHEMA_ACTION_ARGUMENT_HANDLE) * action->ArgumentCount);
                        if (oldArguments == NULL)
                        {
                            LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                        }
                        else
                        {
                            action->ArgumentHandles = oldArguments;
                        }
                    }
                    else
                    {
                        action->ArgumentHandles = NULL;
                    }
                }
            }
        }
    }
    return result;
}

static bool methodFindArgumentByBame(const void* element, const void* value)
{
    /*element is a pointer to SCHEMA_METHOD_ARGUMENT_HANDLE*/
    const SCHEMA_METHOD_ARGUMENT_HANDLE* decodedElement = (const SCHEMA_METHOD_ARGUMENT_HANDLE*)element;
    const char* name = (const char*)value;
    return (strcmp((*decodedElement)->Name, name) == 0);
}

SCHEMA_RESULT Schema_AddModelMethodArgument(SCHEMA_METHOD_HANDLE methodHandle, const char* argumentName, const char* argumentType)
{
    SCHEMA_RESULT result;
    /*Codes_SRS_SCHEMA_02_105: [ If methodHandle is NULL then Schema_AddModelMethodArgument shall fail and return SCHEMA_INVALID_ARG. ]*/
    /*Codes_SRS_SCHEMA_02_106: [ If argumentName is NULL then Schema_AddModelMethodArgument shall fail and return SCHEMA_INVALID_ARG. ]*/
    /*Codes_SRS_SCHEMA_02_107: [ If argumentType is NULL then Schema_AddModelMethodArgument shall fail and return SCHEMA_INVALID_ARG. ]*/
    if ((methodHandle == NULL) ||
        (argumentName == NULL) ||
        (argumentType == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_108: [ If argumentName already exists in the list of arguments then then Schema_AddModelMethodArgument shall fail and return SCHEMA_INVALID_ARG. ]*/
        if (VECTOR_find_if(methodHandle->methodArguments, methodFindArgumentByBame, argumentName) != NULL)
        {
            LogError("an argument with name %s already exists", argumentName);
            result = SCHEMA_INVALID_ARG;
        }
        else
        {
            /*Codes_SRS_SCHEMA_02_109: [ Schema_AddModelMethodArgument shall allocate memory for the new argument. ]*/
            SCHEMA_METHOD_ARGUMENT_HANDLE_DATA* argument = malloc(sizeof(SCHEMA_METHOD_ARGUMENT_HANDLE_DATA));
            if (argument == NULL)
            {
                /*Codes_SRS_SCHEMA_02_113: [ If any of the above operations fails, then Schema_AddModelMethodArgument shall fail and return SCHEMA_ERROR. ]*/
                LogError("failure to malloc");
                result = SCHEMA_ERROR;
            }
            else
            {
                /*Codes_SRS_SCHEMA_02_110: [ Schema_AddModelMethodArgument shall clone methodHandle. ]*/
                if (mallocAndStrcpy_s(&argument->Name, argumentName) != 0)
                {
                    /*Codes_SRS_SCHEMA_02_113: [ If any of the above operations fails, then Schema_AddModelMethodArgument shall fail and return SCHEMA_ERROR. ]*/
                    LogError("failure in mallocAndStrcpy_s");
                    free(argument);
                    result = SCHEMA_ERROR;
                }
                else
                {
                    /*Codes_SRS_SCHEMA_02_111: [ Schema_AddModelMethodArgument shall clone argumentType. ]*/
                    if (mallocAndStrcpy_s(&argument->Type, argumentType) != 0)
                    {
                        /*Codes_SRS_SCHEMA_02_113: [ If any of the above operations fails, then Schema_AddModelMethodArgument shall fail and return SCHEMA_ERROR. ]*/
                        LogError("failure in mallocAndStrcpy_s");
                        free(argument->Name);
                        free(argument);
                        result = SCHEMA_ERROR;
                    }
                    else
                    {
                        /*Codes_SRS_SCHEMA_02_112: [ Schema_AddModelMethodArgument shall add the created argument to the method's list of arguments. ]*/
                        if (VECTOR_push_back(methodHandle->methodArguments, &argument, 1) != 0)
                        {
                            /*Codes_SRS_SCHEMA_02_113: [ If any of the above operations fails, then Schema_AddModelMethodArgument shall fail and return SCHEMA_ERROR. ]*/
                            LogError("failure in VECTOR_push_back");
                            free(argument->Name);
                            free(argument->Type);
                            free(argument);
                            result = SCHEMA_ERROR;
                        }
                        else
                        {
                            /*Codes_SRS_SCHEMA_02_114: [ Otherwise, Schema_AddModelMethodArgument shall succeed and return SCHEMA_OK. ]*/
                            result = SCHEMA_OK;
                        }
                    }
                }
            }
        }
    }
    return result;
}

SCHEMA_PROPERTY_HANDLE Schema_GetModelPropertyByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName)
{
    SCHEMA_PROPERTY_HANDLE result;

    /* Codes_SRS_SCHEMA_99_038:[Schema_GetModelPropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
    if ((modelTypeHandle == NULL) ||
        (propertyName == NULL))
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        size_t i;
        SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;

        /* Codes_SRS_SCHEMA_99_036:[Schema_GetModelPropertyByName shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the model type identified by modelTypeHandle and matching the propertyName argument value.] */
        for (i = 0; i < modelType->PropertyCount; i++)
        {
            SCHEMA_PROPERTY_HANDLE_DATA* modelProperty = (SCHEMA_PROPERTY_HANDLE_DATA*)modelType->Properties[i];
            if (strcmp(modelProperty->PropertyName, propertyName) == 0)
            {
                break;
            }
        }

        if (i == modelType->PropertyCount)
        {
            /* Codes_SRS_SCHEMA_99_038:[Schema_GetModelPropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
            result = NULL;
            LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ELEMENT_NOT_FOUND));
        }
        else
        {
            result = (SCHEMA_PROPERTY_HANDLE)(modelType->Properties[i]);
        }
    }

    return result;
}

SCHEMA_RESULT Schema_GetModelPropertyCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* propertyCount)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_092: [Schema_GetModelPropertyCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL.] */
    if ((modelTypeHandle == NULL) ||
        (propertyCount == NULL))
    {
        result = SCHEMA_INVALID_ARG;
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_089: [Schema_GetModelPropertyCount shall provide the number of properties defined in the model type identified by modelTypeHandle.] */
        SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;

        /* Codes_SRS_SCHEMA_99_090: [The count shall be provided via the out argument propertyCount.]*/
        *propertyCount = modelType->PropertyCount;

        /* Codes_SRS_SCHEMA_99_091: [On success, Schema_GetModelPropertyCount shall return SCHEMA_OK.] */
        result = SCHEMA_OK;
    }

    return result;
}

SCHEMA_RESULT Schema_GetModelReportedPropertyCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* reportedPropertyCount)
{
    SCHEMA_RESULT result;
    /*Codes_SRS_SCHEMA_02_008: [ If parameter modelTypeHandle is NULL then Schema_GetModelReportedPropertyCount shall fail and return SCHEMA_INVALID_ARG. ]*/
    /*Codes_SRS_SCHEMA_02_009: [ If parameter reportedPropertyCount is NULL then Schema_GetModelReportedPropertyCount shall fail and return SCHEMA_INVALID_ARG. ]*/
    if (
        (modelTypeHandle == NULL)||
        (reportedPropertyCount==NULL)
        )
    {
        LogError("SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, size_t* reportedPropertyCount=%p", modelTypeHandle, reportedPropertyCount);
        result = SCHEMA_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_010: [ Schema_GetModelReportedPropertyCount shall provide in reportedPropertyCount the number of reported properties and return SCHEMA_OK. ]*/
        SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        *reportedPropertyCount = VECTOR_size(modelType->reportedProperties);
        result = SCHEMA_OK;
    }
    return result;
}

SCHEMA_REPORTED_PROPERTY_HANDLE Schema_GetModelReportedPropertyByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* reportedPropertyName)
{
    SCHEMA_REPORTED_PROPERTY_HANDLE result;
    /*Codes_SRS_SCHEMA_02_011: [ If argument modelTypeHandle is NULL then Schema_GetModelReportedPropertyByName shall fail and return NULL. ]*/
    /*Codes_SRS_SCHEMA_02_012: [ If argument reportedPropertyName is NULL then Schema_GetModelReportedPropertyByName shall fail and return NULL. ]*/
    if(
        (modelTypeHandle == NULL) ||
        (reportedPropertyName == NULL)
        )
    {
        LogError("invalid argument SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, const char* reportedPropertyName=%p", modelTypeHandle, reportedPropertyName);
        result = NULL;
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        /*Codes_SRS_SCHEMA_02_013: [ If reported property by the name reportedPropertyName exists then Schema_GetModelReportedPropertyByName shall succeed and return a non-NULL value. ]*/
        /*Codes_SRS_SCHEMA_02_014: [ Otherwise Schema_GetModelReportedPropertyByName shall fail and return NULL. ]*/
        if((result = VECTOR_find_if(modelType->reportedProperties, reportedPropertyExists, reportedPropertyName))==NULL)
        {
            LogError("a reported property with name \"%s\" does not exist", reportedPropertyName);
        }
        else
        {
            /*return as is*/
        }
    }
    return result;
}

SCHEMA_REPORTED_PROPERTY_HANDLE Schema_GetModelReportedPropertyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index)
{
    SCHEMA_REPORTED_PROPERTY_HANDLE result;
    /*Codes_SRS_SCHEMA_02_015: [ If argument modelTypeHandle is NULL then Schema_GetModelReportedPropertyByIndex shall fail and return NULL. ]*/
    if (modelTypeHandle == NULL)
    {
        LogError("invalid argument SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, size_t index=%lu", modelTypeHandle, (unsigned long)index);
        result = NULL;
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;

        /*Codes_SRS_SCHEMA_02_016: [ If a reported property with index equal to index exists then Schema_GetModelReportedPropertyByIndex shall succeed and return the non-NULL handle of that REPORTED_PROPERTY. ]*/
        /*Codes_SRS_SCHEMA_02_017: [ Otherwise Schema_GetModelReportedPropertyByIndex shall fail and return NULL. ]*/
        if ((result = VECTOR_element(modelType->reportedProperties, index)) == NULL)
        {
            LogError("index %lu is invalid", (unsigned long)index);
        }
        else
        {
            /*return as is*/
        }
    }
    return result;
}

SCHEMA_PROPERTY_HANDLE Schema_GetModelPropertyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index)
{
    SCHEMA_PROPERTY_HANDLE result;
    SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;

    /* Codes_SRS_SCHEMA_99_094: [Schema_GetModelProperty shall return NULL if the index specified is outside the valid range or if modelTypeHandle argument is NULL.] */
    if ((modelTypeHandle == NULL) ||
        (index >= modelType->PropertyCount))
    {
        result = NULL;
        LogError("(Error code: %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Tests_SRS_SCHEMA_99_093: [Schema_GetModelProperty shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the model type identified by modelTypeHandle and matching the index number provided by the index argument.] */
        /* Codes_SRS_SCHEMA_99_097: [index is zero based, and the order in which actions were added shall be the index in which they will be retrieved.] */
        result = modelType->Properties[index];
    }

    return result;
}

SCHEMA_ACTION_HANDLE Schema_GetModelActionByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* actionName)
{
    SCHEMA_ACTION_HANDLE result;

    /* Codes_SRS_SCHEMA_99_041:[Schema_GetModelActionByName shall return NULL if unable to find a matching action, if any of the arguments are NULL.] */
    if ((modelTypeHandle == NULL) ||
        (actionName == NULL))
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        size_t i;
        SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;

        /* Codes_SRS_SCHEMA_99_040:[Schema_GetModelActionByName shall return a non-NULL SCHEMA_ACTION_HANDLE corresponding to the model type identified by modelTypeHandle and matching the actionName argument value.] */
        for (i = 0; i < modelType->ActionCount; i++)
        {
            SCHEMA_ACTION_HANDLE_DATA* modelAction = (SCHEMA_ACTION_HANDLE_DATA*)modelType->Actions[i];
            if (strcmp(modelAction->ActionName, actionName) == 0)
            {
                break;
            }
        }

        if (i == modelType->ActionCount)
        {
            /* Codes_SRS_SCHEMA_99_041:[Schema_GetModelActionByName shall return NULL if unable to find a matching action, if any of the arguments are NULL.] */
            result = NULL;
            LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ELEMENT_NOT_FOUND));
        }
        else
        {
            result = modelType->Actions[i];
        }
    }

    return result;
}

static bool matchModelMethod(const void* element, const void* value)
{
    /*element is a pointer to SCHEMA_METHOD_HANDLE_DATA*/
    const SCHEMA_METHOD_HANDLE* decodedElement = (const SCHEMA_METHOD_HANDLE* )element;
    const char* name = (const char*)value;
    return (strcmp((*decodedElement)->methodName, name) == 0);
}

SCHEMA_METHOD_HANDLE Schema_GetModelMethodByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* methodName)
{
    SCHEMA_METHOD_HANDLE result;

    /*Codes_SRS_SCHEMA_02_115: [ If modelTypeHandle is NULL then Schema_GetModelMethodByName shall fail and return NULL. ]*/
    /*Codes_SRS_SCHEMA_02_116: [ If methodName is NULL then Schema_GetModelMethodByName shall fail and return NULL. ]*/
    if ((modelTypeHandle == NULL) ||
        (methodName == NULL))
    {
        result = NULL;
        LogError("invalid arguments SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, const char* methodName=%p", modelTypeHandle, methodName);
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_117: [ If a method with the name methodName exists then Schema_GetModelMethodByName shall succeed and returns its handle. ]*/
        SCHEMA_METHOD_HANDLE* found = VECTOR_find_if(modelTypeHandle->methods, matchModelMethod, methodName);
        if (found == NULL)
        {
            /*Codes_SRS_SCHEMA_02_118: [ Otherwise, Schema_GetModelMethodByName shall fail and return NULL. ]*/
            LogError("no such method by name = %s", methodName);
            result = NULL;
        }
        else
        {
            result = *found;
        }
    }

    return result;
}

SCHEMA_RESULT Schema_GetModelActionCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* actionCount)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_045:[If any of the modelTypeHandle or actionCount arguments is NULL, Schema_GetModelActionCount shall return SCHEMA_INVALID_ARG.] */
    if ((modelTypeHandle == NULL) ||
        (actionCount == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result=%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_042:[Schema_GetModelActionCount shall provide the total number of actions defined in a model type identified by the modelTypeHandle.] */
        SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;

        /* Codes_SRS_SCHEMA_99_043:[The count shall be provided via the out argument actionCount.] */
        *actionCount = modelType->ActionCount;

        /* Codes_SRS_SCHEMA_99_044:[On success, Schema_GetModelActionCount shall return SCHEMA_OK.] */
        result = SCHEMA_OK;
    }

    return result;
}

SCHEMA_ACTION_HANDLE Schema_GetModelActionByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index)
{
    SCHEMA_ACTION_HANDLE result;
    SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;

    /* Codes_SRS_SCHEMA_99_048:[Schema_GetModelAction shall return NULL if the index specified is outside the valid range or if modelTypeHandle argument is NULL.] */
    if ((modelType == NULL) ||
        (index >= modelType->ActionCount))
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_047:[Schema_GetModelAction shall return a non-NULL SCHEMA_ACTION_HANDLE corresponding to the model type identified by modelTypeHandle and matching the index number provided by the index argument.] */
        /* Codes_SRS_SCHEMA_99_096: [index is zero based and the order in which actions were added shall be the index in which they will be retrieved.] */
        result = modelType->Actions[index];
    }

    return result;
}

const char* Schema_GetModelActionName(SCHEMA_ACTION_HANDLE actionHandle)
{
    const char* result;

    /* Codes_SRS_SCHEMA_99_050:[If the actionHandle is NULL, Schema_GetModelActionName shall return NULL.] */
    if (actionHandle == NULL)
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        SCHEMA_ACTION_HANDLE_DATA* action = (SCHEMA_ACTION_HANDLE_DATA*)actionHandle;
        /* Codes_SRS_SCHEMA_99_049:[Schema_GetModelActionName shall return the action name for a given action handle.] */
        result = action->ActionName;
    }

    return result;
}

SCHEMA_RESULT Schema_GetModelActionArgumentCount(SCHEMA_ACTION_HANDLE actionHandle, size_t* argumentCount)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_054:[If any argument is NULL, Schema_GetModelActionArgumentCount shall return SCHEMA_INVALID_ARG.] */
    if ((actionHandle == NULL) ||
        (argumentCount == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result=%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        SCHEMA_ACTION_HANDLE_DATA* modelAction = (SCHEMA_ACTION_HANDLE_DATA*)actionHandle;

        /* Codes_SRS_SCHEMA_99_051:[Schema_GetModelActionArgumentCount shall provide the number of arguments for a specific schema action identified by actionHandle.] */
        /* Codes_SRS_SCHEMA_99_052:[The argument count shall be provided via the out argument argumentCount.] */
        *argumentCount = modelAction->ArgumentCount;

        /* Codes_SRS_SCHEMA_99_053:[On success, Schema_GetModelActionArgumentCount shall return SCHEMA_OK.] */
        result = SCHEMA_OK;
    }

    return result;
}

SCHEMA_RESULT Schema_GetModelMethodArgumentCount(SCHEMA_METHOD_HANDLE methodHandle, size_t* argumentCount)
{
    SCHEMA_RESULT result;

    /*Codes_SRS_SCHEMA_02_119: [ If methodHandle is NULL then Schema_GetModelMethodArgumentCount shall fail and return SCHEMA_INVALID_ARG. ]*/
    /*Codes_SRS_SCHEMA_02_120: [ If argumentCount is NULL then Schema_GetModelMethodArgumentCount shall fail and return SCHEMA_INVALID_ARG. ]*/
    if ((methodHandle == NULL) ||
        (argumentCount == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result=%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_121: [ Otherwise, Schema_GetModelMethodArgumentCount shall succeed, return in argumentCount the number of arguments for the method and return SCHEMA_OK. ]*/
        *argumentCount = VECTOR_size(methodHandle->methodArguments);
        result = SCHEMA_OK;
    }

    return result;
}


SCHEMA_ACTION_ARGUMENT_HANDLE Schema_GetModelActionArgumentByName(SCHEMA_ACTION_HANDLE actionHandle, const char* actionArgumentName)
{
    SCHEMA_ACTION_ARGUMENT_HANDLE result;

    /* Codes_SRS_SCHEMA_99_118: [Schema_GetModelActionArgumentByName shall return NULL if unable to find a matching argument or if any of the arguments are NULL.] */
    if ((actionHandle == NULL) ||
        (actionArgumentName == NULL))
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_118: [Schema_GetModelActionArgumentByName shall return NULL if unable to find a matching argument or if any of the arguments are NULL.] */
        SCHEMA_ACTION_HANDLE_DATA* modelAction = (SCHEMA_ACTION_HANDLE_DATA*)actionHandle;
        size_t i;

        for (i = 0; i < modelAction->ArgumentCount; i++)
        {
            SCHEMA_ACTION_ARGUMENT_HANDLE_DATA* actionArgument = (SCHEMA_ACTION_ARGUMENT_HANDLE_DATA*)modelAction->ArgumentHandles[i];
            if (strcmp(actionArgument->Name, actionArgumentName) == 0)
            {
                break;
            }
        }

        if (i == modelAction->ArgumentCount)
        {
            /* Codes_SRS_SCHEMA_99_118: [Schema_GetModelActionArgumentByName shall return NULL if unable to find a matching argument or if any of the arguments are NULL.] */
            result = NULL;
            LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ELEMENT_NOT_FOUND));
        }
        else
        {
            /* Codes_SRS_SCHEMA_99_117: [Schema_GetModelActionArgumentByName shall return a non-NULL handle corresponding to an action argument identified by the actionHandle and actionArgumentName.] */
            result = modelAction->ArgumentHandles[i];
        }
    }

    return result;
}

SCHEMA_ACTION_ARGUMENT_HANDLE Schema_GetModelActionArgumentByIndex(SCHEMA_ACTION_HANDLE actionHandle, size_t argumentIndex)
{
    SCHEMA_ACTION_ARGUMENT_HANDLE result;
    SCHEMA_ACTION_HANDLE_DATA* modelAction = (SCHEMA_ACTION_HANDLE_DATA*)actionHandle;

    /* Codes_SRS_SCHEMA_99_056:[Schema_GetModelActionArgument shall return NULL if the index specified is outside the valid range or if the actionHandle argument is NULL.] */
    if ((actionHandle == NULL) ||
        (argumentIndex >= modelAction->ArgumentCount))
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_055:[Schema_GetModelActionArgument shall return a non-NULL SCHEMA_ACTION_ARGUMENT_HANDLE corresponding to the action type identified by actionHandle and matching the index number provided by the argumentIndex argument.] */
        result = modelAction->ArgumentHandles[argumentIndex];
    }

    return result;
}

SCHEMA_METHOD_ARGUMENT_HANDLE Schema_GetModelMethodArgumentByIndex(SCHEMA_METHOD_HANDLE methodHandle, size_t argumentIndex)
{
    SCHEMA_METHOD_ARGUMENT_HANDLE result;

    /*Codes_SRS_SCHEMA_02_122: [ If methodHandle is NULL then Schema_GetModelMethodArgumentByIndex shall fail and return NULL. ]*/
    if (methodHandle == NULL)
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_123: [ If argumentIndex does not exist then Schema_GetModelMethodArgumentByIndex shall fail and return NULL. ]*/
        SCHEMA_METHOD_ARGUMENT_HANDLE *temp = VECTOR_element(methodHandle->methodArguments, argumentIndex);
        if (temp == NULL)
        {
            result = NULL;
        }
        else
        {
            /*Codes_SRS_SCHEMA_02_124: [ Otherwise, Schema_GetModelMethodArgumentByIndex shall succeed and return a non-NULL value. ]*/
            result = *temp;
        }
    }

    return result;
}


const char* Schema_GetActionArgumentName(SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle)
{
    const char* result;
    /* Codes_SRS_SCHEMA_99_114: [Schema_GetActionArgumentName shall return NULL if actionArgumentHandle is NULL.] */
    if (actionArgumentHandle == NULL)
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_113: [Schema_GetActionArgumentName shall return the argument name identified by the actionArgumentHandle.] */
        SCHEMA_ACTION_ARGUMENT_HANDLE_DATA* actionArgument = (SCHEMA_ACTION_ARGUMENT_HANDLE_DATA*)actionArgumentHandle;
        result = actionArgument->Name;
    }
    return result;
}

const char* Schema_GetMethodArgumentName(SCHEMA_METHOD_ARGUMENT_HANDLE methodArgumentHandle)
{
    const char* result;
    /*Codes_SRS_SCHEMA_02_125: [ If methodArgumentHandle is NULL then Schema_GetMethodArgumentName shall fail and return NULL. ]*/
    if (methodArgumentHandle == NULL)
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_126: [ Otherwise, Schema_GetMethodArgumentName shall succeed and return a non-NULL value. ]*/
        result = methodArgumentHandle->Name;
    }
    return result;
}


const char* Schema_GetActionArgumentType(SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle)
{
    const char* result;
    /* Codes_SRS_SCHEMA_99_116: [Schema_GetActionArgumentType shall return NULL if actionArgumentHandle is NULL.] */
    if (actionArgumentHandle == NULL)
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_115: [Schema_GetActionArgumentType shall return the argument type identified by the actionArgumentHandle.] */
        SCHEMA_ACTION_ARGUMENT_HANDLE_DATA* actionArgument = (SCHEMA_ACTION_ARGUMENT_HANDLE_DATA*)actionArgumentHandle;
        result = actionArgument->Type;
    }
    return result;
}

const char* Schema_GetMethodArgumentType(SCHEMA_METHOD_ARGUMENT_HANDLE methodArgumentHandle)
{
    const char* result;
    /*Codes_SRS_SCHEMA_02_127: [ If methodArgumentHandle is NULL then Schema_GetMethodArgumentType shall fail and return NULL. ]*/
    if (methodArgumentHandle == NULL)
    {
        result = NULL;
        LogError("invalid argument SCHEMA_METHOD_ARGUMENT_HANDLE methodArgumentHandle=%p", methodArgumentHandle);
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_128: [ Otherwise, Schema_GetMethodArgumentType shall succeed and return a non-NULL value. ]*/
        result = methodArgumentHandle->Type;
    }
    return result;
}


SCHEMA_STRUCT_TYPE_HANDLE Schema_CreateStructType(SCHEMA_HANDLE schemaHandle, const char* typeName)
{
    SCHEMA_STRUCT_TYPE_HANDLE result;
    SCHEMA_HANDLE_DATA* schema = (SCHEMA_HANDLE_DATA*)schemaHandle;

    /* Codes_SRS_SCHEMA_99_060:[If any of the arguments is NULL, Schema_CreateStructType shall return NULL.] */
    if ((schema == NULL) ||
        (typeName == NULL))
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        SCHEMA_STRUCT_TYPE_HANDLE_DATA* structType;
        size_t i;

        /* Codes_SRS_SCHEMA_99_061:[If a struct type with the same name already exists, Schema_CreateStructType shall return NULL.] */
        for (i = 0; i < schema->StructTypeCount; i++)
        {
            structType = (SCHEMA_STRUCT_TYPE_HANDLE_DATA*)schema->StructTypes[i];
            if (strcmp(structType->Name, typeName) == 0)
            {
                break;
            }
        }

        if (i < schema->StructTypeCount)
        {
            result = NULL;
            LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_DUPLICATE_ELEMENT));
        }
        else
        {
            SCHEMA_STRUCT_TYPE_HANDLE* newStructTypes = (SCHEMA_STRUCT_TYPE_HANDLE*)realloc(schema->StructTypes, sizeof(SCHEMA_STRUCT_TYPE_HANDLE) * (schema->StructTypeCount + 1));
            if (newStructTypes == NULL)
            {
                /* Codes_SRS_SCHEMA_99_066:[On any other error, Schema_CreateStructType shall return NULL.] */
                result = NULL;
                LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
            }
            else
            {
                schema->StructTypes = newStructTypes;
                if ((structType = (SCHEMA_STRUCT_TYPE_HANDLE_DATA*)calloc(1, sizeof(SCHEMA_STRUCT_TYPE_HANDLE_DATA))) == NULL)
                {
                    /* Codes_SRS_SCHEMA_99_066:[On any other error, Schema_CreateStructType shall return NULL.] */
                    result = NULL;
                    LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                }
                else if (mallocAndStrcpy_s((char**)&structType->Name, typeName) != 0)
                {
                    /* Codes_SRS_SCHEMA_99_066:[On any other error, Schema_CreateStructType shall return NULL.] */
                    result = NULL;
                    free(structType);
                    LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                }
                else
                {
                    /* Codes_SRS_SCHEMA_99_057:[Schema_CreateStructType shall create a new struct type and return a handle to it.] */
                    schema->StructTypes[schema->StructTypeCount] = structType;
                    schema->StructTypeCount++;
                    structType->PropertyCount = 0;
                    structType->Properties = NULL;

                    /* Codes_SRS_SCHEMA_99_058:[On success, a non-NULL handle shall be returned.] */
                    result = (SCHEMA_STRUCT_TYPE_HANDLE)structType;
                }

                /* If possible, reduce the memory of over allocation */
                if (result == NULL)
                {
                    if (schema->StructTypeCount > 0)
                    {
                        SCHEMA_STRUCT_TYPE_HANDLE *oldStructTypes = (SCHEMA_STRUCT_TYPE_HANDLE *)realloc(schema->StructTypes, sizeof(SCHEMA_STRUCT_TYPE_HANDLE) * schema->StructTypeCount);
                        if (oldStructTypes == NULL)
                        {
                            result = NULL;
                            LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                        }
                        else
                        {
                            schema->StructTypes = oldStructTypes;
                        }
                    }
                    else
                    {
                        schema->StructTypes = NULL;
                    }
                }
            }
        }
    }

    return result;
}

const char* Schema_GetStructTypeName(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle)
{
    const char* result;

    /* Codes_SRS_SCHEMA_99_136: [If structTypeHandle is NULL, Schema_GetStructTypeName shall return NULL.] */
    if (structTypeHandle == NULL)
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_135: [Schema_GetStructTypeName shall return the name of a struct type identified by the structTypeHandle argument.] */
        result = ((SCHEMA_STRUCT_TYPE_HANDLE_DATA*)structTypeHandle)->Name;
    }

    return result;
}

SCHEMA_RESULT Schema_GetStructTypeCount(SCHEMA_HANDLE schemaHandle, size_t* structTypeCount)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_140: [Schema_GetStructTypeCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL.] */
    if ((schemaHandle == NULL) ||
        (structTypeCount == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_137: [Schema_GetStructTypeCount shall provide the number of structs defined in the schema identified by schemaHandle.] */
        SCHEMA_HANDLE_DATA* schema = (SCHEMA_HANDLE_DATA*)schemaHandle;
        /* Codes_SRS_SCHEMA_99_138: [The count shall be provided via the out argument structTypeCount.] */
        *structTypeCount = schema->StructTypeCount;
        /* Codes_SRS_SCHEMA_99_139: [On success, Schema_GetStructTypeCount shall return SCHEMA_OK.] */
        result = SCHEMA_OK;
    }

    return result;
}

SCHEMA_STRUCT_TYPE_HANDLE Schema_GetStructTypeByName(SCHEMA_HANDLE schemaHandle, const char* name)
{
    SCHEMA_STRUCT_TYPE_HANDLE result;
    SCHEMA_HANDLE_DATA* schema = (SCHEMA_HANDLE_DATA*)schemaHandle;

    /* Codes_SRS_SCHEMA_99_069:[Schema_GetStructTypeByName shall return NULL if unable to find a matching struct or if any of the arguments are NULL.] */
    if ((schemaHandle == NULL) ||
        (name == NULL))
    {
        result = NULL;
        LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        size_t i;

        /* Codes_SRS_SCHEMA_99_068:[Schema_GetStructTypeByName shall return a non-NULL handle corresponding to the struct type identified by the structTypeName in the schemaHandle schema.] */
        for (i = 0; i < schema->StructTypeCount; i++)
        {
            SCHEMA_STRUCT_TYPE_HANDLE_DATA* structType = (SCHEMA_STRUCT_TYPE_HANDLE_DATA*)schema->StructTypes[i];
            if (strcmp(structType->Name, name) == 0)
            {
                break;
            }
        }

        if (i == schema->StructTypeCount)
        {
            /* Codes_SRS_SCHEMA_99_069:[Schema_GetStructTypeByName shall return NULL if unable to find a matching struct or if any of the arguments are NULL.] */
            result = NULL;
            LogError("(Error code:%s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ELEMENT_NOT_FOUND));
        }
        else
        {
            result = schema->StructTypes[i];
        }
    }

    return result;
}

SCHEMA_STRUCT_TYPE_HANDLE Schema_GetStructTypeByIndex(SCHEMA_HANDLE schemaHandle, size_t index)
{
    SCHEMA_STRUCT_TYPE_HANDLE result;
    SCHEMA_HANDLE_DATA* schema = (SCHEMA_HANDLE_DATA*)schemaHandle;

    /* Codes_SRS_SCHEMA_99_143: [Schema_GetStructTypeByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL.] */
    /* Codes_SRS_SCHEMA_99_142: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
    if ((schemaHandle == NULL) ||
        (index >= schema->StructTypeCount))
    {
        result = NULL;
        LogError("(Error code: %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {

        /* Codes_SRS_SCHEMA_99_141: [Schema_GetStructTypeByIndex shall return a non-NULL SCHEMA_STRUCT_TYPE_HANDLE corresponding to the struct type identified by schemaHandle and matching the index number provided by the index argument.] */
        /* Codes_SRS_SCHEMA_99_142: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
        result = schema->StructTypes[index];
    }
    return result;
}

SCHEMA_RESULT Schema_AddStructTypeProperty(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, const char* propertyName, const char* propertyType)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_072:[If any of the arguments is NULL, Schema_AddStructTypeProperty shall return SCHEMA_INVALID_ARG.] */
    if ((structTypeHandle == NULL) ||
        (propertyName == NULL) ||
        (propertyType == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        size_t i;
        SCHEMA_STRUCT_TYPE_HANDLE_DATA* structType = (SCHEMA_STRUCT_TYPE_HANDLE_DATA*)structTypeHandle;

        /* Codes_SRS_SCHEMA_99_074:[The property name shall be unique per struct type, if the same property name is added twice to a struct type, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
        for (i = 0; i < structType->PropertyCount; i++)
        {
            SCHEMA_PROPERTY_HANDLE_DATA* property = (SCHEMA_PROPERTY_HANDLE_DATA*)structType->Properties[i];
            if (strcmp(property->PropertyName, propertyName) == 0)
            {
                break;
            }
        }

        if (i < structType->PropertyCount)
        {
            result = SCHEMA_DUPLICATE_ELEMENT;
            LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
        }
        else
        {
            SCHEMA_PROPERTY_HANDLE* newProperties = (SCHEMA_PROPERTY_HANDLE*)realloc(structType->Properties, sizeof(SCHEMA_PROPERTY_HANDLE) * (structType->PropertyCount + 1));
            if (newProperties == NULL)
            {
                result = SCHEMA_ERROR;
                LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
            }
            else
            {
                SCHEMA_PROPERTY_HANDLE_DATA* newProperty;

                structType->Properties = newProperties;
                if ((newProperty = (SCHEMA_PROPERTY_HANDLE_DATA*)malloc(sizeof(SCHEMA_PROPERTY_HANDLE_DATA))) == NULL)
                {
                    result = SCHEMA_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
                }
                else
                {
                    if (mallocAndStrcpy_s((char**)&newProperty->PropertyName, propertyName) != 0)
                    {
                        free(newProperty);
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else if (mallocAndStrcpy_s((char**)&newProperty->PropertyType, propertyType) != 0)
                    {
                        free((void*)newProperty->PropertyName);
                        free(newProperty);
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else
                    {
                        /* Codes_SRS_SCHEMA_99_070:[Schema_AddStructTypeProperty shall add one property to the struct type identified by structTypeHandle.] */
                        structType->Properties[structType->PropertyCount] = (SCHEMA_PROPERTY_HANDLE)newProperty;
                        structType->PropertyCount++;

                        /* Codes_SRS_SCHEMA_99_071:[On success, Schema_AddStructTypeProperty shall return SCHEMA_OK.] */
                        result = SCHEMA_OK;
                    }
                }

                /* If possible, reduce the memory of over allocation */
                if (result != SCHEMA_OK)
                {
                    if (structType->PropertyCount > 0)
                    {
                        SCHEMA_PROPERTY_HANDLE *oldProperties = (SCHEMA_PROPERTY_HANDLE *)realloc(structType->Properties, sizeof(SCHEMA_PROPERTY_HANDLE) * structType->PropertyCount);
                        if (oldProperties == NULL)
                        {
                            result = SCHEMA_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
                        }
                        else
                        {
                            structType->Properties = oldProperties;
                        }
                    }
                    else
                    {
                        structType->Properties = NULL;
                    }
                }
            }
        }
    }

    return result;
}

SCHEMA_PROPERTY_HANDLE Schema_GetStructTypePropertyByName(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, const char* propertyName)
{
    SCHEMA_PROPERTY_HANDLE result;

    /* Codes_SRS_SCHEMA_99_076:[Schema_GetStructTypePropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
    if ((structTypeHandle == NULL) ||
        (propertyName == NULL))
    {
        result = NULL;
        LogError("(Error code: %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        size_t i;
        SCHEMA_STRUCT_TYPE_HANDLE_DATA* structType = (SCHEMA_STRUCT_TYPE_HANDLE_DATA*)structTypeHandle;

        for (i = 0; i < structType->PropertyCount; i++)
        {
            SCHEMA_PROPERTY_HANDLE_DATA* modelProperty = (SCHEMA_PROPERTY_HANDLE_DATA*)structType->Properties[i];
            if (strcmp(modelProperty->PropertyName, propertyName) == 0)
            {
                break;
            }
        }

        /* Codes_SRS_SCHEMA_99_076:[Schema_GetStructTypePropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
        if (i == structType->PropertyCount)
        {
            result = NULL;
            LogError("(Error code: %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ELEMENT_NOT_FOUND));
        }
        /* Codes_SRS_SCHEMA_99_075:[Schema_GetStructTypePropertyByName shall return a non-NULL handle corresponding to a property identified by the structTypeHandle and propertyName.] */
        else
        {
            result = structType->Properties[i];
        }
    }

    return result;
}

SCHEMA_RESULT Schema_GetStructTypePropertyCount(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, size_t* propertyCount)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_079: [Schema_GetStructTypePropertyCount shall return SCHEMA_INVALID_ARG if any of the structlTypeHandle or propertyCount arguments is NULL.] */
    if ((structTypeHandle == NULL) ||
        (propertyCount == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        SCHEMA_STRUCT_TYPE_HANDLE_DATA* structType = (SCHEMA_STRUCT_TYPE_HANDLE_DATA*)structTypeHandle;

        /* Codes_SRS_SCHEMA_99_077: [Schema_GetStructTypePropertyCount shall provide the total number of properties defined in a struct type identified by structTypeHandle. The value is provided via the out argument propertyCount.] */
        /* Codes_SRS_SCHEMA_99_081: [The count shall be provided via the out argument propertyCount.] */
        *propertyCount = structType->PropertyCount;

        /* Codes_SRS_SCHEMA_99_078: [On success, Schema_ GetStructTypePropertyCount shall return SCHEMA_OK.] */
        result = SCHEMA_OK;
    }

    return result;
}

SCHEMA_PROPERTY_HANDLE Schema_GetStructTypePropertyByIndex(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, size_t index)
{
    SCHEMA_PROPERTY_HANDLE result;
    SCHEMA_STRUCT_TYPE_HANDLE_DATA* structType = (SCHEMA_STRUCT_TYPE_HANDLE_DATA*)structTypeHandle;

    /* Codes_SRS_SCHEMA_99_083: [Schema_ GetStructTypeProperty shall return NULL if the index specified is outside the valid range, if structTypeHandle argument is NULL] */
    if ((structTypeHandle == NULL) ||
        (index >= structType->PropertyCount))
    {
        result = NULL;
        LogError("(Error code: %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_082: [Schema_GetStructTypeProperty shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the struct type identified by strutTypeHandle and matching the index number provided by the index argument.] */
        /* Codes_SRS_SCHEMA_99_098: [index is zero based and the order in which actions were added shall be the index in which they will be retrieved.] */
        result = structType->Properties[index];
    }

    return result;
}

const char* Schema_GetPropertyName(SCHEMA_PROPERTY_HANDLE propertyHandle)
{
    const char* result;

    /* Codes_SRS_SCHEMA_99_086: [If propertyHandle is NULL, Schema_GetPropertyName shall return NULL.] */
    if (propertyHandle == NULL)
    {
        result = NULL;
        LogError("(Error code: %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        SCHEMA_PROPERTY_HANDLE_DATA* propertyType = (SCHEMA_PROPERTY_HANDLE_DATA*)propertyHandle;

        /* Codes_SRS_SCHEMA_99_085: [Schema_GetPropertyName shall return the property name identified by the propertyHandle.] */
        result = propertyType->PropertyName;
    }

    return result;
}

const char* Schema_GetPropertyType(SCHEMA_PROPERTY_HANDLE propertyHandle)
{
    const char* result;

    /* Codes_SRS_SCHEMA_99_088: [If propertyHandle is NULL, Schema_GetPropertyType shall return NULL.] */
    if (propertyHandle == NULL)
    {
        result = NULL;
        LogError("(Error code: %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        SCHEMA_PROPERTY_HANDLE_DATA* modelProperty = (SCHEMA_PROPERTY_HANDLE_DATA*)propertyHandle;

        /* Codes_SRS_SCHEMA_99_087: [Schema_GetPropertyType shall return the property type identified by the propertyHandle.] */
        result = modelProperty->PropertyType;
    }

    return result;
}

SCHEMA_RESULT Schema_GetModelCount(SCHEMA_HANDLE schemaHandle, size_t* modelCount)
{
    SCHEMA_RESULT result;
    /* Codes_SRS_SCHEMA_99_123: [Schema_GetModelCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL.] */
    if ((schemaHandle == NULL) ||
        (modelCount == NULL) )
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_120: [Schema_GetModelCount shall provide the number of models defined in the schema identified by schemaHandle.] */
        SCHEMA_HANDLE_DATA* schema = (SCHEMA_HANDLE_DATA*)schemaHandle;
        /* Codes_SRS_SCHEMA_99_121: [The count shall be provided via the out argument modelCount.] */
        *modelCount = schema->ModelTypeCount;
        /* Codes_SRS_SCHEMA_99_122: [On success, Schema_GetModelCount shall return SCHEMA_OK.] */
        result = SCHEMA_OK;
    }
    return result;
}

SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelByName(SCHEMA_HANDLE schemaHandle, const char* modelName)
{
    SCHEMA_MODEL_TYPE_HANDLE result;

    /* Codes_SRS_SCHEMA_99_125: [Schema_GetModelByName shall return NULL if unable to find a matching model, or if any of the arguments are NULL.] */
    if ((schemaHandle == NULL) ||
        (modelName == NULL))
    {
        result = NULL;
        LogError("(Error code: %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_124: [Schema_GetModelByName shall return a non-NULL SCHEMA_MODEL_TYPE_HANDLE corresponding to the model identified by schemaHandle and matching the modelName argument value.] */
        SCHEMA_HANDLE_DATA* schema = (SCHEMA_HANDLE_DATA*)schemaHandle;
        size_t i;
        for (i = 0; i < schema->ModelTypeCount; i++)
        {
            SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)schema->ModelTypes[i];
            if (strcmp(modelName, modelType->Name)==0)
            {
                break;
            }
        }
        if (i == schema->ModelTypeCount)
        {
            /* Codes_SRS_SCHEMA_99_125: [Schema_GetModelByName shall return NULL if unable to find a matching model, or if any of the arguments are NULL.] */
            result = NULL;
        }
        else
        {
            result = schema->ModelTypes[i];
        }
    }
    return result;
}

SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelByIndex(SCHEMA_HANDLE schemaHandle, size_t index)
{
    SCHEMA_MODEL_TYPE_HANDLE result;
    SCHEMA_HANDLE_DATA* schema = (SCHEMA_HANDLE_DATA*)schemaHandle;

    /* Codes_SRS_SCHEMA_99_128: [Schema_GetModelByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL.] */
    /* Codes_SRS_SCHEMA_99_127: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
    if ((schemaHandle == NULL) ||
        (index >= schema->ModelTypeCount))
    {
        result = NULL;
        LogError("(Error code: %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {

        /* Codes_SRS_SCHEMA_99_126: [Schema_GetModelByIndex shall return a non-NULL SCHEMA_MODEL_TYPE_HANDLE corresponding to the model identified by schemaHandle and matching the index number provided by the index argument.] */
        /* Codes_SRS_SCHEMA_99_127: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
        result = schema->ModelTypes[index];
    }
    return result;
}

/*Codes_SRS_SCHEMA_99_160: [Schema_GetModelName shall return the name of the model identified by modelTypeHandle. If the name cannot be retrieved, then NULL shall be returned.]*/
const char* Schema_GetModelName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle)
{
    const char* result;
    if (modelTypeHandle == NULL)
    {
        result = NULL;
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        result = modelType->Name;
    }
    return result;
}

/*Codes_SRS_SCHEMA_99_163: [Schema_AddModelModel shall insert an existing model, identified by the handle modelType, into the existing model identified by modelTypeHandle under a property having the name propertyName.]*/
SCHEMA_RESULT Schema_AddModelModel(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName, SCHEMA_MODEL_TYPE_HANDLE modelType, size_t offset, pfOnDesiredProperty onDesiredProperty)
{
    SCHEMA_RESULT result;
    /*Codes_SRS_SCHEMA_99_165: [If any of the parameters is NULL then Schema_AddModelModel shall return SCHEMA_INVALID_ARG.]*/
    if (
        (modelTypeHandle == NULL) ||
        (propertyName == NULL) ||
        (modelType == NULL)
        )
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(Error code: %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* parentModel = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        MODEL_IN_MODEL temp;
        temp.modelHandle = modelType;
        temp.offset = offset;
        temp.onDesiredProperty = onDesiredProperty;
        if (mallocAndStrcpy_s((char**)&(temp.propertyName), propertyName) != 0)
        {
            result = SCHEMA_ERROR;
            LogError("(Error code: %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
        }
        else if (VECTOR_push_back(parentModel->models, &temp, 1) != 0)
        {
            /*Codes_SRS_SCHEMA_99_174: [The function shall return SCHEMA_ERROR if any other error occurs.]*/
            free((void*)temp.propertyName);
            result = SCHEMA_ERROR;
            LogError("(Error code: %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
        }
        else
        {
            /*Codes_SRS_SCHEMA_99_164: [If the function succeeds, then the return value shall be SCHEMA_OK.]*/

            result = SCHEMA_OK;
        }
    }
    return result;
}


SCHEMA_RESULT Schema_GetModelModelCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* modelCount)
{
    SCHEMA_RESULT result;
    /*Codes_SRS_SCHEMA_99_169: [If any of the parameters is NULL, then the function shall return SCHEMA_INVALID_ARG.]*/
    if (
        (modelTypeHandle == NULL) ||
        (modelCount == NULL)
        )
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(Error code: %s)", MU_ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* model = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        /*Codes_SRS_SCHEMA_99_167: [Schema_GetModelModelCount shall return in parameter modelCount the number of models inserted in the model identified by parameter modelTypeHandle.]*/
        *modelCount = VECTOR_size(model->models);
        /*SRS_SCHEMA_99_168: [If the function succeeds, it shall return SCHEMA_OK.]*/
        result = SCHEMA_OK;
    }
    return result;
}

static bool matchModelName(const void* element, const void* value)
{
    MODEL_IN_MODEL* decodedElement = (MODEL_IN_MODEL*)element;
    const char* name = (const char*)value;
    return (strcmp(decodedElement->propertyName, name) == 0);
}

SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelModelByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName)
{
    SCHEMA_MODEL_TYPE_HANDLE result;
    if (
        (modelTypeHandle == NULL) ||
        (propertyName == NULL)
        )
    {
        /*Codes_SRS_SCHEMA_99_171: [If Schema_GetModelModelByName is unable to provide the handle it shall return NULL.]*/
        result = NULL;
        LogError("error SCHEMA_INVALID_ARG");
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* model = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        /*Codes_SRS_SCHEMA_99_170: [Schema_GetModelModelByName shall return a handle to the model identified by the property with the name propertyName in the model identified by the handle modelTypeHandle.]*/
        /*Codes_SRS_SCHEMA_99_171: [If Schema_GetModelModelByName is unable to provide the handle it shall return NULL.]*/
        void* temp = VECTOR_find_if(model->models, matchModelName, propertyName);
        if (temp == NULL)
        {
            LogError("specified propertyName not found (%s)", propertyName);
            result = NULL;
        }
        else
        {
            result = ((MODEL_IN_MODEL*)temp)->modelHandle;
        }
    }
    return result;
}

size_t Schema_GetModelModelByName_Offset(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName)
{
    size_t result;
    /*Codes_SRS_SCHEMA_02_053: [ If modelTypeHandle is NULL then Schema_GetModelModelByName_Offset shall fail and return 0. ]*/
    /*Codes_SRS_SCHEMA_02_054: [ If propertyName is NULL then Schema_GetModelModelByName_Offset shall fail and return 0. ]*/
    if (
        (modelTypeHandle == NULL) ||
        (propertyName == NULL)
        )
    {
        /*Codes_SRS_SCHEMA_99_171: [If Schema_GetModelModelByName is unable to provide the handle it shall return NULL.]*/
        result = 0;
        LogError("error SCHEMA_INVALID_ARG");
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* model = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        /*Codes_SRS_SCHEMA_02_056: [ If propertyName is not a model then Schema_GetModelModelByName_Offset shall fail and return 0. ]*/
        void* temp = VECTOR_find_if(model->models, matchModelName, propertyName);
        if (temp == NULL)
        {
            LogError("specified propertyName not found (%s)", propertyName);
            result = 0;
        }
        else
        {
            /*Codes_SRS_SCHEMA_02_055: [ Otherwise Schema_GetModelModelByName_Offset shall succeed and return the offset. ]*/
            result = ((MODEL_IN_MODEL*)temp)->offset;
        }
    }
    return result;
}

pfOnDesiredProperty Schema_GetModelModelByName_OnDesiredProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName)
{
    pfOnDesiredProperty result;
    /*Codes_SRS_SCHEMA_02_086: [ If modelTypeHandle is NULL then Schema_GetModelModelByName_OnDesiredProperty shall return NULL. ]*/
    /*Codes_SRS_SCHEMA_02_087: [ If propertyName is NULL then Schema_GetModelModelByName_OnDesiredProperty shall return NULL. ]*/
    if (
        (modelTypeHandle == NULL) ||
        (propertyName == NULL)
        )
    {
        result = NULL;
        LogError("invalid argument SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, const char* propertyName=%p",modelTypeHandle, propertyName);
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* model = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        void* temp = VECTOR_find_if(model->models, matchModelName, propertyName);
        if (temp == NULL)
        {
            LogError("specified propertyName not found (%s)", propertyName);
            result = NULL;
        }
        else
        {
            /*Codes_SRS_SCHEMA_02_089: [ Otherwise Schema_GetModelModelByName_OnDesiredProperty shall return the desired property callback. ]*/
            result = ((MODEL_IN_MODEL*)temp)->onDesiredProperty;
        }
    }
    return result;

}

size_t Schema_GetModelModelByIndex_Offset(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index)
{
    size_t result;
    /*Codes_SRS_SCHEMA_02_057: [ If modelTypeHandle is NULL then Schema_GetModelModelByIndex_Offset shall fail and return 0. ]*/
    if (modelTypeHandle == NULL)
    {
        result = 0;
        LogError("error SCHEMA_INVALID_ARG");
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* model = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        /*Codes_SRS_SCHEMA_02_058: [ If index is not valid then Schema_GetModelModelByIndex_Offset shall fail and return 0. ]*/
        void* temp = VECTOR_element(model->models, index);
        if (temp == 0)
        {
            LogError("specified index [out of bounds] (%lu)", (unsigned long)index);
            result = 0;
        }
        else
        {
            /*Codes_SRS_SCHEMA_02_059: [ Otherwise Schema_GetModelModelByIndex_Offset shall succeed and return the offset. ]*/
            result = ((MODEL_IN_MODEL*)temp)->offset;
        }
    }
    return result;
}

SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelModelyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index)
{
    SCHEMA_MODEL_TYPE_HANDLE result;
    if (modelTypeHandle == NULL)
    {
        /*Codes_SRS_SCHEMA_99_173: [Schema_GetModelModelyByIndex shall return NULL in the cases when it cannot provide the handle.]*/
        result = NULL;
        LogError("error SCHEMA_INVALID_ARG");
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* model = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        size_t nModelsInModel;
        /*Codes_SRS_SCHEMA_99_172: [ Schema_GetModelModelyByIndex shall return a handle to the "index"th model inserted in the model identified by the parameter modelTypeHandle.]*/
        /*Codes_SRS_SCHEMA_99_173: [Schema_GetModelModelyByIndex shall return NULL in the cases when it cannot provide the handle.]*/
        nModelsInModel = VECTOR_size(model->models);
        if (index < nModelsInModel)
        {
            result = ((MODEL_IN_MODEL*)VECTOR_element(model->models, index))->modelHandle;
        }
        else
        {
            LogError("attempted out of bounds access in array.");
            result = NULL; /*out of bounds array access*/
        }
    }
    return result;
}

const char* Schema_GetModelModelPropertyNameByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index)
{
    const char* result;
    if (modelTypeHandle == NULL)
    {
        /*Codes_SRS_SCHEMA_99_176: [If Schema_GetModelModelPropertyNameByIndex cannot produce the property name, it shall return NULL.]*/
        result = NULL;
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* model = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        size_t nModelsInModel;
        /*Codes_SRS_SCHEMA_99_175: [Schema_GetModelModelPropertyNameByIndex shall return the name of the property for the "index"th model in the model identified by modelTypeHandle parameter.]*/
        /*Codes_SRS_SCHEMA_99_176: [If Schema_GetModelModelPropertyNameByIndex cannot produce the property name, it shall return NULL.]*/
        nModelsInModel = VECTOR_size(model->models);
        if (index < nModelsInModel)
        {
            result = ((MODEL_IN_MODEL*)VECTOR_element(model->models, index))->propertyName;
        }
        else
        {
            LogError("attempted out of bounds access in array.");
            result = NULL; /*out of bounds array access*/
        }
    }
    return result;
}

bool Schema_ModelPropertyByPathExists(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyPath)
{
    bool result;

    /* Codes_SRS_SCHEMA_99_180: [If any of the arguments are NULL, Schema_ModelPropertyByPathExists shall return false.] */
    if ((modelTypeHandle == NULL) ||
        (propertyPath == NULL))
    {
        LogError("error SCHEMA_INVALID_ARG");
        result = false;
    }
    else
    {
        const char* slashPos;
        result = false;

        /* Codes_SRS_SCHEMA_99_182: [A single slash ('/') at the beginning of the path shall be ignored and the path shall still be valid.] */
        if (*propertyPath == '/')
        {
            propertyPath++;
        }

        do
        {
            const char* endPos;
            size_t i;
            size_t modelCount;
            SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;

            /* Codes_SRS_SCHEMA_99_179: [The propertyPath shall be assumed to be in the format model1/model2/.../propertyName.] */
            slashPos = strchr(propertyPath, '/');
            endPos = slashPos;
            if (endPos == NULL)
            {
                endPos = &propertyPath[strlen(propertyPath)];
            }

            /* get the child-model */
            modelCount = VECTOR_size(modelType->models);
            for (i = 0; i < modelCount; i++)
            {
                MODEL_IN_MODEL* childModel = (MODEL_IN_MODEL*)VECTOR_element(modelType->models, i);
                if (childModel != NULL &&
                    (strncmp(childModel->propertyName, propertyPath, endPos - propertyPath) == 0) &&
                    (strlen(childModel->propertyName) == (size_t)(endPos - propertyPath)))
                {
                    /* found */
                    modelTypeHandle = childModel->modelHandle;
                    break;
                }
            }

            if (i < modelCount)
            {
                /* model found, check if there is more in the path */
                if (slashPos == NULL)
                {
                    /* this is the last one, so this is the thing we were looking for */
                    result = true;
                    break;
                }
                else
                {
                    /* continue looking, there's more  */
                    propertyPath = slashPos + 1;
                }
            }
            else
            {
                /* no model found, let's see if this is a property */
                /* Codes_SRS_SCHEMA_99_178: [The argument propertyPath shall be used to find the leaf property.] */
                for (i = 0; i < modelType->PropertyCount; i++)
                {
                    SCHEMA_PROPERTY_HANDLE_DATA* property = (SCHEMA_PROPERTY_HANDLE_DATA*)modelType->Properties[i];
                    if ((strncmp(property->PropertyName, propertyPath, endPos - propertyPath) == 0) &&
                        (strlen(property->PropertyName) == (size_t)(endPos - propertyPath)))
                    {
                        /* found property */
                        /* Codes_SRS_SCHEMA_99_177: [Schema_ModelPropertyByPathExists shall return true if a leaf property exists in the model modelTypeHandle.] */
                        result = true;
                        break;
                    }
                }

                break;
            }
        } while (slashPos != NULL);
    }

    return result;
}

bool Schema_ModelReportedPropertyByPathExists(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* reportedPropertyPath)
{
    bool result;

    /*Codes_SRS_SCHEMA_02_018: [ If argument modelTypeHandle is NULL then Schema_ModelReportedPropertyByPathExists shall fail and return false. ]*/
    /*Codes_SRS_SCHEMA_02_019: [ If argument reportedPropertyPath is NULL then Schema_ModelReportedPropertyByPathExists shall fail and return false. ]*/
    if ((modelTypeHandle == NULL) ||
        (reportedPropertyPath == NULL))
    {
        LogError("invalid argument SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, const char* reportedPropertyPath=%s", modelTypeHandle, reportedPropertyPath);
        result = false;
    }
    else
    {
        const char* slashPos;

        /*Codes_SRS_SCHEMA_02_021: [ If the reported property cannot be found Schema_ModelReportedPropertyByPathExists shall fail and return false. ]*/
        result = false;

        /*Codes_SRS_SCHEMA_02_020: [ reportedPropertyPath shall be assumed to be in the format model1/model2/.../reportedPropertyName. ]*/
        if (*reportedPropertyPath == '/')
        {
            reportedPropertyPath++;
        }

        do
        {
            const char* endPos;
            size_t i;
            size_t modelCount;
            SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;

            slashPos = strchr(reportedPropertyPath, '/');
            endPos = slashPos;
            if (endPos == NULL)
            {
                endPos = &reportedPropertyPath[strlen(reportedPropertyPath)];
            }

            modelCount = VECTOR_size(modelType->models);
            for (i = 0; i < modelCount; i++)
            {
                MODEL_IN_MODEL* childModel = (MODEL_IN_MODEL*)VECTOR_element(modelType->models, i);
                if (childModel != NULL &&
                    (strncmp(childModel->propertyName, reportedPropertyPath, endPos - reportedPropertyPath) == 0) &&
                    (strlen(childModel->propertyName) == (size_t)(endPos - reportedPropertyPath)))
                {
                    /* found */
                    modelTypeHandle = childModel->modelHandle;
                    break;
                }
            }

            if (i < modelCount)
            {
                /* model found, check if there is more in the path */
                if (slashPos == NULL)
                {
                    /*Codes_SRS_SCHEMA_02_022: [ If the path reportedPropertyPath points to a sub-model, Schema_ModelReportedPropertyByPathExists shall succeed and true. ]*/
                    /* this is the last one, so this is the thing we were looking for */
                    result = true;
                    break;
                }
                else
                {
                    /* continue looking, there's more  */
                    reportedPropertyPath = slashPos + 1;
                }
            }
            else
            {
                /* no model found, let's see if this is a property */
                result = (VECTOR_find_if(modelType->reportedProperties, reportedPropertyExists, reportedPropertyPath) != NULL);
                if (!result)
                {
                    LogError("no such reported property \"%s\"", reportedPropertyPath);
                }
                break;
            }
        } while (slashPos != NULL);
    }

    return result;
}

static bool desiredPropertyExists(const void* element, const void* value)
{
    SCHEMA_DESIRED_PROPERTY_HANDLE_DATA* desiredProperty = *(SCHEMA_DESIRED_PROPERTY_HANDLE_DATA**)element;
    return (strcmp(desiredProperty->desiredPropertyName, value) == 0);
}

SCHEMA_RESULT Schema_AddModelDesiredProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* desiredPropertyName, const char* desiredPropertyType, pfDesiredPropertyFromAGENT_DATA_TYPE desiredPropertyFromAGENT_DATA_TYPE, pfDesiredPropertyInitialize desiredPropertyInitialize, pfDesiredPropertyDeinitialize desiredPropertyDeinitialize, size_t offset, pfOnDesiredProperty onDesiredProperty)
{
    SCHEMA_RESULT result;
    /*Codes_SRS_SCHEMA_02_024: [ If modelTypeHandle is NULL then Schema_AddModelDesiredProperty shall fail and return SCHEMA_INVALID_ARG. ]*/
    /*Codes_SRS_SCHEMA_02_025: [ If desiredPropertyName is NULL then Schema_AddModelDesiredProperty shall fail and return SCHEMA_INVALID_ARG. ]*/
    /*Codes_SRS_SCHEMA_02_026: [ If desiredPropertyType is NULL then Schema_AddModelDesiredProperty shall fail and return SCHEMA_INVALID_ARG. ]*/
    /*Codes_SRS_SCHEMA_02_048: [ If desiredPropertyFromAGENT_DATA_TYPE is NULL then Schema_AddModelDesiredProperty shall fail and return SCHEMA_INVALID_ARG. ]*/
    /*Codes_SRS_SCHEMA_02_049: [ If desiredPropertyInitialize is NULL then Schema_AddModelDesiredProperty shall fail and return SCHEMA_INVALID_ARG. ]*/
    /*Codes_SRS_SCHEMA_02_050: [ If desiredPropertyDeinitialize is NULL then Schema_AddModelDesiredProperty shall fail and return SCHEMA_INVALID_ARG. ]*/
    if (
        (modelTypeHandle == NULL) ||
        (desiredPropertyName == NULL) ||
        (desiredPropertyType == NULL) ||
        (desiredPropertyFromAGENT_DATA_TYPE == NULL) ||
        (desiredPropertyInitialize == NULL) ||
        (desiredPropertyDeinitialize== NULL)
        )
    {
        LogError("invalid arg SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, const char* desiredPropertyName=%p, const char* desiredPropertyType=%p, pfDesiredPropertyFromAGENT_DATA_TYPE desiredPropertyFromAGENT_DATA_TYPE=%p, pfDesiredPropertyInitialize desiredPropertyInitialize=%p, pfDesiredPropertyDeinitialize desiredPropertyDeinitialize=%p, size_t offset=%lu",
            modelTypeHandle, desiredPropertyName, desiredPropertyType, desiredPropertyFromAGENT_DATA_TYPE, desiredPropertyInitialize, desiredPropertyDeinitialize, (unsigned long)offset);
        result = SCHEMA_INVALID_ARG;
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* handleData = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        /*Codes_SRS_SCHEMA_02_027: [ Schema_AddModelDesiredProperty shall add the desired property given by the name desiredPropertyName and the type desiredPropertyType to the collection of existing desired properties. ]*/
        if (VECTOR_find_if(handleData->desiredProperties, desiredPropertyExists, desiredPropertyName) != NULL)
        {
            /*Codes_SRS_SCHEMA_02_047: [ If the desired property already exists, then Schema_AddModelDesiredProperty shall fail and return SCHEMA_DUPLICATE_ELEMENT. ]*/
            LogError("unable to Schema_AddModelDesiredProperty because a desired property with the same name (%s) already exists", desiredPropertyName);
            result = SCHEMA_DUPLICATE_ELEMENT;
        }
        else
        {
            SCHEMA_DESIRED_PROPERTY_HANDLE_DATA* desiredProperty = (SCHEMA_DESIRED_PROPERTY_HANDLE_DATA*)calloc(1, sizeof(SCHEMA_DESIRED_PROPERTY_HANDLE_DATA));
            if (desiredProperty == NULL)
            {
                /*Codes_SRS_SCHEMA_02_028: [ If any failure occurs then Schema_AddModelDesiredProperty shall fail and return SCHEMA_ERROR. ]*/
                LogError("failure in malloc");
                result = SCHEMA_ERROR;
            }
            else
            {
                if (mallocAndStrcpy_s((char**)&desiredProperty->desiredPropertyName, desiredPropertyName) != 0)
                {
                    /*Codes_SRS_SCHEMA_02_028: [ If any failure occurs then Schema_AddModelDesiredProperty shall fail and return SCHEMA_ERROR. ]*/
                    LogError("failure in mallocAndStrcpy_s");
                    free(desiredProperty);
                    result = SCHEMA_ERROR;
                }
                else
                {
                    if (mallocAndStrcpy_s((char**)&desiredProperty->desiredPropertyType, desiredPropertyType) != 0)
                    {
                        /*Codes_SRS_SCHEMA_02_028: [ If any failure occurs then Schema_AddModelDesiredProperty shall fail and return SCHEMA_ERROR. ]*/
                        LogError("failure in mallocAndStrcpy_s");
                        free((void*)desiredProperty->desiredPropertyName);
                        free(desiredProperty);
                        result = SCHEMA_ERROR;
                    }
                    else
                    {
                        if (VECTOR_push_back(handleData->desiredProperties, &desiredProperty, 1) != 0)
                        {
                            /*Codes_SRS_SCHEMA_02_028: [ If any failure occurs then Schema_AddModelDesiredProperty shall fail and return SCHEMA_ERROR. ]*/
                            LogError("failure in VECTOR_push_back");
                            free((void*)desiredProperty->desiredPropertyType);
                            free((void*)desiredProperty->desiredPropertyName);
                            free(desiredProperty);
                            result = SCHEMA_ERROR;
                        }
                        else
                        {
                            /*Codes_SRS_SCHEMA_02_029: [ Otherwise, Schema_AddModelDesiredProperty shall succeed and return SCHEMA_OK. ]*/
                            desiredProperty->desiredPropertyFromAGENT_DATA_TYPE = desiredPropertyFromAGENT_DATA_TYPE;
                            desiredProperty->desiredPropertInitialize = desiredPropertyInitialize;
                            desiredProperty->desiredPropertDeinitialize = desiredPropertyDeinitialize;
                            desiredProperty->onDesiredProperty = onDesiredProperty; /*NULL is a perfectly fine value*/
                            desiredProperty->offset = offset;
                            result = SCHEMA_OK;
                        }
                    }
                }
            }
        }
    }
    return result;
}


SCHEMA_RESULT Schema_GetModelDesiredPropertyCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* desiredPropertyCount)
{
    SCHEMA_RESULT result;
    /*Codes_SRS_SCHEMA_02_030: [ If modelTypeHandle is NULL then Schema_GetModelDesiredPropertyCount shall fail and return SCHEMA_INVALID_ARG. ]*/
    /*Codes_SRS_SCHEMA_02_031: [ If desiredPropertyCount is NULL then Schema_GetModelDesiredPropertyCount shall fail and return SCHEMA_INVALID_ARG. ]*/
    if (
        (modelTypeHandle == NULL) ||
        (desiredPropertyCount == NULL)
        )
    {
        LogError("invalid arg: SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, size_t* desiredPropertyCount=%p", modelTypeHandle, desiredPropertyCount);
        result = SCHEMA_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_032: [ Otherwise, Schema_GetModelDesiredPropertyCount shall succeed and write in desiredPropertyCount the existing number of desired properties. ]*/
        SCHEMA_MODEL_TYPE_HANDLE_DATA* handleData = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        *desiredPropertyCount = VECTOR_size(handleData->desiredProperties);
        result = SCHEMA_OK;
    }
    return result;
}

SCHEMA_DESIRED_PROPERTY_HANDLE Schema_GetModelDesiredPropertyByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* desiredPropertyName)
{
    SCHEMA_DESIRED_PROPERTY_HANDLE result;
    /*Codes_SRS_SCHEMA_02_034: [ If modelTypeHandle is NULL then Schema_GetModelDesiredPropertyByName shall fail and return NULL. ]*/
    /*Codes_SRS_SCHEMA_02_035: [ If desiredPropertyName is NULL then Schema_GetModelDesiredPropertyByName shall fail and return NULL. ]*/
    if (
        (modelTypeHandle == NULL) ||
        (desiredPropertyName == NULL)
        )
    {
        LogError("invalid arg SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, const char* desiredPropertyName=%p", modelTypeHandle, desiredPropertyName);
        result = NULL;
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_036: [ If a desired property having the name desiredPropertyName exists then Schema_GetModelDesiredPropertyByName shall succeed and return a non-NULL value. ]*/
        /*Codes_SRS_SCHEMA_02_037: [ Otherwise, Schema_GetModelDesiredPropertyByName shall fail and return NULL. ]*/
        SCHEMA_MODEL_TYPE_HANDLE_DATA* handleData = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        SCHEMA_DESIRED_PROPERTY_HANDLE* temp = VECTOR_find_if(handleData->desiredProperties, desiredPropertyExists, desiredPropertyName);
        if (temp == NULL)
        {
            LogError("no such desired property by name %s", desiredPropertyName);
            result = NULL;
        }
        else
        {
            result = *temp;
        }
    }
    return result;
}

SCHEMA_DESIRED_PROPERTY_HANDLE Schema_GetModelDesiredPropertyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index)
{
    SCHEMA_DESIRED_PROPERTY_HANDLE result;
    /*Codes_SRS_SCHEMA_02_038: [ If modelTypeHandle is NULL then Schema_GetModelDesiredPropertyByIndex shall fail and return NULL. ]*/
    if (modelTypeHandle == NULL)
    {
        LogError("invalid argument SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, size_t index=%lu", modelTypeHandle, (unsigned long)index);
        result = NULL;
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* handleData = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;
        /*Codes_SRS_SCHEMA_02_039: [ If index is outside the range for existing indexes of desire properties, then Schema_GetModelDesiredPropertyByIndex shall fail and return NULL. ]*/
        /*Codes_SRS_SCHEMA_02_040: [ Otherwise, Schema_GetModelDesiredPropertyByIndex shall succeed and return a non-NULL value. ]*/
        SCHEMA_DESIRED_PROPERTY_HANDLE* temp = VECTOR_element(handleData->desiredProperties, index);
        if (temp == NULL)
        {
            LogError("VECTOR_element produced NULL (likely out of bounds index)");
            result = NULL;
        }
        else
        {
            result = *temp;
        }
    }
    return result;
}

bool Schema_ModelDesiredPropertyByPathExists(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* desiredPropertyPath)
{
    bool result;
    /*Codes_SRS_SCHEMA_02_041: [ If modelTypeHandle is NULL then Schema_ModelDesiredPropertyByPathExists shall fail and return false. ]*/
    /*Codes_SRS_SCHEMA_02_042: [ If desiredPropertyPath is NULL then Schema_ModelDesiredPropertyByPathExists shall fail and return false. ]*/
    if (
        (modelTypeHandle == NULL) ||
        (desiredPropertyPath == NULL)
        )
    {
        LogError("invalid arg SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, const char* desiredPropertyPath=%p", modelTypeHandle, desiredPropertyPath);
        result = false;
    }
    else
    {
        const char* slashPos;

        /*Codes_SRS_SCHEMA_02_044: [ If the desired property cannot be found Schema_ModelDesiredPropertyByPathExists shall fail and return false. ]*/
        result = false;

        /*Codes_SRS_SCHEMA_02_043: [ desiredPropertyPath shall be assumed to be in the format model1/model2/.../desiredPropertyName. ]*/
        if (*desiredPropertyPath == '/')
        {
            desiredPropertyPath++;
        }

        do
        {
            const char* endPos;
            size_t i;
            size_t modelCount;
            SCHEMA_MODEL_TYPE_HANDLE_DATA* modelType = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;

            slashPos = strchr(desiredPropertyPath, '/');
            endPos = slashPos;
            if (endPos == NULL)
            {
                endPos = &desiredPropertyPath[strlen(desiredPropertyPath)];
            }

            modelCount = VECTOR_size(modelType->models);
            for (i = 0; i < modelCount; i++)
            {
                MODEL_IN_MODEL* childModel = (MODEL_IN_MODEL*)VECTOR_element(modelType->models, i);
                if (childModel != NULL &&
                    (strncmp(childModel->propertyName, desiredPropertyPath, endPos - desiredPropertyPath) == 0) &&
                    (strlen(childModel->propertyName) == (size_t)(endPos - desiredPropertyPath)))
                {
                    /* found */
                    modelTypeHandle = childModel->modelHandle;
                    break;
                }
            }

            if (i < modelCount)
            {
                /* model found, check if there is more in the path */
                if (slashPos == NULL)
                {
                    /*Codes_SRS_SCHEMA_02_045: [ If the path desiredPropertyPath points to a sub-model, Schema_ModelDesiredPropertyByPathExists shall succeed and true. ]*/
                    /* this is the last one, so this is the thing we were looking for */
                    result = true;
                    break;
                }
                else
                {
                    /* continue looking, there's more  */
                    desiredPropertyPath = slashPos + 1;
                }
            }
            else
            {
                /* no model found, let's see if this is a property */
                result = (VECTOR_find_if(modelType->desiredProperties, desiredPropertyExists, desiredPropertyPath) != NULL);
                if (!result)
                {
                    LogError("no such desired property \"%s\"", desiredPropertyPath);
                }
                break;
            }
        } while (slashPos != NULL);
    }
    return result;
}

const char* Schema_GetModelDesiredPropertyType(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle)
{
    const char* result;
    /*Codes_SRS_SCHEMA_02_062: [ If desiredPropertyHandle is NULL then Schema_GetModelDesiredPropertyType shall fail and return NULL. ]*/
    if (desiredPropertyHandle == NULL)
    {
        LogError("invalid argument SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle=%p", desiredPropertyHandle);
        result = NULL;
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_063: [ Otherwise, Schema_GetModelDesiredPropertyType shall return the type of the desired property. ]*/
        SCHEMA_DESIRED_PROPERTY_HANDLE_DATA* desirePropertyHandleData = (SCHEMA_DESIRED_PROPERTY_HANDLE_DATA*)desiredPropertyHandle;
        result = desirePropertyHandleData->desiredPropertyType;
    }
    return result;
}

pfDesiredPropertyFromAGENT_DATA_TYPE Schema_GetModelDesiredProperty_pfDesiredPropertyFromAGENT_DATA_TYPE(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle)
{
    pfDesiredPropertyFromAGENT_DATA_TYPE result;
    if (desiredPropertyHandle == NULL)
    {
        LogError("invalid argument SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle=%p", desiredPropertyHandle);
        result = NULL;
    }
    else
    {
        SCHEMA_DESIRED_PROPERTY_HANDLE_DATA* desirePropertyHandleData = (SCHEMA_DESIRED_PROPERTY_HANDLE_DATA*)desiredPropertyHandle;
        result = desirePropertyHandleData->desiredPropertyFromAGENT_DATA_TYPE;
    }
    return result;
}

pfOnDesiredProperty Schema_GetModelDesiredProperty_pfOnDesiredProperty(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle)
{
    pfOnDesiredProperty result;
    /*Codes_SRS_SCHEMA_02_084: [ If desiredPropertyHandle is NULL then Schema_GetModelDesiredProperty_pfOnDesiredProperty shall return NULL. ]*/
    if (desiredPropertyHandle == NULL)
    {
        LogError("invalid argument SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle=%p", desiredPropertyHandle);
        result = NULL;
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_085: [ Otherwise Schema_GetModelDesiredProperty_pfOnDesiredProperty shall return the saved desired property callback. ]*/
        SCHEMA_DESIRED_PROPERTY_HANDLE_DATA* desirePropertyHandleData = (SCHEMA_DESIRED_PROPERTY_HANDLE_DATA*)desiredPropertyHandle;
        result = desirePropertyHandleData->onDesiredProperty;
    }
    return result;
}

size_t Schema_GetModelDesiredProperty_offset(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle)
{
    size_t result;
    /*Codes_SRS_SCHEMA_02_060: [ If desiredPropertyHandle is NULL then Schema_GetModelDesiredProperty_offset shall fail and return 0. ]*/
    if (desiredPropertyHandle == NULL)
    {
        LogError("invalid argument SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle=%p", desiredPropertyHandle);
        result = 0;
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_061: [ Otherwise Schema_GetModelDesiredProperty_offset shall succeed and return the offset. ]*/
        SCHEMA_DESIRED_PROPERTY_HANDLE_DATA* desirePropertyHandleData = (SCHEMA_DESIRED_PROPERTY_HANDLE_DATA*)desiredPropertyHandle;
        result = desirePropertyHandleData->offset;
    }
    return result;
}

static bool modelInModelExists(const void* element, const void* value)
{
    MODEL_IN_MODEL* modelInModel = (MODEL_IN_MODEL*)element;
    return (strcmp(modelInModel->propertyName, value) == 0);
}

SCHEMA_MODEL_ELEMENT Schema_GetModelElementByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* elementName)
{
    SCHEMA_MODEL_ELEMENT result;
    /*Codes_SRS_SCHEMA_02_076: [ If modelTypeHandle is NULL then Schema_GetModelElementByName shall fail and set SCHEMA_MODEL_ELEMENT.elementType to SCHEMA_SEARCH_INVALID_ARG. ]*/
    /*Codes_SRS_SCHEMA_02_077: [ If elementName is NULL then Schema_GetModelElementByName shall fail and set SCHEMA_MODEL_ELEMENT.elementType to SCHEMA_SEARCH_INVALID_ARG. ]*/
    if (
        (modelTypeHandle == NULL) ||
        (elementName == NULL)
        )
    {
        LogError("invalid argument SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle=%p, const char* elementName=%p", modelTypeHandle, elementName);
        result.elementType = SCHEMA_SEARCH_INVALID_ARG;
    }
    else
    {
        SCHEMA_MODEL_TYPE_HANDLE_DATA* handleData = (SCHEMA_MODEL_TYPE_HANDLE_DATA*)modelTypeHandle;

        SCHEMA_DESIRED_PROPERTY_HANDLE* desiredPropertyHandle = VECTOR_find_if(handleData->desiredProperties, desiredPropertyExists, elementName);
        if (desiredPropertyHandle != NULL)
        {
            /*Codes_SRS_SCHEMA_02_080: [ If elementName is a desired property then Schema_GetModelElementByName shall succeed and set SCHEMA_MODEL_ELEMENT.elementType to SCHEMA_DESIRED_PROPERTY and SCHEMA_MODEL_ELEMENT.elementHandle.desiredPropertyHandle to the handle of the desired property. ]*/
            result.elementType = SCHEMA_DESIRED_PROPERTY;
            result.elementHandle.desiredPropertyHandle = *desiredPropertyHandle;
        }
        else
        {
            size_t nProcessedProperties = 0;
            SCHEMA_PROPERTY_HANDLE_DATA* property = NULL;
            for (size_t i = 0; i < handleData->PropertyCount;i++)
            {
                property = (SCHEMA_PROPERTY_HANDLE_DATA*)(handleData->Properties[i]);
                if (strcmp(property->PropertyName, elementName) == 0)
                {
                    i = handleData->PropertyCount; /*found it*/
                }
                else
                {
                    nProcessedProperties++;
                }
            }

            if (nProcessedProperties < handleData->PropertyCount)
            {
                /*Codes_SRS_SCHEMA_02_078: [ If elementName is a property then Schema_GetModelElementByName shall succeed and set SCHEMA_MODEL_ELEMENT.elementType to SCHEMA_PROPERTY and SCHEMA_MODEL_ELEMENT.elementHandle.propertyHandle to the handle of the property. ]*/
                result.elementType = SCHEMA_PROPERTY;
                result.elementHandle.propertyHandle = property;
            }
            else
            {

                SCHEMA_REPORTED_PROPERTY_HANDLE* reportedPropertyHandle = VECTOR_find_if(handleData->reportedProperties, reportedPropertyExists, elementName);
                if (reportedPropertyHandle != NULL)
                {
                    /*Codes_SRS_SCHEMA_02_079: [ If elementName is a reported property then Schema_GetModelElementByName shall succeed and set SCHEMA_MODEL_ELEMENT.elementType to SCHEMA_REPORTED_PROPERTY and SCHEMA_MODEL_ELEMENT.elementHandle.reportedPropertyHandle to the handle of the reported property. ]*/
                    result.elementType = SCHEMA_REPORTED_PROPERTY;
                    result.elementHandle.reportedPropertyHandle = *reportedPropertyHandle;
                }
                else
                {

                    size_t nProcessedActions = 0;
                    SCHEMA_ACTION_HANDLE_DATA* actionHandleData = NULL;
                    for (size_t i = 0;i < handleData->ActionCount; i++)
                    {
                        actionHandleData = (SCHEMA_ACTION_HANDLE_DATA*)(handleData->Actions[i]);
                        if (strcmp(actionHandleData->ActionName, elementName) == 0)
                        {
                            i = handleData->ActionCount; /*get out quickly*/
                        }
                        else
                        {
                            nProcessedActions++;
                        }
                    }

                    if (nProcessedActions < handleData->ActionCount)
                    {
                        /*Codes_SRS_SCHEMA_02_081: [ If elementName is a model action then Schema_GetModelElementByName shall succeed and set SCHEMA_MODEL_ELEMENT.elementType to SCHEMA_MODEL_ACTION and SCHEMA_MODEL_ELEMENT.elementHandle.actionHandle to the handle of the action. ]*/
                        result.elementType = SCHEMA_MODEL_ACTION;
                        result.elementHandle.actionHandle = actionHandleData;
                    }
                    else
                    {
                        MODEL_IN_MODEL* modelInModel = VECTOR_find_if(handleData->models, modelInModelExists, elementName);
                        if (modelInModel != NULL)
                        {
                            /*Codes_SRS_SCHEMA_02_082: [ If elementName is a model in model then Schema_GetModelElementByName shall succeed and set SCHEMA_MODEL_ELEMENT.elementType to SCHEMA_MODEL_IN_MODEL and SCHEMA_MODEL_ELEMENT.elementHandle.modelHandle to the handle of the model. ]*/
                            result.elementType = SCHEMA_MODEL_IN_MODEL;
                            result.elementHandle.modelHandle = modelInModel->modelHandle;
                        }
                        else
                        {
                            /*Codes_SRS_SCHEMA_02_083: [ Otherwise Schema_GetModelElementByName shall fail and set SCHEMA_MODEL_ELEMENT.elementType to SCHEMA_NOT_FOUND. ]*/
                            result.elementType = SCHEMA_NOT_FOUND;
                        }
                    }
                }
            }
        }
    }
    return result;
}

pfDesiredPropertyDeinitialize Schema_GetModelDesiredProperty_pfDesiredPropertyDeinitialize(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle)
{
    pfDesiredPropertyDeinitialize result;
    /*Ccodes_SRS_SCHEMA_02_064: [ If desiredPropertyHandle is NULL then Schema_GetModelDesiredProperty_pfDesiredPropertyDeinitialize shall fail and return NULL. ]*/
    if (desiredPropertyHandle == NULL)
    {
        LogError("invalid argument SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle=%p", desiredPropertyHandle);
        result = NULL;
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_065: [ Otherwise Schema_GetModelDesiredProperty_pfDesiredPropertyDeinitialize shall return a non-NULL function pointer. ]*/
        SCHEMA_DESIRED_PROPERTY_HANDLE_DATA* handleData = (SCHEMA_DESIRED_PROPERTY_HANDLE_DATA*)desiredPropertyHandle;
        result = handleData->desiredPropertDeinitialize;
    }
    return result;
}

pfDesiredPropertyInitialize Schema_GetModelDesiredProperty_pfDesiredPropertyInitialize(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle)
{
    pfDesiredPropertyInitialize result;
    /*Codes_SRS_SCHEMA_02_066: [ If desiredPropertyHandle is NULL then Schema_GetModelDesiredProperty_pfDesiredPropertyInitialize shall fail and return NULL. ]*/
    if (desiredPropertyHandle == NULL)
    {
        LogError("invalid argument SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle=%p", desiredPropertyHandle);
        result = NULL;
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_067: [ Otherwise Schema_GetModelDesiredProperty_pfDesiredPropertyInitialize shall return a non-NULL function pointer. ]*/
        SCHEMA_DESIRED_PROPERTY_HANDLE_DATA* handleData = (SCHEMA_DESIRED_PROPERTY_HANDLE_DATA*)desiredPropertyHandle;
        result = handleData->desiredPropertInitialize;
    }
    return result;
}

static bool SchemaHasModel(const SCHEMA_HANDLE* schemaHandle, const char* modelName)
{
    return (Schema_GetModelByName(*schemaHandle, modelName) != NULL);
}


SCHEMA_HANDLE Schema_GetSchemaForModel(const char* modelName)
{
    SCHEMA_HANDLE result;
    /*Codes_SRS_SCHEMA_02_093: [ If modelName is NULL then Schema_GetSchemaForModel shall fail and return NULL. ]*/
    if (modelName == NULL)
    {
        LogError("invalid arg const char* modelName=%p", modelName);
        result = NULL;
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_094: [ Schema_GetSchemaForModel shall find the SCHEMA_HANDLE that contains a model by name modelName and if found, succeed and return that. ]*/
        SCHEMA_HANDLE* temp = VECTOR_find_if(g_schemas, (PREDICATE_FUNCTION)SchemaHasModel, modelName);

        if (temp == NULL)
        {
            /*Codes_SRS_SCHEMA_02_095: [ If the model is not found in any schema, then Schema_GetSchemaForModel shall fail and return NULL. ]*/
            LogError("unable to find a schema that has a model named %s", modelName);
            result = NULL;
        }
        else
        {
            /*Codes_SRS_SCHEMA_02_094: [ Schema_GetSchemaForModel shall find the SCHEMA_HANDLE that contains a model by name modelName and if found, succeed and return that. ]*/
            result = *temp;
        }
    }

    return result;
}

void* Schema_GetMetadata(SCHEMA_HANDLE schemaHandle)
{
    void* result;
    /*Codes_SRS_SCHEMA_02_091: [ If schemaHandle is NULL then Schema_GetMetadata shall fail and return NULL. ]*/
    if (schemaHandle == NULL)
    {
        LogError("invalid arg SCHEMA_HANDLE schemaHandle=%p", schemaHandle);
        result = NULL;
    }
    else
    {
        /*Codes_SRS_SCHEMA_02_092: [ Otherwise, Schema_GetMetadata shall succeed and return the saved metadata pointer. ]*/
        result = ((SCHEMA_HANDLE_DATA*)schemaHandle)->metadata;
    }
    return result;
}
