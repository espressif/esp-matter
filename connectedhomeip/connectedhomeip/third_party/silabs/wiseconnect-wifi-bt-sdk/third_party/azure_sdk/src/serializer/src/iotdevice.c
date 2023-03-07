// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"

#include <stdbool.h>
#include "iotdevice.h"
#include "datapublisher.h"
#include "commanddecoder.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"

#define LOG_DEVICE_ERROR \
    LogError("(result = %s)", MU_ENUM_TO_STRING(DEVICE_RESULT, result))



typedef struct DEVICE_HANDLE_DATA_TAG
{
    SCHEMA_MODEL_TYPE_HANDLE model;
    DATA_PUBLISHER_HANDLE dataPublisherHandle;
    pfDeviceActionCallback deviceActionCallback;
    void* callbackUserContext;
    pfDeviceMethodCallback deviceMethodCallback;
    void* methodCallbackUserContext;

    COMMAND_DECODER_HANDLE commandDecoderHandle;
} DEVICE_HANDLE_DATA;

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(DEVICE_RESULT, DEVICE_RESULT_VALUES);

static EXECUTE_COMMAND_RESULT DeviceInvokeAction(void* actionCallbackContext, const char* relativeActionPath, const char* actionName, size_t argCount, const AGENT_DATA_TYPE* args)
{
    EXECUTE_COMMAND_RESULT result;

    /*Codes_SRS_DEVICE_02_011: [If the parameter actionCallbackContent passed the callback is NULL then the callback shall return EXECUTE_COMMAND_ERROR.] */
    if (actionCallbackContext == NULL)
    {
        result = EXECUTE_COMMAND_ERROR;
        LogError("(Error code = %s)", MU_ENUM_TO_STRING(DEVICE_RESULT, DEVICE_INVALID_ARG));
    }
    else
    {
        DEVICE_HANDLE_DATA* device = (DEVICE_HANDLE_DATA*)actionCallbackContext;

        /* Codes_SRS_DEVICE_01_052: [When the action callback passed to CommandDecoder is called, Device shall call the appropriate user callback associated with the device handle.] */
        /* Codes_SRS_DEVICE_01_055: [The value passed in callbackUserContext when creating the device shall be passed to the callback as the value for the callbackUserContext argument.] */
        result = device->deviceActionCallback((DEVICE_HANDLE)device, device->callbackUserContext, relativeActionPath, actionName, argCount, args);
    }

    return result;
}

static METHODRETURN_HANDLE DeviceInvokeMethod(void* methodCallbackContext, const char* relativeMethodPath, const char* methodName, size_t argCount, const AGENT_DATA_TYPE* args)
{
    METHODRETURN_HANDLE result;

    if (methodCallbackContext == NULL)
    {
        result = NULL;
        LogError("(Error code = %s)", MU_ENUM_TO_STRING(DEVICE_RESULT, DEVICE_INVALID_ARG));
    }
    else
    {
        DEVICE_HANDLE_DATA* device = (DEVICE_HANDLE_DATA*)methodCallbackContext;

        result = device->deviceMethodCallback((DEVICE_HANDLE)device, device->methodCallbackUserContext, relativeMethodPath, methodName, argCount, args);
    }

    return result;
}

DEVICE_RESULT Device_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, pfDeviceActionCallback deviceActionCallback, void* callbackUserContext, pfDeviceMethodCallback methodCallback, void* methodCallbackContext, bool includePropertyPath, DEVICE_HANDLE* deviceHandle)
{
    DEVICE_RESULT result;

    /* Codes_SRS_DEVICE_05_014: [If any of the modelHandle, deviceHandle or deviceActionCallback arguments are NULL, Device_Create shall return DEVICE_INVALID_ARG.]*/
    if (modelHandle == NULL || deviceHandle == NULL || deviceActionCallback == NULL || methodCallback == NULL || methodCallbackContext == NULL)
    {
        result = DEVICE_INVALID_ARG;
        LOG_DEVICE_ERROR;
    }
    else
    {
        DEVICE_HANDLE_DATA* device = (DEVICE_HANDLE_DATA*)malloc(sizeof(DEVICE_HANDLE_DATA));
        if (device == NULL)
        {
            /* Codes_SRS_DEVICE_05_015: [If an error occurs while trying to create the device, Device_Create shall return DEVICE_ERROR.] */
            result = DEVICE_ERROR;
            LOG_DEVICE_ERROR;
        }
        else
        {
            /* Codes_SRS_DEVICE_01_018: [Device_Create shall create a DataPublisher instance by calling DataPublisher_Create.] */
            /* Codes_SRS_DEVICE_01_020: [Device_Create shall pass to DataPublisher_Create the FrontDoor instance obtained earlier.] */
            /* Codes_SRS_DEVICE_01_004: [DeviceCreate shall pass to DataPublisher_create the includePropertyPath argument.] */
            if ((device->dataPublisherHandle = DataPublisher_Create(modelHandle, includePropertyPath)) == NULL)
            {
                free(device);

                /* Codes_SRS_DEVICE_01_019: [If creating the DataPublisher instance fails, Device_Create shall return DEVICE_DATA_PUBLISHER_FAILED.] */
                result = DEVICE_DATA_PUBLISHER_FAILED;
                LOG_DEVICE_ERROR;
            }
            else
            {
                device->model = modelHandle;
                device->deviceActionCallback = deviceActionCallback;
                device->callbackUserContext = callbackUserContext;
                device->deviceMethodCallback = methodCallback;
                device->methodCallbackUserContext = methodCallbackContext;

                /* Codes_SRS_DEVICE_01_001: [Device_Create shall create a CommandDecoder instance by calling CommandDecoder_Create and passing to it the model handle.] */
                /* Codes_SRS_DEVICE_01_002: [Device_Create shall also pass to CommandDecoder_Create a callback to be invoked when a command is received and a context that shall be the device handle.] */
                if ((device->commandDecoderHandle = CommandDecoder_Create(modelHandle, DeviceInvokeAction, device, DeviceInvokeMethod, device)) == NULL)
                {
                    DataPublisher_Destroy(device->dataPublisherHandle);
                    free(device);

                    /* Codes_SRS_DEVICE_01_003: [If CommandDecoder_Create fails, Device_Create shall return DEVICE_COMMAND_DECODER_FAILED.] */
                    result = DEVICE_COMMAND_DECODER_FAILED;
                    LOG_DEVICE_ERROR;
                }
                else
                {
                    /* Codes_SRS_DEVICE_03_003: [The DEVICE_HANDLE shall be provided via the deviceHandle out argument.] */
                    *deviceHandle = (DEVICE_HANDLE)device;
                    /* Codes_SRS_DEVICE_03_004: [Device_Create shall return DEVICE_OK upon success.] */
                    result = DEVICE_OK;
                }
            }
        }
    }

    return result;
}

/* Codes_SRS_DEVICE_03_006: [Device_Destroy shall free all resources associated with a device.] */
void Device_Destroy(DEVICE_HANDLE deviceHandle)
{
    /* Codes_SRS_DEVICE_03_007: [Device_Destroy will not do anything if deviceHandle is NULL.] */
    if (deviceHandle != NULL)
    {
        DEVICE_HANDLE_DATA* device = (DEVICE_HANDLE_DATA*)deviceHandle;

        DataPublisher_Destroy(device->dataPublisherHandle);
        CommandDecoder_Destroy(device->commandDecoderHandle);

        free(device);
    }
}

TRANSACTION_HANDLE Device_StartTransaction(DEVICE_HANDLE deviceHandle)
{
    TRANSACTION_HANDLE result;

    /* Codes_SRS_DEVICE_01_035: [If any argument is NULL, Device_StartTransaction shall return NULL.] */
    if (deviceHandle == NULL)
    {
        result = NULL;
        LogError("(Error code = %s)", MU_ENUM_TO_STRING(DEVICE_RESULT, DEVICE_INVALID_ARG));
    }
    else
    {
        DEVICE_HANDLE_DATA* deviceInstance = (DEVICE_HANDLE_DATA*)deviceHandle;

        /* Codes_SRS_DEVICE_01_034: [Device_StartTransaction shall invoke DataPublisher_StartTransaction for the DataPublisher handle associated with the deviceHandle argument.] */
        /* Codes_SRS_DEVICE_01_043: [On success, Device_StartTransaction shall return a non NULL handle.] */
        /* Codes_SRS_DEVICE_01_048: [When DataPublisher_StartTransaction fails, Device_StartTransaction shall return NULL.] */
        result = DataPublisher_StartTransaction(deviceInstance->dataPublisherHandle);
    }

    return result;
}

DEVICE_RESULT Device_PublishTransacted(TRANSACTION_HANDLE transactionHandle, const char* propertyPath, const AGENT_DATA_TYPE* data)
{
    DEVICE_RESULT result;

    /* Codes_SRS_DEVICE_01_037: [If any argument is NULL, Device_PublishTransacted shall return DEVICE_INVALID_ARG.] */
    if (
        (transactionHandle == NULL) ||
        (propertyPath == NULL) ||
        (data == NULL)
       )
    {
        result = DEVICE_INVALID_ARG;
        LOG_DEVICE_ERROR;
    }
    /* Codes_SRS_DEVICE_01_036: [Device_PublishTransacted shall invoke DataPublisher_PublishTransacted.] */
    else if (DataPublisher_PublishTransacted(transactionHandle, propertyPath, data) != DATA_PUBLISHER_OK)
    {
        /* Codes_SRS_DEVICE_01_049: [When DataPublisher_PublishTransacted fails, Device_PublishTransacted shall return DEVICE_DATA_PUBLISHER_FAILED.] */
        result = DEVICE_DATA_PUBLISHER_FAILED;
        LOG_DEVICE_ERROR;
    }
    else
    {
        /* Codes_SRS_DEVICE_01_044: [On success, Device_PublishTransacted shall return DEVICE_OK.] */
        result = DEVICE_OK;
    }

    return result;
}

DEVICE_RESULT Device_EndTransaction(TRANSACTION_HANDLE transactionHandle, unsigned char** destination, size_t* destinationSize)
{
    DEVICE_RESULT result;

    /* Codes_SRS_DEVICE_01_039: [If any parameter is NULL, Device_EndTransaction shall return DEVICE_INVALID_ARG.]*/
    if (
        (transactionHandle == NULL) ||
        (destination == NULL) ||
        (destinationSize == NULL)
        )
    {
        result = DEVICE_INVALID_ARG;
        LOG_DEVICE_ERROR;
    }
    /* Codes_SRS_DEVICE_01_038: [Device_EndTransaction shall invoke DataPublisher_EndTransaction.] */
    else if (DataPublisher_EndTransaction(transactionHandle, destination, destinationSize) != DATA_PUBLISHER_OK)
    {
        /* Codes_SRS_DEVICE_01_050: [When DataPublisher_EndTransaction fails, Device_EndTransaction shall return DEVICE_DATA_PUBLISHER_FAILED.] */
        result = DEVICE_DATA_PUBLISHER_FAILED;
        LOG_DEVICE_ERROR;
    }
    else
    {
        /* Codes_SRS_DEVICE_01_045: [On success, Device_EndTransaction shall return DEVICE_OK.] */
        result = DEVICE_OK;
    }

    return result;
}

DEVICE_RESULT Device_CancelTransaction(TRANSACTION_HANDLE transactionHandle)
{
    DEVICE_RESULT result;

    /* Codes_SRS_DEVICE_01_041: [If any argument is NULL, Device_CancelTransaction shall return DEVICE_INVALID_ARG.] */
    if (transactionHandle == NULL)
    {
        result = DEVICE_INVALID_ARG;
        LOG_DEVICE_ERROR;
    }
    /* Codes_SRS_DEVICE_01_040: [Device_CancelTransaction shall invoke DataPublisher_CancelTransaction.] */
    else if (DataPublisher_CancelTransaction(transactionHandle) != DATA_PUBLISHER_OK)
    {
        /* Codes_SRS_DEVICE_01_051: [When DataPublisher_CancelTransaction fails, Device_CancelTransaction shall return DEVICE_DATA_PUBLISHER_FAILED.] */
        result = DEVICE_DATA_PUBLISHER_FAILED;
        LOG_DEVICE_ERROR;
    }
    else
    {
        /* Codes_SRS_DEVICE_01_046: [On success, Device_PublishTransacted shall return DEVICE_OK.] */
        result = DEVICE_OK;
    }

    return result;
}

EXECUTE_COMMAND_RESULT Device_ExecuteCommand(DEVICE_HANDLE deviceHandle, const char* command)
{
    EXECUTE_COMMAND_RESULT result;
    /*Codes_SRS_DEVICE_02_012: [If any parameters are NULL, then Device_ExecuteCommand shall return EXECUTE_COMMAND_ERROR.] */
    if (
        (deviceHandle == NULL) ||
        (command == NULL)
        )
    {
        result = EXECUTE_COMMAND_ERROR;
        LogError("invalid parameter (NULL passed to Device_ExecuteCommand DEVICE_HANDLE deviceHandle=%p, const char* command=%p", deviceHandle, command);
    }
    else
    {
        /*Codes_SRS_DEVICE_02_013: [Otherwise, Device_ExecuteCommand shall call CommandDecoder_ExecuteCommand and return what CommandDecoder_ExecuteCommand is returning.] */
        DEVICE_HANDLE_DATA* device = (DEVICE_HANDLE_DATA*)deviceHandle;
        result = CommandDecoder_ExecuteCommand(device->commandDecoderHandle, command);
    }
    return result;
}

METHODRETURN_HANDLE Device_ExecuteMethod(DEVICE_HANDLE deviceHandle, const char* methodName, const char* methodPayload)
{
    METHODRETURN_HANDLE result;
    if (
        (deviceHandle == NULL) ||
        (methodName == NULL) /*methodPayload can be NULL*/
        )
    {
        result = NULL;
        LogError("invalid parameter (NULL passed to Device_ExecuteMethod DEVICE_HANDLE deviceHandle=%p, const char* methodPayload=%p", deviceHandle, methodPayload);
    }
    else
    {
        DEVICE_HANDLE_DATA* device = (DEVICE_HANDLE_DATA*)deviceHandle;
        result = CommandDecoder_ExecuteMethod(device->commandDecoderHandle, methodName, methodPayload);
    }
    return result;
}

REPORTED_PROPERTIES_TRANSACTION_HANDLE Device_CreateTransaction_ReportedProperties(DEVICE_HANDLE deviceHandle)
{
    REPORTED_PROPERTIES_TRANSACTION_HANDLE result;

    /*Codes_SRS_DEVICE_02_014: [ If argument deviceHandle is NULL then Device_CreateTransaction_ReportedProperties shall fail and return NULL. ]*/
    if (deviceHandle == NULL)
    {
        LogError("invalid argument DEVICE_HANDLE deviceHandle=%p", deviceHandle);
        result = NULL;
    }
    else
    {
        DEVICE_HANDLE_DATA* deviceInstance = (DEVICE_HANDLE_DATA*)deviceHandle;
        /*Codes_SRS_DEVICE_02_015: [ Otherwise, Device_CreateTransaction_ReportedProperties shall call DataPublisher_CreateTransaction_ReportedProperties. ]*/
        /*Codes_SRS_DEVICE_02_016: [ If DataPublisher_CreateTransaction_ReportedProperties fails then Device_CreateTransaction_ReportedProperties shall fail and return NULL. ]*/
        /*Codes_SRS_DEVICE_02_017: [ Otherwise Device_CreateTransaction_ReportedProperties shall succeed and return a non-NULL value. ]*/
        result = DataPublisher_CreateTransaction_ReportedProperties(deviceInstance->dataPublisherHandle);
        if (result == NULL)
        {
            LogError("unable to DataPublisher_CreateTransaction_ReportedProperties");
            /*return as is*/
        }
        else
        {
            /*return as is*/
        }
    }

    return result;
}

DEVICE_RESULT Device_PublishTransacted_ReportedProperty(REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle, const char* reportedPropertyPath, const AGENT_DATA_TYPE* data)
{
    DEVICE_RESULT result;
    /*Codes_SRS_DEVICE_02_018: [ If argument transactionHandle is NULL then Device_PublishTransacted_ReportedProperty shall fail and return DEVICE_INVALID_ARG. ]*/
    /*Codes_SRS_DEVICE_02_019: [ If argument reportedPropertyPath is NULL then Device_PublishTransacted_ReportedProperty shall fail and return DEVICE_INVALID_ARG. ]*/
    /*Codes_SRS_DEVICE_02_020: [ If argument data is NULL then Device_PublishTransacted_ReportedProperty shall fail and return DEVICE_INVALID_ARG. ]*/
    if (
        (transactionHandle == NULL) ||
        (reportedPropertyPath == NULL) ||
        (data == NULL)
        )
    {
        LogError("invalid argument REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle=%p, const char* reportedPropertyPath=%s, const AGENT_DATA_TYPE* data=%p", transactionHandle, reportedPropertyPath, data);
        result = DEVICE_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_DEVICE_02_021: [ Device_PublishTransacted_ReportedProperty shall call DataPublisher_PublishTransacted_ReportedProperty. ]*/
        DATA_PUBLISHER_RESULT r = DataPublisher_PublishTransacted_ReportedProperty(transactionHandle, reportedPropertyPath, data);
        if (r != DATA_PUBLISHER_OK)
        {
            LogError("unable to DataPublisher_PublishTransacted_ReportedProperty");
            /*Codes_SRS_DEVICE_02_022: [ If DataPublisher_PublishTransacted_ReportedProperty fails then Device_PublishTransacted_ReportedProperty shall fail and return DEVICE_DATA_PUBLISHER_FAILED. ]*/
            result = DEVICE_DATA_PUBLISHER_FAILED;
        }
        else
        {
            /*Codes_SRS_DEVICE_02_023: [ Otherwise, Device_PublishTransacted_ReportedProperty shall succeed and return DEVICE_OK. ]*/
            result = DEVICE_OK;
        }
    }
    return result;
}

DEVICE_RESULT Device_CommitTransaction_ReportedProperties(REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle, unsigned char** destination, size_t* destinationSize)
{
    DEVICE_RESULT result;

    /*Codes_SRS_DEVICE_02_024: [ If argument transactionHandle is NULL then Device_CommitTransaction_ReportedProperties shall fail and return DEVICE_INVALID_ARG. ]*/
    /*Codes_SRS_DEVICE_02_025: [ If argument destination is NULL then Device_CommitTransaction_ReportedProperties shall fail and return DEVICE_INVALID_ARG. ]*/
    /*Codes_SRS_DEVICE_02_026: [ If argument destinationSize is NULL then Device_CommitTransaction_ReportedProperties shall fail and return DEVICE_INVALID_ARG. ]*/
    if (
        (transactionHandle == NULL) ||
        (destination == NULL) ||
        (destinationSize == NULL)
        )
    {
        LogError("invalid argument REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle=%p, unsigned char** destination=%p, size_t* destinationSize=%p", transactionHandle, destination, destinationSize);
        result = DEVICE_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_DEVICE_02_027: [ Device_CommitTransaction_ReportedProperties shall call DataPublisher_CommitTransaction_ReportedProperties. ]*/
        DATA_PUBLISHER_RESULT r = DataPublisher_CommitTransaction_ReportedProperties(transactionHandle, destination, destinationSize);

        /*Codes_SRS_DEVICE_02_028: [ If DataPublisher_CommitTransaction_ReportedProperties fails then Device_CommitTransaction_ReportedProperties shall fail and return DEVICE_DATA_PUBLISHER_FAILED. ]*/
        if (r != DATA_PUBLISHER_OK)
        {
            LogError("unable to DataPublisher_CommitTransaction_ReportedProperties");
            result = DEVICE_DATA_PUBLISHER_FAILED;
        }
        else
        {
            /*Codes_SRS_DEVICE_02_029: [ Otherwise Device_CommitTransaction_ReportedProperties shall succeed and return DEVICE_OK. ]*/
            result = DEVICE_OK;
        }
    }
    return result;
}

void Device_DestroyTransaction_ReportedProperties(REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle)
{
    /*Codes_SRS_DEVICE_02_030: [ If argument transactionHandle is NULL then Device_DestroyTransaction_ReportedProperties shall return. ]*/
    if (transactionHandle == NULL)
    {
        LogError("invalid argument REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle=%p", transactionHandle);
    }
    else
    {
        /*Codes_SRS_DEVICE_02_031: [ Otherwise Device_DestroyTransaction_ReportedProperties shall free all used resources. ]*/
        DataPublisher_DestroyTransaction_ReportedProperties(transactionHandle);
    }
}

DEVICE_RESULT Device_IngestDesiredProperties(void* startAddress, DEVICE_HANDLE deviceHandle, const char* jsonPayload, bool parseDesiredNode)
{
    DEVICE_RESULT result;
    /*Codes_SRS_DEVICE_02_032: [ If deviceHandle is NULL then Device_IngestDesiredProperties shall fail and return DEVICE_INVALID_ARG. ]*/
    /*Codes_SRS_DEVICE_02_033: [ If jsonPayload is NULL then Device_IngestDesiredProperties shall fail and return DEVICE_INVALID_ARG. ]*/
    /*Codes_SRS_DEVICE_02_037: [ If startAddress is NULL then Device_IngestDesiredProperties shall fail and return DEVICE_INVALID_ARG. ]*/
    if (
        (deviceHandle == NULL) ||
        (jsonPayload == NULL) ||
        (startAddress == NULL)
        )
    {
        LogError("invalid argument void* startAddress=%p, DEVICE_HANDLE deviceHandle=%p, const char* jsonPayload=%p\n", startAddress, deviceHandle, jsonPayload);
        result = DEVICE_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_DEVICE_02_034: [ Device_IngestDesiredProperties shall call CommandDecoder_IngestDesiredProperties. ]*/
        DEVICE_HANDLE_DATA* device = (DEVICE_HANDLE_DATA*)deviceHandle;
        if (CommandDecoder_IngestDesiredProperties(startAddress, device->commandDecoderHandle, jsonPayload, parseDesiredNode) != EXECUTE_COMMAND_SUCCESS)
        {
            /*Codes_SRS_DEVICE_02_035: [ If any failure happens then Device_IngestDesiredProperties shall fail and return DEVICE_ERROR. ]*/
            LogError("failure in CommandDecoder_IngestDesiredProperties");
            result = DEVICE_ERROR;
        }
        else
        {
            /*Codes_SRS_DEVICE_02_036: [ Otherwise, Device_IngestDesiredProperties shall succeed and return DEVICE_OK. ]*/
            result = DEVICE_OK;
        }
    }
    return result;
}
