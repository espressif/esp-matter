// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef DATAMARSHALLER_H
#define DATAMARSHALLER_H

#include <stdbool.h>
#include "agenttypesystem.h"
#include "schema.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/vector.h"
#ifdef __cplusplus
extern "C"
{
#endif

/*Codes_SRS_DATA_MARSHALLER_99_002:[ DataMarshaller shall have the following interface]*/
#define DATA_MARSHALLER_RESULT_VALUES           \
DATA_MARSHALLER_OK,                             \
DATA_MARSHALLER_INVALID_ARG,                    \
DATA_MARSHALLER_NO_DEVICE_IDENTITY,             \
DATA_MARSHALLER_SCHEMA_FAILED,                  \
DATA_MARSHALLER_INVALID_MODEL_PROPERTY,         \
DATA_MARSHALLER_JSON_ENCODER_ERROR,             \
DATA_MARSHALLER_ERROR,                          \
DATA_MARSHALLER_AGENT_DATA_TYPES_ERROR,         \
DATA_MARSHALLER_MULTITREE_ERROR,                \
DATA_MARSHALLER_ONLY_ONE_VALUE_ALLOWED          \

MU_DEFINE_ENUM_WITHOUT_INVALID(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_RESULT_VALUES);

typedef struct DATA_MARSHALLER_VALUE_TAG
{
    const char* PropertyPath;
    const AGENT_DATA_TYPE* Value;
} DATA_MARSHALLER_VALUE;

typedef struct DATA_MARSHALLER_HANDLE_DATA_TAG* DATA_MARSHALLER_HANDLE;
#include "umock_c/umock_c_prod.h"

MOCKABLE_FUNCTION(,DATA_MARSHALLER_HANDLE, DataMarshaller_Create, SCHEMA_MODEL_TYPE_HANDLE, modelHandle, bool, includePropertyPath);
MOCKABLE_FUNCTION(,void, DataMarshaller_Destroy, DATA_MARSHALLER_HANDLE, dataMarshallerHandle);
MOCKABLE_FUNCTION(,DATA_MARSHALLER_RESULT, DataMarshaller_SendData, DATA_MARSHALLER_HANDLE, dataMarshallerHandle, size_t, valueCount, const DATA_MARSHALLER_VALUE*, values, unsigned char**, destination, size_t*, destinationSize);

MOCKABLE_FUNCTION(, DATA_MARSHALLER_RESULT, DataMarshaller_SendData_ReportedProperties, DATA_MARSHALLER_HANDLE, dataMarshallerHandle, VECTOR_HANDLE, values, unsigned char**, destination, size_t*, destinationSize);

#ifdef __cplusplus
}
#endif

#endif /* DATAMARSHALLER_H */
