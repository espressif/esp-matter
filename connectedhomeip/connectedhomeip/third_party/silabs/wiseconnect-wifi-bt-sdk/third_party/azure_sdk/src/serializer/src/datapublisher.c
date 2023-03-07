// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"

#include <stdbool.h>
#include "datapublisher.h"
#include "jsonencoder.h"
#include "datamarshaller.h"
#include "agenttypesystem.h"
#include "schema.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/vector.h"

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_RESULT_VALUES)

#define LOG_DATA_PUBLISHER_ERROR \
    LogError("(result = %s)", MU_ENUM_TO_STRING(DATA_PUBLISHER_RESULT, result))

#define DEFAULT_MAX_BUFFER_SIZE 10240
/* Codes_SRS_DATA_PUBLISHER_99_066:[ A single value shall be used by all instances of DataPublisher.] */
/* Codes_SRS_DATA_PUBLISHER_99_067:[ Before any call to DataPublisher_SetMaxBufferSize, the default max buffer size shall be equal to 10KB.] */
static size_t maxBufferSize_ = DEFAULT_MAX_BUFFER_SIZE;

typedef struct DATA_PUBLISHER_HANDLE_DATA_TAG
{
    DATA_MARSHALLER_HANDLE DataMarshallerHandle;
    SCHEMA_MODEL_TYPE_HANDLE ModelHandle;
} DATA_PUBLISHER_HANDLE_DATA;

typedef struct TRANSACTION_HANDLE_DATA_TAG
{
    DATA_PUBLISHER_HANDLE_DATA* DataPublisherInstance;
    size_t ValueCount;
    DATA_MARSHALLER_VALUE* Values;
} TRANSACTION_HANDLE_DATA;

typedef struct REPORTED_PROPERTIES_TRANSACTION_HANDLE_DATA_TAG
{
    DATA_PUBLISHER_HANDLE_DATA* DataPublisherInstance;
    VECTOR_HANDLE value; /*holds (DATA_MARSHALLER_VALUE*) */
}REPORTED_PROPERTIES_TRANSACTION_HANDLE_DATA;

DATA_PUBLISHER_HANDLE DataPublisher_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, bool includePropertyPath)
{
    DATA_PUBLISHER_HANDLE_DATA* result;

    /* Codes_SRS_DATA_PUBLISHER_99_042:[ If a NULL argument is passed to it, DataPublisher_Create shall return NULL.] */
    if (modelHandle == NULL)
    {
        result = NULL;
        LogError("(result = %s)", MU_ENUM_TO_STRING(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG));
    }
    else if ((result = (DATA_PUBLISHER_HANDLE_DATA*)malloc(sizeof(DATA_PUBLISHER_HANDLE_DATA))) == NULL)
    {
        /* Codes_SRS_DATA_PUBLISHER_99_047:[ For any other error not specified here, DataPublisher_Create shall return NULL.] */
        result = NULL;
        LogError("(result = %s)", MU_ENUM_TO_STRING(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_ERROR));
    }
    else
    {
        /* Codes_SRS_DATA_PUBLISHER_99_043:[ DataPublisher_Create shall initialize and hold a handle to a DataMarshaller instance.] */
        /* Codes_SRS_DATA_PUBLISHER_01_001: [DataPublisher_Create shall pass the includePropertyPath argument to DataMarshaller_Create.] */
        if ((result->DataMarshallerHandle = DataMarshaller_Create(modelHandle, includePropertyPath)) == NULL)
        {
            free(result);

            /* Codes_SRS_DATA_PUBLISHER_99_044:[ If the creation of the DataMarshaller instance fails, DataPublisher_Create shall return NULL.] */
            result = NULL;
            LogError("(result = %s)", MU_ENUM_TO_STRING(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_MARSHALLER_ERROR));
        }
        else
        {
            /* Codes_SRS_DATA_PUBLISHER_99_041:[ DataPublisher_Create shall create a new DataPublisher instance and return a non-NULL handle in case of success.] */
            result->ModelHandle = modelHandle;
        }
    }

    return result;
}

void DataPublisher_Destroy(DATA_PUBLISHER_HANDLE dataPublisherHandle)
{
    if (dataPublisherHandle != NULL)
    {
        DATA_PUBLISHER_HANDLE_DATA* dataPublisherInstance = (DATA_PUBLISHER_HANDLE_DATA*)dataPublisherHandle;
        DataMarshaller_Destroy(dataPublisherInstance->DataMarshallerHandle);

        free(dataPublisherHandle);
    }
}

TRANSACTION_HANDLE DataPublisher_StartTransaction(DATA_PUBLISHER_HANDLE dataPublisherHandle)
{
    TRANSACTION_HANDLE_DATA* transaction;

    /* Codes_SRS_DATA_PUBLISHER_99_038:[ If DataPublisher_StartTransaction is called with a NULL argument it shall return NULL.] */
    if (dataPublisherHandle == NULL)
    {
        transaction = NULL;
        LogError("(Error code: %s)", MU_ENUM_TO_STRING(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_DATA_PUBLISHER_99_007:[ A call to DataPublisher_StartTransaction shall start a new transaction.] */
        transaction = (TRANSACTION_HANDLE_DATA*)calloc(1, sizeof(TRANSACTION_HANDLE_DATA));
        if (transaction == NULL)
        {
            LogError("Allocating transaction failed (Error code: %s)", MU_ENUM_TO_STRING(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_ERROR));
        }
        else
        {
            transaction->ValueCount = 0;
            transaction->Values = NULL;
            transaction->DataPublisherInstance = (DATA_PUBLISHER_HANDLE_DATA*)dataPublisherHandle;
        }
    }

    /* Codes_SRS_DATA_PUBLISHER_99_008:[ DataPublisher_StartTransaction shall return a non-NULL handle upon success.] */
    /* Codes_SRS_DATA_PUBLISHER_99_009:[ DataPublisher_StartTransaction shall return NULL upon failure.] */
    return transaction;
}

DATA_PUBLISHER_RESULT DataPublisher_PublishTransacted(TRANSACTION_HANDLE transactionHandle, const char* propertyPath, const AGENT_DATA_TYPE* data)
{
    DATA_PUBLISHER_RESULT result;
    char* propertyPathCopy;

    /* Codes_SRS_DATA_PUBLISHER_99_017:[ When one or more NULL parameter(s) are specified, DataPublisher_PublishTransacted is called with a NULL transactionHandle, it shall return DATA_PUBLISHER_INVALID_ARG.] */
    if ((transactionHandle == NULL) ||
        (propertyPath == NULL) ||
        (data == NULL))
    {
        result = DATA_PUBLISHER_INVALID_ARG;
        LOG_DATA_PUBLISHER_ERROR;
    }
    else if (mallocAndStrcpy_s(&propertyPathCopy, propertyPath) != 0)
    {
        /* Codes_SRS_DATA_PUBLISHER_99_020:[ For any errors not explicitly mentioned here the DataPublisher APIs shall return DATA_PUBLISHER_ERROR.] */
        result = DATA_PUBLISHER_ERROR;
        LOG_DATA_PUBLISHER_ERROR;
    }
    else
    {
        TRANSACTION_HANDLE_DATA* transaction = (TRANSACTION_HANDLE_DATA*)transactionHandle;
        AGENT_DATA_TYPE* propertyValue;

        if (!Schema_ModelPropertyByPathExists(transaction->DataPublisherInstance->ModelHandle, propertyPath))
        {
            free(propertyPathCopy);

            /* Codes_SRS_DATA_PUBLISHER_99_040:[ When propertyPath does not exist in the supplied model, DataPublisher_Publish shall return DATA_PUBLISHER_SCHEMA_FAILED without dispatching data.] */
            result = DATA_PUBLISHER_SCHEMA_FAILED;
            LOG_DATA_PUBLISHER_ERROR;
        }
        else if ((propertyValue = (AGENT_DATA_TYPE*)calloc(1, sizeof(AGENT_DATA_TYPE))) == NULL)
        {
            free(propertyPathCopy);

            /* Codes_SRS_DATA_PUBLISHER_99_020:[ For any errors not explicitly mentioned here the DataPublisher APIs shall return DATA_PUBLISHER_ERROR.] */
            result = DATA_PUBLISHER_ERROR;
            LOG_DATA_PUBLISHER_ERROR;
        }
        else if (Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(propertyValue, data) != AGENT_DATA_TYPES_OK)
        {
            free(propertyPathCopy);
            free(propertyValue);

            /* Codes_SRS_DATA_PUBLISHER_99_028:[ If creating the copy fails then DATA_PUBLISHER_AGENT_DATA_TYPES_ERROR shall be returned.] */
            result = DATA_PUBLISHER_AGENT_DATA_TYPES_ERROR;
            LOG_DATA_PUBLISHER_ERROR;
        }
        else
        {
            size_t i;
            DATA_MARSHALLER_VALUE* propertySlot = NULL;

            /* Codes_SRS_DATA_PUBLISHER_99_019:[ If the same property is associated twice with a transaction, then the last value shall be kept associated with the transaction.] */
            for (i = 0; i < transaction->ValueCount; i++)
            {
                if (strcmp(transaction->Values[i].PropertyPath, propertyPath) == 0)
                {
                    propertySlot = &transaction->Values[i];
                    break;
                }
            }

            if (propertySlot == NULL)
            {
                DATA_MARSHALLER_VALUE* newValues = (DATA_MARSHALLER_VALUE*)realloc(transaction->Values, sizeof(DATA_MARSHALLER_VALUE)* (transaction->ValueCount + 1));
                if (newValues != NULL)
                {
                    transaction->Values = newValues;
                    propertySlot = &transaction->Values[transaction->ValueCount];
                    propertySlot->Value = NULL;
                    propertySlot->PropertyPath = NULL;
                    transaction->ValueCount++;
                }
            }

            if (propertySlot == NULL)
            {
                Destroy_AGENT_DATA_TYPE((AGENT_DATA_TYPE*)propertyValue);
                free(propertyValue);
                free(propertyPathCopy);

                /* Codes_SRS_DATA_PUBLISHER_99_020:[ For any errors not explicitly mentioned here the DataPublisher APIs shall return DATA_PUBLISHER_ERROR.] */
                result = DATA_PUBLISHER_ERROR;
                LOG_DATA_PUBLISHER_ERROR;
            }
            else
            {
                if (propertySlot->Value != NULL)
                {
                    Destroy_AGENT_DATA_TYPE((AGENT_DATA_TYPE*)propertySlot->Value);
                    free((AGENT_DATA_TYPE*)propertySlot->Value);
                }
                if (propertySlot->PropertyPath != NULL)
                {
                    char* existingValue = (char*)propertySlot->PropertyPath;
                    free(existingValue);
                }

                /* Codes_SRS_DATA_PUBLISHER_99_016:[ When DataPublisher_PublishTransacted is invoked, DataPublisher shall associate the data with the transaction identified by the transactionHandle argument and return DATA_PUBLISHER_OK. No data shall be dispatched at the time of the call.] */
                propertySlot->PropertyPath = propertyPathCopy;
                propertySlot->Value = propertyValue;

                result = DATA_PUBLISHER_OK;
            }
        }
    }

    return result;
}

DATA_PUBLISHER_RESULT DataPublisher_EndTransaction(TRANSACTION_HANDLE transactionHandle, unsigned char** destination, size_t* destinationSize)
{
    DATA_PUBLISHER_RESULT result;

    /*Codes_SRS_DATA_PUBLISHER_02_006: [If the destination argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
    /*Codes_SRS_DATA_PUBLISHER_02_007: [If the destinationSize argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
    if (
        (transactionHandle == NULL) ||
        (destination == NULL) ||
        (destinationSize == NULL)
        )
    {
        /* Codes_SRS_DATA_PUBLISHER_99_011:[ If the transactionHandle argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
        result = DATA_PUBLISHER_INVALID_ARG;
        LOG_DATA_PUBLISHER_ERROR;
    }
    else
    {
        TRANSACTION_HANDLE_DATA* transaction = (TRANSACTION_HANDLE_DATA*)transactionHandle;

        if (transaction->ValueCount == 0)
        {
            /* Codes_SRS_DATA_PUBLISHER_99_024:[ If no values have been associated with the transaction, no data shall be dispatched
                                            to DataMarshaller, the transaction shall be discarded and DataPublisher_EndTransaction shall return DATA_PUBLISHER_EMPTY_TRANSACTION.] */
            result = DATA_PUBLISHER_EMPTY_TRANSACTION;
            LOG_DATA_PUBLISHER_ERROR;
        }
        /* Codes_SRS_DATA_PUBLISHER_99_010:[ A call to DataPublisher_EndTransaction shall mark the end of a transaction and, trigger a dispatch of all the data grouped by that transaction.] */
        else if (DataMarshaller_SendData(transaction->DataPublisherInstance->DataMarshallerHandle, transaction->ValueCount, transaction->Values, destination, destinationSize) != DATA_MARSHALLER_OK)
        {
            /* Codes_SRS_DATA_PUBLISHER_99_025:[ When the DataMarshaller_SendData call fails, DataPublisher_EndTransaction shall return DATA_PUBLISHER_MARSHALLER_ERROR.] */
            result = DATA_PUBLISHER_MARSHALLER_ERROR;
            LOG_DATA_PUBLISHER_ERROR;
        }
        else
        {
            /* Codes_SRS_DATA_PUBLISHER_99_026:[ On success, DataPublisher_EndTransaction shall return DATA_PUBLISHER_OK.] */
            result = DATA_PUBLISHER_OK;
        }

        /* Codes_SRS_DATA_PUBLISHER_99_012:[ DataPublisher_EndTransaction shall dispose of any resources associated with the transaction.] */
        (void)DataPublisher_CancelTransaction(transactionHandle);
    }

    return result;
}

DATA_PUBLISHER_RESULT DataPublisher_CancelTransaction(TRANSACTION_HANDLE transactionHandle)
{
    DATA_PUBLISHER_RESULT result;

    if (transactionHandle == NULL)
    {
        /* Codes_SRS_DATA_PUBLISHER_99_014:[ If the transactionHandle argument is NULL DataPublisher_CancelTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
        result = DATA_PUBLISHER_INVALID_ARG;
        LOG_DATA_PUBLISHER_ERROR;
    }
    else
    {
        TRANSACTION_HANDLE_DATA* transaction = (TRANSACTION_HANDLE_DATA*)transactionHandle;
        size_t i;

        /* Codes_SRS_DATA_PUBLISHER_99_015:[ DataPublisher_CancelTransaction shall dispose of any resources associated with the transaction.] */
        for (i = 0; i < transaction->ValueCount; i++)
        {
            Destroy_AGENT_DATA_TYPE((AGENT_DATA_TYPE*)transaction->Values[i].Value);
            free((char*)transaction->Values[i].PropertyPath);
            free((AGENT_DATA_TYPE*)transaction->Values[i].Value);
        }

        /* Codes_SRS_DATA_PUBLISHER_99_015:[ DataPublisher_CancelTransaction shall dispose of any resources associated with the transaction.] */
        free(transaction->Values);
        free(transaction);

        /* Codes_SRS_DATA_PUBLISHER_99_013:[ A call to DataPublisher_CancelTransaction shall dispose of the transaction without dispatching
                                        the data to the DataMarshaller module and it shall return DATA_PUBLISHER_OK.] */
        result = DATA_PUBLISHER_OK;
    }

    return result;
}

/* Codes_SRS_DATA_PUBLISHER_99_065:[ DataPublisher_SetMaxBufferSize shall directly update the value used to limit how much data (in bytes) can be buffered in the BufferStorage instance.] */
void DataPublisher_SetMaxBufferSize(size_t value)
{
    maxBufferSize_ = value;
}

/* Codes_SRS_DATA_PUBLISHER_99_069:[ DataMarshaller_GetMaxBufferSize shall return the current max buffer size value used by any new instance of DataMarshaller.] */
size_t DataPublisher_GetMaxBufferSize(void)
{
    return maxBufferSize_;
}

REPORTED_PROPERTIES_TRANSACTION_HANDLE DataPublisher_CreateTransaction_ReportedProperties(DATA_PUBLISHER_HANDLE dataPublisherHandle)
{
    REPORTED_PROPERTIES_TRANSACTION_HANDLE_DATA* result;
    /*Codes_SRS_DATA_PUBLISHER_02_027: [ If argument dataPublisherHandle is NULL then DataPublisher_CreateTransaction_ReportedProperties shall fail and return NULL. ]*/
    if (dataPublisherHandle == NULL)
    {
        LogError("invalid argument DATA_PUBLISHER_HANDLE dataPublisherHandle=%p", dataPublisherHandle);
        result = NULL;
    }
    else
    {
        /*Codes_SRS_DATA_PUBLISHER_02_028: [ DataPublisher_CreateTransaction_ReportedProperties shall create a VECTOR_HANDLE holding the individual elements of the transaction (DATA_MARSHALLER_VALUE). ]*/
        result = (REPORTED_PROPERTIES_TRANSACTION_HANDLE_DATA*)malloc(sizeof(REPORTED_PROPERTIES_TRANSACTION_HANDLE_DATA));
        if (result == NULL)
        {
            /*Codes_SRS_DATA_PUBLISHER_02_029: [ If any error occurs then DataPublisher_CreateTransaction_ReportedProperties shall fail and return NULL. ]*/
            LogError("unable to malloc");
            /*return as is */
        }
        else
        {
            result->value = VECTOR_create(sizeof(DATA_MARSHALLER_VALUE*));
            if (result->value == NULL)
            {
                /*Codes_SRS_DATA_PUBLISHER_02_029: [ If any error occurs then DataPublisher_CreateTransaction_ReportedProperties shall fail and return NULL. ]*/
                LogError("unable to VECTOR_create");
                free(result);
                result = NULL;
            }
            else
            {
                /*Codes_SRS_DATA_PUBLISHER_02_030: [ Otherwise DataPublisher_CreateTransaction_ReportedProperties shall succeed and return a non-NULL handle. ]*/
                result->DataPublisherInstance = dataPublisherHandle;
            }
        }
    }

    return result;
}

static bool reportedPropertyExistsByPath(const void* element, const void* value)
{
    DATA_MARSHALLER_VALUE* dataMarshallerValue = *(DATA_MARSHALLER_VALUE**)element;
    return (strcmp(dataMarshallerValue->PropertyPath, (const char*)value) == 0);
}

DATA_PUBLISHER_RESULT DataPublisher_PublishTransacted_ReportedProperty(REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle, const char* reportedPropertyPath, const AGENT_DATA_TYPE* data)
{
    DATA_PUBLISHER_RESULT result;
    /*Codes_SRS_DATA_PUBLISHER_02_009: [ If argument transactionHandle is NULL then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    /*Codes_SRS_DATA_PUBLISHER_02_010: [ If argument reportedPropertyPath is NULL then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    /*Codes_SRS_DATA_PUBLISHER_02_011: [ If argument data is NULL then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    if (
        (transactionHandle == NULL) ||
        (reportedPropertyPath == NULL) ||
        (data == NULL)
        )
    {
        LogError("invalid argument REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle=%p, const char* reportedPropertyPath=%p, const AGENT_DATA_TYPE* data=%p", transactionHandle, reportedPropertyPath, data);
        result = DATA_PUBLISHER_INVALID_ARG;
    }
    else
    {
        REPORTED_PROPERTIES_TRANSACTION_HANDLE_DATA* handleData = (REPORTED_PROPERTIES_TRANSACTION_HANDLE_DATA*)transactionHandle;
        /*Codes_SRS_DATA_PUBLISHER_02_012: [ DataPublisher_PublishTransacted_ReportedProperty shall verify that a reported property having the path reportedPropertyPath exists in the model by calling Schema_ModelReportedPropertyByPathExists ]*/
        if (!Schema_ModelReportedPropertyByPathExists(handleData->DataPublisherInstance->ModelHandle, reportedPropertyPath))
        {
            /*Codes_SRS_DATA_PUBLISHER_02_013: [ If a reported property with path reportedPropertyPath does not exist in the model then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
            LogError("unable to find a reported property by path \"%s\"", reportedPropertyPath);
            result = DATA_PUBLISHER_INVALID_ARG;
        }
        else
        {
            DATA_MARSHALLER_VALUE** existingValue = VECTOR_find_if(handleData->value, reportedPropertyExistsByPath, reportedPropertyPath);
            if(existingValue != NULL)
            {
                /*Codes_SRS_DATA_PUBLISHER_02_014: [ If the same (by reportedPropertypath) reported property has already been added to the transaction, then DataPublisher_PublishTransacted_ReportedProperty shall overwrite the previous reported property. ]*/
                AGENT_DATA_TYPE *clone = (AGENT_DATA_TYPE *)calloc(1, sizeof(AGENT_DATA_TYPE));
                if(clone == NULL)
                {
                    /*Codes_SRS_DATA_PUBLISHER_02_016: [ If any error occurs then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_ERROR. ]*/
                    LogError("unable to malloc");
                    result = DATA_PUBLISHER_ERROR;
                }
                else
                {
                    if (Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(clone, data) != AGENT_DATA_TYPES_OK)
                    {
                        /*Codes_SRS_DATA_PUBLISHER_02_016: [ If any error occurs then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_ERROR. ]*/
                        LogError("unable to Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE");
                        free(clone);
                        result = DATA_PUBLISHER_ERROR;
                    }
                    else
                    {
                        /*Codes_SRS_DATA_PUBLISHER_02_017: [ Otherwise DataPublisher_PublishTransacted_ReportedProperty shall succeed and return DATA_PUBLISHER_OK. ]*/
                        Destroy_AGENT_DATA_TYPE((AGENT_DATA_TYPE*)((*existingValue)->Value));
                        free((void*)((*existingValue)->Value));
                        (*existingValue)->Value = clone;
                        result = DATA_PUBLISHER_OK;
                    }
                }
            }
            else
            {
                /*totally new reported property*/
                DATA_MARSHALLER_VALUE* newValue = (DATA_MARSHALLER_VALUE*)malloc(sizeof(DATA_MARSHALLER_VALUE));
                if (newValue == NULL)
                {
                    /*Codes_SRS_DATA_PUBLISHER_02_016: [ If any error occurs then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_ERROR. ]*/
                    LogError("unable to malloc");
                    result = DATA_PUBLISHER_ERROR;
                }
                else
                {
                    if (mallocAndStrcpy_s((char**)&(newValue->PropertyPath), reportedPropertyPath) != 0)
                    {
                        /*Codes_SRS_DATA_PUBLISHER_02_016: [ If any error occurs then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_ERROR. ]*/
                        LogError("unable to mallocAndStrcpy_s");
                        free(newValue);
                        result = DATA_PUBLISHER_ERROR;
                    }
                    else
                    {
                        if ((newValue->Value = (AGENT_DATA_TYPE*)calloc(1, sizeof(AGENT_DATA_TYPE))) == NULL)
                        {
                            LogError("unable to malloc");
                            free((void*)newValue->PropertyPath);
                            free(newValue);
                            result = DATA_PUBLISHER_ERROR;
                        }
                        else
                        {
                            if (Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE((AGENT_DATA_TYPE*)newValue->Value, data) != AGENT_DATA_TYPES_OK)
                            {
                                /*Codes_SRS_DATA_PUBLISHER_02_016: [ If any error occurs then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_ERROR. ]*/
                                LogError("unable to Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE");
                                free((void*)newValue->Value);
                                free((void*)newValue->PropertyPath);
                                free(newValue);
                                result = DATA_PUBLISHER_ERROR;
                            }
                            else
                            {
                                /*Codes_SRS_DATA_PUBLISHER_02_015: [ DataPublisher_PublishTransacted_ReportedProperty shall add a new DATA_MARSHALLER_VALUE to the VECTOR_HANDLE. ]*/
                                if (VECTOR_push_back(handleData->value, &newValue, 1) != 0)
                                {
                                    /*Codes_SRS_DATA_PUBLISHER_02_016: [ If any error occurs then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_ERROR. */
                                    LogError("unable to VECTOR_push_back");
                                    Destroy_AGENT_DATA_TYPE((AGENT_DATA_TYPE*)newValue->Value);
                                    free((void*)newValue->Value);
                                    free((void*)newValue->PropertyPath);
                                    free(newValue);
                                    result = DATA_PUBLISHER_ERROR;
                                }
                                else
                                {
                                    /*Codes_SRS_DATA_PUBLISHER_02_017: [ Otherwise DataPublisher_PublishTransacted_ReportedProperty shall succeed and return DATA_PUBLISHER_OK. ]*/
                                    result = DATA_PUBLISHER_OK;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

DATA_PUBLISHER_RESULT DataPublisher_CommitTransaction_ReportedProperties(REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle, unsigned char** destination, size_t* destinationSize)
{
    DATA_PUBLISHER_RESULT result;
    /*Codes_SRS_DATA_PUBLISHER_02_019: [ If argument transactionHandle is NULL then DataPublisher_CommitTransaction_ReportedProperties shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    /*Codes_SRS_DATA_PUBLISHER_02_020: [ If argument destination is NULL then DataPublisher_CommitTransaction_ReportedProperties shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    /*Codes_SRS_DATA_PUBLISHER_02_021: [ If argument destinationSize NULL then DataPublisher_CommitTransaction_ReportedProperties shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    if (
        (transactionHandle == NULL) ||
        (destination == NULL) ||
        (destinationSize == NULL)
        )
    {
        LogError("invalid argument REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle=%p, unsigned char** destination=%p, size_t* destinationSize=%p", transactionHandle, destination, destinationSize);
        result = DATA_PUBLISHER_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_DATA_PUBLISHER_02_031: [ If the transaction contains zero elements then DataPublisher_CommitTransaction_ReportedProperties shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
        REPORTED_PROPERTIES_TRANSACTION_HANDLE_DATA* handle = (REPORTED_PROPERTIES_TRANSACTION_HANDLE_DATA*)transactionHandle;
        if (VECTOR_size(handle->value) == 0)
        {
            LogError("cannot commit empty transaction");
            result = DATA_PUBLISHER_INVALID_ARG;
        }
        else
        {
            /*Codes_SRS_DATA_PUBLISHER_02_022: [ DataPublisher_CommitTransaction_ReportedProperties shall call DataMarshaller_SendData_ReportedProperties providing the VECTOR_HANDLE holding the transacted reported properties, destination and destinationSize. ]*/
            if (DataMarshaller_SendData_ReportedProperties(handle->DataPublisherInstance->DataMarshallerHandle, handle->value, destination, destinationSize) != DATA_MARSHALLER_OK)
            {
                /*Codes_SRS_DATA_PUBLISHER_02_023: [ If any error occurs then DataPublisher_CommitTransaction_ReportedProperties shall fail and return DATA_PUBLISHER_ERROR. ]*/
                LogError("unable to DataMarshaller_SendData_ReportedProperties");
                result = DATA_PUBLISHER_ERROR;
            }
            else
            {
                /*Codes_SRS_DATA_PUBLISHER_02_024: [ Otherwise DataPublisher_CommitTransaction_ReportedProperties shall succeed and return DATA_PUBLISHER_OK. ]*/
                result = DATA_PUBLISHER_OK;
            }
        }
    }

    return result;
}

void DataPublisher_DestroyTransaction_ReportedProperties(REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle)
{
    /*Codes_SRS_DATA_PUBLISHER_02_025: [ If argument transactionHandle is NULL then DataPublisher_DestroyTransaction_ReportedProperties shall return. ]*/
    if (transactionHandle == NULL)
    {
        LogError("invalig argument REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle=%p", transactionHandle);
    }
    else
    {
        /*Codes_SRS_DATA_PUBLISHER_02_026: [ Otherwise DataPublisher_DestroyTransaction_ReportedProperties shall free all resources associated with the reported properties transactionHandle. ]*/
        REPORTED_PROPERTIES_TRANSACTION_HANDLE_DATA* handleData = (REPORTED_PROPERTIES_TRANSACTION_HANDLE_DATA*)transactionHandle;
        size_t i, nReportedProperties;
        nReportedProperties = VECTOR_size(handleData->value);
        for (i = 0;i < nReportedProperties;i++)
        {
            DATA_MARSHALLER_VALUE *value = *(DATA_MARSHALLER_VALUE**)VECTOR_element(handleData->value, i);
            Destroy_AGENT_DATA_TYPE((AGENT_DATA_TYPE*)value->Value);
            free((void*)value->Value);
            free((void*)value->PropertyPath);
            free((void*)value);
        }
        VECTOR_destroy(handleData->value);
        free(handleData);
    }
    return;
}
