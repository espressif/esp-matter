// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef DATA_PUBLISHER_H
#define DATA_PUBLISHER_H

#include "agenttypesystem.h"
#include "schema.h"
/* Normally we could include <stdbool> for cpp, but some toolchains are not well behaved and simply don't have it - ARM CC for example */
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DATA_PUBLISHER_RESULT_VALUES    \
DATA_PUBLISHER_OK,                      \
DATA_PUBLISHER_INVALID_ARG,             \
DATA_PUBLISHER_MARSHALLER_ERROR,        \
DATA_PUBLISHER_EMPTY_TRANSACTION,       \
DATA_PUBLISHER_AGENT_DATA_TYPES_ERROR,  \
DATA_PUBLISHER_SCHEMA_FAILED,           \
DATA_PUBLISHER_BUFFER_STORAGE_ERROR,    \
DATA_PUBLISHER_ERROR

MU_DEFINE_ENUM_WITHOUT_INVALID(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_RESULT_VALUES);

#include "umock_c/umock_c_prod.h"

typedef struct TRANSACTION_HANDLE_DATA_TAG* TRANSACTION_HANDLE;
typedef struct REPORTED_PROPERTIES_TRANSACTION_HANDLE_DATA_TAG* REPORTED_PROPERTIES_TRANSACTION_HANDLE;
typedef struct DATA_PUBLISHER_HANDLE_DATA_TAG* DATA_PUBLISHER_HANDLE;

MOCKABLE_FUNCTION(,DATA_PUBLISHER_HANDLE, DataPublisher_Create, SCHEMA_MODEL_TYPE_HANDLE, modelHandle, bool, includePropertyPath);
MOCKABLE_FUNCTION(,void, DataPublisher_Destroy, DATA_PUBLISHER_HANDLE, dataPublisherHandle);

MOCKABLE_FUNCTION(,TRANSACTION_HANDLE, DataPublisher_StartTransaction, DATA_PUBLISHER_HANDLE, dataPublisherHandle);
MOCKABLE_FUNCTION(,DATA_PUBLISHER_RESULT, DataPublisher_PublishTransacted, TRANSACTION_HANDLE, transactionHandle, const char*, propertyPath, const AGENT_DATA_TYPE*, data);
MOCKABLE_FUNCTION(,DATA_PUBLISHER_RESULT, DataPublisher_EndTransaction, TRANSACTION_HANDLE, transactionHandle, unsigned char**, destination, size_t*, destinationSize);
MOCKABLE_FUNCTION(,DATA_PUBLISHER_RESULT, DataPublisher_CancelTransaction, TRANSACTION_HANDLE, transactionHandle);
MOCKABLE_FUNCTION(,void, DataPublisher_SetMaxBufferSize, size_t, value);
MOCKABLE_FUNCTION(,size_t, DataPublisher_GetMaxBufferSize);

MOCKABLE_FUNCTION(, REPORTED_PROPERTIES_TRANSACTION_HANDLE, DataPublisher_CreateTransaction_ReportedProperties, DATA_PUBLISHER_HANDLE, dataPublisherHandle);
MOCKABLE_FUNCTION(, DATA_PUBLISHER_RESULT, DataPublisher_PublishTransacted_ReportedProperty, REPORTED_PROPERTIES_TRANSACTION_HANDLE, transactionHandle, const char*, reportedPropertyPath, const AGENT_DATA_TYPE*, data);
MOCKABLE_FUNCTION(, DATA_PUBLISHER_RESULT, DataPublisher_CommitTransaction_ReportedProperties, REPORTED_PROPERTIES_TRANSACTION_HANDLE, transactionHandle, unsigned char**, destination, size_t*, destinationSize);
MOCKABLE_FUNCTION(, void, DataPublisher_DestroyTransaction_ReportedProperties, REPORTED_PROPERTIES_TRANSACTION_HANDLE, transactionHandle);


#ifdef __cplusplus
}
#endif

#endif /* DATA_PUBLISHER_H */
