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

//*****************************************************************************
//
//! \addtogroup file_operations
//! @{
//
//*****************************************************************************

// Standard includes
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <mqueue.h>

#include <ti/drivers/net/wifi/simplelink.h>
#include "httpsrv_if.h"
#include "utils_if.h"
#include "debug_if.h"

#undef DEBUG_IF_NAME
#define DEBUG_IF_NAME       "HTTPSRV"
#undef DEBUG_IF_SEVERITY
#define DEBUG_IF_SEVERITY   E_INFO

//*****************************************************************************
//                 GLOBAL DEFINITIONS
//*****************************************************************************
/* Content types list */
#define TEXT_HTML                    "text/html"
#define TEXT_CSS                     "text/css"
#define TEXT_XML                     "text/xml"
#define APPLICATION_JSON             "application/json"
#define IMAGE_PNG                    "image/png"
#define IMAGE_GIF                    "image/gif"
#define TEXT_PLAIN                   "text/plain"
#define TEXT_CSV                     "text/csv"
#define APPLICATION_JAVASCRIPT       "application/javascript"
#define IMAGE_JPEG                   "image/jpeg"
#define APPLICATION_PDF              "application/pdf"
#define APPLICATION_ZIP              "application/zip"
#define SHOCKWAVE_FLASH              "application/x-shockwave-flash"
#define AUDIO_X_AAC                  "audio/x-aac"
#define IMAGE_X_ICON                 "image/x-icon"
#define TEXT_VCARD                   "text/vcard"
#define APPLICATION_OCTEC_STREAM     "application/octet-stream"
#define VIDEO_AVI                    "video/avi"
#define VIDEO_MPEG                   "video/mpeg"
#define VIDEO_MP4                    "video/mp4"
#define FORM_URLENCODED              "application/x-www-form-urlencoded"

// Request Context
typedef struct
{
    uint16_t type;              // input
    uint16_t sessionHandle;     // input
    uint32_t contentLen;        // input
    uint32_t flags;             // input
    char *pQueryStr;        // input
    uint16_t queryStrLen;       // input
    uint8_t *pMetadata;         // input
    uint16_t metadataLen;        // input
    uint8_t *pPayload;          // input
    uint16_t payloadLen;        // input
    uint8_t *pResponse;         // output
    uint16_t *pResponseLen;     // input (max length)/output (Actual length)
    uint16_t responseOffset;    // output
} httpRequest_t;


// HTTP Handler per request
typedef int32_t (*httpRequestHandler_f)(httpRequest_t *);

// User Callback
typedef struct _httpUserParams_t
{
    char                            *pURI;
    HTTPSRV_IF_Callback_t           pCB;
    struct _httpUserParams_t        *pNext;
} httpUserParams_t;

//*****************************************************************************
// Static Functions
//*****************************************************************************
static int HTTPGetHandler(httpRequest_t *pRequest);
static int HTTPPostHandler(httpRequest_t *pRequest);
static void prepareImmediateResponse(SlNetAppResponse_t *pResponse, uint16_t status);
static void *searchMetadata(uint8_t reqType,
                            uint8_t *pMetadata, uint16_t MetadataLen,
                            uint16_t *pLen);
static int prepareMetadata(uint8_t *pMetadata, int16_t status,
                            uint32_t contentLen, char *contentType);
static void setNetAppHttp(int *rc, const uint8_t Option,
                          const uint8_t OptionLen, const uint8_t *pOptionValue);
static char *lookForNextDelimeter(char *pBase, char *pEnd, char delim);


//*****************************************************************************
//                 GLOBAL VARIABLES
//*****************************************************************************

// Currently only GET and POST request are supported
const httpRequestHandler_f httpCallbacks[4] =
{
    (httpRequestHandler_f)HTTPGetHandler,       // GET
    NULL,                                       // DELETE
    (httpRequestHandler_f)HTTPPostHandler,      // POST
    NULL,                                       // PUT
};

httpUserParams_t *g_pUserParams[4] = {0, 0, 0, 0};

static struct
{
    workQ_t         hWQ;
    bool            bInit;
} m_http = { 0 };


//*****************************************************************************
//
//! \brief
//
//*****************************************************************************
static int32_t WorkQCallback(void *hMsg)
{
    httpRequest_t *pRequest = (httpRequest_t*)hMsg;
    LOG_DEBUG("HTTPSRVThread:: (type=%d)", pRequest->type);

    if( (pRequest->type > SL_NETAPP_REQUEST_HTTP_DELETE) ||
            (httpCallbacks[pRequest->type-1] == NULL) ) {
        return -1;
    }
    return httpCallbacks[pRequest->type-1](pRequest);
}

static int scheduleHttpTask(httpRequest_t *pRequest)
{
    return WQ_schedule(m_http.hWQ, WorkQCallback, pRequest, 0);
}

//*****************************************************************************
//
//! \brief This function configures netapp settings
//!
//! \param[in]  None
//!
//! \return None
//!
//****************************************************************************
static void setNetAppHttp(int *rc, const uint8_t Option, const uint8_t OptionLen, const uint8_t *pOptionValue)
{
    if(*rc == 0)
    {
        *rc =  sl_NetAppSet(SL_NETAPP_HTTP_SERVER_ID, Option, OptionLen, pOptionValue);
        if(*rc != 0)
        {
            LOG_ERROR("HTTP setting error: %d, option: %d ", rc, Option);
        }
    }
}

//*****************************************************************************
//
//! \brief This function prepare error netapp response in case memory could
//!        not be allocated
//!
//! \param[in]  pResponse    netapp response structure
//!
//! \return none
//!
//****************************************************************************
static void prepareImmediateResponse(SlNetAppResponse_t *pNetAppResponse, uint16_t status)
{

    /* Prepare error response */
    pNetAppResponse->Status = status;
    pNetAppResponse->ResponseData.pMetadata = NULL;
    pNetAppResponse->ResponseData.MetadataLen = 0;
    pNetAppResponse->ResponseData.pPayload = NULL;
    pNetAppResponse->ResponseData.PayloadLen = 0;
    pNetAppResponse->ResponseData.Flags = 0;
}



//*****************************************************************************
//
//! \brief This function prepares metadata for HTTP GET requests
//!
//! \param[in] parsingStatus      validity of HTTP GET request
//!
//! \param[in] contentLen         content length in respond to HTTP GET request
//!
//! \return metadataLen
//!
//****************************************************************************
static int prepareMetadata(uint8_t *pMetadata, int16_t status,
                            uint32_t contentLen, char *contentType)
{
    uint8_t *p = pMetadata;

    /* http status */
    *p++ = (uint8_t) SL_NETAPP_REQUEST_METADATA_TYPE_STATUS;
     *(uint16_t *)p = (uint16_t) 2;
    p += 2;
    *(uint16_t *)p = status;
    p += 2;

    if(contentLen)
    {
        /* Content type */
        *p++ = (uint8_t) SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_TYPE;
        *(uint16_t *)p = (uint16_t) strlen ((const char *)contentType);
        p += 2;
        sl_Memcpy (p, contentType, strlen((const char *)contentType));
        p += strlen((const char *)contentType);

        /* Content len */
        *p++ = SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_LEN;
        *(uint16_t *)p = (uint16_t) 4;
        p += 2;
        *(uint32_t *)p = (uint32_t) contentLen;
        p += 4;
    }

    return(p-pMetadata);
}





/* returns a pointer to the requested value (by type) in the original metadata buffer,
 * If needed, the caller should copy the content to its own structures.
 * Returns NULL, if type was not found.
 */
static void *searchMetadata(uint8_t reqType, uint8_t *pMetadata, uint16_t MetadataLen, uint16_t *pLen)
{
    int i = 0;

    while(i < MetadataLen)
    {
        uint8_t  type = pMetadata[i];
        uint16_t len  = *(uint16_t*)&pMetadata[i+1];
        if(type == reqType)
        {
            *pLen = len;
            return &pMetadata[i+3];
        }
        i += (3 + len);
    }
    // can't find the type
    *pLen = 0;
    LOG_DEBUG("searchMetadata:: Can't find type=%d", reqType);

    return NULL;

}

static char *lookForNextDelimeter(char *pBase, char *pEnd, char delim)
{
    while(*pBase != delim)
    {
        pBase ++;
    }
    return pBase;
}



static int HTTPGetHandler(httpRequest_t *pRequest)
{
    char *pURI;
    uint16_t URILen;
    httpUserParams_t *pUserParams = g_pUserParams[SL_NETAPP_REQUEST_HTTP_GET-1];

    pURI = searchMetadata(SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_REQUEST_URI,
                             pRequest->pMetadata, pRequest->metadataLen,
                             &URILen);

    LOG_DEBUG("HTTPGetHandler:: (uri=%s)", pURI);

    if(pURI)
    {
        while(pUserParams)
        {
            if(strncmp(pUserParams->pURI, pURI, URILen) == 0)
            {
                uint8_t response[200];
                uint16_t responselen = sizeof(response);
                uint16_t paramLen;

                // In GET request the Query String is part of the HTTP Headers and received as metadata
                pRequest->pQueryStr = searchMetadata(SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_QUERY_STRING,
                                          pRequest->pMetadata, pRequest->metadataLen,
                                          &pRequest->queryStrLen);
                // The content Len is part of the HTTP Headers and received as metadata
                uint32_t *p = searchMetadata(SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_LEN,
                                      pRequest->pMetadata, pRequest->metadataLen,
                                      &paramLen);
                pRequest->contentLen = *p;

                pRequest->pResponse = response;
                pRequest->pResponseLen = &responselen;
                pRequest->responseOffset = 0;
                if(pUserParams->pCB)
                    return pUserParams->pCB(pRequest, pRequest->pPayload, pRequest->payloadLen);
            }
            pUserParams = pUserParams->pNext;
        }
    }
    LOG_WARNING("HTTGetHandler: can't find handler for URI=%.*s", (pURI)?URILen:1, (pURI)?pURI:"-");
    HTTPSRV_IF_respond(pRequest, -1, NULL, 0);
    return -1;
}

static int HTTPPostHandler(httpRequest_t *pRequest)
{
    char *pURI;
    uint16_t URILen;
    httpUserParams_t *pUserParams = g_pUserParams[SL_NETAPP_REQUEST_HTTP_POST-1];

    pURI = searchMetadata(SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_REQUEST_URI,
                             pRequest->pMetadata, pRequest->metadataLen,
                             &URILen);
    if(pURI)
    {
        while(pUserParams)
        {
            // In POST request the Query String is payload
            if(strncmp(pUserParams->pURI, pURI, URILen) == 0)
            {
                uint32_t *p;
                uint8_t response[200];
                uint16_t responselen = sizeof(response);
                uint16_t paramLen;

                pRequest->pQueryStr = searchMetadata(SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_QUERY_STRING,
                                          pRequest->pMetadata, pRequest->metadataLen,
                                          &pRequest->queryStrLen);
                // In POST request the Query String is part of the content (payload)
                if(pRequest->pQueryStr == NULL && pRequest->pPayload)
                {
                    pRequest->pQueryStr = (char*)pRequest->pPayload;
                    pRequest->queryStrLen = strlen((char*)pRequest->pPayload);
                }
                p = searchMetadata(SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_LEN,
                                      pRequest->pMetadata, pRequest->metadataLen,
                                      &paramLen);
                if(p)
                {
                    pRequest->contentLen = *p;
                    pRequest->pResponse = response;
                    pRequest->pResponseLen = &responselen;
                    pRequest->responseOffset = 0;
                    if(pUserParams->pCB)
                        return  pUserParams->pCB(pRequest, pRequest->pPayload, pRequest->payloadLen);
                }
            }
            pUserParams = pUserParams->pNext;
        }
    }
    LOG_WARNING("HTTPostHandler: can't find handler for URI=%.*s", (pURI)?URILen:1, (pURI)?pURI:"-");
    HTTPSRV_IF_respond(pRequest, -1, NULL, 0);
    return -1;
}



//*****************************************************************************
// External Functions
//*****************************************************************************

//*****************************************************************************
//
//! \brief This function is registered as netapp request callback
//!
//! \param[in]  pRequest       netapp request structure
//!
//! \param[out] pResponse      netapp response structure
//!
//! \return none
//!
//****************************************************************************
void SimpleLinkNetAppRequestEventHandler(SlNetAppRequest_t *pNetAppRequest,
                                         SlNetAppResponse_t *pNetAppResponse)
{
    httpRequest_t *pRequest;

    if(!m_http.bInit)
    {
        LOG_DEBUG("SimpleLinkNetAppRequestEventHandler: HTTPSRV_IF is not ready to handle requests!");
        prepareImmediateResponse(pNetAppResponse, SL_NETAPP_RESPONSE_NONE);
        return;
    }


    LOG_DEBUG("NetApp Request Received - AppId = %d, Type = %d, Handle = %d",
        pNetAppRequest->AppId, pNetAppRequest->Type, pNetAppRequest->Handle);

    if( (pNetAppRequest->AppId == SL_NETAPP_HTTP_SERVER_ID)
                            &&
       ((pNetAppRequest->Type == SL_NETAPP_REQUEST_HTTP_GET) ||
        (pNetAppRequest->Type == SL_NETAPP_REQUEST_HTTP_DELETE) ||
        (pNetAppRequest->Type == SL_NETAPP_REQUEST_HTTP_POST) ||
        (pNetAppRequest->Type == SL_NETAPP_REQUEST_HTTP_PUT))
                            &&
        (httpCallbacks[pNetAppRequest->Type - 1] != NULL) )
    {
        /* Prepare pending response */
        LOG_DEBUG("handling netapp request");
        prepareImmediateResponse(pNetAppResponse, SL_NETAPP_RESPONSE_PENDING);
    }
    else
    {
        LOG_ERROR("unsupported netapp request!");
        prepareImmediateResponse(pNetAppResponse, SL_NETAPP_RESPONSE_NONE);
        return;
    }

    pRequest = (httpRequest_t *)calloc(1, sizeof(httpRequest_t) + pNetAppRequest->requestData.MetadataLen + pNetAppRequest->requestData.PayloadLen);
    if(NULL == pRequest)
    {
        LOG_ERROR("out of resources!");
        prepareImmediateResponse(pNetAppResponse, SL_NETAPP_RESPONSE_NONE);
        return;
    }
    pRequest->type = pNetAppRequest->Type;
    pRequest->sessionHandle = pNetAppRequest->Handle;
    pRequest->flags = pNetAppRequest->requestData.Flags;
    pRequest->payloadLen = pNetAppRequest->requestData.PayloadLen;
    pRequest->metadataLen = pNetAppRequest->requestData.MetadataLen;
    pRequest->pMetadata =  ((uint8_t*)&pRequest[1]);
    pRequest->pPayload = (pRequest->pMetadata + pRequest->metadataLen);

    /* Copy Metadata */
    if(pRequest->metadataLen > 0)
    {
        memcpy(pRequest->pMetadata, pNetAppRequest->requestData.pMetadata, pRequest->metadataLen);
    }

    /* Copy the payload */
    if(pRequest->payloadLen > 0)
    {
        memcpy(pRequest->pPayload, pNetAppRequest->requestData.pPayload, pRequest->payloadLen);
    }
    scheduleHttpTask(pRequest);
}



//*****************************************************************************
//
//! \brief  This function configures the HTTPS server
//!
//! \param  pServerCert         Server's Certificate filename (NULL if no security is needed)
//!
//! \param  pServerKey          Server's Private Key filename
//!
//! \param  pClientRootCACert   Clients' root CA (if client authentication is needed)
//!
//! \return NetApp error codes or 0 upon success.
//!
//*****************************************************************************
int HTTPSRV_IF_config(HTTPSRV_IF_params_t *pSrvParams)
{
    int rc = 0;
    int status = 0;
    SlFsFileInfo_t FsFileInfo;
    uint16_t primaryPort = DEFAULT_HTTP_SRV_PORT;
    uint16_t secondaryPort = 0;
    uint8_t securityMode = 0;
    uint8_t secondaryPortEnable = 0;

    /* By default - only set primary to HTTP (not secure) port
     */


    if(pSrvParams)
    {
        /* Update the primary/secondary ports if needed */
        if(pSrvParams->primaryPort != 0)
        {
            primaryPort = pSrvParams->primaryPort;
        }
        secondaryPort = pSrvParams->secondaryPort;

        /* Check for HTTPS params */
        if(pSrvParams->pServerCert)
        {
            LOG_DEBUG("Configure Server for secured mode...");
            securityMode = 1;
            /* Check if the file used for SSL exists in the FS */
            status = sl_FsGetInfo((const uint8_t *)pSrvParams->pServerCert, 0, &FsFileInfo);
            if(status < 0)
            {
                if (status == SL_ERROR_FS_FILE_NOT_EXISTS)
                {
                    LOG_ERROR("File %s status=SL_ERROR_FS_FILE_NOT_EXISTS\r\n",pSrvParams->pServerCert);
                }
                else
                {
                    LOG_ERROR("Error sl_FsOpen %s, Status=%d\r\n", pSrvParams->pServerCert, status);
                }
                return status;
            }
            /* Check if the file used for SSL key exists in the FS */
            status = sl_FsGetInfo((const uint8_t *)pSrvParams->pServerKey, 0, &FsFileInfo);
            if(status < 0)
            {
                if (status == SL_ERROR_FS_FILE_NOT_EXISTS)
                {
                    LOG_ERROR("File %s status=SL_ERROR_FS_FILE_NOT_EXISTS\r\n",pSrvParams->pServerKey);
                }
                else
                {
                    LOG_ERROR("Error sl_FsOpen %s, Status=%d\r\n", pSrvParams->pServerKey, status);
                }
                return status;
            }
            /* Set the file names used for SSL key exchange */
            setNetAppHttp(&rc, SL_NETAPP_HTTP_DEVICE_CERTIFICATE_FILENAME,
                          strlen((char *)pSrvParams->pServerCert), (const uint8_t *)pSrvParams->pServerCert);

            setNetAppHttp(&rc, SL_NETAPP_HTTP_PRIVATE_KEY_FILENAME,
                          strlen((char *)pSrvParams->pServerKey), (const uint8_t *)pSrvParams->pServerKey);

            if(pSrvParams->pClientRootCa)
            {
                setNetAppHttp(&rc, SL_NETAPP_HTTP_CA_CERTIFICATE_FILE_NAME,
                              strlen((char *)pSrvParams->pClientRootCa), (const uint8_t *)pSrvParams->pClientRootCa);
            }
        }
    }
    setNetAppHttp(&rc, SL_NETAPP_HTTP_PRIMARY_PORT_NUMBER,
                  sizeof(primaryPort), (uint8_t *)&primaryPort);
    if(secondaryPort)
    {
        /* Enable secondary HTTP port (can only be used for redirecting
         connections to the secure primary port) */
        setNetAppHttp(&rc, SL_NETAPP_HTTP_SECONDARY_PORT_NUMBER,
                      sizeof(secondaryPort), (uint8_t *)&secondaryPort);
        secondaryPortEnable = 1;
    }
    setNetAppHttp(&rc,
                  SL_NETAPP_HTTP_SECONDARY_PORT_ENABLE,
                  sizeof(secondaryPortEnable),
                  &secondaryPortEnable);

    setNetAppHttp(&rc,
                  SL_NETAPP_HTTP_PRIMARY_PORT_SECURITY_MODE,
                  sizeof(securityMode),
                  &securityMode);

    if(rc >= 0)
    {
        rc = sl_NetAppStop(SL_NETAPP_HTTP_SERVER_ID);
        LOG_DEBUG("HTTP Server Stopped");

        rc = sl_NetAppStart(SL_NETAPP_HTTP_SERVER_ID);
        while (rc == SL_ERROR_HTTP_SERVER_ENABLE_FAILED)
        {
            sleep(1);
            rc = sl_NetAppStart(SL_NETAPP_HTTP_SERVER_ID);
        }

        LOG_DEBUG("HTTP Server Re-started");
    }
    return rc;
}


//*****************************************************************************
//!
//! \brief Register a handler for HTTP request (based on type and URI)
//!
//! \param[in] reqType     SL_NETAPP_REQUEST_HTTP_GET/PUT/POST/DELETE
//!
//! \param[in] pURI        URI, starting with "/" (e.g. "/device").
//!                         note: used by reference (content is not copied), expecting
//!                         the caller to keep it valid until it is unregistered.
//!
//! \param[in] fCallback   pointer to the handler function
//!
//! \return 0, upon success, or negative number
//!
//****************************************************************************
int HTTPSRV_IF_registerCallback(int reqType, char *pURI, HTTPSRV_IF_Callback_t fCallback)
{
    httpUserParams_t *pUserParams = g_pUserParams[reqType-1];
    if(pURI == NULL || pURI[0] != '/' || fCallback == NULL)
    {
        LOG_ERROR("HTTPSRV_IF_registerCallback: wrong parameter");
        return SL_INVALPARAM;
    }

    /* First check if the URI is already registered */
    while(pUserParams)
    {
        if(strcmp(pUserParams->pURI, pURI) == 0)
        {
            /* If URI is already found - replace the callback */
            pUserParams->pCB = fCallback;
            return 0;
        }
        pUserParams = pUserParams->pNext;
     }

    /* if we got here - this is a new URI (entry allocation is needed) */
    pUserParams = (httpUserParams_t*)malloc(sizeof(httpUserParams_t));
    if(pUserParams == NULL)
    {
        LOG_ERROR("HTTPSRV_IF_registerCallback: Entry allocation error");
        return SL_ERROR_BSD_ENOMEM;
    }

    pUserParams->pCB = fCallback;
    pUserParams->pURI = pURI;
    pUserParams->pNext = g_pUserParams[reqType-1];
    g_pUserParams[reqType-1] = pUserParams;
    return 0;
}

//*****************************************************************************
//!
//! \brief UnRegister a handler for HTTP request (based on type and URI)
//!
//! \param[in]  reqType     SL_NETAPP_REQUEST_HTTP_GET/PUT/POST/DELETE
//!
//! \param[out] pURI        URI, starting with "/" (e.g. "/device")
//!
//! \return 0, upon success, or negative number
//!
//****************************************************************************
int HTTPSRV_IF_unregisterCallback(int reqType, char *pURI)
{
    httpUserParams_t *pUserParams = g_pUserParams[reqType-1];
    httpUserParams_t *pPrevParams = NULL;
    if(pURI == NULL || pURI[0] != '/')
    {
        LOG_ERROR("HTTPSRV_IF_unregisterCallback: wrong parameter");
        return SL_INVALPARAM;
    }

    /* First check if the URI is already registered */
    while(pUserParams)
    {
        if(strcmp(pUserParams->pURI, pURI) == 0)
        {
            if(pPrevParams)
            {
                pPrevParams->pNext = pUserParams->pNext;
            }
            else
            {
                g_pUserParams[reqType-1] = pUserParams->pNext;
            }
            free (pUserParams);
            return 0;
        }
        pPrevParams = pUserParams;
        pUserParams = pUserParams->pNext;
     }
    /* if we got here - the URI was not found */
    LOG_WARNING("HTTPSRV_IF_unregisterCallback: pURI (%s) was not found", pURI);
    return -1;
}

int HTTPSRV_IF_getContentLength(httpHandle_t hRequest)
{
    httpRequest_t *pRequest = (httpRequest_t *)hRequest;
    return pRequest->contentLen;

}

//*****************************************************************************
//!
//! \brief This function scans received query string, looking for certain
//!        attribute (pKey)
//!
//! \param[in]  pRequest        context of the http request
//!
//! \param[in]  pKey        'key' (characteristic name) string to look for
//!
//! \param[out] pValue      pointer to a buffer to store the characteristic value (if exist)
//!
//! \return negative value if the characteristic was not found.
//!                  if found, return the length of the retrieved characteristic
//!                            value, or 0 there is no value associated
//!
//****************************************************************************
int HTTPSRV_IF_searchQueryString(httpHandle_t hRequest, char *pKey, char *pValue)
{
    httpRequest_t *pRequest = (httpRequest_t *)hRequest;
    /* Parse payload list */
    char *pToken = pRequest->pQueryStr;
    char *pStrEnd = (pRequest->pQueryStr + pRequest->queryStrLen);

    if(pKey != NULL)
    {
        while(pToken && (pToken < pStrEnd))
        {
            char *pNextDelimeter = lookForNextDelimeter(pToken, pStrEnd, '&');

            /* run over all possible characteristics, if exist */
            if(0 == strncmp(pToken, pKey, strlen(pKey)))
            {
                char *p = &pToken[strlen(pKey)];
                LOG_DEBUG("[wifi_http_search_query_string] characteristic (%s) was found!", pKey);
                if(*p == '=')
                {
                    // if there is a given value, copy it to user buffer
                    p++;
                    int valueLen = (pNextDelimeter-p);

                    memcpy(pValue, p, valueLen);
                    pValue[valueLen] = 0;
                    return valueLen;
                }
                if(*p == '&' || p == pStrEnd)
                {
                    // return 0 if not token was found without value set
                    return 0;
                }
            }

            pToken = pNextDelimeter + 1;
        }
    }
    // failed to find a match
    return(-1);
}

//*****************************************************************************
//
//! \brief This function sets the parameter and velue ("key=vlaue") in the output
//!         query string (it will be sent when wifi_http_repsond gets invoked)
//!
//! \param[in]  pRequest        context of the http request
//!
//! \param[in]  pKey        'key' (characteristic name) string to look for
//!
//! \param[in] pValue      pointer to a buffer to store the characteristic value (if exist)
//!
//! \return negative upon any failure, 0 - upon success
//!
//****************************************************************************
int HTTPSRV_IF_setResponseQueryString(httpHandle_t hRequest, char *pKey, char *pValue)
{
    httpRequest_t *pRequest = (httpRequest_t *)hRequest;
   if(pRequest->responseOffset)
    {
        pRequest->pResponse[pRequest->responseOffset] = '&';
        pRequest->responseOffset++;
    }
    memcpy(&pRequest->pResponse[pRequest->responseOffset], pKey, strlen(pKey));
    pRequest->responseOffset += strlen(pKey);

    pRequest->pResponse[pRequest->responseOffset] = '=';
    pRequest->responseOffset++;

    strcpy((char*)&pRequest->pResponse[pRequest->responseOffset], pValue);
    pRequest->responseOffset += strlen(pValue);

    return 0;
}



//*****************************************************************************
//
//! \brief Use this function to receive the next chunk of a fragmented HTTP POST payload
//!
//! \param[in]      pRequest    context of the http request
//!
//! \param[out]     *pData     Pointer for received data
//!
//! \param[in,out]  *DataLen   (pointer to) Max buffer size (in) / Actual data received (out)
//!
//! \return negative upon any failure, 0 - upon success
//!
//****************************************************************************
int HTTPSRV_IF_receive(httpHandle_t hRequest, uint8_t *pData, uint16_t *pDataLen)
{
    httpRequest_t *pRequest = (httpRequest_t *)hRequest;

    return sl_NetAppRecv(pRequest->sessionHandle, pDataLen, pData, (_u32*)&pRequest->flags);
}



//*****************************************************************************
//
//! \brief Send the HTTP response to the caller (including: metadata/headers,
//!         Query String (if set before, using "wifi_http_set_query_string"),
//!         and provided header.
//!
//! \param[in]  pRequest       context of the http request
//!
//! \param[in]  status     negative value indicates a failure (currently only 404 is supported)
//!                        0 indicates success (200/204)
//!
//! \param[in]  pPayload   pointer to user response payload (if exists, i.e.. in GET request), or NULL
//!
//! \param[in]  payloadLen length user response payload, or 0
//!
//! \return negative if any failure occured, or 0 upon success response
//!
//****************************************************************************
int HTTPSRV_IF_respond(httpHandle_t hRequest, int status, uint8_t *pPayload, uint32_t payloadLen)
{
    httpRequest_t *pRequest = (httpRequest_t *)hRequest;
    const uint8_t pageNotFound[] = "<html>404 - Sorry page not found</html>";
    uint8_t  metadata[100];
    uint16_t metadataStatus;
    int rc;
    int outputLen = pRequest->responseOffset + payloadLen;
    int metadataLen = 0;
    uint32_t flag;

    if(status < 0)
    {

        metadataStatus = (uint16_t) SL_NETAPP_HTTP_RESPONSE_404_NOT_FOUND;
        if(pRequest->type == SL_NETAPP_REQUEST_HTTP_GET && payloadLen == 0)
        {
            payloadLen = strlen((char*)pageNotFound);
            pPayload = (uint8_t*)pageNotFound;
        }
    }
    else
    {
        if(outputLen)
            metadataStatus = (uint16_t) SL_NETAPP_HTTP_RESPONSE_200_OK;
        else
            metadataStatus = (uint16_t) SL_NETAPP_HTTP_RESPONSE_204_OK_NO_CONTENT;
    }
    metadataLen = prepareMetadata(metadata, metadataStatus, payloadLen, TEXT_HTML);


    if(metadataLen)
    {
        flag = SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA;
        if(outputLen)
        {
            flag |= SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION;
        }
        rc = sl_NetAppSend (pRequest->sessionHandle, metadataLen, metadata, flag);
        LOG_DEBUG("wifi_http_respond:: Metadata len = %d (status=%d)", metadataLen, rc);
    }
    if(pRequest->responseOffset)
    {
        flag = 0;
        if(payloadLen)
        {
            flag |= SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION;
        }
        rc = sl_NetAppSend (pRequest->sessionHandle, pRequest->responseOffset, pRequest->pResponse, flag); /* mark as last segment */
        LOG_DEBUG("wifi_http_respond:: Query string len = %d (status=%d)", pRequest->responseOffset, rc);
        LOG_DEBUG("                    %s", pRequest->pResponse);
    }
    if(payloadLen)
    {
        rc = sl_NetAppSend (pRequest->sessionHandle, payloadLen, pPayload, 0); /* mark as last segment */
         LOG_DEBUG("wifi_http_respond:: Data len = %d (status=%d)", payloadLen, rc);
    }
    free(pRequest);
    return rc;
}




int HTTPSRV_IF_init(workQ_t hWQ)
{
    int rc = SL_ERROR_BSD_EINVAL;

    if(m_http.bInit == false)
    {
        if(hWQ) {
            m_http.hWQ = hWQ;
        }
        else
        {
            /* If not provided with Work Queue (hWQ == NULL),
             * need to create one for module */
            m_http.hWQ = WQ_create(HTTPSRV_THREAD_PRIORITY, HTTPSRV_THREAD_STACK_SIZE, 8, "HTTPSRV");
        }
        if(m_http.hWQ) {
            rc = 0;
            m_http.bInit = true;
            LOG_INFO("Initialized successfully");
        }
        else {
            LOG_ERROR("Initialization error (%d)", rc);
        }
    }
    else
    {
        LOG_WARNING("Already initialized");
        rc = SL_RET_CODE_DEV_ALREADY_STARTED;
    }
    return rc;
}




//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
