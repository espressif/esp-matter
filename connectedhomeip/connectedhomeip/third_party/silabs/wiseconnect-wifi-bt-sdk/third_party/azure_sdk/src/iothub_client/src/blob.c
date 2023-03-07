// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>

#include "azure_c_shared_utility/gballoc.h"
#include "internal/blob.h"
#include "internal/iothub_client_ll_uploadtoblob.h"

#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/azure_base64.h"
#include "azure_c_shared_utility/shared_util_options.h"

static const char blockListXmlBegin[]  = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<BlockList>";
static const char blockListXmlEnd[] = "</BlockList>";
static const char blockListUriMarker[] = "&comp=blocklist";

BLOB_RESULT Blob_UploadBlock(
        HTTPAPIEX_HANDLE httpApiExHandle,
        const char* relativePath,
        BUFFER_HANDLE requestContent,
        unsigned int blockID,
        STRING_HANDLE blockIDList,
        unsigned int* httpStatus,
        BUFFER_HANDLE httpResponse)
{
    BLOB_RESULT result;

    if (requestContent == NULL ||
        blockIDList == NULL ||
        relativePath == NULL ||
        httpApiExHandle == NULL ||
        httpStatus == NULL ||
        httpResponse == NULL)
    {
        LogError("invalid argument detected requestContent=%p blockIDList=%p relativePath=%p httpApiExHandle=%p httpStatus=%p httpResponse=%p", requestContent, blockIDList, relativePath, httpApiExHandle, httpStatus, httpResponse);
        result = BLOB_ERROR;
    }
    else if (blockID > 49999) /*outside the expected range of 000000... 049999*/
    {
        LogError("block ID too large");
        result = BLOB_ERROR;
    }
    else
    {
        char temp[7]; /*this will contain 000000... 049999*/
        if (sprintf(temp, "%6u", (unsigned int)blockID) != 6) /*produces 000000... 049999*/
        {
            /*Codes_SRS_BLOB_02_033: [ If any previous operation that doesn't have an explicit failure description fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_ERROR ]*/
            LogError("failed to sprintf");
            result = BLOB_ERROR;
        }
        else
        {
            STRING_HANDLE blockIdString = Azure_Base64_Encode_Bytes((const unsigned char*)temp, 6);
            if (blockIdString == NULL)
            {
                /*Codes_SRS_BLOB_02_033: [ If any previous operation that doesn't have an explicit failure description fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_ERROR ]*/
                LogError("unable to Azure_Base64_Encode_Bytes");
                result = BLOB_ERROR;
            }
            else
            {
                /*add the blockId base64 encoded to the XML*/
                if (!(
                    (STRING_concat(blockIDList, "<Latest>") == 0) &&
                    (STRING_concat_with_STRING(blockIDList, blockIdString) == 0) &&
                    (STRING_concat(blockIDList, "</Latest>") == 0)
                    ))
                {
                    /*Codes_SRS_BLOB_02_033: [ If any previous operation that doesn't have an explicit failure description fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_ERROR ]*/
                    LogError("unable to STRING_concat");
                    result = BLOB_ERROR;
                }
                else
                {
                    /*Codes_SRS_BLOB_02_022: [ Blob_UploadMultipleBlocksFromSasUri shall construct a new relativePath from following string: base relativePath + "&comp=block&blockid=BASE64 encoded string of blockId" ]*/
                    STRING_HANDLE newRelativePath = STRING_construct(relativePath);
                    if (newRelativePath == NULL)
                    {
                        /*Codes_SRS_BLOB_02_033: [ If any previous operation that doesn't have an explicit failure description fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_ERROR ]*/
                        LogError("unable to STRING_construct");
                        result = BLOB_ERROR;
                    }
                    else
                    {
                        if (!(
                            (STRING_concat(newRelativePath, "&comp=block&blockid=") == 0) &&
                            (STRING_concat_with_STRING(newRelativePath, blockIdString) == 0)
                            ))
                        {
                            /*Codes_SRS_BLOB_02_033: [ If any previous operation that doesn't have an explicit failure description fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_ERROR ]*/
                            LogError("unable to STRING concatenate");
                            result = BLOB_ERROR;
                        }
                        else
                        {
                            /*Codes_SRS_BLOB_02_024: [ Blob_UploadMultipleBlocksFromSasUri shall call HTTPAPIEX_ExecuteRequest with a PUT operation, passing httpStatus and httpResponse. ]*/
                            if (HTTPAPIEX_ExecuteRequest(
                                httpApiExHandle,
                                HTTPAPI_REQUEST_PUT,
                                STRING_c_str(newRelativePath),
                                NULL,
                                requestContent,
                                httpStatus,
                                NULL,
                                httpResponse) != HTTPAPIEX_OK
                                )
                            {
                                /*Codes_SRS_BLOB_02_025: [ If HTTPAPIEX_ExecuteRequest fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_HTTP_ERROR. ]*/
                                LogError("unable to HTTPAPIEX_ExecuteRequest");
                                result = BLOB_HTTP_ERROR;
                            }
                            else if (*httpStatus >= 300)
                            {
                                /*Codes_SRS_BLOB_02_026: [ Otherwise, if HTTP response code is >=300 then Blob_UploadMultipleBlocksFromSasUri shall succeed and return BLOB_OK. ]*/
                                LogError("HTTP status from storage does not indicate success (%d)", (int)*httpStatus);
                                result = BLOB_OK;
                            }
                            else
                            {
                                /*Codes_SRS_BLOB_02_027: [ Otherwise Blob_UploadMultipleBlocksFromSasUri shall continue execution. ]*/
                                result = BLOB_OK;
                            }
                        }
                        STRING_delete(newRelativePath);
                    }
                }
                STRING_delete(blockIdString);
            }
        }
    }
    return result;
}


// InvokeUserCallbackAndSendBlobs invokes the application's getDataCallbackEx as many time as callback requests and, for each call,
// sends the blob contents to the server.
static BLOB_RESULT InvokeUserCallbackAndSendBlobs(HTTPAPIEX_HANDLE httpApiExHandle, const char* relativePath, STRING_HANDLE blockIDList, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK_EX getDataCallbackEx, void* context, unsigned int* httpStatus, BUFFER_HANDLE httpResponse)
{
    BLOB_RESULT result;

    /*Codes_SRS_BLOB_02_021: [ For every block returned by `getDataCallbackEx` the following operations shall happen: ]*/
    unsigned int blockID = 0; /* incremented for each new block */
    unsigned int isError = 0; /* set to 1 if a block upload fails or if getDataCallbackEx returns incorrect blocks to upload */
    unsigned int uploadOneMoreBlock = 1; /* set to 1 while getDataCallbackEx returns correct blocks to upload */
    unsigned char const * source = NULL; /* data set by getDataCallbackEx */
    size_t size = 0; /* source size set by getDataCallbackEx */
    IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_RESULT getDataReturnValue;

    do
    {
        getDataReturnValue = getDataCallbackEx(FILE_UPLOAD_OK, &source, &size, context);
        if (getDataReturnValue == IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_ABORT)
        {
            /*Codes_SRS_BLOB_99_004: [ If `getDataCallbackEx` returns `IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_RESULT_ABORT`, then `Blob_UploadMultipleBlocksFromSasUri` shall exit the loop and return `BLOB_ABORTED`. ]*/
            LogInfo("Upload to blob has been aborted by the user");
            uploadOneMoreBlock = 0;
            result = BLOB_ABORTED;
        }
        else if (source == NULL || size == 0)
        {
            /*Codes_SRS_BLOB_99_002: [ If the size of the block returned by `getDataCallbackEx` is 0 or if the data is NULL, then `Blob_UploadMultipleBlocksFromSasUri` shall exit the loop. ]*/
            uploadOneMoreBlock = 0;
            result = BLOB_OK;
        }
        else
        {
            if (size > BLOCK_SIZE)
            {
                /*Codes_SRS_BLOB_99_001: [ If the size of the block returned by `getDataCallbackEx` is bigger than 4MB, then `Blob_UploadMultipleBlocksFromSasUri` shall fail and return `BLOB_INVALID_ARG`. ]*/
                LogError("tried to upload block of size %lu, max allowed size is %d", (unsigned long)size, BLOCK_SIZE);
                result = BLOB_INVALID_ARG;
                isError = 1;
            }
            else if (blockID >= MAX_BLOCK_COUNT)
            {
                /*Codes_SRS_BLOB_99_003: [ If `getDataCallbackEx` returns more than 50000 blocks, then `Blob_UploadMultipleBlocksFromSasUri` shall fail and return `BLOB_INVALID_ARG`. ]*/
                LogError("unable to upload more than %lu blocks in one blob", (unsigned long)MAX_BLOCK_COUNT);
                result = BLOB_INVALID_ARG;
                isError = 1;
            }
            else
            {
                /*Codes_SRS_BLOB_02_023: [ Blob_UploadMultipleBlocksFromSasUri shall create a BUFFER_HANDLE from source and size parameters. ]*/
                BUFFER_HANDLE requestContent = BUFFER_create(source, size);
                if (requestContent == NULL)
                {
                    /*Codes_SRS_BLOB_02_033: [ If any previous operation that doesn't have an explicit failure description fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_ERROR ]*/
                    LogError("unable to BUFFER_create");
                    result = BLOB_ERROR;
                    isError = 1;
                }
                else
                {
                    result = Blob_UploadBlock(
                            httpApiExHandle,
                            relativePath,
                            requestContent,
                            blockID,
                            blockIDList,
                            httpStatus,
                            httpResponse);

                    BUFFER_delete(requestContent);
                }

                /*Codes_SRS_BLOB_02_026: [ Otherwise, if HTTP response code is >=300 then Blob_UploadMultipleBlocksFromSasUri shall succeed and return BLOB_OK. ]*/
                if (result != BLOB_OK)
                {
                    LogError("unable to Blob_UploadBlock. Returned value=%d", result);
                    isError = 1;
                }
                else if (*httpStatus >= 300)
                {
                    LogError("unable to Blob_UploadBlock. Returned httpStatus=%u", (unsigned int)*httpStatus);
                    isError = 1;
                }
            }
            blockID++;
        }
    }
    while(uploadOneMoreBlock && !isError);

    return result;
}

// SendBlockIdList to send an XML of uploaded blockIds to the server after the application's payload block(s) have been transfered.
static BLOB_RESULT SendBlockIdList(HTTPAPIEX_HANDLE httpApiExHandle, const char* relativePath, STRING_HANDLE blockIDList, unsigned int* httpStatus, BUFFER_HANDLE httpResponse)
{
    BLOB_RESULT result;
    
    /*complete the XML*/
    if (STRING_concat(blockIDList, blockListXmlEnd) != 0)
    {
        /*Codes_SRS_BLOB_02_033: [ If any previous operation that doesn't have an explicit failure description fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_ERROR ]*/
        LogError("failed to STRING_concat");
        result = BLOB_ERROR;
    }
    else
    {
        /*Codes_SRS_BLOB_02_029: [Blob_UploadMultipleBlocksFromSasUri shall construct a new relativePath from following string : base relativePath + "&comp=blocklist"]*/
        STRING_HANDLE newRelativePath = STRING_construct(relativePath);
        if (newRelativePath == NULL)
        {
            /*Codes_SRS_BLOB_02_033: [ If any previous operation that doesn't have an explicit failure description fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_ERROR ]*/
            LogError("failed to STRING_construct");
            result = BLOB_ERROR;
        }
        else
        {
            if (STRING_concat(newRelativePath, blockListUriMarker) != 0)
            {
                /*Codes_SRS_BLOB_02_033: [ If any previous operation that doesn't have an explicit failure description fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_ERROR ]*/
                LogError("failed to STRING_concat");
                result = BLOB_ERROR;
            }
            else
            {
                /*Codes_SRS_BLOB_02_030: [ Blob_UploadMultipleBlocksFromSasUri shall call HTTPAPIEX_ExecuteRequest with a PUT operation, passing the new relativePath, httpStatus and httpResponse and the XML string as content. ]*/
                const char* s = STRING_c_str(blockIDList);
                BUFFER_HANDLE blockIDListAsBuffer = BUFFER_create((const unsigned char*)s, strlen(s));
                if (blockIDListAsBuffer == NULL)
                {
                    /*Codes_SRS_BLOB_02_033: [ If any previous operation that doesn't have an explicit failure description fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_ERROR ]*/
                    LogError("failed to BUFFER_create");
                    result = BLOB_ERROR;
                }
                else
                {
                    if (HTTPAPIEX_ExecuteRequest(
                        httpApiExHandle,
                        HTTPAPI_REQUEST_PUT,
                        STRING_c_str(newRelativePath),
                        NULL,
                        blockIDListAsBuffer,
                        httpStatus,
                        NULL,
                        httpResponse
                    ) != HTTPAPIEX_OK)
                    {
                        /*Codes_SRS_BLOB_02_031: [ If HTTPAPIEX_ExecuteRequest fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_HTTP_ERROR. ]*/
                        LogError("unable to HTTPAPIEX_ExecuteRequest");
                        result = BLOB_HTTP_ERROR;
                    }
                    else
                    {
                        /*Codes_SRS_BLOB_02_032: [ Otherwise, Blob_UploadMultipleBlocksFromSasUri shall succeed and return BLOB_OK. ]*/
                        result = BLOB_OK;
                    }
                    BUFFER_delete(blockIDListAsBuffer);
                }
            }
            STRING_delete(newRelativePath);
        }
    }

    return result;
}


BLOB_RESULT Blob_UploadMultipleBlocksFromSasUri(const char* SASURI, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK_EX getDataCallbackEx, void* context, unsigned int* httpStatus, BUFFER_HANDLE httpResponse, const char* certificates, HTTP_PROXY_OPTIONS *proxyOptions)
{
    BLOB_RESULT result;
    const char* hostnameBegin;
    STRING_HANDLE blockIDList = NULL;
    HTTPAPIEX_HANDLE httpApiExHandle = NULL;
    char* hostname = NULL;
    
    /*Codes_SRS_BLOB_02_001: [ If SASURI is NULL then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
    /*Codes_SRS_BLOB_02_002: [ If getDataCallbackEx is NULL then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
    if ((SASURI == NULL) || (getDataCallbackEx == NULL))
    {
        LogError("One or more required values is NULL, SASURI=%p, getDataCallbackEx=%p", SASURI, getDataCallbackEx);
        result = BLOB_INVALID_ARG;
    }
    /*Codes_SRS_BLOB_02_017: [ Blob_UploadMultipleBlocksFromSasUri shall copy from SASURI the hostname to a new const char* ]*/
    /*to find the hostname, the following logic is applied:*/
    /*the hostname starts at the first character after "://"*/
    /*the hostname ends at the first character before the next "/" after "://"*/
    else if ((hostnameBegin = strstr(SASURI, "://")) == NULL)
    {
        /*Codes_SRS_BLOB_02_005: [ If the hostname cannot be determined, then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
        LogError("hostname cannot be determined");
        result = BLOB_INVALID_ARG;
    }
    else
    {
        hostnameBegin += 3; /*have to skip 3 characters which are "://"*/
        const char* relativePath = strchr(hostnameBegin, '/');
        if (relativePath == NULL)
        {
            /*Codes_SRS_BLOB_02_005: [ If the hostname cannot be determined, then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
            LogError("hostname cannot be determined");
            result = BLOB_INVALID_ARG;
        }
        else
        {
            size_t hostnameSize = relativePath - hostnameBegin;
            if ((hostname = (char*)malloc(hostnameSize + 1)) == NULL)
            {
                /*Codes_SRS_BLOB_02_016: [ If the hostname copy cannot be made then then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_ERROR ]*/
                LogError("oom - out of memory");
                result = BLOB_ERROR;
            }
            else
            {
                (void)memcpy(hostname, hostnameBegin, hostnameSize);
                hostname[hostnameSize] = '\0';

                /*Codes_SRS_BLOB_02_018: [ Blob_UploadMultipleBlocksFromSasUri shall create a new HTTPAPI_EX_HANDLE by calling HTTPAPIEX_Create passing the hostname. ]*/
                httpApiExHandle = HTTPAPIEX_Create(hostname);
                if (httpApiExHandle == NULL)
                {
                    /*Codes_SRS_BLOB_02_007: [ If HTTPAPIEX_Create fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_ERROR. ]*/
                    LogError("unable to create a HTTPAPIEX_HANDLE");
                    result = BLOB_ERROR;
                }
                else if ((certificates != NULL) && (HTTPAPIEX_SetOption(httpApiExHandle, "TrustedCerts", certificates) == HTTPAPIEX_ERROR))
                {
                    LogError("failure in setting trusted certificates");
                    result = BLOB_ERROR;
                }
                else if ((proxyOptions != NULL && proxyOptions->host_address != NULL) && HTTPAPIEX_SetOption(httpApiExHandle, OPTION_HTTP_PROXY, proxyOptions) == HTTPAPIEX_ERROR)
                {
                    LogError("failure in setting proxy options");
                    result = BLOB_ERROR;
                }
                /*Codes_SRS_BLOB_02_028: [ Blob_UploadMultipleBlocksFromSasUri shall construct an XML string with the following content: ]*/
                else if ((blockIDList = STRING_construct(blockListXmlBegin)) == NULL)
                {
                    /*Codes_SRS_BLOB_02_033: [ If any previous operation that doesn't have an explicit failure description fails then Blob_UploadMultipleBlocksFromSasUri shall fail and return BLOB_ERROR ]*/
                    LogError("failed to STRING_construct");
                    result = BLOB_HTTP_ERROR;
                }
                else if ((result = InvokeUserCallbackAndSendBlobs(httpApiExHandle, relativePath, blockIDList, getDataCallbackEx, context, httpStatus, httpResponse)) != BLOB_OK)
                {
                   LogError("Failed in invoking callback/sending blob step");
                }
                else if (*httpStatus < 300)
                {
                    // Per SRS_BLOB_02_026, it possible for us to have a result=BLOB_OK AND a non-success HTTP status code.
                    // In order to maintain back-compat with existing code, we will return the BLOB_OK to the caller but NOT invoke this final step.
                    result = SendBlockIdList(httpApiExHandle, relativePath, blockIDList, httpStatus, httpResponse);
                }
            }
        }
    }

    HTTPAPIEX_Destroy(httpApiExHandle);
    STRING_delete(blockIDList);
    free(hostname);

    return result;
}
