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

#if OTA_VENDOR_GITHUB_SUPPORT

#include <ifmod/ota_vendors.h>
#include "ota_if.h"
#include "string.h"
#include "debug_if.h"
#include "utils_if.h"
#include <ti/net/http/httpclient.h>
#include <ti/net/utils/str_mpl.h>
#include <ti/utils/json/json.h>
#include <ti/drivers/net/wifi/simplelink.h>

#undef  DEBUG_IF_NAME
#define DEBUG_IF_NAME "OTA-GITHUB"

#undef  DEBUG_IF_SEVERITY
#define DEBUG_IF_SEVERITY OTA_IF_DEBUG_LEVEL

/**************************************************************************/
/* GITHUB CONNECTION DEFINIONS */

#define GITHUB_ROOT_CA       "RootCACerts.pem"
#define ALT_GITHUB_ROOT_CA   "digicert_high_assurance_ca.der"

/**************************************************************************/

#define HOSTNAME            "https://api.github.com"
#define USER_AGENT          "OTA Example (ARM; TI-RTOS)"
#define CONTENT_TYPE        "application/vnd.github.v3+json"
#define REQUEST_URI         "/repos/" GITHUB_USER_NAME "/" GITHUB_USER_REPO "/contents/" GITHUB_USER_PATH

#define JSON_ARRAY_KEY      "\"#\""
#define JSON_TEMPLATE \
        "{"\
    JSON_ARRAY_KEY ":["\
                      "{"\
        "\"name\": string,"\
        "\"size\": int32,"\
        "\"download_url\": string"\
        "}"\
        "]"\
        "}"

#define MAX_FILENAME_LEN    (OTA_VERSION_LEN + 32)
#define MAX_JSON_KEY_LEN    30
#define HTTP_REC_SIZE       128



/* 2 implementations for HITHUB response are provided.
 * The Full GITHUB Response parser is provided in case JSON fields other than the "download_url"
 * is needed. By default, we are only checking the "download_url" and thus use the simpler
 * parser.
 * Note: among other the "simple" implementation, doesn't requires the large allocation of the
 * entire json response file (that may include more than one entry). Instead it parses the file in
 * smaller chunks using the already available URL buffer.
 * The handler can be switch by uncommenting the FULL_RESPONSE_PARSER definition below.
 */

//#ifdef FULL_RESPONSE_PARSER

static int32_t GithubResponseParser(FileServerParams_t *pFileServerParams, HTTPClient_Handle httpClientHandle);


/* GITHUB API - parse response for directory files request, json RESPONSE, example:
    [
        {
            "name": "20151217_1_28.tar",
            "path": "OTA_R2/20151217_1_28.tar",
            "sha": "7b30216698eca244ac4851cc9f81aa022f5837ec",
            "size": 260096,
            "url": "https://api.github.com/repos/CC3X20/SL_OTA/contents/OTA_R2/20151217_1_28.tar?ref=master",
            "html_url": "https://github.com/CC3X20/SL_OTA/blob/master/OTA_R2/20151217_1_28.tar",
            "git_url": "https://api.github.com/repos/CC3X20/SL_OTA/git/blobs/7b30216698eca244ac4851cc9f81aa022f5837ec",
            "download_url": "https://raw.githubusercontent.com/CC3X20/SL_OTA/master/OTA_R2/20151217_1_28.tar",
            "type": "file",
            "_links": {
                "self": "https://api.github.com/repos/CC3X20/SL_OTA/contents/OTA_R2/20151217_1_28.tar?ref=master",
                "git": "https://api.github.com/repos/CC3X20/SL_OTA/git/blobs/7b30216698eca244ac4851cc9f81aa022f5837ec",
                "html": "https://github.com/CC3X20/SL_OTA/blob/master/OTA_R2/20151217_1_28.tar"
            }
        }
    ]
 */

#ifdef FULL_RESPONSE_PARSER
/*
 * returns: 0 - when new version is found
 *          1 - when new version is not found
 *          negative value upon error
 */
static int32_t GithubResponseParser(FileServerParams_t *pFileServerParams,
                                    HTTPClient_Handle httpClientHandle)
{
    int32_t ret = 0;
    uint8_t i,iNew;
    char *pJsonBuff;  /* Buffer for the entire JSON content */
    char contentLengthString[16];
    uint32_t contentLengthStringLen = 16;
    uint16_t nRead, nLeftToRead;
    int16_t numArrayMembers;
    Json_Handle templateHandle, objectHandle;
    bool bMoreDataFlag;
    bool bFound;
    char jsonKey[MAX_JSON_KEY_LEN];
    char filename[MAX_FILENAME_LEN];

    // Get size of JSON, which is the content length of the body received from GET request.
    ret = HTTPClient_getHeader(httpClientHandle,
                                 HTTPClient_HFIELD_RES_CONTENT_LENGTH,
                                 &contentLengthString, &contentLengthStringLen, 0);
    if(ret < 0) {
        LOG_ERROR("get header failed: %d", ret);
        return (ret);
    }
    nLeftToRead = strtoul(contentLengthString, NULL, 10);


    // Allocate enough memory to store entire JSON file
    pJsonBuff = (char*) malloc((nLeftToRead + 1) * sizeof(char));

    // Read HTTP GET response into pJsonBuff
    nRead = 0;
    bMoreDataFlag = false;
    bFound = false;
    do
    {
        ret = HTTPClient_readResponseBody(httpClientHandle, (pJsonBuff + nRead),
                                          nLeftToRead, &bMoreDataFlag);
        if(ret < 0)
        {
            LOG_ERROR("readResponse failed: %d", ret);
            free(pJsonBuff);
            return (ret);
        }
        nRead += ret;
        nLeftToRead -= ret;
    } while (bMoreDataFlag);
    LOG_TRACE("%s", pJsonBuff);

    // Setup JSON Template for parsing HTTP response
    ret = Json_createTemplate(&templateHandle, JSON_TEMPLATE,
                                strlen(JSON_TEMPLATE));
    if(ret < 0) {
        LOG_ERROR("create JSON template failed: %d", ret);
        free(pJsonBuff);
        return (ret);
    }

    // Setup JSON Object
    ret = Json_createObject(&objectHandle, templateHandle, 0);
    if(ret < 0) {
        LOG_ERROR("create JSON object failed: %d", ret);
        free(pJsonBuff);
        return (ret);
    }
    ret = Json_parse(objectHandle, pJsonBuff, strlen(pJsonBuff));
    if(ret < 0) {
        LOG_ERROR("parse JSON failed: %d", ret);
        free(pJsonBuff);
        return (ret);
    }

    // Search JSON for number of files (array members) in JSON
    numArrayMembers = Json_getArrayMembersCount(objectHandle, JSON_ARRAY_KEY);
    if(numArrayMembers < 0){
        free(pJsonBuff);
        return (ret);
    }

    iNew = 0;
    for(i = 0; (i < numArrayMembers && !iNew); i++)
    {


        snprintf(jsonKey, MAX_JSON_KEY_LEN,
                 "%s%d%s", "\"#\".[", i, "].\"name\"");
        LOG_DEBUG("jsonKey: %s", jsonKey);
        ret = Json_getValue(objectHandle, jsonKey,
                            filename, NULL);
        if(ret < 0){
            LOG_ERROR("get JSON value failed (%s), %d", jsonKey, ret)
                    return (ret);
        }
        LOG_DEBUG("Checking file [%d]: %s", i, filename);

        if(OTA_IF_isNewVersion(filename))
        {
            LOG_DEBUG("...New Image", i, filename);
            iNew = i;
            bFound = true;
            break;
        }
        else
        {
            LOG_DEBUG("...Old Image", i, filename);
            continue;
        }
    }

    // New file was not found in Github folder.
    if(!bFound)
    {
        free(pJsonBuff);
        return(OTA_VENDOR_NO_NEW_IMAGE_FOUND);
    }

    snprintf(jsonKey, MAX_JSON_KEY_LEN,
             "%s%d%s", "\"#\".[", iNew, "].\"download_url\"");
    LOG_TRACE("jsonKey: %s", jsonKey);
    ret = Json_getValue(objectHandle, jsonKey,
                        pFileServerParams->URL, NULL);
    if(ret < 0) {
        LOG_ERROR("get JSON value failed (%s): %d", jsonKey, ret);
        return (ret);
    }

    ret = Json_destroyTemplate(templateHandle);
    if(ret < 0) {
        return (ret);
    }
    ret = Json_destroyObject(objectHandle);
    if(ret < 0) {
        return (ret);
    }
    free(pJsonBuff);
    return(0);
}
#else // Simple Response Parser

/*!

    \brief     Skipping white space in JSON content

    \param[in] start - pointer to start of json buffer
    \param[in] end - pointer to end of json buffer

    \return    pointer to first non-space character
                (make sure it is not "end")

*/
char * jsonSkipWhiteSpace(char* start)
{
    while( (*start != 0) &&
            (*start == ' ' || *start == '\n' || *start == '\t') )
    {
        ++start;
    }
    return start;
}

/*!

    \brief     Copy a JSON value into allocated buffer based on given key

    \param[in] json - pointer to start of json buffer
    \param[in] key - pointer to the JSON Key string
    \param[out] output - pointer to the value buffer
    \param[out] outpitSize - size of the value (i.e. of the allocated buffer)

    \return    0 upon success or error code (see below).

*/
#define JSON_ERR_KEY_NOT_FOUND          -10001
#define JSON_ERR_VALUE_NOT_COMPLETE     -10002

int16_t jsonSimpleGetStr(char* jsonBuff, char* key, char** output, uint16_t* outputSize)
{
    bool isKey= false;
    int rc = 0;
    char *pKeyStart = jsonBuff;
    char *pValStart = NULL;

    /* Find an occurrence of key that is actually a JSON key
     * (i.e look for the sequence "<key>\s*:\s*\"").
     */
    do
    {
        // Move to next occurrence
        pKeyStart = strstr(pKeyStart, key);
        if(pKeyStart != NULL)
        {
            pValStart = pKeyStart + strlen(key);

            // Clear any whitespace
            pValStart = jsonSkipWhiteSpace(pValStart);

            // JSON specifies that a ':' must occur after a key.
            if(*pValStart++ == ':')
            {
                // Clear any whitespace
                pValStart = jsonSkipWhiteSpace(pValStart);

                // TODO: Make logic more generic? Extend to more than just getting a string...
                if(*pValStart++ == '\"'){
                    // Key does correspond to a string.
                    isKey = true;
                }
            }
        }
    } while(pKeyStart && isKey == false);


    if(isKey == false)
    {
        rc = JSON_ERR_KEY_NOT_FOUND;
    }
    else
    {
        /* Key was found, verify that the entire value is available */
        char *pValEnd = pValStart;
        do
        {
            pValEnd = strstr(pValEnd, "\"");
            // If found "\"" - Make sure it is not meant to be escaped.
            if( pValEnd && (*(pValEnd-1) != '\\') )
                break;
        } while(pValEnd != NULL);

        if(pValEnd == NULL)
        {
            /* Value End ('"') was not found, Move pointer back to catch the
             * key in the next attempt (having more data) */
            *output = pKeyStart;
            rc = JSON_ERR_VALUE_NOT_COMPLETE;
        }
        else
        {
            /* Key + Value found, report Value start and length */
            *output = pValStart;

            if(outputSize){
                *outputSize = pValEnd - pValStart;
            }
        }
    }
    return(rc);
}

#define MAX_READ_SIZE   (OTA_URL_LEN-1)
#define JSON_KEY        "\"download_url\""

/*
 * returns: 0 - when new version is found
 *          1 - when new version is not found
 *          negative value upon error
 */
static int32_t GithubResponseParser(FileServerParams_t *pFileServerParams,
                                    HTTPClient_Handle httpClientHandle)
{
    int32_t ret;
    bool moreDataFlag = false;
    /* Use the pre-allocated URL Buffer as temporary parsing buff */
    /* it is assumed that the size (OTA_URL_LEN) is enough for at least one GITHUB entry */
    char *pBuff = pFileServerParams->URL;
    uint16_t nRead = 0, nProcessed = 0;

    // TODO: move to external function and add in old code as well
    do {
        // First, Push non-processed bytes to the start of the buffer
        if(nRead > nProcessed)
        {
            memcpy(pBuff, pBuff+nProcessed, nRead-nProcessed);
            nRead -= nProcessed;
            nProcessed = 0;
        }
        // Second, Read the next chunk of the HTTP response, .
        ret = HTTPClient_readResponseBody(httpClientHandle, pBuff+nRead, MAX_READ_SIZE-nRead, &moreDataFlag);

        if(ret < 0)
        {
            return(ret);
        }
        else
        {
            nRead += ret;
            pBuff[nRead] = 0;
            LOG_DEBUG("GithubResponseParser: read chunk (read=%d)", nRead);
            LOG_TRACE("JSON: %s\n\r", pBuff);
        }

        do {
            char *urlLoc;
            uint16_t urlSize;
            char* filename;

            /* See if "download_url" is a key with a corresponding string value in the JSON.
             * If not, return to get the "next chunk" from HTTP response.
             */
            ret = jsonSimpleGetStr(pBuff+nProcessed, JSON_KEY, &urlLoc, &urlSize);

            if(ret == 0)
            {
                /* update the nProcess index according to the end location of the "download_url" value */
                nProcessed = (urlLoc - pBuff) + urlSize;
                LOG_DEBUG("GithubResponseParser: found URL (processed=%d)", nProcessed);

                /* The key/value pair existed and the value is now in pFileServerParams->URL
                 *    Find the filename from the URL.
                 *   TODO: fix for all cases
                 */
                filename = strstr(urlLoc, GITHUB_USER_PATH) + strlen(GITHUB_USER_PATH) + 1;
                LOG_DEBUG("File: %s", filename);

                if(OTA_IF_isNewVersion((uint8_t*)filename))
                {
                    // File is a new version.
                    memcpy((uint8_t*)pFileServerParams->URL, (uint8_t*)urlLoc, urlSize);
                    pFileServerParams->URL[urlSize] = '\0';
                    LOG_DEBUG("pFileServerParams->URL: %s", pFileServerParams->URL);
                    return(OTA_VENDOR_NEW_IMAGE_FOUND);
                } else {
                    // File is not new - keep searching in the folder...
                    LOG_DEBUG("candidate version: (%.*s) is not newer than current version",
                             OTA_VERSION_LEN, filename);
                    ret = 0;
                }
            }
            // These two errors are expected and handled. Exit in case of any other error.
            else if(ret == JSON_ERR_KEY_NOT_FOUND)
            {
                /* increment processed pointer, but keep some data from the end of the
                 * buffer in case the key (+json symbols) was cut in the middle
                 */
                nProcessed = nRead - (strlen("\"download_url\"") + 20);
                LOG_DEBUG("GithubResponseParser: Key Not Found (processed=%d)", nProcessed);
            }
            else if(ret == JSON_ERR_VALUE_NOT_COMPLETE)
            {
                /* In this case the Url location is pointing on the key start, so the key
                 * and the entire URL will be parsed in the next attempt (when more data
                 * will be read */
                nProcessed = (urlLoc - pBuff);
                LOG_DEBUG("GithubResponseParser: Value Not Complete (processed=%d)", nProcessed);
            }
            else
            {
                return(ret);
            }

        } while(ret == 0);
    } while(moreDataFlag );

    // There is no more data to check and no new version has been found.
    return(OTA_VENDOR_NO_NEW_IMAGE_FOUND);
}
#endif // FULL_RESPONSE_PARSER



/* PUBLIC API -
 * This can be called directly or as a  (primary or backup) callback for the OTA_IF_downloadImageByCloudVendor().
 */
int32_t OTA_GITHUB_getDownloadLink(FileServerParams_t *pFileServerParams) {
    int32_t ret = 0;
    HTTPClient_Handle httpClientHandle = NULL;
    LOG_INFO("*** Attempting GITHUB getDownloadLink ***");

    ret = HTTP_setupConnection(&httpClientHandle, HOSTNAME, NULL, NULL, GITHUB_ROOT_CA, ALT_GITHUB_ROOT_CA, 0);
    if(ret < 0){
        LOG_ERROR("Connection setup failed: %d", ret);
    }
    else
    {
        uint8_t *base64 = (uint8_t *)GITHUB_USER_TOKEN_B64;
        char userToken[sizeof(GITHUB_USER_TOKEN_B64) + sizeof("token")] = "token ";
        uint32_t len = sizeof(userToken) - 6;


        //int32_t StrMpl_decodeBase64(uint8_t *inputData, uint32_t inputLength, uint8_t *outputData, uint32_t *outputLength);
        ret = StrMpl_decodeBase64(base64, sizeof(GITHUB_USER_TOKEN_B64)-1, (uint8_t*)&userToken[6], &len);
        if(ret < 0){
            LOG_ERROR("Token Base64 decoding failed: %d", ret);
        }
        else
        {
            userToken[6+len-1] = 0;

            // prepare the HTTP header
            ret = HTTP_setHeaders(httpClientHandle, USER_AGENT, userToken, CONTENT_TYPE, HTTPClient_HFIELD_NOT_PERSISTENT);

            if(ret < 0)
            {
                LOG_ERROR("setting HTTP headers failed: %d", ret);
            }
            else
            {
                // Send HTTP GET request
                LOG_INFO("=== Sending GET Request ===\r\n uri: %s", REQUEST_URI);
                ret = HTTPClient_sendRequest(httpClientHandle, HTTP_METHOD_GET,
                                             REQUEST_URI, NULL, 0, 0);
                LOG_DEBUG("HTTP Response Status Code: %d", ret);
                if(ret < 0) {
                    LOG_ERROR("Sending Get request failed: %d", ret);
                }
                else if(ret > 0 && ret != 200)
                {
                    LOG_ERROR("HTTP Response Error: %d", ret);
                    ret = (0-ret);
                }
                else
                {
                    ret = GithubResponseParser(pFileServerParams, httpClientHandle);
                    if(ret == OTA_VENDOR_NO_NEW_IMAGE_FOUND) {
                        LOG_INFO("*** No new image was found!!! ***");
                    }
                    else if(ret < 0) {
                        LOG_ERROR("Parsing response failed: %d", ret);
                    }
                    else // (ret == 0) -> success
                    {
                        LOG_INFO("*** New Image Found: (URL: %s) ***", pFileServerParams->URL);
                        pFileServerParams->pClientCert = NULL;
                        pFileServerParams->pPrivateKey = NULL;
                        pFileServerParams->pRootCa1     = GITHUB_ROOT_CA;
                        pFileServerParams->pRootCa2     = ALT_GITHUB_ROOT_CA;
                        pFileServerParams->pVersion = NULL; /* NULL means that the version will be compared again
                                                            in OTA_IF against the timestamp at the beginning of the
                                                            tar file (which is part of the filename).
                                                            In case the version should be something else, make sure
                                                            allocate the space for it (and to free it after the
                                                            downloadImage call returns). */
                    }
                }
            }
        }
        if(HTTPClient_disconnect(httpClientHandle) < 0) {
            LOG_ERROR("disconnect failed: %d", ret);
        }
        if(HTTPClient_destroy(httpClientHandle) < 0) {
            LOG_ERROR("destroy failed: %d", ret);
        }
    }
    return(ret);
}

#endif
