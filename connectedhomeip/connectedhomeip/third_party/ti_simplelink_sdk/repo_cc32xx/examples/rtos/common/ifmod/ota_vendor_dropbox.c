/*
 * Copyright (C) 2016-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ota_settings.h"

#if OTA_VENDOR_DROPBOX_SUPPORT

#include "string.h"
#include <ifmod/ota_vendors.h>
#include "ota_if.h"
#include "debug_if.h"
#include "utils_if.h"
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/net/http/httpclient.h>
#include <ti/utils/json/json.h>

#undef  DEBUG_IF_NAME
#define DEBUG_IF_NAME "OTA-DROPBOX"

#undef  DEBUG_IF_SEVERITY
#define DEBUG_IF_SEVERITY OTA_IF_DEBUG_LEVEL

/**************************************************************************/
/* DROPBOX CONNECTION DEFINIONS */

#define DROPBOX_ROOT_CA         "RootCACerts.pem"
#define ALT_DROPBOX_ROOT_CA     "digicert_high_assurance_ca.der"

/**************************************************************************/

#define USER_AGENT              "OTA Example (ARM; TIRTOS)"

#define HOSTNAME                "https://api.dropboxapi.com"
#define APP_TOKEN               "Bearer " DROPBOX_USER_TOKEN
#define CONTENT_TYPE            "application/json"

#define HTTP_REC_SIZE           128

#define LIST_FOLDER_REQ_URI      "/2/files/list_folder"
#define LIST_FOLDER_CONT_REQ_URI "/2/files/list_folder/continue"
#define GET_LINK_REQ_URI         "/2/files/get_temporary_link"


#define REQ_PATH_JSON           "{\"path\": \""
#define REQ_CURSOR_JSON         "{\"cursor\": \""

#define LIST_FOLDER_REQ_DATA \
    "{"\
        "\"path\": \"/" DROPBOX_USER_PATH "\","\
        "\"limit\": 1"\
    "}"

#define LIST_FOLDER_RES_JSON_TEMPLATE \
    "{"\
        "\"entries\": ["\
            "{"\
                "\"path_display\": string,"\
                "\"size\": int32"\
            "}"\
        "],"\
        "\"cursor\": string,"\
        "\"has_more\": boolean"\
    "}"

#define GET_LINK_RES_JSON_TEMPLATE \
    "{"\
        "\"metadata\": {"\
            "\"name\": string"\
        "},"\
        "\"link\": string"\
    "}"

#define MAX_REQ_URI_LEN                     30
#define MAX_BUFF_DATA_LEN                   1000
#define D_KEEP_LOOKING_FOR_NEW_VERSION      -35099

static int16_t ReadHTTPResponse(HTTPClient_Handle httpClientHandle, char *resp_buf);

typedef struct {
    char data   [MAX_BUFF_DATA_LEN];
} gdl_buffer_t; // Rename to something more descriptive?


#ifdef OATH2_SUPPORT
#define OAUTH2_AUTHORIZE_REQ_URI    "/oauth2/authorize"
#define OAUTH2_TOKEN_REQ_URI        "/oauth2/token?client_id=" APP_KEY "&response_type=code&token_access_type=offline"
#define OAUTH2_AUTHORIZE_REQ_DATA \
    "client_id=" APP_KEY "\n\rredirect_uri="HOSTNAME"/oauth2/token\n\rresponse_type=code\n\rtoken_access_type=offline\n\r\n\r"

int GetTempToken(HTTPClient_Handle httpClientHandle, uint8_t *pBuff)
{

    int ret;
    LOG_DEBUG("GetTempToken");


    ret = HTTPClient_sendRequest(httpClientHandle,
                                   HTTP_METHOD_GET,OAUTH2_AUTHORIZE_REQ_URI,
                                   OAUTH2_AUTHORIZE_REQ_DATA,strlen(OAUTH2_AUTHORIZE_REQ_DATA),
                                   0);
    if(ret < 0) {

        LOG_ERROR("post request failed: %d", ret);
        free(pBuff);
    }
    else
    {
        LOG_DEBUG("HTTP Response Status Code: %d", ret);
        LOG_DEBUG("%s\n", pBuff);
    }
    return ret;
}
#endif

/*
 * returns: OTA_VENDOR_NEW_IMAGE_FOUND (0)  - when new version is found
 *          OTA_VENDOR_NO_NEW_IMAGE_FOUND   - when new version is not found
 *          D_KEEP_LOOKING_FOR_NEW_VERSION  - when current file is not new, but there are more file in the folder
 *          negative value                  - upon error
 */
int ParseListFolderResponse(Json_Handle listFolderTemplateHandle, char *pBuff, uint16_t len)
{
    int ret;
    Json_Handle listFolderObjectHandle;
    char *pPathDisp, *pFilename;
    bool bHasMore;

    // Setup the JSON object to read entries
    ret = Json_createObject(&listFolderObjectHandle, listFolderTemplateHandle, 0);
    if(ret < 0) {
        LOG_ERROR("create JSON object failed: %d", ret);
    }
    else
    {
        ret = Json_parse(listFolderObjectHandle, pBuff, len);
        if(ret < 0){
            LOG_ERROR("parse JSON failed: %d", ret);
        }
        else
        {
            // Get the full path name from the JSON response.
            memcpy(pBuff, REQ_PATH_JSON, strlen(REQ_PATH_JSON));
            pPathDisp = pBuff + strlen(REQ_PATH_JSON);
            len -= strlen(REQ_PATH_JSON);

            ret = Json_getValue(listFolderObjectHandle, "\"entries\".[0].\"path_display\"",
                                pPathDisp, &len);
            if(ret < 0) {
                LOG_ERROR("JSON getValue (path_display) failed: %d", ret);
            }
            else
            {
                pPathDisp[len] = 0;
                LOG_TRACE("path_display: %s", pPathDisp);

                // filename is a pointer to the beginning of the filename in the path
                pFilename = pPathDisp + strlen(DROPBOX_USER_PATH) + 2; // /DROPBOX_USER_PATH/
                LOG_DEBUG("filename: %s", pFilename);
                strcat(pFilename, "\"}\0");

                // check version

                if(OTA_IF_isNewVersion((uint8_t*)pFilename))
                {
                    ret = OTA_VENDOR_NEW_IMAGE_FOUND;
                }
                else
                {
                    // This part of the code is only used when the file checked is not newer than the current version.

                    // Get the hasMore flag from the JSON response.
                    bHasMore = 0;
                    ret = Json_getValue(listFolderObjectHandle, "\"has_more\"",
                                        &bHasMore, NULL);
                    if(ret < 0)
                    {
                        LOG_ERROR("JSON getValue (hasMore) failed: %d", ret);
                    }
                    else
                    {
                        LOG_DEBUG("hasMore: %d", &bHasMore);
                        if (bHasMore == 0)
                        {
                            // New file was not found in Dropbox folder
                            ret = OTA_VENDOR_NO_NEW_IMAGE_FOUND;
                        }
                        else
                        {
                            // Get the cursor from the JSON response.
                            memcpy(pBuff, REQ_CURSOR_JSON, strlen(REQ_CURSOR_JSON));
                            pBuff += strlen(REQ_CURSOR_JSON);
                            len = MAX_BUFF_DATA_LEN - strlen(REQ_CURSOR_JSON);

                            ret = Json_getValue(listFolderObjectHandle, "\"cursor\"",
                                                pBuff, &len);
                            if(ret < 0){
                                LOG_ERROR("JSON getValue (cursor) failed: %d", ret);
                            }
                            else
                            {
                                pBuff[len] = 0;
                                strcat(pBuff, "\"}\0");
                                ret = D_KEEP_LOOKING_FOR_NEW_VERSION;
                            }
                        }
                    }
                }
            }
        }
        Json_destroyObject(listFolderObjectHandle);
    }

    return ret;
}

static int16_t ReadHTTPResponse(HTTPClient_Handle httpClientHandle, char* resp_buf) {
    int16_t ret;
    int16_t resp_size;
    bool more_data_flag;

    resp_size = 0;
    more_data_flag = 0;
    do {
        ret =
            HTTPClient_readResponseBody(httpClientHandle, (resp_buf + resp_size),
                                        HTTP_REC_SIZE, &more_data_flag);
        if(ret < 0){
            LOG_ERROR("readResponse failed: %d", ret);
            return (ret);
        }
        resp_size += ret;
    } while (more_data_flag);

    LOG_TRACE("resp_buf: %.*s\r\n", resp_size, resp_buf);
    LOG_TRACE("resp_size: %d\r\n", resp_size);

    return resp_size;
}


/* PUBLIC API -
 * This can be called directly or as a  (primary or backup) callback for the OTA_IF_downloadImageByCloudVendor().
 */
int32_t OTA_DROPBOX_getDownloadLink(FileServerParams_t *pServerParams){
    int32_t ret;
    gdl_buffer_t* buff;

    HTTPClient_Handle httpClientHandle = NULL;
    int16_t jsonSize;
    uint16_t linkSize;
    Json_Handle listFolderTemplateHandle, getLinkTemplateHandle;
    Json_Handle getLinkObjectHandle;


    // Allocate the buffer and the output URL on the heap.
    buff = (gdl_buffer_t*) calloc (1, sizeof(gdl_buffer_t));

    if(buff)
    {
        LOG_INFO("*** Attempting DROPBOX getDownloadLink ***");

        // Create template for parsing the list_folder request response.
        ret = Json_createTemplate(&listFolderTemplateHandle, LIST_FOLDER_RES_JSON_TEMPLATE,
                                  strlen(LIST_FOLDER_RES_JSON_TEMPLATE));
        if(ret < 0){
            LOG_ERROR("create JSON template failed: %d", ret);
        }
        else
        {
            ret = HTTP_setupConnection(&httpClientHandle, HOSTNAME, NULL, NULL, DROPBOX_ROOT_CA, ALT_DROPBOX_ROOT_CA, 0);
        }

        if(ret == 0)
        {
            char *pReqURI = LIST_FOLDER_REQ_URI;

            // Setup the first HTTP POST request with the list_folder API call.
            strcpy(buff->data, LIST_FOLDER_REQ_DATA);

            do {
                // Send HTTP POST request (LIST_FOLDER)
                LOG_DEBUG("=== Sending POST Request ===\r\n uri:%s\r\n data:%s", pReqURI, buff->data);
                ret = HTTP_setHeaders(httpClientHandle, NULL, APP_TOKEN, CONTENT_TYPE, HTTPClient_HFIELD_NOT_PERSISTENT);
                if(ret == 0)
                {
                    ret = HTTPClient_sendRequest(httpClientHandle,
                                                 HTTP_METHOD_POST,pReqURI,
                                                 buff->data,strlen(buff->data),
                                                 0);
                    LOG_DEBUG("HTTP Response Status Code: %d", ret);
#ifdef OATH2_SUPPORT
                    if(ret == HTTP_SC_UNAUTHORIZED)
                    {
                        GetTempToken(httpClientHandle, buff->data);
                    } else
#endif
                    if(ret < 0) {

                        LOG_ERROR("post request failed: %d", ret);
                        free(buff);
                        return (ret);
                    }
                }
                // Read JSON Response from POST request into data buffer
                ret = ReadHTTPResponse(httpClientHandle, buff->data);
                if(ret < 0) { // Negative on error
                    free(buff);
                }
                else
                {
                    buff->data[ret] = 0;

                    ret = ParseListFolderResponse(listFolderTemplateHandle, buff->data, ret);

                    // Setup the next HTTP POST request with the list_folder/continue API call.
                    pReqURI =  LIST_FOLDER_CONT_REQ_URI;
                }
            } while(ret == D_KEEP_LOOKING_FOR_NEW_VERSION); // Only continue when there are more files in the folder to check.

            if(ret == OTA_VENDOR_NO_NEW_IMAGE_FOUND)
            {
                LOG_INFO("*** No new image was found!!! ***");
            }
            else if(ret == OTA_VENDOR_NEW_IMAGE_FOUND)
            {
                // Send HTTP POST request (GET_LINK)
                LOG_DEBUG("\n\r=== Sending POST Request ===\r\n uri:%s\r\n data:%s", GET_LINK_REQ_URI, buff->data);
                ret = HTTP_setHeaders(httpClientHandle, NULL, APP_TOKEN, CONTENT_TYPE, HTTPClient_HFIELD_NOT_PERSISTENT);
                if(ret == 0)
                {
                    ret = HTTPClient_sendRequest(httpClientHandle,
                                                 HTTP_METHOD_POST,GET_LINK_REQ_URI,
                                                 buff->data,strlen(buff->data),
                                                 0);
                    if(ret < 0) {
                        LOG_ERROR("post request failed: %d", ret);
                    }
                    else
                    {
                        LOG_DEBUG("HTTP Response Status Code: %d", ret);

                        // Read JSON Response from POST request into data buffer
                        LOG_DEBUG("Reading JSON\r\n");
                        memset(buff->data, 0, MAX_BUFF_DATA_LEN);
                        jsonSize = ReadHTTPResponse(httpClientHandle, buff->data);
                        if(jsonSize < 0) {
                            ret = jsonSize;
                        }
                        else
                        {
                            // Create template for parsing the list_folder request response.
                            ret = Json_createTemplate(&getLinkTemplateHandle, GET_LINK_RES_JSON_TEMPLATE,
                                                      strlen(GET_LINK_RES_JSON_TEMPLATE));
                            if(ret < 0) {
                                LOG_ERROR("create JSON template failed: %d", ret);
                            }
                            else
                            {
                                // Setup the JSON object to read entries
                                ret = Json_createObject(&getLinkObjectHandle, getLinkTemplateHandle, 0);
                                if(ret < 0) {
                                    LOG_ERROR("create JSON object failed: %d", ret);
                                }
                                else
                                {
                                    ret = Json_parse(getLinkObjectHandle, buff->data, jsonSize);
                                    if(ret < 0) {
                                        LOG_ERROR("parse JSON failed: %d", ret);
                                    }
                                    else
                                    {
                                        // Get the download link size from the JSON response.
                                        ret = Json_getValue(getLinkObjectHandle, "\"link\"",
                                                            NULL, &linkSize);
                                        if(ret < 0) {
                                            LOG_ERROR("JSON getValue (size of link) failed: %d", ret);
                                        }
                                        else
                                        {

                                            // Get the download link from the JSON response.
                                            memset(buff->data, 0, MAX_BUFF_DATA_LEN);
                                            ret =
                                                    Json_getValue(getLinkObjectHandle, "\"link\"",
                                                                  buff->data, NULL);
                                            if(ret < 0) {
                                                LOG_ERROR("JSON getValue (link) failed: %d", ret);
                                            }
                                            else
                                            {
                                                linkSize = strlen(buff->data);
                                                memcpy(pServerParams->URL, buff->data, linkSize);
                                                pServerParams->URL[linkSize] = '\0';
                                                pServerParams->pVersion = NULL; /* NULL means that the version will be compared again
                                                   in OTA_IF against the timestamp at the beginning of the
                                                   tar file (which is part of the filename).
                                                   In case the version should be something else, make sure
                                                   allocate the space for it (and to free it after the
                                                   downloadImage call returns). */
                                                pServerParams->pClientCert = NULL;
                                                pServerParams->pPrivateKey = NULL;
                                                pServerParams->pRootCa1 = DROPBOX_ROOT_CA;
                                                pServerParams->pRootCa2 = ALT_DROPBOX_ROOT_CA;

                                                LOG_INFO("*** New Image Found: (URL: %s) ***", pServerParams->URL);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            // Close the HTTP connection and destroy the associated object.
            if(HTTPClient_disconnect(httpClientHandle) < 0) {
                LOG_ERROR("http disconnect failed: %d", ret);
            }
            if(HTTPClient_destroy(httpClientHandle) < 0) {
                LOG_ERROR("destroy failed: %d", ret);
            }
        }
        free(buff);

    }
    return (ret);
}




#endif // OTA_VENDOR_DROPBOX

