// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#ifndef SERIALIZER_DEVICE_TWIN_H
#define SERIALIZER_DEVICE_TWIN_H

#include "serializer.h"

#include "iothub_client.h"
#include "iothub_client_ll.h"
#include "parson.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/vector.h"
#include "methodreturn.h"

static void serializer_ingest(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback)
{
    /*by convention, userContextCallback is a pointer to a model instance created with CodeFirst_CreateDevice*/

    /*Codes_SRS_SERIALIZERDEVICETWIN_02_001: [ serializer_ingest shall clone the payload into a null terminated string. ]*/
    char* copyOfPayload = (char*)malloc(size + 1);
    if (copyOfPayload == NULL)
    {
        /*Codes_SRS_SERIALIZERDEVICETWIN_02_008: [ If any of the above operations fail, then serializer_ingest shall return. ]*/
        LogError("unable to malloc\n");
    }
    else
    {
        (void)memcpy(copyOfPayload, payLoad, size);
        copyOfPayload[size] = '\0';

        bool parseDesiredNode = (update_state == DEVICE_TWIN_UPDATE_COMPLETE);

        if (CodeFirst_IngestDesiredProperties(userContextCallback, copyOfPayload, parseDesiredNode) != CODEFIRST_OK)
        {
            /*Codes_SRS_SERIALIZERDEVICETWIN_02_008: [ If any of the above operations fail, then serializer_ingest shall return. ]*/
            LogError("failure ingesting desired properties\n");
        }
        else
        {
            /*all is fine*/
        }

        free(copyOfPayload);
    }
}

/*both LL and convenience layer can be served by the same callback*/
static int deviceMethodCallback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* resp_size, void* userContextCallback)
{
    int result;
    /*Codes_SRS_SERIALIZERDEVICETWIN_02_021: [ deviceMethodCallback shall transform payload and size into a null terminated string. ]*/
    char* payloadZeroTerminated = (char*)malloc(size + 1);
    if (payloadZeroTerminated == NULL)
    {
        LogError("failure in malloc");
        /*Codes_SRS_SERIALIZERDEVICETWIN_02_026: [ If any failure occurs in the above operations, then deviceMethodCallback shall fail, return 500, set *response to NULL and '*resp_size` to 0. ]*/
        *response = NULL;
        *resp_size = 0;
        result = 500;
    }
    else
    {
        (void)memcpy(payloadZeroTerminated, payload, size);
        payloadZeroTerminated[size] = '\0';

        /*Codes_SRS_SERIALIZERDEVICETWIN_02_022: [ deviceMethodCallback shall call EXECUTE_METHOD passing the userContextCallback, method_name and the null terminated string build before. ]*/
        METHODRETURN_HANDLE mr = EXECUTE_METHOD(userContextCallback, method_name, payloadZeroTerminated);

        /*Codes_SRS_SERIALIZERDEVICETWIN_02_023: [ deviceMethodCallback shall get the MethodReturn_Data and shall copy the response JSON value into a new byte array. ]*/
        const METHODRETURN_DATA* data = (mr == NULL) ? NULL : MethodReturn_GetReturn(mr);

        if (mr == NULL || data == NULL)
        {
            LogError("failure in EXECUTE_METHOD");
            /*Codes_SRS_SERIALIZERDEVICETWIN_02_026: [ If any failure occurs in the above operations, then deviceMethodCallback shall fail, return 500, set *response to NULL and '*resp_size` to 0. ]*/
            *response = NULL;
            *resp_size = 0;
            result = 500;
        }
        else
        {
            /*Codes_SRS_SERIALIZERDEVICETWIN_02_025: [ deviceMethodCallback returns the statusCode from the user. ]*/
            result = data->statusCode;

            if (data->jsonValue == NULL)
            {
                *resp_size = 0;
                *response = NULL;
            }
            else
            {
                *resp_size = strlen(data->jsonValue);
                *response = (unsigned char*)malloc(*resp_size);
                if (*response == NULL)
                {
                    LogError("failure in malloc");
                    /*Codes_SRS_SERIALIZERDEVICETWIN_02_026: [ If any failure occurs in the above operations, then deviceMethodCallback shall fail, return 500, set *response to NULL and '*resp_size` to 0. ]*/
                    *response = NULL;
                    *resp_size = 0;
                    result = 500;
                }
                else
                {
                    /*Codes_SRS_SERIALIZERDEVICETWIN_02_024: [ deviceMethodCallback shall set *response to this new byte array, *resp_size to the size of the array. ]*/
                    (void)memcpy(*response, data->jsonValue, *resp_size);
                }
            }
            MethodReturn_Destroy(mr);
        }
        free(payloadZeroTerminated);
    }
    return result;
}

/*an enum that sets the type of the handle used to record IoTHubDeviceTwin_Create was called*/
#define IOTHUB_CLIENT_HANDLE_TYPE_VALUES \
    IOTHUB_CLIENT_CONVENIENCE_HANDLE_TYPE, \
    IOTHUB_CLIENT_LL_HANDLE_TYPE

MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_CLIENT_HANDLE_TYPE, IOTHUB_CLIENT_HANDLE_TYPE_VALUES)

typedef union IOTHUB_CLIENT_HANDLE_VALUE_TAG
{
    IOTHUB_CLIENT_HANDLE iothubClientHandle;
    IOTHUB_CLIENT_LL_HANDLE iothubClientLLHandle;
} IOTHUB_CLIENT_HANDLE_VALUE;

typedef struct IOTHUB_CLIENT_HANDLE_VARIANT_TAG
{
    IOTHUB_CLIENT_HANDLE_TYPE iothubClientHandleType;
    IOTHUB_CLIENT_HANDLE_VALUE iothubClientHandleValue;
} IOTHUB_CLIENT_HANDLE_VARIANT;

typedef struct SERIALIZER_DEVICETWIN_PROTOHANDLE_TAG /*it is called "PROTOHANDLE" because it is a primitive type of handle*/
{
    IOTHUB_CLIENT_HANDLE_VARIANT iothubClientHandleVariant;
    void* deviceAssigned;
} SERIALIZER_DEVICETWIN_PROTOHANDLE;

static VECTOR_HANDLE g_allProtoHandles=NULL; /*contains SERIALIZER_DEVICETWIN_PROTOHANDLE*/

static int lazilyAddProtohandle(const SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle)
{
    int result;
    if ((g_allProtoHandles == NULL) && ((g_allProtoHandles = VECTOR_create(sizeof(SERIALIZER_DEVICETWIN_PROTOHANDLE))) == NULL))
    {
        LogError("failure in VECTOR_create");
        result = MU_FAILURE;
    }
    else
    {
        if (VECTOR_push_back(g_allProtoHandles, protoHandle, 1) != 0)
        {
            LogError("failure in VECTOR_push_back");
            result = MU_FAILURE;

            /*leave it as it was*/

            if (VECTOR_size(g_allProtoHandles) == 0)
            {
                VECTOR_destroy(g_allProtoHandles);
                g_allProtoHandles = NULL;
            }
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

static IOTHUB_CLIENT_RESULT Generic_IoTHubClient_SetCallbacks(const SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK deviceTwinCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_SERIALIZERDEVICETWIN_02_011: [ IoTHubDeviceTwinCreate_Impl shall set the device twin callback. ]*/
    switch (protoHandle->iothubClientHandleVariant.iothubClientHandleType)
    {
    case IOTHUB_CLIENT_CONVENIENCE_HANDLE_TYPE:
    {
        if ((result = IoTHubClient_SetDeviceTwinCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientHandle, deviceTwinCallback, userContextCallback)) != IOTHUB_CLIENT_OK)
        {
            LogError("failure in IoTHubClient_SetDeviceTwinCallback");
        }
        else
        {
            /*Codes_SRS_SERIALIZERDEVICETWIN_02_027: [ IoTHubDeviceTwinCreate_Impl shall set the device method callback ]*/
            if ((result = IoTHubClient_SetDeviceMethodCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientHandle, deviceMethodCallback, userContextCallback)) != IOTHUB_CLIENT_OK)
            {
                (void)IoTHubClient_SetDeviceTwinCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientHandle, NULL, NULL);
                LogError("failure in IoTHubClient_SetDeviceMethodCallback");
            }
        }
        break;
    }
    case IOTHUB_CLIENT_LL_HANDLE_TYPE:
    {
        if ((result =IoTHubClient_LL_SetDeviceTwinCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientLLHandle, deviceTwinCallback, userContextCallback)) != IOTHUB_CLIENT_OK)
        {
            LogError("failure in IoTHubClient_LL_SetDeviceTwinCallback");
        }
        else
        {
            /*Codes_SRS_SERIALIZERDEVICETWIN_02_027: [ IoTHubDeviceTwinCreate_Impl shall set the device method callback ]*/
            if ((result = IoTHubClient_LL_SetDeviceMethodCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientLLHandle, deviceMethodCallback, userContextCallback)) != IOTHUB_CLIENT_OK)
            {
                (void)IoTHubClient_LL_SetDeviceTwinCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientLLHandle, NULL, NULL);
                LogError("failure in IoTHubClient_SetDeviceMethodCallback");
            }
        }
        break;
    }
    default:
    {
        result = IOTHUB_CLIENT_ERROR;
        LogError("INTERNAL ERROR");
    }
    }/*switch*/
    return result;
}

static void* IoTHubDeviceTwinCreate_Impl(const char* name, size_t sizeOfName, SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle)
{
    void* result;
    /*Codes_SRS_SERIALIZERDEVICETWIN_02_009: [ IoTHubDeviceTwinCreate_Impl shall locate the model and the metadata for name by calling Schema_GetSchemaForModel/Schema_GetMetadata/Schema_GetModelByName. ]*/
    SCHEMA_HANDLE h = Schema_GetSchemaForModel(name);
    if (h == NULL)
    {
        /*Codes_SRS_SERIALIZERDEVICETWIN_02_014: [ Otherwise, IoTHubDeviceTwinCreate_Impl shall fail and return NULL. ]*/
        LogError("failure in Schema_GetSchemaForModel.");
        result = NULL;
    }
    else
    {
        void* metadata = Schema_GetMetadata(h);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_GetModelByName(h, name);
        if (modelType == NULL)
        {
            /*Codes_SRS_SERIALIZERDEVICETWIN_02_014: [ Otherwise, IoTHubDeviceTwinCreate_Impl shall fail and return NULL. ]*/
            LogError("failure in Schema_GetModelByName");
            result = NULL;
        }
        else
        {
            /*Codes_SRS_SERIALIZERDEVICETWIN_02_010: [ IoTHubDeviceTwinCreate_Impl shall call CodeFirst_CreateDevice. ]*/
            result = CodeFirst_CreateDevice(modelType, (REFLECTED_DATA_FROM_DATAPROVIDER *)metadata, sizeOfName, true);
            if (result == NULL)
            {
                /*Codes_SRS_SERIALIZERDEVICETWIN_02_014: [ Otherwise, IoTHubDeviceTwinCreate_Impl shall fail and return NULL. ]*/
                LogError("failure in CodeFirst_CreateDevice");
                /*return as is*/
            }
            else
            {
                protoHandle->deviceAssigned = result;
                if (Generic_IoTHubClient_SetCallbacks(protoHandle, serializer_ingest, result) != IOTHUB_CLIENT_OK)
                {
                    /*Codes_SRS_SERIALIZERDEVICETWIN_02_014: [ Otherwise, IoTHubDeviceTwinCreate_Impl shall fail and return NULL. ]*/
                    LogError("failure in Generic_IoTHubClient_SetCallbacks");
                    CodeFirst_DestroyDevice(result);
                    result = NULL;
                }
                else
                {
                    /*lazily add the protohandle to the array of tracking handles*/

                    /*Codes_SRS_SERIALIZERDEVICETWIN_02_012: [ IoTHubDeviceTwinCreate_Impl shall record the pair of (device, IoTHubClient(_LL)). ]*/
                    if (lazilyAddProtohandle(protoHandle) != 0)
                    {
                        /*Codes_SRS_SERIALIZERDEVICETWIN_02_014: [ Otherwise, IoTHubDeviceTwinCreate_Impl shall fail and return NULL. ]*/
                        LogError("unable to add the protohandle to the collection of handles");
                        /*unsubscribe*/
                        if (Generic_IoTHubClient_SetCallbacks(protoHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
                        {
                            /*just log the error*/
                            LogError("failure in Generic_IoTHubClient_SetCallbacks");
                        }
                        CodeFirst_DestroyDevice(result);
                        result = NULL;
                    }
                    else
                    {
                        /*Codes_SRS_SERIALIZERDEVICETWIN_02_013: [ If all operations complete successfully then IoTHubDeviceTwinCreate_Impl shall succeeds and return a non-NULL value. ]*/
                        /*return as is*/
                    }
                }
            }
        }
    }
    return result;
}

static bool protoHandleHasDeviceStartAddress(const void* element, const void* value)
{
    const SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle = (const SERIALIZER_DEVICETWIN_PROTOHANDLE*)element;
    return protoHandle->deviceAssigned == value;
}

static void IoTHubDeviceTwin_Destroy_Impl(void* model)
{
    /*Codes_SRS_SERIALIZERDEVICETWIN_02_020: [ If model is NULL then IoTHubDeviceTwin_Destroy_Impl shall return. ]*/
    if (model == NULL)
    {
        LogError("invalid argument void* model=%p", model);
    }
    else
    {
        /*Codes_SRS_SERIALIZERDEVICETWIN_02_015: [ IoTHubDeviceTwin_Destroy_Impl shall locate the saved handle belonging to model. ]*/
        SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle = (SERIALIZER_DEVICETWIN_PROTOHANDLE*)VECTOR_find_if(g_allProtoHandles, protoHandleHasDeviceStartAddress, model);
        if (protoHandle == NULL)
        {
            LogError("failure in VECTOR_find_if [not found]");
        }
        else
        {
            /*Codes_SRS_SERIALIZERDEVICETWIN_02_016: [ IoTHubDeviceTwin_Destroy_Impl shall set the devicetwin callback to NULL. ]*/
            switch (protoHandle->iothubClientHandleVariant.iothubClientHandleType)
            {
            case IOTHUB_CLIENT_CONVENIENCE_HANDLE_TYPE:
            {
                if (IoTHubClient_SetDeviceTwinCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
                {
                    LogError("failure in IoTHubClient_SetDeviceTwinCallback");
                }
                /*Codes_SRS_SERIALIZERDEVICETWIN_02_028: [ IoTHubDeviceTwin_Destroy_Impl shall set the method callback to NULL. ]*/
                if (IoTHubClient_SetDeviceMethodCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
                {
                    LogError("failure in IoTHubClient_SetDeviceMethodCallback");
                }
                break;
            }
            case IOTHUB_CLIENT_LL_HANDLE_TYPE:
            {
                if (IoTHubClient_LL_SetDeviceTwinCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientLLHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
                {
                    LogError("failure in IoTHubClient_LL_SetDeviceTwinCallback");
                }
                /*Codes_SRS_SERIALIZERDEVICETWIN_02_028: [ IoTHubDeviceTwin_Destroy_Impl shall set the method callback to NULL. ]*/
                if (IoTHubClient_LL_SetDeviceMethodCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientLLHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
                {
                    LogError("failure in IoTHubClient_LL_SetDeviceMethodCallback");
                }
                break;
            }
            default:
            {
                LogError("INTERNAL ERROR");
            }
            }/*switch*/

             /*Codes_SRS_SERIALIZERDEVICETWIN_02_017: [ IoTHubDeviceTwin_Destroy_Impl shall call CodeFirst_DestroyDevice. ]*/
            CodeFirst_DestroyDevice(protoHandle->deviceAssigned);

            /*Codes_SRS_SERIALIZERDEVICETWIN_02_018: [ IoTHubDeviceTwin_Destroy_Impl shall remove the IoTHubClient_Handle and the device handle from the recorded set. ]*/
            VECTOR_erase(g_allProtoHandles, protoHandle, 1);
        }

        /*Codes_SRS_SERIALIZERDEVICETWIN_02_019: [ If the recorded set of IoTHubClient handles is zero size, then the set shall be destroyed. ]*/
        if (VECTOR_size(g_allProtoHandles) == 0) /*lazy init means more work @ destroy time*/
        {
            VECTOR_destroy(g_allProtoHandles);
            g_allProtoHandles = NULL;
        }
    }
}

/*the below function sends the reported state of a model previously created by IoTHubDeviceTwin_Create*/
/*this function serves both the _LL and the convenience layer because of protohandles*/
static IOTHUB_CLIENT_RESULT IoTHubDeviceTwin_SendReportedState_Impl(void* model, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK deviceTwinCallback, void* context)
{
    unsigned char*buffer;
    size_t bufferSize;

    IOTHUB_CLIENT_RESULT result;

    if (SERIALIZE_REPORTED_PROPERTIES_FROM_POINTERS(&buffer, &bufferSize, model) != CODEFIRST_OK)
    {
        LogError("Failed serializing reported state");
        result = IOTHUB_CLIENT_ERROR;
    }
    else
    {
        SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle = (SERIALIZER_DEVICETWIN_PROTOHANDLE*)VECTOR_find_if(g_allProtoHandles, protoHandleHasDeviceStartAddress, model);
        if (protoHandle == NULL)
        {
            LogError("failure in VECTOR_find_if [not found]");
            result = IOTHUB_CLIENT_ERROR;
        }
        else
        {
            switch (protoHandle->iothubClientHandleVariant.iothubClientHandleType)
            {
                case IOTHUB_CLIENT_CONVENIENCE_HANDLE_TYPE:
                {
                    if (IoTHubClient_SendReportedState(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientHandle, buffer, bufferSize, deviceTwinCallback, context) != IOTHUB_CLIENT_OK)
                    {
                        LogError("Failure sending data");
                        result = IOTHUB_CLIENT_ERROR;
                    }
                    else
                    {
                        result = IOTHUB_CLIENT_OK;
                    }
                    break;
                }
                case IOTHUB_CLIENT_LL_HANDLE_TYPE:
                {
                    if (IoTHubClient_LL_SendReportedState(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientLLHandle, buffer, bufferSize, deviceTwinCallback, context) != IOTHUB_CLIENT_OK)
                    {
                        LogError("Failure sending data");
                        result = IOTHUB_CLIENT_ERROR;
                    }
                    else
                    {
                        result = IOTHUB_CLIENT_OK;
                    }
                    break;
                }
                default:
                {
                    LogError("INTERNAL ERROR: unexpected value for enum (%d)", (int)protoHandle->iothubClientHandleVariant.iothubClientHandleType);
                    result = IOTHUB_CLIENT_ERROR;
                    break;
                }
            }
        }
        free(buffer);
    }
    return result;
}

#define DECLARE_DEVICETWIN_MODEL(name, ...)    \
    DECLARE_MODEL(name, __VA_ARGS__)           \
    static name* MU_C2(IoTHubDeviceTwin_Create, name)(IOTHUB_CLIENT_HANDLE iotHubClientHandle)                                                                                         \
    {                                                                                                                                                                               \
        SERIALIZER_DEVICETWIN_PROTOHANDLE protoHandle;                                                                                                                              \
        protoHandle.iothubClientHandleVariant.iothubClientHandleType = IOTHUB_CLIENT_CONVENIENCE_HANDLE_TYPE;                                                                       \
        protoHandle.iothubClientHandleVariant.iothubClientHandleValue.iothubClientHandle = iotHubClientHandle;                                                                      \
        return (name*)IoTHubDeviceTwinCreate_Impl(#name, sizeof(name), &protoHandle);                                                                                               \
    }                                                                                                                                                                               \
                                                                                                                                                                                    \
    static void MU_C2(IoTHubDeviceTwin_Destroy, name) (name* model)                                                                                                                    \
    {                                                                                                                                                                               \
        IoTHubDeviceTwin_Destroy_Impl(model);                                                                                                                                       \
    }                                                                                                                                                                               \
                                                                                                                                                                                    \
    static name* MU_C2(IoTHubDeviceTwin_LL_Create, name)(IOTHUB_CLIENT_LL_HANDLE iotHubClientLLHandle)                                                                                 \
    {                                                                                                                                                                               \
        SERIALIZER_DEVICETWIN_PROTOHANDLE protoHandle;                                                                                                                              \
        protoHandle.iothubClientHandleVariant.iothubClientHandleType = IOTHUB_CLIENT_LL_HANDLE_TYPE;                                                                                \
        protoHandle.iothubClientHandleVariant.iothubClientHandleValue.iothubClientLLHandle = iotHubClientLLHandle;                                                                  \
        return (name*)IoTHubDeviceTwinCreate_Impl(#name, sizeof(name), &protoHandle);                                                                                               \
    }                                                                                                                                                                               \
                                                                                                                                                                                    \
    static void MU_C2(IoTHubDeviceTwin_LL_Destroy, name) (name* model)                                                                                                                 \
    {                                                                                                                                                                               \
        IoTHubDeviceTwin_Destroy_Impl(model);                                                                                                                                       \
    }                                                                                                                                                                               \
    static IOTHUB_CLIENT_RESULT MU_C2(IoTHubDeviceTwin_LL_SendReportedState, name) (name* model, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK deviceTwinCallback, void* context)              \
    {                                                                                                                                                                               \
        return IoTHubDeviceTwin_SendReportedState_Impl(model, deviceTwinCallback, context);                                                                                         \
    }                                                                                                                                                                               \
    static IOTHUB_CLIENT_RESULT MU_C2(IoTHubDeviceTwin_SendReportedState, name) (name* model, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK deviceTwinCallback, void* context)                 \
    {                                                                                                                                                                               \
        return IoTHubDeviceTwin_SendReportedState_Impl(model, deviceTwinCallback, context);                                                                                         \
    }                                                                                                                                                                               \

#endif /*SERIALIZER_DEVICE_TWIN_H*/


