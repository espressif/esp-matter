// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTDEVICE_H
#define IOTDEVICE_H

#include <stdbool.h>
#include "azure_macro_utils/macro_utils.h"
#include "schema.h"
#include "datapublisher.h"
#include "commanddecoder.h"
#include "methodreturn.h"

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

#define DEVICE_RESULT_VALUES            \
    DEVICE_OK,                            \
    DEVICE_INVALID_ARG,                    \
    DEVICE_DATA_PUBLISHER_FAILED,        \
    DEVICE_COMMAND_DECODER_FAILED,        \
    DEVICE_ERROR

MU_DEFINE_ENUM_WITHOUT_INVALID(DEVICE_RESULT, DEVICE_RESULT_VALUES)

#include "umock_c/umock_c_prod.h"

typedef struct DEVICE_HANDLE_DATA_TAG* DEVICE_HANDLE;
typedef EXECUTE_COMMAND_RESULT (*pfDeviceActionCallback)(DEVICE_HANDLE deviceHandle, void* callbackUserContext, const char* relativeActionPath, const char* actionName, size_t argCount, const AGENT_DATA_TYPE* args);
typedef METHODRETURN_HANDLE    (*pfDeviceMethodCallback)(DEVICE_HANDLE deviceHandle, void* callbackUserContext, const char* relativeMethodPath, const char* methodName, size_t argCount, const AGENT_DATA_TYPE* args);

MOCKABLE_FUNCTION(,DEVICE_RESULT, Device_Create, SCHEMA_MODEL_TYPE_HANDLE, modelHandle, pfDeviceActionCallback, deviceActionCallback, void*, callbackUserContext, pfDeviceMethodCallback, methodCallback, void*, methodCallbackContext, bool, includePropertyPath, DEVICE_HANDLE*, deviceHandle);
MOCKABLE_FUNCTION(, void, Device_Destroy, DEVICE_HANDLE, deviceHandle);

MOCKABLE_FUNCTION(,TRANSACTION_HANDLE, Device_StartTransaction, DEVICE_HANDLE, deviceHandle);
MOCKABLE_FUNCTION(,DEVICE_RESULT, Device_PublishTransacted, TRANSACTION_HANDLE, transactionHandle, const char*, propertyPath, const AGENT_DATA_TYPE*, data);
MOCKABLE_FUNCTION(,DEVICE_RESULT, Device_EndTransaction, TRANSACTION_HANDLE, transactionHandle, unsigned char**, destination, size_t*, destinationSize);
MOCKABLE_FUNCTION(,DEVICE_RESULT, Device_CancelTransaction, TRANSACTION_HANDLE, transactionHandle);

MOCKABLE_FUNCTION(, REPORTED_PROPERTIES_TRANSACTION_HANDLE, Device_CreateTransaction_ReportedProperties, DEVICE_HANDLE, deviceHandle);
MOCKABLE_FUNCTION(, DEVICE_RESULT, Device_PublishTransacted_ReportedProperty, REPORTED_PROPERTIES_TRANSACTION_HANDLE, transactionHandle, const char*, reportedPropertyPath, const AGENT_DATA_TYPE*, data);
MOCKABLE_FUNCTION(, DEVICE_RESULT, Device_CommitTransaction_ReportedProperties, REPORTED_PROPERTIES_TRANSACTION_HANDLE, transactionHandle, unsigned char**, destination, size_t*, destinationSize);
MOCKABLE_FUNCTION(, void, Device_DestroyTransaction_ReportedProperties, REPORTED_PROPERTIES_TRANSACTION_HANDLE, transactionHandle);

MOCKABLE_FUNCTION(, EXECUTE_COMMAND_RESULT, Device_ExecuteCommand, DEVICE_HANDLE, deviceHandle, const char*, command);
MOCKABLE_FUNCTION(, METHODRETURN_HANDLE, Device_ExecuteMethod, DEVICE_HANDLE, deviceHandle, const char*, methodName, const char*, methodPayload);

MOCKABLE_FUNCTION(, DEVICE_RESULT, Device_IngestDesiredProperties, void*, startAddress, DEVICE_HANDLE, deviceHandle, const char*, jsonPayload, bool, parseDesiredNode);
#ifdef __cplusplus
}
#endif

#endif /* IOTDEVICE_H */

