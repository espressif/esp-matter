// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file blob.h
*    @brief Contains blob APIs needed for File Upload feature of IoTHub client.
*
*    @details IoTHub client needs to upload a byte array by using blob storage API
*             IoTHub service provides the complete SAS URI to execute a PUT request
*             that will upload the data.
*
*/

#ifndef BLOB_H
#define BLOB_H

#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/strings_types.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "iothub_client_core_ll.h"
#include "azure_c_shared_utility/shared_util_options.h"

#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#endif

#include "umock_c/umock_c_prod.h"

/* Allow unit tests to override MAX_BLOCK_COUNT to something much smaller */
#ifndef MAX_BLOCK_COUNT
/* Maximum count of blocks uploaded is 50000, per server*/
#define MAX_BLOCK_COUNT 50000
#endif

#define BLOB_RESULT_VALUES \
    BLOB_OK,               \
    BLOB_ERROR,            \
    BLOB_NOT_IMPLEMENTED,  \
    BLOB_HTTP_ERROR,       \
    BLOB_INVALID_ARG,      \
    BLOB_ABORTED

MU_DEFINE_ENUM_WITHOUT_INVALID(BLOB_RESULT, BLOB_RESULT_VALUES)

/**
* @brief  Synchronously uploads a byte array to blob storage
*
* @param  SASURI            The URI to use to upload data
* @param  getDataCallbackEx A callback to be invoked to acquire the file chunks to be uploaded, as well as to indicate the status of the upload of the previous block.
* @param  context           Any data provided by the user to serve as context on getDataCallback.
* @param  httpStatus        A pointer to an out argument receiving the HTTP status (available only when the return value is BLOB_OK)
* @param  httpResponse      A BUFFER_HANDLE that receives the HTTP response from the server (available only when the return value is BLOB_OK)
* @param  certificates      A null terminated string containing CA certificates to be used
* @param    proxyOptions    A structure that contains optional web proxy information
*
* @return    A @c BLOB_RESULT. BLOB_OK means the blob has been uploaded successfully. Any other value indicates an error
*/
MOCKABLE_FUNCTION(, BLOB_RESULT, Blob_UploadMultipleBlocksFromSasUri, const char*, SASURI, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK_EX, getDataCallbackEx, void*, context, unsigned int*, httpStatus, BUFFER_HANDLE, httpResponse, const char*, certificates, HTTP_PROXY_OPTIONS*, proxyOptions)

/**
* @brief  Synchronously uploads a byte array as a new block to blob storage
*
* @param  requestContent      The data to upload
* @param  blockId             The block id (from 00000 to 49999)
* @param  xml                 The XML file containing the blockId list
* @param  relativePath        The destination path within the storage
* @param  httpApiExHandle     The connection handle
* @param  httpStatus          A pointer to an out argument receiving the HTTP status (available only when the return value is BLOB_OK)
* @param  httpResponse        A BUFFER_HANDLE that receives the HTTP response from the server (available only when the return value is BLOB_OK)
*/
//MOCKABLE_FUNCTION(, BLOB_RESULT, Blob_UploadNextBlock, BUFFER_HANDLE, requestContent, unsigned int, blockID, STRING_HANDLE, xml, const char*, relativePath, HTTPAPIEX_HANDLE, httpApiExHandle, unsigned int*, httpStatus, BUFFER_HANDLE, httpResponse)
MOCKABLE_FUNCTION(, BLOB_RESULT, Blob_UploadBlock, HTTPAPIEX_HANDLE, httpApiExHandle, const char*, relativePath, BUFFER_HANDLE, requestContent, unsigned int, blockID, STRING_HANDLE, blockIDList, unsigned int*, httpStatus, BUFFER_HANDLE, httpResponse)
#ifdef __cplusplus
}
#endif

#endif /* BLOB_H */
