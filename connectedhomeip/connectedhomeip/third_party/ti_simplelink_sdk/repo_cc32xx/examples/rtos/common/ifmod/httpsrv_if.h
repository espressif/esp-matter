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
// Includes
//*****************************************************************************
// Standard includes

#ifndef HTTPSRV_IF_H
#define HTTPSRV_IF_H

#include <stdint.h>
#include "utils_if.h"

#define HTTPSRV_THREAD_STACK_SIZE   2048
#define HTTPSRV_THREAD_PRIORITY     2


// the defaults will be used when HTTPSRV_IF_config primaryPort == 0
#define DEFAULT_HTTP_SRV_PORT       80
#define DEFAULT_HTTPS_SRV_PORT      443


typedef struct
{
    unsigned int stack_size;
    uint8_t thread_priority;
} initParams_t;

typedef void *httpHandle_t;
typedef int (*HTTPSRV_IF_Callback_t)(httpHandle_t hRequest, uint8_t *pPayload, uint16_t payloadLen);
typedef struct
{
    uint16_t primaryPort;   /* set to primary port or 0 to use default (80) */
    uint16_t secondaryPort; /* set to secondary port or 0 to use default (no secondary) */
    char *pServerCert;      /* set to HTTPS server's certificate, or to NULL (when using HTTP) */
    char *pServerKey;       /* set to HTTPS server's private key (null when usign HTTP) */
    char *pClientRootCa;    /* set to Client root CA (if needed), or to NULL (if not needed) */
} HTTPSRV_IF_params_t;

/*!

    \brief     HTTP Server initialization.

    \param[in] hWQ - handle to a Work Queue.
                     If provided - use the caller WQ.
                     If null - create the WQ for the module.

    \return    0 upon success or a negative error code

    \note      Should be called (typically at boot time) before any access
               to other HTTPSRV_IF.

*/
int HTTPSRV_IF_init(workQ_t hWQ);

/*!

    \brief     HTTP Server configuration.

    \param[in] pSrvParams - pointer to user configuration, or NULL for default assignment.

    \return    0 upon success or a negative error code

*/
int HTTPSRV_IF_config(HTTPSRV_IF_params_t *pSrvParams);

/*!

    \brief     Register a callback for specific HTTP Request.

    \param[in] reqType - type of request (SL_NETAPP_REQUEST_HTTP_POST, SL_NETAPP_REQUEST_HTTP_POST)
    \param[in] pURI - URI string, starting with '/' (e.g. "/abc")
               note: used by reference (content is not copied), expecting
                     the caller to keep it valid until calling the "HTTPSRV_IF_registerCallback".
    \param[in] pCB - Callback pointer

    \return    0 upon success or a negative error code

*/
int HTTPSRV_IF_registerCallback(int reqType, char *pURI, HTTPSRV_IF_Callback_t fCallback);


/*!

    \brief     Unregister a callback for specific HTTP Request.

    \param[in] reqType - type of request (SL_NETAPP_REQUEST_HTTP_POST, SL_NETAPP_REQUEST_HTTP_POST)
    \param[in] pURI - URI string, starting with '/' (e.g. "/abc")

    \return    0 upon success or a negative error code

*/
int HTTPSRV_IF_unregisterCallback(int reqType, char *pURI);
/*!

    \brief     Provided a request structure, look for the given key (string) in the
                request's query string (e.g. "?keyA=valA&keyB=valB") and return its value

    \param[in] hRequest - request handle (provided to the callback)
    \param[in] pKey - searched key (string)
    \param[out] pValue - found value

    \return    value len upon success, 0 if not found or a negative error code
*/
int HTTPSRV_IF_searchQueryString(httpHandle_t hRequest, char *pKey, char *pValue);

/*!

    \brief     Add pair of key+value to response's query string

    \param[in] hRequest - request handle (provided to the callback)
    \param[in] pKey - searched key (string)
    \param[out] pValue - found value

    \return    0 upon success or a negative error code
*/
int HTTPSRV_IF_setResponseQueryString(httpHandle_t hRequest, char *pKey, char *pValue);

/*!

    \brief     Receive next chunk of a POST payload

    \param[in] hRequest - request handle (provided to the callback)
    \param[in] pData - pointer for the incoming data buffer
    \param[in/out] pDataLen - [in] max size of the incoming data buffer
                              [out] actual size copied to the buffer

    \return   0 on success, or negative error code on failure
*/
int HTTPSRV_IF_receive(httpHandle_t hRequest, uint8_t *pData, uint16_t *pDataLen);

/*!

    \brief     Send (deferred) response for the request

    \param[in] hRequest - request handle (provided to the callback)
    \param[in] status - HTTP status cofde
    \param[in] pPayload - [in] pointer to response payload
    \param[in] payloadLen - [in] length of response payload

    \return   0 on success, or negative error code on failure
*/
int HTTPSRV_IF_respond(httpHandle_t hRequest, int status, uint8_t *pPayload, uint32_t payloadLen);

/*!

    \brief     retrieve the content length of the request

    \param[in] hRequest - request handle (provided to the callback)

    \return   content length on success, or negative error code on failure
*/
int HTTPSRV_IF_getContentLength(httpHandle_t hRequest);




#endif // OTA_IF_H
